/*
    SPDX-FileCopyrightText: 2006 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_BACKENDS_HAL_BLOCK_H
#define SOLID_BACKENDS_HAL_BLOCK_H

#include <solid/devices/ifaces/block.h>
#include "haldeviceinterface.h"

namespace Solid
{
namespace Backends
{
namespace Hal
{
class Block : public DeviceInterface, virtual public Solid::Ifaces::Block
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::Block)

public:
    Block(HalDevice *device);
    virtual ~Block();

    int deviceMajor() const override;
    int deviceMinor() const override;
    QString device() const override;
};
}
}
}

#endif // SOLID_BACKENDS_HAL_BLOCK_H
