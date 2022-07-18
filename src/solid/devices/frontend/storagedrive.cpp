/*
    SPDX-FileCopyrightText: 2006-2007 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "storagedrive.h"
#include "storagedrive_p.h"

#include "device.h"
#include "device_p.h"
#include "predicate.h"
#include "soliddefs_p.h"
#include "storageaccess.h"
#include <solid/devices/ifaces/storagedrive.h>

Solid::StorageDrive::StorageDrive(QObject *backendObject)
    : DeviceInterface(*new StorageDrivePrivate(), backendObject)
{
}

Solid::StorageDrive::StorageDrive(StorageDrivePrivate &dd, QObject *backendObject)
    : DeviceInterface(dd, backendObject)
{
}

Solid::StorageDrive::~StorageDrive()
{
}

Solid::StorageDrive::Bus Solid::StorageDrive::bus() const
{
    Q_D(const StorageDrive);
    auto *p = iface_cast<Ifaces::StorageDrive>(d->backendObject());
    return p ? p->bus() : Platform;
}

Solid::StorageDrive::DriveType Solid::StorageDrive::driveType() const
{
    Q_D(const StorageDrive);
    auto *p = iface_cast<Ifaces::StorageDrive>(d->backendObject());
    return p ? p->driveType() : HardDisk;
}

bool Solid::StorageDrive::isRemovable() const
{
    Q_D(const StorageDrive);
    auto *p = iface_cast<Ifaces::StorageDrive>(d->backendObject());
    return p ? p->isRemovable() : false;
}

bool Solid::StorageDrive::isHotpluggable() const
{
    Q_D(const StorageDrive);
    auto *p = iface_cast<Ifaces::StorageDrive>(d->backendObject());
    return p ? p->isHotpluggable() : false;
}

qulonglong Solid::StorageDrive::size() const
{
    Q_D(const StorageDrive);
    auto *p = iface_cast<Ifaces::StorageDrive>(d->backendObject());
    return p ? p->size() : 0;
}

bool Solid::StorageDrive::isInUse() const
{
    Q_D(const StorageDrive);
    Predicate p(DeviceInterface::StorageAccess);
    const QList<Device> devices = Device::listFromQuery(p, d->devicePrivate()->udi());

    bool inUse = false;
    for (const Device &dev : devices) {
        if (dev.is<Solid::StorageAccess>()) {
            const Solid::StorageAccess *access = dev.as<Solid::StorageAccess>();
            inUse |= (access->isAccessible());
        }
    }
    return inUse;
}
