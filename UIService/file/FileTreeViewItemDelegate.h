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
#ifndef FILETREEVIEWITEMDELEGATE_H
#define FILETREEVIEWITEMDELEGATE_H

#include "base/BaseTreeViewItemDelegate.h"

/**
 * @brief 文管的列表 代理绘制
 */

class FileTreeViewItemDelegate : public BaseTreeViewItemDelegate
{
    Q_OBJECT
public:
    explicit FileTreeViewItemDelegate(QAbstractItemView *parent = nullptr);

signals:
    /**
     * @brief 新增文件夹信号
     */
    void sigFileNewFolder(const QString &);

    // QAbstractItemDelegate interface
public:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private slots:
    void onEditWidgetFocusOut();
};

#endif // FILETREEVIEWITEMDELEGATE_H
