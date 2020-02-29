/*
    SPDX-FileCopyrightText: 2006 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_BACKENDS_HAL_FSTABHANDLING_H
#define SOLID_BACKENDS_HAL_FSTABHANDLING_H

#include <QString>

class QProcess;
class QObject;

namespace Solid
{
namespace Backends
{
namespace Hal
{
class FstabHandling
{
public:
    static bool isInFstab(const QString &device);
    static QStringList possibleMountPoints(const QString &device);
    static QProcess *callSystemCommand(const QString &commandName,
                                       const QStringList &args,
                                       QObject *obj, const char *slot);
    static QProcess *callSystemCommand(const QString &commandName,
                                       const QString &device,
                                       QObject *obj, const char *slot);
};
}
}
}

#endif

