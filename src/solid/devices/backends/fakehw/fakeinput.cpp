/*
    SPDX-FileCopyrightText: 2022 Aleix Pol i Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "fakeinput.h"
#include <QVariant>

using namespace Solid::Backends::Fake;

FakeInput::FakeInput(FakeDevice *device)
    : FakeDeviceInterface(device)
{
}

FakeInput::~FakeInput()
{
}

QString FakeInput::device() const
{
    return fakeDevice()->property("device").toString();
}
