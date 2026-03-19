/*
 * SPDX-FileCopyrightText: 2026 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "kdeconnectutils.h"

#include <QDBusAbstractInterface>
#include <QDBusMessage>

namespace Solid
{
namespace Backends
{
namespace KdeConnect
{
namespace Utils
{

QString udiPrefix()
{
    return QStringLiteral("/org/kde/solid/kdeconnect");
}

QString dbusService()
{
    return QStringLiteral("org.kde.kdeconnect");
}

QString deviceDBusPath(const QString &deviceId)
{
    return QStringLiteral("/modules/kdeconnect/devices/%1").arg(deviceId);
}

QDBusMessage getAllPropertiesCall(QDBusAbstractInterface &iface)
{
    QDBusMessage message =
        QDBusMessage::createMethodCall(iface.service(), iface.path(), QStringLiteral("org.freedesktop.DBus.Properties"), QStringLiteral("GetAll"));
    message.setArguments({iface.interface()});
    return message;
}

QDBusMessage getPropertyCall(QDBusAbstractInterface &iface, const QString &name)
{
    QDBusMessage message =
        QDBusMessage::createMethodCall(iface.service(), iface.path(), QStringLiteral("org.freedesktop.DBus.Properties"), QStringLiteral("Get"));
    message.setArguments({iface.interface(), name});
    return message;
}
}
}
}
}
