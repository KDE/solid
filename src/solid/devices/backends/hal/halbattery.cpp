/*
    Copyright 2006 Kevin Ottens <ervin@kde.org>
    Copyright 2012 Lukas Tinkl <ltinkl@redhat.com>
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

#include "halbattery.h"

using namespace Solid::Backends::Hal;

Battery::Battery(HalDevice *device)
    : DeviceInterface(device)
{
    connect(device, SIGNAL(propertyChanged(QMap<QString,int>)),
            this, SLOT(slotPropertyChanged(QMap<QString,int>)));
}

Battery::~Battery()
{

}

bool Battery::isPresent() const
{
    return static_cast<HalDevice *>(m_device)->prop("battery.present").toBool();
}

Solid::Battery::BatteryType Battery::type() const
{
    QString name = static_cast<HalDevice *>(m_device)->prop("battery.type").toString();

    if (name == "pda") {
        return Solid::Battery::PdaBattery;
    } else if (name == "ups") {
        return Solid::Battery::UpsBattery;
    } else if (name == "primary") {
        return Solid::Battery::PrimaryBattery;
    } else if (name == "mouse") {
        return Solid::Battery::MouseBattery;
    } else if (name == "keyboard") {
        return Solid::Battery::KeyboardBattery;
    } else if (name == "keyboard_mouse") {
        return Solid::Battery::KeyboardMouseBattery;
    } else if (name == "camera") {
        return Solid::Battery::CameraBattery;
    } else {
        return Solid::Battery::UnknownBattery;
    }
}

int Battery::chargePercent() const
{
    return static_cast<HalDevice *>(m_device)->prop("battery.charge_level.percentage").toInt();
}

int Battery::capacity() const
{
    const qreal lastFull = static_cast<HalDevice *>(m_device)->prop("battery.charge_level.last_full").toDouble();
    const qreal designFull = static_cast<HalDevice *>(m_device)->prop("battery.charge_level.design").toDouble();

    return lastFull / designFull * 100;
}

bool Battery::isRechargeable() const
{
    return static_cast<HalDevice *>(m_device)->prop("battery.is_rechargeable").toBool();
}

bool Battery::isPowerSupply() const
{
    // NOTE Hal doesn't support the is power supply property, so we're assuming that primary
    // and UPS batteries are power supply and all the others are not
    if (type() == Solid::Battery::PrimaryBattery || type() == Solid::Battery::UpsBattery) {
        return true;
    }

    return false;
}

Solid::Battery::ChargeState Battery::chargeState() const
{
    bool charging = static_cast<HalDevice *>(m_device)->prop("battery.rechargeable.is_charging").toBool();
    bool discharging = static_cast<HalDevice *>(m_device)->prop("battery.rechargeable.is_discharging").toBool();

    if (!charging && !discharging) {
        return Solid::Battery::NoCharge;
    } else if (charging) {
        return Solid::Battery::Charging;
    } else {
        return Solid::Battery::Discharging;
    }
}

qlonglong Battery::timeToEmpty() const
{
    // NOTE Hal doesn't differentiate between time to empty and full
    return static_cast<HalDevice *>(m_device)->prop("battery.remaining_time").toLongLong();
}

qlonglong Battery::timeToFull() const
{
    return static_cast<HalDevice *>(m_device)->prop("battery.remaining_time").toLongLong();
}

Solid::Battery::Technology Battery::technology() const
{
    const QString tech = static_cast<HalDevice *>(m_device)->prop("battery.technology").toString();

    if (tech == "lithium-ion") {
        return Solid::Battery::LithiumIon;
    } else if (tech == "lead-acid") {
        return Solid::Battery::LeadAcid;
    } else if (tech == "lithium-polymer") {
        return Solid::Battery::LithiumPolymer;
    } else if (tech == "nickel-metal-hydride") {
        return Solid::Battery::NickelMetalHydride;
    } else if (tech == "lithium-iron-phosphate") {
        return Solid::Battery::LithiumIronPhosphate;
    }

    return Solid::Battery::UnknownTechnology;
}

double Battery::energy() const
{
    return static_cast<HalDevice *>(m_device)->prop("battery.charge_level.current").toInt() / 1000;
}

double Battery::energyFull() const
{
    return static_cast<HalDevice *>(m_device)->prop("battery.charge_level.last_full").toInt() / 1000;
}

double Battery::energyFullDesign() const
{
    return static_cast<HalDevice *>(m_device)->prop("battery.charge_level.design").toInt() / 1000;
}

double Battery::energyRate() const
{
    return static_cast<HalDevice *>(m_device)->prop("battery.charge_level.rate").toInt() / 1000;
}

double Battery::voltage() const
{
    return static_cast<HalDevice *>(m_device)->prop("battery.voltage.current").toInt() / 1000;
}

double Battery::temperature() const
{
    return 0; // not supported by HAL
}

bool Battery::isRecalled() const
{
    return static_cast<HalDevice *>(m_device)->prop("info.is_recalled").toBool();
}

QString Battery::recallVendor() const
{
    return static_cast<HalDevice *>(m_device)->prop("info.recall.vendor").toString();
}

QString Battery::recallUrl() const
{
    return static_cast<HalDevice *>(m_device)->prop("info.recall.website_url").toString();
}

QString Battery::serial() const
{
    return static_cast<HalDevice *>(m_device)->prop("system.hardware.serial").toString();
}

void Battery::slotPropertyChanged(const QMap<QString, int> &changes)
{
    if (changes.contains("battery.present")) {
        emit presentStateChanged(isPresent(), m_device->udi());
    }

    if (changes.contains("battery.charge_level.percentage")) {
        emit chargePercentChanged(chargePercent(), m_device->udi());
    }

    if (changes.contains("battery.charge_level.last_full")
            || changes.contains("battery.charge_level.design")) {
        emit capacityChanged(capacity(), m_device->udi());
    }

    if (changes.contains("battery.rechargeable.is_charging")
            || changes.contains("battery.rechargeable.is_discharging")) {
        emit chargeStateChanged(chargeState(), m_device->udi());
    }

    if (changes.contains("battery.remaining_time")) {
        emit timeToEmptyChanged(timeToEmpty(), m_device->udi());
        emit timeToFullChanged(timeToFull(), m_device->udi());
    }

    if (changes.contains("battery.charge_level.current")) {
        emit energyChanged(energy(), m_device->udi());
    }

    if (changes.contains("battery.charge_level.last_full")) {
        emit energyFullChanged(energyFull(), m_device->udi());
    }

    if (changes.contains("battery.charge_level.design")) {
        emit energyFullDesignChanged(energyFullDesign(), m_device->udi());
    }

    if (changes.contains("battery.charge_level.rate")) {
        emit energyRateChanged(energyRate(), m_device->udi());
    }

    if (changes.contains("battery.voltage.current")) {
        emit voltageChanged(voltage(), m_device->udi());
    }

}
