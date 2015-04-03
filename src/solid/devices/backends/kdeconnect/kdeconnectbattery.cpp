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

#include "kdeconnectbattery.h"

#include <QDBusPendingReply>

using namespace Solid::Backends::KdeConnect;

Battery::Battery(KdeConnectDevice *device)
    : DeviceInterface(device)
    , m_battery("org.kde.kdeconnect",
                "/modules/kdeconnect/devices/" + device->deviceId(), // ugly, perhaps use udi and slice the actual id out of it
                "org.kde.kdeconnect.device.battery",
                QDBusConnection::sessionBus())
    , m_udi(device->udi())
    , m_chargePercent(0)
    , m_charging(false)
{

    if (m_battery.isValid()) {
        connect(&m_battery, SIGNAL(stateChanged(bool)), this, SLOT(onStateChanged(bool)));
        connect(&m_battery, SIGNAL(chargeChanged(int)), this, SLOT(onChargeChanged(int)));

        // TODO async
        QDBusPendingReply<int> percentageReply = m_battery.asyncCall("charge");
        percentageReply.waitForFinished();
        m_chargePercent = qMax(0, percentageReply.value()); // FIXME should not be neccessary

        QDBusPendingReply<bool> stateReply = m_battery.asyncCall("isCharging");
        stateReply.waitForFinished();
        m_charging = stateReply.value();
    }
}

Battery::~Battery()
{

}

bool Battery::isPresent() const
{
    // We could theoretically keep all the devices and have them not present
    // but that would still clutter your battery monitor, our backend
    // only reports visible devices, so we can just assume they're present
    return true;
}

Solid::Battery::BatteryType Battery::type() const
{
    // TODO KDE Connect can handle more than phones
    return Solid::Battery::PhoneBattery;
}

int Battery::chargePercent() const
{
    return m_chargePercent;
}

int Battery::capacity() const
{
    return 0;
}

bool Battery::isRechargeable() const
{
    return true;
}

bool Battery::isPowerSupply() const
{
    return false; // USB 3.1 anyone? ;)
}

Solid::Battery::ChargeState Battery::chargeState() const
{
    if (m_charging) {
        return Solid::Battery::Charging;
    } else {
        // TODO Fully charged / not charging
        return Solid::Battery::Discharging;
    }
}

qlonglong Battery::timeToEmpty() const
{
    return 0;
}

qlonglong Battery::timeToFull() const
{
    return 0;
}

Solid::Battery::Technology Battery::technology() const
{
    return Solid::Battery::UnknownTechnology;
}

double Battery::energy() const
{
    return 0;
}

double Battery::energyFull() const
{
    return 0;
}

double Battery::energyFullDesign() const
{
    return 0;
}

double Battery::energyRate() const
{
    return 0;
}

double Battery::voltage() const
{
    return 0;
}

double Battery::temperature() const
{
    return 0;
}

bool Battery::isRecalled() const
{
    return false;
}

QString Battery::recallVendor() const
{
    return QString();
}

QString Battery::recallUrl() const
{
    return QString();
}

QString Battery::serial() const
{
    return QString();
}

qlonglong Battery::remainingTime() const
{
    return 0;
}

void Battery::onChargeChanged(int charge)
{
    m_chargePercent = qMax(0, charge); // FIXME should not be neccessary
    emit chargePercentChanged(m_chargePercent, m_udi);
}

void Battery::onStateChanged(bool charging)
{
    m_charging = charging;
    emit chargeStateChanged(chargeState(), m_udi);
}
