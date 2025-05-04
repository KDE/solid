/*
    SPDX-FileCopyrightText: 2006-2007 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_STORAGEVOLUME_H
#define SOLID_STORAGEVOLUME_H

#include <solid/solid_export.h>

#include <solid/deviceinterface.h>

namespace Solid
{
class StorageVolumePrivate;
class Device;

/*!
 * \class Solid::StorageVolume
 * \inheaderfile Solid/StorageVolume
 * \inmodule Solid
 *
 * \brief This device interface is available on volume devices.
 *
 * A volume is anything that can contain data (partition, optical disc,
 * memory card). It's a particular kind of block device.
 */
class SOLID_EXPORT StorageVolume : public DeviceInterface
{
    Q_OBJECT

    /*!
     * \property Solid::StorageVolume::ignored
     */
    Q_PROPERTY(bool ignored READ isIgnored)

    /*!
     * \property Solid::StorageVolume::usage
     */
    Q_PROPERTY(UsageType usage READ usage)

    /*!
     * \property Solid::StorageVolume::fsType
     */
    Q_PROPERTY(QString fsType READ fsType)

    /*!
     * \property Solid::StorageVolume::label
     */
    Q_PROPERTY(QString label READ label)

    /*!
     * \property Solid::StorageVolume::uuid
     */
    Q_PROPERTY(QString uuid READ uuid)

    /*!
     * \property Solid::StorageVolume::size
     */
    Q_PROPERTY(qulonglong size READ size)

    Q_DECLARE_PRIVATE(StorageVolume)
    friend class Device;

public:
    /*!
     * This enum type defines the how a volume is used.
     *
     * \value FileSystem A mountable filesystem volume
     * \value PartitionTable A volume containing a partition table
     * \value Raid A volume member of a raid set (not mountable)
     * \value Other A not mountable volume (like a swap partition)
     * \value Unused An unused or free volume
     * \value Encrypted
     */
    enum UsageType { Other = 0, Unused = 1, FileSystem = 2, PartitionTable = 3, Raid = 4, Encrypted = 5 };
    Q_ENUM(UsageType)

private:
    /*!
     * \internal
     * Creates a new StorageVolume object.
     * You generally won't need this. It's created when necessary using
     * Device::as().
     *
     * \a backendObject the device interface object provided by the backend
     * \sa Solid::Device::as()
     */
    SOLID_NO_EXPORT explicit StorageVolume(QObject *backendObject);

public:
    ~StorageVolume() override;

    /*!
     * Get the Solid::DeviceInterface::Type of the StorageVolume device interface.
     *
     * Returns the StorageVolume device interface type
     * \sa Solid::DeviceInterface::Type
     */
    static Type deviceInterfaceType()
    {
        return DeviceInterface::StorageVolume;
    }

    /*!
     * Indicates if this volume should be ignored by applications.
     *
     * If it should be ignored, it generally means that it should be
     * invisible to the user. It's useful for firmware partitions or
     * OS reinstall partitions on some systems.
     *
     * Returns true if the volume should be ignored
     */
    bool isIgnored() const;

    /*!
     * Retrieves the type of use for this volume (for example filesystem).
     *
     * Returns the usage type
     * \sa Solid::StorageVolume::UsageType
     */
    UsageType usage() const;

    /*!
     * Retrieves the filesystem type of this volume.
     *
     * FIXME: It's a platform dependent string, maybe we should switch to
     * an enum?
     *
     * Returns the filesystem type if applicable, QString() otherwise
     */
    QString fsType() const;

    /*!
     * Retrieves this volume label.
     *
     * Returns the volume label if available, QString() otherwise
     */
    QString label() const;

    /*!
     * Retrieves this volume Universal Unique IDentifier (UUID).
     *
     * You can generally assume that this identifier is unique with reasonable
     * confidence. Except if the volume UUID has been forged to intentionally
     * provoke a collision, the probability to have two volumes having the same
     * UUID is low.
     *
     * Returns the Universal Unique IDentifier if available, QString() otherwise
     */
    QString uuid() const;

    /*!
     * Retrieves this volume size in bytes.
     *
     * Returns the size of this volume
     */
    qulonglong size() const;

    /*!
     * Retrieves the crypto container of this volume.
     *
     * Returns the encrypted volume containing the current volume if applicable,
     * an invalid device otherwise
     */
    Device encryptedContainer() const;

protected:
    SOLID_NO_EXPORT StorageVolume(StorageVolumePrivate &dd, QObject *backendObject);
};
}

#endif // SOLID_STORAGEVOLUME_H
