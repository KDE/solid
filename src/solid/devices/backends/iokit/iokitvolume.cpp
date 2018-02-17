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

#include "iokitvolume.h"
#include "iokitgenericinterface.h"

#include <QDebug>

#include <CoreFoundation/CoreFoundation.h>
#include <DiskArbitration/DiskArbitration.h>

using namespace Solid::Backends::IOKit;

IOKitVolume::IOKitVolume(IOKitDevice *device)
    : Block(device)
    , daDict(new DADictionary(device))
{
}

IOKitVolume::IOKitVolume(const IOKitDevice *device)
    : Block(device)
    , daDict(new DADictionary(device))
{
}

IOKitVolume::~IOKitVolume()
{
    delete daDict;
}

bool IOKitVolume::isIgnored() const
{
    // ignore storage volumes that aren't mounted
    bool isIgnored = m_device->property(QStringLiteral("Open")).toBool() == false;
    m_device->setProperty("isIgnored", isIgnored);
    m_device->setProperty("isMounted", !isIgnored);
    return isIgnored;
}

Solid::StorageVolume::UsageType IOKitVolume::usage() const
{
    const QString content = m_device->property(QStringLiteral("Content")).toString();
    if (content == QStringLiteral("CD_DA")) {
        // this is (probably) a CD track
        return Solid::StorageVolume::Other;
    }
    if (content.contains(QStringLiteral("partition_scheme"))) {
        return Solid::StorageVolume::PartitionTable;
    }
    return Solid::StorageVolume::FileSystem;
}

QString IOKitVolume::fsType() const
{
    return daDict->stringForKey(kDADiskDescriptionVolumeKindKey);
}

QString IOKitVolume::label() const
{
    return daDict->stringForKey(kDADiskDescriptionVolumeNameKey);
}

QString IOKitVolume::uuid() const
{
    return m_device->property(QStringLiteral("UUID")).toString();
}

qulonglong IOKitVolume::size() const
{
    return m_device->property(QStringLiteral("Size")).toULongLong();
}

QString IOKitVolume::encryptedContainerUdi() const
{
    return QString();
}

QString IOKitVolume::vendor() const
{
    return daDict->stringForKey(kDADiskDescriptionDeviceVendorKey);
}

QString IOKitVolume::product() const
{
    return daDict->stringForKey(kDADiskDescriptionDeviceModelKey);
}

QString IOKitVolume::description() const
{
    return daDict->stringForKey(kDADiskDescriptionMediaNameKey);
}

DADiskRef IOKitVolume::daRef() const
{
    return daDict->daRef;
}
