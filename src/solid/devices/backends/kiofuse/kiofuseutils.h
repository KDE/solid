/*
    SPDX-FileCopyrightText: 2026 Chinmoy Pradhan <chinmoy.pradhan@machinesoul.in>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_BACKENDS_KIOFUSE_UTILS_H
#define SOLID_BACKENDS_KIOFUSE_UTILS_H

#include <QString>
#include <QUrl>

namespace Solid
{
namespace Backends
{
namespace KioFuse
{
namespace Utils
{

QString udiPrefix();

QString dbusService();

QString dbusPath();

QString dbusInterface();

/*!
 * The UDI must convert back to the URL it came from, because kio-fuse only takes
 * URLs. Percent encoding also hides the slashes, so a UDI stays one path part.
 */
QString udiForUrl(const QUrl &url);

QUrl urlForUdi(const QString &udi);

}
}
}
}

#endif // SOLID_BACKENDS_KIOFUSE_UTILS_H
