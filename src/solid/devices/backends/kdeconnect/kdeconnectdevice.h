/*
 * SPDX-FileCopyrightText: 2026 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef SOLID_BACKENDS_KDECONNECT_DEVICE_H
#define SOLID_BACKENDS_KDECONNECT_DEVICE_H

#include <ifaces/device.h>
#include <solid/deviceinterface.h>

#include <QStringList>

#include <optional>

namespace Solid
{
namespace Backends
{
namespace KdeConnect
{

class Manager;

class Device : public Solid::Ifaces::Device
{
    Q_OBJECT
public:
    Device(Manager *manager, const QString &udi);
    ~Device() override;

    // from kdeconnect-kde/core/pairstate.h
    enum class PairState {
        NotPaired,
        Requested,
        RequestedByPeer,
        Paired,
    };

    QObject *createDeviceInterface(const Solid::DeviceInterface::Type &type) override;
    bool queryDeviceInterface(const Solid::DeviceInterface::Type &type) const override;
    QString description() const override;
    QStringList emblems() const override;
    QString icon() const override;
    QString product() const override;
    QString vendor() const override;
    QString udi() const override;
    QString parentUdi() const override;

    QString deviceType() const;

    QString deviceId() const;
    QString dbusPath() const;

private:
    Manager *const m_manager;
    QString m_udi;
    mutable QStringList m_loadedPlugins;

    QSet<Solid::DeviceInterface::Type> m_supportedDeviceTypes;
};

}
}
}

#endif // SOLID_BACKENDS_KDECONNECT_DEVICE_H
