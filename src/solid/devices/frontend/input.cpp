/*
    SPDX-FileCopyrightText: 2022 Aleix Pol i Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "input.h"
#include "input_p.h"

#include "soliddefs_p.h"
#include <solid/devices/ifaces/input.h>

Solid::Input::Input(QObject *backendObject)
    : DeviceInterface(*new InputPrivate(), backendObject)
{
}

Solid::Input::~Input()
{
}

QString Solid::Input::device() const
{
    Q_D(const Input);
    return_SOLID_CALL(Ifaces::Input *, d->backendObject(), QString(), device());
}
