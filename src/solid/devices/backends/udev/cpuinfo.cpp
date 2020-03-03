/*
    Copyright 2010 Alex Merry <alex.merry@kdemail.net>

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

#include "cpuinfo.h"

#include <QFile>
#include <QRegularExpression>
#include <QStringList>

namespace Solid
{
namespace Backends
{
namespace UDev
{

class CpuInfo
{
public:
    CpuInfo();
    QString extractCpuInfoLine(int processorNumber, const QString &regExpStr);
    QString extractInfoLine(const QString &regExpStr);

private:
    QStringList cpuInfo;
};

QString extractCpuVendor(int processorNumber) {
    CpuInfo info;
    QString vendor = info.extractCpuInfoLine(processorNumber, "vendor_id\\s+:\\s+(\\S.+)");

    if (vendor.isEmpty()) {
        vendor = info.extractInfoLine("Hardware\\s+:\\s+(\\S.+)");
    }

    return vendor;
}

QString extractCpuModel(int processorNumber) {
    CpuInfo info;
    QString model = info.extractCpuInfoLine(processorNumber, "model name\\s+:\\s+(\\S.+)");

    if (model.isEmpty()) {
        model = info.extractInfoLine("Processor\\s+:\\s+(\\S.+)");
    }

    return model;
}

int extractCurrentCpuSpeed(int processorNumber) {
    CpuInfo info;
    int speed = info.extractCpuInfoLine(processorNumber, "cpu MHz\\s+:\\s+(\\d+).*").toInt();
    return speed;
}


CpuInfo::CpuInfo() {
    QFile cpuInfoFile("/proc/cpuinfo");
    if (!cpuInfoFile.open(QIODevice::ReadOnly)) {
        return;
    }
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
    cpuInfo = QString(cpuInfoFile.readAll()).split('\n', QString::SkipEmptyParts);
#else
    cpuInfo = QString(cpuInfoFile.readAll()).split('\n', Qt::SkipEmptyParts);
#endif
}

QString CpuInfo::extractCpuInfoLine(int processorNumber, const QString &regExpStr)
{
    const QRegularExpression processorRegExp(
                   QRegularExpression::anchoredPattern(QStringLiteral("processor\\s+:\\s+(\\d+)")));
    const QRegularExpression regExp(QRegularExpression::anchoredPattern(regExpStr));

    int line = 0;
    while (line < cpuInfo.size()) {
        QRegularExpressionMatch match;
        if ((match = processorRegExp.match(cpuInfo.at(line))).hasMatch()) {
            int recordProcNum = match.captured(1).toInt();
            if (recordProcNum == processorNumber) {
                ++line;
                while (line < cpuInfo.size()) {
                    if ((match = regExp.match(cpuInfo.at(line))).hasMatch()) {
                        return match.captured(1);
                    }
                    ++line;
                }
            }
        }
        ++line;
    }

    return QString();
}

QString CpuInfo::extractInfoLine(const QString &regExpStr)
{
    const QRegularExpression regExp(QRegularExpression::anchoredPattern(regExpStr));

    foreach (const QString &line, cpuInfo) {
        QRegularExpressionMatch match = regExp.match(line);
        if (match.hasMatch()) {
            return match.captured(1);
        }
    }
    return QString();
}


}
}
}
