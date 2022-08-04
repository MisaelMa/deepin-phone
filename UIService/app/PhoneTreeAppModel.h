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
#ifndef PHONETREEAPPMODEL_H
#define PHONETREEAPPMODEL_H

#include "base/BaseItemModel.h"

class SocketThread;

/**
 * @brief 手机助手 列表对象模型
 */

class PhoneTreeAppModel : public BaseItemModel
{
    Q_OBJECT
public:
    explicit PhoneTreeAppModel(QObject *parent = nullptr);
    ~PhoneTreeAppModel() override;

    // BaseItemModel interface
public:
    /**
     * @brief app 应用数据
     * @param appInfo
     */
    void reloadModel() override;
    void deleteRowByPath(const QString &, const QString &path) override;
    void updateData(const QString &, const QVariant &) override;
    qint64 getTotalSize() override;

private:
    void appendData(const QVariant &appInfo) override;

private slots:
    /**
     * @brief socket 获取到的数据 通过信号-槽 进行设置
     * @param sDevid 设备Id， 用于区分获取的app 数据 的设备id
     * @param m_appInfoList
     */
    void slotDispResult(const QString &sDevid, const QList<PhoneAppInfo> &m_appInfoList);

private:
    QTimer *m_pDataTimer = nullptr;
    SocketThread *m_pSocketThread = nullptr;
};

#endif // PHONETREEITEMMODEL_H
