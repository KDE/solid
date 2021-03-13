/*
    SPDX-FileCopyrightText: 2013 Ivan Cukic <ivan.cukic(at)kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef SOLID_EXTENSION_PLUGIN_H
#define SOLID_EXTENSION_PLUGIN_H

#include <QQmlExtensionPlugin>

class SolidExtensionPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.solid")

public:
    SolidExtensionPlugin(QObject *parent = nullptr);

    void registerTypes(const char *uri) override;
};

#endif
