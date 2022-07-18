/*
    SPDX-FileCopyrightText: 2006-2007 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "processor.h"
#include "processor_p.h"

#include "soliddefs_p.h"
#include <solid/devices/ifaces/processor.h>

Solid::Processor::Processor(QObject *backendObject)
    : DeviceInterface(*new ProcessorPrivate(), backendObject)
{
}

Solid::Processor::~Processor()
{
}

int Solid::Processor::number() const
{
    Q_D(const Processor);
    auto *p = iface_cast<Ifaces::Processor>(d->backendObject());
    return p ? p->number() : 0;
}

int Solid::Processor::maxSpeed() const
{
    Q_D(const Processor);
    auto *p = iface_cast<Ifaces::Processor>(d->backendObject());
    return p ? p->maxSpeed() : 0;
}

bool Solid::Processor::canChangeFrequency() const
{
    Q_D(const Processor);
    auto *p = iface_cast<Ifaces::Processor>(d->backendObject());
    return p ? p->canChangeFrequency() : false;
}

Solid::Processor::InstructionSets Solid::Processor::instructionSets() const
{
    Q_D(const Processor);
    auto *p = iface_cast<Ifaces::Processor>(d->backendObject());
    return p ? p->instructionSets() : InstructionSets();
}
