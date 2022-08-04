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
#include "BaseItemModel.h"

BaseItemModel::BaseItemModel(QObject *parent)
    : QStandardItemModel(parent)
{
}

BaseItemModel::~BaseItemModel()
{
}

void BaseItemModel::reloadModel()
{
}

void BaseItemModel::updateDataPixmap(const QJsonObject &, const QPixmap &)
{
}

//qint64 BaseItemModel::getTotalSize()
//{
//    return m_nTotalSize;
//}

QString BaseItemModel::dataSizeToString(const quint64 &inSize)
{
    QStringList units = QStringList() << "B"
                                      << "KB"
                                      << "MB"
                                      << "GB"
                                      << "TB"
                                      << "PB";
    double mod = 1024.0;
    double size = inSize;

    int i = 0;

    while (size >= mod && i < units.count() - 1) {
        size /= mod;
        i++;
    }
    QString szResult = QString::number(size, 'd', 1);
    szResult += " " + units[i];
    return szResult;
}

void BaseItemModel::setStrPhoneDevId(const QString &strPhoneDevId)
{
    m_strPhoneDevId = strPhoneDevId;
}

QString BaseItemModel::strPhoneDevId() const
{
    return m_strPhoneDevId;
}

DEVICE_TYPE BaseItemModel::nDeviceType() const
{
    return m_nDeviceType;
}

void BaseItemModel::setNDeviceType(const DEVICE_TYPE &nDeviceType)
{
    m_nDeviceType = nDeviceType;
}

QString BaseItemModel::strCurPath() const
{
    return m_strCurPath;
}

void BaseItemModel::setStrCurPath(const QString &strCurPath)
{
    m_strCurPath = strCurPath;
}

QIcon BaseItemModel::getFileIcon(const QString &suffix, const QFileInfo &filePath)
{
    return Model::getInstance()->getIconBySuffix(suffix, filePath);
}

void BaseItemModel::clearTotalSize()
{
    m_nTotalSize = 0;
}
