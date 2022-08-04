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
#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H

#include <QThread>
#include <QTimer>

#include "defines.h"

//class PhoneSocket;

class SocketThread : public QThread
{
    Q_OBJECT
public:
    explicit SocketThread(QObject *parent = nullptr);
    ~SocketThread() override;

signals:
    void sigAppInfoList(const QString &, const QList<PhoneAppInfo> &);

public:
    void setPhoneInfo(const QString &, const DEVICE_TYPE &);

public slots:
    void slotSocketTimerOut();

    // QThread interface
protected:
    void run() override;

    void parseApps(const QByteArray &result);

private:
    //PhoneSocket *m_pPhoneSocket = nullptr;
    bool m_bIsCanRun = true;
    DEVICE_TYPE m_nDeviceType = Mount_OTHER;
    QString m_strPhoneDevId = "";
    QByteArray m_recvMsg;
    QTimer *m_socketTimer;
};

#endif // SOCKETTHREAD_H
