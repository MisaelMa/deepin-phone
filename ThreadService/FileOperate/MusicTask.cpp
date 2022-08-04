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
#include "MusicTask.h"

#include <QDebug>
#include <QDir>
#include <QThread>
#include <QMutex>

#include "FileModel.h"
#include "Model.h"
#include "MountService.h"
#include "utils.h"

extern QMutex g_enterDirMutex; //全局加载文件时读取目录信息用

MusicTask::MusicTask(QObject *parent)
    : FileDisplayTask(parent)
{
}

MusicTask::~MusicTask()
{
    //    qDebug() << __FUNCTION__;
}

void MusicTask::run()
{
    if (m_strLoadDirPath == "")
        getRootPath();

    //    qDebug() << __FUNCTION__ << " begin m_path = " << m_strLoadDirPath;

    getMusicInfoLevel(m_strLoadDirPath, 1);
    if (m_bsTaskCanRun) {
        emit sigTaskFinished();
    }
    //    qDebug() << __FUNCTION__ << " end m_path = " << m_strLoadDirPath;
}

//检索所有目录下的音乐文件，为了提高速度，目前只检索从根目录开始的4层
void MusicTask::getMusicInfoLevel(QString path, int dirLevel)
{
    if (m_bsTaskCanRun == false)
        return;

    QDir dir(path);
    if (path.isEmpty() || !dir.exists()) {
        return;
    }

    if (searchLevel != -1 && dirLevel > searchLevel)
        return;

    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    g_enterDirMutex.lock(); //加锁
    QFileInfoList fileList = dir.entryInfoList();
    g_enterDirMutex.unlock(); //解锁
    for (auto file : fileList) {
        if (m_bsTaskCanRun == false)
            break;

        //判断是否要暂停
        //checkRunabled();

        if (file.isDir()) {
            if (musicSkipDir.contains(file.fileName()))
                continue;

            getMusicInfoLevel(file.absoluteFilePath(), dirLevel + 1);
        } else {
            QString suffix = file.suffix();
            if (musicSuffix.contains(suffix, Qt::CaseInsensitive)) {
                emit sigFileInfo(file);
            }
        }

        //QThread::usleep(1);
    }
}
