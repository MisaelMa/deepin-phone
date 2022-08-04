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
#include "PhoneIconFileModel.h"

#include <QJsonObject>

PhoneIconFileModel::PhoneIconFileModel(QObject *parnet)
    : BaseItemModel(parnet)
{
}

//使用FILE_INFO结构添加 model
void PhoneIconFileModel::appendData_FI(const FILE_INFO &file_info)
{
    QVariant data;
    data.setValue(file_info.fileInfo);

    auto item = new QStandardItem(file_info.fileInfo.fileName());
    item->setIcon(file_info.icon);

    item->setData(data, ROLE_ITEM_DATA);

    this->appendRow(item);

    m_nTotalSize += file_info.fileInfo.size();
}

void PhoneIconFileModel::appendData(const QVariant &data)
{
    auto fi = data.value<QFileInfo>();

    auto item = new QStandardItem(fi.fileName());
    item->setIcon(getFileIcon(fi.suffix(), fi));

    item->setData(data, ROLE_ITEM_DATA);

    this->appendRow(item);

    m_nTotalSize += fi.size();
}

void PhoneIconFileModel::deleteRowByPath(const QString &name, const QString &path)
{
    auto items = this->findItems(name);
    foreach (auto item, items) {
        auto info = item->data(ROLE_ITEM_DATA).value<QFileInfo>();
        if (info.absoluteFilePath() == path) {
            this->removeRow(item->row());
            break;
        }
    }
}

void PhoneIconFileModel::updateDataPixmap(const QJsonObject &object, const QPixmap &pixmap)
{
    QString path = object.value("path").toString();

    //修改文件在照片等其它页面删除后，文件页面未刷新再次导入时不显示缩略图等问题
    QString name;
    if (path.contains("/")) {
        name = path.mid(path.lastIndexOf("/") + 1);
    } else {
        name = path;
    }
    auto items = this->findItems(name);
    foreach (auto item, items) {
        auto info = item->data(ROLE_ITEM_DATA).value<QFileInfo>();
        if (info.absoluteFilePath() == path) {
            item->setIcon(pixmap);
            break;
        }
    }

    //    for (int i = 0; i < rowCount(); i++) {
    //        auto fi = this->index(i, 0).data(ROLE_ITEM_DATA).value<QFileInfo>();
    //        if (fi.absoluteFilePath() == path) {
    //            auto item = this->item(i, 0);
    //            item->setIcon(pixmap);

    //            break;
    //        }
    //    }
}

void PhoneIconFileModel::updateData(const QString &, const QVariant &)
{
}

qint64 PhoneIconFileModel::getTotalSize()
{
    return m_nTotalSize;
}
