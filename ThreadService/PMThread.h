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
#ifndef PMTHREAD_H
#define PMTHREAD_H

#include <QThread>

class PMThread : public QThread
{
    Q_OBJECT
public:
    explicit PMThread(QObject *parent = nullptr);
    virtual ~PMThread() override;

    void setThreadStop();
    void setThreadRun();

signals:
    void error(QString strErrorMsg);

protected:
    bool m_isCanRun = false; //标识位
};

#endif // PMTHREAD_H
