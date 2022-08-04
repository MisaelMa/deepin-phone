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
#include "FileDisplayTask.h"

#include <QThread>

#include "MountService.h"

FileDisplayTask::FileDisplayTask(QObject *parent)
    : PMTask(parent)
{
    setAutoDelete(false);
    m_bLocked = false;
}

FileDisplayTask::~FileDisplayTask()
{
    //线程解锁
    m_bLocked = false;
    m_mutex_read.unlock();
}

void FileDisplayTask::setPathAndType(const QString &path, const DEVICE_TYPE &devType, const QString &phoneId)
{
    m_strLoadDirPath = path;
    m_pDeviceType = devType;
    m_strPhoneID = phoneId;
}

void FileDisplayTask::getRootPath()
{
    //回避手机挂载慢的导致获取目录失败问题，尝试10次后仍不成功返回失败
    int retryCount = 10; //解决有些机器挂载慢问题，尝试10秒
    while (retryCount > 0 && m_bsTaskCanRun) {
        retryCount--;

        m_strLoadDirPath = MountService::getService()->GetMountPath(m_pDeviceType, m_strPhoneID);

        if (m_strLoadDirPath.isEmpty()) {
            QThread::sleep(1);
        } else {
            break;
        }
    }

    if (m_bsTaskCanRun) {
        if (m_strLoadDirPath.isEmpty()) {
            emit sigRootPathNotFound();
        } else {
            emit sigRootPath(m_strLoadDirPath);
        }
    }
}

// 标记线程是否要加锁
bool FileDisplayTask::lock()
{
    if (m_bLocked)
        return true;

    m_bLocked = true; //true时线程要加锁
    return true;
}

// 标记线程是否要解锁
bool FileDisplayTask::unlock()
{
    if (!m_bLocked)
        return true;

    m_bLocked = false; //标识线程不用加锁
    m_mutex_read.unlock(); //解锁线程
    return true;
}

//检查线程是否可继续运行
//使用Mutex自锁停住执行
void FileDisplayTask::checkRunabled()
{
    //判断是否要暂停
    if (m_bLocked) {
        //通过两次加锁，让自己锁住，线程暂停
        m_mutex_read.lock();
        m_mutex_read.lock();
    } else {
        //解锁
        m_mutex_read.unlock();
    }
}
