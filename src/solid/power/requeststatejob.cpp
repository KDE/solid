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

#include "requeststatejob.h"
#include "requeststatejob_p.h"
#include "powerbackendloader.h"
#include "backends/abstractrequeststatejob.h"


using namespace Solid;

RequestStateJobPrivate::RequestStateJobPrivate()
{
    backendJob = nullptr;
    state = Power::None;
}

RequestStateJob::RequestStateJob(QObject* parent) : Job(*new RequestStateJobPrivate(), parent)
{
}

void RequestStateJob::setState(Power::InhibitionType state)
{
    Q_D(RequestStateJob);
    d->state = state;
}

void RequestStateJob::doStart()
{
    Q_D(RequestStateJob);
    d->backendJob = PowerBackendLoader::requestState();
    d->backendJob->state = d->state;

    connect(d->backendJob, &AbstractRequestStateJob::result, [this](Job *job) {
        if (job->error()) {
            setError(job->error());
            setErrorText(job->errorText());
        }
        emitResult();
    });

    d->backendJob->start();
}
