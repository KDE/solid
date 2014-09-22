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

#ifndef SOLID_STATES_JOB_H
#define SOLID_STATES_JOB_H

#include "solid_export.h"
#include "power.h"

#include <Solid/Job>

namespace Solid
{
class StatesJobPrivate;
/**
 * Returns the states supported on the device
 *
 * Different devices and different operating systems support
 * a different set of states such Sleep or Hibernation.
 *
 */
class SOLID_EXPORT StatesJob : public Job
{
    Q_OBJECT

public:
    explicit StatesJob(QObject *parent = 0);

    /**
     * Once the job is finished, returns the supported Power::States
     */
    Power::States states() const;

private Q_SLOTS:
    virtual void doStart() Q_DECL_OVERRIDE;

/**
 * We have to re-declare the signal because
 * if not, Q_PROPERTY wouldn't work.
 */
Q_SIGNALS:
    void result(Solid::Job *job);

private:
    Q_DECLARE_PRIVATE(StatesJob)
};
} //Solid namespace
#endif //SOLID_STATES_JOB_H
