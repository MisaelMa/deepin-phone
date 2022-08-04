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
#ifndef BASEENGINETHREAD_H
#define BASEENGINETHREAD_H

#include <QThread>

/**
 * @brief 引擎渲染基类
 */

class BaseEngineThread : public QThread
{
    Q_OBJECT
public:
    explicit BaseEngineThread(QObject *parent = nullptr);
    virtual ~BaseEngineThread() override;

public:
    void setImagePathList(const QStringList &path)
    {
        m_pFileList = path;
    }

signals:
    void sigImageBackLoaded(const QString &path, const QPixmap &data);

protected:
    bool m_bIsExit = true;
    QStringList m_pFileList;
};

#endif // BASEENGINETHREAD_H
