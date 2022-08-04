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
#ifndef BASELISTVIEWITEMDELEGATE_H
#define BASELISTVIEWITEMDELEGATE_H

#include <DStyledItemDelegate>
#include "BaseListView.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief 网格 状态下的 item 绘制
 */

class BaseListViewItemDelegate : public DStyledItemDelegate
{
    Q_OBJECT
public:
    explicit BaseListViewItemDelegate(QAbstractItemView *parent = nullptr);

private:
    BaseListView *m_pListView = nullptr;
};

#endif // LISTVIEWITEMDELEGATE_H
