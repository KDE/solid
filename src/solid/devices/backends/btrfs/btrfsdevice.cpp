/*
 * SPDX-FileCopyrightText: 2026 Bharadwaj Raju <bharadwaj.raju@machinesoul.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "btrfsdevice.h"
#include "btrfsgenericinterface.h"
#include "btrfshandling.h"

#include "btrfsstoragesubvolumetree.h"

using namespace Solid::Backends::Btrfs;

BtrfsDevice::BtrfsDevice(const QString &udi)
    : Solid::Ifaces::Device()
    , m_udi(udi)
{
    m_props = BtrfsHandling::deviceProperties(m_udi);
}

BtrfsDevice::~BtrfsDevice()
{
}

QString BtrfsDevice::udi() const
{
    return m_udi;
}

QString BtrfsDevice::parentUdi() const
{
    // TODO make the parent == what it is a snapshot of?
    // or the block dev?
    return QStringLiteral(BTRFS_UDI_PREFIX);
}

QString BtrfsDevice::vendor() const
{
    return QStringLiteral("Btrfs");
}

QString BtrfsDevice::product() const
{
    return m_props.value(QStringLiteral("Path")).toString();
}

QString BtrfsDevice::icon() const
{
    return QStringLiteral("folder");
}

QStringList BtrfsDevice::emblems() const
{
    return QStringList();
}

QString BtrfsDevice::displayName() const
{
    return product();
}

QString BtrfsDevice::description() const
{
    return tr("Btrfs Subvolume on %1").arg(m_props.value(QStringLiteral("SourceDevice")).toString());
}

bool BtrfsDevice::queryDeviceInterface(const Solid::DeviceInterface::Type &type) const
{
    return type == Solid::DeviceInterface::StorageSubvolumeTree | type == Solid::DeviceInterface::GenericInterface;
}

QObject *BtrfsDevice::createDeviceInterface(const Solid::DeviceInterface::Type &type)
{
    if (type == Solid::DeviceInterface::StorageSubvolumeTree) {
        return new BtrfsStorageSubvolumeTree(this);
    }
    if (type == Solid::DeviceInterface::GenericInterface) {
        return new BtrfsGenericInterface(this);
    }
    return nullptr;
}

QVariant BtrfsDevice::prop(const QString &key) const
{
    return m_props.value(key);
}

bool BtrfsDevice::propertyExists(const QString &key) const
{
    return m_props.contains(key);
}

QVariantMap BtrfsDevice::allProperties() const
{
    return m_props;
}

#include "moc_btrfsdevice.cpp"
