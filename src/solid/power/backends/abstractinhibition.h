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

#ifndef ABSTRACT_INHIBITION_H
#define ABSTRACT_INHIBITION_H

#include <QObject>

#include <solid/power/inhibition.h>

namespace Solid
{
/**
 * Represents an inhibition, allows to stop and start it
 *
 * When implementing this class take into account that the
 * inhibition should be stopped upon object deletion, so
 * stateChanged should be emitted on it
 */
class AbstractInhibition : public QObject
{
    Q_OBJECT
public:
    explicit AbstractInhibition(QObject *parent = 0) : QObject(parent) {}
    virtual ~AbstractInhibition(){}

    virtual void start() = 0;
    virtual void stop() = 0;
    virtual Inhibition::State state() const = 0;

Q_SIGNALS:
    void stateChanged(Inhibition::State state);
};
} //Solid nanmespace
#endif //ABSTRACT_INHIBITION_H