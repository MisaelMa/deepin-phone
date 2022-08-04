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
#ifndef BASELISTVIEW_H
#define BASELISTVIEW_H

#include <DListView>
#include <DWidget>

#include "BaseObject.h"
#include "BaseItemModel.h"

DWIDGET_USE_NAMESPACE

class BaseListViewItemDelegate;
class BaseListView : public DListView
    , public BaseObject
{
    Q_OBJECT
public:
    explicit BaseListView(const E_Widget_Type &type, DWidget *p = nullptr);
    virtual ~BaseListView() override;

signals:
    void sigLoadDirData(const QString &); //  加载文件夹下面的数据
    // BaseObject interface
public:
    BaseItemModel *getSourceModel() const override;
    void deleteModelByPath(const QString &, const QString &path) override;

public:
    bool isSelectAll();

private slots:
    void slotItemDoubleClicked(const QModelIndex &index);

protected:
    BaseListViewItemDelegate *m_viewDelegate = nullptr;
};

#endif // CUSTOMLISTVIEW_H
