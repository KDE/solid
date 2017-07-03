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


#ifndef SOLID_FD_INHIBITION_H
#define SOLID_FD_INHIBITION_H

#include "backends/abstractinhibition.h"
#include "inhibition.h"
#include "solid/power.h"

#include <qglobal.h>
#include <QDBusMessage>

namespace Solid
{
class FdInhibition : public AbstractInhibition
{
    Q_OBJECT
public:
    explicit FdInhibition(Power::InhibitionTypes inhibitions, const QString &description, QObject* parent = nullptr);
    virtual ~FdInhibition();

    void start() Q_DECL_OVERRIDE;
    void stop() Q_DECL_OVERRIDE;
    Inhibition::State state() const Q_DECL_OVERRIDE;

    void setDescription(const QString &description);

private Q_SLOTS:
    void slotDBusReply(const QDBusMessage &msg);
    void slotDBusError(const QDBusError &error);

private:
    void setState(const Inhibition::State &state);

    int m_fd;
    Inhibition::State m_state;
    QString m_description;
    Power::InhibitionTypes m_inhibitions;
};
}

#endif //SOLID_FD_INHIBITION_H