/*
 * SPDX-FileCopyrightText: 2026 Bharadwaj Raju <bharadwaj.raju@machinesoul.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef SOLID_BACKENDS_BTRFS_GENERICINTERFACE_H
#define SOLID_BACKENDS_BTRFS_GENERICINTERFACE_H

#include "btrfsdevice.h"
#include "btrfsdeviceinterface.h"

#include <solid/devices/ifaces/genericinterface.h>
#include <solid/genericinterface.h>

namespace Solid
{
namespace Backends
{
namespace Btrfs
{
class BtrfsGenericInterface : public BtrfsDeviceInterface, virtual public Solid::Ifaces::GenericInterface
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::GenericInterface)

public:
    BtrfsGenericInterface(BtrfsDevice *device);
    ~BtrfsGenericInterface() override;

    QVariant property(const QString &key) const override;
    QVariantMap allProperties() const override;
    bool propertyExists(const QString &key) const override;

    /* we don't actually raise these, but we need to override them */
Q_SIGNALS:
    void propertyChanged(const QMap<QString, int> &changes) override;
    void conditionRaised(const QString &condition, const QString &reason) override;
};
}
}
}

#endif // SOLID_BACKENDS_BTRFS_GENERICINTERFACE_H
