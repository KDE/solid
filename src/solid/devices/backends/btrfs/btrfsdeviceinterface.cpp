/*
 * SPDX-FileCopyrightText: 2026 Bharadwaj Raju <bharadwaj.raju@machinesoul.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "btrfsdeviceinterface.h"

using namespace Solid::Backends::Btrfs;

BtrfsDeviceInterface::BtrfsDeviceInterface(BtrfsDevice *device)
    : QObject(device)
    , m_device(device)
{
}

BtrfsDeviceInterface::~BtrfsDeviceInterface()
{
}

#include "moc_btrfsdeviceinterface.cpp"
