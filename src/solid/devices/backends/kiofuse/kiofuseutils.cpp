/*
    SPDX-FileCopyrightText: 2026 Chinmoy Pradhan <chinmoy.pradhan@machinesoul.in>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "kiofuseutils.h"

namespace Solid
{
namespace Backends
{
namespace KioFuse
{
namespace Utils
{

QString udiPrefix()
{
    return QStringLiteral("/org/kde/solid/kiofuse");
}

QString dbusService()
{
    return QStringLiteral("org.kde.KIOFuse");
}

QString dbusPath()
{
    return QStringLiteral("/org/kde/KIOFuse");
}

QString dbusInterface()
{
    return QStringLiteral("org.kde.KIOFuse.VFS");
}

QString udiForUrl(const QUrl &url)
{
    return udiPrefix() + QLatin1Char('/') + QString::fromLatin1(QUrl::toPercentEncoding(url.toString()));
}

QUrl urlForUdi(const QString &udi)
{
    return QUrl(QUrl::fromPercentEncoding(udi.mid(udiPrefix().length() + 1).toLatin1()));
}

}
}
}
}
