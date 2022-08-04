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
#include "PhoneFileTask.h"

#include <QDebug>
#include <QDir>
#include <QThread>
#include <QMutex>

#include "MountService.h"

QMutex g_enterDirMutex; //全局加载文件时读取目录信息用

PhoneFileTask::PhoneFileTask(QObject *parent)
    : FileDisplayTask(parent)
{
}

void PhoneFileTask::run()
{
    if (m_strLoadDirPath == "")
        getRootPath();

    qDebug() << __FUNCTION__ << "begin m_path = " << m_strLoadDirPath;

    getAllFileInfo();

    if (m_bsTaskCanRun) {
        emit sigTaskFinished();
    }
    qDebug() << __FUNCTION__ << " end m_path = " << m_strLoadDirPath;
}

void PhoneFileTask::getAllFileInfo()
{
    if (m_bsTaskCanRun == false) {
        return;
    }
    QDir dir(m_strLoadDirPath);
    if (!dir.exists()) {
        return;
    }

    qDebug() << __FUNCTION__ << " entryInfoList in.";
    g_enterDirMutex.lock(); //加锁
    QFileInfoList fileList = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot, QDir::DirsFirst);
    g_enterDirMutex.unlock(); //解锁
    qDebug() << __FUNCTION__ << " entryInfoList out.";
    for (auto file : fileList) {
        if (m_bsTaskCanRun == false)
            break;

        //判断是否要暂停
        //checkRunabled();

        emit sigFileInfo(file);

        QThread::usleep(1);
    }
}
