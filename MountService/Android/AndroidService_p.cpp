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
#include "AndroidService_p.h"
#include "GlobalDefine.h"

#include <unistd.h>
#include <QProcess>
#include <QDir>
#include <QThread>
#include <QDebug>

#include "AndroidService.h"
#include "utils.h"

AndroidServicePrivate::AndroidServicePrivate(QObject *parent)
    : QObject(parent)
{
    q_ptr = static_cast<AndroidService *>(parent);
}

/**
 * @brief 获取adb 的设备链接
 * @return
 */
QString AndroidServicePrivate::getAddDevices()
{
    return Utils::execCmdNoResult("adb devices");
}

QString AndroidServicePrivate::getAdbPackageVersionCode(const QString &sDeviceId)
{
    QString sCmd = QString("adb -s %1 shell dumpsys package %2 | grep versionCode").arg(sDeviceId).arg(s_apk_package);

    QString sResUlt = "";

    for (int i = 0; i < 5; i++) {
        sResUlt = Utils::execCmdNoResult(sCmd);
        if (sResUlt != "")
            break;

        QThread::msleep(200);
    }
    return sResUlt;
}

QString AndroidServicePrivate::getAdbPackage(const QString &sDeviceId)
{
    QString sResUlt = "";

    QString sCmd = (QString("adb -s %1 shell pm list packages | grep %2").arg(sDeviceId).arg(s_apk_package));
    for (int i = 0; i < 5; i++) {
        sResUlt = Utils::execCmdNoResult(sCmd);
        if (sResUlt != "")
            break;
        QThread::msleep(200);
    }
    return sResUlt;
}

/**
 * @brief 启动adb server 服务
 */
void AndroidServicePrivate::runAdbServer()
{
    Utils::execCmdNoResult("adb start-server");
}

QString AndroidServicePrivate::getAdbUsbState(const QString &deviceId)
{
    return Utils::execCmdNoResult(QString("adb -s %1 shell getprop sys.usb.state").arg(deviceId));
}

void AndroidServicePrivate::execAdbForward(const QString &devId)
{
    //转发socket
    QString adbForward = QString("adb -s %1 forward tcp:%2 tcp:%3").arg(devId).arg(10033).arg(10033);
    Utils::execCmdNoResult(adbForward);
}

QString AndroidServicePrivate::packagePath(const QString &Id, const QString &package)
{
    QString sOutMsg = "", sErrorMsg = "";
    int nRet = Utils::writeCommand(QString("adb -s %1 shell pm path %2").arg(Id).arg(package), sOutMsg, sErrorMsg);
    if (nRet == QProcess::NormalExit) {
        return sOutMsg;
    }
    return "";
}
/**
 * @brief 导出 app apk
 * @param 设备唯一id
 * @param apk 安装路径
 * @param 导出的apk 保存路径
 */
QString AndroidServicePrivate::exportPackage(const QString &devID, const QString &path_apk, const QString &out_apk, const int &msecs)
{
    return Utils::execCmdNoResult(QString("adb -s %1 pull  %2  %3 ").arg(devID).arg(path_apk).arg(out_apk), msecs);
}

//  卸载 app
QString AndroidServicePrivate::uninstallPhoneAPP(const QString &devID, const QString &package, const int &msecs)
{
    return Utils::execCmdNoResult(QString("adb -s %1 uninstall %2").arg(devID).arg(package), msecs);
}

//  获取 电量
QString AndroidServicePrivate::getPhoneBattery(const QString &sDeviceId)
{
    return Utils::execCmdNoResult(QString("adb -s %1 shell dumpsys battery | grep level").arg(sDeviceId));
}

/**
 * @brief 获取设备型号
 * @param sDeviceId
 * @return
 */
QString AndroidServicePrivate::getPhoneModel(const QString &sDeviceId)
{
    return Utils::execCmdNoResult(QString("adb -s %1 shell getprop ro.product.model").arg(sDeviceId));
}

/**
 * @brief 获取设备品牌
 * @param sDeviceId
 * @return
 */
QString AndroidServicePrivate::getPhoneBrand(const QString &sDeviceId)
{
    return Utils::execCmdNoResult(QString("adb -s %1 shell getprop ro.product.brand").arg(sDeviceId));
}

/**
 * @brief 版本
 * @param sDeviceId
 * @return
 */
QString AndroidServicePrivate::getPhoneVersionRelease(const QString &sDeviceId)
{
    return Utils::execCmdNoResult(QString("adb -s %1 shell getprop ro.build.version.release").arg(sDeviceId));
}

void AndroidServicePrivate::getSystemUseAndTotal(const QString &sDeviceId, quint64 &nSysTotal, quint64 &nSysUse, QString &nPer)
{
    QString strCommand = QString("adb -s %1 shell df /data").arg(sDeviceId);

    QString strOutMsg = Utils::execCmdNoResult(strCommand);
    if (strOutMsg != "") {
        auto strList = strOutMsg.split("\n");
        strList.removeAt(0);

        //adb shell df /data 结果显示的格式与Android版本有关, 要分别处理
        //Android7以下系统结果为：
        // Filesystem               Size     Used     Free   Blksize
        // /data                   11.6G     6.8G     4.8G   4096
        //Android7及以上系统结果为：
        // Filesystem            1K-blocks     Used Available Use% Mounted on
        // /dev/block/mmcblk0p87  51320796 21546708  29774088  42% /data

        const int unitValue = 1000; //单位换算值
        const long GBValue = unitValue * unitValue * unitValue; //GB to B
        const long MBValue = unitValue * unitValue; //MB to B
        const long KBValue = unitValue; //KB

        //获取Android版本
        QString strVer = getPhoneVersionRelease(sDeviceId);
        QString mVer = strVer.split(".").at(0); //取出主版本号

        if (mVer.toInt() >= 7) {
            //7以上高版本安卓 显示单位为K
            // Filesystem            1K-blocks     Used Available Use% Mounted on
            // /dev/block/mmcblk0p87  51320796 21546708  29774088  42% /data
            foreach (auto ss, strList) {
                auto sList = ss.split(" ", QString::SkipEmptyParts);
                //if (sList.indexOf("/data") > -1) {
                if (sList.size() >= 5) {
                    //修复PMS50250问题，有些手机df结果可能不含/data问题
                    quint64 nTempSysTotal = sList.at(1).toULong(); //  1k-blocks
                    nSysUse = sList.at(2).toULong(); // used
                    nPer = sList.at(4); //  百分比 容量

                    if (nTempSysTotal < 32 * 1000 * 1000) {
                        nSysTotal = 32 * 1000 * 1000;
                    } else if (nTempSysTotal < 64 * 1000 * 1000) {
                        nSysTotal = 64 * 1000 * 1000;
                    } else if (nTempSysTotal < 128 * 1000 * 1000) {
                        nSysTotal = 128 * 1000 * 1000;
                    } else if (nTempSysTotal < 256 * 1000 * 1000) {
                        nSysTotal = 256 * 1000 * 1000;
                    } else if (nTempSysTotal < 512 * 1000 * 1000) {
                        nSysTotal = 512 * 1000 * 1000;
                    }

                    nSysUse += nSysTotal - nTempSysTotal;
                }
            }
        } else {
            //7以下版本安卓 显示已转换单位的结果
            // Filesystem               Size     Used     Free   Blksize
            // /data                   11.6G     6.8G     4.8G   4096
            QString strTotal, strUsed;
            double nTotal, nUsed;
            foreach (auto ss, strList) {
                auto sList = ss.split(" ", QString::SkipEmptyParts);
                //if (sList.indexOf("/data") > -1) {
                if (sList.size() >= 5) {
                    //修复PMS50250问题，有些手机df结果可能不含/data问题
                    strTotal = sList.at(1);
                    strUsed = sList.at(2);

                    if (strTotal.endsWith('G')) {
                        strTotal = strTotal.replace('G', "");
                        nTotal = strTotal.toDouble() * GBValue;
                    } else if (strTotal.endsWith('M')) {
                        strTotal = strTotal.replace('M', "");
                        nTotal = strTotal.toDouble() * MBValue;
                    } else if (strTotal.endsWith('K')) {
                        strTotal = strTotal.replace('M', "");
                        nTotal = strTotal.toDouble() * KBValue;
                    } else {
                        nTotal = strTotal.toDouble();
                    }

                    if (strUsed.endsWith('G')) {
                        strUsed = strUsed.replace('G', "");
                        nUsed = strUsed.toDouble() * GBValue;
                    } else if (strUsed.endsWith('M')) {
                        strUsed = strUsed.replace('M', "");
                        nUsed = strUsed.toDouble() * MBValue;
                    } else if (strUsed.endsWith('K')) {
                        strUsed = strUsed.replace('M', "");
                        nUsed = strUsed.toDouble() * KBValue;
                    } else {
                        nUsed = strUsed.toDouble();
                    }

                    //总容量 获取的值不是全部存储的值，这里取整
                    if (nTotal < 16 * GBValue) {
                        nSysTotal = 16 * GBValue;
                    } else if (nTotal < 32 * GBValue) {
                        nSysTotal = 32 * GBValue;
                    } else if (nTotal < 64 * GBValue) {
                        nSysTotal = 64 * GBValue;
                    } else if (nTotal < 128 * GBValue) {
                        nSysTotal = 128 * GBValue;
                    } else if (nTotal < 256 * GBValue) {
                        nSysTotal = 256 * GBValue;
                    } else if (nTotal < 512 * GBValue) {
                        nSysTotal = 512 * GBValue;
                    } else {
                        nSysTotal = quint64(nTotal);
                    }

                    //已使用空间
                    nSysUse = quint64(nUsed);
                    nSysUse += nSysTotal - nTotal;
                    nSysUse = nSysUse / unitValue; //转换成K
                    nSysTotal = nSysTotal / unitValue; //转换成K
                    //百分比 容量
                    int pers = int(double(nSysUse) / nSysTotal * 100);
                    nPer = QString::number(pers);
                }
            }
        }
    }
}

int AndroidServicePrivate::adbPull(const QString &sDevId, const QString &sSrcPath, const QString &sDestPath)
{
    QString cmd = QString("adb -s %1 pull \"%2\" \"%3\"").arg(sDevId).arg(sSrcPath).arg(sDestPath);
    return QProcess::execute(cmd);
}

int AndroidServicePrivate::adbPush(const QString &sDevId, const QString &sSrcPath, const QString &sDestPath)
{
    QString cmd = QString("adb -s %1 push \"%2\" \"%3\"").arg(sDevId).arg(sSrcPath).arg(sDestPath);
    return QProcess::execute(cmd);
}

QString AndroidServicePrivate::androidDevMountPath(const QString &devId)
{
    QString devMntPath;
    QString gvfsPath = mountGvfsPath();
    QDir dir(gvfsPath);
    if (!dir.exists()) {
        return devMntPath;
    }

    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::DirsFirst);
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); i++) {
        QFileInfo fileInfo = list.at(i);
        if (fileInfo.isDir()) {
            QString fileName = fileInfo.fileName();
            if (fileName.contains(devId, Qt::CaseInsensitive)) {
                if (fileName.startsWith("gphoto2:")) {
                    devMntPath = fileInfo.absoluteFilePath() + QDir::separator();
                } else {
                    QString subDir = fileInfo.absoluteFilePath();
                    //判断根目录下的文件夹个数，如有多个文件夹，则返回根目录 + /
                    QDir strPath(subDir);
                    auto fileList = strPath.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
                    if (fileList.size() == 1) {
                        devMntPath = subDir;
                    } else if (fileList.size() > 1) {
                        devMntPath = subDir + QDir::separator();
                    }
                }
                //devMntPath = fileInfo.absoluteFilePath();
                break;
            }
        }
    }

    return devMntPath;
}

QString AndroidServicePrivate::mountGvfsPath()
{
    QString path = QString("/run/user/%1/gvfs").arg(getuid());
    return path;
}

//  启动 手机端  app
bool AndroidServicePrivate::startApkExe(const QString &sDeviceId)
{
    bool apkStartOk = false;
    auto outputLines = Utils::execCmdNoResult(QString("adb -s %1 shell ps | grep %2").arg(sDeviceId).arg(s_apk_package));
    if (outputLines == "") {
        outputLines = Utils::execCmdNoResult(QString("adb -s %1 shell am start -n %2/%3").arg(sDeviceId).arg(s_apk_package).arg(s_apk_class_name));
        if (outputLines.contains("Error:")) {
            qDebug() << __FUNCTION__ << outputLines;
            if (outputLines.endsWith("not exist.")) {
                //如果应用不存在，就重新安装
                QString apkPath = QString("/usr/share/%1").arg(PhoneAssistantModel::g_application_name);
                QString cmd = QString("adb -s %1 install -r \"%2/apk/com.ut.phoneassistant.apk\"").arg(sDeviceId).arg(apkPath);
                outputLines = Utils::execCmdNoResult(cmd);
                qDebug() << __FUNCTION__ << "install app:" << outputLines;
            }
        } else {
            apkStartOk = true;
        }
    } else {
        apkStartOk = true;
    }

    if (!apkStartOk) {
        //重新启动APP
        outputLines = Utils::execCmdNoResult(QString("adb -s %1 shell am start -n %2/%3").arg(sDeviceId).arg(s_apk_package).arg(s_apk_class_name));
        qDebug() << __FUNCTION__ << "restart app:" << outputLines;
    }

    return true;
}

QString AndroidServicePrivate::getLocaleRegion(const QString &strPhoneID)
{
    QString strCommand = QString("adb -s %1 shell getprop ro.product.locale.region").arg(strPhoneID);
    return Utils::execCmdNoResult(strCommand);
}

void AndroidServicePrivate::changeInstallationPackageName(const QString &devID, const QString &oldName, const QString &newName)
{
    Q_UNUSED(devID);
    QString strCommand = QString("mv %1 %2").arg(oldName).arg(newName);
    Utils::execCmdNoResult(strCommand);
}
