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
#include "phonesocket.h"

#include <QHostAddress>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTcpSocket>

#include "MountService.h"
#include "phonethread.h"
#include "defines.h"

const int CMD_GET_APP_REQ = 100;
const int CMD_GET_APP_RSP = 300;

const QString KEY_CMD = "command";
const QString KEY_Device_Id = "deviceid";
const QString KEY_DATA = "data";
const QString KEY_APPS = "apps";

PhoneSocket::PhoneSocket(QObject *parent)
    : QObject(parent)
{
}

void PhoneSocket::sendCommand(const QString &devId)
{
    m_strDeviceID = devId;
    QJsonObject cmdJsonObj;
    cmdJsonObj.insert(KEY_CMD, CMD_GET_APP_REQ);
    cmdJsonObj.insert(KEY_Device_Id, devId);

    MountService::getService()->execAdbForward(devId);

    //序列化
    QJsonDocument jsonDoc(cmdJsonObj);
    QString sendJson = jsonDoc.toJson(QJsonDocument::Compact);
    tcp = new QTcpSocket(this);
    tcp->connectToHost(QHostAddress("127.0.0.1"), 10033);

    if (tcp->waitForConnected(5000)) {
        // 发送消息
        //socket->waitForBytesWritten();
        tcp->write(sendJson.toLocal8Bit().data());

        //读消息
        tcp->waitForReadyRead();
        QString sData = QString(tcp->readAll());

        recvSocketData(CMD_GET_APP_REQ, sData);
    }
}

void PhoneSocket::recvSocketData(int flag, const QString &result)
{
    //qDebug() << "slotRecvSocketData: " << flag << ", result: " << result;
    // 解析
    switch (flag) {
    case CMD_GET_APP_REQ:
        //        qDebug() << "parseApps";
        parseApps(result);
        break;
        //    case CMD_GET_SYS_REQ:
        //        parseSysInfo(result);
        //        break;
    }
}

void PhoneSocket::parseAppsEmpty()
{
    if (m_bIsSendTwice == false) { //  5秒没数据， 重新安装
        m_bIsSendTwice = true;
        PhoneThread *tempThread = new PhoneThread();
        tempThread->readPhoneInfo(m_strDeviceID, DEVICE_TYPE::Mount_Android);
        tempThread->findPhoneInfo();

        delete tempThread;

        QThread::sleep(2);

        sendCommand(m_strDeviceID);
    } else { //  还是没有数据， 另行处理

        //没有数据返回空
        QList<PhoneAppInfo> appInfoList;
        emit sigAppInfoList(m_strDeviceID, appInfoList);
    }
}

void PhoneSocket::parseApps(const QString &result)
{
    QJsonParseError err_rpt;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(result.toLocal8Bit().data(), &err_rpt); //字符串格式化为JSON
    if (err_rpt.error == QJsonParseError::NoError) {
        //获取对方发送的内容
        QList<PhoneAppInfo> m_appInfoList;
        QString sDeviceId;
        if (parse_doucment.isObject()) { //判断是否是对象
            QJsonObject obj = parse_doucment.object();
            if (obj.contains(KEY_CMD)) {
                int cmd = obj.value(KEY_CMD).toInt();
                sDeviceId = obj.value(KEY_Device_Id).toString();

                if (CMD_GET_APP_RSP == cmd) {
                    if (obj.contains(KEY_DATA)) {
                        QJsonObject data = obj.value(KEY_DATA).toObject();

                        if (data.contains(KEY_APPS)) {
                            QJsonArray appInfoArray = data.value(KEY_APPS).toArray();

                            //QList<PhoneAppInfo> m_appInfoList;

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
                            }
                            //emit sigAppInfoList(sDeviceId, m_appInfoList);
                        }
                    }
                }
            }
        }
        //从这里返回，当解决返回的数据异常时返回空
        emit sigAppInfoList(sDeviceId, m_appInfoList);
    } else {
        parseAppsEmpty();
    }
}
