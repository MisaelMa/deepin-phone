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
#ifndef THREADSERVICE_H
#define THREADSERVICE_H

#include "threadservice_global.h"

#include <QMap>
#include <QObject>
#include <QThreadPool>

#include "PMTask.h"
#include "Mount/MountFileManager.h"

class THREADSERVICESHARED_EXPORT ThreadService
{
private:
    ThreadService();

public:
    ~ThreadService();
    static void InitService();
    static ThreadService *getService();

    enum E_Thread_Type {
        E_Locate_App_Version_Task, //  本地apk的版本任务
        E_Battery_Task, //  电量任务
        E_Phone_Use_Size_Task, //  手机空间使用任务
        E_Music_Display_Task, //  音乐数据展示
        E_Music_Delete_Task, //  音乐文件删除
        E_Music_Info_Task, //  音乐信息获取任务
        E_EBook_Display_Task, //  电子书展示
        E_EBook_Delete_Task, //  电子书删除
        E_File_Display_Task, //  文件展示
        E_File_Delete_Task, //   文件模块 删除
        //        E_Image_Render_Task, //  图片渲染
        //        E_Video_Render_Task, //  视频渲染
        E_File_Icon_Task, //  文件默认图标
    };

public:
    /**
     * @brief 开始任务
     */
    void startTask(const E_Thread_Type &, PMTask *);
    /**
     * @brief 停止任务
     */
    void stopTask(const E_Thread_Type &);

private:
    static ThreadService *g_instance;

    MountFileManager *m_pMountFileManager = nullptr;

    QMap<E_Thread_Type, PMTask *> m_mapTask;
};

#endif // THREADSERVICE_H
