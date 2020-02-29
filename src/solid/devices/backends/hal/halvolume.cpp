/*
    SPDX-FileCopyrightText: 2006 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "halvolume.h"
#include "halstorageaccess.h"
#include "halgenericinterface.h"

using namespace Solid::Backends::Hal;

Volume::Volume(HalDevice *device)
    : Block(device)
{
}

Volume::~Volume()
{

}

bool Volume::isIgnored() const
{
    static HalDevice lock("/org/freedesktop/Hal/devices/computer");
    bool isLocked = lock.prop("info.named_locks.Global.org.freedesktop.Hal.Device.Storage.locked").toBool();

    if (m_device->prop("volume.ignore").toBool() || isLocked) {
        return true;
    }

    const QString mount_point = StorageAccess(m_device).filePath();
    const bool mounted = m_device->prop("volume.is_mounted").toBool();
    if (!mounted) {
        return false;
    } else if (mount_point.startsWith(QLatin1String("/media/")) || mount_point.startsWith(QLatin1String("/mnt/"))) {
        return false;
    }

    /* Now be a bit more aggressive on what we want to ignore,
     * the user generally need to check only what's removable or in /media
     * the volumes mounted to make the system (/, /boot, /var, etc.)
     * are useless to him.
     */
    HalDevice drive(m_device->prop("block.storage_device").toString());
    Hal::GenericInterface gif(&drive);

    const bool removable = gif.property("storage.removable").toBool();
    const bool hotpluggable = gif.property("storage.hotpluggable").toBool();

    return !removable && !hotpluggable;
}

Solid::StorageVolume::UsageType Volume::usage() const
{
    QString usage = m_device->prop("volume.fsusage").toString();

    if (usage == "filesystem") {
        return Solid::StorageVolume::FileSystem;
    } else if (usage == "partitiontable") {
        return Solid::StorageVolume::PartitionTable;
    } else if (usage == "raid") {
        return Solid::StorageVolume::Raid;
    } else if (usage == "crypto") {
        return Solid::StorageVolume::Encrypted;
    } else if (usage == "unused") {
        return Solid::StorageVolume::Unused;
    } else {
        return Solid::StorageVolume::Other;
    }
}

QString Volume::fsType() const
{
    return m_device->prop("volume.fstype").toString();
}

QString Volume::label() const
{
    return m_device->prop("volume.label").toString();
}

QString Volume::uuid() const
{
    return m_device->prop("volume.uuid").toString();
}

qulonglong Volume::size() const
{
    return m_device->prop("volume.size").toULongLong();
}

QString Solid::Backends::Hal::Volume::encryptedContainerUdi() const
{
    return m_device->prop("volume.crypto_luks.clear.backing_volume").toString();
}

