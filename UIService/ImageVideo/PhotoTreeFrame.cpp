/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co., Ltd.
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
#include "PhotoTreeFrame.h"

#include <DFrame>
#include <DStandardItem>
#include <QFileInfo>
#include <QHeaderView>
#include <DFileIconProvider>
#include <QDebug>

#include "widget/TitleWidget.h"
#include "TrObject.h"
#include "utils.h"
#include "Model.h"

#include "base/BaseTreeView.h"
#include "base/SortFilterProxyModel.h"
#include "PhoneTreePhotoModel.h"
#include "base/BaseTreeViewItemDelegate.h"

PhotoTreeFrame::PhotoTreeFrame(const E_Widget_Type &type, DWidget *parent)
    : BaseTreeView(type, parent)
{
    m_pSourceModel = new PhoneTreePhotoModel(this);
    setSourceModel();

    m_viewDelegate = new BaseTreeViewItemDelegate(this);
    m_viewDelegate->setTreeType(type);
    this->setItemDelegate(m_viewDelegate);

    initUI();
    initConn();
}

void PhotoTreeFrame::delItem(QString strPath)
{
    auto model = getSourceModel();
    if (model) {
        model->deleteRowByPath("", strPath);
    }
    //    for (int i = 0; i < sortModel->rowCount(); ++i) {
    //        QModelIndex index = sortModel->index(i, 1);
    //        QString strItemPath = sortModel->data(index, ROLE_ITEM_FILE_PATH).toString();
    //        if (strItemPath == strPath) {
    //            sortModel->removeRow(index.row());
    //            break;
    //        }
    //    }
}

void PhotoTreeFrame::reloadImage(const QString &path, const QPixmap &data)
{
    if (data.isNull())
        return;

    auto sortModel = getSortFilterProxyModel();

    for (int i = 0; i < sortModel->rowCount(); ++i) {
        QModelIndex index = sortModel->index(i, 1);
        QString strItemPath = sortModel->data(index, ROLE_ITEM_FILE_PATH).toString();
        if (strItemPath == path) {
            BaseItemModel *model = getSourceModel();
            model->itemFromIndex(sortModel->mapToSource(index))->setIcon(data);
            //                        m_pSortFilterProxyModel->setData(index, data, Qt::DecorationRole);
            break;
        }
    }
}

void PhotoTreeFrame::setItemSelect(const QString &sPath, const bool &bSelected)
{
    auto sortModel = getSortFilterProxyModel();

    BaseItemModel *model = getSourceModel();
    for (int i = 0; i < model->rowCount(); ++i) {
        QModelIndex index = model->index(i, 1);
        QString itemPath = model->data(index, ROLE_ITEM_FILE_PATH).toString();
        if (itemPath == sPath) {
            int iRow = index.row();

            QModelIndex headModelIndex = model->index(iRow, 0);
            QModelIndex tailModelIndex = model->index(iRow, this->model()->columnCount() - 1);

            //  转换为 排序之后的 索引
            headModelIndex = sortModel->mapFromSource(headModelIndex);
            tailModelIndex = sortModel->mapFromSource(tailModelIndex);

            QItemSelection itemSelection(headModelIndex, tailModelIndex);

            this->selectionModel()->blockSignals(true);

            if (bSelected)
                this->selectionModel()->select(itemSelection, QItemSelectionModel::Select);
            else
                this->selectionModel()->select(itemSelection, QItemSelectionModel::Deselect);

            this->selectionModel()->blockSignals(false);

            break;
        }
    }

    refreshHeaderChecked();
}

QStringList PhotoTreeFrame::getSelectFilePath()
{
    auto sortModel = getSortFilterProxyModel();

    QStringList list_retPath;

    auto selectRows = selectionModel()->selectedRows();
    for (int i = 0; i < selectRows.count(); i++) {
        list_retPath.append(sortModel->index(selectRows.at(i).row(), 1).data(ROLE_ITEM_FILE_PATH).toString());
    }
    return list_retPath;
}

void PhotoTreeFrame::setTitleBtnEnble()
{
    bool rl = selectionModel()->hasSelection();
    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Delete_Btn, rl);
    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Export_Btn, rl);
}

void PhotoTreeFrame::refreshHeaderChecked()
{
    auto selectRows = selectionModel()->selectedRows();
    auto rowCount = model()->rowCount();

    setHeaderViewState(selectRows.count() == rowCount);
}

void PhotoTreeFrame::selectAllEx(bool bSelected)
{
    selectionModel()->blockSignals(true);

    if (bSelected == true) {
        selectAll();
    } else {
        clearSelection();
    }

    selectionModel()->blockSignals(false);

    //    setTitleBtnEnble();// grid里有判断
    setHeaderViewState(bSelected);
}

void PhotoTreeFrame::resetHeaderState()
{
    setHeaderViewState(false);
}

void PhotoTreeFrame::initUI()
{
    setColumnWidth(0, 50);
    setColumnWidth(1, 350);
    setColumnWidth(3, 150);

    header()->setSortIndicator(3, Qt::DescendingOrder);
    header()->setSortIndicatorShown(false);
}

void PhotoTreeFrame::initConn()
{
    connect(this, &BaseTreeView::sigSelectChanged, this, &PhotoTreeFrame::onTreeSelectChanged);
}

void PhotoTreeFrame::onTreeSelectChanged(const QModelIndexList &selected, const QModelIndexList &deselected)
{
    if (this->isVisible() == false)
        return;

    foreach (QModelIndex itemIdx, selected) {
        if (itemIdx.column() != 0)
            continue;
        emit sigFileSelectChanged(getSortFilterProxyModel()->index(itemIdx.row(), 1).data(ROLE_ITEM_FILE_PATH).toString(), true);
    }
    foreach (QModelIndex itemIdx, deselected) {
        if (itemIdx.column() != 0)
            continue;
        emit sigFileSelectChanged(getSortFilterProxyModel()->index(itemIdx.row(), 1).data(ROLE_ITEM_FILE_PATH).toString(), false);
    }

    //    setTitleBtnEnble(); // 统一由PhotoVideoWidget判断
    refreshHeaderChecked();
}
