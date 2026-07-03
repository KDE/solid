/*
    SPDX-FileCopyrightText: 2026 Bharadwaj Raju <bharadwaj.raju@machinesoul.in>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_IFACES_STORAGESUBVOLUMETREE_H
#define SOLID_IFACES_STORAGESUBVOLUMETREE_H

#include <solid/devices/ifaces/deviceinterface.h>
#include <solid/storagesubvolumetree.h>

namespace Solid
{
namespace Ifaces
{
/**
 * This device interface is available on filesystems with subvolume capabilities, such as Btrfs.
 *
 * A subvolume is an independent FS hierarchy and inode number space within a larger FS.
 */
class StorageSubvolumeTree : virtual public DeviceInterface
{
public:
    /**
     * Destroys a StorageSubvolumeTree object.
     */
    ~StorageSubvolumeTree() override;

    /**
     * Retrieves the absolute path of this subvolume.
     *
     * @return the absolute path to the subvolume
     */
    virtual QString filePath() const = 0;

    /**
     * Retrieves the internal ID of the subvolume.
     *
     * @return the ID
     */
    virtual qulonglong id() const = 0;

    /**
     * Retrieves the UUID of the subvolume.
     *
     * @return the UUID
     */
    virtual QUuid uuid() const = 0;

    /**
     * Retrieves the UUID of the parent of the subvolume.
     *
     * A subvolume having a parent UUID indicates that it is a snapshot of that parent.
     *
     * @return the parent's UUID if it has one, else a null UUID
     */
    virtual QUuid parentUuid() const = 0;

    /**
     * Retrieves the creation time of the subvolume.
     *
     * @return the time of its creation
     */
    virtual QDateTime creationTime() const = 0;
};
}
}

Q_DECLARE_INTERFACE(Solid::Ifaces::StorageSubvolumeTree, "org.kde.Solid.Ifaces.StorageSubvolumeTree/0.1")

#endif
