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

#include "upowerbattery.h"

using namespace Solid::Backends::UPower;

Battery::Battery(UPowerDevice *device)
    : DeviceInterface(device)
{
    connect(device, SIGNAL(changed()), this, SLOT(slotChanged()));

    updateCache();
}

Battery::~Battery()
{
}

bool Battery::isPresent() const
{
    return m_device.data()->prop("IsPresent").toBool();
}

Solid::Battery::BatteryType Battery::type() const
{
    Solid::Battery::BatteryType result = Solid::Battery::UnknownBattery;
    const uint t = m_device.data()->prop("Type").toUInt();
    switch (t) {
    case 1: // TODO "Line Power"
        break;
    case 2:
        result = Solid::Battery::PrimaryBattery;
        break;
    case 3:
        result = Solid::Battery::UpsBattery;
        break;
    case 4:
        result = Solid::Battery::MonitorBattery;
        break;
    case 5:
        result = Solid::Battery::MouseBattery;
        break;
    case 6:
        result = Solid::Battery::KeyboardBattery;
        break;
    case 7:
        result = Solid::Battery::PdaBattery;
        break;
    case 8:
        result = Solid::Battery::PhoneBattery;
        break;
    }
    return result;
}

int Battery::chargePercent() const
{
    return qRound(m_device.data()->prop("Percentage").toDouble());
}

int Battery::capacity() const
{
    return m_device.data()->prop("Capacity").toDouble();
}

bool Battery::isRechargeable() const
{
    return m_device.data()->prop("IsRechargeable").toBool();
}

bool Battery::isPowerSupply() const
{
    return m_device.data()->prop("PowerSupply").toBool();
}

Solid::Battery::ChargeState Battery::chargeState() const
{
    Solid::Battery::ChargeState result = Solid::Battery::NoCharge;
    const uint state = m_device.data()->prop("State").toUInt();
    switch (state) {
    case 0:
        result = Solid::Battery::NoCharge; // stable or unknown
        break;
    case 1:
        result = Solid::Battery::Charging;
        break;
    case 2:
        result = Solid::Battery::Discharging;
        break;
    case 3: // TODO "Empty"
        break;
    case 4:
        result = Solid::Battery::FullyCharged;
        break;
    case 5: // TODO "Pending charge"
        break;
    case 6: // TODO "Pending discharge"
        break;
    }
    return result;
}

qlonglong Battery::timeToEmpty() const
{
    return m_device.data()->prop("TimeToEmpty").toLongLong();
}

qlonglong Battery::timeToFull() const
{
    return m_device.data()->prop("TimeToFull").toLongLong();
}

Solid::Battery::Technology Battery::technology() const
{
    const uint tech = m_device.data()->prop("Technology").toUInt();
    switch (tech) {
    case 1:
        return Solid::Battery::LithiumIon;
    case 2:
        return Solid::Battery::LithiumPolymer;
    case 3:
        return Solid::Battery::LithiumIronPhosphate;
    case 4:
        return Solid::Battery::LeadAcid;
    case 5:
        return Solid::Battery::NickelCadmium;
    case 6:
        return Solid::Battery::NickelMetalHydride;
    default:
        return Solid::Battery::UnknownTechnology;
    }
}

double Battery::energy() const
{
    return m_device.data()->prop("Energy").toDouble();
}

double Battery::energyRate() const
{
    return m_device.data()->prop("EnergyRate").toDouble();
}

double Battery::voltage() const
{
    return m_device.data()->prop("Voltage").toDouble();
}

double Battery::temperature() const
{
    return m_device.data()->prop("Temperature").toDouble();
}

bool Battery::isRecalled() const
{
    return m_device.data()->prop("RecallNotice").toBool();
}

QString Battery::recallVendor() const
{
    return m_device.data()->prop("RecallVendor").toString();
}

QString Battery::recallUrl() const
{
    return m_device.data()->prop("RecallUrl").toString();
}

QString Battery::serial() const
{
    return m_device.data()->prop("Serial").toString();
}

void Battery::slotChanged()
{
    if (m_device) {
        const bool old_isPresent = m_isPresent;
        const int old_chargePercent = m_chargePercent;
        const int old_capacity = m_capacity;
        const bool old_isPowerSupply = m_isPowerSupply;
        const Solid::Battery::ChargeState old_chargeState = m_chargeState;
        const qlonglong old_timeToEmpty = m_timeToEmpty;
        const qlonglong old_timeToFull = m_timeToFull;
        const double old_energy = m_energy;
        const double old_energyRate = m_energyRate;
        const double old_voltage = m_voltage;
        const double old_temperature = m_temperature;
        updateCache();

        if (old_isPresent != m_isPresent) {
            emit presentStateChanged(m_isPresent, m_device.data()->udi());
        }

        if (old_chargePercent != m_chargePercent) {
            emit chargePercentChanged(m_chargePercent, m_device.data()->udi());
        }

        if (old_capacity != m_capacity) {
            emit capacityChanged(m_capacity, m_device.data()->udi());
        }

        if (old_isPowerSupply != m_isPowerSupply) {
            emit powerSupplyStateChanged(m_isPowerSupply, m_device.data()->udi());
        }

        if (old_chargeState != m_chargeState) {
            emit chargeStateChanged(m_chargeState, m_device.data()->udi());
        }

        if (old_timeToEmpty != m_timeToEmpty) {
            emit timeToEmptyChanged(m_timeToEmpty, m_device.data()->udi());
        }

        if (old_timeToFull != m_timeToFull) {
            emit timeToFullChanged(m_timeToFull, m_device.data()->udi());
        }

        if (old_energy != m_energy) {
            emit energyChanged(m_energy, m_device.data()->udi());
        }

        if (old_energyRate != m_energyRate) {
            emit energyRateChanged(m_energyRate, m_device.data()->udi());
        }

        if (old_voltage != m_voltage) {
            emit voltageChanged(m_voltage, m_device.data()->udi());
        }

        if (old_temperature != m_temperature) {
            emit temperatureChanged(m_temperature, m_device.data()->udi());
        }
    }
}

void Battery::updateCache()
{
    m_isPresent = isPresent();
    m_chargePercent = chargePercent();
    m_capacity = capacity();
    m_isPowerSupply = isPowerSupply();
    m_chargeState = chargeState();
    m_timeToEmpty = timeToEmpty();
    m_timeToFull = timeToFull();
    m_energy = energy();
    m_energyRate = energyRate();
    m_voltage = voltage();
    m_temperature = temperature();
}
