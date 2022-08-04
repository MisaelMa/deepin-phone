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
#ifndef MUSICINFOTASK_H
#define MUSICINFOTASK_H

#include "FileDisplayTask.h"
#include "defines.h"

#include <QFileInfo>
/**
 * @brief 音乐信息获取任务
 */

class MusicInfoTask : public FileDisplayTask
{
    Q_OBJECT
public:
    explicit MusicInfoTask(QObject *parent = nullptr);

signals:
    void sigMusicInfoLoaded(const QString &path, const QString &);

public:
    void setData(const QStringList &);

    // QRunnable interface
public:
    void run() override;

private:
    void getMusicLength();
    QString lengthString(const qint64 &length);

private:
    QStringList m_strFilePathList;
};

#endif // MUSICTASK_H
