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
#include "SocketThread.h"

#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTcpSocket>

#include "Android/phonesocket.h"
#include "MountService.h"

const int CMD_GET_APP_REQ = 100;
const int CMD_GET_APP_RSP = 300;

const QString KEY_CMD = "command";
const QString KEY_Device_Id = "deviceid";
const QString KEY_DATA = "data";
const QString KEY_APPS = "apps";

SocketThread::SocketThread(QObject *p)
    : QThread(p)
{
    m_socketTimer = nullptr;
}

SocketThread::~SocketThread()
{
    //    if (m_pPhoneSocket) {
    //        delete m_pPhoneSocket;
    //        m_pPhoneSocket = nullptr;
    //    }

    if (m_socketTimer != nullptr) {
        m_socketTimer->stop();
        delete m_socketTimer;
        m_socketTimer = nullptr;
    }

    m_bIsCanRun = false;

    quit();
    wait();
}

void SocketThread::setPhoneInfo(const QString &phoneId, const DEVICE_TYPE &devType)
{
    m_strPhoneDevId = phoneId;
    m_nDeviceType = devType;
}

void SocketThread::run()
{
    if (m_nDeviceType == Mount_Ios) {
        QList<PhoneAppInfo> apps;

        for (int i = 0; i < 9; i++) {
            if (m_bIsCanRun == false) {
                break;
            }

            QThread::msleep(500);

            apps = MountService::getService()->getIosApps();
            if (!apps.isEmpty())
                break;
        }

        if (m_bIsCanRun == true) {
            emit sigAppInfoList(m_strPhoneDevId, apps);
        }
    } else {
        //  ?????????????????? ??????????????????
        MountService::getService()->startApkExe(m_strPhoneDevId);

        for (int i = 0; i < 4; i++) {
            QThread::msleep(500);
            if (!m_bIsCanRun) {
                break;
            }
        }

        //        if (m_bIsCanRun == true) {
        //            m_pPhoneSocket = new PhoneSocket(m_strPhoneDevId);
        //            connect(m_pPhoneSocket, &PhoneSocket::sigAppInfoList, this, &SocketThread::sigAppInfoList);
        //            m_pPhoneSocket->start();
        //            while (m_pPhoneSocket->isRunning()) {}
        //            //m_pPhoneSocket->sendCommand(m_strPhoneDevId);

        //            delete m_pPhoneSocket;
        //            m_pPhoneSocket = nullptr;
        //        }

        QTcpSocket *m_tcpSocket = new QTcpSocket();
        connect(m_tcpSocket, &QIODevice::readyRead, this, [=]() {
            m_recvMsg += m_tcpSocket->readAll();
            QJsonParseError err_rpt;
            //??????JSON?????????????????????????????????????????????
            QJsonDocument::fromJson(m_recvMsg, &err_rpt); //?????????????????????JSON
            if (err_rpt.error == QJsonParseError::NoError) {
                //????????????????????????????????????
                if (m_socketTimer != nullptr) {
                    m_socketTimer->stop();
                    delete m_socketTimer;
                    m_socketTimer = nullptr;
                }
                //????????????????????????????????????JSON
                parseApps(m_recvMsg);
            } else {
                //????????????????????????????????????????????????
            }
        },
                Qt::DirectConnection);

        MountService::getService()->execAdbForward(m_strPhoneDevId);
        m_tcpSocket->connectToHost("127.0.0.1", 10033);

        QJsonObject cmdJsonObj;
        cmdJsonObj.insert(KEY_CMD, CMD_GET_APP_REQ);
        cmdJsonObj.insert(KEY_Device_Id, m_strPhoneDevId);

        //?????????
        QJsonDocument jsonDoc(cmdJsonObj);
        QString sendJson = jsonDoc.toJson(QJsonDocument::Compact);

        if (m_tcpSocket->waitForConnected(5000)) {
            // ????????????
            m_tcpSocket->write(sendJson.toLocal8Bit().data());
            m_recvMsg.clear();
            //??????????????????????????????????????????????????????????????????
            m_socketTimer = new QTimer();
            connect(m_socketTimer, &QTimer::timeout, this, &SocketThread::slotSocketTimerOut, Qt::DirectConnection);
            m_socketTimer->start(15000);
            exec(); //????????????????????????????????????????????????Socket??????????????????

            if (m_socketTimer != nullptr) {
                m_socketTimer->stop();
                delete m_socketTimer;
                m_socketTimer = nullptr;
            }
        } else {
            //??????????????????
            QList<PhoneAppInfo> appInfoList;
            emit sigAppInfoList(m_strPhoneDevId, appInfoList);
            qDebug() << "connect to phone error.";
        }
    }
}

//?????????????????????app????????????
void SocketThread::parseApps(const QByteArray &result)
{
    QJsonParseError err_rpt;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(result, &err_rpt); //?????????????????????JSON
    if (err_rpt.error == QJsonParseError::NoError) {
        //???????????????????????????
        QList<PhoneAppInfo> m_appInfoList;
        QString sDeviceId;
        if (parse_doucment.isObject()) { //?????????????????????
            QJsonObject obj = parse_doucment.object();
            if (obj.contains(KEY_CMD)) {
                int cmd = obj.value(KEY_CMD).toInt();
                sDeviceId = obj.value(KEY_Device_Id).toString();

                if (CMD_GET_APP_RSP == cmd) {
                    if (obj.contains(KEY_DATA)) {
                        QJsonObject data = obj.value(KEY_DATA).toObject();

                        if (data.contains(KEY_APPS)) {
                            QJsonArray appInfoArray = data.value(KEY_APPS).toArray();

                            for (auto appInfo : appInfoArray) {
                                QJsonObject appObj = appInfo.toObject();
                                QString pkgName = appObj.value("package").toString();
                                if (pkgName.startsWith("com.ut.utassistant"))
                                    continue;

                                PhoneAppInfo phoneApp;
                                phoneApp.strAppName = appObj.value("name").toString();
                                phoneApp.strPacketName = pkgName; //appObj.value("package").toString();
                                //                                phoneApp.strIconPath = appObj.value("icon").toString();
                                phoneApp.nAppSize = quint64(appObj.value("size").toInt());
                                phoneApp.strAppVersion = appObj.value("version").toString();

                                m_appInfoList.append(phoneApp);
                                if (!m_bIsCanRun) {
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        //????????????????????????????????????????????????????????????
        emit sigAppInfoList(sDeviceId, m_appInfoList);
    }

    m_recvMsg.clear();
    quit(); //????????????????????????
}

void SocketThread::slotSocketTimerOut()
{
    if (m_socketTimer != nullptr) {
        m_socketTimer->stop();
        delete m_socketTimer;
        m_socketTimer = nullptr;
    }
    //Socket ??????????????????????????? ?????????????????????
    QList<PhoneAppInfo> appInfoList;
    emit sigAppInfoList(m_strPhoneDevId, appInfoList);
    m_recvMsg.clear();
    quit(); //????????????????????????
    qDebug() << "Socket does not return data for a long time.";
}
