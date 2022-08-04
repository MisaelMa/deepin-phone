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
#include "FileListView.h"

#include <QMouseEvent>

#include "PhoneIconFileModel.h"
#include "fileiconitemdelegate.h"

FileListView::FileListView(DWidget *parent)
    : BaseListView(E_Widget_File, parent)
{
    setSpacing(0); //文件图标展示不设间距

    setWordWrap(true);

    auto model = new PhoneIconFileModel(this);
    this->setModel(model);

    m_viewDelegate = new FileIconItemDelegate(this);
    connect(static_cast<FileIconItemDelegate *>(m_viewDelegate), &FileIconItemDelegate::sigFileNewFolder, this, &FileListView::sigFileNewFolder);
    connect(static_cast<FileIconItemDelegate *>(m_viewDelegate), &FileIconItemDelegate::sigItemRefresh, this, &FileListView::sigItemRefresh);

    this->setItemDelegate(m_viewDelegate);
}

void FileListView::mousePressEvent(QMouseEvent *event)
{
    // 点击空白处时取消所有已选择项
    auto index = this->indexAt(event->pos());
    if (index.row() == -1) {
        setCurrentIndex(index);

        emit sigCleanSelectItem();
    } else {
        DListView::mousePressEvent(event);
    }
}

void FileListView::setItemSelect(const QString &sPath, const bool &)
{
    QFileInfo fileInfo(sPath);
    auto model = getSourceModel();
    if (model != nullptr) {
        auto items = model->findItems(fileInfo.fileName());
        foreach (auto item, items) {
            auto info = item->data(ROLE_ITEM_DATA).value<QFileInfo>();
            if (info.absoluteFilePath() == sPath) {
                this->setCurrentIndex(item->index());
                break;
            }
        }
    }
}
