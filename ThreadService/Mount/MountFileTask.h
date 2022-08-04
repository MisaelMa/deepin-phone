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
#ifndef MOUNTFILETASK_H
#define MOUNTFILETASK_H

#include <QFileInfo>

#include "PMTask.h"
#include "defines.h"
/**
 * @brief 获取挂文件数据任务
 */

class MountFileTask : public PMTask
{
    Q_OBJECT
public:
    explicit MountFileTask(QObject *parent = nullptr);

    // QRunnable interface
public:
    void run() override;

    void setPhoneIdAndType(const QString &sId, const DEVICE_TYPE &);

private:
    QString getRootPath();
    void getRootFileInfo(const QString &);
    void getRootChildren(const QFileInfo &, const QString &parentPath);

private:
    DEVICE_TYPE m_pDeviceType = Mount_OTHER;
    QString m_strPhoneId = "";
};

#endif // MOUNTFILETASK_H
