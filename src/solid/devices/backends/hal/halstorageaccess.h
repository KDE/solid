/*
    SPDX-FileCopyrightText: 2006 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_BACKENDS_HAL_STORAGEACCESS_H
#define SOLID_BACKENDS_HAL_STORAGEACCESS_H

#include <solid/devices/ifaces/storageaccess.h>
#include "haldeviceinterface.h"

#include <QProcess>
#include <QDBusMessage>
#include <QDBusError>

namespace Solid
{
namespace Backends
{
namespace Hal
{
class StorageAccess : public DeviceInterface, virtual public Solid::Ifaces::StorageAccess
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::StorageAccess)

public:
    StorageAccess(HalDevice *device);
    virtual ~StorageAccess();

    bool isAccessible() const override;
    QString filePath() const override;
    bool isIgnored() const override;
    bool setup() override;
    bool teardown() override;

Q_SIGNALS:
    void accessibilityChanged(bool accessible, const QString &udi) override;
    void setupDone(Solid::ErrorType error, QVariant errorData, const QString &udi) override;
    void teardownDone(Solid::ErrorType error, QVariant errorData, const QString &udi) override;
    void ejectDone(Solid::ErrorType error, QVariant errorData, const QString &udi);
    void setupRequested(const QString &udi) override;
    void teardownRequested(const QString &udi) override;

private Q_SLOTS:
    void connectDBusSignals();
    void slotPropertyChanged(const QMap<QString, int> &changes);
    void slotDBusReply(const QDBusMessage &reply);
    void slotDBusError(const QDBusError &error);
    void slotProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void slotSetupRequested();
    void slotTeardownRequested();
    void slotEjectRequested();
    void slotSetupDone(int error, const QString &errorString);
    void slotTeardownDone(int error, const QString &errorString);
    void slotEjectDone(int error, const QString &errorString);

public Q_SLOTS:
    Q_SCRIPTABLE Q_NOREPLY void passphraseReply(const QString &passphrase);

private:
    bool callHalVolumeMount();
    bool callHalVolumeUnmount();
    bool callHalVolumeEject();

    bool callSystemMount();
    bool callSystemUnmount();

    bool requestPassphrase();
    void callCryptoSetup(const QString &passphrase);
    bool callCryptoTeardown();

private:
    bool m_setupInProgress;
    bool m_teardownInProgress;
    bool m_ejectInProgress;
    bool m_passphraseRequested;
    QString m_lastReturnObject;
    QProcess *m_process;
};
}
}
}

#endif // SOLID_BACKENDS_HAL_STORAGEACCESS_H
