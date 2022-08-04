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
#include "LocalApkVersionTask.h"

#include <QThread>

#include "utils.h"
#include "GlobalDefine.h"

LocalApkVersionTask::LocalApkVersionTask(QObject *parent)
    : PMTask(parent)
{
}

void LocalApkVersionTask::run()
{
    QString sResult = "";
    do {
        sResult = getLocalApkVersion();
        if (sResult != "") {
            emit sigFlagRes(sResult);
            break;
        }

        QThread::sleep(1);
    } while (sResult == "");
}

QString LocalApkVersionTask::getLocalApkVersion()
{
    QString sResult = "";

    QString apkPath = QString("/usr/share/%1/apk/com.ut.phoneassistant.apk").arg(PhoneAssistantModel::g_application_name);
    QString sCmd = QString("aapt dump badging %1 | grep versionCode").arg(apkPath);

    QString sVersionCode = "versionCode=";

    QString sRes = Utils::execCmdNoResult(sCmd, 1000);
    //  package: name='com.ut.utassistant' versionCode='2' versionName='1.1' platformBuildVersionName=''
    int nIndex = sRes.indexOf(sVersionCode);
    if (nIndex > -1) {
        auto tempRes = sRes.mid(nIndex);
        auto tempResList = tempRes.split(" ");
        foreach (auto s, tempResList) {
            if (s.startsWith(sVersionCode)) {
                sResult = s.replace(sVersionCode, "").replace("'", "");

                break;
            }
        }
    }

    return sResult;
}
