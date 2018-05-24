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

#ifndef SOLID_ADD_INHIBITION_JOB
#define SOLID_ADD_INHIBITION_JOB

#include "power.h"

#include <solid/job.h>

#include "solid_export.h"

namespace Solid
{
class Inhibition;
class InhibitionJobPrivate;
class SOLID_EXPORT InhibitionJob : public Job
{
    Q_OBJECT
public:
    enum Error {
        InvalidInhibitions = Job::UserDefinedError,
        EmptyDescription
    };
    Q_ENUM(Error)

    /**
     * Instantiate InhibitionJob
     *
     * When this job emits result(Solid::Job*) and in case no
     * error has happened, an Inhibition object will be
     * returned using inhibition(). Delete the returned object
     * as soon as the inhibition should be released.
     *
     * At least one action to inhibit and description should be
     * added.
     */
    explicit InhibitionJob(QObject* parent = nullptr);


    /**
     * Sets th list of action to inhibit
     *
     * @param inhibitions list of actions to inhibit
     */
    void setInhibitions(Power::InhibitionTypes inhibitions);

    /**
     * Returns the list of actions this job will inhibit
     *
     * @return List of Power::Imhibitions
     */
    Power::InhibitionTypes inhibitions() const;

    /**
     * Sets the description of this inhibition
     *
     * @param description explains why the inhibition is required
     */
    void setDescription(const QString &description);

    /**
     * Returns the description to be used
     */
    QString description() const;

    /**
     * Returns the inhibition
     *
     * The result of this job is an object called Inhibition
     * which should be kept as long as the inhibition is desired.
     *
     * If this method is called before result(Solid::Job*) is emitted,
     * it will return nullptr.
     */
    Inhibition* inhibition() const;

private Q_SLOTS:
    void doStart() override;

/**
 * We have to re-declare the signal because
 * if not, Q_PROPERTY wouldn't work.
 */
Q_SIGNALS:
    void result(Solid::Job *);

private:
    Q_DECLARE_PRIVATE(InhibitionJob)
};
}
#endif //SOLID_ADD_INHIBITION_JOB
