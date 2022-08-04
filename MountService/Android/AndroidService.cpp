/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     wangzhixuan <wangzhixuan@uniontech.com>
*
* Maintainer: wangzhixuan <wangzhixuan@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "AndroidService.h"

#include "AndroidService_p.h"

AndroidService::AndroidService(QObject *p)
    : QObject(p)
    , d_ptr(new AndroidServicePrivate(this))
{
}

AndroidService::~AndroidService()
{
    Q_D(AndroidService);
    delete d;
}

QString AndroidService::getAdbPackage(const QString &sDeviceId)
{
    Q_D(AndroidService);
    return d->getAdbPackage(sDeviceId);
}

QString AndroidService::getAdbPackageVersionCode(const QString &sDeviceId)
{
    Q_D(AndroidService);
    return d->getAdbPackageVersionCode(sDeviceId);
}

QString AndroidService::getAddDevices()
{
    Q_D(AndroidService);
    return d->getAddDevices();
}

void AndroidService::runAdbServer()
{
    Q_D(AndroidService);
    d->runAdbServer();
}

bool AndroidService::startApkExe(const QString &sDeviceId)
{
    Q_D(AndroidService);
    return d->startApkExe(sDeviceId);
}

QString AndroidService::getAdbUsbState(const QString &deviceId)
{
    Q_D(AndroidService);
    return d->getAdbUsbState(deviceId);
}

void AndroidService::execAdbForward(const QString &devId)
{
    Q_D(AndroidService);
    d->execAdbForward(devId);
}

QString AndroidService::packagePath(const QString &Id, const QString &package)
{
    Q_D(AndroidService);
    return d->packagePath(Id, package);
}

QString AndroidService::getPhoneBattery(const QString &sDeviceId)
{
    Q_D(AndroidService);
    return d->getPhoneBattery(sDeviceId);
}

QString AndroidService::uninstallPhoneAPP(const QString &devID, const QString &package, const int &msecs)
{
    Q_D(AndroidService);
    return d->uninstallPhoneAPP(devID, package, msecs);
}

QString AndroidService::exportPackage(const QString &devID, const QString &path_apk, const QString &out_apk, const int &msecs)
{
    Q_D(AndroidService);
    return d->exportPackage(devID, path_apk, out_apk, msecs);
}

QString AndroidService::getPhoneModel(const QString &sDeviceId)
{
    Q_D(AndroidService);
    return d->getPhoneModel(sDeviceId);
}

QString AndroidService::getPhoneBrand(const QString &sDeviceId)
{
    Q_D(AndroidService);
    return d->getPhoneBrand(sDeviceId);
}

QString AndroidService::getPhoneVersionRelease(const QString &sDeviceId)
{
    Q_D(AndroidService);
    return d->getPhoneVersionRelease(sDeviceId);
}

QString AndroidService::getLocaleRegion(const QString &strPhoneID)
{
    Q_D(AndroidService);
    return d->getLocaleRegion(strPhoneID);
}

void AndroidService::changeInstallationPackageName(const QString &devID, const QString &oldName, const QString &newName)
{
    Q_D(AndroidService);
    d->changeInstallationPackageName(devID, oldName, newName);
}

void AndroidService::getSystemUseAndTotal(const QString &sDeviceId, quint64 &nSysTotal, quint64 &nSysUse, QString &sPer)
{
    Q_D(AndroidService);
    d->getSystemUseAndTotal(sDeviceId, nSysTotal, nSysUse, sPer);
}

int AndroidService::adbPull(const QString &sDevId, const QString &sSrcPath, const QString &sDestPth)
{
    Q_D(AndroidService);
    return d->adbPull(sDevId, sSrcPath, sDestPth);
}

int AndroidService::adbPush(const QString &sDevId, const QString &sSrcPath, const QString &sDestPth)
{
    Q_D(AndroidService);
    return d->adbPush(sDevId, sSrcPath, sDestPth);
}

QString AndroidService::androidDevMountPath(const QString &devId)
{
    Q_D(AndroidService);
    return d->androidDevMountPath(devId);
}
