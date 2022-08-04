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
#ifndef BASETREEVIEW_H
#define BASETREEVIEW_H

#include <DTreeView>
#include <DWidget>

#include "defines.h"
#include "BaseObject.h"

#include "TreeHeaderView.h"

DWIDGET_USE_NAMESPACE

#define CHECK_COL 0 //CheckBox
#define NAME_COL 1 // 文件名
#define EBSIZE_COL 2 // 文件大小
#define EBDATE_COL 3 // 修改时间
#define EBTYPE_COL 4 // 类型

#define FILE_NAME_COLUMN 0 // 文件名
#define FILE_SIZE_COLUMN 1 // 文件大小
#define FILE_DATE_COLUMN 2 // 修改时间
#define FILE_TYPE_COLUMN 3 // 类型

class BaseItemModel;
class BaseTreeViewItemDelegate;
class SortFilterProxyModel;

class BaseTreeView : public DTreeView
    , public BaseObject
{
    Q_OBJECT
public:
    explicit BaseTreeView(const E_Widget_Type &type, DWidget *p = nullptr);
    virtual ~BaseTreeView() override;

signals:
    void sigLoadDataEnd(); //  结束加载数据信号
    void sigLoadDirData(const QString &); //  加载文件夹下面的数据
    void sigUpdateSelectData(const bool &);
    void sigSelectAll(const bool &bSelection);
    void sigSelectChanged(const QModelIndexList &, const QModelIndexList &);

public:
    void setHeaderViewState(const bool &);
    void setHeaderViewEnable(const bool &);
    bool getIsHeaderChecked();

public:
    void setItemSelect(const QString &sPath, const bool &bSelected) override;
    void deleteModelByPath(const QString &, const QString &path) override;
    BaseItemModel *getSourceModel() const override;

    SortFilterProxyModel *getSortFilterProxyModel() const;

    DWidget *getParent() const;

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void slotItemDoubleClick(const QModelIndex &index);
    //    void slotCustomContextMenuRequested(const QPoint &);
    /**
        设置表头 列的宽度
     * @brief updateSectionWidth
     * @param logicalIndex
     * @param newSize
     */
    void updateSectionWidth(int logicalIndex, int /* oldSize */, int /*newSize*/);
    void slotSelectAll(const bool &bSelection);

    void slotSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void slotsetSortingEnabled(const bool &bSort);

protected:
    void setSourceModel();

private:
    /**
     * @brief 每个treeView 都有headerview
     */
    void setHeaderView();

protected:
    SortFilterProxyModel *m_pSortFilterProxyModel = nullptr;
    BaseTreeViewItemDelegate *m_viewDelegate = nullptr;
    TreeHeaderView *m_myHendView = nullptr;

private:
    DWidget *m_pParent = nullptr; //  外面的 margin 10 壳子
};

#endif // CUSTOMTREEVIEW_H
