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
#ifndef PMTASK_H
#define PMTASK_H

#include <QObject>
#include <QRunnable>

#include <QFileInfo>

#include "defines.h"

class PMTask : public QObject
    , public QRunnable
{
    Q_OBJECT
public:
    explicit PMTask(QObject *parent = nullptr);
    virtual ~PMTask() override {}

signals:
    void sigTaskFinished(); //  任务执行结束

public:
    void setTaskStop();
    void setTaskCanRun();

protected:
    bool m_bsTaskCanRun = true;
};

#endif // PMTASK_H
