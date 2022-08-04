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
#include "SortFilterProxyModel.h"

#include <QDebug>

#include "defines.h"

#define FILE_NAME_COLUMN 0 // 文件名
#define FILE_SIZE_COLUMN 1 // 文件大小
#define FILE_DATE_COLUMN 2 // 修改时间
#define FILE_TYPE_COLUMN 3 // 类型

#define EBSIZE_COL 2 // 文件大小

#define MSIZE_COL 3 // 文件大小

#define ROLE_ITEM_DATA (Qt::UserRole + 66)
#define ROLE_ITEM_FILE_SIZE (Qt::UserRole + 101)

SortFilterProxyModel::SortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

SortFilterProxyModel::~SortFilterProxyModel()
{
}

void SortFilterProxyModel::setType(const int &iType)
{
    m_nType = iType;
}

bool SortFilterProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    if (!source_left.isValid() || !source_right.isValid())
        return false;

    if (m_nType == E_Widget_File) {
        return fileLessThan(source_left, source_right);
    }

    if (m_nType == E_Widget_App) {
        return appLessThan(source_left, source_right);
    }

    if (m_nType == E_Widget_Book) {
        return eBookLessThan(source_left, source_right);
    }

    if (m_nType == E_Widget_Music) {
        return musicLessThan(source_left, source_right);
    }

    if (m_nType == E_Widget_Photo || m_nType == E_Widget_Video) {
        return videoPhotoLessThan(source_left, source_right);
    }

    return QSortFilterProxyModel::lessThan(source_left, source_right);
}

bool SortFilterProxyModel::fileLessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    if ((source_left.column() == FILE_NAME_COLUMN) && (source_right.column() == FILE_NAME_COLUMN)) {
        //文件名按字符串简单排序，不区分大小写，没有复杂规则
        QVariant leftData = sourceModel()->data(source_left);
        QVariant rightData = sourceModel()->data(source_right);

        if (leftData.canConvert<QString>() && rightData.canConvert<QString>()) {
            QString strLeft = leftData.toString();
            QString strRight = rightData.toString();

            // 去掉后缀
            if (strLeft.contains(".")) {
                int nIndex = strLeft.lastIndexOf(".");
                strLeft = strLeft.left(nIndex);
            }
            if (strRight.contains(".")) {
                int nIndex = strRight.lastIndexOf(".");
                strRight = strRight.left(nIndex);
            }

            int ret = QString::compare(strLeft, strRight, Qt::CaseInsensitive);
            bool rst = ret < 0 ? true : false;
            return rst;
        }
    } else if ((source_left.column() == FILE_SIZE_COLUMN) && (source_right.column() == FILE_SIZE_COLUMN)) {
        // 这里我们所取得数据是用户源数据
        QVariant leftData = sourceModel()->data(source_left, ROLE_ITEM_FILE_SIZE);
        QVariant rightData = sourceModel()->data(source_right, ROLE_ITEM_FILE_SIZE);

        if (leftData.canConvert<qint64>() && rightData.canConvert<qint64>()) {
            return leftData.toLongLong() < rightData.toLongLong();
        }
    }

    return QSortFilterProxyModel::lessThan(source_left, source_right);
}

bool SortFilterProxyModel::appLessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    if ((source_left.column() == 2) && (source_right.column() == 2)) {
        // 这里我们所取得数据是用户源数据
        QVariant leftData = sourceModel()->data(source_left, ROLE_ITEM_FILE_SIZE);
        QVariant rightData = sourceModel()->data(source_right, ROLE_ITEM_FILE_SIZE);

        if (leftData.canConvert<qint64>() && rightData.canConvert<qint64>()) {
            return leftData.toLongLong() < rightData.toLongLong();
        }
    }

    return QSortFilterProxyModel::lessThan(source_left, source_right);
}

bool SortFilterProxyModel::eBookLessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    if ((source_left.column() == EBSIZE_COL) && (source_right.column() == EBSIZE_COL)) {
        // 这里我们所取得数据是用户源数据
        QVariant leftData = sourceModel()->data(source_left, ROLE_ITEM_FILE_SIZE);
        QVariant rightData = sourceModel()->data(source_right, ROLE_ITEM_FILE_SIZE);

        if (leftData.canConvert<qint64>() && rightData.canConvert<qint64>()) {
            return leftData.toLongLong() < rightData.toLongLong();
        }
    }

    return QSortFilterProxyModel::lessThan(source_left, source_right);
}

bool SortFilterProxyModel::musicLessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    if ((source_left.column() == MSIZE_COL) && (source_right.column() == MSIZE_COL)) {
        // 这里我们所取得数据是用户源数据
        QVariant leftData = sourceModel()->data(source_left, ROLE_ITEM_FILE_SIZE);
        QVariant rightData = sourceModel()->data(source_right, ROLE_ITEM_FILE_SIZE);

        if (leftData.canConvert<qint64>() && rightData.canConvert<qint64>()) {
            return leftData.toLongLong() < rightData.toLongLong();
        }
    }

    return QSortFilterProxyModel::lessThan(source_left, source_right);
}

bool SortFilterProxyModel::videoPhotoLessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    if ((source_left.column() == EBSIZE_COL) && (source_right.column() == EBSIZE_COL)) {
        // 这里我们所取得数据是用户源数据
        QVariant leftData = sourceModel()->data(source_left, ROLE_ITEM_FILE_SIZE);
        QVariant rightData = sourceModel()->data(source_right, ROLE_ITEM_FILE_SIZE);

        if (leftData.canConvert<qint64>() && rightData.canConvert<qint64>()) {
            return leftData.toLongLong() < rightData.toLongLong();
        }
    }

    return QSortFilterProxyModel::lessThan(source_left, source_right);
}
