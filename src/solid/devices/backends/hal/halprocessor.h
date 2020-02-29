/*
    SPDX-FileCopyrightText: 2006 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_BACKENDS_HAL_PROCESSOR_H
#define SOLID_BACKENDS_HAL_PROCESSOR_H

#include <solid/devices/ifaces/processor.h>
#include "haldeviceinterface.h"

namespace Solid
{
namespace Backends
{
namespace Hal
{
class HalDevice;

class Processor : public DeviceInterface, virtual public Solid::Ifaces::Processor
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::Processor)

public:
    Processor(HalDevice *device);
    virtual ~Processor();

    int number() const override;
    int maxSpeed() const override;
    bool canChangeFrequency() const override;
    Solid::Processor::InstructionSets instructionSets() const override;
};
}
}
}

#endif // SOLID_BACKENDS_HAL_PROCESSOR_H
