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
#include "Application.h"

#include <QThreadPool>
#include <QDebug>

#include "GlobalDefine.h"
#include "Model.h"
#include "TrObject.h"
#include "UIService.h"
#include "MountService.h"
#include "ThreadService.h"

DWIDGET_USE_NAMESPACE

Application::Application(int &argc, char **argv)
    : DApplication(argc, argv)
{
    loadTranslator();

    Model::InitInstance(this);
    TrObject::initInstance(this);
    MountService::InitService(this);

    setAttribute(Qt::AA_UseHighDpiPixmaps);
    setAttribute(Qt::AA_EnableHighDpiScaling);

    setOrganizationName(PhoneAssistantModel::g_organization_name);
    setApplicationName(PhoneAssistantModel::g_application_name);
    setApplicationVersion(DApplication::buildVersion(""));
    setProductIcon(QIcon::fromTheme(PhoneAssistantModel::g_app_icon_name));
    //    setWindowIcon(QIcon::fromTheme(PhoneAssistantModel::g_app_icon_name));

    setApplicationDisplayName(TrObject::getInstance()->getDlgTitle(dlg_app_title));
    setApplicationDescription(TrObject::getInstance()->getDlgTitle(dlg_app_Description));

    ThreadService::InitService();

    UIService::InitService();
}

Application::~Application()
{
    delete UIService::getService();

    delete ThreadService::getService();

    QThreadPool::globalInstance()->clear(); //清除队列
    QThreadPool::globalInstance()->waitForDone();
    qDebug() << "xigou   current Threads:" << QThreadPool::globalInstance()->activeThreadCount();
}
