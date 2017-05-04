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

#ifndef SOLID_FAKE_UDISKS2
#define SOLID_FAKE_UDISKS2

#include <QString>
#include <QDBusAbstractAdaptor>
#include <QDBusObjectPath>

class FakeUdisks2BlockDevice : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.UDisks2.Block")
    Q_PROPERTY(QString Device READ device)
    Q_PROPERTY(quint64 DeviceNumber READ device_number)
public:
    FakeUdisks2BlockDevice(QObject *parent, const QString &device, quint64 device_number);

    const QString &device();
    QString m_device;
    quint64 device_number();
    quint64 m_device_number;
};

#endif // SOLID_FAKE_UDISKS2
