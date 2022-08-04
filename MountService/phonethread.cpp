/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co., Ltd.
 *
 * Author:     yelei <yelei@uniontech.com>
 * Maintainer: yelei <yelei@uniontech.com>
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "phonethread.h"

#include <QProcess>
#include <QDebug>

#include "utils.h"
#include "GlobalDefine.h"
#include "MountService.h"
#include "Ios/IosUtils.h"

bool PhoneThread::readPhoneInfo(QString strPhoneID, DEVICE_TYPE type)
{
    if (strPhoneID.isEmpty()) {
        return false;
    }
    m_strPhoneID = strPhoneID;
    m_type = type;
    return true;
}

PhoneThread::PhoneThread(QObject *parent)
    : QThread(parent)
{
}

PhoneThread::~PhoneThread()
{
    quit();
    wait();
}

bool PhoneThread::findPhoneInfo()
{
    qDebug() << __LINE__ << __FUNCTION__ << m_strPhoneID << m_type;
    PhoneInfo info;
    QString strCommand;
    int nRes;
    QString strOutMsg;
    QString strErrMsg;
    switch (int(m_type)) {
    case Mount_Ios: {
        // IOS
        strCommand = QString("ideviceinfo -u %1").arg(m_strPhoneID);
        nRes = Utils::writeCommand(strCommand, strOutMsg, strErrMsg);
        if (nRes != QProcess::NormalExit) {
            emit error(nRes, strErrMsg);
            return false;
        }
        info.type = Mount_Ios;
        info.strPhoneID = m_strPhoneID;
        info.status = STATUS_CONNECTED;
        info.strDevName = Utils::getValueFromMsg(strOutMsg, "DeviceName");
        info.strBrand = Utils::getValueFromMsg(strOutMsg, "DeviceClass");
        info.strProductType = IosUtils::transIPhoneDevName(Utils::getValueFromMsg(strOutMsg, "ProductType"));
        //        info.strSysName = getValueFromMsg(strOutMsg, "ProductName");
        info.strProductVersion = "iOS " + Utils::getValueFromMsg(strOutMsg, "ProductVersion");
        info.strRegionInfo = Utils::getValueFromMsg(strOutMsg, "RegionInfo");

        info.nMemTotal = 0;
        info.nMemFree = 0;

        info.nMemTotal = Utils::getValueFromMsg(strOutMsg, "MemTotal").replace("kB", "").toULongLong();
        info.nMemFree = Utils::getValueFromMsg(strOutMsg, "MemFree").replace("kB", "").toULongLong();
    } break;
    case Mount_Android: {
        info.type = Mount_Android;
        info.strPhoneID = m_strPhoneID;
        info.status = STATUS_CONNECTED;
        strOutMsg = MountService::getService()->getPhoneModel(m_strPhoneID);

        if (strOutMsg.contains("error")) {
            for (int i = 0; i < 4; i++) {
                qDebug() << __LINE__ << __FUNCTION__ << strOutMsg;
                QThread::msleep(500);
                strOutMsg = MountService::getService()->getPhoneModel(m_strPhoneID);
                if (!strOutMsg.contains("error"))
                    break;
                qDebug() << __LINE__ << __FUNCTION__ << strOutMsg;
            }
        }
        qDebug() << __LINE__ << __FUNCTION__ << strOutMsg;

        info.strDevName = strOutMsg;
        info.strProductType = strOutMsg;

        strOutMsg = MountService::getService()->getPhoneBrand(m_strPhoneID);
        info.strBrand = strOutMsg;

        strOutMsg = MountService::getService()->getPhoneVersionRelease(m_strPhoneID);
        info.strProductVersion = "Android " + strOutMsg;

        strOutMsg = MountService::getService()->getLocaleRegion(m_strPhoneID);
        info.strRegionInfo = strOutMsg;

        strCommand = QString("adb -s %1 shell cat /proc/meminfo").arg(m_strPhoneID);
        nRes = Utils::writeCommand(strCommand, strOutMsg, strErrMsg);
        if (nRes != 0) {
            emit error(nRes, strErrMsg);
            return false;
        }

        strOutMsg = strOutMsg.replace("kB", "");
        info.nMemTotal = Utils::getValueFromMsg(strOutMsg, "MemTotal").toULongLong();

        quint64 nBuffers = Utils::getValueFromMsg(strOutMsg, "Buffers").toULongLong();
        quint64 nCached = Utils::getValueFromMsg(strOutMsg, "Cached").toULongLong();
        info.nMemFree = Utils::getValueFromMsg(strOutMsg, "MemFree").toULongLong();

        info.nMemFree += nBuffers + nCached;

        if (!apkInstall(info.strPhoneID, info)) {
            //install apk error
            info.status = STATUS_APKINSTALL_ERROR;
        }
    } break;
    default:
        return false;
    }

    emit sigFindPhoneInfo(info);
    return true;
}

void PhoneThread::run()
{
    QThread::msleep(200);

    findPhoneInfo();
}

bool PhoneThread::apkInstall(const QString &sDeviceId, PhoneInfo info)
{
    QString outputLines = MountService::getService()->getAdbPackage(sDeviceId);
    if (outputLines.contains(s_apk_package)) {
        QString sApkInDeviceVerison = "";

        outputLines = MountService::getService()->getAdbPackageVersionCode(sDeviceId);
        if (outputLines != "") {
            auto list = outputLines.split(" ");
            foreach (auto s, list) {
                if (s != "" && s.startsWith("versionCode")) {
                    sApkInDeviceVerison = s.replace("versionCode=", "");
                    break;
                }
            }
        }

        if (sApkInDeviceVerison != m_strApkVersion) {
            qDebug() << __FUNCTION__ << "   version    " << sApkInDeviceVerison << "        local " << m_strApkVersion;

            info.status = STATUS_APK_INSTALLING;
            emit sigFindPhoneInfo(info);
            return installNewApkInDevice(sDeviceId);
        }

        return MountService::getService()->startApkExe(sDeviceId);
    } else {
        qDebug() << __FUNCTION__ << "   app is not exist";

        info.status = STATUS_APK_INSTALLING;
        emit sigFindPhoneInfo(info);
        return installNewApkInDevice(sDeviceId);
    }
}

bool PhoneThread::installNewApkInDevice(const QString &sDeviceId)
{
    QProcess proc;
    QString outputLines = "";

    QString apkPath = QString("/usr/share/%1").arg(PhoneAssistantModel::g_application_name);

    QString cmd =
        QString("adb -s %1 install -r \"%2/apk/com.ut.phoneassistant.apk\"").arg(sDeviceId).arg(apkPath);
    qDebug() << "APP Path: " << cmd;
    // app不存在，安装app
    proc.start(cmd);
    proc.waitForFinished();
    outputLines = proc.readAll();

    if (outputLines.contains("Success")) {
        MountService::getService()->startApkExe(sDeviceId);

        return true;
    } else {
        if (m_bInstalFailed) {
            qDebug() << "Install second app error.";
            return false;
        }
        m_bInstalFailed = true;
        MountService::getService()->uninstallPhoneAPP(Mount_Android, sDeviceId, s_apk_package, 30000);

        return installNewApkInDevice(sDeviceId);
    }
}

void PhoneThread::setStrApkVersion(const QString &strApkVersion)
{
    m_strApkVersion = strApkVersion;
}
