/*
 * SPDX-FileCopyrightText: 2026 Kai Uwe Broulik <kde@broulik.de>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "kdeconnectdeviceinterface.h"

using namespace Solid::Backends::KdeConnect;

DeviceInterface::DeviceInterface(Device *device)
    : QObject(device)
    , m_device(device)
{
}

DeviceInterface::~DeviceInterface() = default;

#include "moc_kdeconnectdeviceinterface.cpp"
