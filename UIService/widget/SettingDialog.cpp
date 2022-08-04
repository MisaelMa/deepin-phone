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
#include "SettingDialog.h"

#include <DSettings>
#include <qsettingbackend.h>
#include <QStandardPaths>
#include <DSettingsOption>

#include "GlobalDefine.h"
#include "dbustartmanager.h"
#include "utils.h"

DCORE_USE_NAMESPACE

SettingDialog::SettingDialog(DWidget *parent)
    : DSettingsDialog(parent)
{
    setAttribute(Qt::WA_DeleteOnClose);

    QString m_configPath = QString("%1/%2/%3/config.conf").arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)).arg(PhoneAssistantModel::g_organization_name).arg(PhoneAssistantModel::g_application_name);

    m_backend = new QSettingBackend(m_configPath, this);

    m_pDSettings = DSettings::fromJsonFile(":/resources/settings.json");
    m_pDSettings->setBackend(m_backend);
    updateSettings(m_pDSettings);
    setFixedSize(720, 560);

    auto startAutomatically = m_pDSettings->option("base.Startupsettings.checkbox_one");
    connect(startAutomatically, &Dtk::Core::DSettingsOption::valueChanged, this, [=](QVariant value) {
        setAppStartShortcut(value.toBool());
    });

    auto settingsRadioGroup = m_pDSettings->option("base.Closemainwindow.radiogroupclose");
    bool miniOrClose = m_backend->getOption("base.Closemainwindow.radiogroupclose").toBool();
    settingsRadioGroup->setValue(miniOrClose);

    auto shortcuts = m_pDSettings->option("base.Shortcut.checkbox_two");
    connect(shortcuts, &Dtk::Core::DSettingsOption::valueChanged, this, [=](QVariant value) {
        setAppDesktopIcon(value.toBool());
    });

    auto askMeAlways = m_pDSettings->option("base.Closemainwindow.askmealways");

    bool nRemeber = m_backend->getOption("base.Closemainwindow.askmealways").toBool();
    askMeAlways->setValue(nRemeber);

    /**
    * @brief   fix:36956  每次打开new设置界面，读取快捷方式是否存在，判定按钮状态
    * @author huangyong
    * @date 2020-07-13 14:54
    * @copyright (c) 2020
    */
    QString desktop_path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    QString delete_path = desktop_path + "/deepin-phone-assistant.desktop";

    /**
    * @brief  判断是否是第一次安装手机助手，含有配置文件，执行，否则设置界面保持初始状态
    */
    QFile fileconfig(m_configPath);
    if (fileconfig.exists()) {
        QFile file(delete_path);
        if (file.exists()) {
            shortcuts->setValue(true);
        } else {
            shortcuts->setValue(false);
        };
    } else {
        askMeAlways->setValue(true);
        settingsRadioGroup->setValue(true);
        setAppStartShortcut(true);
        setAppDesktopIcon(false);
    }

    /**
        * @brief 启动器自启动按钮与设置界面同步
        * @author huangyong
        * @date 2020-07-13 19:36
        * @copyright (c) 2020
        */
    auto m_startManagerInterface = new DBusStartManager(this);

    auto reply = m_startManagerInterface->IsAutostart(executable_path);
    bool ret = reply.argumentAt(0).toBool();
    startAutomatically->setValue(ret);
    delete m_startManagerInterface;
}

SettingDialog::~SettingDialog()
{
    //删除指针
    if (m_backend) {
        delete m_backend;
        m_backend = nullptr;
    }
    if (m_pDSettings) {
        delete m_pDSettings;
        m_pDSettings = nullptr;
    }
}

/**
 * @brief SettingDialog::setAppDesktopShortcut  程序 自启动
 * @param bEnable true 是， false 否
 */
void SettingDialog::setAppStartShortcut(const bool &bEnable)
{
    auto m_startManagerInterface = new DBusStartManager(this);
    if (bEnable == false) {
        QDBusPendingReply<bool> reply = m_startManagerInterface->RemoveAutostart(executable_path);
        reply.waitForFinished();
        if (!reply.isError()) {
            bool ret = reply.argumentAt(0).toBool();
            qDebug() << "remove from startup:" << ret;
        } else {
            qCritical() << reply.error().name() << reply.error().message();
        }
    } else {
        //  已经是自启动了
        auto rl = m_startManagerInterface->IsAutostart(executable_path);
        if (rl == true)
            return;

        QDBusPendingReply<bool> reply = m_startManagerInterface->AddAutostart(executable_path);
        reply.waitForFinished();
        if (!reply.isError()) {
            bool ret = reply.argumentAt(0).toBool();
            qDebug() << "add to startup:" << ret;
        } else {
            qCritical() << reply.error().name() << reply.error().message();
        }
    }
    delete m_startManagerInterface;
}

/**
 * @brief SettingDialog::setAppDesktopIcon      设置桌面应用图标
 * @param enable
 */
void SettingDialog::setAppDesktopIcon(const bool &enable)
{
    QString sOutMsg = "", sErrorMsg = "";
    QString desktop_path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    if (enable == true) {
        Utils::writeCommand("cp " + executable_path + " " + desktop_path, sOutMsg, sErrorMsg);
    } else {
        QString delete_path = desktop_path + "/deepin-phone-assistant.desktop";
        Utils::writeCommand("rm -f " + delete_path, sOutMsg, sErrorMsg);
    }
}
