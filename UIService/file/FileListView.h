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
#ifndef FILELISTVIEW_H
#define FILELISTVIEW_H

#include "base/BaseListView.h"

/**
 * @brief 文件 网格view
 */

class FileListView : public BaseListView
{
    Q_OBJECT
public:
    explicit FileListView(DWidget *p = nullptr);

signals:
    void sigCleanSelectItem();
    void sigFileNewFolder(const QString &);
    void sigItemRefresh();
    // BaseObject interface
public:
    void setItemSelect(const QString &sPath, const bool &bSelected) override;

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // FILELISTVIEW_H
