/*
 * SPDX-FileCopyrightText: 2026 Bharadwaj Raju <bharadwaj.raju@machinesoul.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "btrfshandling.h"
#include "btrfs_debug.h"

#include <QDateTime>
#include <QFile>
#include <QString>
#include <QThreadStorage>
#include <QUuid>

#include <btrfsutil.h>
#include <libmount/libmount.h>

using namespace Qt::StringLiterals;

using namespace Solid::Backends::Btrfs;

Q_GLOBAL_STATIC(QThreadStorage<BtrfsHandling>, globalBtrfsHandling)

BtrfsHandling::BtrfsHandling()
{
}
BtrfsHandling::~BtrfsHandling()
{
}

QStringList BtrfsHandling::deviceList()
{
    auto &instance = globalBtrfsHandling->localData();
    if (!instance.m_cacheValid) {
        instance.updateSubvolumes();
    }
    return instance.m_subvolCache.keys();
}

QVariantMap BtrfsHandling::deviceProperties(const QString &udi)
{
    auto &instance = globalBtrfsHandling->localData();
    return instance.m_subvolCache.value(udi);
}

void BtrfsHandling::updateSubvolumes()
{
    auto &instance = globalBtrfsHandling->localData();

    instance.m_subvolCache.clear();

    struct libmnt_table *table = mnt_new_table();
    if (!table || mnt_table_parse_mtab(table, NULL) != 0) {
        if (table)
            mnt_free_table(table);
        return;
    }

    struct libmnt_iter *itr = mnt_new_iter(MNT_ITER_FORWARD);
    struct libmnt_fs *fs;

    while (mnt_table_next_fs(table, itr, &fs) == 0) {
        if (qstrcmp(mnt_fs_get_fstype(fs), "btrfs") == 0) {
            const QString mountpoint = QFile::decodeName(mnt_fs_get_target(fs));
            const QString sourceDevice = QFile::decodeName(mnt_fs_get_srcpath(fs));

            discoverSubvolumesForMount(mountpoint, sourceDevice);
        }
    }

    mnt_free_iter(itr);
    mnt_free_table(table);

    instance.m_cacheValid = true;
}

void BtrfsHandling::discoverSubvolumesForMount(const QString &mountpoint, const QString &sourceDevice)
{
    auto &instance = globalBtrfsHandling->localData();

    enum btrfs_util_error btrfs_err;

    struct btrfs_util_subvolume_iterator *iter;
    // TODO add aliases for these so we can build on libbtrfsutil < 6.17 or so
    btrfs_err = btrfs_util_subvolume_iter_create(mountpoint.toLocal8Bit().constData(), 0, 0, &iter);
    if (btrfs_err != 0) {
        qCDebug(BTRFS_LOG()) << btrfs_util_strerror(btrfs_err);
        return;
    }

    struct btrfs_util_subvolume_info iter_info;
    char *iter_path;
    while ((btrfs_err = btrfs_util_subvolume_iter_next_info(iter, &iter_path, &iter_info)) == 0) {
        QString udi = QStringLiteral(BTRFS_UDI_PREFIX) + "/"_L1 + sourceDevice + ":"_L1 + mountpoint + ":/subvol/"_L1 + QString::number(iter_info.id);
        QVariantMap props;
        props["Id"_L1] = static_cast<qulonglong>(iter_info.id);
        props["Uuid"_L1] = QUuid::fromBytes(iter_info.uuid);
        props["Path"_L1] = QString::fromUtf8(iter_path);
        props["SourceDevice"_L1] = sourceDevice;
        props["RootMountpoint"_L1] = mountpoint;
        props["ParentUuid"_L1] = QUuid::fromBytes(iter_info.uuid);
        props["CreationTime"_L1] = QDateTime::fromMSecsSinceEpoch(iter_info.otime.tv_sec * 1000 + iter_info.otime.tv_nsec / 1000000);
        instance.m_subvolCache.insert(udi, props);
        free(iter_path);
    }
}
