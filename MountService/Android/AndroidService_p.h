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
#ifndef ANDROIDSERVICE_P_H
#define ANDROIDSERVICE_P_H

#include <QObject>

class AndroidService;

class AndroidServicePrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(AndroidService)

public:
    explicit AndroidServicePrivate(QObject *parent = nullptr);

private:
    QString getAdbPackage(const QString &sDeviceId);
    QString getAdbPackageVersionCode(const QString &sDeviceId);

    QString getAddDevices();
    void runAdbServer();
    QString getAdbUsbState(const QString &deviceId);

    void execAdbForward(const QString &devId);
    QString packagePath(const QString &Id, const QString &package);
    QString getPhoneBattery(const QString &sDeviceId);
    QString uninstallPhoneAPP(const QString &devID, const QString &package, const int &msecs);
    QString exportPackage(const QString &devID, const QString &path_apk, const QString &out_apk, const int &msecs);
    QString getPhoneModel(const QString &sDeviceId);
    QString getPhoneBrand(const QString &sDeviceId);
    QString getPhoneVersionRelease(const QString &sDeviceId);
    QString getLocaleRegion(const QString &strPhoneID);
    void changeInstallationPackageName(const QString &devID, const QString &oldName, const QString &newName);

    void getSystemUseAndTotal(const QString &sDeviceId, quint64 &nSysUse, quint64 &nSysTotal, QString &);
    int adbPull(const QString &, const QString &, const QString &);
    int adbPush(const QString &, const QString &, const QString &);

    QString androidDevMountPath(const QString &devId);
    QString mountGvfsPath();

    bool startApkExe(const QString &sDeviceId);

private:
    AndroidService *q_ptr;

    const QString s_apk_package = "com.ut.utassistant";
    const QString s_apk_class_name = s_apk_package + ".MainActivity";
};

#endif // ADBCMDOBJECT_H
