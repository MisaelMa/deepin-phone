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
#include "BaseTreeView.h"

#include <DApplication>
#include <DApplicationHelper>
#include <DStyle>
#include <QVBoxLayout>
#include <QPainterPath>

#include "TrObject.h"
#include "SortFilterProxyModel.h"
#include "BaseItemModel.h"
#include "utils.h"

BaseTreeView::BaseTreeView(const E_Widget_Type &type, DWidget *p)
    : DTreeView(p)
    , BaseObject(type)
{
    m_pParent = new DWidget(p);

    setParent(m_pParent);

    auto topLayout = new QVBoxLayout(m_pParent);
    topLayout->setContentsMargins(10, 10, 10, 10);
    topLayout->setSpacing(0);

    topLayout->addWidget(this);

    setSortingEnabled(true);

    setEditTriggers(QAbstractItemView::NoEditTriggers); //设置项不可编辑
    setRootIsDecorated(false);
    setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerItem);
    setSelectionMode(QAbstractItemView::MultiSelection); //  modify by wzx, pms 36239
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setItemsExpandable(false);
    setFrameStyle(QFrame::NoFrame);

    viewport()->setAutoFillBackground(false);

    setHeaderView();

    connect(this, &BaseTreeView::doubleClicked, this, &BaseTreeView::slotItemDoubleClick);
}

BaseTreeView::~BaseTreeView()
{
}

void BaseTreeView::setHeaderView()
{
    m_myHendView = new TreeHeaderView(0, Qt::Horizontal, this);
    m_myHendView->setTreeType(m_nWidgetType);
    connect(m_myHendView, &TreeHeaderView::sectionResized, this, &BaseTreeView::updateSectionWidth);
    connect(m_myHendView, &TreeHeaderView::checkStausChanged, this, &BaseTreeView::slotSelectAll);
    connect(m_myHendView, &TreeHeaderView::sigSortIndicatorShown, this, &BaseTreeView::slotsetSortingEnabled);
    this->setHeader(m_myHendView);
}

void BaseTreeView::setHeaderViewState(const bool &bCheck)
{
    if (m_myHendView) {
        m_myHendView->slotSetChecked(bCheck);
    }
}

void BaseTreeView::setHeaderViewEnable(const bool &bCheck)
{
    if (m_myHendView) {
        m_myHendView->setEnabled(bCheck);
    }
}

bool BaseTreeView::getIsHeaderChecked()
{
    if (m_myHendView) {
        return m_myHendView->getIsChecked();
    }
    return false;
}

void BaseTreeView::setSourceModel()
{
    if (m_pSourceModel) {
        connect(m_pSourceModel, &BaseItemModel::sigLoadDataEnd, this, &BaseTreeView::sigLoadDataEnd); //  开始加载数据信号

        m_pSortFilterProxyModel = new SortFilterProxyModel(this);
        m_pSortFilterProxyModel->setType(m_nWidgetType);
        m_pSortFilterProxyModel->setSourceModel(m_pSourceModel);
        setModel(m_pSortFilterProxyModel);

        connect(this->selectionModel(), &QItemSelectionModel::selectionChanged, this, &BaseTreeView::slotSelectionChanged);
    }
}

DWidget *BaseTreeView::getParent() const
{
    return m_pParent;
}

void BaseTreeView::deleteModelByPath(const QString &, const QString &path)
{
    for (auto i = 0; i < m_pSortFilterProxyModel->rowCount(); i++) {
        auto info = m_pSortFilterProxyModel->index(i, 0).data(ROLE_ITEM_DATA).value<QFileInfo>();
        if (info.absoluteFilePath().contains('/') && info.absoluteFilePath().compare(path) == 0) {
            m_pSortFilterProxyModel->removeRow(i);
            this->reset();
            break;
        }
    }
}

void BaseTreeView::slotItemDoubleClick(const QModelIndex &index)
{
    QModelIndex nameInx = index;

    if (m_nWidgetType == E_Widget_Book || m_nWidgetType == E_Widget_Music) {
        if (nameInx.column() == 0)
            return;

        if (nameInx.column() != NAME_COL) {
            auto model = static_cast<BaseItemModel *>(m_pSortFilterProxyModel->sourceModel());

            nameInx = model->index(index.row(), NAME_COL);
        }
    } else if (m_nWidgetType == E_Widget_File) {
        if (nameInx.column() != FILE_NAME_COLUMN) {
            auto model = static_cast<BaseItemModel *>(m_pSortFilterProxyModel->sourceModel());

            nameInx = model->index(nameInx.row(), FILE_NAME_COLUMN);
        }
    }

    QFileInfo fi = nameInx.data(ROLE_ITEM_DATA).value<QFileInfo>();
    if (fi.isDir()) {
        emit sigLoadDirData(fi.absoluteFilePath());
    } else {
        //pms47535文管中新建文件夹，连续三击该文件夹会唤出文件管理器
        //在新建文件夹时，快速连续多次单击该文件夹时，会执行到该双击动作，此时可能文件夹还还真正创建，在判断文件类型时会判断为文件
        //如果是正在新建文件夹时，则忽略
        bool isNewDir = nameInx.data(ROLE_NEW_DIR).toBool();
        if (!isNewDir) //文件，执行打开
            Utils::execOpenPath(fi.absoluteFilePath());
    }
}

void BaseTreeView::updateSectionWidth(int logicalIndex, int, int)
{
    if (m_nWidgetType == E_Widget_File)
        return;

    if (logicalIndex == 0) {
        this->setColumnWidth(0, 50);
    }
}

//全选操作
void BaseTreeView::slotSelectAll(const bool &bSelection)
{
    this->blockSignals(true);

    if (bSelection == true) {
        this->selectAll();
    } else {
        this->clearSelection();
    }

    this->blockSignals(false);

    emit sigSelectAll(bSelection);
}

// modify by wzx, 2020-07-04 pms 36168
void BaseTreeView::slotSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);

    auto selectRows = this->selectionModel()->selectedRows();
    auto rowCount = this->model()->rowCount();
    if (selectRows.count() == rowCount) {
        m_myHendView->slotSetChecked(true);
    } else {
        m_myHendView->slotSetChecked(false);
    }

    emit sigSelectChanged(selected.indexes(), deselected.indexes());

    bool rl = this->selectionModel()->hasSelection();
    emit sigUpdateSelectData(rl);
}

void BaseTreeView::slotsetSortingEnabled(const bool &bSort)
{
    setSortingEnabled(bSort);
}

SortFilterProxyModel *BaseTreeView::getSortFilterProxyModel() const
{
    return m_pSortFilterProxyModel;
}

void BaseTreeView::setItemSelect(const QString &sPath, const bool &bSelected)
{
    QItemSelectionModel *selectionModel = this->selectionModel();
    this->clearSelection();

    auto model = static_cast<BaseItemModel *>(m_pSortFilterProxyModel->sourceModel());

    QFileInfo file(sPath);

    auto items = model->findItems(file.fileName(), Qt::MatchExactly, 0);
    foreach (auto item, items) {
        QFileInfo info = item->data(ROLE_ITEM_DATA).value<QFileInfo>();
        QString itemPath = info.absoluteFilePath();
        if (itemPath == sPath) {
            int iRow = item->row();

            QModelIndex headModelIndex = model->index(iRow, 0);
            QModelIndex tailModelIndex = model->index(iRow, this->model()->columnCount() - 1);

            //  转换为 排序之后的 索引
            headModelIndex = m_pSortFilterProxyModel->mapFromSource(headModelIndex);
            tailModelIndex = m_pSortFilterProxyModel->mapFromSource(tailModelIndex);

            QItemSelection itemSelection(headModelIndex, tailModelIndex);

            this->selectionModel()->blockSignals(true);

            if (bSelected)
                selectionModel->select(itemSelection, QItemSelectionModel::Select);
            else
                selectionModel->select(itemSelection, QItemSelectionModel::Deselect);

            this->selectionModel()->blockSignals(false);
            this->setCurrentIndex(headModelIndex);

            break;
        }
    }
}

BaseItemModel *BaseTreeView::getSourceModel() const
{
    return static_cast<BaseItemModel *>(this->getSortFilterProxyModel()->sourceModel());
}

void BaseTreeView::paintEvent(QPaintEvent *event)
{
    QPainter painter(viewport());
    painter.save();
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setOpacity(1);
    painter.setClipping(true);

    QWidget *wnd = DApplication::activeWindow();
    DPalette::ColorGroup cg;
    if (!wnd) {
        cg = DPalette::Inactive;
    } else {
        cg = DPalette::Active;
    }

    auto style = dynamic_cast<DStyle *>(DApplication::style());
    auto *dAppHelper = DApplicationHelper::instance();
    auto palette = dAppHelper->applicationPalette();

    QBrush bgBrush(palette.color(cg, DPalette::Base));

    QStyleOptionFrame option;
    initStyleOption(&option);
    int radius = style->pixelMetric(DStyle::PM_FrameRadius, &option);

    QRect rect = viewport()->rect();
    QRectF clipRect(rect.x(), rect.y() - rect.height(), rect.width(), rect.height() * 2);
    QRectF subRect(rect.x(), rect.y() - rect.height(), rect.width(), rect.height());
    QPainterPath clipPath, subPath;
    clipPath.addRoundedRect(clipRect, radius, radius);
    subPath.addRect(subRect);
    clipPath = clipPath.subtracted(subPath);

    painter.fillPath(clipPath, bgBrush);

    painter.restore();
    DTreeView::paintEvent(event);
}
