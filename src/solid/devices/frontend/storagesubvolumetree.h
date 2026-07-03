/*
    SPDX-FileCopyrightText: 2026 Bharadwaj Raju <bharadwaj.raju@machinesoul.in>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_STORAGESUBVOLUME_H
#define SOLID_STORAGESUBVOLUME_H

#include <QDateTime>
#include <QUuid>

#include <solid/solid_export.h>

#include <solid/deviceinterface.h>

namespace Solid
{
class StorageSubvolumeTreePrivate;
class Device;

/*!
 * \class Solid::StorageSubvolumeTree
 * \inheaderfile Solid/StorageSubvolumeTree
 * \inmodule Solid
 *
 * \brief Represents a subvolume within a filesystem.
 *
 * A subvolume is an independent filesystem hierarchy and inode number-space within a larger filesystem, like Btrfs.
 */
class SOLID_EXPORT StorageSubvolumeTree : public DeviceInterface
{
    Q_OBJECT

    /*!
     * \property Solid::StorageSubvolumeTree::filePath
     */
    Q_PROPERTY(QString filePath READ filePath)

    /*!
     * \property Solid::StorageSubvolumeTree::id
     */
    Q_PROPERTY(qulonglong id READ id)

    /*!
     * \property Solid::StorageSubvolumeTree::uuid
     */
    Q_PROPERTY(QUuid uuid READ uuid)

    /*!
     * \property Solid::StorageSubvolumeTree::parentUuid
     */
    Q_PROPERTY(QUuid parentUuid READ parentUuid)

    /*!
     * \property Solid::StorageSubvolumeTree::creationTime
     */
    Q_PROPERTY(QDateTime creationTime READ creationTime)

    Q_DECLARE_PRIVATE(StorageSubvolumeTree)
    friend class Device;

public:
private:
    /*!
     * \internal
     * Creates a new StorageSubvolumeTree object.
     * You generally won't need this. It's created when necessary using
     * Device::as().
     *
     * \a backendObject the device interface object provided by the backend
     * \sa Solid::Device::as()
     */
    SOLID_NO_EXPORT explicit StorageSubvolumeTree(QObject *backendObject);

public:
    ~StorageSubvolumeTree() override;

    /*!
     * Returns the Solid::DeviceInterface::Type of the StorageSubvolumeTree device interface.
     * \sa Solid::DeviceInterface::Type
     */
    static Type deviceInterfaceType()
    {
        return DeviceInterface::StorageSubvolumeTree;
    }

    /*!
     * Retrieves the absolute path of this subvolume.
     *
     * @return the absolute path to the subvolume
     */
    QString filePath() const;

    /*!
     * Retrieves the internal ID of the subvolume.
     *
     * @return the ID
     */
    qulonglong id() const;

    /*!
     * Retrieves the UUID of the subvolume.
     *
     * @return the UUID
     */
    QUuid uuid() const;

    /*!
     * Retrieves the UUID of the parent of the subvolume.
     *
     * A subvolume having a parent UUID indicates that it is a snapshot of that parent.
     *
     * @return the parent's UUID if it has one, else a null UUID
     */
    QUuid parentUuid() const;

    /*!
     * Retrieves the creation time of the subvolume.
     *
     * @return the time of its creation
     */
    QDateTime creationTime() const;
};
}

#endif
