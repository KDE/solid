/*
    Copyright 2017 René J.V. Bertin <rjvbertin@gmail.com>

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

#include "iokitstorage.h"

#include <QDebug>

#include <CoreFoundation/CoreFoundation.h>
#include <DiskArbitration/DiskArbitration.h>

using namespace Solid::Backends::IOKit;

IOKitStorage::IOKitStorage(IOKitDevice *device)
    : Block(device)
    , daDict(new DADictionary(device))
{
}

IOKitStorage::IOKitStorage(const IOKitDevice *device)
    : Block(device)
    , daDict(new DADictionary(device))
{
}

IOKitStorage::~IOKitStorage()
{
    delete daDict;
}

Solid::StorageDrive::Bus IOKitStorage::bus() const
{
    const QString udi = m_device->udi();
    // TODO: figure out how to return something useful here.
    if (udi.contains(QStringLiteral("/SATA@"))) {
        return Solid::StorageDrive::Sata;
    }
    if (udi.contains(QStringLiteral("/SDXC@"))) {
        // TODO: return something finer grained; the built-in card reader
        // is NOT connected via USB on Macs, for instance (but there's no PCI option)
        return Solid::StorageDrive::Usb;
    }
    if (udi.contains(QStringLiteral("/IOUSBInterface@"))) {
        return Solid::StorageDrive::Usb;
    }
    if (daDict->stringForKey(kDADiskDescriptionDeviceProtocolKey) == QStringLiteral("USB")) {
        return Solid::StorageDrive::Usb;
    }
    return Solid::StorageDrive::Platform;
}

Solid::StorageDrive::DriveType IOKitStorage::driveType() const
{
    const QString udi = m_device->udi();
    const QString type = m_device->property(QLatin1String("className")).toString();

    if (type == QStringLiteral("IOCDMedia")
        || type == QStringLiteral("IOBDMedia")
        || type == QStringLiteral("IODVDMedia")) {
        return Solid::StorageDrive::CdromDrive;
    }
    if (udi.contains(QStringLiteral("/SDXC@"))) {
        return Solid::StorageDrive::SdMmc;
    }
    if (daDict->stringForKey(kDADiskDescriptionDeviceModelKey) == QStringLiteral("Compact Flash")) {
        return Solid::StorageDrive::CompactFlash;
    }
    return Solid::StorageDrive::HardDisk;
}

bool IOKitStorage::isRemovable() const
{
    bool isExternal = false;
    daDict->boolForKey(kDADiskDescriptionDeviceInternalKey, isExternal);
    return isExternal || m_device->property(QLatin1String("Removable")).toBool();
}

bool IOKitStorage::isHotpluggable() const
{
    const Solid::StorageDrive::DriveType type = driveType();
    return bus() == Solid::StorageDrive::Usb
        || type == Solid::StorageDrive::CdromDrive || type == Solid::StorageDrive::SdMmc;
}

qulonglong IOKitStorage::size() const
{
    return m_device->property(QLatin1String("Size")).toULongLong();
}

QString IOKitStorage::vendor() const
{
    return daDict->stringForKey(kDADiskDescriptionDeviceVendorKey);
}

QString IOKitStorage::product() const
{
    return daDict->stringForKey(kDADiskDescriptionDeviceModelKey);
}

QString IOKitStorage::description() const
{
    return daDict->stringForKey(kDADiskDescriptionMediaNameKey);
}
