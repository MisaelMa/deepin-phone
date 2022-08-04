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
#include "PhoneUseTask.h"

#include <QThread>

#include "utils.h"

#include "MountService.h"
#include "Ios/iphonemountcontrol.h"

PhoneUseTask::PhoneUseTask(QObject *parent)
    : PMTask(parent)
{
}

void PhoneUseTask::run()
{
    do {
        if (m_bsTaskCanRun == false)
            break;

        if (m_strPhoneDeviceId != "")
            getDeviceStorage();

        for (int i = 0; i < 5; i++) {
            QThread::msleep(200);
            if (m_bsTaskCanRun == false) {
                break;
            }
        }
    } while (true);
}

void PhoneUseTask::getDeviceStorage()
{
    quint64 nSysTotal = 0, nSysFree = 0;

    QString sUsePer = "";

    MountService::getService()->getMountSize(m_devType, m_strPhoneDeviceId, nSysTotal, nSysFree, sUsePer);

    emit sigPhoneUse(nSysTotal, nSysFree, sUsePer);
}

void PhoneUseTask::setStrPhoneDeviceId(const QString &strPhoneDeviceId, const int &devType)
{
    m_strPhoneDeviceId = strPhoneDeviceId;
    m_devType = devType;
}
