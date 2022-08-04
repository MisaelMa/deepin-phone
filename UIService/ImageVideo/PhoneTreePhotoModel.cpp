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
#include "PhoneTreePhotoModel.h"

#include <QFileInfo>
#include <DFileIconProvider>

#include "TrObject.h"

DWIDGET_USE_NAMESPACE

PhoneTreePhotoModel::PhoneTreePhotoModel(QObject *parent)
    : BaseItemModel(parent)
{
    setHorizontalHeaderLabels(QStringList()
                              << "" << TrObject::getInstance()->getTreeHeaderText(Header_Name)
                              << TrObject::getInstance()->getTreeHeaderText(Header_Size)
                              << TrObject::getInstance()->getTreeHeaderText(Header_Time_modified)
                              << TrObject::getInstance()->getTreeHeaderText(Header_Format));
}

void PhoneTreePhotoModel::appendData(const QVariant &data)
{
    auto info = data.value<PhoneFileInfo>();

    // filter
    if (info.bIsDir) {
        return;
    }

    int nRow = this->rowCount();
    QStandardItem *item = nullptr;
    item = new QStandardItem("");
    this->setItem(nRow, 0, item);

    QFileInfo fileInfo(info.path);

    item = new QStandardItem(info.name.section('.', 0, -2));
    if (info.damaged) {
        DFileIconProvider fip;
        QIcon icon = fip.icon(fileInfo);
        item->setIcon(icon);
    } else {
        item->setIcon(info.scaleIcon);
    }
    item->setData(info.path, ROLE_ITEM_FILE_PATH);
    this->setItem(nRow, 1, item);

    item = new QStandardItem(dataSizeToString(info.size));
    item->setData(info.size, ROLE_ITEM_FILE_SIZE);
    this->setItem(nRow, 2, item);

    item = new QStandardItem(fileInfo.lastModified().toString("yyyy/MM/dd hh:mm:ss"));
    this->setItem(nRow, 3, item);

    item = new QStandardItem(info.name.section('.', -1));
    this->setItem(nRow, 4, item);

    m_nTotalSize += info.size;
}

void PhoneTreePhotoModel::deleteRowByPath(const QString &, const QString &inPath)
{
    for (auto i = this->rowCount() - 1; i >= 0; i--) {
        QString sPath = this->index(i, 1).data(ROLE_ITEM_FILE_PATH).toString();

        if (sPath.compare(inPath) == 0) {
            m_nTotalSize -= this->index(i, 2).data(ROLE_ITEM_FILE_SIZE).toInt();

            this->removeRow(i);
            break;
        }
    }
}

void PhoneTreePhotoModel::updateData(const QString &, const QVariant &)
{
}

qint64 PhoneTreePhotoModel::getTotalSize()
{
    return m_nTotalSize;
}
