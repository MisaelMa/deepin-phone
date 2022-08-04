/*
 * Copyright (C) 2019 ~ 2020 Deepin Technology Co., Ltd.
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
#include "iphonemountcontrol.h"

#include <QProcess>
#include <QDir>
#include <QDebug>

#include "TrObject.h"

const QString g_mount_dir_prefix = "pm_mount_";

IPhoneMountControl::IPhoneMountControl(const QString &strPhoneID, const IPhoneMountControl::RunMode &mode,
                                       QObject *parent)
    : QObject(parent)
{
    m_strPhoneID = strPhoneID;
    m_mode = mode;
}

IPhoneMountControl::~IPhoneMountControl() {}

bool IPhoneMountControl::start()
{
    switch (int(m_mode)) {
    case RM_MOUNT:
        if (!mount()) {
            unmount();
            return false;
        }
        break;
    case RM_UNMOUNT:
        if (isExist_MountPath()) {
            unmount();
        }
        break;
    }
    return true;
}

bool IPhoneMountControl::mount(const QString &strPhoneID)
{
    IPhoneMountControl devMount(strPhoneID, RM_MOUNT);
    return devMount.start();
}

bool IPhoneMountControl::unmount(const QString &strPhoneID)
{
    IPhoneMountControl devMount(strPhoneID, RM_UNMOUNT);
    return devMount.start();
}

bool IPhoneMountControl::unmountAll()
{
    QDir dir(getMountPath());
    foreach (QString itemChildrenDir, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        if (itemChildrenDir.startsWith(g_mount_dir_prefix)) {
            unmount(itemChildrenDir.remove(g_mount_dir_prefix));
        }
    }

    return true;
}

bool IPhoneMountControl::mount()
{
    if (isExist_MountPath()) {
        unmount();
    }

    QDir dir;
    bool bRet = dir.mkpath(getMountAbsolutePath(m_strPhoneID));
    if (!bRet) {
        qDebug() << __LINE__ << __FUNCTION__ << "mount error: mkdir failed!";
        emit error(PMERROR_MKPATH_FAILED,
                   TrObject::getInstance()->getMountText(ios_mount_create_failed) + getMountAbsolutePath(m_strPhoneID));
        return false;
    }

    QString strCmd, strOutMsg;
    int nRes = 0;
    strCmd = QString("idevicepair pair -u %1").arg(m_strPhoneID);
    nRes = writeCommand(strCmd, strOutMsg);
    if (nRes != 0) {
        QString strErrMsg;
        if (strOutMsg == "No device found with udid (null), is it plugged in?") {
            qDebug() << __LINE__;
        } else if (strOutMsg
                   == QString("ERROR: Could not validate with device %1 because a passcode is set. Please enter the "
                              "passcode on the device and retry.")
                          .arg(m_strPhoneID)) {
            qDebug() << __LINE__;
        } else if (strOutMsg
                   == QString("ERROR: Device %1 said that the user denied the trust dialog.").arg(m_strPhoneID)) {
            qDebug() << __LINE__;
        } else {
            strErrMsg = strOutMsg;
        }
        //
        strErrMsg = strOutMsg;
        //
        emit error(nRes, strErrMsg);
        return false;
    }

    strCmd = QString("ifuse %1 -u %2").arg(getMountAbsolutePath(m_strPhoneID)).arg(m_strPhoneID);
    nRes = writeCommand(strCmd, strOutMsg);
    //    if (nRes != 0) {
    //        emit error(nRes, strOutMsg);
    //        return false;
    //    }

    if (nRes == 255) // 安装失败ifuse
        return false;

    return true;
}

bool IPhoneMountControl::unmount()
{
    QString strCmd, strOutMsg;
    int nRes = 0;
    strCmd = QString("idevicepair unpair -u %1").arg(m_strPhoneID);
    nRes = writeCommand(strCmd, strOutMsg);
    if (nRes != 0) {
        qDebug() << __LINE__ << strCmd << strOutMsg;
        //        emit error(nRes, strErrMsg);
        //        return false;
    }
    strCmd = QString("fusermount -zu %1").arg(getMountAbsolutePath(m_strPhoneID));
    nRes = writeCommand(strCmd, strOutMsg);
    if (nRes != 0) {
        qDebug() << __LINE__ << strCmd << strOutMsg;
        //        emit error(nRes, strErrMsg);
        //        return false;
    }

    // rm path
    QDir dir;
    bool bRet = dir.rmpath(getMountAbsolutePath(m_strPhoneID));
    if (!bRet) {
        emit error(PMERROR_MKPATH_FAILED,
                   TrObject::getInstance()->getMountText(ios_mount_delete_failed) + getMountAbsolutePath(m_strPhoneID));
        qDebug() << __LINE__ << __FUNCTION__ << "rm path error:" << getMountAbsolutePath(m_strPhoneID);
        return false;
    }
    return true;
}

int IPhoneMountControl::writeCommand(const QString &strCommand, QString &strOutMsg)
{
    QProcess p;
    p.setProcessChannelMode(QProcess::MergedChannels);
    p.start(strCommand);
    p.waitForStarted();
    p.closeWriteChannel(); //关闭写通道 ，解决未响应问题
    p.waitForFinished();
    strOutMsg = QString::fromLocal8Bit(p.readAllStandardOutput()).trimmed();
    int nExitCode = p.exitCode();
    return nExitCode;
}

bool IPhoneMountControl::isExist_MountPath()
{
    QDir dir(getMountPath());
    QFileInfoList list_fileInfo = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach (QFileInfo item_fileInfo, list_fileInfo) {
        if (item_fileInfo.fileName() == getMountDirName(m_strPhoneID))
            return true;
    }
    return false;
}

QString IPhoneMountControl::getMountAbsolutePath(const QString &strPhoneID)
{
    return getMountPath() + QDir::separator() + getMountDirName(strPhoneID);
}

QString IPhoneMountControl::getMountDirName(const QString &strPhoneID)
{
    return g_mount_dir_prefix + strPhoneID;
}

QString IPhoneMountControl::getMountDevice()
{
    QString sRes = "";
    writeCommand("idevice_id -l", sRes);
    return sRes;
}

QString IPhoneMountControl::getMountPath()
{
    QString path = QString("/var/tmp/phoneassitant");
    return path;
}

bool IPhoneMountControl::getMountSize(const QString &strPhoneID, quint64 &nTotal, quint64 &nUsed, QString &sPer)
{
    QString strOutMsg;
    if (0 != writeCommand("df", strOutMsg)) {
        qDebug() << __LINE__ << strOutMsg;
        return false;
    }
    foreach (QString strItemOutmsg, strOutMsg.split("\n")) {
        if (strItemOutmsg.contains(strPhoneID)) {
            QStringList listDf = strItemOutmsg.split(" ", QString::SkipEmptyParts);
            if (listDf.count() != 6)
                return false;
            nTotal = listDf.at(1).toULongLong();
            nUsed = listDf.at(2).toULongLong();
            sPer = listDf.at(4);
            break;
        }
    }
    return true;
}

QString IPhoneMountControl::getProductType(const QString &devId)
{
    QString sRes = "";
    writeCommand(QString("ideviceinfo -u %1 -k ProductType").arg(devId), sRes);

    return sRes;
}
