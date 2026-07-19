/*
 * SPDX-FileCopyrightText: 2026 Bharadwaj Raju <bharadwaj.raju@machinesoul.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef SOLID_BACKENDS_BTRFS_BTRFSDEVICE_H
#define SOLID_BACKENDS_BTRFS_BTRFSDEVICE_H

#include <QVariantMap>
#include <solid/devices/ifaces/device.h>

namespace Solid
{
namespace Backends
{
namespace Btrfs
{

class BtrfsDevice : public Solid::Ifaces::Device
{
    Q_OBJECT
public:
    explicit BtrfsDevice(const QString &udi);
    ~BtrfsDevice() override;

    QString udi() const override;
    QString parentUdi() const override;
    QString vendor() const override;
    QString product() const override;
    QString icon() const override;
    QStringList emblems() const override;
    QString displayName() const override;
    QString description() const override;

    bool queryDeviceInterface(const Solid::DeviceInterface::Type &type) const override;
    QObject *createDeviceInterface(const Solid::DeviceInterface::Type &type) override;

    QVariant prop(const QString &key) const;
    bool propertyExists(const QString &key) const;
    QVariantMap allProperties() const;

private:
    QString m_udi;
    QVariantMap m_props;
};

}
}
}
#endif
