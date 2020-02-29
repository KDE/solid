/*
    SPDX-FileCopyrightText: 2006 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "halstorage.h"

using namespace Solid::Backends::Hal;

Storage::Storage(HalDevice *device)
    : Block(device)
{

}

Storage::~Storage()
{

}

Solid::StorageDrive::Bus Storage::bus() const
{
    QString bus = m_device->prop("storage.bus").toString();

    if (bus == "ide") {
        return Solid::StorageDrive::Ide;
    } else if (bus == "usb") {
        return Solid::StorageDrive::Usb;
    } else if (bus == "ieee1394") {
        return Solid::StorageDrive::Ieee1394;
    } else if (bus == "scsi") {
        return Solid::StorageDrive::Scsi;
    } else if (bus == "sata") {
        return Solid::StorageDrive::Sata;
    } else {
        return Solid::StorageDrive::Platform;
    }
}

Solid::StorageDrive::DriveType Storage::driveType() const
{
    QString type = m_device->prop("storage.drive_type").toString();

    if (type == "disk") {
        return Solid::StorageDrive::HardDisk;
    } else if (type == "cdrom") {
        return Solid::StorageDrive::CdromDrive;
    } else if (type == "floppy") {
        return Solid::StorageDrive::Floppy;
    } else if (type == "tape") {
        return Solid::StorageDrive::Tape;
    } else if (type == "compact_flash") {
        return Solid::StorageDrive::CompactFlash;
    } else if (type == "memory_stick") {
        return Solid::StorageDrive::MemoryStick;
    } else if (type == "smart_media") {
        return Solid::StorageDrive::SmartMedia;
    } else if (type == "sd_mmc") {
        return Solid::StorageDrive::SdMmc;
    } else {
        return Solid::StorageDrive::HardDisk;
    }
}

bool Storage::isRemovable() const
{
    return m_device->prop("storage.removable").toBool();
}

bool Storage::isHotpluggable() const
{
    return m_device->prop("storage.hotpluggable").toBool();
}

qulonglong Storage::size() const
{
    return m_device->prop("storage.size").toULongLong();
}

