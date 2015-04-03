/*
    Copyright 2015 Kai Uwe Broulik <kde@privat.broulik.de>

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


#ifndef KDECONNECTBATTERY_H
#define KDECONNECTBATTERY_H

#include <solid/devices/ifaces/battery.h>

#include <QtDBus/QDBusInterface>

#include "kdeconnectdeviceinterface.h"

namespace Solid {
namespace Backends {
namespace KdeConnect {

class Battery : public DeviceInterface, virtual public Solid::Ifaces::Battery
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::Battery)

public:
    Battery(KdeConnectDevice *device);
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
    Solid::Battery::Technology technology() const Q_DECL_OVERRIDE;
    double energy() const Q_DECL_OVERRIDE;
    double energyFull() const Q_DECL_OVERRIDE;
    double energyFullDesign() const Q_DECL_OVERRIDE;
    double energyRate() const Q_DECL_OVERRIDE;
    double voltage() const Q_DECL_OVERRIDE;
    double temperature() const Q_DECL_OVERRIDE;
    bool isRecalled() const Q_DECL_OVERRIDE;
    QString recallVendor() const Q_DECL_OVERRIDE;
    QString recallUrl() const Q_DECL_OVERRIDE;
    QString serial() const Q_DECL_OVERRIDE;
    qlonglong remainingTime() const Q_DECL_OVERRIDE;

Q_SIGNALS:
    void presentStateChanged(bool newState, const QString &udi) Q_DECL_OVERRIDE;
    void chargePercentChanged(int value, const QString &udi = QString()) Q_DECL_OVERRIDE;
    void capacityChanged(int value, const QString &udi) Q_DECL_OVERRIDE;
    void powerSupplyStateChanged(bool newState, const QString &udi) Q_DECL_OVERRIDE;
    void chargeStateChanged(int newState, const QString &udi = QString()) Q_DECL_OVERRIDE;
    void timeToEmptyChanged(qlonglong time, const QString &udi) Q_DECL_OVERRIDE;
    void timeToFullChanged(qlonglong time, const QString &udi) Q_DECL_OVERRIDE;
    void energyChanged(double energy, const QString &udi) Q_DECL_OVERRIDE;
    void energyFullChanged(double energyFull, const QString &udi) Q_DECL_OVERRIDE;
    void energyFullDesignChanged(double energyFullDesign, const QString &udi) Q_DECL_OVERRIDE;
    void energyRateChanged(double energyRate, const QString &udi) Q_DECL_OVERRIDE;
    void voltageChanged(double voltage, const QString &udi) Q_DECL_OVERRIDE;
    void temperatureChanged(double temperature, const QString &udi) Q_DECL_OVERRIDE;
    void remainingTimeChanged(qlonglong time, const QString &udi) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void onChargeChanged(int charge);
    void onStateChanged(bool charging);

private:
    mutable QDBusInterface m_battery;

    QString m_udi; // ugly, take that from the kdeconnectdevice?
    int m_chargePercent;
    bool m_charging;

};

} // namespace KdeConnect
} // namespace Backends
} // namespace Solid

#endif // KDECONNECTBATTERY_H
