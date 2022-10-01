/*  This file is part of the KDE project
    SPDX-FileCopyrightText: 2006 Kevin Ottens <ervin@kde.org>
    SPDX-FileCopyrightText: 2014 Alejandro Fiestas Olivares <afiestas@kde.org>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "solid-hardware.h"

#if defined QT_DBUS_LIB
#include <QDBusArgument>
#include <QDBusObjectPath>
#endif
#include <QMetaEnum>
#include <QMetaProperty>
#include <QString>
#include <QStringList>
#include <QTextStream>

#include <QCommandLineParser>

#include <solid/device.h>
#include <solid/genericinterface.h>
#include <solid/opticaldrive.h>

#include <iostream>
#include <solid/devicenotifier.h>
using namespace std;

static const char appName[] = "solid-hardware";

static const char version[] = "0.1a";

std::ostream &operator<<(std::ostream &out, const QString &msg)
{
    return (out << msg.toLocal8Bit().constData());
}

std::ostream &operator<<(std::ostream &out, const QVariant &value);
#if defined QT_DBUS_LIB
std::ostream &operator<<(std::ostream &out, const QDBusArgument &arg)
{
    auto type = arg.currentType();
    switch (type) {
    case QDBusArgument::ArrayType:
        out << " { ";
        arg.beginArray();
        while (!arg.atEnd()) {
            out << arg;
            if (!arg.atEnd()) {
                out << ", ";
            }
        }
        arg.endArray();
        out << " }";
        break;
    case QDBusArgument::StructureType:
        out << " ( ";
        arg.beginStructure();
        while (!arg.atEnd()) {
            out << arg.asVariant();
        }
        arg.endStructure();
        out << " )";
        break;
    case QDBusArgument::MapType:
        out << " [ ";
        arg.beginMap();
        if (!arg.atEnd()) {
            out << arg;
        }
        arg.endMap();
        out << " ]";
        break;
    case QDBusArgument::MapEntryType:
        arg.beginMapEntry();
        out << arg.asVariant() << " = " << arg;
        arg.endMapEntry();
        break;
    case QDBusArgument::UnknownType:
        out << "(unknown DBus type)";
        break;
    case QDBusArgument::BasicType:
    case QDBusArgument::VariantType:
        out << arg.asVariant();
    }
    return out;
}
#endif

std::ostream &operator<<(std::ostream &out, const QVariant &value)
{
    switch (value.type()) {
    case QVariant::StringList: {
        out << "{";

        const QStringList list = value.toStringList();

        QStringList::ConstIterator it = list.constBegin();
        QStringList::ConstIterator end = list.constEnd();

        for (; it != end; ++it) {
            out << "'" << *it << "'";

            if (it + 1 != end) {
                out << ", ";
            }
        }

        out << "} (string list)";
        break;
    }
    case QVariant::String:
        out << "'" << value.toString() << "' (string)";
        break;
    case QVariant::Bool:
        out << (value.toBool() ? "true" : "false") << " (bool)";
        break;
    case QVariant::Int:
    case QVariant::LongLong:
        out << value.toString() << "  (0x" << QString::number(value.toLongLong(), 16) << ")  (" << QVariant::typeToName(value.type()) << ")";
        break;
    case QVariant::UInt:
    case QVariant::ULongLong:
        out << value.toString() << "  (0x" << QString::number(value.toULongLong(), 16) << ")  (" << QVariant::typeToName(value.type()) << ")";
        break;
    case QVariant::Double:
        out << value.toString() << " (double)";
        break;
    case QVariant::ByteArray:
        out << "'" << value.toString() << "' (bytes)";
        break;
    case QVariant::UserType:
        // qDebug() << "got variant type:" << value.typeName();
        if (value.canConvert<QList<int>>()) {
            const QList<int> intlist = value.value<QList<int>>();
            QStringList tmp;
            for (const int val : intlist) {
                tmp.append(QString::number(val));
            }
            out << "{" << tmp.join(",") << "} (int list)";
#if defined QT_DBUS_LIB
        } else if (value.canConvert<QDBusObjectPath>()) {
            out << value.value<QDBusObjectPath>().path() << " (ObjectPath)";
        } else if (value.canConvert<QDBusVariant>()) {
            out << value.value<QDBusVariant>().variant() << "(Variant)";
        } else if (value.canConvert<QDBusArgument>()) {
            out << value.value<QDBusArgument>();
#endif
        } else {
            out << value.toString() << " (unhandled)";
        }

        break;
    default:
        out << "'" << value.toString() << "' (" << QVariant::typeToName(value.type()) << ")";
        break;
    }

    return out;
}

std::ostream &operator<<(std::ostream &out, const Solid::Device &device)
{
    out << "  parent = " << QVariant(device.parentUdi()) << endl;
    out << "  vendor = " << QVariant(device.vendor()) << endl;
    out << "  product = " << QVariant(device.product()) << endl;
    out << "  description = " << QVariant(device.description()) << endl;
    out << "  icon = " << QVariant(device.icon()) << endl;

    int index = Solid::DeviceInterface::staticMetaObject.indexOfEnumerator("Type");
    QMetaEnum typeEnum = Solid::DeviceInterface::staticMetaObject.enumerator(index);

    for (int i = 0; i < typeEnum.keyCount(); i++) {
        Solid::DeviceInterface::Type type = (Solid::DeviceInterface::Type)typeEnum.value(i);
        const Solid::DeviceInterface *interface = device.asDeviceInterface(type);

        if (interface) {
            const QMetaObject *meta = interface->metaObject();

            for (int i = meta->propertyOffset(); i < meta->propertyCount(); i++) {
                QMetaProperty property = meta->property(i);
                out << "  " << QString(meta->className()).mid(7) << "." << property.name() << " = ";

                QVariant value = property.read(interface);

                if (property.isEnumType()) {
                    QMetaEnum metaEnum = property.enumerator();
                    if (metaEnum.isFlag()) {
                        out << "'" << metaEnum.valueToKeys(value.toInt()).constData() << "'"
                            << "  (0x" << QString::number(value.toInt(), 16) << ")  (flag)";
                    } else {
                        out << "'" << metaEnum.valueToKey(value.toInt()) << "'"
                            << "  (0x" << QString::number(value.toInt(), 16) << ")  (enum)";
                    }
                    out << endl;
                } else {
                    out << value << endl;
                }
            }
        }
    }

    return out;
}

std::ostream &operator<<(std::ostream &out, const QMap<QString, QVariant> &properties)
{
    for (auto it = properties.cbegin(); it != properties.cend(); ++it) {
        out << "  " << it.key() << " = " << it.value() << endl;
    }

    return out;
}

QString getUdiFromArguments(QCoreApplication &app, QCommandLineParser &parser)
{
    parser.addPositionalArgument("udi", QCoreApplication::translate("solid-hardware", "Device udi"));
    parser.process(app);
    if (parser.positionalArguments().count() < 2) {
        parser.showHelp(1);
    }
    return parser.positionalArguments().at(1);
}

static QString commandsHelp()
{
    QString data;
    QTextStream cout(&data);
    cout << '\n' << QCoreApplication::translate("solid-hardware", "Syntax:") << '\n' << '\n';

    cout << "  solid-hardware list [details|nonportableinfo]" << '\n';
    cout << QCoreApplication::translate("solid-hardware",
                                        "             # List the hardware available in the system.\n"
                                        "             # - If the 'nonportableinfo' option is specified, the device\n"
                                        "             # properties are listed (be careful, in this case property names\n"
                                        "             # are backend dependent),\n"
                                        "             # - If the 'details' option is specified, the device interfaces\n"
                                        "             # and the corresponding properties are listed in a platform\n"
                                        "             # neutral fashion,\n"
                                        "             # - Otherwise only device UDIs are listed.\n")
         << '\n';

    cout << "  solid-hardware details 'udi'" << '\n';
    cout << QCoreApplication::translate("solid-hardware",
                                        "             # Display all the interfaces and properties of the device\n"
                                        "             # corresponding to 'udi' in a platform neutral fashion.\n")
         << '\n';

    cout << "  solid-hardware nonportableinfo 'udi'" << '\n';
    cout << QCoreApplication::translate("solid-hardware",
                                        "             # Display all the properties of the device corresponding to 'udi'\n"
                                        "             # (be careful, in this case property names are backend dependent).\n")
         << '\n';

    cout << "  solid-hardware query 'predicate' ['parentUdi']" << '\n';
    cout << QCoreApplication::translate("solid-hardware",
                                        "             # List the UDI of devices corresponding to 'predicate'.\n"
                                        "             # - If 'parentUdi' is specified, the search is restricted to the\n"
                                        "             # branch of the corresponding device,\n"
                                        "             # - Otherwise the search is done on all the devices.\n")
         << '\n';

    cout << "  solid-hardware mount 'udi'" << '\n';
    cout << QCoreApplication::translate("solid-hardware", "             # If applicable, mount the device corresponding to 'udi'.\n") << '\n';

    cout << "  solid-hardware unmount 'udi'" << '\n';
    cout << QCoreApplication::translate("solid-hardware", "             # If applicable, unmount the device corresponding to 'udi'.\n") << '\n';

    cout << "  solid-hardware eject 'udi'" << '\n';
    cout << QCoreApplication::translate("solid-hardware", "             # If applicable, eject the device corresponding to 'udi'.\n") << '\n';

    cout << "  solid-hardware listen" << '\n';
    cout << QCoreApplication::translate("solid-hardware", "             # Listen to all add/remove events on supported hardware.") << '\n';

    return data;
}

int main(int argc, char **argv)
{
    SolidHardware app(argc, argv);
    app.setApplicationName(appName);
    app.setApplicationVersion(version);

    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::translate("solid-hardware", "KDE tool for querying your hardware from the command line"));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("command", QCoreApplication::translate("solid-hardware", "Command to execute"), commandsHelp());

    QCommandLineOption commands("commands", QCoreApplication::translate("solid-hardware", "Show available commands"));
    // --commands only for backwards compat, it's now in the "syntax help"
    // of the positional argument.
    commands.setFlags(QCommandLineOption::HiddenFromHelp);
    parser.addOption(commands);

    parser.process(app);
    if (parser.isSet(commands)) {
        cout << commandsHelp() << endl;
        return 0;
    }

    QStringList args = parser.positionalArguments();
    if (args.count() < 1) {
        parser.showHelp(1);
    }

    parser.clearPositionalArguments();

    QString command(args.at(0));

    if (command == "list") {
        parser.addPositionalArgument("details", QCoreApplication::translate("solid-hardware", "Show device details"));
        parser.addPositionalArgument("nonportableinfo", QCoreApplication::translate("solid-hardware", "Show non portable information"));
        parser.process(app);
        args = parser.positionalArguments();
        QByteArray extra(args.count() == 2 ? args.at(1).toLocal8Bit() : QByteArray());
        return app.hwList(extra == "details", extra == "nonportableinfo");
    } else if (command == "details") {
        const QString udi = getUdiFromArguments(app, parser);
        return app.hwCapabilities(udi);
    } else if (command == "nonportableinfo") {
        const QString udi = getUdiFromArguments(app, parser);
        return app.hwProperties(udi);
    } else if (command == "query") {
        parser.addPositionalArgument("udi", QCoreApplication::translate("solid-hardware", "Device udi"));
        parser.addPositionalArgument("parent", QCoreApplication::translate("solid-hardware", "Parent device udi"));
        parser.process(app);
        if (parser.positionalArguments().count() < 2 || parser.positionalArguments().count() > 3) {
            parser.showHelp(1);
        }

        QString query = args.at(1);
        QString parent;

        if (args.count() == 3) {
            parent = args.at(2);
        }

        return app.hwQuery(parent, query);
    } else if (command == "mount") {
        const QString udi = getUdiFromArguments(app, parser);
        return app.hwVolumeCall(SolidHardware::Mount, udi);
    } else if (command == "unmount") {
        const QString udi = getUdiFromArguments(app, parser);
        return app.hwVolumeCall(SolidHardware::Unmount, udi);
    } else if (command == "eject") {
        const QString udi = getUdiFromArguments(app, parser);
        return app.hwVolumeCall(SolidHardware::Eject, udi);
    } else if (command == "listen") {
        return app.listen();
    }

    cerr << QCoreApplication::translate("solid-hardware", "Syntax Error: Unknown command '%1'").arg(command) << endl;

    return 1;
}

bool SolidHardware::hwList(bool interfaces, bool system)
{
    const QList<Solid::Device> all = Solid::Device::allDevices();

    for (const Solid::Device &device : all) {
        cout << "udi = '" << device.udi() << "'" << endl;

        if (interfaces) {
            cout << device << endl;
        } else if (system && device.is<Solid::GenericInterface>()) {
            QMap<QString, QVariant> properties = device.as<Solid::GenericInterface>()->allProperties();
            cout << properties << endl;
        }
    }

    return true;
}

bool SolidHardware::hwCapabilities(const QString &udi)
{
    const Solid::Device device(udi);

    cout << "udi = '" << device.udi() << "'" << endl;
    cout << device << endl;

    return true;
}

bool SolidHardware::hwProperties(const QString &udi)
{
    const Solid::Device device(udi);

    cout << "udi = '" << device.udi() << "'" << endl;
    if (device.is<Solid::GenericInterface>()) {
        QMap<QString, QVariant> properties = device.as<Solid::GenericInterface>()->allProperties();
        cout << properties << endl;
    }

    return true;
}

bool SolidHardware::hwQuery(const QString &parentUdi, const QString &query)
{
    const QList<Solid::Device> devices = Solid::Device::listFromQuery(query, parentUdi);

    for (const Solid::Device &device : devices) {
        cout << "udi = '" << device.udi() << "'" << endl;
    }

    return true;
}

bool SolidHardware::hwVolumeCall(SolidHardware::VolumeCallType type, const QString &udi)
{
    Solid::Device device(udi);

    if (!device.is<Solid::StorageAccess>() && type != Eject) {
        cerr << tr("Error: %1 does not have the interface StorageAccess.").arg(udi) << endl;
        return false;
    } else if (!device.is<Solid::OpticalDrive>() && type == Eject) {
        cerr << tr("Error: %1 does not have the interface OpticalDrive.").arg(udi) << endl;
        return false;
    }

    switch (type) {
    case Mount:
        connect(device.as<Solid::StorageAccess>(),
                SIGNAL(setupDone(Solid::ErrorType, QVariant, QString)),
                this,
                SLOT(slotStorageResult(Solid::ErrorType, QVariant)));
        device.as<Solid::StorageAccess>()->setup();
        break;
    case Unmount:
        connect(device.as<Solid::StorageAccess>(),
                SIGNAL(teardownDone(Solid::ErrorType, QVariant, QString)),
                this,
                SLOT(slotStorageResult(Solid::ErrorType, QVariant)));
        device.as<Solid::StorageAccess>()->teardown();
        break;
    case Eject:
        connect(device.as<Solid::OpticalDrive>(),
                SIGNAL(ejectDone(Solid::ErrorType, QVariant, QString)),
                this,
                SLOT(slotStorageResult(Solid::ErrorType, QVariant)));
        device.as<Solid::OpticalDrive>()->eject();
        break;
    }

    m_loop.exec();

    if (m_error) {
        cerr << tr("Error: %1").arg(m_errorString) << endl;
        return false;
    }

    return true;
}

bool SolidHardware::listen()
{
    Solid::DeviceNotifier *notifier = Solid::DeviceNotifier::instance();
    bool a = connect(notifier, SIGNAL(deviceAdded(QString)), this, SLOT(deviceAdded(QString)));
    bool d = connect(notifier, SIGNAL(deviceRemoved(QString)), this, SLOT(deviceRemoved(QString)));

    if (!a || !d) {
        return false;
    }

    cout << "Listening to add/remove events: " << endl;
    m_loop.exec();
    return true;
}

void SolidHardware::deviceAdded(const QString &udi)
{
    cout << "Device Added:" << endl;
    cout << "udi = '" << udi << "'" << endl;
}

void SolidHardware::deviceRemoved(const QString &udi)
{
    cout << "Device Removed:" << endl;
    cout << "udi = '" << udi << "'" << endl;
}

void SolidHardware::slotStorageResult(Solid::ErrorType error, const QVariant &errorData)
{
    if (error) {
        m_error = 1;
        m_errorString = errorData.toString();
    }
    m_loop.exit();
}
