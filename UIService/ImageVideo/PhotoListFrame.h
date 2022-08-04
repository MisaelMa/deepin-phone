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
#ifndef PHOTOLISTFRAME_H
#define PHOTOLISTFRAME_H

#include "base/BaseListView.h"

DWIDGET_USE_NAMESPACE

class PhotoListFrame : public BaseListView
{
    Q_OBJECT
public:
    explicit PhotoListFrame(const E_Widget_Type &type, QWidget *parent = nullptr);

    void setItemSize(const QSize &size);
    void setIconSize(const QSize &size);

    bool isEmpty();

    // 获取选中文件
    QStringList getSelectFilePath();
    //    QList<PhoneFileInfo> getSelectFile();
    QList<PhoneFileInfo> getAllFile();

    quint64 getSelectSize();

    // BaseObject interface
public:
    void setItemSelect(const QString &sPath, const bool &bSelected) override;

public slots:
    // 全选
    void selectAllEx(bool bSelected);

signals:
    void sigFileSelectChanged(QString strFilePath, bool bSelected);

private:
    void initConn();

private slots:
    void slotSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
};

#endif // PHOTOLISTFRAME_H
