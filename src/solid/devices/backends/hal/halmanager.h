/*
    SPDX-FileCopyrightText: 2005, 2006 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_BACKENDS_HAL_HALMANAGER_H
#define SOLID_BACKENDS_HAL_HALMANAGER_H

#include <solid/devices/ifaces/devicemanager.h>
#include <solid/deviceinterface.h>

#include <QStringList>

namespace Solid
{
namespace Backends
{
namespace Hal
{
class HalManagerPrivate;

class HalManager : public Solid::Ifaces::DeviceManager
{
    Q_OBJECT

public:
    HalManager(QObject *parent);
    virtual ~HalManager();

    QString udiPrefix() const override;
    QSet<Solid::DeviceInterface::Type> supportedInterfaces() const override;

    bool deviceExists(const QString &udi);
    QStringList allDevices() override;

    virtual QStringList devicesFromQuery(const QString &parentUdi,
                                         Solid::DeviceInterface::Type type) override;

    QObject *createDevice(const QString &udi) override;

private Q_SLOTS:
    void slotDeviceAdded(const QString &udi);
    void slotDeviceRemoved(const QString &udi);

private:
    HalManagerPrivate *d;
};
}
}
}

#endif // SOLID_BACKENDS_HAL_HALMANAGER_H
