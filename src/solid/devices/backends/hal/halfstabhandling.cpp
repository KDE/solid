/*
    SPDX-FileCopyrightText: 2007 Kevin Ottens <ervin@kde.org>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "halfstabhandling.h"

#include <QFile>
#include <QMultiHash>
#include <QObject>
#include <QProcess>
#include <QRegularExpression>
#include <QTextStream>
#include <QWriteLocker>
#include <QElapsedTimer>

#include <soliddefs_p.h>
#include <config-solid.h>

#if HAVE_MNTENT_H
#include <mntent.h>
#elif defined(HAVE_SYS_MNTENT_H)
#include <sys/mntent.h>
#endif

#ifdef Q_OS_SOLARIS
#define FSTAB "/etc/vfstab"
#else
#define FSTAB "/etc/fstab"
#endif

typedef QMultiHash<QString, QString> QStringMultiHash;

class MountPointsCache
{
public:
    QReadWriteLock lock;
    bool firstCall = true;
    QElapsedTimer elapsedTime;
    QMultiHash<QString, QString> values;
};

Q_GLOBAL_STATIC(MountPointsCache, globalMountPointsCache)

QString _k_resolveSymLink(const QString &filename)
{
    QString resolved = filename;
    QString tmp = QFile::symLinkTarget(filename);

    while (!tmp.isEmpty()) {
        resolved = tmp;
        tmp = QFile::symLinkTarget(resolved);
    }

    return resolved;
}

bool _k_isNetworkFileSystem(const QString &fstype, const QString &devName)
{
    if (fstype == "nfs"
            || fstype == "nfs4"
            || fstype == "smbfs"
            || fstype == "cifs"
            || devName.startsWith(QLatin1String("//"))) {
        return true;
    }
    return false;
}

void _k_updateMountPointsCache()
{
    QWriteLocker locker(&globalMountPointsCache->lock);
    if (globalMountPointsCache->firstCall) {
        globalMountPointsCache->firstCall = false;
        globalMountPointsCache->elapsedTime.start();
    } else if (globalMountPointsCache->elapsedTime.elapsed() > 10000) {
        globalMountPointsCache->elapsedTime.restart();
    } else {
        return;
    }

    globalMountPointsCache->values.clear();

#if HAVE_SETMNTENT

    FILE *fstab;
    if ((fstab = setmntent(FSTAB, "r")) == nullptr) {
        return;
    }

    struct mntent *fe;
    while ((fe = getmntent(fstab)) != nullptr) {
        if (!_k_isNetworkFileSystem(fe->mnt_type, fe->mnt_fsname)) {
            const QString device = _k_resolveSymLink(QFile::decodeName(fe->mnt_fsname));
            const QString mountpoint = _k_resolveSymLink(QFile::decodeName(fe->mnt_dir));

            globalMountPointsCache->values.insert(device, mountpoint);
        }
    }

    endmntent(fstab);

#else

    QFile fstab(FSTAB);
    if (!fstab.open(QIODevice::ReadOnly)) {
        return;
    }

    QTextStream stream(&fstab);
    QString line;

    while (!stream.atEnd()) {
        line = stream.readLine().simplified();
        if (line.isEmpty() || line.startsWith('#')) {
            continue;
        }

        // not empty or commented out by '#'
        const QStringList items = line.split(' ');

#ifdef Q_OS_SOLARIS
        if (items.count() < 5) {
            continue;
        }
#else
        if (items.count() < 4) {
            continue;
        }
#endif
        //prevent accessing a blocking directory
        if (!_k_isNetworkFileSystem(items.at(2), items.at(0))) {
            const QString device = _k_resolveSymLink(items.at(0));
            const QString mountpoint = _k_resolveSymLink(items.at(1));

            globalMountPointsCache->values.insert(device, mountpoint);
        }
    }

    fstab.close();
#endif
}

bool Solid::Backends::Hal::FstabHandling::isInFstab(const QString &device)
{
    _k_updateMountPointsCache();
    const QString deviceToFind = _k_resolveSymLink(device);

    QReadLocker lock(&globalMountPointsCache->lock);
    return globalMountPointsCache->values.contains(deviceToFind);
}

QStringList Solid::Backends::Hal::FstabHandling::possibleMountPoints(const QString &device)
{
    _k_updateMountPointsCache();
    const QString deviceToFind = _k_resolveSymLink(device);

    QReadLocker lock(&globalMountPointsCache->lock);
    return globalMountPointsCache->values.values(deviceToFind);
}

QProcess *Solid::Backends::Hal::FstabHandling::callSystemCommand(const QString &commandName,
        const QStringList &args,
        QObject *obj, const char *slot)
{
    QStringList env = QProcess::systemEnvironment();
    env.replaceInStrings(
        QRegularExpression(QStringLiteral("^PATH=(.*)"), QRegularExpression::CaseInsensitiveOption),
        QStringLiteral("PATH=/sbin:/bin:/usr/sbin/:/usr/bin"));

    QProcess *process = new QProcess(obj);

    QObject::connect(process, SIGNAL(finished(int,QProcess::ExitStatus)),
                     obj, slot);

    process->setEnvironment(env);
    process->start(commandName, args);

    if (process->waitForStarted()) {
        return process;
    } else {
        delete process;
        return nullptr;
    }
}

QProcess *Solid::Backends::Hal::FstabHandling::callSystemCommand(const QString &commandName,
        const QString &device,
        QObject *obj, const char *slot)
{
    return callSystemCommand(commandName, QStringList() << device, obj, slot);
}

