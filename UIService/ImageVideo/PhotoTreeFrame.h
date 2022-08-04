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
#ifndef PHOTOTREEFRAME_H
#define PHOTOTREEFRAME_H

#include "defines.h"

#include "base/BaseTreeView.h"

#include <QItemSelection>

DWIDGET_USE_NAMESPACE

class PhotoTreeFrame : public BaseTreeView
{
    Q_OBJECT
public:
    explicit PhotoTreeFrame(const E_Widget_Type &, DWidget *parent = nullptr);

    //    void addFileInfo(PhoneFileInfo &info);
    //    void clear();
    void delItem(QString strPath); // 根据文件路径 删除item
    void reloadImage(const QString &path, const QPixmap &data);
    //    void setSelect(QString strPath, bool bSelected);
    void setItemSelect(const QString &sPath, const bool &bSelected);
    QStringList getSelectFilePath();
    void setTitleBtnEnble(); //选中切换 按钮改变
    void refreshHeaderChecked(); //刷新表头选中状态
    void selectAllEx(bool bSelected); //

    /**
     * 进入文件夹，重置表头状态
     */
    void resetHeaderState();

signals:
    void sigHeaderSelectAll(const bool &);
    void sigFileSelectChanged(QString strFilePath, bool bSelected);

private:
    void initUI();
    void initConn();

private slots:
    void onTreeSelectChanged(const QModelIndexList &selected, const QModelIndexList &deselected);
};

#endif // PHOTOTREEFRAME_H
