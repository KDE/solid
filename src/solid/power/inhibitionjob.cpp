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

#include "inhibitionjob.h"
#include "inhibitionjob_p.h"
#include "powerbackendloader.h"
#include "backends/abstractinhibitionjob.h"

#include "inhibition.h"

#include <QDebug>
#include <qglobal.h>

using namespace Solid;

InhibitionJobPrivate::InhibitionJobPrivate()
    : inhibitions(Power::None)
    , inhibition(Q_NULLPTR)
    , backendJob(Q_NULLPTR)
{

}

InhibitionJob::InhibitionJob(QObject* parent) : Job(*new InhibitionJobPrivate(), parent)
{

}

Inhibition* InhibitionJob::inhibition() const
{
    Q_ASSERT_X(d_func()->inhibition, "addInhibitionJob", "::inhibition() called before result() has been emitted");

    if(d_func()->inhibition) {
        return d_func()->inhibition;
    }

    qWarning() << "result() has not been emitted yet, job not finished";
    return Q_NULLPTR;
}

void InhibitionJob::doStart()
{
    Q_D(InhibitionJob);

    if (!d->inhibitions) {
        setError(InvalidInhibitions);
        emitResult();
        return;
    }
    if (d->description.isEmpty()) {
        setError(EmptyDescription);
        emitResult();
        return;
    }

    d->backendJob = PowerBackendLoader::addInhibitionJob(d->inhibitions, d->description);
    connect(d->backendJob, &AbstractInhibitionJob::result, [this, d]() {
        d_func()->inhibition = d->backendJob->inhibition();
        emitResult();
    });

    d->backendJob->start();
}

void InhibitionJob::setInhibitions(Power::States inhibitions)
{
    Q_D(InhibitionJob);
    d->inhibitions = inhibitions;
}

Power::States InhibitionJob::inhibitions() const
{
    return d_func()->inhibitions;
}

void InhibitionJob::setDescription(const QString& description)
{
    Q_D(InhibitionJob);
    d->description = description;
}

QString InhibitionJob::description() const
{
    return d_func()->description;
}
