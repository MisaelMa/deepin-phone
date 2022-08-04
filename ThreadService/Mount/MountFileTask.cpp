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
#include "MountFileTask.h"

#include <QThread>
#include <QDir>
#include <QDebug>

#include "MountService.h"

MountFileTask::MountFileTask(QObject *parent)
    : PMTask(parent)
{
}

void MountFileTask::run()
{
    while (1) {
        if (m_bsTaskCanRun == false)
            break;

        QString sPath = getRootPath();
        if (sPath != "") {
            getRootFileInfo(sPath);
        } else {
            //  挂载失败了, 如何处理
        }

        for (int i = 0; i < 120; i++) {
            if (m_bsTaskCanRun) {
                QThread::msleep(500); //  1分钟更新一次数据
            }
        }
    }
}

void MountFileTask::setPhoneIdAndType(const QString &sId, const DEVICE_TYPE &pDeviceType)
{
    m_strPhoneId = sId;
    m_pDeviceType = pDeviceType;
}

void MountFileTask::getRootFileInfo(const QString &sPath)
{
    if (m_bsTaskCanRun == false) {
        return;
    }
    QDir dir(sPath);
    if (!dir.exists()) {
        return;
    }

    getRootChildren(QFileInfo(), sPath);
}

void MountFileTask::getRootChildren(const QFileInfo &parentdir, const QString &parentPath)
{
    if (m_bsTaskCanRun == false) {
        return;
    }
    Q_UNUSED(parentdir);

    QDir dir(parentPath);
    if (!dir.exists()) {
        return;
    }

    //取到所有的文件和文件名，但是去掉.和..的文件夹（这是QT默认有的）
    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);

    //文件夹优先
    dir.setSorting(QDir::DirsFirst);

    //转化成一个list
    QFileInfoList list = dir.entryInfoList();
    if (list.size() < 1) {
        return;
    }
    int i = 0;

    //递归算法的核心部分
    do {
        QFileInfo fileInfo = list.at(i);
        //如果是文件夹，递归
        bool bisDir = fileInfo.isDir();
        if (bisDir) {
            getRootChildren(fileInfo, fileInfo.filePath());
        } else {
        }
        i++;
    } while (i < list.size() && m_bsTaskCanRun);
}

QString MountFileTask::getRootPath()
{
    QString m_strRootPath = "";
    //回避手机挂载慢的导致获取目录失败问题，尝试5次后仍不成功返回失败
    int retryCount = 5;
    while (retryCount > 0 && m_bsTaskCanRun) {
        retryCount--;

        m_strRootPath = MountService::getService()->GetMountPath(m_pDeviceType, m_strPhoneId);

        if (m_strRootPath.isEmpty()) {
            QThread::sleep(1);
        } else {
            break;
        }
    }

    return m_strRootPath;
}
