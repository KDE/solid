/*
 * SPDX-FileCopyrightText: 2026 Bharadwaj Raju <bharadwaj.raju@machinesoul.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef SOLID_BACKENDS_BTRFS_BTRFSHANDLING_H
#define SOLID_BACKENDS_BTRFS_BTRFSHANDLING_H

#include <QHash>
#include <QStringList>
#include <QVariantMap>

#define BTRFS_UDI_PREFIX "/org/kde/solid/btrfs"

namespace Solid
{
namespace Backends
{
namespace Btrfs
{

class BtrfsHandling
{
public:
    BtrfsHandling();
    ~BtrfsHandling();

    static void updateSubvolumes();
    static QStringList deviceList();
    static QVariantMap deviceProperties(const QString &udi);

private:
    static void discoverSubvolumesForMount(const QString &mountpoint, const QString &sourceDevice);

    // UDI -> Subvolume Metadata
    QHash<QString, QVariantMap> m_subvolCache;
    bool m_cacheValid = false;
};

}
}
}
#endif
