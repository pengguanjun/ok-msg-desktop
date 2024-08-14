/*
 * Copyright (c) 2022 船山信息 chuanshaninfo.com
 * The project is licensed under Mulan PubL v2.
 * You can use this software according to the terms and conditions of the Mulan
 * PubL v2. You may obtain a copy of Mulan PubL v2 at:
 *          http://license.coscl.org.cn/MulanPubL-2.0
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PubL v2 for more details.
 */

#include "v4l2.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <QDebug>
#include <map>

/**
 * Most of this file is adapted from libavdevice's v4l2.c,
 * which retrieves useful information but only exposes it to
 * stdout and is not part of the public API for some reason.
 */

static std::map<uint32_t, uint8_t> createPixFmtToQuality() {
    std::map<uint32_t, uint8_t> m;
    m[V4L2_PIX_FMT_H264] = 3;
    m[V4L2_PIX_FMT_MJPEG] = 2;
    m[V4L2_PIX_FMT_YUYV] = 1;
    m[V4L2_PIX_FMT_UYVY] = 1;
    return m;
}
const std::map<uint32_t, uint8_t> pixFmtToQuality = createPixFmtToQuality();

static std::map<uint32_t, QString> createPixFmtToName() {
    std::map<uint32_t, QString> m;
    m[V4L2_PIX_FMT_H264] = QString("h264");
    m[V4L2_PIX_FMT_MJPEG] = QString("mjpeg");
    m[V4L2_PIX_FMT_YUYV] = QString("yuyv422");
    m[V4L2_PIX_FMT_UYVY] = QString("uyvy422");
    return m;
}
const std::map<uint32_t, QString> pixFmtToName = createPixFmtToName();

static int deviceOpen(QString devName, int* error) {
    struct v4l2_capability cap;
    int fd;

    const std::string devNameString = devName.toStdString();
    fd = open(devNameString.c_str(), O_RDWR, 0);
    if (fd < 0) {
        *error = errno;
        return fd;
    }

    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) {
        *error = errno;
        goto fail;
    }

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
        *error = ENODEV;
        goto fail;
    }

    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
        *error = ENOSYS;
        goto fail;
    }

    return fd;

fail:
    close(fd);
    return -1;
}

static QVector<float> getDeviceModeFramerates(int fd, unsigned w, unsigned h,
                                              uint32_t pixelFormat) {
    QVector<float> rates;
    v4l2_frmivalenum vfve{};
    vfve.pixel_format = pixelFormat;
    vfve.height = h;
    vfve.width = w;

    while (!ioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &vfve)) {
        float rate;
        switch (vfve.type) {
            case V4L2_FRMSIZE_TYPE_DISCRETE:
                rate = vfve.discrete.denominator / vfve.discrete.numerator;
                if (!rates.contains(rate)) rates.append(rate);
                break;
            case V4L2_FRMSIZE_TYPE_CONTINUOUS:
            case V4L2_FRMSIZE_TYPE_STEPWISE:
                rate = vfve.stepwise.min.denominator / vfve.stepwise.min.numerator;
                if (!rates.contains(rate)) rates.append(rate);
        }
        vfve.index++;
    }

    return rates;
}

QVector<VideoMode> v4l2::getDeviceModes(QString devName) {
    QVector<VideoMode> modes;

    int error = 0;
    int fd = deviceOpen(devName, &error);
    if (fd < 0 || error != 0) {
        return modes;
    }

    v4l2_fmtdesc vfd{};
    vfd.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    while (!ioctl(fd, VIDIOC_ENUM_FMT, &vfd)) {
        vfd.index++;

        v4l2_frmsizeenum vfse{};
        vfse.pixel_format = vfd.pixelformat;

        while (!ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &vfse)) {
            VideoMode mode;
            mode.pixel_format = vfse.pixel_format;
            switch (vfse.type) {
                case V4L2_FRMSIZE_TYPE_DISCRETE:
                    mode.width = vfse.discrete.width;
                    mode.height = vfse.discrete.height;
                    break;
                case V4L2_FRMSIZE_TYPE_CONTINUOUS:
                case V4L2_FRMSIZE_TYPE_STEPWISE:
                    mode.width = vfse.stepwise.max_width;
                    mode.height = vfse.stepwise.max_height;
                    break;
                default:
                    continue;
            }

            QVector<float> rates =
                    getDeviceModeFramerates(fd, mode.width, mode.height, vfd.pixelformat);

            // insert dummy FPS value to have the mode in the list even if we don't know the FPS
            // this fixes support for some webcams, see #5082
            if (rates.isEmpty()) {
                rates.append(0.0f);
            }

            for (float rate : rates) {
                mode.FPS = rate;
                if (!modes.contains(mode)) {
                    modes.append(std::move(mode));
                }
            }
            vfse.index++;
        }
    }

    return modes;
}

QVector<QPair<QString, QString>> v4l2::getDeviceList() {
    QVector<QPair<QString, QString>> devices;
    QStringList deviceFiles;

    DIR* dir = opendir("/dev");
    if (!dir) return devices;

    dirent* e;
    while ((e = readdir(dir)))
        if (!strncmp(e->d_name, "video", 5) || !strncmp(e->d_name, "vbi", 3))
            deviceFiles += QString("/dev/") + e->d_name;
    closedir(dir);

    for (QString file : deviceFiles) {
        const std::string filePath = file.toStdString();
        int fd = open(filePath.c_str(), O_RDWR);
        if (fd < 0) {
            continue;
        }

        v4l2_capability caps;
        if (ioctl(fd, VIDIOC_QUERYCAP, &caps) == 0) {
            if (!(caps.device_caps & V4L2_CAP_VIDEO_CAPTURE)) {
                continue;
            }
            devices += {file, (const char*)caps.card};
        }
        close(fd);
    }

    for (auto item : devices) {
        qDebug() << "v4l2 device:" << item.first << "=>" << item.second;
    }
    return devices;
}

QString v4l2::getPixelFormatString(uint32_t pixel_format) {
    if (pixFmtToName.find(pixel_format) == pixFmtToName.end()) {
        qWarning() << "Pixel format not found";
        return QString("invalid");
    }
    return pixFmtToName.at(pixel_format);
}

bool v4l2::betterPixelFormat(uint32_t a, uint32_t b) {
    if (pixFmtToQuality.find(a) == pixFmtToQuality.end()) {
        return false;
    } else if (pixFmtToQuality.find(b) == pixFmtToQuality.end()) {
        return true;
    }
    return pixFmtToQuality.at(a) > pixFmtToQuality.at(b);
}
