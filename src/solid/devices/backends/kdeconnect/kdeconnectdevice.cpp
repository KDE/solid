/*
    Copyright 2013 Albert Vaca <albertvaka@gmail.com>
    Copyright 2015 Kai Uwe Broulik <kde@privat.broulik.de>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#include "kdeconnectdevice.h"
#include "kdeconnectdeviceinterface.h"
#include "kdeconnectbattery.h"
#include "kdeconnectportablemediaplayer.h"

#include <solid/genericinterface.h>
#include <solid/device.h>

#include <QtCore/QStringList>
#include <QtDBus/QDBusPendingReply>

using namespace Solid::Backends::KdeConnect;

KdeConnectDevice::KdeConnectDevice(const QString &udi, const QString &id)
    : Solid::Ifaces::Device()
    , m_device("org.kde.kdeconnect",
               "/modules/kdeconnect/devices/" + id,
               "org.kde.kdeconnect.device",
               QDBusConnection::sessionBus())
    , m_udi(udi)
    , m_id(id)
{

}

KdeConnectDevice::~KdeConnectDevice()
{

}

QObject *KdeConnectDevice::createDeviceInterface(const Solid::DeviceInterface::Type &type)
{
    if (!queryDeviceInterface(type)) {
        return Q_NULLPTR;
    }

    DeviceInterface *iface = Q_NULLPTR;

    switch (type) {
    case Solid::DeviceInterface::Battery:
        iface = new Battery(this);
        break;
    case Solid::DeviceInterface::PortableMediaPlayer:
        iface = new PortableMediaPlayer(this);
        break;
    default:
        break;
    }

    return iface;
}

bool KdeConnectDevice::queryDeviceInterface(const Solid::DeviceInterface::Type &type) const
{
    return (type == Solid::DeviceInterface::Battery || type == Solid::DeviceInterface::PortableMediaPlayer);
}

QStringList KdeConnectDevice::emblems() const
{
    return QStringList();
}

QString KdeConnectDevice::description() const
{
    return m_device.property("name").toString(); // FIXME
}

QString KdeConnectDevice::icon() const
{
    return m_device.property("iconName").toString();
}

QString KdeConnectDevice::product() const
{
    return m_device.property("name").toString();
}

QString KdeConnectDevice::vendor() const
{
    return QString(); // we just want the configured device name to appear
}

QString KdeConnectDevice::udi() const
{
    return m_udi;
}

QString KdeConnectDevice::parentUdi() const
{
    return QStringLiteral("/org/kde/kdeconnect");
}

QString KdeConnectDevice::deviceId() const
{
    return m_id;
}
