/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co., Ltd.
 *
 * Author:     yelei <yelei@uniontech.com>
 * Maintainer: yelei <yelei@uniontech.com>
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "defines.h"

#include <QItemSelection>

#include <DWidget>

DWIDGET_USE_NAMESPACE

class PhoneListView;
class MainRightWidget;

class MainWidget : public DWidget
{
    Q_OBJECT
public:
    explicit MainWidget(DWidget *parent = nullptr);

    void setLocalApkVersion(const QString &);

signals:
    void sigDeviceListUpdate();
    void sigNoDeviceConnected();

private slots:
    void initUI();
    void startDeviceConnectCheck();
    void getDefaultFileIcon();

    void slotDeviceListUpdate(const DEV_CONN_INFO &devInfo);
    void slotDeviceAuthorizeUpdate(const DEV_CONN_INFO &devInfo);

    void slotListCurrentChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void onFindPhoneInfo(const PhoneInfo &info);
    void slotUpdateBattery(QString phoneId, int value);

private:
    void readPhoneInfo(const PhoneInfo &info); // 读取手机信息

    void addDev(const DEV_CONN_INFO &devInfo); // 设备链接
    void delDev(const DEV_CONN_INFO &devInfo); // 设备断开
    void refreshDev(const DEV_CONN_INFO &devInfo); // 设备刷新

private:
    QString m_strLocalApkVersion = "";
    PhoneListView *m_pPhoneListView = nullptr;

    MainRightWidget *m_pMainRightWidget = nullptr;
};

#endif // MAINWIDGET_H
