/*  This file is part of the KDE project
    Copyright (C) 2006 Kevin Ottens <ervin@kde.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License version 2 as published by the Free Software Foundation.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#include "solid-hardware.h"


#include <QString>
#include <QStringList>
#include <QMetaProperty>
#include <QMetaEnum>
#include <QTimer>

#include <kcomponentdata.h>
#include <kcmdlineargs.h>
#include <kdebug.h>
#include <kjob.h>
#include <klocale.h>
#include <k3socketaddress.h>

#include <solid/device.h>
#include <solid/genericinterface.h>
#include <solid/storageaccess.h>
#include <solid/opticaldrive.h>

#include <iostream>
using namespace std;

static const char appName[] = "solid-hardware";
static const char programName[] = I18N_NOOP("solid-hardware");

static const char description[] = I18N_NOOP("KDE tool for querying your hardware from the command line");

static const char version[] = "0.1a";

std::ostream &operator<<(std::ostream &out, const QString &msg)
{
    return (out << msg.toLocal8Bit().constData());
}

std::ostream &operator<<(std::ostream &out, const QVariant &value)
{
    switch (value.type())
    {
    case QVariant::StringList:
    {
        out << "{";

        QStringList list = value.toStringList();

        QStringList::ConstIterator it = list.begin();
        QStringList::ConstIterator end = list.end();

        for (; it!=end; ++it)
        {
            out << "'" << *it << "'";

            if (it+1!=end)
            {
                out << ", ";
            }
        }

        out << "}  (string list)";
        break;
    }
    case QVariant::Bool:
        out << (value.toBool()?"true":"false") << "  (bool)";
        break;
    case QVariant::Int:
        out << value.toString()
            << "  (0x" << QString::number(value.toInt(), 16) << ")  (int)";
        break;
    default:
        out << "'" << value.toString() << "'  (string)";
        break;
    }

    return out;
}

std::ostream &operator<<(std::ostream &out, const Solid::Device &device)
{
    out << "  parent = " << QVariant(device.parentUdi()) << endl;
    out << "  vendor = " << QVariant(device.vendor()) << endl;
    out << "  product = " << QVariant(device.product()) << endl;

    int index = Solid::DeviceInterface::staticMetaObject.indexOfEnumerator("Type");
    QMetaEnum typeEnum = Solid::DeviceInterface::staticMetaObject.enumerator(index);

    for (int i=0; i<typeEnum.keyCount(); i++)
    {
        Solid::DeviceInterface::Type type = (Solid::DeviceInterface::Type)typeEnum.value(i);
        const Solid::DeviceInterface *interface = device.asDeviceInterface(type);

        if (interface)
        {
            const QMetaObject *meta = interface->metaObject();

            for (int i=meta->propertyOffset(); i<meta->propertyCount(); i++)
            {
                QMetaProperty property = meta->property(i);
                out << "  " << QString(meta->className()).mid(7) << "." << property.name()
                    << " = ";

                QVariant value = property.read(interface);

                if (property.isEnumType()) {
                    QMetaEnum metaEnum = property.enumerator();
                    out << "'" << metaEnum.valueToKeys(value.toInt()).constData() << "'"
                        << "  (0x" << QString::number(value.toInt(), 16) << ")  ";
                    if (metaEnum.isFlag()) {
                        out << "(flag)";
                    } else {
                        out << "(enum)";
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

std::ostream &operator<<(std::ostream &out, const QMap<QString,QVariant> &properties)
{
    foreach (QString key, properties.keys())
    {
        out << "  " << key << " = " << properties[key] << endl;
    }

    return out;
}

void checkArgumentCount(int min, int max)
{
    int count = KCmdLineArgs::parsedArgs()->count();

    if (count < min)
    {
        cerr << i18n("Syntax Error: Not enough arguments") << endl;
        ::exit(1);
    }

    if ((max > 0) && (count > max))
    {
        cerr << i18n("Syntax Error: Too many arguments") << endl;
        ::exit(1);
    }
}

int main(int argc, char **argv)
{
  KCmdLineArgs::init(argc, argv, appName, 0, ki18n(programName), version, ki18n(description), false);


  KCmdLineOptions options;

  options.add("commands", ki18n("Show available commands by domains"));

  options.add("+command", ki18n("Command (see --commands)"));

  options.add("+[arg(s)]", ki18n("Arguments for command"));

  KCmdLineArgs::addCmdLineOptions(options);

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

  KComponentData componentData(appName);

  if (args->isSet("commands"))
  {
      KCmdLineArgs::enable_i18n();

      cout << endl << i18n("Syntax:") << endl << endl;

      cout << "  solid-hardware list [details|nonportableinfo]" << endl;
      cout << i18n("             # List the hardware available in the system.\n"
                    "             # - If the 'nonportableinfo' option is specified, the device\n"
                    "             # properties are listed (be careful, in this case property names\n"
                    "             # are backend dependent),\n"
                    "             # - If the 'details' option is specified, the device interfaces\n"
                    "             # and the corresponding properties are listed in a platform\n"
                    "             # neutral fashion,\n"
                    "             # - Otherwise only device UDIs are listed.\n") << endl;

      cout << "  solid-hardware details 'udi'" << endl;
      cout << i18n("             # Display all the interfaces and properties of the device\n"
                    "             # corresponding to 'udi' in a platform neutral fashion.\n") << endl;

      cout << "  solid-hardware nonportableinfo 'udi'" << endl;
      cout << i18n("             # Display all the properties of the device corresponding to 'udi'\n"
                    "             # (be careful, in this case property names are backend dependent).\n") << endl;

      cout << "  solid-hardware query 'predicate' ['parentUdi']" << endl;
      cout << i18n("             # List the UDI of devices corresponding to 'predicate'.\n"
                    "             # - If 'parentUdi' is specified, the search is restricted to the\n"
                    "             # branch of the corresponding device,\n"
                    "             # - Otherwise the search is done on all the devices.\n") << endl;

      cout << "  solid-hardware mount 'udi'" << endl;
      cout << i18n("             # If applicable, mount the device corresponding to 'udi'.\n") << endl;

      cout << "  solid-hardware unmount 'udi'" << endl;
      cout << i18n("             # If applicable, unmount the device corresponding to 'udi'.\n") << endl;

      cout << "  solid-hardware hardware eject 'udi'" << endl;
      cout << i18n("             # If applicable, eject the device corresponding to 'udi'.\n") << endl;

      return 0;
  }

  return SolidShell::doIt() ? 0 : 1;
}

bool SolidShell::doIt()
{
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    checkArgumentCount(1, 0);

    QString command(args->arg(0));

    int fake_argc = 0;
    char **fake_argv = 0;
    SolidShell shell(fake_argc, fake_argv);

    if (command == "list")
    {
        checkArgumentCount(1, 2);
        QByteArray extra(args->count()==2 ? args->arg(1).toLocal8Bit() : "");
        return shell.hwList(extra=="details", extra=="nonportableinfo");
    }
    else if (command == "details")
    {
        checkArgumentCount(2, 2);
        QString udi(args->arg(1));
        return shell.hwCapabilities(udi);
    }
    else if (command == "nonportableinfo")
    {
        checkArgumentCount(2, 2);
        QString udi(args->arg(1));
        return shell.hwProperties(udi);
    }
    else if (command == "query")
    {
        checkArgumentCount(2, 3);

        QString query = args->arg(1);
        QString parent;

        if (args->count() == 3)
        {
            parent = args->arg(2);
        }

        return shell.hwQuery(parent, query);
    }
    else if (command == "mount")
    {
        checkArgumentCount(2, 2);
        QString udi(args->arg(1));
        return shell.hwVolumeCall(Mount, udi);
    }
    else if (command == "unmount")
    {
        checkArgumentCount(2, 2);
        QString udi(args->arg(1));
        return shell.hwVolumeCall(Unmount, udi);
    }
    else if (command == "eject")
    {
        checkArgumentCount(2, 2);
        QString udi(args->arg(1));
        return shell.hwVolumeCall(Eject, udi);
    }
    else
    {
        cerr << i18n("Syntax Error: Unknown command '%1'" ,command) << endl;
    }

    return false;
}

bool SolidShell::hwList(bool interfaces, bool system)
{
    const QList<Solid::Device> all = Solid::Device::allDevices();

    foreach (const Solid::Device device, all)
    {
        cout << "udi = '" << device.udi() << "'" << endl;

        if (interfaces)
        {
            cout << device << endl;
        }
        else if (system && device.is<Solid::GenericInterface>())
        {
            QMap<QString,QVariant> properties = device.as<Solid::GenericInterface>()->allProperties();
            cout << properties << endl;
        }
    }

    return true;
}

bool SolidShell::hwCapabilities(const QString &udi)
{
    const Solid::Device device(udi);

    cout << "udi = '" << device.udi() << "'" << endl;
    cout << device << endl;

    return true;
}

bool SolidShell::hwProperties(const QString &udi)
{
    const Solid::Device device(udi);

    cout << "udi = '" << device.udi() << "'" << endl;
    if (device.is<Solid::GenericInterface>()) {
        QMap<QString,QVariant> properties = device.as<Solid::GenericInterface>()->allProperties();
        cout << properties << endl;
    }

    return true;
}

bool SolidShell::hwQuery(const QString &parentUdi, const QString &query)
{
    const QList<Solid::Device> devices
        = Solid::Device::listFromQuery(query, parentUdi);

    foreach (const Solid::Device device, devices)
    {
        cout << "udi = '" << device.udi() << "'" << endl;
    }

    return true;
}

bool SolidShell::hwVolumeCall(SolidShell::VolumeCallType type, const QString &udi)
{
    Solid::Device device(udi);

    if (!device.is<Solid::StorageAccess>() && type!=Eject)
    {
        cerr << i18n("Error: %1 does not have the interface StorageAccess." , udi) << endl;
        return false;
    }
    else if (!device.is<Solid::OpticalDrive>() && type==Eject)
    {
        cerr << i18n("Error: %1 does not have the interface OpticalDrive." , udi) << endl;
        return false;
    }

    switch(type)
    {
    case Mount:
        connect(device.as<Solid::StorageAccess>(),
                SIGNAL(setupDone(Solid::ErrorType, QVariant, const QString &)),
                this,
                SLOT(slotStorageResult(Solid::ErrorType, QVariant)));
        device.as<Solid::StorageAccess>()->setup();
        break;
    case Unmount:
        connect(device.as<Solid::StorageAccess>(),
                SIGNAL(teardownDone(Solid::ErrorType, QVariant, const QString &)),
                this,
                SLOT(slotStorageResult(Solid::ErrorType, QVariant)));
        device.as<Solid::StorageAccess>()->teardown();
        break;
    case Eject:
        connect(device.as<Solid::OpticalDrive>(),
                SIGNAL(ejectDone(Solid::ErrorType, QVariant, const QString &)),
                this,
                SLOT(slotStorageResult(Solid::ErrorType, QVariant)));
        device.as<Solid::OpticalDrive>()->eject();
        break;
    }

    m_loop.exec();

    if (m_error)
    {
        cerr << i18n("Error: %1" , m_errorString) << endl;
        return false;
    }

    return true;
}

void SolidShell::connectJob(KJob *job)
{
    connect(job, SIGNAL(result(KJob *)),
             this, SLOT(slotResult(KJob *)));
    connect(job, SIGNAL(percent(KJob *, unsigned long)),
             this, SLOT(slotPercent(KJob *, unsigned long)));
    connect(job, SIGNAL(infoMessage(KJob *, const QString &, const QString &)),
             this, SLOT(slotInfoMessage(KJob *, const QString &)));
}

void SolidShell::slotPercent(KJob */*job */, unsigned long percent)
{
    cout << i18n("Progress: %1%" , percent) << endl;
}

void SolidShell::slotInfoMessage(KJob */*job */, const QString &message)
{
    cout << i18n("Info: %1" , message) << endl;
}

void SolidShell::slotResult(KJob *job)
{
    m_error = 0;

    if (job->error())
    {
        m_error = job->error();
        m_errorString = job->errorString();
    }

    m_loop.exit();
}

void SolidShell::slotStorageResult(Solid::ErrorType error, const QVariant &errorData)
{
    if (error) {
        m_error = 1;
        m_errorString = errorData.toString();
    }
    m_loop.exit();
}

#include "solid-hardware.moc"
