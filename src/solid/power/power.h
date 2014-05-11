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

#ifndef SOLID_POWER_H
#define SOLID_POWER_H

#include <QObject>
namespace Solid
{
class AcPluggedJob;
class Power : public QObject
{
    Q_OBJECT
public:
    /**
     * Returns an instance of Power
     *
     * Having an instance of Power is useful to monitor any changes
     * in the power management system by connecting to its signals, like
     * acPluggedChanged().
     *
     * If you are interested on Power during the entire live cycle of your
     * application, then you want to use this singlethon. If instead you are
     * only intested for a short period of time, consider instancing your own
     * Solid::Power so you can free the memory at any point.
     */
    static Power* self();
    /**
     * Returns an AcPluggedJob
     *
     * The returned AcPluggedJob has to be started, when finished
     * the Job::result() signal will be emitted.
     */
    static AcPluggedJob* isAcPlugged(QObject *parent = 0);


    /**
     * If you are not going to destroy this object for the entire
     * application live cycle, you might want to use self() singlethon.
     *
     * The only reason to instance your own Power object is for when an
     * application is only interested in power management during a small
     * period of time and you want to free the memory after using it.
     */
    explicit Power(QObject* parent = 0);
Q_SIGNALS:
    void acPluggedChanged(bool plugged);
private:

    class Private;
    Private *d;
};
}
#endif //SOLID_POWER_H