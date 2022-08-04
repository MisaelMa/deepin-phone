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
#ifndef UIINTERFACE_H
#define UIINTERFACE_H

#include <QString>
#include "defines.h"

class UIInterface
{
protected:
    virtual ~UIInterface() {}

public:
    /**
     * @brief 当左侧选项切换， 当前显示的界面需要根据自己的数据情况，设置顶部标题栏的状态
     */
    virtual void setWidgetBtnState() = 0;

    /**
     * @brief 设备移除的时候， 清空当前数据
     * @param devInfo
     */
    virtual void clearWidgetInfo(const QString &devInfo) = 0;

    /**
     * @brief 更新当前模块的数据
     * @param info
     */
    virtual void updateWidgetInfo(const PhoneInfo &info) = 0;

protected:
    virtual void refreshWidgetData() = 0; //  刷新
    virtual void calculateSelectCountSize() = 0;

    // 是否开启读取线程 QThread::isRunning
    virtual bool isReading() = 0;
    // 暂停读取
    virtual bool pauseRead() = 0;
    // 继续读取
    virtual bool continueRead() = 0;
};

#endif // UIINTERFACE_H
