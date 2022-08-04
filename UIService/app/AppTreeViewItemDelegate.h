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
#ifndef APPTREEVIEWITEMDELEGATE_H
#define APPTREEVIEWITEMDELEGATE_H

#include "base/BaseTreeViewItemDelegate.h"

class AppTreeViewItemDelegate : public BaseTreeViewItemDelegate
{
    Q_OBJECT
public:
    explicit AppTreeViewItemDelegate(QAbstractItemView *parent = nullptr);

signals:
    void sigDeleteData(const QModelIndex &index); // add by Airy
    void sigExportData(const QModelIndex &index); // add by Airy

public:
    void setIsCanExport(const bool &);

    // QAbstractItemDelegate interface
public:
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

private:
    void drawItemDeleteBtn(QPainter *painter, const QStyleOptionViewItem &option,
                           const QModelIndex &index) const;

private:
    bool m_bIsCanExport = false;
};

#endif // APPTREEVIEWITEMDELEGATE_H
