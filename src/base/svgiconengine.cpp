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

#include "svgiconengine.h"

#include <QApplication>
#include <QPainter>
#include <QPalette>
#include <QPixmapCache>
namespace ok::base {
QSize SvgIconEngine::actualSize(const QSize& size, QIcon::Mode mode, QIcon::State state) {
    Q_UNUSED(mode);
    Q_UNUSED(state);
    return size.isEmpty() ? renderer->defaultSize()
                          : renderer->defaultSize().scaled(size, Qt::KeepAspectRatio);
}

QIconEngine* SvgIconEngine::clone() const { return new SvgIconEngine(name, renderer); }

void SvgIconEngine::paint(QPainter* painter, const QRect& rect, QIcon::Mode mode,
                          QIcon::State state) {
    Q_UNUSED(mode);
    Q_UNUSED(state);
    auto r = rect.isEmpty() ? QRect(0, 0, painter->device()->width(), painter->device()->height())
                            : rect;
    renderer->render(painter, r);
}

void SvgIconEngine::virtual_hook(int id, void* data) {
    switch (id) {
        case QIconEngine::AvailableSizesHook:
            reinterpret_cast<AvailableSizesArgument*>(data)->sizes.clear();
            break;
        case QIconEngine::IconNameHook:
            *reinterpret_cast<QString*>(data) = name;
            break;
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
        case QIconEngine::IsNullHook:
            *reinterpret_cast<bool*>(data) = !(renderer && renderer->isValid());
            break;
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
        case QIconEngine::ScaledPixmapHook: {
            auto arg = reinterpret_cast<ScaledPixmapArgument*>(data);
            arg->pixmap = pixmap(arg->size * arg->scale, arg->mode, arg->state);
            break;
        }
#endif
    }
}

QPixmap SvgIconEngine::pixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) {
    QPixmap pm;
    static auto findCache = [](std::list<CacheEntry>& cache, const QSize& size,
                               QPixmap& pm) mutable {
        auto it = cache.begin();
        while (it != cache.end()) {
            if (it->size == size) {
                if (QPixmapCache::find(it->key, &pm)) {
                    return true;
                }
                cache.erase(it);
                return false;
            }
            ++it;
        }
        return false;
    };

    static auto insertCache = [](std::list<CacheEntry>& cache, const QSize& size,
                                 const QPixmap& pm) mutable {
        cache.push_back(CacheEntry{QPixmapCache::insert(pm), size});
        if (cache.size() > 5) {
            QPixmapCache::remove(cache.front().key);
        }
    };

    if (mode == QIcon::Disabled && findCache(disabledCache, size, pm)) return pm;

    if (findCache(normalCache, size, pm)) {
        if (mode == QIcon::Active || mode == QIcon::Normal) return pm;
    }

    if (!pm) {
        pm = renderPixmap(size, mode, state);
        insertCache(normalCache, size, pm);
    }  // else we need selected or disabled from normal which we took from the cache

    if (mode == QIcon::Selected) {
        auto hlColor = qApp->palette().color(QPalette::Normal, QPalette::Highlight);
        hlColor.setAlpha(128);
        QPainter p(&pm);
        p.setPen(Qt::NoPen);
        p.fillRect(pm.rect(), hlColor);
    } else if (mode == QIcon::Disabled) {
        auto img = pm.toImage();
        for (int x = 0; x < img.width(); x++) {
            for (int y = 0; y < img.height(); y++) {
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
                QColor c = QColor(img.pixel(x, y));
#else
                QColor c = img.pixelColor(x, y);
#endif
                auto t = c.alpha();
                auto h = c.hue();
                auto v = c.value();
                c.setHsv(h, 0, v, t);
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
                img.setPixel(x, y, c.rgba());
#else
                img.setPixelColor(x, y, c);
#endif
            }
        }
        pm = QPixmap::fromImage(img);
        insertCache(disabledCache, size, pm);
    }

    return pm;
}

QPixmap SvgIconEngine::renderPixmap(const QSize& size, QIcon::Mode mode, QIcon::State state) {
    Q_UNUSED(mode)
    Q_UNUSED(state)
    auto sz = size.isEmpty() ? renderer->defaultSize()
                             : renderer->defaultSize().scaled(size, Qt::KeepAspectRatio);
    QPixmap pix(sz);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    renderer->render(&p);
    return pix;
}
}
