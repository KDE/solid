/*
    Copyright 2017 Konstantinos Tsanaktsidis <kjtsanaktsidis@gmail.com>

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

#include "fakeUdisks2.h"

FakeUdisks2BlockDevice::FakeUdisks2BlockDevice(QObject *parent, const QString &device, quint64 device_number)
    : QDBusAbstractAdaptor(parent), m_device(device), m_device_number(device_number)
{

}

const QString &FakeUdisks2BlockDevice::device()
{
    return m_device;
}

quint64 FakeUdisks2BlockDevice::device_number()
{
    return m_device_number;
}
