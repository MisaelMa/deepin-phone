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
#ifndef PHONETREEEBOOKMODEL_H
#define PHONETREEEBOOKMODEL_H

#include "base/BaseItemModel.h"
/**
 * @brief 手机助手 列表对象模型
 */

class PhoneTreeEBookModel : public BaseItemModel
{
    Q_OBJECT
public:
    explicit PhoneTreeEBookModel(QObject *parent = nullptr);

    // BaseItemModel interface
public:
    void appendData(const QVariant &) override;
    void deleteRowByPath(const QString &, const QString &path) override;
    void updateData(const QString &, const QVariant &) override;
    qint64 getTotalSize() override;
};

#endif // PHONETREEITEMMODEL_H
