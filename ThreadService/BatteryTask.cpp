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
#include "BatteryTask.h"

#include <QDebug>
#include <QThread>

#include "utils.h"

BatteryTask::BatteryTask(QObject *parent)
    : PMTask(parent)
{
}

void BatteryTask::run()
{
    do {
        if (m_bsTaskCanRun == false)
            break;

        getBattery();

        for (int i = 0; i < 5; i++) {
            QThread::msleep(200);

            if (m_bsTaskCanRun == false) {
                break;
            }
        }
    } while (true);
}

void BatteryTask::getBattery()
{
    QString sValue;
    if (m_strPhoneDeviceId != "") {
        QString strCommand = "";
        QString strKey = "";
        QString separator = "";
        QString phoneId = m_strPhoneDeviceId;
        if (m_devType == Mount_Ios) {
            strCommand = QString("ideviceinfo -u %1 --domain com.apple.mobile.battery").arg(phoneId);
            strKey = "BatteryCurrentCapacity";
            separator = "\n";
        } else if (m_devType == Mount_Android) {
            strCommand = QString("adb -s %1 shell dumpsys battery | grep level").arg(phoneId);
            strKey = "level";
            separator = "[\r\n]";
        }

        if (strCommand != "") {
            QString strOutMsg = Utils::execCmdNoResult(strCommand);

            sValue = Utils::getValueFromMsg(strOutMsg, strKey, separator);

            //qDebug() << __FUNCTION__ << " deviceId  " << m_strPhoneDeviceId << "        " << sValue;
            if (!sValue.isEmpty()) {
                //防止在获取电量命令执行过程中切换手机问题
                if (phoneId == m_strPhoneDeviceId)
                    emit sigBatteryValue(m_strPhoneDeviceId, sValue.toInt());
            }
        }
    }
}

void BatteryTask::setStrPhoneDeviceId(const QString &strPhoneDeviceId, const int &devType)
{
    m_strPhoneDeviceId = strPhoneDeviceId;
    m_devType = devType;
}
