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
#ifndef BASEOBJECT_H
#define BASEOBJECT_H

#include <QString>

#include "defines.h"

class BaseItemModel;
class BaseObject
{
public:
    BaseObject(const E_Widget_Type &);

protected:
    virtual ~BaseObject() {}

public:
    /**
     * @brief 获取源数据模型对象
     * @return
     */
    virtual BaseItemModel *getSourceModel() const = 0;

    /**
     * @brief 根据名称和路径删除model对应的数据
     * @param path
     */
    virtual void deleteModelByPath(const QString &, const QString &path) = 0;

    /**
     * @brief 设置item 选中
     * @param sPath
     * @param bSelected
     */
    virtual void setItemSelect(const QString &sPath, const bool &bSelected) = 0;

protected:
    BaseItemModel *m_pSourceModel = nullptr; //  模型基类
    E_Widget_Type m_nWidgetType = E_No_Define; //  窗口类型
};

#endif // BASEOBJECT_H
