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
#include "FileUtils.h"

#include <QStringList>
#include <QProcess>

FileUtils::FileUtils()
{
}

bool FileUtils::_delFile_one(const QString &strFile)
{
    int nRet = remove(strFile.toLocal8Bit().data());
    if (nRet != 0) {
        return false;
    }
    return true;
}

int FileUtils::deleteFunc_cmd(const QString &path)
{
    int ret = remove(path.toLocal8Bit().data()); // C库函数快速删除
    if (ret != 0) {
        QString cmd = "rm";
        QStringList args;
        args << "-rf" << path;
        ret = QProcess::execute(cmd, args);
    }
    return ret;
}

QStringList FileUtils::getImageFilters()
{
    QStringList filters;
    filters << "*.png"
            << "*.jpg"
            << "*.bmp"
            << "*.jpeg"
            << "*.svg"
            << "*.gif";
    return filters;
}

QStringList FileUtils::getVideoFilters()
{
    QStringList filters;
    filters << "*.mp4"
            << "*.mov"
            << "*.avi"
            << "*.rmvb"
            << "*.rm"
            << "*.flv"
            << "*.mkv"
            << "*.f4v"
            << "*.wmv"
            << "*.3gp";
    return filters;
}
