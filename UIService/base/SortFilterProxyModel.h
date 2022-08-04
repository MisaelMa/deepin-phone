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
#ifndef SORTFILTERPROXYMODEL_H
#define SORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

/**
 * @brief 用于 列表排序功能
 */

class SortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit SortFilterProxyModel(QObject *parent = nullptr);
    ~SortFilterProxyModel() override;

public:
    void setType(const int &);

    // QSortFilterProxyModel interface
protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

private:
    bool fileLessThan(const QModelIndex &source_left, const QModelIndex &source_right) const;
    bool appLessThan(const QModelIndex &source_left, const QModelIndex &source_right) const;
    bool eBookLessThan(const QModelIndex &source_left, const QModelIndex &source_right) const;
    bool musicLessThan(const QModelIndex &source_left, const QModelIndex &source_right) const;
    bool videoPhotoLessThan(const QModelIndex &source_left, const QModelIndex &source_right) const;

private:
    int m_nType = -1;
};

#endif // SORTFILTERPROXYMODEL_H
