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

#ifndef SOLID_INHIBITION_H
#define SOLID_INHIBITION_H

#include <QObject>

#include "solid_export.h"

namespace Solid
{

class InhibitionPrivate;
class AbstractInhibition;
/**
 * Holds an inhibition
 *
 * This object is returned by Power::InhibitionJob::inhibition and it
 * holds a reference to the inhibition that has been performed.
 *
 * When this object is deleted the inhibition will be released
 */
class SOLID_EXPORT Inhibition : public QObject
{
    Q_OBJECT
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
public:
    enum State {
        Stopped = 0,
        Started = 1
    };
    Q_ENUM(State)

    /**
     * This is meant to be instantiated by backends only
     *
     * AbstractInhibition is not part of Solid public API so this
     * constructor is meant to be used only by backends.
     */
    explicit Inhibition(AbstractInhibition *backend, QObject *parent=0);
    virtual ~Inhibition();

    /**
     * Returns the current state of the object
     *
     * The initial value is Started since that is how InhibitionJob will
     * return it. The state can be modified by calling stop() and start().
     * Also stateChanged() signal is available.
     */
    State state() const;

public Q_SLOTS:
    /**
     * Stops the inhibition
     *
     * In case the state() is Started, it will stop the inhibition.
     * This happens asynchronously so connect to stateChanged() signal to know
     * when stop() has changed the state.
     */
    void stop();

    /**
     * Starts the inhibition
     *
     * In case state() is Stopped, it will resume the inhibition.
     * This happens asynchronously so connect to stateChanged() signal to
     * know when start() has changed the state.
     */
    void start();

protected:
    InhibitionPrivate *const d_ptr;

Q_SIGNALS:
    void stateChanged(Inhibition::State newState);
};
}
Q_DECLARE_METATYPE(Solid::Inhibition::State)
#endif //SOLID_INHIBITION_H
