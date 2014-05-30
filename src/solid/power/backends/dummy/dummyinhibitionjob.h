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

#ifndef SOLID_DUMMY_ADD_INHIBITION_JOB_H
#define SOLID_DUMMY_ADD_INHIBITION_JOB_H

#include "backends/abstractinhibitionjob.h"
#include "solid/power.h"

namespace Solid
{
class Inhibition;
class DummyInhibitionJob : public AbstractInhibitionJob
{
    Q_OBJECT
public:
    explicit DummyInhibitionJob(Power::States inhibitions, const QString &description, QObject* parent = 0);

    virtual Inhibition* inhibition() Q_DECL_OVERRIDE;
private Q_SLOTS:
    virtual void doStart() Q_DECL_OVERRIDE;

private:
    Power::States m_inhibitions;
    QString m_description;
};
}
#endif //SOLID_DUMMY_ADD_INHIBITION_JOB_H