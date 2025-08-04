/*
    SPDX-FileCopyrightText: 2013 Patrick von Reth <vonreth@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef WINBATTERY_H
#define WINBATTERY_H

#include <solid/devices/ifaces/battery.h>

#include "wininterface.h"

namespace Solid
{
namespace Backends
{
namespace Win
{
class WinBattery : public WinInterface, virtual public Solid::Ifaces::Battery
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::Battery)

public:
    typedef QPair<QString, ulong> Battery;

    WinBattery(WinDevice *device);

    virtual Solid::Battery::BatteryType type() const;

    virtual int chargePercent() const;

    virtual int capacity() const;

    int cycleCount() const override;

    virtual bool isRechargeable() const;

    virtual bool isPowerSupply() const;

    virtual Solid::Battery::ChargeState chargeState() const;

    Solid::Battery::Technology technology() const;

    double energy() const;
    double energyFull() const;
    double energyFullDesign() const;
    double energyRate() const;

    double voltage() const;

    virtual qlonglong timeToEmpty() const;

    virtual qlonglong timeToFull() const;

    virtual double temperature() const;

    virtual QString serial() const;

    // not yet implemented
    // ------------
    virtual bool isPresent() const;

    virtual qlonglong remainingTime() const;

    bool chargeLimitSupported() const override;
    bool chargeLimitEnabled() const override;

    bool chargeStartThresholdSupported() const override;
    int chargeStartThreshold() const override;

    bool chargeEndThresholdSupported() const override;
    int chargeEndThreshold() const override;

    // ------------

    static QSet<QString> getUdis();

    static const Battery batteryInfoFromUdi(const QString &udi);

Q_SIGNALS:
    void chargePercentChanged(int value, const QString &udi);
    void capacityChanged(int value, const QString &udi);
    void cycleCountChanged(int value, const QString &udi);
    void chargeStateChanged(int newState, const QString &udi);
    void powerSupplyStateChanged(bool newState, const QString &udi);
    void energyChanged(double energy, const QString &udi);
    void energyFullChanged(double energyFull, const QString &udi);
    void energyFullDesignChanged(double energyFullDesign, const QString &udi);
    void energyRateChanged(double energyRate, const QString &udi);
    void timeToEmptyChanged(qlonglong time, const QString &udi);
    void temperatureChanged(double temperature, const QString &udi);
    void voltageChanged(double voltage, const QString &udi);

    // not yet implemented
    // ------------
    void presentStateChanged(bool newState, const QString &udi);
    void timeToFullChanged(qlonglong time, const QString &udi);
    void remainingTimeChanged(qlonglong time, const QString &udi);
    void chargeLimitSupportedChanged(bool supported, const QString &udi) override;
    void chargeLimitEnabledChanged(bool enabled, const QString &udi) override;
    void chargeStartThresholdSupportedChanged(bool startThresholdSupported, const QString &udi) override;
    void chargeStartThresholdChanged(int startThreshold, const QString &udi) override;
    void chargeEndThresholdSupportedChanged(bool endThresholdSupported, const QString &udi) override;
    void chargeEndThresholdChanged(int endThreshold, const QString &udi) override;
    // ------------

private Q_SLOTS:
    void powerChanged();

private:
    void initSerial(const Battery &b);
    void updateTimeToEmpty(const Battery &b);
    void updateBatteryTemp(const Battery &b);

    static QMap<QString, Battery> m_udiToGDI;
    Solid::Battery::BatteryType m_type;
    int m_charge;
    int m_capacity;
    int m_cycleCount = -1;
    bool m_rechargeable;
    bool m_isPowerSupply;
    Solid::Battery::ChargeState m_state;
    Solid::Battery::Technology m_technology;
    double m_energy;
    double m_energyFull;
    double m_energyFullDesign;
    double m_energyRate;
    double m_voltage;

    QString m_serial;
    ulong m_temperature;
    ulong m_timeUntilEmpty;
};
}
}
}

#endif // WINBATTERY_H
