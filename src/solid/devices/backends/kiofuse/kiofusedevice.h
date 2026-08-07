/*
    SPDX-FileCopyrightText: 2026 Chinmoy Pradhan <chinmoy.pradhan@machinesoul.in>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_BACKENDS_KIOFUSE_DEVICE_H
#define SOLID_BACKENDS_KIOFUSE_DEVICE_H

#include <solid/devices/ifaces/device.h>

#include <QString>
#include <QUrl>

namespace Solid
{
namespace Backends
{
namespace KioFuse
{
class Manager;

class Device : public Solid::Ifaces::Device
{
    Q_OBJECT

public:
    Device(Manager *manager, const QString &udi);
    ~Device() override;

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

    QUrl url() const;

    QString filePath() const;

private:
    Manager *m_manager;
    QString m_udi;
    QUrl m_url;
};

}
}
}

#endif // SOLID_BACKENDS_KIOFUSE_DEVICE_H
