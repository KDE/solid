/*
    Copyright 2006-2007 Kevin Ottens <ervin@kde.org>
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

#include "battery.h"
#include "battery_p.h"

#include "soliddefs_p.h"
#include <solid/devices/ifaces/battery.h>

Solid::Battery::Battery(QObject *backendObject)
    : DeviceInterface(*new BatteryPrivate(), backendObject)
{
    connect(backendObject, SIGNAL(presentStateChanged(bool,QString)),
            this, SIGNAL(presentStateChanged(bool,QString)));

    connect(backendObject, SIGNAL(chargePercentChanged(int,QString)),
            this, SIGNAL(chargePercentChanged(int,QString)));

    connect(backendObject, SIGNAL(capacityChanged(int,QString)),
            this, SIGNAL(capacityChanged(int,QString)));

    connect(backendObject, SIGNAL(powerSupplyStateChanged(bool,QString)),
            this, SIGNAL(powerSupplyStateChanged(bool,QString)));

    connect(backendObject, SIGNAL(chargeStateChanged(int,QString)),
            this, SIGNAL(chargeStateChanged(int,QString)));

    connect(backendObject, SIGNAL(timeToEmptyChanged(qlonglong,QString)),
            this, SIGNAL(timeToEmptyChanged(qlonglong,QString)));

    connect(backendObject, SIGNAL(timeToFullChanged(qlonglong,QString)),
            this, SIGNAL(timeToFullChanged(qlonglong,QString)));

    connect(backendObject, SIGNAL(energyChanged(double,QString)),
            this, SIGNAL(energyChanged(double,QString)));

    connect(backendObject, SIGNAL(energyRateChanged(double,QString)),
            this, SIGNAL(energyRateChanged(double,QString)));

    connect(backendObject, SIGNAL(voltageChanged(double,QString)),
            this, SIGNAL(voltageChanged(double,QString)));

    connect(backendObject, SIGNAL(temperatureChanged(double,QString)),
            this, SIGNAL(temperatureChanged(double,QString)));

}

Solid::Battery::~Battery()
{

}

bool Solid::Battery::isPresent() const
{
    Q_D(const Battery);
    return_SOLID_CALL(Ifaces::Battery *, d->backendObject(), false, isPresent());
}

Solid::Battery::BatteryType Solid::Battery::type() const
{
    Q_D(const Battery);
    return_SOLID_CALL(Ifaces::Battery *, d->backendObject(), UnknownBattery, type());
}

int Solid::Battery::chargePercent() const
{
    Q_D(const Battery);
    return_SOLID_CALL(Ifaces::Battery *, d->backendObject(), 0, chargePercent());
}

int Solid::Battery::capacity() const
{
    Q_D(const Battery);
    return_SOLID_CALL(Ifaces::Battery *, d->backendObject(), 100, capacity());
}

bool Solid::Battery::isRechargeable() const
{
    Q_D(const Battery);
    return_SOLID_CALL(Ifaces::Battery *, d->backendObject(), false, isRechargeable());
}

bool Solid::Battery::isPowerSupply() const
{
    Q_D(const Battery);
    return_SOLID_CALL(Ifaces::Battery *, d->backendObject(), true, isPowerSupply());
}

Solid::Battery::ChargeState Solid::Battery::chargeState() const
{
    Q_D(const Battery);
    return_SOLID_CALL(Ifaces::Battery *, d->backendObject(), NoCharge, chargeState());
}

qlonglong Solid::Battery::timeToEmpty() const
{
    Q_D(const Battery);
    return_SOLID_CALL(Ifaces::Battery *, d->backendObject(), 0, timeToEmpty());
}

qlonglong Solid::Battery::timeToFull() const
{
    Q_D(const Battery);
    return_SOLID_CALL(Ifaces::Battery *, d->backendObject(), 0, timeToFull());
}

Solid::Battery::Technology Solid::Battery::technology() const
{
    Q_D(const Battery);
    return_SOLID_CALL(Ifaces::Battery *, d->backendObject(), UnknownTechnology, technology());
}

double Solid::Battery::energy() const
{
    Q_D(const Battery);
    return_SOLID_CALL(Ifaces::Battery *, d->backendObject(), 0.0, energy());
}

double Solid::Battery::energyRate() const
{
    Q_D(const Battery);
    return_SOLID_CALL(Ifaces::Battery *, d->backendObject(), 0.0, energyRate());
}

double Solid::Battery::voltage() const
{
    Q_D(const Battery);
    return_SOLID_CALL(Ifaces::Battery *, d->backendObject(), 0.0, voltage());
}

double Solid::Battery::temperature() const
{
    Q_D(const Battery);
    return_SOLID_CALL(Ifaces::Battery *, d->backendObject(), 0.0, temperature());
}

bool Solid::Battery::isRecalled() const
{
    Q_D(const Battery);
    return_SOLID_CALL(Ifaces::Battery *, d->backendObject(), false, isRecalled());
}

QString Solid::Battery::recallVendor() const
{
    Q_D(const Battery);
    return_SOLID_CALL(Ifaces::Battery *, d->backendObject(), QString(), recallVendor());
}

QString Solid::Battery::recallUrl() const
{
    Q_D(const Battery);
    return_SOLID_CALL(Ifaces::Battery *, d->backendObject(), QString(), recallUrl());
}

QString Solid::Battery::serial() const
{
    Q_D(const Battery);
    return_SOLID_CALL(Ifaces::Battery *, d->backendObject(), QString(), serial());
}
