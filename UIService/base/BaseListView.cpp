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
#include "BaseListView.h"

#include <QDebug>

#include "utils.h"

BaseListView::BaseListView(const E_Widget_Type &type, DWidget *p)
    : DListView(p)
    , BaseObject(type)
{
    setResizeMode(QListView::Adjust);
    setViewMode(QListView::IconMode);
    setMovement(QListView::Static);
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setFrameStyle(QFrame::NoFrame);

    connect(this, &BaseListView::doubleClicked, this, &BaseListView::slotItemDoubleClicked);
}

BaseListView::~BaseListView()
{
}

BaseItemModel *BaseListView::getSourceModel() const
{
    return static_cast<BaseItemModel *>(this->model());
}

void BaseListView::deleteModelByPath(const QString &name, const QString &path)
{
    auto model = static_cast<BaseItemModel *>(this->model());
    if (model) {
        model->deleteRowByPath(name, path);

        this->reset();
    }
}

bool BaseListView::isSelectAll()
{
    auto selectRows = this->selectionModel()->selectedRows();
    auto rowCount = this->model()->rowCount();

    return (selectRows.count() == rowCount && rowCount != 0);
}

void BaseListView::slotItemDoubleClicked(const QModelIndex &index)
{
    if (m_nWidgetType == E_Widget_File) {
        QFileInfo fi = index.data(ROLE_ITEM_DATA).value<QFileInfo>();
        if (fi.isDir()) {
            emit sigLoadDirData(fi.absoluteFilePath());
        } else {
            //pms47535文管中新建文件夹，连续三击该文件夹会唤出文件管理器
            //在新建文件夹时，快速连续多次单击该文件夹时，会执行到该双击动作，此时可能文件夹还还真正创建，在判断文件类型时会判断为文件
            //如果是正在新建文件夹时，则忽略
            bool isNewDir = index.data(ROLE_NEW_DIR).toBool();
            if (!isNewDir) //文件，执行打开
                Utils::execOpenPath(fi.absoluteFilePath());
        }
    } else if (m_nWidgetType == E_Widget_Photo || m_nWidgetType == E_Widget_Video) {
        PhoneFileInfo info = index.data(ROLE_ITEM_DATA).value<PhoneFileInfo>();
        if (info.bIsDir) {
            emit sigLoadDirData(info.dirPath);
        } else {
            if (info.type == VIDEO) { // 视频可以打开， 暂不做图片打开
                Utils::execOpenPath(info.path);
            }
        }
    }
}
