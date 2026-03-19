/*
 * SPDX-FileCopyrightText: 2026 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef SOLID_BACKENDS_KDECONNECT_BATTERY_H
#define SOLID_BACKENDS_KDECONNECT_BATTERY_H

#include "kdeconnectdeviceinterface.h"
#include <solid/devices/ifaces/battery.h>

#include "dbus/kdeconnect_device_battery_interface.h"

#include <optional>

namespace Solid
{
namespace Backends
{
namespace KdeConnect
{

class Device;

class Battery : public DeviceInterface, virtual public Solid::Ifaces::Battery
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::Battery)

public:
    Battery(Device *device);
    ~Battery() override;

    bool isPresent() const override;

    Solid::Battery::BatteryType type() const override;

    int chargePercent() const override;

    int capacity() const override;

    int cycleCount() const override;

    bool isRechargeable() const override;

    bool isPowerSupply() const override;

    Solid::Battery::ChargeState chargeState() const override;

    qlonglong timeToEmpty() const override;

    qlonglong timeToFull() const override;

    Solid::Battery::Technology technology() const override;

    double energy() const override;

    double energyFull() const override;

    double energyFullDesign() const override;

    double energyRate() const override;

    double voltage() const override;

    double temperature() const override;

    QString serial() const override;

    qlonglong remainingTime() const override;

Q_SIGNALS:
    void presentStateChanged(bool newState, const QString &udi) override;
    void chargePercentChanged(int value, const QString &udi = QString()) override;
    void capacityChanged(int value, const QString &udi) override;
    void cycleCountChanged(int value, const QString &udi) override;
    void powerSupplyStateChanged(bool newState, const QString &udi) override;
    void chargeStateChanged(int newState, const QString &udi = QString()) override;
    void timeToEmptyChanged(qlonglong time, const QString &udi) override;
    void timeToFullChanged(qlonglong time, const QString &udi) override;
    void energyChanged(double energy, const QString &udi) override;
    void energyFullChanged(double energyFull, const QString &udi) override;
    void energyFullDesignChanged(double energyFullDesign, const QString &udi) override;
    void energyRateChanged(double energyRate, const QString &udi) override;
    void voltageChanged(double voltage, const QString &udi) override;
    void temperatureChanged(double temperature, const QString &udi) override;
    void remainingTimeChanged(qlonglong time, const QString &udi) override;

private:
    void refresh(const std::optional<bool> &isCharging, const std::optional<int> &chargePercent);

    OrgKdeKdeconnectDeviceBatteryInterface m_batteryIface;

    mutable std::optional<bool> m_isCharging;
    mutable std::optional<int> m_chargePercent;
    Solid::Battery::BatteryType m_batteryType = Solid::Battery::PhoneBattery;
};

}
}
}

#endif // SOLID_BACKENDS_KDECONNECT_BATTERY_H
