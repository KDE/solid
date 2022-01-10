/*
    SPDX-FileCopyrightText: 2022 Aleix Pol i Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_BACKENDS_FAKEHW_FAKEINPUT_H
#define SOLID_BACKENDS_FAKEHW_FAKEINPUT_H

#include "fakedeviceinterface.h"
#include <solid/devices/ifaces/input.h>

namespace Solid
{
namespace Backends
{
namespace Fake
{
class FakeInput : public FakeDeviceInterface, virtual public Solid::Ifaces::Input
{
    Q_OBJECT
    Q_INTERFACES(Solid::Ifaces::Input)

public:
    explicit FakeInput(FakeDevice *device);
    ~FakeInput() override;

    QString device() const override;
};
}
}
}

#endif // SOLID_BACKENDS_FAKEHW_FAKEINPUT_H
