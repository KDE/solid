/*
    SPDX-FileCopyrightText: 2006-2007 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "opticaldrive.h"
#include "opticaldrive_p.h"

#include "soliddefs_p.h"
#include <solid/devices/ifaces/opticaldrive.h>

Solid::OpticalDrive::OpticalDrive(QObject *backendObject)
    : StorageDrive(*new OpticalDrivePrivate(), backendObject)
{
    connect(backendObject, SIGNAL(ejectPressed(QString)), this, SIGNAL(ejectPressed(QString)));
    connect(backendObject, SIGNAL(ejectDone(Solid::ErrorType, QVariant, QString)), this, SIGNAL(ejectDone(Solid::ErrorType, QVariant, QString)));
    connect(backendObject, SIGNAL(ejectRequested(QString)), this, SIGNAL(ejectRequested(QString)));
}

Solid::OpticalDrive::~OpticalDrive()
{
}

Solid::OpticalDrive::MediumTypes Solid::OpticalDrive::supportedMedia() const
{
    Q_D(const OpticalDrive);
    auto *p = iface_cast<Ifaces::OpticalDrive>(d->backendObject());
    return p ? p->supportedMedia() : MediumTypes();
}

int Solid::OpticalDrive::readSpeed() const
{
    Q_D(const OpticalDrive);
    auto *p = iface_cast<Ifaces::OpticalDrive>(d->backendObject());
    return p ? p->readSpeed() : 0;
}

int Solid::OpticalDrive::writeSpeed() const
{
    Q_D(const OpticalDrive);
    auto *p = iface_cast<Ifaces::OpticalDrive>(d->backendObject());
    return p ? p->writeSpeed() : 0;
}

QList<int> Solid::OpticalDrive::writeSpeeds() const
{
    Q_D(const OpticalDrive);
    auto *p = iface_cast<Ifaces::OpticalDrive>(d->backendObject());
    return p ? p->writeSpeeds() : QList<int>();
}

bool Solid::OpticalDrive::eject()
{
    Q_D(OpticalDrive);
    auto *p = iface_cast<Ifaces::OpticalDrive>(d->backendObject());
    return p ? p->eject() : false;
}
