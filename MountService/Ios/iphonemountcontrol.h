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
#ifndef IPHONEMOUNTCONTROL_H
#define IPHONEMOUNTCONTROL_H

#include "defines.h"

#include <QObject>

#define PMERROR_MKPATH_FAILED 2001
#define PMERROR_MOUNT_FAILED 2001

class IPhoneMountControl : public QObject
{
    Q_OBJECT
public:
    enum RunMode {
        RM_MOUNT = 0,
        RM_UNMOUNT
    };

public:
    //挂载
    static bool mount(const QString &strPhoneID);
    //取消挂载
    static bool unmount(const QString &strPhoneID);
    //取消当前全部挂载
    static bool unmountAll();
    //根据Phoneid获取完整挂载路径
    static QString getMountAbsolutePath(const QString &strPhoneID);
    //获取挂载文件夹名称
    static QString getMountDirName(const QString &strPhoneID);
    //获取挂载目录
    static QString getMountDevice();
    //获取挂载目录
    static QString getMountPath();
    //获取挂载大小
    static bool getMountSize(const QString &strPhoneID, quint64 &nTotal, quint64 &nUsed, QString &);

    static QString getProductType(const QString &);

    IPhoneMountControl(const QString &strPhoneID, const RunMode &mode, QObject *parent = nullptr);
    virtual ~IPhoneMountControl();
    bool start();

signals:
    void error(int nErrorCode, QString strErrorMsg);

private:
    //挂载
    bool mount();
    //取消挂载
    bool unmount();
    //传输cmd命令
    static int writeCommand(const QString &strCommand, QString &strOutMsg);
    //挂载文件夹是否已经存在
    bool isExist_MountPath();

private:
    QString m_strPhoneID;
    RunMode m_mode;
};

#endif // IPHONEMOUNTCONTROL_H
