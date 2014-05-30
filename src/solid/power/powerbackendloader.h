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

#ifndef POWER_BACKEND_LOADER_H
#define POWER_BACKEND_LOADER_H

#include <solid/power.h>

namespace Solid
{
class AbstractAcPluggedJob;
class AbstractInhibitionJob;
class AbstractStatesJob;
class AbstractRequestStateJob;
class PowerNotifier;
class PowerBackendLoader
{
public:
    static AbstractAcPluggedJob* AcPluggedJob();
    static AbstractInhibitionJob* addInhibitionJob(Power::States inhibitions, const QString &description);
    static AbstractStatesJob* statesJob();
    static AbstractRequestStateJob* requestState();
    static PowerNotifier* notifier();
};
};

#endif //POWER_BACKEND_LOADER_H