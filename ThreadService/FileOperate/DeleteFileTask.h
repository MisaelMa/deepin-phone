/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co., Ltd.
 *
 * Author:     zhangliangliang <zhangliangliang@uniontech.com>
 * Maintainer: zhangliangliang <zhangliangliang@uniontech.com>
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef DELETEFILETASK_H
#define DELETEFILETASK_H

#include "PMTask.h"

/**
 * @brief 删除文件 任务
 */

class DeleteFileTask : public PMTask
{
    Q_OBJECT

public:
    DeleteFileTask(QObject *parent = nullptr);
    ~DeleteFileTask() override;

signals:
    void sigDeleteFileResult(const int &result, const QString &filePath);

public:
    void setPathList(const QStringList &pathList);

protected:
    void run() override;

private:
    void deleteFile();

private:
    QStringList m_fileList;
};

#endif // FILEMANAGETHREAD_H
