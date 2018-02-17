/*
    Copyright 2009 Harald Fernengel <harry@kdevelop.org>
    Copyright 2017 René J.V. Bertin <rjvbertin@gmail.com>

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

#include "iokitbattery.h"
#include "iokitdevice.h"

#include <QDebug>

// TODO - emit the signals

using namespace Solid::Backends::IOKit;

Battery::Battery(IOKitDevice *device)
    : DeviceInterface(device)
{
}

Battery::~Battery()
{
}

// properties: QMap(("AdapterInfo", QVariant(int, 0))
//     ("Amperage", QVariant(int, 0))
//     ("AvgTimeToEmpty", QVariant(int, 65535))
//     ("AvgTimeToFull", QVariant(int, 65535))
//     ("BatteryInstalled", QVariant(bool, true))
//     ("BatteryInvalidWakeSeconds", QVariant(int, 30))
//     ("BatterySerialNumber", QVariant(QString, "W01286PEED3BA"))
//     ("BootPathUpdated", QVariant(int, 1501532930))
//     ("CellVoltage", QVariant(QVariantList, (QVariant(int, 4136), QVariant(int, 4134), QVariant(int, 4134), QVariant(int, 0))))
//     ("CurrentCapacity", QVariant(int, 5552))
//     ("CycleCount", QVariant(int, 16))
//     ("DesignCapacity", QVariant(int, 5770))
//     ("DesignCycleCount", QVariant(int, 1000))
//     ("DeviceName", QVariant(QString, "bq20z451"))
//     ("ExternalChargeCapable", QVariant(bool, true))
//     ("ExternalConnected", QVariant(bool, true))
//     ("FirmwareSerialNumber", QVariant(int, 48))
//     ("FullPathUpdated", QVariant(int, 1502790621))
//     ("FullyCharged", QVariant(bool, true))
//     ("IOGeneralInterest", QVariant(QString, "IOCommand is not serializable"))
//     ("InstantAmperage", QVariant(int, 0))
//     ("InstantTimeToEmpty", QVariant(int, 65535))
//     ("IsCharging", QVariant(bool, false))
//     ("LegacyBatteryInfo", QVariant(QVariantMap, QMap(("Amperage", QVariant(int, 0))
//         ("Capacity", QVariant(int, 5814))
//         ("Current", QVariant(int, 5552))
//         ("Cycle Count", QVariant(int, 16))
//         ("Flags", QVariant(int, 5))
//         ("Voltage", QVariant(int, 12403)))))
//     ("Location", QVariant(int, 0))
//     ("ManufactureDate", QVariant(int, 16106))
//     ("Manufacturer", QVariant(QString, "SMP"))
//     ("ManufacturerData", QVariant(QByteArray, "\x00\x00\x00\x00\x02\x01\x00\n\x01X\x00\x00\x02K6c\x03""00A\x03""ATL\x00\x12\x00\x00"))
//     ("MaxCapacity", QVariant(int, 5814))
//     ("MaxErr", QVariant(int, 1))
//     ("OperationStatus", QVariant(int, 58435))
//     ("PackReserve", QVariant(int, 200))
//     ("PermanentFailureStatus", QVariant(int, 0))
//     ("PostChargeWaitSeconds", QVariant(int, 120))
//     ("PostDischargeWaitSeconds", QVariant(int, 120))
//     ("Temperature", QVariant(int, 2965))
//     ("TimeRemaining", QVariant(int, 0))
//     ("UserVisiblePathUpdated", QVariant(int, 1502790679))
//     ("Voltage", QVariant(int, 12403))
//     ("className", QVariant(QString, "AppleSmartBattery")))

qlonglong Battery::timeToEmpty() const
{
    if (chargeState() != Solid::Battery::Charging) {
        int t = m_device->property(QStringLiteral("AvgTimeToEmpty")).toInt();
        return t == 65535 ? -1 : t * 60;
    }
    return -1;
}

qlonglong Battery::timeToFull() const
{
    if (chargeState() == Solid::Battery::Charging) {
        int t = m_device->property(QStringLiteral("AvgTimeToFull")).toInt();
        return t == 65535 ? -1 : t * 60;
    }
    return -1;
}

double Battery::voltage() const
{
    return m_device->property(QStringLiteral("Voltage")).toInt() / 1000.0;
}

double Battery::temperature() const
{
    return m_device->property(QStringLiteral("Temperature")).toInt() / 100.0;
}


QString Battery::serial() const
{
    return m_device->property(QStringLiteral("BatterySerialNumber")).toString();
}

bool Battery::isPresent() const
{
    return m_device->property(QStringLiteral("ExternalConnected")).toBool();
}

Solid::Battery::BatteryType Battery::type() const
{
    // TODO - how to figure that one out? Just presume we're
    // only called with the main battery.
    return Solid::Battery::PrimaryBattery;
}

int Battery::chargePercent() const
{
    // always calculate since FullyCharged remains true down to 92% or so.
    int maxCapacity = m_device->property(QStringLiteral("MaxCapacity")).toInt();
    if (maxCapacity == 0) {
        return 0;    // prevent divide by 0
    }
    return int(m_device->property(QStringLiteral("CurrentCapacity")).toInt() * 100.0 / maxCapacity + 0.5);
}

int Battery::capacity() const
{
    if (m_device->iOKitPropertyExists(QStringLiteral("PermanentFailureStatus"))
        && m_device->property(QStringLiteral("PermanentFailureStatus")).toInt()) {
        return 0;
    }
    return 100;
}

bool Battery::isRechargeable() const
{
    return m_device->property(QStringLiteral("CycleCount")).toInt() > 1;
}

bool Battery::isPowerSupply() const
{
    return m_device->iOKitPropertyExists(QStringLiteral("BatteryInstalled"))
        ? m_device->property(QStringLiteral("BatteryInstalled")).toBool()
        : true;
}

Solid::Battery::ChargeState Battery::chargeState() const
{
    if (m_device->property(QStringLiteral("IsCharging")).toBool()) {
        return Solid::Battery::Charging;
    }
    if (m_device->property(QStringLiteral("FullyCharged")).toBool()) {
        return Solid::Battery::FullyCharged;
    }
    return Solid::Battery::Discharging;
}

