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
#ifndef EBOOKTASK_H
#define EBOOKTASK_H

#include "FileDisplayTask.h"
#include "defines.h"
/**
 * @brief 电子书数据任务
 */

class EBookTask : public FileDisplayTask
{
    Q_OBJECT
public:
    explicit EBookTask(QObject *parent = nullptr);
    ~EBookTask() override;

    // QRunnable interface
public:
    void run() override;

private:
    void getEBookInfoLevel(QString path, int dirLevel);

private:
    QStringList ebookSkipDir = QStringList() << "DCIM"
                                             << "Pictures"
                                             << "Screenshots"
                                             << "Music"
                                             << "Movies";
};

#endif // PHONEFILETASK_H
