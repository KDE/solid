/*
 * SPDX-FileCopyrightText: 2026 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "kdeconnectportablemediaplayer.h"

using namespace Solid::Backends::KdeConnect;

PortableMediaPlayer::PortableMediaPlayer(Device *device)
    : DeviceInterface(device)
{
}

PortableMediaPlayer::~PortableMediaPlayer() = default;

QStringList PortableMediaPlayer::supportedProtocols() const
{
    return {QStringLiteral("kdeconnect")};
}

QStringList PortableMediaPlayer::supportedDrivers(QString protocol) const
{
    if (protocol == QLatin1String("kdeconnect")) {
        return {QStringLiteral("sftp")};
    }
    return {};
}

QVariant PortableMediaPlayer::driverHandle(const QString &driver) const
{
    if (driver == QLatin1String("sftp")) {
        return m_device->deviceId();
    }
    return {};
}

#include "moc_kdeconnectportablemediaplayer.cpp"
