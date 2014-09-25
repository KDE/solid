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

#include "powerbackendloader.h"
#include "backends/abstractacpluggedjob.h"
#include "backends/dummy/dummyacpluggedjob.h"
#include "backends/dummy/dummypowernotifier.h"
#include "backends/dummy/dummyinhibitionjob.h"
#include "backends/dummy/dummystatesjob.h"
#include "backends/dummy/dummyrequeststatejob.h"
#include "backends/freedesktop/fdacpluggedjob.h"
#include "backends/freedesktop/fdpowernotifier.h"
#include "backends/freedesktop/fdinhibitionjob.h"

using namespace Solid;

AbstractAcPluggedJob* PowerBackendLoader::AcPluggedJob()
{
    if (qgetenv("SOLID_POWER_BACKEND") == "DUMMY") {
        return new DummyAcPluggedJob();
    }
    return new FDAcPluggedJob();
}

AbstractInhibitionJob* PowerBackendLoader::addInhibitionJob(Power::States inhibitions, const QString &description)
{
    if (qgetenv("SOLID_POWER_BACKEND") == "DUMMY") {
        return new DummyInhibitionJob(inhibitions, description);
    }
    return new FdInhibitionJob(inhibitions, description);
}

AbstractStatesJob* PowerBackendLoader::statesJob()
{
    return new DummyStatesJob();
}

AbstractRequestStateJob* PowerBackendLoader::requestState()
{
    return new DummyRequestStateJob();
}

PowerNotifier* PowerBackendLoader::notifier()
{
    if (qgetenv("SOLID_POWER_BACKEND") == "DUMMY") {
        return new DummyPowerNotifier();
    }
    return new FDPowerNotifier();
}