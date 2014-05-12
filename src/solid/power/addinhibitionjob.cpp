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

#include "addinhibitionjob.h"
#include "addinhibitionjob_p.h"
#include "powerbackendloader.h"
#include "backends/abstractaddinhibitionjob.h"

#include "inhibitionholder.h"

#include <QDebug>

using namespace Solid;

AddInhibitionJobPrivate::AddInhibitionJobPrivate()
    : inhibitions(Power::None)
    , backendJob(Q_NULLPTR)
{

}

AddInhibitionJob::AddInhibitionJob(QObject* parent) : Job(*new AddInhibitionJobPrivate(), parent)
{

}

InhibitionHolder* AddInhibitionJob::inhibition() const
{
    Q_ASSERT_X(!d_func()->backendJob, "addInhibitionJob", "::inhibition() called before result() has been emitted");

    if(d_func()->backendJob) {
        return d_func()->backendJob->inhibition();
    }
    qWarning() << "result() has not been emitted yet, job not finished";
    return Q_NULLPTR;
}

void AddInhibitionJob::doStart()
{
    Q_D(AddInhibitionJob);

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

    d->backendJob = PowerBackendLoader::addInhibitionJob();
    connect(d->backendJob, &AbstractAddInhibitionJob::result, [this]() {
        emitResult();
    });

    d->backendJob->start();
}

void AddInhibitionJob::setInhibitions(Power::Inhibitions inhibitions)
{
    Q_D(AddInhibitionJob);
    d->inhibitions = inhibitions;
}

Power::Inhibitions AddInhibitionJob::inhibitions() const
{
    return d_func()->inhibitions;
}

void AddInhibitionJob::setDescription(const QString& description)
{
    Q_D(AddInhibitionJob);
    d->description = description;
}

QString AddInhibitionJob::description() const
{
    return d_func()->description;
}
