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
#include "DataService.h"

#include <DFileIconProvider>

#include "FileModel.h"

#include <QDebug>
#include <QImageReader>
#include <QDir>
#include <QApplication>

DWIDGET_USE_NAMESPACE

DataService::DataService(QObject *parent)
    : QObject(parent)
{
}

QIcon DataService::getIconBySuffix(const QString &k, const QFileInfo &info)
{
    if (info.isDir()) {
        DFileIconProvider icon;
        return icon.icon(info);
    } else {
        auto it = m_pSuffixIcon.find(k);
        while (it != m_pSuffixIcon.end() && it.key() == k) {
            return it.value();
        }

        DFileIconProvider icon;
        auto icc = icon.icon(info);

        m_pSuffixIcon.insert(k, icc);

        return icc;
    }
}

//void DataService::intsertPathAndSize(const QString &path, const qint64 &size)
//{
//    m_pPathAndSize.insert(path, size);
//}

//qint64 DataService::getFileSizeByPath(const QString &path)
//{
//    return m_pPathAndSize[path];
//}
