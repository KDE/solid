/*
 * SPDX-FileCopyrightText: 2026 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef SOLID_BACKENDS_KDECONNECT_UTILS_H
#define SOLID_BACKENDS_KDECONNECT_UTILS_H

#include <QDBusMessage>

class QDBusAbstractInterface;

#include <QVariantMap>

namespace Solid
{
namespace Backends
{
namespace KdeConnect
{
namespace Utils
{

QString udiPrefix();

QString dbusService();

QString deviceDBusPath(const QString &deviceId);

QDBusMessage getAllPropertiesCall(QDBusAbstractInterface &iface);

QDBusMessage getPropertyCall(QDBusAbstractInterface &iface, const QString &name);

} // namespace Utils
}
}
}

#endif // SOLID_BACKENDS_KDECONNECT_UTILS_H
