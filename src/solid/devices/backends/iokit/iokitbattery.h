/*
    Copyright 2009 Harald Fernengel <harry@kdevelop.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SOLID_BACKENDS_IOKIT_BATTERY_H
#define SOLID_BACKENDS_IOKIT_BATTERY_H

#include <solid/devices/ifaces/battery.h>
#include "iokitdeviceinterface.h"

namespace Solid
{
namespace Backends
{
namespace IOKit
{
class IOKitDevice;

class Battery : public DeviceInterface, virtual public Solid::Ifaces::Battery
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::Battery)

public:
    Battery(IOKitDevice *device);
    virtual ~Battery();

    bool isPresent() const Q_DECL_OVERRIDE;
    Solid::Battery::BatteryType type() const Q_DECL_OVERRIDE;

    int chargePercent() const Q_DECL_OVERRIDE;
    int capacity() const Q_DECL_OVERRIDE;

    bool isRechargeable() const Q_DECL_OVERRIDE;
    bool isPowerSupply() const Q_DECL_OVERRIDE;

    Solid::Battery::ChargeState chargeState() const Q_DECL_OVERRIDE;

    qlonglong timeToEmpty() const Q_DECL_OVERRIDE;
    qlonglong timeToFull() const Q_DECL_OVERRIDE;
    double voltage() const Q_DECL_OVERRIDE;
    double temperature() const Q_DECL_OVERRIDE;
    QString serial() const Q_DECL_OVERRIDE;

    // ### the ones below are TODO
    Solid::Battery::Technology technology() const Q_DECL_OVERRIDE { return Solid::Battery::UnknownTechnology; }
    double energy() const Q_DECL_OVERRIDE { return 0.0; }
    double energyFull() const Q_DECL_OVERRIDE { return 0.0; }
    double energyFullDesign() const Q_DECL_OVERRIDE { return 0.0; }
    double energyRate() const Q_DECL_OVERRIDE { return 0.0; }

    bool isRecalled() const Q_DECL_OVERRIDE { return false; }
    QString recallVendor() const Q_DECL_OVERRIDE { return QString(); }
    QString recallUrl() const Q_DECL_OVERRIDE { return QString(); }

    qlonglong remainingTime() const Q_DECL_OVERRIDE { return -1; }

Q_SIGNALS:
    void energyChanged(double energy, const QString &udi) Q_DECL_OVERRIDE;
    void energyFullChanged(double energyFull, const QString &udi) Q_DECL_OVERRIDE;
    void energyFullDesignChanged(double energyFullDesign, const QString &udi) Q_DECL_OVERRIDE;
    void energyRateChanged(double energyRate, const QString &udi) Q_DECL_OVERRIDE;
    void chargePercentChanged(int value, const QString &udi) Q_DECL_OVERRIDE;
    void capacityChanged(int value, const QString &udi) Q_DECL_OVERRIDE;
    void chargeStateChanged(int newState, const QString &udi) Q_DECL_OVERRIDE;
    void presentStateChanged(bool newState, const QString &udi) Q_DECL_OVERRIDE;
    void powerSupplyStateChanged(bool newState, const QString &udi) Q_DECL_OVERRIDE;
    void timeToEmptyChanged(qlonglong time, const QString &udi) Q_DECL_OVERRIDE;
    void timeToFullChanged(qlonglong time, const QString &udi) Q_DECL_OVERRIDE;
    void temperatureChanged(double temperature, const QString &udi) Q_DECL_OVERRIDE;
    void voltageChanged(double voltage, const QString &udi) Q_DECL_OVERRIDE;
    void remainingTimeChanged(qlonglong time, const QString &udi) Q_DECL_OVERRIDE;
};
}
}
}

#endif // SOLID_BACKENDS_IOKIT_BATTERY_H
