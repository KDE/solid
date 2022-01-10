/*
    SPDX-FileCopyrightText: 2022 Aleix Pol i Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_BACKENDS_UDEV_INPUT_H
#define SOLID_BACKENDS_UDEV_INPUT_H

#include "udevdeviceinterface.h"
#include <solid/devices/ifaces/input.h>

namespace Solid
{
namespace Backends
{
namespace UDev
{
class Input : public DeviceInterface, virtual public Solid::Ifaces::Input
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::Input)

public:
    Input(UDevDevice *device);
    ~Input() override;

    QString device() const override;
};
}
}
}

#endif
