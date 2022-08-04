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
#ifndef BASEITEMMODEL_H
#define BASEITEMMODEL_H

#include <QObject>
#include <QStandardItemModel>

#include "defines.h"
#include "FileModel.h"
#include "Model.h"

/**
 * @brief 手机助手对象模型
 */

#define APP_PACKAGE_NAME (Qt::UserRole + 60)
#define APP_APK_NAME (Qt::UserRole + 61)

#define ROLE_ITEM_DATA (Qt::UserRole + 66)
#define ROLE_NEW_DIR (Qt::UserRole + 68) //新建文件夹

#define ROLE_ITEM_FILE_SIZE (Qt::UserRole + 101) //  文件大小
#define ROLE_ITEM_FILE_NAME (Qt::UserRole + 101) //  文件名称
#define ROLE_ITEM_FILE_PATH (Qt::UserRole + 102) //  文件绝对路径

class BaseItemModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit BaseItemModel(QObject *parent = nullptr);
    virtual ~BaseItemModel() override;

signals:
    void sigLoadDataEnd(); //  结束加载数据信号

public:
    virtual void reloadModel();
    virtual void deleteRowByPath(const QString &, const QString &path) = 0;
    virtual void appendData(const QVariant &) = 0;
    virtual void updateData(const QString &, const QVariant &) = 0;
    virtual void updateDataPixmap(const QJsonObject &, const QPixmap &);

    void setStrPhoneDevId(const QString &strPhoneDevId);
    QString strPhoneDevId() const;

    DEVICE_TYPE nDeviceType() const;
    void setNDeviceType(const DEVICE_TYPE &nDeviceType);

    QString strCurPath() const;
    void setStrCurPath(const QString &strCurPath);

    virtual qint64 getTotalSize() = 0;
    void clearTotalSize();

protected:
    QString dataSizeToString(const quint64 &inSize);

    QIcon getFileIcon(const QString &, const QFileInfo &fileinfo);

protected:
    qint64 m_nTotalSize = 0; //  当前model 的大小
    QString m_strPhoneDevId = ""; //   当前显示 的设备id
    DEVICE_TYPE m_nDeviceType = Mount_OTHER; //  挂载设备类型
    QString m_strCurPath = ""; //  当前显示的路径
};

#endif // PHONEITEMMODEL_H
