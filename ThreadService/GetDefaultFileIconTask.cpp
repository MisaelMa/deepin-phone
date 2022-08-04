/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zhangliangliang <zhangliangliang@uniontech.com>
*
* Maintainer: zhangliangliang <zhangliangliang@uniontech.com>
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
#include "GetDefaultFileIconTask.h"
#include "FileModel.h"
#include "Model.h"

#include <QTemporaryFile>
#include <QDebug>

GetDefaultFileIconTask::GetDefaultFileIconTask(QObject *parent)
    : PMTask(parent)
{
}

void GetDefaultFileIconTask::run()
{
    getAllFileIcon();
}

/**
 * @brief 获取文件类型图标 保存在DataService中
 * @author zhangliangliang
 */
void GetDefaultFileIconTask::getAllFileIcon()
{
    QStringList suffixList;
    suffixList << musicSuffix << ebookSuffix << photoSuffix << videoSuffix;

    foreach (auto suffix, suffixList) {
        QTemporaryFile tmpFile;
        if (tmpFile.open()) {
            tmpFile.rename(tmpFile.fileName() + "." + suffix);
            //qDebug() << __FUNCTION__ << suffix << tmpFile.fileName();
            Model::getInstance()->getIconBySuffix(suffix, tmpFile.fileName());
        }
    }
}
