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

#include <DSettings>

void GenerateSettingTranslate()
{
    auto setting_basic = QObject::tr("Basic");
    auto setting_basic_startup = QObject::tr("Startup");
    auto setting_basic_close_main_window = QObject::tr("Close Main Window");
    auto setting_basic_desktop_shortcut = QObject::tr("Desktop Shortcut");

    auto basic_auto_start = QObject::tr("Auto startup (Recommended)");
    auto basic_exit = QObject::tr("Exit");
    auto basic_minimize_to_system_tray = QObject::tr("Minimize to system tray");
    auto basic_ask_me_always = QObject::tr("Ask me always");

    auto show_icon_on_desktop = QObject::tr("Show icon on desktop");

    auto reset_button_name = QObject::tr("Restore Defaults");
}
