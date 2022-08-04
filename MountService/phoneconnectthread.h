/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co., Ltd.
 *
 * Author:     zhangliangliang <zhangliangliang@uniontech.com>
 * Maintainer: zhangliangliang <zhangliangliang@uniontech.com>
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
#ifndef PHONECONNECTTHREAD_H
#define PHONECONNECTTHREAD_H

#include "defines.h"

#include <QThread>
#include <QDebug>
#include <QHash>
#include <QList>
#include <QRunnable>

typedef QMap<QString, QMap<QString, QString>> DatabaseMap;

const QString Devicetype_HwInfo_Twospace = "  ";
const QString Devicetype_HwInfo_Fourspace = "    ";
const QString Devicetype_HwInfo_Resolution = "Resolution";
const QString Devicetype_HwInfo_ResolutionList = "Support Resolution";
const QString Devicetype_HwInfo_Currentresolution = "Current Resolution";

const QString Devicetype_lpstat_4Space = "    ";
const QString Devicetype_lpstat_Tab = "\t";

class DevMountControl;

class PhoneConnectThread : public QThread
{
    Q_OBJECT

public:
    PhoneConnectThread(QObject *parent = nullptr);
    ~PhoneConnectThread() override;

protected:
    void run() override;

signals:
    void sigDeviceListChanged(const DEV_CONN_INFO &devInfo);
    void sigDeviceAuthorezeChanged(const DEV_CONN_INFO devInfo);
    void sigNoDeviceList();
    //void sigClearWidgetInfo(DEV_CONN_INFO devInfo);

    //void slotNewDeviceFind(QString devId);
    //void slotMountDeviceDel(QString devId);

private:
    //    void deviceCheck();

    void androidDeviceList();
    void iosDeviceList();
    void delIOSDevice(QString id);
    void delAndroidDevice(QString id);

    bool loadHwinfoUSBInfo(); // USB hwinfo --usb
    void checkUsbAndroidDevice(); //检测USB安卓设备
    void checkAllDeviceNum(); //检查所有连接的设备数

private:
    bool m_bIsCanRun = true;

    QList<QString> m_androidDevIDList;
    QList<QString> m_iosDevIDList;
    QList<QString> m_iosMountList;
    QMap<QString, bool> m_devAuthorizeList;
    QMap<QString, DEV_CONN_INFO> m_devUsbInfoList;
    bool m_bHaveDevice; //是否有设备连接
    DevMountControl *m_devMountCtrl;
    //QList<QString> m_mountFindDevIDList; //是否是Mount发现的设备
    int m_androidDevCheckNum; //用于记录连续检测android设备记数

    QString m_strIosDeviceId = "";
    bool m_adbCheck;
    DatabaseMap m_usbInfoMap;
    QList<QString> m_usbAndroidList;
};

#endif // PHONECONNECTTHREAD_H
