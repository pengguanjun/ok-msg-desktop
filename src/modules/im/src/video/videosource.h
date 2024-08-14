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

#ifndef VIDEOSOURCE_H
#define VIDEOSOURCE_H

#include <QObject>

#include <atomic>
#include <memory>

class VideoFrame;

class VideoSource : public QObject {
    Q_OBJECT

public:
    // Declare type aliases
    using IDType = std::uint_fast64_t;
    using AtomicIDType = std::atomic_uint_fast64_t;

public:
    VideoSource() : id(sourceIDs++) {}

    virtual ~VideoSource() = default;
    /**
     * @brief If subscribe sucessfully opens the source, it will start emitting frameAvailable
     * signals.
     */
    virtual void subscribe() = 0;
    /**
     * @brief Stop emitting frameAvailable signals, and free associated resources if necessary.
     */
    virtual void unsubscribe() = 0;

    /// ID of this VideoSource
    const IDType id;
signals:
    /**
     * @brief Emitted when new frame available to use.
     * @param frame New frame.
     */
    void frameAvailable(std::shared_ptr<VideoFrame> frame);
    /**
     * @brief Emitted when the source is stopped for an indefinite amount of time, but might restart
     * sending frames again later
     */
    void sourceStopped();

private:
    // Used to manage a global ID for all VideoSources
    static AtomicIDType sourceIDs;
};

#endif  // VIDEOSOURCE_H
