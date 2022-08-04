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
#include "PhoneTreeEBookModel.h"

#include <QFileInfo>
#include <QDebug>
#include <DFileIconProvider>

#include "TrObject.h"

#include "defines.h"

DWIDGET_USE_NAMESPACE

#define CHECK_COL 0 //CheckBox
#define NAME_COL 1 // 文件名
#define EBSIZE_COL 2 // 文件大小
#define EBDATE_COL 3 // 修改时间
#define EBTYPE_COL 4 // 类型

PhoneTreeEBookModel::PhoneTreeEBookModel(QObject *parent)
    : BaseItemModel(parent)
{
    setHorizontalHeaderLabels(QStringList()
                              << "" << TrObject::getInstance()->getTreeHeaderText(Header_Name)
                              << TrObject::getInstance()->getTreeHeaderText(Header_Size)
                              << TrObject::getInstance()->getTreeHeaderText(Header_Time_modified)
                              << TrObject::getInstance()->getTreeHeaderText(Header_Type));
}

void PhoneTreeEBookModel::appendData(const QVariant &data)
{
    auto fi = data.value<QFileInfo>();

    int nRow = this->rowCount();
    QStandardItem *item = nullptr;
    item = new QStandardItem("");
    item->setData(fi.size(), ROLE_ITEM_FILE_SIZE);
    item->setData(fi.absoluteFilePath(), ROLE_ITEM_FILE_PATH);
    this->setItem(nRow, CHECK_COL, item);

    item = new QStandardItem(fi.completeBaseName());
    item->setIcon(getFileIcon(fi.suffix(), fi));
    item->setData(data, ROLE_ITEM_DATA);
    this->setItem(nRow, NAME_COL, item);

    item = new QStandardItem(dataSizeToString(quint64(fi.size())));
    item->setData(fi.size(), ROLE_ITEM_FILE_SIZE);
    this->setItem(nRow, EBSIZE_COL, item);

    item = new QStandardItem(fi.lastModified().toString("yyyy/MM/dd hh:mm:ss"));
    this->setItem(nRow, EBDATE_COL, item);

    item = new QStandardItem(fi.suffix());
    this->setItem(nRow, EBTYPE_COL, item);

    m_nTotalSize += fi.size();
}

void PhoneTreeEBookModel::deleteRowByPath(const QString &, const QString &inPath)
{
    for (auto i = this->rowCount() - 1; i >= 0; i--) {
        QFileInfo fileInfo = this->index(i, NAME_COL).data(ROLE_ITEM_DATA).value<QFileInfo>();
        QString path = fileInfo.absoluteFilePath();
        if (path.compare(inPath) == 0) {
            m_nTotalSize -= fileInfo.size();

            this->removeRow(i);
            break;
        }
    }
}

void PhoneTreeEBookModel::updateData(const QString &tmp_fileName, const QVariant &var)
{
    Q_UNUSED(tmp_fileName);

    auto fi = var.value<QFileInfo>();

    auto listItems = this->findItems(fi.completeBaseName(), Qt::MatchExactly, NAME_COL);

    foreach (auto item, listItems) {
        int nRow = item->row();

        auto filePath = this->item(nRow, CHECK_COL)->data(ROLE_ITEM_FILE_PATH).toString();
        if (fi.absoluteFilePath() == filePath) {
            qint64 oldSize = this->item(nRow, CHECK_COL)->data(ROLE_ITEM_FILE_SIZE).toLongLong();
            m_nTotalSize -= oldSize; //  减去 旧的文件大小

            m_nTotalSize += fi.size(); //  加上 新的文件大小

            QStandardItem *item = this->item(nRow, CHECK_COL);
            item->setData(fi.size(), ROLE_ITEM_FILE_SIZE);
            item->setData(fi.absoluteFilePath(), ROLE_ITEM_FILE_PATH);

            item = this->item(nRow, NAME_COL);
            item->setData(var, ROLE_ITEM_DATA);

            item = this->item(nRow, EBSIZE_COL);
            item->setText(dataSizeToString(quint64(fi.size())));
            item->setData(fi.size(), ROLE_ITEM_FILE_SIZE);

            item = this->item(nRow, EBDATE_COL);
            item->setText(fi.lastModified().toString("yyyy/MM/dd hh:mm:ss"));
        }
    }
}

qint64 PhoneTreeEBookModel::getTotalSize()
{
    qint64 nTempSize = 0;
    for (auto i = this->rowCount() - 1; i >= 0; i--) {
        nTempSize += this->index(i, 0).data(ROLE_ITEM_FILE_SIZE).toLongLong();
    }

    return nTempSize;
}
