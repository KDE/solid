/*
    SPDX-FileCopyrightText: 2006-2007 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_STORAGEDRIVE_H
#define SOLID_STORAGEDRIVE_H

#include <solid/solid_export.h>

#include <solid/deviceinterface.h>

namespace Solid
{
class StorageDrivePrivate;
class Device;

/*!
 * \class Solid::StorageDrive
 * \inheaderfile Solid/StorageDrive
 * \inmodule Solid
 *
 * \brief This device interface is available on storage devices.
 *
 * A storage is anything that can contain a set of volumes (card reader,
 * hard disk, cdrom drive...). It's a particular kind of block device.
 *
 * A StorageDrive is commonly used in conjunction with StorageVolume and StorageAccess.
 *
 * \table
 * \header
 *   \li StorageDrive
 *   \li StorageVolume
 *   \li StorageAccess
 * \row
 *   \li drives
 *   \li partition tables / filesystems
 *   \li mount points / partitions
 * \endtable
 *
 * It can be used to fetch a parent device of a StorageVolume that is a StorageDrive
 * in order to check its properties, for example:
 *
 * \code
 * const Solid::Device& dev = m_device;
 * if (dev.is<Solid::StorageVolume>() && dev.parent().is<Solid::StorageDrive>()) {
 *     auto parent = dev.parent().as<Solid::StorageDrive>();
 *     if (parent->isRemovable() || parent->isHotpluggable()) {
 *         usable = false;
 *     }
 *
 *     const Solid::StorageVolume* volume = dev.as<Solid::StorageVolume>();
 *     if (volume->isIgnored() || volume->usage() != Solid::StorageVolume::FileSystem) {
 *         usable = false;
 *     }
 * }
 * \endcode
 */
class SOLID_EXPORT StorageDrive : public DeviceInterface
{
    Q_OBJECT

    /*!
     * \property Solid::StorageDrive::bus
     */
    Q_PROPERTY(Bus bus READ bus)

    /*!
     * \property Solid::StorageDrive::driveType
     */
    Q_PROPERTY(DriveType driveType READ driveType)

    /*!
     * \property Solid::StorageDrive::removable
     */
    Q_PROPERTY(bool removable READ isRemovable)

    /*!
     * \property Solid::StorageDrive::hotpluggable
     */
    Q_PROPERTY(bool hotpluggable READ isHotpluggable)

    /*!
     * \property Solid::StorageDrive::inUse
     */
    Q_PROPERTY(bool inUse READ isInUse)

    /*!
     * \property Solid::StorageDrive::size
     */
    Q_PROPERTY(qulonglong size READ size)

    /*!
     * \property Solid::StorageDrive::timeDetected
     */
    Q_PROPERTY(QDateTime timeDetected READ timeDetected CONSTANT)

    /*!
     * \property Solid::StorageDrive::timeMediaDetected
     */
    Q_PROPERTY(QDateTime timeMediaDetected READ timeDetected)

    Q_DECLARE_PRIVATE(StorageDrive)
    friend class Device;

public:
    /*!
     * This enum type defines the type of bus a storage device is attached to.
     *
     * \value Ide An Integrated Drive Electronics (IDE) bus, also known as ATA
     * \value Usb An Universal Serial Bus (USB)
     * \value Ieee1394 An Ieee1394 bus, also known as Firewire
     * \value Scsi A Small Computer System Interface bus
     * \value Sata A Serial Advanced Technology Attachment (SATA) bus
     * \value Platform A legacy bus that is part of the underlying platform
     */
    enum Bus { Ide, Usb, Ieee1394, Scsi, Sata, Platform };
    Q_ENUM(Bus)

    /*!
     * This enum type defines the type of drive a storage device can be.
     *
     * \value HardDisk A hard disk
     * \value CdromDrive An optical drive
     * \value Floppy A floppy disk drive
     * \value Tape A tape drive
     * \value CompactFlash A Compact Flash card reader
     * \value MemoryStick A Memory Stick card reader
     * \value SmartMedia A Smart Media card reader
     * \value SdMmc A SecureDigital/MultiMediaCard card reader
     * \value Xd A xD card reader
     */
    enum DriveType { HardDisk, CdromDrive, Floppy, Tape, CompactFlash, MemoryStick, SmartMedia, SdMmc, Xd };
    Q_ENUM(DriveType)

private:
    /*!
     * \internal
     * Creates a new StorageDrive object.
     * You generally won't need this. It's created when necessary using
     * Device::as().
     *
     * \a backendObject the device interface object provided by the backend
     * \sa Solid::Device::as()
     */
    SOLID_NO_EXPORT explicit StorageDrive(QObject *backendObject);

public:
    ~StorageDrive() override;

    /*!
     * Returns the Solid::DeviceInterface::Type of the StorageDrive device interface.
     * \sa Solid::DeviceInterface::Type
     */
    static Type deviceInterfaceType()
    {
        return DeviceInterface::StorageDrive;
    }

    /*!
     * Returns the bus type of the physical interface this storage device is
     * connected to.
     * \sa Solid::StorageDrive::Bus
     */
    Bus bus() const;

    /*!
     * Returns the type of this storage drive.
     * \sa Solid::StorageDrive::DriveType
     */
    DriveType driveType() const;

    /*!
     * Returns whether the media contained by this drive can be removed.
     *
     * For example, a memory card can be removed from the drive by the user,
     * while partitions can't be removed from hard disks.
     */
    bool isRemovable() const;

    /*!
     * Returns whether this storage device can be plugged or unplugged while
     * the computer is running (that is, it supports hotplugging).
     */
    bool isHotpluggable() const;

    /*!
     * Returns this drive's size in bytes.
     */
    qulonglong size() const;

    /*!
     * Returns whether the storage device is currently in use,
     * that is, if at least one child storage access is
     * mounted.
     */
    bool isInUse() const;

    /*!
     * Returns the time when the drive was detected.
     *
     * Typically this means the time a drive was plugged in, or the computer rebooted.
     *
     * An invalid datetime may be returned if the underlying information is not available.
     * \since 6.0
     */
    QDateTime timeDetected() const;

    /*!
     * Returns the time when media in the drive was detected.
     *
     * Typically this means the time a card was inserted into a reader, or the computer rebooted.
     *
     * An invalid datetime may be returned if the underlying information is not available.
     * \since 6.0
     */
    QDateTime timeMediaDetected() const;

protected:
    SOLID_NO_EXPORT StorageDrive(StorageDrivePrivate &dd, QObject *backendObject);
};
}

#endif // SOLID_STORAGEDRIVE_H
