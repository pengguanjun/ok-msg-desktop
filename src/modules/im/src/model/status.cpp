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

#include <src/model/status.h>

#include <QDebug>
#include <QObject>
#include <QPixmap>
#include <QString>

#include <cassert>

namespace Status {
QString getTitle(Status status) {
    switch (status) {
        case Status::Online:
            return QObject::tr("online", "contact status");
        case Status::Away:
            return QObject::tr("away", "contact status");
        case Status::Busy:
            return QObject::tr("busy", "contact status");
        case Status::Offline:
            return QObject::tr("offline", "contact status");
        case Status::Blocked:
            return QObject::tr("blocked", "contact status");
    }

    //        assert(false);
    return QStringLiteral("");
}

QString getAssetSuffix(Status status) {
    switch (status) {
        case Status::Online:
            return "online";
        case Status::Away:
            return "away";
        case Status::Busy:
            return "busy";
        case Status::Offline:
            return "offline";
        case Status::Blocked:
            return "blocked";
    }
    assert(false);
    return QStringLiteral("");
}

QString getIconPath(Status status, bool event) {
    const QString eventSuffix = event ? QStringLiteral("_notification") : QString();
    const QString statusSuffix = getAssetSuffix(status);
    if (status == Status::Blocked) {
        return ":/img/status/" + statusSuffix + ".svg";
    } else {
        return ":/img/status/" + statusSuffix + eventSuffix + ".svg";
    }
}

bool isOnline(Status status) { return status != Status::Offline && status != Status::Blocked; }
}  // namespace Status
