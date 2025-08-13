/*
    SPDX-FileCopyrightText: 2006 Kevin Ottens <ervin@kde.org>
    SPDX-FileCopyrightText: 2012 Lukas Tinkl <ltinkl@redhat.com>
    SPDX-FileCopyrightText: 2014 Kai Uwe Broulik <kde@privat.broulik.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_IFACES_BATTERY_H
#define SOLID_IFACES_BATTERY_H

#include <solid/battery.h>
#include <solid/devices/ifaces/deviceinterface.h>

namespace Solid
{
namespace Ifaces
{
/**
 * This device interface is available on batteries.
 */
class Battery : virtual public DeviceInterface
{
public:
    /**
     * Destroys a Battery object.
     */
    ~Battery() override;

    /**
     * Indicates if this battery is currently present in its bay.
     *
     * @return true if the battery is present, false otherwise
     */
    virtual bool isPresent() const = 0;

    /**
     * Retrieves the type of device holding this battery.
     *
     * @return the type of device holding this battery
     * @see Solid::Battery::BatteryType
     */
    virtual Solid::Battery::BatteryType type() const = 0;

    /**
     * Retrieves the current charge level of the battery normalised
     * to percent.
     *
     * @return the current charge level normalised to percent
     */
    virtual int chargePercent() const = 0;

    /**
     * Retrieves the battery capacity normalised to percent,
     * meaning how much energy can it hold compared to what it is designed to.
     * The capacity of the battery will reduce with age.
     * A capacity value less than 75% is usually a sign that you should renew your battery.
     *
     * @since 4.11
     * @return the battery capacity normalised to percent
     */
    virtual int capacity() const = 0;

    /**
     * Retrieves the number of charge cycles this battery has experienced so far,
     * or -1 if this information is unavailable.
     *
     * @since 6.9
     * @return the number of charge cycles
     */
    virtual int cycleCount() const = 0;

    /**
     * Indicates if the battery is rechargeable.
     *
     * @return true if the battery is rechargeable, false otherwise (one time usage)
     */
    virtual bool isRechargeable() const = 0;

    /**
     * Indicates if the battery is powering the machine.
     *
     * @return true if the battery is powersupply, false otherwise
     */
    virtual bool isPowerSupply() const = 0;

    /**
     * Retrieves the current charge state of the battery. It can be in a stable
     * state (no charge), charging or discharging.
     *
     * @return the current battery charge state
     * @see Solid::Battery::ChargeState
     */
    virtual Solid::Battery::ChargeState chargeState() const = 0;

    /**
     * Time (in seconds) until the battery is empty.
     *
     * @return time until the battery is empty
     * @since 5.0
     */
    virtual qlonglong timeToEmpty() const = 0;

    /**
     * Time (in seconds) until the battery is full.
     *
     * @return time until the battery is full
     * @since 5.0
     */
    virtual qlonglong timeToFull() const = 0;

    /**
     * Retrieves the technology used to manufacture the battery.
     *
     * @return the battery technology
     * @see Solid::Battery::Technology
     */
    virtual Solid::Battery::Technology technology() const = 0;

    /**
     * Amount of energy (measured in Wh) currently available in the power source.
     *
     * @return amount of battery energy in Wh
     */
    virtual double energy() const = 0;

    /**
     * Amount of energy (measured in Wh) the battery has when it is full.
     *
     * @return amount of battery energy when full in Wh
     * @since 5.7
     */
    virtual double energyFull() const = 0;

    /**
     * Amount of energy (measured in Wh) the battery should have by design hen it is full.
     *
     * @return amount of battery energy when full by design in Wh
     * @since 5.7
     */
    virtual double energyFullDesign() const = 0;

    /**
     * Amount of energy being drained from the source, measured in W.
     * If positive, the source is being discharged, if negative it's being charged.
     *
     * @return battery rate in Watts
     *
     */
    virtual double energyRate() const = 0;

    /**
     * Voltage in the Cell or being recorded by the meter.
     *
     * @return voltage in Volts
     */
    virtual double voltage() const = 0;

    /**
     * The temperature of the battery in degrees Celsius.
     *
     * @return the battery temperature in degrees Celsius
     * @since 5.0
     */
    virtual double temperature() const = 0;

    /**
     * The serial number of the battery
     *
     * @return the serial number of the battery
     * @since 5.0
     */
    virtual QString serial() const = 0;

    /**
     * Retrieves the current estimated remaining time of the system batteries
     *
     * @return the current global estimated remaining time in seconds
     * @since 5.8
     */
    virtual qlonglong remainingTime() const = 0;

    /**
     * Indicates if setting battery charge limits is supported for this battery.
     *
     * @return true if limits are supported, false otherwise
     * @since 6.18
     */
    virtual bool chargeLimitSupported() const = 0;

    /**
     * Indicates if battery charge limits are applied.
     *
     * This may or may not make use of charge end or charge start thresholds,
     * depending on firmware behavior.
     *
     * @return true if limits are applied, false otherwise
     * @since 6.18
     */
    virtual bool chargeLimitEnabled() const = 0;

    /**
     * Indicates if chargeStartThreshold() will be used when
     * chargeThresholdEnabled() == true.
     *
     * @return true if the charge start threshold value is relevant, false otherwise
     * @since 6.18
     */
    virtual bool chargeStartThresholdSupported() const = 0;

    /**
     * When chargeThresholdEnabled() == true and a start charge threshold is set,
     * the battery won't get charged until the charge drops under this threshold.
     * Undefined and ignored if chargeStartThresholdSupported() == false.
     *
     * @return charge start threshold percentage between 0 and 100 (0 if not set)
     * @since 6.18
     */
    virtual int chargeStartThreshold() const = 0;

    /**
     * Indicates if chargeEndThreshold() will be used when
     * chargeThresholdEnabled() == true.
     *
     * @return true if the charge end threshold value is relevant, false otherwise
     * @since 6.18
     */
    virtual bool chargeEndThresholdSupported() const = 0;

    /**
     * When chargeThresholdEnabled() == true and an end charge threshold is set,
     * the battery stops getting charged after the set threshold.
     * Undefined and ignored if chargeEndThresholdSupported() == false.
     *
     * @return charge end threshold percentage between 0 and 100 (100 if not set)
     * @since 6.18
     */
    virtual int chargeEndThreshold() const = 0;

protected:
    // Q_SIGNALS:
    /**
     * This signal is emitted if the battery gets plugged in/out of the
     * battery bay.
     *
     * @param newState the new plugging state of the battery, type is boolean
     * @param udi the UDI of the battery with thew new plugging state
     */
    virtual void presentStateChanged(bool newState, const QString &udi) = 0;

    /**
     * This signal is emitted when the charge percent value of this
     * battery has changed.
     *
     * @param value the new charge percent value of the battery
     * @param udi the UDI of the battery with the new charge percent
     */
    virtual void chargePercentChanged(int value, const QString &udi) = 0;

    /**
     * This signal is emitted when the capacity of this battery has changed.
     *
     * @param value the new capacity of the battery
     * @param udi the UDI of the battery with the new capacity
     * @since 4.11
     */
    virtual void capacityChanged(int value, const QString &udi) = 0;

    /**
     * This signal is emitted when the number of charge cycles of the
     * battery has changed.
     *
     * @param value the new number of charge cycles of the battery
     * @param udi the UDI of the battery with the new number of charge cycles
     * @since 6.9
     */
    virtual void cycleCountChanged(int value, const QString &udi) = 0;

    /**
     * This signal is emitted when the power supply state of the battery
     * changes.
     *
     * @param newState the new power supply state, type is boolean
     * @param udi the UDI of the battery with the new power supply state
     * @since 4.11
     */
    virtual void powerSupplyStateChanged(bool newState, const QString &udi) = 0;

    /**
     * This signal is emitted when the charge state of this battery
     * has changed.
     *
     * @param newState the new charge state of the battery, it's one of
     * the type Solid::Battery::ChargeState
     * @see Solid::Battery::ChargeState
     * @param udi the UDI of the battery with the new charge state
     */
    virtual void chargeStateChanged(int newState, const QString &udi = QString()) = 0;

    /**
     * This signal is emitted when the time until the battery is empty
     * has changed.
     *
     * @param time the new remaining time
     * @param udi the UDI of the battery with the new remaining time
     * @since 5.0
     */
    virtual void timeToEmptyChanged(qlonglong time, const QString &udi) = 0;

    /**
     * This signal is emitted when the time until the battery is full
     * has changed.
     *
     * @param time the new remaining time
     * @param udi the UDI of the battery with the new remaining time
     * @since 5.0
     */
    virtual void timeToFullChanged(qlonglong time, const QString &udi) = 0;

    /**
     * This signal is emitted when the energy value of this
     * battery has changed.
     *
     * @param energy the new energy value of the battery
     * @param udi the UDI of the battery with the new energy value
     */
    virtual void energyChanged(double energy, const QString &udi) = 0;

    /**
     * This signal is emitted when the energy full value of this
     * battery has changed.
     *
     * @param energy the new energy full value of the battery
     * @param udi the UDI of the battery with the new energy full value
     */
    virtual void energyFullChanged(double energy, const QString &udi) = 0;

    /**
     * This signal is emitted when the energy full design value of this
     * battery has changed.
     *
     * @param energy the new energy full design value of the battery
     * @param udi the UDI of the battery with the new energy full design value
     */
    virtual void energyFullDesignChanged(double energy, const QString &udi) = 0;

    /**
     * This signal is emitted when the energy rate value of this
     * battery has changed.
     *
     * If positive, the source is being discharged, if negative it's being charged.
     *
     * @param energyRate the new energy rate value of the battery
     * @param udi the UDI of the battery with the new charge percent
     */
    virtual void energyRateChanged(double energyRate, const QString &udi) = 0;

    /**
     * This signal is emitted when the voltage in the cell has changed.
     *
     * @param voltage the new voltage of the cell
     * @param udi the UDI of the battery with the new voltage
     * @since 5.0
     */
    virtual void voltageChanged(double voltage, const QString &udi) = 0;

    /**
     * This signal is emitted when the battery temperature has changed.
     *
     * @param temperature the new temperature of the battery in degrees Celsius
     * @param udi the UDI of the battery with the new temperature
     * @since 5.0
     */
    virtual void temperatureChanged(double temperature, const QString &udi) = 0;

    /**
     * This signal is emitted when the estimated battery remaining time changes.
     *
     * @param time the new remaining time
     * @param udi the UDI of the battery with the new remaining time
     * @since 5.8
     */
    virtual void remainingTimeChanged(qlonglong time, const QString &udi) = 0;

    /**
     * This signal is emitted if support for battery charge limits is
     * newly detected or not detected anymore.
     *
     * @param supported true if limits are supported, false otherwise
     * @param udi the UDI of the battery with the charge limit support
     * @since 6.18
     */
    virtual void chargeLimitSupportedChanged(bool supported, const QString &udi) = 0;

    /**
     * This signal is emitted if battery charge start and end limits
     * start or stop being applied.
     *
     * @param enabled the new state of charge limits being applied
     * @param udi the UDI of the battery with the charge limit change
     * @since 6.18
     */
    virtual void chargeLimitEnabledChanged(bool enabled, const QString &udi) = 0;

    /**
     * This signal is emitted if the battery charge start threshold is
     * newly detected as relevant or not relevant anymore.
     *
     * @param startThresholdSupported true if the start threshold is relevant, false otherwise
     * @param udi the UDI of the battery with the charge limit change
     * @since 6.18
     */
    virtual void chargeStartThresholdSupportedChanged(bool startThresholdSupported, const QString &udi) = 0;

    /**
     * This signal is emitted if the battery charge start threshold changes.
     *
     * @param startThreshold the new charge start threshold  between 0 and 100 (0 if not set)
     * @param udi the UDI of the battery with the charge limit change
     * @since 6.18
     */
    virtual void chargeStartThresholdChanged(int startThreshold, const QString &udi) = 0;

    /**
     * This signal is emitted if the battery charge end threshold is
     * newly detected as relevant or not relevant anymore.
     *
     * @param startThresholdSupported true if the end threshold is relevant, false otherwise
     * @param udi the UDI of the battery with the charge limit change
     * @since 6.18
     */
    virtual void chargeEndThresholdSupportedChanged(bool endThresholdSupported, const QString &udi) = 0;

    /**
     * This signal is emitted if the battery charge end threshold changes.
     *
     * @param endThreshold the new charge end threshold, between 0 and 100 (100 if not set)
     * @param udi the UDI of the battery with the charge limit change
     * @since 6.18
     */
    virtual void chargeEndThresholdChanged(int endThreshold, const QString &udi) = 0;
};
}
}

Q_DECLARE_INTERFACE(Solid::Ifaces::Battery, "org.kde.Solid.Ifaces.Battery/0.3")

#endif
