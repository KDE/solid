/*
 * SPDX-FileCopyrightText: 2026 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "kdeconnect_debug.h"

#include "kdeconnectbattery.h"
#include "kdeconnectdevice.h"
#include "kdeconnectdeviceinterface.h"
#include "kdeconnectmanager.h"
#include "kdeconnectportablemediaplayer.h"
#include "kdeconnectutils.h"

#include <solid/device.h>

#include <QStringList>

using namespace Solid::Backends::KdeConnect;

Device::Device(Manager *manager, const QString &udi)
    : Solid::Ifaces::Device()
    , m_manager(manager)
    , m_udi(udi)
{
    // We don't monitor any device properties here since most of that is handled by the manager.
    // For example, (un)pairing a device will emit pairingRequestsChanged on the manager
    // where it will update the device. Furthermore, once a device becomes reachable, we will
    // update the list of loaded modules.
    // NameChanged is the only signal we could realistically listen for but Solid::Device has
    // no signal for that.

    const QStringList loadedPlugins = m_manager->deviceProperty(m_udi, QStringLiteral("loadedPlugins")).toStringList();

    if (loadedPlugins.contains(QLatin1String("kdeconnect_battery"))) {
        m_supportedDeviceTypes.insert(Solid::DeviceInterface::Type::Battery);
    }
    if (loadedPlugins.contains(QLatin1String("kdeconnect_sftp"))) {
        m_supportedDeviceTypes.insert(Solid::DeviceInterface::PortableMediaPlayer);
    }
}

Device::~Device() = default;

QObject *Device::createDeviceInterface(const Solid::DeviceInterface::Type &type)
{
    if (!queryDeviceInterface(type)) {
        return nullptr;
    }

    switch (type) {
    case Solid::DeviceInterface::Battery:
        return new Battery(this);
    case Solid::DeviceInterface::PortableMediaPlayer:
        return new PortableMediaPlayer(this);
    default:
        return nullptr;
    }
}

bool Device::queryDeviceInterface(const Solid::DeviceInterface::Type &type) const
{
    return m_supportedDeviceTypes.contains(type);
}

QStringList Device::emblems() const
{
    // statusIconName?
    return {};
}

QString Device::description() const
{
    return product();
}

QString Device::icon() const
{
    // iconName?
    return QStringLiteral("kdeconnect");
}

QString Device::product() const
{
    return m_manager->deviceProperty(m_udi, QStringLiteral("name")).toString();
}

QString Device::deviceType() const
{
    return m_manager->deviceProperty(m_udi, QStringLiteral("type")).toString();
}

QString Device::vendor() const
{
    return {};
}

QString Device::udi() const
{
    return m_udi;
}

QString Device::parentUdi() const
{
    return Utils::udiPrefix();
}

QString Device::deviceId() const
{
    return m_udi.section(QLatin1Char('/'), -1);
}

QString Device::dbusPath() const
{
    return Utils::deviceDBusPath(deviceId());
}

#include "moc_kdeconnectdevice.cpp"
