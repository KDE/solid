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

#include "solid-power.h"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QTextStream>

static QTextStream sOut(stdout);
int main(int argc, char **argv)
{
    qputenv("SOLID_POWER_BACKEND", "FREE_DESKTOP");

    QCoreApplication app(argc, argv);
    app.setApplicationName(QLatin1String("solid-power"));

    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::translate("solid-power", "Tool to know and set the power management state of your device"));
    parser.addHelpOption();
    parser.addPositionalArgument("command", QCoreApplication::translate("solid-power", "Command to execute"));

    QCommandLineOption commands("commands", QCoreApplication::translate("solid-power", "Show available commands"));
    parser.addOption(commands);

    parser.process(app);

    if (parser.isSet(commands))
    {
        sOut << endl << QCoreApplication::translate("solid-hardware", "Syntax:") << endl << endl;

        sOut << "  solid-power show" << endl;
        sOut << QCoreApplication::translate("solid-power",
                "             # Show all the power management information from the system.\n"
                "             # - acPlugged: whether the device is connected to the AC or not\n") << endl;
        return 1;
    }

    SolidPower power;

    QStringList args = parser.positionalArguments();
    if (args.count() < 1) {
        parser.showHelp(1);
        return 1;
    }

    parser.clearPositionalArguments();

    QString command(args.at(0));

    if (command == QLatin1String("show")) {
        power.show();
    } else if(command == QLatin1String("listen")) {
        sOut << "Listening to events:" << endl;
        power.listen();
        app.exec();
    } else {
        sOut << "Not recognized command" << endl;
    }
}
