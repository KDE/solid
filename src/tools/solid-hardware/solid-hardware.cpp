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
#include <klocale.h>
#include <k3socketaddress.h>
#include <kdebug.h>

#include <solid/device.h>
#include <solid/genericinterface.h>
#include <solid/storageaccess.h>
#include <solid/opticaldrive.h>

#include <solid/control/powermanager.h>

#include <solid/control/ifaces/authentication.h>
#include <solid/control/networkmanager.h>
#include <solid/control/networkinterface.h>
#include <solid/control/network.h>
#include <solid/control/wirelessnetwork.h>

#include <solid/control/bluetoothmanager.h>
#include <solid/control/bluetoothinterface.h>
#include <solid/control/bluetoothremotedevice.h>
#include <solid/control/bluetoothinputdevice.h>

#include <kjob.h>


#include <iostream>
using namespace std;

static const char appName[] = "solidshell";
static const char programName[] = I18N_NOOP("solidshell");

static const char description[] = I18N_NOOP("KDE tool for querying and controlling your hardware from the command line");

static const char version[] = "0.1";

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

std::ostream &operator<<(std::ostream &out, const Solid::Control::NetworkInterface &networkdevice)
{
    out << "  UNI =                " << QVariant(networkdevice.uni()) << endl;
    out << "  Type =               " << (networkdevice.type() == Solid::Control::NetworkInterface::Ieee8023 ? "Wired" : "802.11 Wireless") << endl;
    out << "  Active =             " << (networkdevice.isActive() ? "Yes" : "No") << endl;
    //out << "  HW Address =         " << networkdevice.  // TODO add to solid API.
    out << "\n  Capabilities:" << endl;
    out << "    Supported =        " << (networkdevice.capabilities()  & Solid::Control::NetworkInterface::IsManageable ? "Yes" : "No") << endl;
    out << "    Speed =            " << networkdevice.designSpeed() << endl;
    if (networkdevice.type() == Solid::Control::NetworkInterface::Ieee8023)
        out << "    Carrier Detect =   " << (networkdevice.capabilities()  & Solid::Control::NetworkInterface::SupportsCarrierDetect ? "Yes" : "No") << endl;
    else
        out << "    Wireless Scan =    " << (networkdevice.capabilities()  & Solid::Control::NetworkInterface::SupportsWirelessScan ? "Yes" : "No") << endl;
    out << "    Link Up =          " << (networkdevice.isLinkUp() ? "Yes" : "No") << endl;

    return out;
}

std::ostream &operator<<(std::ostream &out, const Solid::Control::Network &network)
{
    out << "  UNI =                " << QVariant(network.uni()) << endl;
    out << "  Addresses:" << endl;
    foreach (QNetworkAddressEntry addr, network.addressEntries())
    {
        out << "    (" << addr.ip().toString() << "," << addr.broadcast().toString() << "," << addr.ip().toString() << ")" << endl;
    }
    if (network.addressEntries().isEmpty())
        out << "    none" << endl;
    out << "  Route:               " << QVariant(network.route()) <<  endl;
    out << "  DNS Servers:" << endl;
    int i = 1;
    foreach (QHostAddress addr, network.dnsServers())
    {
        out << "  " << i++ << ": " << addr.toString() << endl;
    }
    if (network.dnsServers().isEmpty())
        out << "    none" << endl;
    out << "  Active =             " << (network.isActive() ? "Yes" : "No") << endl;

    return out;
}

std::ostream &operator<<(std::ostream &out, const Solid::Control::WirelessNetwork &network)
{
    out << "  ESSID =                " << QVariant(network.essid()) << endl;
    out << "  Mode =                 ";
    switch (network.mode())
    {
    case Solid::Control::WirelessNetwork::Unassociated:
        cout << "Unassociated" << endl;
        break;
    case Solid::Control::WirelessNetwork::Adhoc:
        cout << "Ad-hoc" << endl;
        break;
    case Solid::Control::WirelessNetwork::Managed:
        cout << "Infrastructure" << endl;
        break;
    case Solid::Control::WirelessNetwork::Master:
        cout << "Master" << endl;
        break;
    case Solid::Control::WirelessNetwork::Repeater:
        cout << "Repeater" << endl;
        break;
    default:
        cout << "Unknown" << endl;
        cerr << "Unknown network operation mode: " << network.mode() << endl;
        break;
    }
    out << "  Frequency =            " << network.frequency() << endl;
    out << "  Rate =                 " << network.bitrate() << endl;
    out << "  Strength =             " << network.signalStrength() << endl;
    if (network.isEncrypted())
    {
        out << "  Encrypted =            Yes (";
        Solid::Control::WirelessNetwork::Capabilities cap = network.capabilities();
        if (cap  & Solid::Control::WirelessNetwork::Wep)
            out << "WEP,";
        if (cap  & Solid::Control::WirelessNetwork::Wpa)
            out << "WPA,";
        if (cap  & Solid::Control::WirelessNetwork::Wpa2)
            out << "WPA2,";
        if (cap  & Solid::Control::WirelessNetwork::Psk)
            out << "PSK,";
        if (cap  & Solid::Control::WirelessNetwork::Ieee8021x)
            out << "Ieee8021x,";
        if (cap  & Solid::Control::WirelessNetwork::Wep40)
            out << "WEP40,";
        if (cap  & Solid::Control::WirelessNetwork::Wep104)
            out << "WEP104,";
        if (cap  & Solid::Control::WirelessNetwork::Wep192)
            out << "WEP192,";
        if (cap  & Solid::Control::WirelessNetwork::Wep256)
            out << "WEP256,";
        if (cap  & Solid::Control::WirelessNetwork::WepOther)
            out << "WEP-Other,";
        if (cap  & Solid::Control::WirelessNetwork::Tkip)
            out << "TKIP";
        if (cap  & Solid::Control::WirelessNetwork::Ccmp)
            out << "CCMP";
        out << ")" << endl;
    }
    else
        out << "  Encrypted =            No" << endl;

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

  options.add("+domain", ki18n("Domain (see --commands)"));

  options.add("+command", ki18n("Command (see --commands)"));

  options.add("+[arg(s)]", ki18n("Arguments for command"));

  KCmdLineArgs::addCmdLineOptions(options);

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

  KComponentData componentData(appName);

  if (args->isSet("commands"))
  {
      KCmdLineArgs::enable_i18n();

      cout << endl << i18n("Syntax:") << endl << endl;

      cout << "  solidshell hardware list [details|nonportableinfo]" << endl;
      cout << i18n("             # List the hardware available in the system.\n"
                    "             # - If the 'nonportableinfo' option is specified, the device\n"
                    "             # properties are listed (be careful, in this case property names\n"
                    "             # are backend dependent),\n"
                    "             # - If the 'details' option is specified, the device interfaces\n"
                    "             # and the corresponding properties are listed in a platform\n"
                    "             # neutral fashion,\n"
                    "             # - Otherwise only device UDIs are listed.\n") << endl;

      cout << "  solidshell hardware details 'udi'" << endl;
      cout << i18n("             # Display all the interfaces and properties of the device\n"
                    "             # corresponding to 'udi' in a platform neutral fashion.\n") << endl;

      cout << "  solidshell hardware nonportableinfo 'udi'" << endl;
      cout << i18n("             # Display all the properties of the device corresponding to 'udi'\n"
                    "             # (be careful, in this case property names are backend dependent).\n") << endl;

      cout << "  solidshell hardware query 'predicate' ['parentUdi']" << endl;
      cout << i18n("             # List the UDI of devices corresponding to 'predicate'.\n"
                    "             # - If 'parentUdi' is specified, the search is restricted to the\n"
                    "             # branch of the corresponding device,\n"
                    "             # - Otherwise the search is done on all the devices.\n") << endl;

      cout << "  solidshell hardware mount 'udi'" << endl;
      cout << i18n("             # If applicable, mount the device corresponding to 'udi'.\n") << endl;

      cout << "  solidshell hardware unmount 'udi'" << endl;
      cout << i18n("             # If applicable, unmount the device corresponding to 'udi'.\n") << endl;

      cout << "  solidshell hardware eject 'udi'" << endl;
      cout << i18n("             # If applicable, eject the device corresponding to 'udi'.\n") << endl;


      cout << endl;


      cout << "  solidshell power query (suspend|scheme|cpufreq)" << endl;
      cout << i18n("             # List a particular set of information regarding power management.\n"
                    "             # - If the 'suspend' option is specified, give the list of suspend\n"
                    "             # method supported by the system\n"
                    "             # - If the 'scheme' option is specified, give the list of\n"
                    "             # supported power management schemes by this system\n"
                    "             # - If the 'cpufreq' option is specified, give the list of\n"
                    "             # supported CPU frequency policy\n") << endl;

      cout << "  solidshell power set (scheme|cpufreq) 'value'" << endl;
      cout << i18n("             # Set power management options of the system.\n"
                    "             # - If the 'scheme' option is specified, the power management\n"
                    "             # scheme set corresponds to 'value'\n"
                    "             # - If the 'cpufreq' option is specified, the CPU frequency policy\n"
                    "             # set corresponds to 'value'\n") << endl;

      cout << "  solidshell power suspend 'method'" << endl;
      cout << i18n("             # Suspend the computer using the given 'method'.\n") << endl;

      cout << endl;

      cout << "  solidshell network listdevices" << endl;
      cout << i18n("             # List the network devices present.\n") << endl;

      cout << "  solidshell network listnetworks 'uni'" << endl;
      cout << i18n("             # List the networks known to the device specified by 'uni'.\n") << endl;

      cout << "  solidshell network query (status|wireless)|(interface 'uni')|(network 'device-uni' 'network-uni')" << endl;
      cout << i18n("             # Query whether networking features are active or not.\n"
                    "             # - If the 'status' option is given, return whether\n"
                    "             # networking is enabled for the system\n"
                    "             # - If the 'wireless' option is is given, return whether\n"
                    "             # wireless is enabled for the system\n"
                    "             # - If the 'interface' option is given, print the\n"
                    "             # properties of the network interface that 'uni' refers to.\n"
                    "             # - If the 'network' option is given, print the\n"
                    "             # properties of the network on 'device-uni' that 'network-uni' refers to.\n") << endl;

      cout << "  solidshell network set wireless (enabled|disabled)" << endl;
      cout << i18n("             # Enable or disable networking on this system.\n") << endl;

      cout << "  solidshell network set networking (enabled|disabled)" << endl;
      cout << i18n("             # Enable or disable networking on this system.\n") << endl;

      cout << "  solidshell network set network 'device-uni' 'network-uni' [authentication 'key']" << endl;
      cout << i18n("             # Activate the network 'network-uni' on 'device-uni'.\n"
                    "             # Optionally, use WEP128, open-system encryption with hex key 'key'. (Hardcoded)\n"
                    "             # Where 'authentication' is one of:\n"
                    "             # wep hex64|ascii64|hex128|ascii128|passphrase64|passphrase128 'key' [open|shared]\n"
                    "             # wpapsk wpa|wpa2 tkip|ccmp-aes password\n"
                    "             # wpaeap UNIMPLEMENTED IN SOLIDSHELL\n") << endl;

      cout << endl;

      cout << "  solidshell bluetooth listadapters" << endl;
      cout << i18n("             # List bluetooth adapters/interfaces\n") << endl;

      cout << "  solidshell bluetooth defaultadapter" << endl;
      cout << i18n("             # List bluetooth default adapter/interface\n") << endl;

      cout << "  solidshell bluetooth query (address|bondings|connections|name) (interface 'ubi')" << endl;
      cout << i18n("             # Query information about the bluetooth adapter/interface with 'ubi'\n") << endl;

      cout << "  solidshell bluetooth set (mode|name) (interface 'ubi') 'value'" << endl;
      cout << i18n("             # Set the bluetooth adapter name.\n"
                    "             # Set the bluetooth adapter mode. Where 'value' is one of:\n"
                    "             # off|connectable|discoverable\n") << endl;

      cout << "  solidshell bluetooth scan (interface 'ubi')" << endl;
      cout << i18n("             # Scan for bluetooth remote devices.\n") << endl;

      cout << "  solidshell bluetooth input listdevices" << endl;
      cout << i18n("             # List configured input devices.\n") << endl;

      cout << "  solidshell bluetooth input (setup|remove|connect|disconnect) (device 'ubi')" << endl;
      cout << i18n("             # Setup bluetooth input device.\n"
                    "             # Remove configuration of remote input device.\n"
                    "             # Connect or disconnect bluetooth input device.\n") << endl;

      cout << "  solidshell bluetooth remote (createbonding|removebonding|hasbonding) (device 'ubi')" << endl;
      cout << i18n("             # Create bonding (pairing) with bluetooth remote device.\n"
                    "             # Remove bonding of bluetooth remote device.\n"
                    "             # Check for bonding of bluetooth remote device.\n") << endl;

      return 0;
  }

  return SolidShell::doIt() ? 0 : 1;
}

bool SolidShell::doIt()
{
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    checkArgumentCount(2, 0);

    QString domain(args->arg(0));
    QString command(args->arg(1));

    int fake_argc = 0;
    char **fake_argv = 0;
    SolidShell shell(fake_argc, fake_argv);

    if (domain == "hardware")
    {
        if (command == "list")
        {
            checkArgumentCount(2, 3);
            QByteArray extra(args->count()==3 ? args->arg(2).toLocal8Bit() : "");
            return shell.hwList(extra=="details", extra=="nonportableinfo");
        }
        else if (command == "details")
        {
            checkArgumentCount(3, 3);
            QString udi(args->arg(2));
            return shell.hwCapabilities(udi);
        }
        else if (command == "nonportableinfo")
        {
            checkArgumentCount(3, 3);
            QString udi(args->arg(2));
            return shell.hwProperties(udi);
        }
        else if (command == "query")
        {
            checkArgumentCount(3, 4);

            QString query = args->arg(2);
            QString parent;

            if (args->count() == 4)
            {
                parent = args->arg(3);
            }

            return shell.hwQuery(parent, query);
        }
        else if (command == "mount")
        {
            checkArgumentCount(3, 3);
            QString udi(args->arg(2));
            return shell.hwVolumeCall(Mount, udi);
        }
        else if (command == "unmount")
        {
            checkArgumentCount(3, 3);
            QString udi(args->arg(2));
            return shell.hwVolumeCall(Unmount, udi);
        }
        else if (command == "eject")
        {
            checkArgumentCount(3, 3);
            QString udi(args->arg(2));
            return shell.hwVolumeCall(Eject, udi);
        }
        else
        {
            cerr << i18n("Syntax Error: Unknown command '%1'" ,command) << endl;
        }
    }
    else if (domain == "power")
    {
        if (command == "suspend")
        {
            checkArgumentCount(3, 3);
            QString method(args->arg(2));

            return shell.powerSuspend(method);
        }
        else if (command == "query")
        {
            checkArgumentCount(3, 3);
            QString type(args->arg(2));

            if (type == "suspend")
            {
                return shell.powerQuerySuspendMethods();
            }
            else if (type == "scheme")
            {
                return shell.powerQuerySchemes();
            }
            else if (type == "cpufreq")
            {
                return shell.powerQueryCpuPolicies();
            }
            else
            {
                cerr << i18n("Syntax Error: Unknown option '%1'" , type) << endl;
            }
        }
        else if (command == "set")
        {
            checkArgumentCount(4, 4);
            QString type(args->arg(2));
            QString value(args->arg(3));

            if (type == "scheme")
            {
                return shell.powerChangeScheme(value);
            }
            else if (type == "cpufreq")
            {
                return shell.powerChangeCpuPolicy(value);
            }
            else
            {
                cerr << i18n("Syntax Error: Unknown option '%1'" , type) << endl;
            }
        }
        else
        {
            cerr << i18n("Syntax Error: Unknown command '%1'" , command) << endl;
        }
    }
    else if (domain == "network")
    {
        if (command == "query")
        {
            checkArgumentCount(3, 5);
            QString what(args->arg(2));
            if (what == "status")
                return shell.netmgrNetworkingEnabled();
            else if (what == "wireless")
                return shell.netmgrWirelessEnabled();
            else if (what == "interface")
            {
                checkArgumentCount(4, 4);
                QString uni(args->arg(3));
                return shell.netmgrQueryNetworkInterface(uni);
            }
            else if (what == "network")
            {
                checkArgumentCount(5, 5);
                QString dev(args->arg(3));
                QString uni(args->arg(4));
                return shell.netmgrQueryNetwork(dev, uni);
            }
            else
                cerr << i18n("Syntax Error: Unknown option '%1'", what) << endl;
        }
        else if (command == "set")
        {
            checkArgumentCount(4, 10);
            QString what(args->arg(2));
            QString how(args->arg(3));
            if (what == "networking")
            {
                bool enabled;
                if (how == "enabled")
                {
                    enabled = true;
                }
                else if (how == "disabled")
                {
                    enabled = false;
                }
                else
                {
                    cerr << i18n("Syntax Error: Unknown option '%1'", how) << endl;
                    return false;
                }
                shell.netmgrChangeNetworkingEnabled(enabled);
                return true;
            }
            else if (what == "wireless")
            {
                bool enabled;
                if (how == "enabled")
                {
                    enabled = true;
                }
                else if (how == "disabled")
                {
                    enabled = false;
                }
                else
                {
                    cerr << i18n("Syntax Error: Unknown option '%1'", how) << endl;
                    return false;
                }
                shell.netmgrChangeWirelessEnabled(enabled);
                return true;
            }
      /*cout << "  solidshell network set network 'device-uni' 'network-uni' [authentication 'key']" << endl; */
            /*wep hex64|ascii64|hex128|ascii128|passphrase 'key' [open|shared] */
            /* wpaeap UNIMPLEMENTED */
            else if (what == "network")
            {
                checkArgumentCount(5, 10);
                QString dev(args->arg(3));
                QString uni(args->arg(4));
                Solid::Control::Authentication * auth = 0;
                QMap<QString,QString> secrets;

                if (KCmdLineArgs::parsedArgs()->count() > 5)
                {
                QString hasAuth = args->arg(5);
                if (hasAuth == "authentication")
                {
                    //encrypted network
                    QString authScheme = args->arg(6);
                    if (authScheme == "wep")
                    {
                        Solid::Control::AuthenticationWep *wepAuth = new Solid::Control::AuthenticationWep();
                        QString keyType = args->arg(7);
                        if (keyType == "hex64")
                        {
                            wepAuth->setType(Solid::Control::AuthenticationWep::WepHex);
                            wepAuth->setKeyLength(64);
                        }
                        else if (keyType == "ascii64")
                        {
                            wepAuth->setType(Solid::Control::AuthenticationWep::WepAscii);
                            wepAuth->setKeyLength(64);
                        }
                        else if (keyType == "hex128")
                        {
                            wepAuth->setType(Solid::Control::AuthenticationWep::WepHex);
                            wepAuth->setKeyLength(128);
                        }
                        else if (keyType == "ascii128")
                        {
                            wepAuth->setType(Solid::Control::AuthenticationWep::WepAscii);
                            wepAuth->setKeyLength(128);
                        }
                        else if (keyType == "passphrase64")
                        {
                            wepAuth->setType(Solid::Control::AuthenticationWep::WepPassphrase);
                            wepAuth->setKeyLength(64);
                        }
                        else if (keyType == "passphrase128")
                        {
                            wepAuth->setType(Solid::Control::AuthenticationWep::WepPassphrase);
                            wepAuth->setKeyLength(128);
                        }
                        else
                        {
                            cerr << i18n("Unrecognised WEP type '%1'", keyType) << endl;
                            delete wepAuth;
                            return false;
                        }

                        QString key = args->arg(8);
                        secrets.insert("key", key);
                        wepAuth->setSecrets(secrets);

                        QString method = args->arg(9);
                        if (method == "open")
                            wepAuth->setMethod(Solid::Control::AuthenticationWep::WepOpenSystem);
                        else if (method == "shared")
                            wepAuth->setMethod(Solid::Control::AuthenticationWep::WepSharedKey);
                        else
                        {
                            cerr << i18n("Unrecognised WEP method '%1'", method) << endl;
                            delete wepAuth;
                            return false;
                        }
                        auth = wepAuth;
                    }
                    else if (authScheme == "wpapsk")
                    {
                        /* wpapsk wpa|wpa2 tkip|ccmp-aes password */
                        Solid::Control::AuthenticationWpaPersonal *wpapAuth = new Solid::Control::AuthenticationWpaPersonal();
                        QString version = args->arg(7);
                        if (version == "wpa")
                            wpapAuth->setVersion(Solid::Control::AuthenticationWpaPersonal::Wpa1);
                        else if (version == "wpa2")
                            wpapAuth->setVersion(Solid::Control::AuthenticationWpaPersonal::Wpa1);
                        else
                        {
                            cerr << i18n("Unrecognised WPA version '%1'", version) << endl;
                            delete wpapAuth;
                            return false;
                        }
                        QString protocol = args->arg(8);
                        if (protocol == "tkip")
                            wpapAuth->setProtocol(Solid::Control::AuthenticationWpaPersonal::WpaTkip);
                        else if (protocol == "ccmp-aes")
                            wpapAuth->setProtocol(Solid::Control::AuthenticationWpaPersonal::WpaCcmpAes);
                        else
                        {
                            cerr << i18n("Unrecognised WPA encryption protocol '%1'", protocol) << endl;
                            delete wpapAuth;
                            return false;
                        }
                        QString key = args->arg(9);
                        secrets.insert("key", key);
                        wpapAuth->setSecrets(secrets);
                        auth = wpapAuth;
                    }
                    else
                    {
                        cerr << i18n("Unimplemented auth scheme '%1'", args->arg(6)) << endl;
                        return false;
                    }
                }
                }
                else
                {
                    //unencrypted network
                    auth = new Solid::Control::AuthenticationNone;
                }

                return shell.netmgrActivateNetwork(dev, uni, auth);
                delete auth;
            }
            else
            {
                cerr << i18n("Syntax Error: Unknown object '%1'", what) << endl;
                return false;
            }
        }
        else if (command == "listdevices")
        {
            return shell.netmgrList();
        }
        else if (command == "listnetworks")
        {
            checkArgumentCount(3, 3);
            QString device(args->arg(2));
            return shell.netmgrListNetworks(device);
        }
        else
        {
            cerr << i18n("Syntax Error: Unknown command '%1'" , command) << endl;
        }
    } else if (domain == "bluetooth")
    {
        if (command == "listadapters")
        {
            return shell.bluetoothListAdapters();
        }
        else if (command == "defaultadapter")
        {
            return shell.bluetoothDefaultAdapter();
        }
        else if (command == "set")
        {
            QString what(args->arg(2));
            QString ubi(args->arg(3));
            QString value(args->arg(4));

            if (what == "name")
            {
                return shell.bluetoothAdapterSetName(ubi, value);
            }
            else if (what == "mode")
            {
                return shell.bluetoothAdapterSetMode(ubi, value);
            }

        }
        else if (command == "query")
        {
            QString what(args->arg(2));
            QString ubi(args->arg(3));

            if (what == "mode")
            {
                return shell.bluetoothAdapterMode(ubi);
            }
            else if (what == "address")
            {
                return shell.bluetoothAdapterAddress(ubi);
            }
            else if (what == "name")
            {
                return shell.bluetoothAdapterName(ubi);
            }
            else if (what == "connections")
            {
                return shell.bluetoothAdapterListConnections(ubi);
            }
            else if (what == "bondings")
            {
                return shell.bluetoothAdapterListBondings(ubi);
            }

        }
        else if (command == "scan")
        {
            QString ubi (args->arg(2));
            return shell.bluetoothAdapterScan(ubi);
        }
        else if (command == "input")
        {
            QString what (args->arg(2));

            if (what == "listdevices")
            {
                return shell.bluetoothInputListDevices();
            }

            QString ubi (args->arg(3));

            if (what == "setup")
            {
                return shell.bluetoothInputSetup(ubi);
            }
            else if (what == "remove")
            {
                return shell.bluetoothInputRemoveSetup(ubi);
            }
            else if (what == "connect")
            {
                return shell.bluetoothInputConnect(ubi);
            }
            else if (what == "disconnect")
            {
                return shell.bluetoothInputDisconnect(ubi);
            }
        }
        else if (command == "remote" && args->count() >= 4)
        {
            QString what (args->arg(2));
            QString adapter (args->arg(3));
            QString remote (args->arg(4));

            if (what == "createbonding")
            {
                return shell.bluetoothRemoteCreateBonding(adapter, remote);
            }
            else if (what == "removebonding")
            {
                return shell.bluetoothRemoteRemoveBonding(adapter, remote);
            }
            else if (what == "hasbonding")
            {
                return shell.bluetoothRemoteHasBonding(adapter, remote);
            }

        }
        else
        {
            cerr << i18n("Syntax Error: Unknown command '%1'" , command) << endl;
        }

    }
    else
    {
        cerr << i18n("Syntax Error: Unknown command group '%1'" , domain) << endl;
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

bool SolidShell::powerQuerySuspendMethods()
{
    Solid::Control::PowerManager::SuspendMethods methods = Solid::Control::PowerManager::supportedSuspendMethods();

    if (methods  & Solid::Control::PowerManager::ToDisk)
    {
        cout << "to_disk" << endl;
    }

    if (methods  & Solid::Control::PowerManager::ToRam)
    {
        cout << "to_ram" << endl;
    }

    if (methods  & Solid::Control::PowerManager::Standby)
    {
        cout << "standby" << endl;
    }

    return true;
}

bool SolidShell::powerSuspend(const QString &strMethod)
{
    Solid::Control::PowerManager::SuspendMethods supported
        = Solid::Control::PowerManager::supportedSuspendMethods();

    Solid::Control::PowerManager::SuspendMethod method = Solid::Control::PowerManager::UnknownSuspendMethod;

    if (strMethod == "to_disk" && (supported  & Solid::Control::PowerManager::ToDisk))
    {
        method = Solid::Control::PowerManager::ToDisk;
    }
    else if (strMethod == "to_ram" && (supported  & Solid::Control::PowerManager::ToRam))
    {
        method = Solid::Control::PowerManager::ToRam;
    }
    else if (strMethod == "standby" && (supported  & Solid::Control::PowerManager::Standby))
    {
        method = Solid::Control::PowerManager::Standby;
    }
    else
    {
        cerr << i18n("Unsupported suspend method: %1" , strMethod) << endl;
        return false;
    }

    KJob *job = Solid::Control::PowerManager::suspend(method);

    if (job==0)
    {
        cerr << i18n("Error: unsupported operation!") << endl;
        return false;
    }

    connectJob(job);

    job->start();
    m_loop.exec();

    if (m_error)
    {
        cerr << i18n("Error: %1" , m_errorString) << endl;
        return false;
    }
    else
    {
        return true;
    }
}

bool SolidShell::powerQuerySchemes()
{
    QString current = Solid::Control::PowerManager::scheme();
    QStringList schemes = Solid::Control::PowerManager::supportedSchemes();

    foreach (QString scheme, schemes)
    {
        cout << scheme << " (" << Solid::Control::PowerManager::schemeDescription(scheme) << ")";

        if (scheme==current)
        {
            cout << " [*]" << endl;
        }
        else
        {
            cout << endl;
        }
    }

    return true;
}

bool SolidShell::powerChangeScheme(const QString &schemeName)
{
    QStringList supported = Solid::Control::PowerManager::supportedSchemes();

    if (!supported.contains(schemeName))
    {
        cerr << i18n("Unsupported scheme: %1" , schemeName) << endl;
        return false;
    }

    return Solid::Control::PowerManager::setScheme(schemeName);
}

bool SolidShell::powerQueryCpuPolicies()
{
    Solid::Control::PowerManager::CpuFreqPolicy current = Solid::Control::PowerManager::cpuFreqPolicy();
    Solid::Control::PowerManager::CpuFreqPolicies policies = Solid::Control::PowerManager::supportedCpuFreqPolicies();

    QList<Solid::Control::PowerManager::CpuFreqPolicy> all_policies;
    all_policies << Solid::Control::PowerManager::OnDemand
                 << Solid::Control::PowerManager::Userspace
                 << Solid::Control::PowerManager::Powersave
                 << Solid::Control::PowerManager::Performance;

    foreach (Solid::Control::PowerManager::CpuFreqPolicy policy, all_policies)
    {
        if (policies  & policy)
        {
            switch (policy)
            {
            case Solid::Control::PowerManager::OnDemand:
                cout << "ondemand";
                break;
            case Solid::Control::PowerManager::Userspace:
                cout << "userspace";
                break;
            case Solid::Control::PowerManager::Powersave:
                cout << "powersave";
                break;
            case Solid::Control::PowerManager::Performance:
                cout << "performance";
                break;
            case Solid::Control::PowerManager::UnknownCpuFreqPolicy:
                break;
            }

            if (policy==current)
            {
                cout << " [*]" << endl;
            }
            else
            {
                cout << endl;
            }
        }
    }

    return true;
}

bool SolidShell::powerChangeCpuPolicy(const QString &policyName)
{
    Solid::Control::PowerManager::CpuFreqPolicies supported
        = Solid::Control::PowerManager::supportedCpuFreqPolicies();

    Solid::Control::PowerManager::CpuFreqPolicy policy = Solid::Control::PowerManager::UnknownCpuFreqPolicy;

    if (policyName == "ondemand" && (supported  & Solid::Control::PowerManager::OnDemand))
    {
        policy = Solid::Control::PowerManager::OnDemand;
    }
    else if (policyName == "userspace" && (supported  & Solid::Control::PowerManager::Userspace))
    {
        policy = Solid::Control::PowerManager::Userspace;
    }
    else if (policyName == "performance" && (supported  & Solid::Control::PowerManager::Performance))
    {
        policy = Solid::Control::PowerManager::Performance;
    }
    else if (policyName == "powersave" && (supported  & Solid::Control::PowerManager::Powersave))
    {
        policy = Solid::Control::PowerManager::Powersave;
    }
    else
    {
        cerr << i18n("Unsupported cpufreq policy: %1" , policyName) << endl;
        return false;
    }

    return Solid::Control::PowerManager::setCpuFreqPolicy(policy);
}

bool SolidShell::netmgrNetworkingEnabled()
{
    if (Solid::Control::NetworkManager::isNetworkingEnabled())
        cout << i18n("networking: is enabled")<< endl;
    else
        cout << i18n("networking: is not enabled")<< endl;
    return Solid::Control::NetworkManager::isNetworkingEnabled();
}

bool SolidShell::netmgrWirelessEnabled()
{
    if (Solid::Control::NetworkManager::isWirelessEnabled())
        cout << i18n("wireless: is enabled")<< endl;
    else
        cout << i18n("wireless: is not enabled")<< endl;
    return Solid::Control::NetworkManager::isWirelessEnabled();
}

bool SolidShell::netmgrChangeNetworkingEnabled(bool enabled)
{
    Solid::Control::NetworkManager::setNetworkingEnabled(enabled);
    return true;
}

bool SolidShell::netmgrChangeWirelessEnabled(bool enabled)
{
    Solid::Control::NetworkManager::setWirelessEnabled(enabled);
    return true;
}

bool SolidShell::netmgrList()
{
    const Solid::Control::NetworkInterfaceList all = Solid::Control::NetworkManager::networkInterfaces();

    cerr << "debug: network interface list contains: " << all.count() << " entries" << endl;
    foreach (const Solid::Control::NetworkInterface device, all)
    {
        cout << "UNI = '" << device.uni() << "'" << endl;
    }
    return true;
}

bool SolidShell::netmgrListNetworks(const QString  & deviceUni)
{
    Solid::Control::NetworkInterface device = Solid::Control::NetworkManager::findNetworkInterface(deviceUni);

    Solid::Control::NetworkList networks = device.networks();
    foreach (const Solid::Control::Network * net, networks)
    {
        cout << "NETWORK UNI = '" << net->uni() << "'" << endl;
    }

    return true;
}

bool SolidShell::netmgrQueryNetworkInterface(const QString  & deviceUni)
{
    cerr << "SolidShell::netmgrQueryNetworkInterface()" << endl;
    Solid::Control::NetworkInterface device = Solid::Control::NetworkManager::findNetworkInterface(deviceUni);
    cout << device << endl;
    return true;
}

bool SolidShell::netmgrQueryNetwork(const QString  & deviceUni, const QString  & networkUni)
{
    cerr << "SolidShell::netmgrQueryNetwork()" << endl;
    Solid::Control::NetworkInterface device = Solid::Control::NetworkManager::findNetworkInterface(deviceUni);
    Solid::Control::Network * network = device.findNetwork(networkUni);
    cout << *network << endl;
    Solid::Control::WirelessNetwork * wlan = qobject_cast<Solid::Control::WirelessNetwork *>(network);
    if (wlan)
    {
        cout << *wlan << endl;
    }
    return true;
}

bool SolidShell::netmgrActivateNetwork(const QString  & deviceUni, const QString  & networkUni, Solid::Control::Authentication * auth)
{
    Solid::Control::NetworkInterface device = Solid::Control::NetworkManager::findNetworkInterface(deviceUni);
    Solid::Control::Network * network = device.findNetwork(networkUni);
    Solid::Control::WirelessNetwork * wlan = 0;
    if (( wlan = qobject_cast<Solid::Control::WirelessNetwork *>(network)))
    {
        wlan->setAuthentication(auth);
        wlan->setActivated(true);
    }
    else
        network->setActivated(true);
    return true;
}

bool SolidShell::bluetoothListAdapters()
{
    Solid::Control::BluetoothManager &manager = Solid::Control::BluetoothManager::self();

    const Solid::Control::BluetoothInterfaceList all = manager.bluetoothInterfaces();

    foreach (const Solid::Control::BluetoothInterface device, all)
    {
        cout << "UBI = '" << device.ubi() << "'" << endl;
    }
    return true;
}

bool SolidShell::bluetoothDefaultAdapter()
{
    Solid::Control::BluetoothManager &manager = Solid::Control::BluetoothManager::self();

    cout << "UBI = '" <<  manager.defaultInterface() << "'" << endl;

    return true;
}

bool SolidShell::bluetoothAdapterAddress(const QString &ubi)
{
    Solid::Control::BluetoothManager &manager = Solid::Control::BluetoothManager::self();
    Solid::Control::BluetoothInterface adapter = manager.findBluetoothInterface(ubi);

    cout << "'" <<  adapter.address() << "'" << endl;

    return true;
}

bool SolidShell::bluetoothAdapterName(const QString &ubi)
{
    Solid::Control::BluetoothManager &manager = Solid::Control::BluetoothManager::self();
    Solid::Control::BluetoothInterface adapter = manager.findBluetoothInterface(ubi);

    cout << "'" <<  adapter.name() << "'" << endl;

    return true;
}

bool SolidShell::bluetoothAdapterSetName(const QString &ubi, const QString &name)
{
    Solid::Control::BluetoothManager &manager = Solid::Control::BluetoothManager::self();
    Solid::Control::BluetoothInterface adapter = manager.findBluetoothInterface(ubi);

    adapter.setName(name);

    return true;
}

bool SolidShell::bluetoothAdapterMode(const QString &ubi)
{
    Solid::Control::BluetoothManager &manager = Solid::Control::BluetoothManager::self();
    Solid::Control::BluetoothInterface adapter = manager.findBluetoothInterface(ubi);

    cout << "'" <<  adapter.mode() << "'" << endl;

    return true;
}

bool SolidShell::bluetoothAdapterSetMode(const QString &ubi, const QString &mode)
{
    Solid::Control::BluetoothManager &manager = Solid::Control::BluetoothManager::self();
    Solid::Control::BluetoothInterface adapter = manager.findBluetoothInterface(ubi);
    Solid::Control::BluetoothInterface::Mode modeEnum(Solid::Control::BluetoothInterface::Off);
    if (mode == "off")
    {
        modeEnum = Solid::Control::BluetoothInterface::Off;
    }
    else if (mode == "connectable")
    {
        modeEnum = Solid::Control::BluetoothInterface::Connectable;
    }
    else if (mode == "discoverable")
    {
        modeEnum = Solid::Control::BluetoothInterface::Discoverable;
    }
    adapter.setMode(modeEnum);

    return true;
}

bool SolidShell::bluetoothAdapterListConnections(const QString &ubi)
{
    Solid::Control::BluetoothManager &manager = Solid::Control::BluetoothManager::self();
    Solid::Control::BluetoothInterface adapter = manager.findBluetoothInterface(ubi);

    const Solid::Control::BluetoothRemoteDeviceList all = adapter.listConnections();

    cout << "Current connections of Bluetooth Adapter: " << ubi << endl;
    foreach (const Solid::Control::BluetoothRemoteDevice device, all)
    {
        cout << "UBI = '" << device.ubi() << "'" << endl;
    }
    return true;
}

bool SolidShell::bluetoothAdapterListBondings(const QString &ubi)
{
    Solid::Control::BluetoothManager &manager = Solid::Control::BluetoothManager::self();
    Solid::Control::BluetoothInterface adapter = manager.findBluetoothInterface(ubi);

    const QStringList all = adapter.listBondings();

    cout << "Current bonded/paired remote bluetooth devices of Bluetooth Adapter: " << ubi << endl;
    foreach (const QString device, all)
    {
        cout << "UBI = '" << device << "'" << endl;
    }
    return true;
}

bool SolidShell::bluetoothAdapterScan(const QString &ubi)
{
    Solid::Control::BluetoothManager &manager = Solid::Control::BluetoothManager::self();
    Solid::Control::BluetoothInterface adapter = manager.findBluetoothInterface(ubi);

    connect(&adapter, SIGNAL(remoteDeviceFound(const QString &, int, int)),
           this, SLOT(slotBluetoothDeviceFound(const QString &, int, int)));
    connect(&adapter, SIGNAL(discoveryCompleted()),
           this, SLOT(slotBluetoothDiscoveryCompleted()));

    adapter.discoverDevices();
    // Workaround for the fakebluetooth backend... quit the discovery after 30 seconds
    QTimer::singleShot(30000, this, SLOT(slotBluetoothDiscoveryCompleted()));
    cout << "Searching ..." << endl;
    m_loop.exec();

    return true;
}

void SolidShell::slotBluetoothDeviceFound(const QString &ubi, int deviceClass, int rssi)
{
    cout << QString("['%1','%2','%3']").arg(ubi).arg(deviceClass).arg(rssi) << endl;
}

void SolidShell::slotBluetoothDiscoveryCompleted()
{
    kDebug() ;
    m_loop.exit();
}

bool SolidShell::bluetoothInputListDevices()
{
    Solid::Control::BluetoothManager &manager = Solid::Control::BluetoothManager::self();
    const Solid::Control::BluetoothInputDeviceList all = manager.bluetoothInputDevices();

    foreach (const Solid::Control::BluetoothInputDevice device, all)
    {
        cout << "UBI = '" << device.ubi() << "'" << endl;
    }

    return true;
}

bool SolidShell::bluetoothInputSetup(const QString &deviceUbi)
{
    Solid::Control::BluetoothManager &manager = Solid::Control::BluetoothManager::self();
    KJob *job = manager.setupInputDevice(deviceUbi);

    if (job==0)
    {
        cerr << i18n("Error: unsupported operation!") << endl;
        return false;
    }

    connectJob(job);

    job->start();
    m_loop.exec();

    if (m_error)
    {
        cerr << i18n("Error: %1" , m_errorString) << endl;
        return false;
    }

    return true;
}

bool SolidShell::bluetoothInputRemoveSetup(const QString &deviceUbi)
{
    Solid::Control::BluetoothManager &manager = Solid::Control::BluetoothManager::self();

    manager.removeInputDevice(deviceUbi);

    return true;
}

bool SolidShell::bluetoothInputConnect(const QString &deviceUbi)
{
    Solid::Control::BluetoothManager &manager = Solid::Control::BluetoothManager::self();
    Solid::Control::BluetoothInputDevice device = manager.findBluetoothInputDevice(deviceUbi);

    device.slotConnect();

    return true;
}

bool SolidShell::bluetoothInputDisconnect(const QString &deviceUbi)
{
    Solid::Control::BluetoothManager &manager = Solid::Control::BluetoothManager::self();
    Solid::Control::BluetoothInputDevice device = manager.findBluetoothInputDevice(deviceUbi);

    device.slotDisconnect();

    return true;
}

bool SolidShell::bluetoothRemoteCreateBonding(const QString &adapterUbi, const QString &deviceUbi)
{
    Solid::Control::BluetoothManager &manager = Solid::Control::BluetoothManager::self();
    Solid::Control::BluetoothInterface adapter = manager.findBluetoothInterface(adapterUbi);
    Solid::Control::BluetoothRemoteDevice device = adapter.findBluetoothRemoteDevice(deviceUbi);

    KJob *job = device.createBonding();

    connectJob(job);

    job->start();
    m_loop.exec();

    if (m_error)
    {
        cerr << i18n("Error: %1" , m_errorString) << endl;
        return false;
    }

    return true;
}

bool SolidShell::bluetoothRemoteRemoveBonding(const QString &adapterUbi, const QString &deviceUbi)
{
    Solid::Control::BluetoothManager &manager = Solid::Control::BluetoothManager::self();
    Solid::Control::BluetoothInterface adapter = manager.findBluetoothInterface(adapterUbi);
    Solid::Control::BluetoothRemoteDevice device = adapter.findBluetoothRemoteDevice(deviceUbi);

    device.removeBonding();

    return true;
}

bool SolidShell::bluetoothRemoteHasBonding(const QString &adapterUbi, const QString &deviceUbi)
{
    Solid::Control::BluetoothManager &manager = Solid::Control::BluetoothManager::self();
    Solid::Control::BluetoothInterface adapter = manager.findBluetoothInterface(adapterUbi);
    Solid::Control::BluetoothRemoteDevice device = adapter.findBluetoothRemoteDevice(deviceUbi);

    if (device.hasBonding())
    {
        cout << "'" << deviceUbi << "' is bonded/paired." << endl;
    } else {
        cout << "'" << deviceUbi << "' is not bonded/paired." << endl;
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
