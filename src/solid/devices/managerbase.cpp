/*
    Copyright 2006-2007 Kevin Ottens <ervin@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#include "managerbase_p.h"

#include <stdlib.h>

#include <config-backends.h>

// do *not* use other defines than BUILD_DEVICE_BACKEND_$backend to include
// the managers, and keep an alphabetical order
#ifdef BUILD_DEVICE_BACKEND_fakehw
#include "backends/fakehw/fakemanager.h"
#endif
#ifdef BUILD_DEVICE_BACKEND_fstab
#include "backends/fstab/fstabmanager.h"
#endif
#ifdef BUILD_DEVICE_BACKEND_hal
#include "backends/hal/halmanager.h"
#endif
#ifdef BUILD_DEVICE_BACKEND_iokit
#include "backends/iokit/iokitmanager.h"
#endif
#ifdef BUILD_DEVICE_BACKEND_udev
#include "backends/udev/udevmanager.h"
#endif
#ifdef BUILD_DEVICE_BACKEND_udisks2
#include "backends/udisks2/udisksmanager.h"
#endif
#ifdef BUILD_DEVICE_BACKEND_upower
#include "backends/upower/upowermanager.h"
#endif
#ifdef BUILD_DEVICE_BACKEND_win
#include "backends/win/windevicemanager.h"
#endif

Solid::ManagerBasePrivate::ManagerBasePrivate()
{
}

Solid::ManagerBasePrivate::~ManagerBasePrivate()
{
    qDeleteAll(m_backends);
}

// do *not* use other defines than BUILD_DEVICE_BACKEND_$backend to add
// the managers, and keep an alphabetical order
void Solid::ManagerBasePrivate::loadBackends()
{
    QString solidFakeXml(QString::fromLocal8Bit(qgetenv("SOLID_FAKEHW")));

    if (!solidFakeXml.isEmpty()) {
#ifdef BUILD_DEVICE_BACKEND_fakehw
        m_backends << new Solid::Backends::Fake::FakeManager(nullptr, solidFakeXml);
#endif
    } else {
#ifdef BUILD_DEVICE_BACKEND_fstab
        m_backends << new Solid::Backends::Fstab::FstabManager(nullptr);
#endif
#ifdef BUILD_DEVICE_BACKEND_hal
        m_backends << new Solid::Backends::Hal::HalManager(nullptr);
#endif
#ifdef BUILD_DEVICE_BACKEND_iokit
        m_backends << new Solid::Backends::IOKit::IOKitManager(nullptr);
#endif
#ifdef BUILD_DEVICE_BACKEND_udev
        m_backends << new Solid::Backends::UDev::UDevManager(nullptr);
#endif
#ifdef BUILD_DEVICE_BACKEND_udisks2
        m_backends << new Solid::Backends::UDisks2::Manager(nullptr);
#endif
#ifdef BUILD_DEVICE_BACKEND_upower
        m_backends << new Solid::Backends::UPower::UPowerManager(nullptr);
#endif
#ifdef BUILD_DEVICE_BACKEND_win
        m_backends << new Solid::Backends::Win::WinDeviceManager(nullptr);
#endif
    }
}

QList<QObject *> Solid::ManagerBasePrivate::managerBackends() const
{
    return m_backends;
}

