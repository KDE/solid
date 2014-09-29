/*
    Copyright 2014 Alejandro Fiestas Olivares <afiestas@kde.org>

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

#ifndef SOLID_FAKE_LOGIND_H
#define SOLID_FAKE_LOGIND_H

#include <QList>
#include <QString>
#include <QDBusUnixFileDescriptor>
#include <QDBusObjectPath>

class FakeLogind : public QObject
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.freedesktop.login1.Manager")
public:
    explicit FakeLogind(QObject* parent);

public Q_SLOTS:
    QDBusUnixFileDescriptor Inhibit(const QString &what, const QString &who, const QString &why, const QString &mode);

    void checkFd();

Q_SIGNALS:
    void inhibitionRemoved();
    void newInhibition(const QString &what, const QString &who, const QString &why, const QString &mode);

private:
    int m_fd;
};

#endif //SOLID_FAKE_LOGIND_H