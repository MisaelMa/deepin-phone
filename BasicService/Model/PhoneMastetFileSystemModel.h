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
#ifndef PHONEMASTETFILESYSTEMMODEL_H
#define PHONEMASTETFILESYSTEMMODEL_H

#include <QFileSystemModel>

/**
 * @brief 文件系统数据模型, 全局 唯一
 */

class PhoneMastetFileSystemModel : public QFileSystemModel
{
    Q_OBJECT

public:
    explicit PhoneMastetFileSystemModel(QObject *parent = nullptr);
};

#endif // PHONEMASTETFILESYSTEMMODEL_H
