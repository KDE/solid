/*
    SPDX-FileCopyrightText: 2006 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "predicate.h"

#include <QMetaEnum>
#include <QSequentialIterable>
#include <QStringList>
#include <solid/device.h>

namespace Solid
{
class Predicate::Private
{
public:
    Private()
        : isValid(false)
        , type(PropertyCheck)
        , compOperator(Predicate::Equals)
        , equalIFace(true)
        , operand1(nullptr)
        , operand2(nullptr)
    {
    }

    bool isValid;
    Type type;

    DeviceInterface::Type ifaceType;
    QString property;
    QVariant value;
    Predicate::ComparisonOperator compOperator;
    bool equalIFace;

    Predicate *operand1;
    Predicate *operand2;
};
}

Solid::Predicate::Predicate()
    : d(new Private())
{
}

Solid::Predicate::Predicate(const Predicate &other)
    : d(new Private())
{
    *this = other;
}

Solid::Predicate::Predicate(const DeviceInterface::Type &ifaceType, const QString &property, const QVariant &value, ComparisonOperator compOperator)
    : d(new Private())
{
    d->isValid = true;
    d->ifaceType = ifaceType;
    d->property = property;
    d->value = value;
    d->compOperator = compOperator;
}

Solid::Predicate::Predicate(const QString &ifaceName, const QString &property, const QVariant &value, ComparisonOperator compOperator)
    : d(new Private())
{
    DeviceInterface::Type ifaceType = DeviceInterface::stringToType(ifaceName);

    if (((int)ifaceType) != -1) {
        d->isValid = true;
        d->ifaceType = ifaceType;
        d->property = property;
        d->value = value;
        d->compOperator = compOperator;
    }
}

Solid::Predicate::Predicate(const DeviceInterface::Type &ifaceType)
    : d(new Private())
{
    d->isValid = true;
    d->type = InterfaceCheck;
    d->ifaceType = ifaceType;
}

Solid::Predicate::Predicate(const QString &ifaceName)
    : d(new Private())
{
    DeviceInterface::Type ifaceType = DeviceInterface::stringToType(ifaceName);

    if (((int)ifaceType) != -1) {
        d->isValid = true;
        d->type = InterfaceCheck;
        d->ifaceType = ifaceType;
    }
}

Solid::Predicate::Predicate(QStringView ifaceName, NotOperator op)
    : Predicate(ifaceName.toString())
{
    Q_UNUSED(op) // It's just there to make the API nicer
    d->equalIFace = false;
}

Solid::Predicate::~Predicate()
{
    if (d->type != PropertyCheck && d->type != InterfaceCheck) {
        delete d->operand1;
        delete d->operand2;
    }

    delete d;
}

Solid::Predicate &Solid::Predicate::operator=(const Predicate &other)
{
    d->isValid = other.d->isValid;
    d->type = other.d->type;
    d->equalIFace = other.d->equalIFace;

    if (d->type != PropertyCheck && d->type != InterfaceCheck) {
        Predicate *operand1 = new Predicate(*(other.d->operand1));
        delete d->operand1;
        d->operand1 = operand1;
        Predicate *operand2 = new Predicate(*(other.d->operand2));
        delete d->operand2;
        d->operand2 = operand2;
    } else {
        d->ifaceType = other.d->ifaceType;
        d->property = other.d->property;
        d->value = other.d->value;
        d->compOperator = other.d->compOperator;
    }

    return *this;
}

Solid::Predicate Solid::Predicate::operator&(const Predicate &other)
{
    Predicate result;

    result.d->isValid = true;
    result.d->type = Conjunction;
    result.d->operand1 = new Predicate(*this);
    result.d->operand2 = new Predicate(other);

    return result;
}

Solid::Predicate &Solid::Predicate::operator&=(const Predicate &other)
{
    *this = *this & other;
    return *this;
}

Solid::Predicate Solid::Predicate::operator|(const Predicate &other)
{
    Predicate result;

    result.d->isValid = true;
    result.d->type = Disjunction;
    result.d->operand1 = new Predicate(*this);
    result.d->operand2 = new Predicate(other);

    return result;
}

Solid::Predicate &Solid::Predicate::operator|=(const Predicate &other)
{
    *this = *this | other;
    return *this;
}

bool Solid::Predicate::isValid() const
{
    return d->isValid;
}

bool Solid::Predicate::matches(const Device &device) const
{
    if (!d->isValid) {
        return false;
    }

    switch (d->type) {
    case Disjunction:
        return d->operand1->matches(device) || d->operand2->matches(device);
    case Conjunction:
        return d->operand1->matches(device) && d->operand2->matches(device);
    case PropertyCheck: {
        const DeviceInterface *iface = device.asDeviceInterface(d->ifaceType);

        if (iface != nullptr) {
            const int index = iface->metaObject()->indexOfProperty(d->property.toLatin1().constData());
            QMetaProperty metaProp = iface->metaObject()->property(index);
            QVariant value = metaProp.isReadable() ? metaProp.read(iface) : QVariant();
            QVariant expected = d->value;

            if (metaProp.isEnumType() && expected.userType() == QMetaType::QString) {
                QMetaEnum metaEnum = metaProp.enumerator();
                int value = metaEnum.keysToValue(d->value.toString().toLatin1().constData());
                if (value >= 0) { // No value found for these keys, resetting expected to invalid
                    expected = QVariant(metaProp.metaType(), &value);
                } else {
                    expected = QVariant();
                }
            } else if (metaProp.isEnumType() && expected.userType() == QMetaType::Int) {
                int expectedValue = expected.toInt();
                expected = QVariant(metaProp.metaType(), &expectedValue);
            }

            if (d->compOperator == Mask) {
                bool v_ok;
                int v = value.toInt(&v_ok);
                bool e_ok;
                int e = expected.toInt(&e_ok);

                return (e_ok && v_ok && (v & e));
            }

            if (value == expected) {
                return true;
            }

            // Make sure we can match single elements inside lists.
            if (value.canConvert<QSequentialIterable>()) {
                const auto iterable = value.value<QSequentialIterable>();
                for (const auto &element : iterable) {
                    if (element == expected) {
                        return true;
                    }
                }
            }
        }
        break;
    }
    case InterfaceCheck:
        return device.isDeviceInterface(d->ifaceType) == d->equalIFace;
    }

    return false;
}

QSet<Solid::DeviceInterface::Type> Solid::Predicate::usedTypes() const
{
    QSet<DeviceInterface::Type> res;

    if (d->isValid) {
        switch (d->type) {
        case Disjunction:
        case Conjunction:
            res += d->operand1->usedTypes();
            res += d->operand2->usedTypes();
            break;
        case PropertyCheck:
        case InterfaceCheck:
            res << d->ifaceType;
            break;
        }
    }

    return res;
}

QString Solid::Predicate::toString() const
{
    if (!d->isValid) {
        return QStringLiteral("False");
    }

    if (d->type != PropertyCheck && d->type != InterfaceCheck) {
        QString op = QStringLiteral("AND");
        if (d->type == Disjunction) {
            op = QStringLiteral("OR");
        }

        return QStringLiteral("[%1 %2 %3]").arg(d->operand1->toString(), op, d->operand2->toString());
    } else {
        QString ifaceName = DeviceInterface::typeToString(d->ifaceType);

        if (ifaceName.isEmpty()) {
            ifaceName = QStringLiteral("Unknown");
        }

        if (d->type == InterfaceCheck) {
            return QStringLiteral("IS ") + ifaceName;
        }

        QString value;

        switch (d->value.userType()) {
        case QMetaType::QStringList: {
            const QStringList list = d->value.toStringList();
            if (list.isEmpty()) {
                value = QStringLiteral("{}");
            } else {
                value = QStringLiteral("{'") + list.join(QStringLiteral("', '")) + QStringLiteral("'}'");
            }
            break;
        }
        case QMetaType::Bool:
            value = (d->value.toBool() ? QStringLiteral("true") : QStringLiteral("false"));
            break;
        case QMetaType::Int:
        case QMetaType::UInt:
        case QMetaType::LongLong:
        case QMetaType::ULongLong:
            value = d->value.toString();
            break;
        default:
            value = QStringLiteral("'%1'").arg(d->value.toString());
            break;
        }

        QString str_operator = QStringLiteral("==");
        if (d->compOperator != Equals) {
            str_operator = QStringLiteral(" &");
        }

        return QStringLiteral("%1.%2 %3 %4").arg(ifaceName, d->property, str_operator, value);
    }
}

Solid::Predicate::Type Solid::Predicate::type() const
{
    return d->type;
}

Solid::DeviceInterface::Type Solid::Predicate::interfaceType() const
{
    return d->ifaceType;
}

QString Solid::Predicate::propertyName() const
{
    return d->property;
}

QVariant Solid::Predicate::matchingValue() const
{
    return d->value;
}

Solid::Predicate::ComparisonOperator Solid::Predicate::comparisonOperator() const
{
    return d->compOperator;
}

Solid::Predicate Solid::Predicate::firstOperand() const
{
    if (d->operand1) {
        return *d->operand1;
    }
    return Predicate();
}

Solid::Predicate Solid::Predicate::secondOperand() const
{
    if (d->operand2) {
        return *d->operand2;
    }
    return Predicate();
}

bool Solid::Predicate::isInterfaceTypeNegated() const
{
    return !d->equalIFace;
}
