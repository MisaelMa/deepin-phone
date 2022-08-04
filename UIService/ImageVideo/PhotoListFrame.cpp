/*
* Copyright (C) 2019 ~ 2020 Deepin Technology Co., Ltd.
*
* Author:     yelei <yelei@uniontech.com>
* Maintainer: yelei <yelei@uniontech.com>
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

#include "PhotoListFrame.h"
#include "utils.h"
#include "PhoneListPhotoModel.h"
#include "PhoneListPhotoDelegate.h"

#include <QDebug>

PhotoListFrame::PhotoListFrame(const E_Widget_Type &type, QWidget *parent)
    : BaseListView(type, parent)
{
    setSelectionMode(QAbstractItemView::ExtendedSelection);

    auto model = new PhoneListPhotoModel(this);
    this->setModel(model);

    m_viewDelegate = new PhoneListPhotoDelegate(this);
    this->setItemDelegate(m_viewDelegate);

    initConn();
}

void PhotoListFrame::setItemSize(const QSize &size)
{
    static_cast<PhoneListPhotoDelegate *>(this->itemDelegate())->setItemSize(size);
    this->setIconSize(size);
}

void PhotoListFrame::setIconSize(const QSize &size)
{
    static_cast<PhoneListPhotoDelegate *>(this->itemDelegate())->setIconSize(size);
}

bool PhotoListFrame::isEmpty()
{
    return (this->getSourceModel()->rowCount() == 0);
}

QStringList PhotoListFrame::getSelectFilePath()
{
    QStringList list_retPath;
    auto selectRows = this->selectionModel()->selectedRows();
    for (int i = 0; i < selectRows.count(); i++) {
        list_retPath.append(this->getSourceModel()->index(selectRows.at(i).row(), 0).data(ROLE_ITEM_FILE_PATH).toString());
    }
    return list_retPath;
}

//QList<PhoneFileInfo> PhotoListFrame::getSelectFile()
//{
//    QList<PhoneFileInfo> list_retFile;
//    auto selectRows = this->selectionModel()->selectedRows();
//    for (int i = 0; i < selectRows.count(); i++) {
//        list_retFile.append(this->getSourceModel()->index(selectRows.at(i).row(), 0).data(ROLE_ITEM_DATA).value<PhoneFileInfo>());
//    }
//    return list_retFile;
//}

QList<PhoneFileInfo> PhotoListFrame::getAllFile()
{
    QList<PhoneFileInfo> list_retFile;
    BaseItemModel *model = this->getSourceModel();
    for (int i = 0; i < model->rowCount(); i++) {
        QModelIndex index = model->index(i, 0);
        list_retFile.append(model->data(index, ROLE_ITEM_DATA).value<PhoneFileInfo>());
    }
    return list_retFile;
}

quint64 PhotoListFrame::getSelectSize()
{
    quint64 selectSize = 0;

    auto selectRows = this->selectionModel()->selectedRows();
    foreach (auto item, selectRows) {
        selectSize += item.data(ROLE_ITEM_FILE_SIZE).value<quint64>();
    }

    return selectSize;
}

void PhotoListFrame::selectAllEx(bool bSelected)
{
    this->blockSignals(true);

    if (bSelected == true) {
        this->selectAll();
    } else {
        this->clearSelection();
    }

    this->blockSignals(false);
}

void PhotoListFrame::initConn()
{
    connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, this, &PhotoListFrame::slotSelectionChanged);
}

void PhotoListFrame::slotSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    if (this->isVisible() == false)
        return;

    foreach (QModelIndex itemIdx, selected.indexes()) {
        emit sigFileSelectChanged(this->getSourceModel()->index(itemIdx.row(), 0).data(ROLE_ITEM_FILE_PATH).toString(), true);
    }
    foreach (QModelIndex itemIdx, deselected.indexes()) {
        emit sigFileSelectChanged(this->getSourceModel()->index(itemIdx.row(), 0).data(ROLE_ITEM_FILE_PATH).toString(), false);
    }
}

void PhotoListFrame::setItemSelect(const QString &sPath, const bool &bSelected)
{
    QItemSelectionModel *selectionModel = this->selectionModel();

    BaseItemModel *model = getSourceModel();
    for (int i = 0; i < model->rowCount(); ++i) {
        QModelIndex index = model->index(i, 0);
        QString strItemPath = model->data(index, ROLE_ITEM_FILE_PATH).toString();
        if (strItemPath == sPath) {
            QItemSelection itemSelection(index, index);

            this->selectionModel()->blockSignals(true);

            if (bSelected)
                selectionModel->select(itemSelection, QItemSelectionModel::Select);
            else
                selectionModel->select(itemSelection, QItemSelectionModel::Deselect);

            this->selectionModel()->blockSignals(false);
            break;
        }
    }
}
