/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co., Ltd.
 *
 * Author:     huangyonga<huangyonga@uniontech.com>
 * Maintainer: huangyonga<huangyonga@uniontech.com>
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
#ifndef BASETREEVIEWITEMDELEGATE_H
#define BASETREEVIEWITEMDELEGATE_H

#include <DStyledItemDelegate>
#include "BaseTreeView.h"
#include <QPainterPath>

class QModelIndex;
class QPainter;
class QStyleOptionViewItem;

DWIDGET_USE_NAMESPACE

class BaseTreeViewItemDelegate : public DStyledItemDelegate
{
    Q_OBJECT
public:
    explicit BaseTreeViewItemDelegate(QAbstractItemView *parent = nullptr);
    virtual ~BaseTreeViewItemDelegate() override;

public:
    void setTreeType(const E_Widget_Type &);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

protected:
    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const override;

private:
    void drawPixmapText(QPainter *painter, const QStyleOptionViewItem &option,
                        const QModelIndex &index) const;

    void drawCheckBox(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void drawItemText(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void drawBkgColor(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QPainterPath getBkPath(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

protected:
    BaseTreeView *m_pItemView = nullptr;

    //    bool m_bIsCanExport = false;
    E_Widget_Type m_treeType = E_No_Define;
};

#endif // TREEVIEWITEMDELEGATE_H
