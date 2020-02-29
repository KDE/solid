/*
    SPDX-FileCopyrightText: 2006 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "halcdrom.h"

#include <QStringList>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDBusError>

#include "halfstabhandling.h"

using namespace Solid::Backends::Hal;

Cdrom::Cdrom(HalDevice *device)
    : Storage(device), m_ejectInProgress(false)
{
    connect(device, SIGNAL(conditionRaised(QString,QString)),
            this, SLOT(slotCondition(QString,QString)));
    m_device->registerAction("eject", this,
                             SLOT(slotEjectRequested()),
                             SLOT(slotEjectDone(int,QString)));
}

Cdrom::~Cdrom()
{

}

Solid::OpticalDrive::MediumTypes Cdrom::supportedMedia() const
{
    Solid::OpticalDrive::MediumTypes supported;

    QMap<Solid::OpticalDrive::MediumType, QString> map;
    map[Solid::OpticalDrive::Cdr] = "storage.cdrom.cdr";
    map[Solid::OpticalDrive::Cdrw] = "storage.cdrom.cdrw";
    map[Solid::OpticalDrive::Dvd] = "storage.cdrom.dvd";
    map[Solid::OpticalDrive::Dvdr] = "storage.cdrom.dvdr";
    map[Solid::OpticalDrive::Dvdrw] = "storage.cdrom.dvdrw";
    map[Solid::OpticalDrive::Dvdram] = "storage.cdrom.dvdram";
    map[Solid::OpticalDrive::Dvdplusr] = "storage.cdrom.dvdplusr";
    map[Solid::OpticalDrive::Dvdplusrw] = "storage.cdrom.dvdplusrw";
    map[Solid::OpticalDrive::Dvdplusdl] = "storage.cdrom.dvdplusrdl";
    map[Solid::OpticalDrive::Dvdplusdlrw] = "storage.cdrom.dvdplusrwdl";
    map[Solid::OpticalDrive::Bd] = "storage.cdrom.bd";
    map[Solid::OpticalDrive::Bdr] = "storage.cdrom.bdr";
    map[Solid::OpticalDrive::Bdre] = "storage.cdrom.bdre";
    map[Solid::OpticalDrive::HdDvd] = "storage.cdrom.hddvd";
    map[Solid::OpticalDrive::HdDvdr] = "storage.cdrom.hddvdr";
    map[Solid::OpticalDrive::HdDvdrw] = "storage.cdrom.hddvdrw";

    Q_FOREACH (const Solid::OpticalDrive::MediumType type, map.keys()) {
        if (m_device->prop(map[type]).toBool()) {
            supported |= type;
        }
    }

    return supported;
}

int Cdrom::readSpeed() const
{
    return m_device->prop("storage.cdrom.read_speed").toInt();
}

int Cdrom::writeSpeed() const
{
    return m_device->prop("storage.cdrom.write_speed").toInt();
}

QList<int> Cdrom::writeSpeeds() const
{
    QList<int> speeds;
    QStringList speed_strlist = m_device->prop("storage.cdrom.write_speeds").toStringList();

    Q_FOREACH (const QString &speed_str, speed_strlist) {
        speeds << speed_str.toInt();
    }

    return speeds;
}

void Cdrom::slotCondition(const QString &name, const QString &/*reason */)
{
    if (name == "EjectPressed") {
        emit ejectPressed(m_device->udi());
    }
}

bool Cdrom::eject()
{
    if (m_ejectInProgress) {
        return false;
    }
    m_ejectInProgress = true;
    m_device->broadcastActionRequested("eject");

    if (FstabHandling::isInFstab(m_device->prop("block.device").toString())) {
        return callSystemEject();
    } else {
        return callHalDriveEject();
    }
}

void Cdrom::slotEjectRequested()
{
    m_ejectInProgress = true;
    emit ejectRequested(m_device->udi());
}

bool Cdrom::callHalDriveEject()
{
    QString udi = m_device->udi();
    QString interface = "org.freedesktop.Hal.Device.Storage";

    // HACK: Eject doesn't work on cdrom drives when there's a mounted disc,
    // let's try to workaround this by calling a child volume...
    if (m_device->prop("storage.removable.media_available").toBool()) {
        QDBusInterface manager("org.freedesktop.Hal",
                               "/org/freedesktop/Hal/Manager",
                               "org.freedesktop.Hal.Manager",
                               QDBusConnection::systemBus());

        QDBusReply<QStringList> reply = manager.call("FindDeviceStringMatch", "info.parent", udi);

        if (reply.isValid()) {
            const QStringList udis = reply;
            if (!udis.isEmpty()) {
                udi = udis[0];
                interface = "org.freedesktop.Hal.Device.Volume";
            }
        }
    }

    QDBusConnection c = QDBusConnection::systemBus();
    QDBusMessage msg = QDBusMessage::createMethodCall("org.freedesktop.Hal", udi,
                       interface, "Eject");

    msg << QStringList();

    return c.callWithCallback(msg, this,
                              SLOT(slotDBusReply(QDBusMessage)),
                              SLOT(slotDBusError(QDBusError)));
}

bool Solid::Backends::Hal::Cdrom::callSystemEject()
{
    const QString device = m_device->prop("block.device").toString();
    m_process = FstabHandling::callSystemCommand("eject", device,
                this, SLOT(slotProcessFinished(int,QProcess::ExitStatus)));

    return m_process != nullptr;
}

void Cdrom::slotDBusReply(const QDBusMessage &/*reply*/)
{
    m_ejectInProgress = false;
    m_device->broadcastActionDone("eject");
}

void Cdrom::slotDBusError(const QDBusError &error)
{
    m_ejectInProgress = false;

    // TODO: Better error reporting here
    m_device->broadcastActionDone("eject", Solid::UnauthorizedOperation,
                                  QString(error.name() + ": " + error.message()));
}

void Solid::Backends::Hal::Cdrom::slotProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitStatus);
    if (m_ejectInProgress) {
        m_ejectInProgress = false;

        if (exitCode == 0) {
            m_device->broadcastActionDone("eject");
        } else {
            m_device->broadcastActionDone("eject", Solid::UnauthorizedOperation,
                                          m_process->readAllStandardError());
        }
    }

    delete m_process;
}

void Cdrom::slotEjectDone(int error, const QString &errorString)
{
    m_ejectInProgress = false;
    emit ejectDone(static_cast<Solid::ErrorType>(error), errorString, m_device->udi());
}

