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
#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <DSettingsDialog>
#include <DWidget>

#include <DSettings>
#include <qsettingbackend.h>

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

class SettingDialog : public DSettingsDialog
{
    Q_OBJECT
public:
    explicit SettingDialog(DWidget *parent = nullptr);
    virtual ~SettingDialog() override;

private:
    void setAppStartShortcut(const bool &);
    void setAppDesktopIcon(const bool &);

private:
    const QString executable_path = "/usr/share/applications/deepin-phone-assistant.desktop";

    DSettings *m_pDSettings = nullptr;
    QSettingBackend *m_backend = nullptr;
};

#endif // SETTINGDIALOG_H
