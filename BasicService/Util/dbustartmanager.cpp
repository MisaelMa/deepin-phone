/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co., Ltd.
 *
 * Author:     huangyonga<huangyonga@uniontech.com>
 * Maintainer: huangyonga<huangyonga@uniontech.com>
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "dbustartmanager.h"

/*
 * Implementation of interface class DBusStartManager
 */

DBusStartManager::DBusStartManager(QObject *parent)
    : QDBusAbstractInterface("com.deepin.SessionManager", "/com/deepin/StartManager", staticInterfaceName(),
                             QDBusConnection::sessionBus(), parent)
{
    QDBusConnection::sessionBus().connect(this->service(), this->path(), "org.freedesktop.DBus.Properties",
                                          "PropertiesChanged", "sa{sv}as", this,
                                          SLOT(__propertyChanged__(QDBusMessage)));
}

DBusStartManager::~DBusStartManager()
{
    QDBusConnection::sessionBus().disconnect(service(), path(), "org.freedesktop.DBus.Properties", "PropertiesChanged",
                                             "sa{sv}as", this, SLOT(propertyChanged(QDBusMessage)));
}
