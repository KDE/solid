/*
 *   Copyright (C) 2013 Ivan Cukic <ivan.cukic(at)kde.org>
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SOLID_DECLARATIVE_EXTENSION_PLUGIN_H
#define SOLID_DECLARATIVE_EXTENSION_PLUGIN_H

#include <QtQml/QQmlExtensionPlugin>

class SolidExtensionPlugin: public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.solid")

public:
    SolidExtensionPlugin(QObject *parent = Q_NULLPTR);

    void registerTypes(const char *uri) Q_DECL_OVERRIDE;

};

#endif

