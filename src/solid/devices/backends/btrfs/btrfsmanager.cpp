/*
    SPDX-FileCopyrightText: 2026 Bharadwaj Raju <bharadwaj.raju@machinesoul.in>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/
#include "btrfsmanager.h"
#include "btrfsdevice.h"
#include "btrfshandling.h"

#include <QString>

using namespace Qt::StringLiterals;
using namespace Solid::Backends::Btrfs;

BtrfsManager::BtrfsManager(QObject *parent)
    : Solid::Ifaces::DeviceManager(parent)
{
    m_supportedInterfaces << Solid::DeviceInterface::StorageSubvolumeTree;
    m_supportedInterfaces << Solid::DeviceInterface::GenericInterface;
    m_deviceList = BtrfsHandling::deviceList();
}

BtrfsManager::~BtrfsManager()
{
}

QString BtrfsManager::udiPrefix() const
{
    return QStringLiteral(BTRFS_UDI_PREFIX);
}

QSet<Solid::DeviceInterface::Type> BtrfsManager::supportedInterfaces() const
{
    return m_supportedInterfaces;
}

QStringList BtrfsManager::allDevices()
{
    return BtrfsHandling::deviceList();
}

QStringList BtrfsManager::devicesFromQuery(const QString &parentUdi, Solid::DeviceInterface::Type type)
{
    if (parentUdi.isEmpty() && type == Solid::DeviceInterface::Unknown) {
        return allDevices();
    }

    QStringList relevantDevices;
    for (const QString &udi : allDevices()) {
        BtrfsDevice dev(udi);
        if ((parentUdi.isEmpty() || dev.parentUdi() == parentUdi) && (type == Solid::DeviceInterface::Unknown || dev.queryDeviceInterface(type))) {
            relevantDevices << udi;
        }
    }
    return relevantDevices;
}

std::unique_ptr<QObject> BtrfsManager::createDevice(const QString &udi)
{
    if (!BtrfsHandling::deviceList().contains(udi)) {
        return nullptr;
    }
    return std::make_unique<BtrfsDevice>(udi);
}

void BtrfsManager::onMountsChanged()
{
    BtrfsHandling::updateSubvolumes();
    _k_updateDeviceList();
}

void BtrfsManager::_k_updateDeviceList()
{
    const QStringList newDeviceList = BtrfsHandling::deviceList();

    QSet<QString> newSet(newDeviceList.begin(), newDeviceList.end());
    QSet<QString> oldSet(m_deviceList.begin(), m_deviceList.end());

    m_deviceList = newDeviceList;

    for (const QString &device : newSet) {
        if (!oldSet.contains(device)) {
            Q_EMIT deviceAdded(device);
        }
    }

    for (const QString &device : oldSet) {
        if (!newSet.contains(device)) {
            Q_EMIT deviceRemoved(device);
        }
    }
}

#include "moc_btrfsmanager.cpp"
