/*
    SPDX-FileCopyrightText: 2006-2007 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "opticaldisc.h"
#include "opticaldisc_p.h"

#include "soliddefs_p.h"
#include <solid/devices/ifaces/opticaldisc.h>

Solid::OpticalDisc::OpticalDisc(QObject *backendObject)
    : StorageVolume(*new OpticalDiscPrivate(), backendObject)
{
}

Solid::OpticalDisc::~OpticalDisc()
{
}

Solid::OpticalDisc::ContentTypes Solid::OpticalDisc::availableContent() const
{
    Q_D(const OpticalDisc);
    auto *p = iface_cast<Ifaces::OpticalDisc>(d->backendObject());
    return p ? p->availableContent() : ContentTypes();
}

Solid::OpticalDisc::DiscType Solid::OpticalDisc::discType() const
{
    Q_D(const OpticalDisc);
    auto *p = iface_cast<Ifaces::OpticalDisc>(d->backendObject());
    return p ? p->discType() : UnknownDiscType;
}

bool Solid::OpticalDisc::isAppendable() const
{
    Q_D(const OpticalDisc);
    auto *p = iface_cast<Ifaces::OpticalDisc>(d->backendObject());
    return p ? p->isAppendable() : false;
}

bool Solid::OpticalDisc::isBlank() const
{
    Q_D(const OpticalDisc);
    auto *p = iface_cast<Ifaces::OpticalDisc>(d->backendObject());
    return p ? p->isBlank() : false;
}

bool Solid::OpticalDisc::isRewritable() const
{
    Q_D(const OpticalDisc);
    auto *p = iface_cast<Ifaces::OpticalDisc>(d->backendObject());
    return p ? p->isRewritable() : false;
}

qulonglong Solid::OpticalDisc::capacity() const
{
    Q_D(const OpticalDisc);
    auto *p = iface_cast<Ifaces::OpticalDisc>(d->backendObject());
    return p ? p->capacity() : 0;
}
