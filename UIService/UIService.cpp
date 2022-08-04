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
#include "UIService.h"

#include <DGuiApplicationHelper>
#include <DMessageManager>
#include <QDebug>

UIService *UIService::g_instance = nullptr;

UIService::UIService()
{
    /**
         * @brief 只显示一个mainwindow
         */
    //修改 PMS46955非窗口特效下，最小化窗口无法唤起窗口问题 把窗口激活放到MainWindow处理
    //connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::newProcessInstance, this, &UIService::onNewProcessInstance);
}

UIService::~UIService()
{
    //    if (g_instance) {
    //        delete g_instance;
    //    }
}

void UIService::InitService()
{
    if (g_instance == nullptr)
        g_instance = new UIService();
}

UIService *UIService::getService()
{
    return g_instance;
}

void UIService::showMainWindow()
{
    //Init Normal window at first time
    m_qspMainWnd.reset(new MainWindow());
    m_qspMainWnd->show();
}

//void UIService::sendOkMessage(const QString &sMsg)
//{
//    DMessageManager::instance()->sendMessage(m_qspMainWnd.get(), QIcon::fromTheme("uospm_message_ok"), sMsg);
//}

//void UIService::sendWarnMessage(const QString &sMsg)
//{
//    DMessageManager::instance()->sendMessage(m_qspMainWnd.get(), QIcon::fromTheme("uospm_message_warning"), sMsg);
//}

void UIService::onNewProcessInstance(qint64 pid, const QStringList &arguments)
{
    Q_UNUSED(pid);
    Q_UNUSED(arguments);

    m_qspMainWnd->activateWindow();
    m_qspMainWnd->show();
}

MainWindow *UIService::getQspMainWnd() const
{
    return m_qspMainWnd.get();
}
