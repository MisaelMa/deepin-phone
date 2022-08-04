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
#include "PhoneTreeMusicModel.h"

#include <QJsonObject>

#include "TrObject.h"

#define CHECK_COL 0 //CheckBox
#define NAME_COL 1 // 文件名

//#define MARTIST_COL 2 // 演唱者
#define MLENGTH_COL 2 // 时长
#define MSIZE_COL 3 // 文件大小
#define MTYPE_COL 4 // 类型
#define MDATE_COL 5 // 修改时间

PhoneTreeMusicModel::PhoneTreeMusicModel(QObject *parent)
    : BaseItemModel(parent)
{
    setHorizontalHeaderLabels(QStringList()
                              << "" << TrObject::getInstance()->getTreeHeaderText(Header_Name)
                              << TrObject::getInstance()->getTreeHeaderText(Header_Duration)
                              << TrObject::getInstance()->getTreeHeaderText(Header_Size)
                              << TrObject::getInstance()->getTreeHeaderText(Header_Type)
                              << TrObject::getInstance()->getTreeHeaderText(Header_Time_modified));
}

void PhoneTreeMusicModel::appendData(const QVariant &data)
{
    auto fi = data.value<QFileInfo>();

    int nRow = this->rowCount();
    QStandardItem *item = nullptr;

    item = new QStandardItem("");
    item->setData(fi.size(), ROLE_ITEM_FILE_SIZE);
    item->setData(fi.absoluteFilePath(), ROLE_ITEM_FILE_PATH);
    this->setItem(nRow, CHECK_COL, item);

    item = new QStandardItem(fi.completeBaseName());
    item->setIcon(getFileIcon(fi.suffix(), fi)); // file.icon, Qt::DecorationRole);
    item->setData(data, ROLE_ITEM_DATA);
    this->setItem(nRow, NAME_COL, item);

    item = new QStandardItem("00:00");
    this->setItem(nRow, MLENGTH_COL, item);

    item = new QStandardItem(dataSizeToString(quint64(fi.size())));
    item->setData(fi.size(), ROLE_ITEM_FILE_SIZE);
    this->setItem(nRow, MSIZE_COL, item);

    item = new QStandardItem(fi.suffix());
    this->setItem(nRow, MTYPE_COL, item);

    item = new QStandardItem(fi.lastModified().toString("yyyy/MM/dd hh:mm:ss"));
    this->setItem(nRow, MDATE_COL, item);

    m_nTotalSize += fi.size();
}

void PhoneTreeMusicModel::deleteRowByPath(const QString &, const QString &inPath)
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

void PhoneTreeMusicModel::updateDataPixmap(const QJsonObject &obj, const QPixmap &)
{
    QString path = obj.value("path").toString();
    QString length = obj.value("length").toString();

    for (int i = 0; i < rowCount(); i++) {
        auto fi = this->index(i, NAME_COL).data(ROLE_ITEM_DATA).value<QFileInfo>();
        if (fi.absoluteFilePath() == path) {
            auto item = this->item(i, MLENGTH_COL);
            item->setText(length);

            break;
        }
    }
}

void PhoneTreeMusicModel::updateData(const QString &tmp_fileName, const QVariant &var)
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

            item = this->item(nRow, MSIZE_COL);
            item->setText(dataSizeToString(quint64(fi.size())));
            item->setData(fi.size(), ROLE_ITEM_FILE_SIZE);

            item = this->item(nRow, MDATE_COL);
            item->setText(fi.lastModified().toString("yyyy/MM/dd hh:mm:ss"));
        }
    }
}

qint64 PhoneTreeMusicModel::getTotalSize()
{
    qint64 nTempSize = 0;
    for (auto i = this->rowCount() - 1; i >= 0; i--) {
        nTempSize += this->index(i, 0).data(ROLE_ITEM_FILE_SIZE).toLongLong();
    }

    return nTempSize;
}
