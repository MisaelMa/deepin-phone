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
#ifndef MOUNTFILEMODEL_H
#define MOUNTFILEMODEL_H

/**
 * @brief 挂载文件数据, 每一分钟获取一次挂载数据
 */

#include <QList>
#include <QFileInfo>

class MountFileModel : public QFileInfo
{
public:
    MountFileModel();

    QList<MountFileModel> getChildrenList() const;
    void insertToChildrenList(const MountFileModel &);
    void clearChildren();

private:
    /**
     * @brief 子数据
     */
    QList<MountFileModel> m_pChildrenList;
};

#endif // MOUNTFILEMODEL_H
