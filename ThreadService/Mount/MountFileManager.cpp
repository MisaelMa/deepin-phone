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
#include "MountFileManager.h"

#include <QApplication>

#include "ThreadService.h"
#include "MountFileTask.h"

MountFileManager::MountFileManager(QObject *parent)
    : QObject(parent)
{
}

MountFileManager::~MountFileManager()
{
    QMapIterator<QString, MountFileTask *> i(m_pMountFileTaskMap);
    while (i.hasNext()) {
        i.next();

        i.value()->setTaskStop();
    }
}

void MountFileManager::addMountFileTask(const QString &strPhoneId, const DEVICE_TYPE &devType)
{
    Q_UNUSED(strPhoneId);
    Q_UNUSED(devType);

    auto task = new MountFileTask(qApp);
    task->setPhoneIdAndType(strPhoneId, devType);
    ThreadService::getService()->startTask(ThreadService::E_File_Display_Task, task);

    m_pMountFileTaskMap.insert(strPhoneId, task);
}

void MountFileManager::removeMountFileTask(const QString &strPhoneId)
{
    Q_UNUSED(strPhoneId);
    auto it = m_pMountFileTaskMap.find(strPhoneId);
    while (it != m_pMountFileTaskMap.end() && it.key() == strPhoneId) {
        it.value()->setTaskStop();
    }
    m_pMountFileTaskMap.remove(strPhoneId);
}
