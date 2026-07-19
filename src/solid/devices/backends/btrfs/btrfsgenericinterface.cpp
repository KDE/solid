/*
 * SPDX-FileCopyrightText: 2026 Bharadwaj Raju <bharadwaj.raju@machinesoul.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "btrfsgenericinterface.h"
#include "btrfsdevice.h"
#include "btrfsdeviceinterface.h"

using namespace Solid::Backends::Btrfs;

BtrfsGenericInterface::BtrfsGenericInterface(BtrfsDevice *device)
    : BtrfsDeviceInterface(device)
{
}

BtrfsGenericInterface::~BtrfsGenericInterface()
{
}

QVariant BtrfsGenericInterface::property(const QString &key) const
{
    return m_device->prop(key);
}

QVariantMap BtrfsGenericInterface::allProperties() const
{
    return m_device->allProperties();
}

bool BtrfsGenericInterface::propertyExists(const QString &key) const
{
    return m_device->propertyExists(key);
}
