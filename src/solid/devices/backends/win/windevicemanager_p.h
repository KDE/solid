/*
    Copyright 2014 Patrick von Reth <vonreth@kde.org>

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
#ifndef WINDEVICEMANAGER_P_H
#define WINDEVICEMANAGER_P_H


namespace Solid
{
namespace Backends
{
namespace Win
{

class SolidWinEventFilter : public QObject
{
    Q_OBJECT
public:

    SolidWinEventFilter();
    ~SolidWinEventFilter();
    void promoteAddedDevice(const QSet<QString> &udi);
    void promoteRemovedDevice(const QSet<QString> &udi);
    void promotePowerChange();

    static SolidWinEventFilter *instance();


    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);




Q_SIGNALS:
    void powerChanged();
    void deviceAdded(const QSet<QString> &udi);
    void deviceRemoved(const QSet<QString> &udi);

private:
    HWND m_windowID;

};
}
}
}

#endif // WINDEVICEMANAGER_P_H
