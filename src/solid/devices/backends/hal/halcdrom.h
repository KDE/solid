/*
    SPDX-FileCopyrightText: 2006 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_BACKENDS_HAL_CDROM_H
#define SOLID_BACKENDS_HAL_CDROM_H

#include <solid/devices/ifaces/opticaldrive.h>
#include "halstorage.h"

#include <QProcess>
#include <QDBusMessage>
#include <QDBusError>

namespace Solid
{
namespace Backends
{
namespace Hal
{
class Cdrom : public Storage, virtual public Solid::Ifaces::OpticalDrive
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::OpticalDrive)

public:
    Cdrom(HalDevice *device);
    virtual ~Cdrom();

    Solid::OpticalDrive::MediumTypes supportedMedia() const override;
    int readSpeed() const override;
    int writeSpeed() const override;
    QList<int> writeSpeeds() const override;
    bool eject() override;

Q_SIGNALS:
    void ejectPressed(const QString &udi) override;
    void ejectDone(Solid::ErrorType error, QVariant errorData, const QString &udi) override;
    void ejectRequested(const QString &udi);

private Q_SLOTS:
    void slotCondition(const QString &name, const QString &reason);
    void slotDBusReply(const QDBusMessage &reply);
    void slotDBusError(const QDBusError &error);
    void slotProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void slotEjectRequested();
    void slotEjectDone(int error, const QString &errorString);

private:
    bool callHalDriveEject();
    bool callSystemEject();

    bool m_ejectInProgress;
    QProcess *m_process;
};
}
}
}

#endif // SOLID_BACKENDS_HAL_CDROM_H
