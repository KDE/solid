/*
    SPDX-FileCopyrightText: 2006 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "block.h"

Solid::Ifaces::Block::~Block()
{
}

bool Solid::Ifaces::Block::isSystem() const
{
    return false;
}
