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
#include "MusicBookTreeView.h"

#include "PhoneTreeEBookModel.h"
#include "PhoneTreeMusicModel.h"

#include "base/BaseTreeViewItemDelegate.h"

MusicBookTreeView::MusicBookTreeView(const E_Widget_Type &type, DWidget *parent)
    : BaseTreeView(type, parent)
{
    if (type == E_Widget_Book) {
        m_pSourceModel = new PhoneTreeEBookModel(this);
    } else {
        m_pSourceModel = new PhoneTreeMusicModel(this);
    }

    setSourceModel();

    m_viewDelegate = new BaseTreeViewItemDelegate(this);
    m_viewDelegate->setTreeType(type);
    this->setItemDelegate(m_viewDelegate);
}

quint64 MusicBookTreeView::getSelectSize(int &selectCount)
{
    quint64 selectSize = 0;

    auto selectRows = this->selectionModel()->selectedRows(0);
    foreach (auto item, selectRows) {
        selectSize += item.data(ROLE_ITEM_FILE_SIZE).value<quint64>();
    }

    selectCount = selectRows.size();

    return selectSize;
}
