/*
    SPDX-FileCopyrightText: 2022 Aleix Pol i Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "udevinput.h"

using namespace Solid::Backends::UDev;

Input::Input(UDevDevice *device)
    : DeviceInterface(device)
{
}

Input::~Input()
{
}

QString Input::device() const
{
    return m_device->property("DEVNAME").toString();
}
