/*
    SPDX-FileCopyrightText: 2026 Chinmoy Pradhan <chinmoy.pradhan@machinesoul.in>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "kiofusedevice.h"
#include "kiofusemanager.h"
#include "kiofusestorageaccess.h"
#include "kiofuseutils.h"

using namespace Solid::Backends::KioFuse;

Device::Device(Manager *manager, const QString &udi)
    : m_manager(manager)
    , m_udi(udi)
    , m_url(Utils::urlForUdi(udi))
{
}

Device::~Device() = default;

QString Device::udi() const
{
    return m_udi;
}

QString Device::parentUdi() const
{
    return Utils::udiPrefix();
}

QString Device::vendor() const
{
    return m_url.host();
}

QString Device::product() const
{
    const QString path = m_url.path();
    const QString lastPart = path.section(QLatin1Char('/'), -1, -1, QString::SectionSkipEmpty);

    return lastPart.isEmpty() ? m_url.host() : lastPart;
}

QString Device::icon() const
{
    return QStringLiteral("folder-remote");
}

QStringList Device::emblems() const
{
    return {QStringLiteral("emblem-mounted")};
}

QString Device::displayName() const
{
    const QString name = product();
    if (name == m_url.host()) {
        return name;
    }

    return tr("%1 on %2", "@item share on host").arg(name, m_url.host());
}

QString Device::description() const
{
    return displayName();
}

QUrl Device::url() const
{
    return m_url;
}

QString Device::filePath() const
{
    return m_manager->filePath(m_udi);
}

bool Device::queryDeviceInterface(const Solid::DeviceInterface::Type &type) const
{
    switch (type) {
    case Solid::DeviceInterface::StorageAccess:
        return true;
    default:
        return false;
    }
}

QObject *Device::createDeviceInterface(const Solid::DeviceInterface::Type &type)
{
    switch (type) {
    case Solid::DeviceInterface::StorageAccess:
        return new StorageAccess(this);
    default:
        return nullptr;
    }
}

#include "moc_kiofusedevice.cpp"
