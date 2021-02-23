/*
    SPDX-FileCopyrightText: 2009 Pino Toscano <pino@kde.org>
    SPDX-FileCopyrightText: 2009-2012 Lukáš Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef UDISKSSTORAGEACCESS_H
#define UDISKSSTORAGEACCESS_H

#include <solid/devices/ifaces/storageaccess.h>
#include "udisksdeviceinterface.h"

#include <QDBusMessage>
#include <QDBusError>

namespace Solid
{
namespace Backends
{
namespace UDisks2
{
class StorageAccess : public DeviceInterface, virtual public Solid::Ifaces::StorageAccess
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::StorageAccess)

public:
    StorageAccess(Device *device);
    virtual ~StorageAccess();

    bool isAccessible() const override;
    QString filePath() const override;
    bool isIgnored() const override;
    bool setup() override;
    bool teardown() override;
    bool isEncrypted() const override;

Q_SIGNALS:
    void accessibilityChanged(bool accessible, const QString &udi) override;
    void setupDone(Solid::ErrorType error, QVariant errorData, const QString &udi) override;
    void teardownDone(Solid::ErrorType error, QVariant errorData, const QString &udi) override;
    void setupRequested(const QString &udi) override;
    void teardownRequested(const QString &udi) override;

public Q_SLOTS:
    Q_SCRIPTABLE Q_NOREPLY void passphraseReply(const QString &passphrase);

private Q_SLOTS:
    void slotDBusReply(const QDBusMessage &reply);
    void slotDBusError(const QDBusError &error);

    void connectDBusSignals();

    void slotSetupRequested();
    void slotSetupDone(int error, const QString &errorString);
    void slotTeardownRequested();
    void slotTeardownDone(int error, const QString &errorString);

    void checkAccessibility();

private:
    /// @return true if this device is luks and unlocked
    bool isLuksDevice() const;

    void updateCache();

    bool mount();
    bool unmount();

    bool requestPassphrase();
    void callCryptoSetup(const QString &passphrase);
    bool callCryptoTeardown(bool actOnParent = false);

    QString generateReturnObjectPath();
    QString clearTextPath() const;

private:
    bool m_isAccessible;
    bool m_setupInProgress;
    bool m_teardownInProgress;
    bool m_passphraseRequested;
    QString m_lastReturnObject;

    static const int s_unmountTimeout = 0x7fffffff;
};
}
}
}

#endif // UDISKSSTORAGEACCESS_H
