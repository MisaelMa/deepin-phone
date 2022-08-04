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
#ifndef DATASERVICE_H
#define DATASERVICE_H

#include <QObject>
#include <QMap>
#include <QIcon>
#include <QFileInfo>

class DataService : public QObject
{
    Q_OBJECT
public:
    explicit DataService(QObject *parent = nullptr);

public:
    QIcon getIconBySuffix(const QString &, const QFileInfo &);
    //    void intsertPathAndSize(const QString &, const qint64 &);
    //    qint64 getFileSizeByPath(const QString &);

private:
    QMap<QString, QIcon> m_pSuffixIcon;
    //    QMap<QString, qint64> m_pPathAndSize;
};

#endif // DATASERVICE_H
