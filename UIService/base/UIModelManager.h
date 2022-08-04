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
#ifndef UIMODELMANAGER_H
#define UIMODELMANAGER_H

#include <QObject>

#include "BaseItemModel.h"

/**
 * @brief 只有一个model 数据管理, widget 中只需要调用接口即可
 */

class UIModelManager : public QObject
{
    Q_OBJECT
public:
    explicit UIModelManager(QObject *parent = nullptr);

public:
    void setUIModel(BaseItemModel *, BaseItemModel *);
    BaseItemModel *getTreeModel() const;
    BaseItemModel *getListModel() const;

    void appendData(const QVariant &);
    void updateData(const QString &, const QVariant &);
    void updateDataPixmap(const QJsonObject &, const QPixmap &);

    void clearModel();
    void deleteModelByPath(const QString &, const QString &path);

public:
    BaseItemModel *m_pTreeModel = nullptr;
    BaseItemModel *m_pListModel = nullptr;
};

#endif // UIMODELMANAGER_H
