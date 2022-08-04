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
#include "DeleteFileTask.h"

#include <QDebug>
#include <QFileInfo>
#include <QProcess>

#include "FileUtils.h"

DeleteFileTask::DeleteFileTask(QObject *parent)
    : PMTask(parent)
{
}

DeleteFileTask::~DeleteFileTask()
{
    qDebug() << "~DeleteFileTask()";
}

void DeleteFileTask::run()
{
    qDebug() << __LINE__ << "delete file start.";

    if (m_fileList.size() > 0) {
        deleteFile(); //使用统一的删除方法
    }

    qDebug() << __LINE__ << "delete file end.";
}

void DeleteFileTask::setPathList(const QStringList &pathList)
{
    m_fileList = pathList;
}

void DeleteFileTask::deleteFile()
{
    foreach (QString strItemFile, m_fileList) {
        if (m_bsTaskCanRun == false)
            break;

        int ret = FileUtils::deleteFunc_cmd(strItemFile);

        emit sigDeleteFileResult(ret, strItemFile);
    }

    if (m_bsTaskCanRun == true) {
        emit sigTaskFinished();
    }
}
