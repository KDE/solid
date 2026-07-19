/*
 * SPDX-FileCopyrightText: 2026 Bharadwaj Raju <bharadwaj.raju@machinesoul.in>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef SOLID_BACKENDS_BTRFS_BTRFSMANAGER_H
#define SOLID_BACKENDS_BTRFS_BTRFSMANAGER_H

#include <QSet>
#include <QStringList>
#include <solid/devices/ifaces/devicemanager.h>

namespace Solid
{
namespace Backends
{
namespace Btrfs
{

class BtrfsManager : public Solid::Ifaces::DeviceManager
{
    Q_OBJECT
public:
    explicit BtrfsManager(QObject *parent);
    ~BtrfsManager() override;

    QString udiPrefix() const override;
    QSet<Solid::DeviceInterface::Type> supportedInterfaces() const override;
    QStringList allDevices() override;
    QStringList devicesFromQuery(const QString &parentUdi, Solid::DeviceInterface::Type type) override;
    std::unique_ptr<QObject> createDevice(const QString &udi) override;

public Q_SLOTS:
    void onMountsChanged();

private:
    QSet<Solid::DeviceInterface::Type> m_supportedInterfaces;
    QStringList m_deviceList;
    void _k_updateDeviceList();
};

}
}
}
#endif
