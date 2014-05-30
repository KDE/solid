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

#include "statesjob.h"
#include "statesjob_p.h"
#include "powerbackendloader.h"
#include "backends/abstractstatesjob.h"

#include <QDebug>

using namespace Solid;

StatesJobPrivate::StatesJobPrivate()
{
    backendJob = Q_NULLPTR;
}

StatesJob::StatesJob(QObject* parent) : Job(*new StatesJobPrivate(), parent)
{
}

void StatesJob::doStart()
{
    Q_D(StatesJob);
    d->backendJob = PowerBackendLoader::statesJob();
    connect(d->backendJob, &AbstractStatesJob::result, [this]() {
        emitResult();
    });

    d->backendJob->start();
}

Power::States StatesJob::states() const
{
    if(d_func()->backendJob) {
        return d_func()->backendJob->states();
    }
    qWarning() << "statesJob called without having called start";
    return Power::None;
}
