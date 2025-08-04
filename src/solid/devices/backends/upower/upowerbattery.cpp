/*
    SPDX-FileCopyrightText: 2009 Pino Toscano <pino@kde.org>
    SPDX-FileCopyrightText: 2010, 2012, 2015 Lukáš Tinkl <ltinkl@redhat.com>
    SPDX-FileCopyrightText: 2014 Kai Uwe Broulik <kde@privat.broulik.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "upowerbattery.h"

#include "upower.h"

using namespace Solid::Backends::UPower;

Battery::Battery(UPowerDevice *device)
    : DeviceInterface(device)
{
    connect(device, &UPowerDevice::propertyChanged, this, &Battery::slotChanged);

    updateCache();
}

Battery::~Battery()
{
}

bool Battery::isPresent() const
{
    return m_device.data()->prop(QStringLiteral("IsPresent")).toBool();
}

Solid::Battery::BatteryType Battery::type() const
{
    Solid::Battery::BatteryType result = Solid::Battery::UnknownBattery;
    const auto t = static_cast<UpDeviceKind>(m_device.data()->prop(QStringLiteral("Type")).toUInt());
    switch (t) {
    case UP_DEVICE_KIND_LINE_POWER: // TODO
        break;
    case UP_DEVICE_KIND_BATTERY:
        result = Solid::Battery::PrimaryBattery;
        break;
    case UP_DEVICE_KIND_UPS:
        result = Solid::Battery::UpsBattery;
        break;
    case UP_DEVICE_KIND_MONITOR:
        result = Solid::Battery::MonitorBattery;
        break;
    case UP_DEVICE_KIND_MOUSE:
        result = Solid::Battery::MouseBattery;
        break;
    case UP_DEVICE_KIND_KEYBOARD:
        result = Solid::Battery::KeyboardBattery;
        break;
    case UP_DEVICE_KIND_PDA:
        result = Solid::Battery::PdaBattery;
        break;
    case UP_DEVICE_KIND_PHONE:
        result = Solid::Battery::PhoneBattery;
        break;
    case UP_DEVICE_KIND_TABLET:
        result = Solid::Battery::TabletBattery;
        break;
    case UP_DEVICE_KIND_GAMING_INPUT:
        result = Solid::Battery::GamingInputBattery;
        break;
    case UP_DEVICE_KIND_HEADPHONES:
        result = Solid::Battery::HeadphoneBattery;
        break;
    case UP_DEVICE_KIND_HEADSET:
        result = Solid::Battery::HeadsetBattery;
        break;
    case UP_DEVICE_KIND_TOUCHPAD:
        result = Solid::Battery::TouchpadBattery;
        break;
    case UP_DEVICE_KIND_BLUETOOTH_GENERIC:
        result = Solid::Battery::BluetoothBattery;
        break;
    case UP_DEVICE_KIND_UNKNOWN:
        break;
    }

    if (result == Solid::Battery::UnknownBattery) {
        // Check if the battery came from Bluez, which is more useful than unknown battery type
        // UP_DEVICE_KIND_BLUETOOTH_GENERIC is only in UPower 0.99.12
        if (m_device.data()->prop(QStringLiteral("NativePath")).toString().startsWith(QLatin1String("/org/bluez/"))) {
            result = Solid::Battery::BluetoothBattery;
        }
    }

    return result;
}

int Battery::chargePercent() const
{
    return qRound(m_device.data()->prop(QStringLiteral("Percentage")).toDouble());
}

int Battery::capacity() const
{
    return qRound(m_device.data()->prop(QStringLiteral("Capacity")).toDouble());
}

int Battery::cycleCount() const
{
    // New in upower v0.99.14.
    bool ok;
    const int cycleCount = m_device.data()->prop(QStringLiteral("ChargeCycles")).toInt(&ok);
    if (ok) {
        return cycleCount;
    } else {
        return -1;
    }
}

bool Battery::isRechargeable() const
{
    return m_device.data()->prop(QStringLiteral("IsRechargeable")).toBool();
}

bool Battery::isPowerSupply() const
{
    return m_device.data()->prop(QStringLiteral("PowerSupply")).toBool();
}

Solid::Battery::ChargeState Battery::chargeState() const
{
    Solid::Battery::ChargeState result = Solid::Battery::NoCharge;
    const UpDeviceState state = static_cast<UpDeviceState>(m_device.data()->prop(QStringLiteral("State")).toUInt());
    switch (state) {
    case UP_DEVICE_STATE_UNKNOWN:
        result = Solid::Battery::NoCharge; // stable or unknown
        break;
    case UP_DEVICE_STATE_CHARGING:
        result = Solid::Battery::Charging;
        break;
    case UP_DEVICE_STATE_DISCHARGING:
        result = Solid::Battery::Discharging;
        break;
    case UP_DEVICE_STATE_EMPTY: // TODO "Empty"
        break;
    case UP_DEVICE_STATE_FULLY_CHARGED:
        result = Solid::Battery::FullyCharged;
        break;
    case UP_DEVICE_STATE_PENDING_CHARGE: // TODO "Pending charge"
        break;
    case UP_DEVICE_STATE_PENDING_DISCHARGE: // TODO "Pending discharge"
        break;
    case UP_DEVICE_STATE_LAST:
        break;
    }
    return result;
}

qlonglong Battery::timeToEmpty() const
{
    return m_device.data()->prop(QStringLiteral("TimeToEmpty")).toLongLong();
}

qlonglong Battery::timeToFull() const
{
    return m_device.data()->prop(QStringLiteral("TimeToFull")).toLongLong();
}

Solid::Battery::Technology Battery::technology() const
{
    const UpDeviceTechnology tech = static_cast<UpDeviceTechnology>(m_device.data()->prop(QStringLiteral("Technology")).toUInt());
    switch (tech) {
    case UP_DEVICE_TECHNOLOGY_UNKNOWN:
        return Solid::Battery::UnknownTechnology;
    case UP_DEVICE_TECHNOLOGY_LITHIUM_ION:
        return Solid::Battery::LithiumIon;
    case UP_DEVICE_TECHNOLOGY_LITHIUM_POLYMER:
        return Solid::Battery::LithiumPolymer;
    case UP_DEVICE_TECHNOLOGY_LITHIUM_IRON_PHOSPHATE:
        return Solid::Battery::LithiumIronPhosphate;
    case UP_DEVICE_TECHNOLOGY_LEAD_ACID:
        return Solid::Battery::LeadAcid;
    case UP_DEVICE_TECHNOLOGY_NICKEL_CADMIUM:
        return Solid::Battery::NickelCadmium;
    case UP_DEVICE_TECHNOLOGY_NICKEL_METAL_HYDRIDE:
        return Solid::Battery::NickelMetalHydride;
    case UP_DEVICE_TECHNOLOGY_LAST:
        return Solid::Battery::UnknownTechnology;
    }
    return Solid::Battery::UnknownTechnology;
}

double Battery::energy() const
{
    return m_device.data()->prop(QStringLiteral("Energy")).toDouble();
}

double Battery::energyFull() const
{
    return m_device.data()->prop(QStringLiteral("EnergyFull")).toDouble();
}

double Battery::energyFullDesign() const
{
    return m_device.data()->prop(QStringLiteral("EnergyFullDesign")).toDouble();
}

double Battery::energyRate() const
{
    return m_device.data()->prop(QStringLiteral("EnergyRate")).toDouble();
}

double Battery::voltage() const
{
    return m_device.data()->prop(QStringLiteral("Voltage")).toDouble();
}

double Battery::temperature() const
{
    return m_device.data()->prop(QStringLiteral("Temperature")).toDouble();
}

QString Battery::serial() const
{
    return m_device.data()->prop(QStringLiteral("Serial")).toString();
}

qlonglong Battery::remainingTime() const
{
    if (chargeState() == Solid::Battery::Charging) {
        return timeToFull();
    } else if (chargeState() == Solid::Battery::Discharging) {
        return timeToEmpty();
    }

    return -1;
}

bool Battery::chargeLimitSupported() const
{
    // property present since UPower 1.90.5 (2024-08-26)
    return m_device.data()->prop(QStringLiteral("ChargeThresholdSupported")).toBool();
}

bool Battery::chargeLimitEnabled() const
{
    // property present since UPower 1.90.5 (2024-08-26)
    return m_device.data()->prop(QStringLiteral("ChargeThresholdEnabled")).toBool();
}

bool Battery::chargeStartThresholdSupported() const
{
    // informational settings flags supported by a upower > 1.90.9:
    // https://gitlab.freedesktop.org/upower/upower/-/merge_requests/276#note_3034202
    QVariant enabled = m_device.data()->prop(QStringLiteral("ChargeThresholdSettingsSupported"));
    return enabled.isValid() ? (enabled.toUInt() & qToUnderlying(ChargeThresholdSettingsSupportedFlag::StartThreshold)) : chargeLimitSupported();
}

int Battery::chargeStartThreshold() const
{
    bool ok = false; // property present since UPower 1.90.5 (2024-08-26)
    auto threshold = m_device.data()->prop(QStringLiteral("ChargeStartThreshold")).toUInt(&ok);
    return (!ok || threshold == UINT_MAX) ? 0 : static_cast<int>(threshold); // UINT_MAX means not set
}

bool Battery::chargeEndThresholdSupported() const
{
    // informational settings flags supported by a upower > 1.90.9:
    // https://gitlab.freedesktop.org/upower/upower/-/merge_requests/276#note_3034202
    QVariant enabled = m_device.data()->prop(QStringLiteral("ChargeThresholdSettingsSupported"));
    return enabled.isValid() ? (enabled.toUInt() & qToUnderlying(ChargeThresholdSettingsSupportedFlag::EndThreshold)) : chargeLimitSupported();
}

int Battery::chargeEndThreshold() const
{
    bool ok = false; // property present since UPower 1.90.5 (2024-08-26)
    auto threshold = m_device.data()->prop(QStringLiteral("ChargeEndThreshold")).toUInt(&ok);
    return (!ok || threshold == UINT_MAX) ? 100 : static_cast<int>(threshold); // UINT_MAX means not set
}

void Battery::slotChanged()
{
    if (m_device) {
        const bool old_isPresent = m_isPresent;
        const int old_chargePercent = m_chargePercent;
        const int old_capacity = m_capacity;
        const int old_cycleCount = m_cycleCount;
        const bool old_isPowerSupply = m_isPowerSupply;
        const Solid::Battery::ChargeState old_chargeState = m_chargeState;
        const qlonglong old_timeToEmpty = m_timeToEmpty;
        const qlonglong old_timeToFull = m_timeToFull;
        const double old_energy = m_energy;
        const double old_energyFull = m_energyFull;
        const double old_energyFullDesign = m_energyFullDesign;
        const double old_energyRate = m_energyRate;
        const double old_voltage = m_voltage;
        const double old_temperature = m_temperature;
        const bool old_chargeLimitSupported = m_chargeLimitSupported;
        const bool old_chargeLimitEnabled = m_chargeLimitEnabled;
        const bool old_chargeStartThresholdSupported = m_chargeStartThresholdSupported;
        const bool old_chargeEndThresholdSupported = m_chargeEndThresholdSupported;
        const int old_chargeStartThreshold = m_chargeStartThreshold;
        const int old_chargeEndThreshold = m_chargeEndThreshold;
        updateCache();

        if (old_isPresent != m_isPresent) {
            Q_EMIT presentStateChanged(m_isPresent, m_device.data()->udi());
        }

        if (old_chargePercent != m_chargePercent) {
            Q_EMIT chargePercentChanged(m_chargePercent, m_device.data()->udi());
        }

        if (old_capacity != m_capacity) {
            Q_EMIT capacityChanged(m_capacity, m_device.data()->udi());
        }

        if (old_cycleCount != m_cycleCount) {
            Q_EMIT cycleCountChanged(m_cycleCount, m_device.data()->udi());
        }

        if (old_isPowerSupply != m_isPowerSupply) {
            Q_EMIT powerSupplyStateChanged(m_isPowerSupply, m_device.data()->udi());
        }

        if (old_chargeState != m_chargeState) {
            Q_EMIT chargeStateChanged(m_chargeState, m_device.data()->udi());
        }

        if (old_timeToEmpty != m_timeToEmpty) {
            Q_EMIT timeToEmptyChanged(m_timeToEmpty, m_device.data()->udi());
        }

        if (old_timeToFull != m_timeToFull) {
            Q_EMIT timeToFullChanged(m_timeToFull, m_device.data()->udi());
        }

        if (old_energy != m_energy) {
            Q_EMIT energyChanged(m_energy, m_device.data()->udi());
        }

        if (old_energyFull != m_energyFull) {
            Q_EMIT energyFullChanged(m_energyFull, m_device.data()->udi());
        }

        if (old_energyFullDesign != m_energyFullDesign) {
            Q_EMIT energyFullChanged(m_energyFullDesign, m_device.data()->udi());
        }

        if (old_energyRate != m_energyRate) {
            Q_EMIT energyRateChanged(m_energyRate, m_device.data()->udi());
        }

        if (old_voltage != m_voltage) {
            Q_EMIT voltageChanged(m_voltage, m_device.data()->udi());
        }

        if (old_temperature != m_temperature) {
            Q_EMIT temperatureChanged(m_temperature, m_device.data()->udi());
        }

        if (old_timeToFull != m_timeToFull || old_timeToEmpty != m_timeToEmpty) {
            Q_EMIT remainingTimeChanged(remainingTime(), m_device.data()->udi());
        }

        if (old_chargeLimitSupported != m_chargeLimitSupported) {
            Q_EMIT chargeLimitSupportedChanged(m_chargeLimitSupported, m_device.data()->udi());
        }

        if (old_chargeLimitEnabled != m_chargeLimitEnabled) {
            Q_EMIT chargeLimitEnabledChanged(m_chargeLimitEnabled, m_device.data()->udi());
        }

        if (old_chargeStartThresholdSupported != m_chargeStartThresholdSupported) {
            Q_EMIT chargeStartThresholdSupportedChanged(m_chargeStartThresholdSupported, m_device.data()->udi());
        }

        if (old_chargeStartThreshold != m_chargeStartThreshold) {
            Q_EMIT chargeStartThresholdChanged(m_chargeStartThreshold, m_device.data()->udi());
        }

        if (old_chargeEndThresholdSupported != m_chargeEndThresholdSupported) {
            Q_EMIT chargeEndThresholdSupportedChanged(m_chargeEndThresholdSupported, m_device.data()->udi());
        }

        if (old_chargeEndThreshold != m_chargeEndThreshold) {
            Q_EMIT chargeEndThresholdChanged(m_chargeEndThreshold, m_device.data()->udi());
        }
    }
}

void Battery::updateCache()
{
    m_isPresent = isPresent();
    m_chargePercent = chargePercent();
    m_capacity = capacity();
    m_cycleCount = cycleCount();
    m_isPowerSupply = isPowerSupply();
    m_chargeState = chargeState();
    m_timeToEmpty = timeToEmpty();
    m_timeToFull = timeToFull();
    m_energy = energy();
    m_energyFull = energyFull();
    m_energyFullDesign = energyFullDesign();
    m_energyRate = energyRate();
    m_voltage = voltage();
    m_temperature = temperature();
    m_chargeLimitSupported = chargeLimitSupported();
    m_chargeLimitEnabled = chargeLimitEnabled();
    m_chargeStartThresholdSupported = chargeStartThresholdSupported();
    m_chargeStartThreshold = chargeStartThreshold();
    m_chargeEndThresholdSupported = chargeEndThresholdSupported();
    m_chargeEndThreshold = chargeEndThreshold();
}

#include "moc_upowerbattery.cpp"
