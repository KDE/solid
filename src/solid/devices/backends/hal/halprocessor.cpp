/*
    SPDX-FileCopyrightText: 2006 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "halprocessor.h"

#include "haldevice.h"
#include "../shared/cpufeatures.h"

using namespace Solid::Backends::Hal;

Processor::Processor(HalDevice *device)
    : DeviceInterface(device)
{

}

Processor::~Processor()
{

}

int Processor::number() const
{
    return m_device->prop("processor.number").toInt();
}

int Processor::maxSpeed() const
{
    // the property is not mandatory in HAL
    return m_device->prop("processor.maximum_speed").toInt();
}

bool Processor::canChangeFrequency() const
{
    return m_device->prop("processor.can_throttle").toBool();
}

Solid::Processor::InstructionSets Processor::instructionSets() const
{
    static Solid::Processor::InstructionSets cpuextensions = Solid::Backends::Shared::cpuFeatures();

    return cpuextensions;
}

