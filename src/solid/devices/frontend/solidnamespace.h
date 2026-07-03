/*
    SPDX-FileCopyrightText: 2007 Kevin Ottens <ervin@kde.org>
    SPDX-FileCopyrightText: 2011 Lukas Tinkl <ltinkl@redhat.com>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_SOLIDNAMESPACE_H
#define SOLID_SOLIDNAMESPACE_H

// All KF6 public headers should provide the version macros.
// Include version header explicitly because the export header
// (which usually takes care of that) isn't used here
#include <solid_version.h> // IWYU pragma: export

/*!
 * \namespace Solid
 * \inheaderfile Solid/SolidNamespace
 * \inmodule Solid
 */
namespace Solid
{

/*!
 * \value NoError
 * \value UnauthorizedOperation
 * \value DeviceBusy
 * \value OperationFailed
 * \value UserCanceled
 * \value InvalidOption
 * \value MissingDriver
 */
enum ErrorType {
    NoError = 0,
    UnauthorizedOperation,
    DeviceBusy,
    OperationFailed,
    UserCanceled,
    InvalidOption,
    MissingDriver,
};
}

#include <QMetaType>

Q_DECLARE_METATYPE(Solid::ErrorType)

#endif
