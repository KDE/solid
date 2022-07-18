/*
    SPDX-FileCopyrightText: 2006-2007 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "storageaccess.h"
#include "storageaccess_p.h"

#include "soliddefs_p.h"
#include <solid/devices/ifaces/storageaccess.h>

Solid::StorageAccess::StorageAccess(QObject *backendObject)
    : DeviceInterface(*new StorageAccessPrivate(), backendObject)
{
    connect(backendObject, SIGNAL(setupDone(Solid::ErrorType, QVariant, QString)), this, SIGNAL(setupDone(Solid::ErrorType, QVariant, QString)));
    connect(backendObject, SIGNAL(teardownDone(Solid::ErrorType, QVariant, QString)), this, SIGNAL(teardownDone(Solid::ErrorType, QVariant, QString)));
    connect(backendObject, SIGNAL(setupRequested(QString)), this, SIGNAL(setupRequested(QString)));
    connect(backendObject, SIGNAL(teardownRequested(QString)), this, SIGNAL(teardownRequested(QString)));

    connect(backendObject, SIGNAL(accessibilityChanged(bool, QString)), this, SIGNAL(accessibilityChanged(bool, QString)));
}

Solid::StorageAccess::StorageAccess(StorageAccessPrivate &dd, QObject *backendObject)
    : DeviceInterface(dd, backendObject)
{
    connect(backendObject,
            SIGNAL(setupDone(Solid::StorageAccess::SetupResult, QVariant, QString)),
            this,
            SIGNAL(setupDone(Solid::StorageAccess::SetupResult, QVariant, QString)));
    connect(backendObject,
            SIGNAL(teardownDone(Solid::StorageAccess::TeardownResult, QVariant, QString)),
            this,
            SIGNAL(teardownDone(Solid::StorageAccess::TeardownResult, QVariant, QString)));
    connect(backendObject, SIGNAL(setupRequested(QString)), this, SIGNAL(setupRequested(QString)));
    connect(backendObject, SIGNAL(teardownRequested(QString)), this, SIGNAL(teardownRequested(QString)));

    connect(backendObject, SIGNAL(accessibilityChanged(bool, QString)), this, SIGNAL(accessibilityChanged(bool, QString)));
}

Solid::StorageAccess::~StorageAccess()
{
}

bool Solid::StorageAccess::isAccessible() const
{
    Q_D(const StorageAccess);
    auto *p = iface_cast<Ifaces::StorageAccess>(d->backendObject());
    return p ? p->isAccessible() : false;
}

QString Solid::StorageAccess::filePath() const
{
    Q_D(const StorageAccess);
    auto *p = iface_cast<Ifaces::StorageAccess>(d->backendObject());
    return p ? p->filePath() : QString();
}

bool Solid::StorageAccess::setup()
{
    Q_D(StorageAccess);
    auto *p = iface_cast<Ifaces::StorageAccess>(d->backendObject());
    return p ? p->setup() : false;
}

bool Solid::StorageAccess::teardown()
{
    Q_D(StorageAccess);
    auto *p = iface_cast<Ifaces::StorageAccess>(d->backendObject());
    return p ? p->teardown() : false;
}

bool Solid::StorageAccess::isIgnored() const
{
    Q_D(const StorageAccess);
    auto *p = iface_cast<Ifaces::StorageAccess>(d->backendObject());
    return p ? p->isIgnored() : true;
}

bool Solid::StorageAccess::isEncrypted() const
{
    Q_D(const StorageAccess);
    auto *p = iface_cast<Ifaces::StorageAccess>(d->backendObject());
    return p ? p->isEncrypted() : false;
}
