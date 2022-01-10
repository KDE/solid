/*
    SPDX-FileCopyrightText: 2022 Aleix Pol i Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_INPUT_P_H
#define SOLID_INPUT_P_H

#include "deviceinterface_p.h"

namespace Solid
{
class InputPrivate : public DeviceInterfacePrivate
{
public:
    InputPrivate()
        : DeviceInterfacePrivate()
    {
    }
};
}

#endif
