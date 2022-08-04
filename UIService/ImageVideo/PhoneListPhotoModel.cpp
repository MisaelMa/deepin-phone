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
#include "PhoneListPhotoModel.h"

PhoneListPhotoModel::PhoneListPhotoModel(QObject *parent)
    : BaseItemModel(parent)
{
}

void PhoneListPhotoModel::updateDataPixmap(const QJsonObject &, const QPixmap &)
{
}

void PhoneListPhotoModel::appendData(const QVariant &data)
{
    auto info = data.value<PhoneFileInfo>();

    QStandardItem *item = new QStandardItem();
    //    item->setText(info.name.section('.', 0, -2));
    //    item->setIcon(info.scaleIcon);
    item->setData(info.size, ROLE_ITEM_FILE_SIZE);
    item->setData(data, ROLE_ITEM_DATA);
    if (info.bIsDir) {
        item->setData(info.dirPath, ROLE_ITEM_FILE_PATH);
    } else {
        item->setData(info.path, ROLE_ITEM_FILE_PATH);
    }

    this->appendRow(item);

    m_nTotalSize += info.size;
}

void PhoneListPhotoModel::deleteRowByPath(const QString &, const QString &strPath)
{
    for (int i = 0; i < this->rowCount(); ++i) {
        QModelIndex index = this->index(i, 0);
        QString strItemPath = this->data(index, ROLE_ITEM_FILE_PATH).toString();
        if (strItemPath == strPath) {
            m_nTotalSize -= this->data(index, ROLE_ITEM_FILE_SIZE).toInt();

            this->removeRow(index.row());

            break;
        }
    }
}

void PhoneListPhotoModel::updateData(const QString &, const QVariant &)
{
}

qint64 PhoneListPhotoModel::getTotalSize()
{
    return m_nTotalSize;
}
