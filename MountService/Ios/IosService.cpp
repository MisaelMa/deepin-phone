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
#include "IosService.h"

#include <QProcess>

#include "utils.h"

IosService::IosService(QObject *parent)
    : QObject(parent)
{
}

QList<PhoneAppInfo> IosService::getApps(const QString &sDeviceId)
{
    Q_UNUSED(sDeviceId);

    QList<PhoneAppInfo> iosAppList;
    QString strCommand = QString("ideviceinstaller -l");
    QString strOutMsg = "", sErrorMsg = "";
    int nRet = Utils::writeCommand(strCommand, strOutMsg, sErrorMsg, 2 * 1000);
    if (nRet == QProcess::NormalExit) {
        QStringList list_msg = strOutMsg.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
        foreach (QString item_msg, list_msg) {
            QStringList list_app = item_msg.split(" - ");
            if (list_app.count() != 2)
                continue;

            QString appInfo = list_app[1].trimmed();
            QString appName;
            QString appVer;
            int lastSpcIndex = appInfo.lastIndexOf(' ');
            if (lastSpcIndex > 0) {
                appName = appInfo.left(lastSpcIndex).trimmed();
                appVer = appInfo.mid(lastSpcIndex + 1).trimmed();
            } else {
                appName = appInfo;
                appVer = "";
            }
            PhoneAppInfo item_app;
            item_app.strAppName = appName;
            item_app.strAppVersion = appVer;
            item_app.strPacketName = list_app[0].trimmed();
            iosAppList.append(item_app);
        }
    }

    return iosAppList;
}

QString IosService::uninstallIosApp(const QString &strPacketName, const int &mescs)
{
    /**
    * @brief  ios高版本，带id卸载命令错误，直接卸载可生效
    * @param ideviceinstaller -U %1 -u %2
    */
    QString strCommand = QString("ideviceinstaller -u %1").arg(strPacketName);

    QString sOutMsg = "", sErrorMsg = "";

    int nRet = Utils::writeCommand(strCommand, sOutMsg, sErrorMsg, mescs);
    if (nRet == 0) {
        return sOutMsg;
    }
    return "";
}
