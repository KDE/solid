/*
    SPDX-FileCopyrightText: 2006-2007 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "block.h"
#include "block_p.h"

#include "soliddefs_p.h"
#include <solid/devices/ifaces/block.h>

Solid::Block::Block(QObject *backendObject)
    : DeviceInterface(*new BlockPrivate(), backendObject)
{
}

Solid::Block::~Block()
{
}

int Solid::Block::deviceMajor() const
{
    Q_D(const Block);
    auto *p = iface_cast<Ifaces::Block>(d->backendObject());
    return p ? p->deviceMajor() : 0;
}

int Solid::Block::deviceMinor() const
{
    Q_D(const Block);
    auto *p = iface_cast<Ifaces::Block>(d->backendObject());
    return p ? p->deviceMinor() : 0;
}

QString Solid::Block::device() const
{
    Q_D(const Block);
    auto *p = iface_cast<Ifaces::Block>(d->backendObject());
    return p ? p->device() : QString();
}
