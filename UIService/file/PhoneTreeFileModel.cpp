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
#include "PhoneTreeFileModel.h"
#include <QJsonObject>

#include "defines.h"

#include "TrObject.h"

PhoneTreeFileModel::PhoneTreeFileModel(QObject *parent)
    : BaseItemModel(parent)
{
    QStringList headList = QStringList() << TrObject::getInstance()->getTreeHeaderText(Header_Name)
                                         << TrObject::getInstance()->getTreeHeaderText(Header_Size)
                                         << TrObject::getInstance()->getTreeHeaderText(Header_Time_modified)
                                         << TrObject::getInstance()->getTreeHeaderText(Header_Type);

    setHorizontalHeaderLabels(headList);
}

//使用FILE_INFO 结构添加信息
void PhoneTreeFileModel::appendData_FI(const FILE_INFO &file_info)
{
    QFileInfo fi = file_info.fileInfo;
    QVariant data;
    data.setValue(fi);

    int nRow = this->rowCount();

    auto listItemName = new QStandardItem(fi.fileName());
    listItemName->setIcon(file_info.icon);
    listItemName->setData(data, ROLE_ITEM_DATA);
    listItemName->setData(fi.size(), ROLE_ITEM_FILE_SIZE);

    this->setItem(nRow, 0, listItemName);

    QString strSize;
    QVariant usrVarSize;
    if (fi.isDir()) {
        strSize = "-";
        usrVarSize.setValue(0);
    } else {
        strSize = dataSizeToString(quint64(fi.size()));
        usrVarSize.setValue(fi.size());
    }
    auto itemSize = new QStandardItem(strSize);
    itemSize->setData(usrVarSize, ROLE_ITEM_FILE_SIZE);
    this->setItem(nRow, 1, itemSize);

    QString date = fi.lastModified().toString("yyyy/MM/dd hh:mm:ss");
    auto itemDate = new QStandardItem(date);
    this->setItem(nRow, 2, itemDate);

    QString type = fi.isDir() ? TrObject::getInstance()->getTreeHeaderText(Tree_Directory) : TrObject::getInstance()->getTreeHeaderText(Tree_File);
    if (!fi.isDir() && !fi.suffix().isEmpty()) {
        type = fi.suffix();
    }
    auto itemType = new QStandardItem(type);
    this->setItem(nRow, 3, itemType);

    m_nTotalSize += fi.size();
}

void PhoneTreeFileModel::appendData(const QVariant &data)
{
    auto fi = data.value<QFileInfo>();

    int nRow = this->rowCount();

    auto listItemName = new QStandardItem(fi.fileName());
    listItemName->setIcon(getFileIcon(fi.suffix(), fi));
    listItemName->setData(data, ROLE_ITEM_DATA);
    listItemName->setData(fi.size(), ROLE_ITEM_FILE_SIZE);

    this->setItem(nRow, 0, listItemName);

    QString strSize;
    QVariant usrVarSize;
    if (fi.isDir()) {
        strSize = "-";
        usrVarSize.setValue(0);
    } else {
        strSize = dataSizeToString(quint64(fi.size()));
        usrVarSize.setValue(fi.size());
    }
    auto itemSize = new QStandardItem(strSize);
    itemSize->setData(usrVarSize, ROLE_ITEM_FILE_SIZE);
    this->setItem(nRow, 1, itemSize);

    QString date = fi.lastModified().toString("yyyy/MM/dd hh:mm:ss");
    auto itemDate = new QStandardItem(date);
    this->setItem(nRow, 2, itemDate);

    QString type = fi.isDir() ? TrObject::getInstance()->getTreeHeaderText(Tree_Directory) : TrObject::getInstance()->getTreeHeaderText(Tree_File);
    if (!fi.isDir() && !fi.suffix().isEmpty()) {
        type = fi.suffix();
    }
    auto itemType = new QStandardItem(type);
    this->setItem(nRow, 3, itemType);

    m_nTotalSize += fi.size();
}

void PhoneTreeFileModel::deleteRowByPath(const QString &, const QString &)
{
}

void PhoneTreeFileModel::updateDataPixmap(const QJsonObject &object, const QPixmap &pixmap)
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

void PhoneTreeFileModel::updateData(const QString &, const QVariant &)
{
}

qint64 PhoneTreeFileModel::getTotalSize()
{
    qint64 nTempSize = 0;
    for (auto i = this->rowCount() - 1; i >= 0; i--) {
        nTempSize += this->index(i, 0).data(ROLE_ITEM_FILE_SIZE).toLongLong();
    }

    return nTempSize;
}
