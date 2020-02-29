/*
    SPDX-FileCopyrightText: 2006 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "haldeviceinterface.h"

using namespace Solid::Backends::Hal;

DeviceInterface::DeviceInterface(HalDevice *device)
    : QObject(device), m_device(device)
{
}

DeviceInterface::~DeviceInterface()
{
}

