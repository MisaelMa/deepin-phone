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
#ifndef FILEDISPLAYTASK_H
#define FILEDISPLAYTASK_H

#include "PMTask.h"

#include <QMutex>

class FileDisplayTask : public PMTask
{
    Q_OBJECT
public:
    explicit FileDisplayTask(QObject *parent = nullptr);
    virtual ~FileDisplayTask() override;

signals:
    void sigFileInfo(const QFileInfo &);
    void sigRootPath(QString path);
    void sigRootPathNotFound();

public:
    void setPathAndType(const QString &path, const DEVICE_TYPE &, const QString &);

    //线程锁住
    bool lock(); //线程锁住
    bool unlock(); //线程解锁
    //检查线程是否可继续运行
    void checkRunabled();

protected:
    void getRootPath();

protected:
    QMutex m_mutex_read; //是否可加载文件锁
    bool m_bLocked; //是否要加锁

    DEVICE_TYPE m_pDeviceType = Mount_OTHER;
    QString m_strPhoneID = "";
    QString m_strLoadDirPath = "";

    const int searchLevel = 4; //目录搜索层数
};

#endif // FILEOPERATETASK_H
