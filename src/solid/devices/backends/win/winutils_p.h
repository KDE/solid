/*
    Copyright 2016 Kevin Funk <kfunk@kde.org>

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

#ifndef WINUTILS_P_H
#define WINUTILS_P_H

#include "windevicemanager.h" // for qGetLastError

#include <qt_windows.h>

/**
 * @brief RAII class for blocking Windows from reporting errors
 *
 * Internally calls GetThreadErrorMode + SetErrorMode. This class stores the
 * current error mode on construction and disables certain error reportings
 * during the life time of this object
 *
 * @code
 * {
 *     WinErrorBlocker blocker; // custom error mode is set
 *     // Your Windows API calls...
 * } // end of scope: original error mode is restored
 * @endcode
 *
 * Note that qstorageinfo_win.cpp (qtbase) does something similar to silence errors
 *
 * See MSDN documentation: https://msdn.microsoft.com/en-us/library/windows/desktop/dd553630(v=vs.85).aspx
 */
class WinErrorBlocker
{
public:
    WinErrorBlocker()
    {
        if (!::SetThreadErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX, &m_oldmode)) {
            qWarning() << "Failed to call SetThreadErrorMode:" << qGetLastError();
        }
    }
    ~WinErrorBlocker()
    {
        if (!::SetThreadErrorMode(m_oldmode, NULL)) {
            qWarning() << "Failed to call SetThreadErrorMode:" << qGetLastError();
        }
    }

private:
    Q_DISABLE_COPY(WinErrorBlocker)
    DWORD m_oldmode;
};

#endif
