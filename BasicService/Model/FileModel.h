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
#ifndef FILEMODEL_H
#define FILEMODEL_H

#include <QStringList>

static QStringList musicSuffix = QStringList() << "mp3"
                                               << "midi"
                                               << "wma"
                                               << "vqf"
                                               << "amr";
//搜索音乐文件时，跳过指定目录，如DCIM，Pictures
static QStringList musicSkipDir = QStringList() << "DCIM"
                                                << "Pictures"
                                                << "Screenshots"
                                                << "EBook";

const QStringList ebookSuffix = QStringList() << "chm"
                                              << "doc"
                                              << "ebk2"
                                              << "epub"
                                              << "fb2"
                                              << "jar"
                                              << "mobi"
                                              << "ndb"
                                              << "ndz"
                                              << "pdb"
                                              << "pdf"
                                              << "pmlz"
                                              << "rb"
                                              << "rtf"
                                              << "tcr"
                                              << "txt"
                                              << "zip";

const QStringList photoSuffix = QStringList() << "bmp"
                                              << "gif"
                                              << "jpeg"
                                              << "jpg"
                                              << "png"
                                              << "wbmp"
                                              << "webp"
                                              << "svg";

const QStringList videoSuffix = QStringList() << "3gp"
                                              << "avi"
                                              << "f4v"
                                              << "flv"
                                              << "mkv"
                                              << "mov"
                                              << "mp4"
                                              << "rmvb"
                                              << "wmv";

class FileModel
{
public:
    static QString getMusicFilterStr()
    {
        QString strFilter = "Music";
        strFilter += "(";
        foreach (QString item, musicSuffix) {
            strFilter += "*.";
            strFilter += item;
            strFilter += " ";
        }
        strFilter += ")";
        return strFilter;
    }

    static QString getEbookFilterStr()
    {
        QString strFilter = "eBook";
        strFilter += "(";
        foreach (QString item, ebookSuffix) {
            strFilter += "*.";
            strFilter += item;
            strFilter += " ";
        }
        strFilter += ")";
        return strFilter;
    }

}; // namespace FileModel

#endif // FILEMODEL_H
