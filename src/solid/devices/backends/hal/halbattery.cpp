/*
    SPDX-FileCopyrightText: 2006 Kevin Ottens <ervin@kde.org>
    SPDX-FileCopyrightText: 2012 Lukas Tinkl <ltinkl@redhat.com>
    SPDX-FileCopyrightText: 2014 Kai Uwe Broulik <kde@privat.broulik.de>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
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
    return remainingTime();
}

qlonglong Battery::timeToFull() const
{
    // NOTE Hal doesn't differentiate between time to empty and full
    return remainingTime();
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

qlonglong Battery::remainingTime() const
{
    return static_cast<HalDevice *>(m_device)->prop("battery.remaining_time").toLongLong();
}

void Battery::slotPropertyChanged(const QMap<QString, int> &changes)
{
    if (changes.contains("battery.present")) {
        Q_EMIT presentStateChanged(isPresent(), m_device->udi());
    }

    if (changes.contains("battery.charge_level.percentage")) {
        Q_EMIT chargePercentChanged(chargePercent(), m_device->udi());
    }

    if (changes.contains("battery.charge_level.last_full")
            || changes.contains("battery.charge_level.design")) {
        Q_EMIT capacityChanged(capacity(), m_device->udi());
    }

    if (changes.contains("battery.rechargeable.is_charging")
            || changes.contains("battery.rechargeable.is_discharging")) {
        Q_EMIT chargeStateChanged(chargeState(), m_device->udi());
    }

    if (changes.contains("battery.remaining_time")) {
        Q_EMIT timeToEmptyChanged(timeToEmpty(), m_device->udi());
        Q_EMIT timeToFullChanged(timeToFull(), m_device->udi());
        Q_EMIT remainingTimeChanged(remainingTime(), m_device->udi());
    }

    if (changes.contains("battery.charge_level.current")) {
        Q_EMIT energyChanged(energy(), m_device->udi());
    }

    if (changes.contains("battery.charge_level.last_full")) {
        Q_EMIT energyFullChanged(energyFull(), m_device->udi());
    }

    if (changes.contains("battery.charge_level.design")) {
        Q_EMIT energyFullDesignChanged(energyFullDesign(), m_device->udi());
    }

    if (changes.contains("battery.charge_level.rate")) {
        Q_EMIT energyRateChanged(energyRate(), m_device->udi());
    }

    if (changes.contains("battery.voltage.current")) {
        Q_EMIT voltageChanged(voltage(), m_device->udi());
    }

}
