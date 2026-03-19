/*
 * SPDX-FileCopyrightText: 2026 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "kdeconnectbattery.h"
#include "kdeconnect_debug.h"
#include "kdeconnectutils.h"

using namespace Solid::Backends::KdeConnect;

Battery::Battery(Device *device)
    : DeviceInterface(device)
    , m_batteryIface(Utils::dbusService(), device->dbusPath() + QLatin1String("/battery"), QDBusConnection::sessionBus())
{
    const QString deviceType = m_device->deviceType();
    // TODO mediacenter? computer?
    if (deviceType != QLatin1String("phone")) {
        // TabletBattery is for graphics tablet, "tablet" device type is a tablet aka large smartphone.
        m_batteryType = Solid::Battery::UnknownBattery;
    }

    connect(&m_batteryIface, &OrgKdeKdeconnectDeviceBatteryInterface::refreshed, this, &Battery::refresh);

    {
        auto message = Utils::getPropertyCall(m_batteryIface, QStringLiteral("charge"));
        auto call = m_batteryIface.connection().asyncCall(message);
        auto *watcher = new QDBusPendingCallWatcher(call, this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, udi = device->udi(), watcher] {
            watcher->deleteLater();

            QDBusPendingReply<QDBusVariant> reply = *watcher;
            if (!reply.isValid()) {
                qCWarning(KDECONNECT) << "Failed to fetch battery charge percent for" << udi << reply.error().message();
            } else {
                refresh(std::nullopt, reply.value().variant().toInt());
            }
        });
    }

    {
        auto message = Utils::getPropertyCall(m_batteryIface, QStringLiteral("isCharging"));
        auto call = m_batteryIface.connection().asyncCall(message);
        auto *watcher = new QDBusPendingCallWatcher(call, this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, udi = device->udi(), watcher] {
            watcher->deleteLater();

            QDBusPendingReply<QDBusVariant> reply = *watcher;
            if (!reply.isValid()) {
                qCWarning(KDECONNECT) << "Failed to fetch battery charge state for" << udi << reply.error().message();
            } else {
                refresh(reply.value().variant().toBool(), std::nullopt);
            }
        });
    }
}

Battery::~Battery() = default;

bool Battery::isPresent() const
{
    return chargePercent() > -1;
}

Solid::Battery::BatteryType Battery::type() const
{
    return m_batteryType;
}

int Battery::chargePercent() const
{
    return m_chargePercent.value_or(-1);
}

int Battery::capacity() const
{
    return -1;
}

int Battery::cycleCount() const
{
    return -1;
}

bool Battery::isRechargeable() const
{
    return true;
}

bool Battery::isPowerSupply() const
{
    return false;
}

Solid::Battery::ChargeState Battery::chargeState() const
{
    if (m_isCharging.has_value()) {
        if (m_isCharging.value()) {
            return Solid::Battery::Charging;
        } else if (isPresent()) {
            if (chargePercent() == 100) {
                return Solid::Battery::FullyCharged;
            } else {
                return Solid::Battery::Discharging;
            }
        }
    }

    return Solid::Battery::NoCharge;
}

qlonglong Battery::timeToEmpty() const
{
    return -1;
}

qlonglong Battery::timeToFull() const
{
    return -1;
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

QString Battery::serial() const
{
    return {};
}

qlonglong Battery::remainingTime() const
{
    return -1;
}

void Battery::refresh(const std::optional<bool> &isCharging, const std::optional<int> &charge)
{
    if (isCharging.has_value() && charge.has_value()) {
        qCDebug(KDECONNECT) << m_device->udi() << "battery refreshed:" << charge.value() << "%" << (isCharging.value() ? "(charging)" : "(discharging)");
    }

    const auto oldChargePercent = chargePercent();
    const auto oldChargeState = chargeState();
    const auto oldPresent = isPresent();

    if (isCharging.has_value()) {
        m_isCharging = isCharging;
    }
    if (charge.has_value()) {
        m_chargePercent = charge;
    }

    if (oldChargePercent != chargePercent()) {
        Q_EMIT chargePercentChanged(chargePercent(), m_device->udi());
    }
    if (oldChargeState != chargeState()) {
        Q_EMIT chargeStateChanged(chargeState(), m_device->udi());
    }
    if (oldPresent != isPresent()) {
        Q_EMIT presentStateChanged(isPresent(), m_device->udi());
    }
}

#include "moc_kdeconnectbattery.cpp"
