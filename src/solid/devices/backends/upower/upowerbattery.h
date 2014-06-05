/*
    Copyright 2009 Pino Toscano <pino@kde.org>
    Copyright 2010, 2012 Lukas Tinkl <ltinkl@redhat.com>
    Copyright 2014 Kai Uwe Broulik <kde@privat.broulik.de>

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

#ifndef SOLID_BACKENDS_UPOWER_BATTERY_H
#define SOLID_BACKENDS_UPOWER_BATTERY_H

#include <solid/devices/ifaces/battery.h>
#include "upowerdeviceinterface.h"

namespace Solid
{
namespace Backends
{
namespace UPower
{
class Battery : public DeviceInterface, virtual public Solid::Ifaces::Battery
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::Battery)

public:
    Battery(UPowerDevice *device);
    virtual ~Battery();

    virtual bool isPresent() const;

    virtual Solid::Battery::BatteryType type() const;

    virtual int chargePercent() const;

    virtual int capacity() const;

    virtual bool isRechargeable() const;

    virtual bool isPowerSupply() const;

    virtual Solid::Battery::ChargeState chargeState() const;

    virtual qlonglong timeToEmpty() const;

    virtual qlonglong timeToFull() const;

    virtual Solid::Battery::Technology technology() const;

    virtual double energy() const;

    virtual double energyRate() const;

    virtual double voltage() const;

    virtual double temperature() const;

    virtual bool isRecalled() const;

    virtual QString recallVendor() const;

    virtual QString recallUrl() const;

    virtual QString serial() const;

Q_SIGNALS:
    void presentStateChanged(bool newState, const QString &udi);
    void chargePercentChanged(int value, const QString &udi = QString());
    void capacityChanged(int value, const QString &udi);
    void powerSupplyStateChanged(bool newState, const QString &udi);
    void chargeStateChanged(int newState, const QString &udi = QString());
    void timeToEmptyChanged(qlonglong time, const QString &udi);
    void timeToFullChanged(qlonglong time, const QString &udi);
    void energyChanged(double energy, const QString &udi);
    void energyRateChanged(double energyRate, const QString &udi);
    void voltageChanged(double voltage, const QString &udi);
    void temperatureChanged(double temperature, const QString &udi);

private Q_SLOTS:
    void slotChanged();

private:
    void updateCache();

    bool m_isPresent;
    int m_chargePercent;
    int m_capacity;
    bool m_isPowerSupply;
    Solid::Battery::ChargeState m_chargeState;
    qlonglong m_timeToEmpty;
    qlonglong m_timeToFull;
    double m_energy;
    double m_energyRate;
    double m_voltage;
    double m_temperature;
};
}
}
}

#endif // SOLID_BACKENDS_UPOWER_BATTERY_H
