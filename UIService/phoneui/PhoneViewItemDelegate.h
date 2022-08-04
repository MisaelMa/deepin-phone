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
#ifndef PHONEVIEWITEMDELEGATE_H
#define PHONEVIEWITEMDELEGATE_H

#include <DStyledItemDelegate>

DWIDGET_USE_NAMESPACE

class PhoneListView;

class PhoneViewItemDelegate : public DStyledItemDelegate
{
    Q_OBJECT
public:
    explicit PhoneViewItemDelegate(QAbstractItemView *parent = nullptr);
    ~PhoneViewItemDelegate() override;

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                     const QModelIndex &index) override;
    //signals:
    //    void itemDisconn(QModelIndex idx);

private:
    void paint_newStyle(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    void drawTreeIcon(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void drawTreeTitle(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index, const int &) const;

private:
    //    QMap<QModelIndex, QStyleOptionButton *> m_btns;
    //    QModelIndex m_lastIdx;
    PhoneListView *m_view;
};

#endif // PHONEVIEWITEMDELEGATE_H
