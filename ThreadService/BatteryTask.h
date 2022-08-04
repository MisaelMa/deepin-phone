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
#ifndef BATTERYTASK_H
#define BATTERYTASK_H

#include "PMTask.h"
#include "defines.h"
/**
 * @brief 获取电池任务
 */

class BatteryTask : public PMTask
{
    Q_OBJECT
public:
    explicit BatteryTask(QObject *parent = nullptr);

signals:
    void sigBatteryValue(const QString &phoneId, const int &);

    // QRunnable interface
public:
    void run() override;

    void setStrPhoneDeviceId(const QString &strPhoneDeviceId, const int &devType);

private:
    void getBattery();

private:
    QString m_strPhoneDeviceId = "";
    int m_devType = Mount_OTHER;
};

#endif // BATTERYTASK_H
