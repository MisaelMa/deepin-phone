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
#include "AppTreeView.h"

#include "AppTreeViewItemDelegate.h"
#include "PhoneTreeAppModel.h"

AppTreeView::AppTreeView(DWidget *p)
    : BaseTreeView(E_Widget_App, p)
{
    setHeaderViewEnable(false);
    header()->setSortIndicatorShown(false);

    m_pSourceModel = new PhoneTreeAppModel(this);
    setSourceModel();

    m_viewDelegate = new AppTreeViewItemDelegate(this);
    this->setItemDelegate(m_viewDelegate);

    connect(static_cast<AppTreeViewItemDelegate *>(m_viewDelegate), &AppTreeViewItemDelegate::sigDeleteData, this, &AppTreeView::sigDeleteData);
    connect(static_cast<AppTreeViewItemDelegate *>(m_viewDelegate), &AppTreeViewItemDelegate::sigExportData, this, &AppTreeView::sigExportData);
}

void AppTreeView::setIsCanExport(const bool &rl)
{
    if (m_viewDelegate) {
        static_cast<AppTreeViewItemDelegate *>(m_viewDelegate)->setIsCanExport(rl);
    }
}
