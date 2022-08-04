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
#ifndef DEVMOUNTCONTROL_H
#define DEVMOUNTCONTROL_H

//#include "defines.h"

#include <dgiomount.h>
#include <ddiskmanager.h>
#include <dblockdevice.h>
#include <ddiskdevice.h>
#include <QObject>
#include <QMap>
#include <QString>
#include <QUrl>

class DGioVolumeManager;

class DevMountControl : public QObject
{
    Q_OBJECT
public:
    explicit DevMountControl(QObject *parent = nullptr);
    ~DevMountControl();

public slots:
    void onVfsMountChangedAdd(QExplicitlySharedDataPointer<DGioMount> mount);
    void onVfsMountChangedRemove(QExplicitlySharedDataPointer<DGioMount> mount);

signals:
    void sigNewDeviceAdd(QString devId);
    void sigMountDeviceDel(QString devId);

private:
    void initConnection();
    void getAllDeviceName();
    const QList<QExplicitlySharedDataPointer<DGioMount>> getVfsMountList();
    void updateExternalDevice(QExplicitlySharedDataPointer<DGioMount> mount);

private:
    DGioVolumeManager *m_vfsManager;
    DDiskManager *m_diskManager;
    QList<QExplicitlySharedDataPointer<DGioMount>> m_mounts;
    QMap<QUrl, QString> m_durlAndNameMap;
};

#endif // DEVMOUNTCONTROL_H
