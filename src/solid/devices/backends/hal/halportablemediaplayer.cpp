/*
    SPDX-FileCopyrightText: 2006 Davide Bettio <davide.bettio@kdemail.net>
    SPDX-FileCopyrightText: 2007 Jeff Mitchell <kde-dev@emailgoeshere.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "halportablemediaplayer.h"

using namespace Solid::Backends::Hal;

PortableMediaPlayer::PortableMediaPlayer(HalDevice *device)
    : DeviceInterface(device)
{

}

PortableMediaPlayer::~PortableMediaPlayer()
{

}

QStringList PortableMediaPlayer::supportedProtocols() const
{
    return m_device->prop("portable_audio_player.access_method.protocols").toStringList();
}

QStringList PortableMediaPlayer::supportedDrivers(QString protocol) const
{
    QStringList drivers = m_device->prop("portable_audio_player.access_method.drivers").toStringList();
    if (protocol.isNull()) {
        return drivers;
    }
    QStringList returnedDrivers;
    QString temp;
    for (int i = 0; i < drivers.size(); i++) {
        temp = drivers.at(i);
        if (m_device->prop("portable_audio_player." + temp + ".protocol") == protocol) {
            returnedDrivers << temp;
        }
    }
    return returnedDrivers;
}

QVariant PortableMediaPlayer::driverHandle(const QString &driver) const
{
    if (driver == "mtp") {
        return m_device->prop("usb.serial");
    }
    // TODO: Fill in the blank for other drivers

    return QVariant();
}

