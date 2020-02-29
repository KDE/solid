/*
    SPDX-FileCopyrightText: 2006 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "halblock.h"

#include "haldevice.h"

using namespace Solid::Backends::Hal;

Block::Block(HalDevice *device)
    : DeviceInterface(device)
{

}

Block::~Block()
{

}

int Block::deviceMajor() const
{
    return m_device->prop("block.major").toInt();
}

int Block::deviceMinor() const
{
    return m_device->prop("block.minor").toInt();
}

QString Block::device() const
{
    return m_device->prop("block.device").toString();
}

