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
#include "ThreadService.h"

#include <QDebug>

ThreadService *ThreadService::g_instance = nullptr;

ThreadService::ThreadService()
{
    m_pMountFileManager = new MountFileManager;
}

ThreadService::~ThreadService()
{
    delete m_pMountFileManager;
    m_pMountFileManager = nullptr;

    QMapIterator<E_Thread_Type, PMTask *> i(m_mapTask);
    while (i.hasNext()) {
        i.next();
        i.value()->setTaskStop();
    }
}

void ThreadService::InitService()
{
    if (g_instance == nullptr)
        g_instance = new ThreadService();
}

ThreadService *ThreadService::getService()
{
    if (g_instance == nullptr)
        g_instance = new ThreadService();
    return g_instance;
}

void ThreadService::startTask(const E_Thread_Type &type, PMTask *task)
{
    m_mapTask.insert(type, task);
    task->setTaskCanRun();

    QThreadPool::globalInstance()->start(task);
}

void ThreadService::stopTask(const E_Thread_Type &type)
{
    auto it = m_mapTask.find(type);
    while (it != m_mapTask.end() && it.key() == type) {
        auto task = it.value();
        task->setTaskStop();

        ++it;
    }

    m_mapTask.remove(type);
}
