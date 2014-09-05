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

#ifndef FREEDESKTOP_AC_PLUGGED_JOB_H
#define FREEDESKTOP_AC_PLUGGED_JOB_H

#include "backends/abstractacpluggedjob.h"

class QDBusError;
class QDBusMessage;
class QDBusPendingCallWatcher;
namespace Solid
{
class   FDAcPluggedJob : public AbstractAcPluggedJob
{
    Q_OBJECT
public:
    explicit FDAcPluggedJob(QObject* parent = 0);
    virtual bool isPlugged() const Q_DECL_OVERRIDE;

private Q_SLOTS:
    virtual void doStart() Q_DECL_OVERRIDE;

    void slotDBusReply(const QDBusMessage &msg);
    void slotDBusError(const QDBusError &error);

private:
    void dbusFinished(QDBusPendingCallWatcher* watcher);

    bool m_isPlugged;
};
}

#endif //FREEDESKTOP_AC_PLUGGED_JOB_H
