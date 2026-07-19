/*
 * SPDX-FileCopyrightText: 2026 Bharadwaj Raju <bharadwaj.raju@machinesoul.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef BTRFSDEVICEINTERFACE_H
#define BTRFSDEVICEINTERFACE_H

#include "btrfsdevice.h"
#include <ifaces/deviceinterface.h>

#include <QObject>
#include <QStringList>

namespace Solid
{
namespace Backends
{
namespace Btrfs
{
class BtrfsDeviceInterface : public QObject, virtual public Solid::Ifaces::DeviceInterface
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::DeviceInterface)
public:
    BtrfsDeviceInterface(BtrfsDevice *device);
    ~BtrfsDeviceInterface() override;

protected:
    BtrfsDevice *m_device;
};
}
}
}

#endif // BTRFSDEVICEINTERFACE_H
