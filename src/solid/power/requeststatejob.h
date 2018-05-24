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

#ifndef SOLID_REQUEST_STATE_JOB_H
#define SOLID_REQUEST_STATE_JOB_H

#include <Solid/Job>

#include "solid_export.h"
#include "power.h"

namespace Solid
{
class RequestStateJobPrivate;
/**
 * Tries to set the device under the state indicated via setState()
 *
 * On success this job will contain no error and the device will
 * be set to the desired state (for example Sleep). On error
 * use error() and errorText() to know more about the issue.
 *
 * @note In some devices changing the system state might happen
 * too fast for the callee of this job to be notified (for example
 * the system might suspend before result() is emitted.
 */
class SOLID_EXPORT RequestStateJob : public Job
{
    Q_OBJECT

public:
    enum Error {
        Unsupported = Job::UserDefinedError
    };

    explicit RequestStateJob(QObject *parent = nullptr);

    void setState(Power::InhibitionType state);

private Q_SLOTS:
    void doStart() override;

/**
 * We have to re-declare the signal because
 * if not, Q_PROPERTY wouldn't work.
 */
Q_SIGNALS:
    void result(Solid::Job *);

private:
    Q_DECLARE_PRIVATE(RequestStateJob)
};
} //Solid namespace
#endif //SOLID_REQUEST_STATE_JOB_H
