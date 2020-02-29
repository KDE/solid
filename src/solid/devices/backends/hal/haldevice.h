/*
    SPDX-FileCopyrightText: 2005, 2006 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_BACKENDS_HAL_HALDEVICE_H
#define SOLID_BACKENDS_HAL_HALDEVICE_H

#include <solid/devices/ifaces/device.h>

class QDBusVariant;

namespace Solid
{
namespace Backends
{
namespace Hal
{
class HalManager;
class HalDevicePrivate;

struct ChangeDescription {
    QString key;
    bool added;
    bool removed;
};

class HalDevice : public Solid::Ifaces::Device
{
    Q_OBJECT

public:
    HalDevice(const QString &udi);
    virtual ~HalDevice();

    QString udi() const override;
    QString parentUdi() const override;

    QString vendor() const override;
    QString product() const override;
    QString icon() const override;
    QStringList emblems() const override;
    QString description() const override;

    bool queryDeviceInterface(const Solid::DeviceInterface::Type &type) const override;
    QObject *createDeviceInterface(const Solid::DeviceInterface::Type &type) override;

public:
    QVariant prop(const QString &key) const;
    QMap<QString, QVariant> allProperties() const;
    bool propertyExists(const QString &key) const;

Q_SIGNALS:
    void propertyChanged(const QMap<QString, int> &changes);
    void conditionRaised(const QString &condition, const QString &reason);

private Q_SLOTS:
    void slotPropertyModified(int count, const QList<ChangeDescription> &changes);
    void slotCondition(const QString &condition, const QString &reason);

private:
    QString storageDescription() const;
    QString volumeDescription() const;

    HalDevicePrivate *d;
};
}
}
}

#endif // SOLID_BACKENDS_HAL_HALDEVICE_H
