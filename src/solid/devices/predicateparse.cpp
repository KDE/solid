/*
    SPDX-FileCopyrightText: 2006 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

extern "C" {
#include "predicateparse.h"

void PredicateParse_mainParse(const char *_code);
}

#include "predicate.h"
#include "soliddefs_p.h"

#include <stdlib.h>

#include <QStringList>
#include <QThreadStorage>

namespace Solid
{
namespace PredicateParse
{
struct ParsingData {
    ParsingData()
        : result(nullptr)
    {
    }

    Solid::Predicate *result;
    QByteArray buffer;
};

}
}

Q_GLOBAL_STATIC(QThreadStorage<Solid::PredicateParse::ParsingData *>, s_parsingData)

Solid::Predicate Solid::Predicate::fromString(const QString &predicate)
{
    Solid::PredicateParse::ParsingData *data = new Solid::PredicateParse::ParsingData();
    s_parsingData->setLocalData(data);
    data->buffer = predicate.toLatin1();
    PredicateParse_mainParse(data->buffer.constData());
    Predicate result;
    if (data->result) {
        result = Predicate(*data->result);
        delete data->result;
    }
    s_parsingData->setLocalData(nullptr);
    return result;
}

void PredicateParse_setResult(void *result)
{
    Solid::PredicateParse::ParsingData *data = s_parsingData->localData();
    data->result = (Solid::Predicate *)result;
}

void PredicateParse_errorDetected(const char *s)
{
    qWarning("ERROR from solid predicate parser: %s", s);
    s_parsingData->localData()->result = nullptr;
}

void PredicateParse_destroy(void *pred)
{
    Solid::PredicateParse::ParsingData *data = s_parsingData->localData();
    Solid::Predicate *p = (Solid::Predicate *)pred;
    if (p != data->result) {
        delete p;
    }
}

void *PredicateParse_newAtom(char *interface, char *property, void *value)
{
    QString iface = QString::fromLatin1(interface, -1);
    QString prop = QString::fromLatin1(property, -1);
    QVariant *val = (QVariant *)value;

    Solid::Predicate *result = new Solid::Predicate(iface, prop, *val);

    delete val;
    free(interface);
    free(property);

    return result;
}

void *PredicateParse_newMaskAtom(char *interface, char *property, void *value)
{
    QString iface = QString::fromLatin1(interface, -1);
    QString prop = QString::fromLatin1(property, -1);
    QVariant *val = (QVariant *)value;

    Solid::Predicate *result = new Solid::Predicate(iface, prop, *val, Solid::Predicate::Mask);

    delete val;
    free(interface);
    free(property);

    return result;
}

void *PredicateParse_newIsAtom(char *interface)
{
    QString iface = QString::fromLatin1(interface);

    Solid::Predicate *result = new Solid::Predicate(iface);

    free(interface);

    return result;
}

void *PredicateParse_newIsNotAtom(char *interface)
{
    QString iface = QString::fromLatin1(interface);

    Solid::Predicate *result = new Solid::Predicate(iface, Solid::Predicate::NOT);

    free(interface);

    return result;
}

void *PredicateParse_newAnd(void *pred1, void *pred2)
{
    Solid::Predicate *result = new Solid::Predicate();

    Solid::PredicateParse::ParsingData *data = s_parsingData->localData();
    Solid::Predicate *p1 = (Solid::Predicate *)pred1;
    Solid::Predicate *p2 = (Solid::Predicate *)pred2;

    if (p1 == data->result || p2 == data->result) {
        data->result = nullptr;
    }

    *result = *p1 & *p2;

    delete p1;
    delete p2;

    return result;
}

void *PredicateParse_newOr(void *pred1, void *pred2)
{
    Solid::Predicate *result = new Solid::Predicate();

    Solid::PredicateParse::ParsingData *data = s_parsingData->localData();
    Solid::Predicate *p1 = (Solid::Predicate *)pred1;
    Solid::Predicate *p2 = (Solid::Predicate *)pred2;

    if (p1 == data->result || p2 == data->result) {
        data->result = nullptr;
    }

    *result = *p1 | *p2;

    delete p1;
    delete p2;

    return result;
}

void *PredicateParse_newStringValue(char *val)
{
    QString s = QString::fromLatin1(val);

    free(val);

    return new QVariant(s);
}

void *PredicateParse_newBoolValue(int val)
{
    bool b = (val != 0);
    return new QVariant(b);
}

void *PredicateParse_newNumValue(int val)
{
    return new QVariant(val);
}

void *PredicateParse_newDoubleValue(double val)
{
    return new QVariant(val);
}

void *PredicateParse_newEmptyStringListValue()
{
    return new QVariant(QStringList());
}

void *PredicateParse_newStringListValue(char *name)
{
    QStringList list{QString::fromLatin1(name)};

    free(name);

    return new QVariant(list);
}

void *PredicateParse_appendStringListValue(char *name, void *list)
{
    QVariant *variant = (QVariant *)list;

    QStringList new_list = variant->toStringList();

    new_list << QString::fromLatin1(name);

    delete variant;
    free(name);

    return new QVariant(new_list);
}

void PredicateLexer_unknownToken(const char *text)
{
    qWarning("ERROR from solid predicate parser: unrecognized token '%s' in predicate '%s'\n", text, s_parsingData->localData()->buffer.constData());
}
