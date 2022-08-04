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
#include "UIModelManager.h"

UIModelManager::UIModelManager(QObject *parent)
    : QObject(parent)
{
}

void UIModelManager::setUIModel(BaseItemModel *treeModel, BaseItemModel *listModel)
{
    m_pTreeModel = treeModel;
    m_pListModel = listModel;
}

BaseItemModel *UIModelManager::getTreeModel() const
{
    return m_pTreeModel;
}

BaseItemModel *UIModelManager::getListModel() const
{
    return m_pListModel;
}

void UIModelManager::appendData(const QVariant &data)
{
    if (m_pListModel) {
        m_pListModel->appendData(data);
    }

    if (m_pTreeModel) {
        m_pTreeModel->appendData(data);
    }
}

void UIModelManager::updateData(const QString &s, const QVariant &d)
{
    if (m_pListModel) {
        m_pListModel->updateData(s, d);
    }

    if (m_pTreeModel) {
        m_pTreeModel->updateData(s, d);
    }
}

void UIModelManager::updateDataPixmap(const QJsonObject &obj, const QPixmap &p)
{
    if (m_pListModel) {
        m_pListModel->updateDataPixmap(obj, p);
    }

    if (m_pTreeModel) {
        m_pTreeModel->updateDataPixmap(obj, p);
    }
}

void UIModelManager::clearModel()
{
    if (m_pListModel) {
        m_pListModel->clear();
        m_pListModel->clearTotalSize();
    }

    if (m_pTreeModel != nullptr) {
        if (m_pTreeModel->hasChildren()) {
            m_pTreeModel->removeRows(0, m_pTreeModel->rowCount());
            m_pTreeModel->clearTotalSize();
        }
    }
}

void UIModelManager::deleteModelByPath(const QString &, const QString &path)
{
    if (m_pListModel) {
        m_pListModel->deleteRowByPath("", path);
    }

    if (m_pTreeModel) {
        m_pTreeModel->deleteRowByPath("", path);
    }
}
