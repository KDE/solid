/*
    SPDX-FileCopyrightText: 2006-2007 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#ifndef SOLID_SOLIDDEFS_P_H
#define SOLID_SOLIDDEFS_P_H

#include <QObject>

template<typename T, typename Object>
T *iface_cast(Object *obj)
{
    return qobject_cast<T *>(obj);
}

#endif
