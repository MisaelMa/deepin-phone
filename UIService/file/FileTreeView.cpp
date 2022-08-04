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
#include "FileTreeView.h"

#include <QMouseEvent>

#include "FileTreeViewItemDelegate.h"
#include "PhoneTreeFileModel.h"

FileTreeView::FileTreeView(DWidget *p)
    : BaseTreeView(E_Widget_File, p)
{
    m_pSourceModel = new PhoneTreeFileModel(this);
    setSourceModel();

    m_viewDelegate = new FileTreeViewItemDelegate(this);
    connect(static_cast<FileTreeViewItemDelegate *>(m_viewDelegate), &FileTreeViewItemDelegate::sigFileNewFolder, this, &FileTreeView::sigFileNewFolder);

    this->setItemDelegate(m_viewDelegate);
}

//  单击事件
void FileTreeView::mousePressEvent(QMouseEvent *event)
{
    if (m_nWidgetType == E_Widget_File) {
        auto index = this->indexAt(event->pos());
        if (index.row() == -1) {
            //  空白区域， 清除选中
            this->selectionModel()->clearSelection();
        }
    }

    DTreeView::mousePressEvent(event);
}
