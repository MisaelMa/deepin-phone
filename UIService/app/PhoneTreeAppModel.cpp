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
#include "PhoneTreeAppModel.h"

#include <QtConcurrent/QtConcurrent>

#include "defines.h"
#include "TrObject.h"
#include "Android/phonesocket.h"
#include "MountService.h"
#include "SocketThread.h"
#include <QApplication>

PhoneTreeAppModel::PhoneTreeAppModel(QObject *parent)
    : BaseItemModel(parent)
{
    setHorizontalHeaderLabels(QStringList()
                              << "" << TrObject::getInstance()->getTreeHeaderText(Header_Name)
                              << TrObject::getInstance()->getTreeHeaderText(Header_Size)
                              << TrObject::getInstance()->getTreeHeaderText(Header_Version)
                              << "packageName"
                              << TrObject::getInstance()->getTreeHeaderText(Header_Action));
}

PhoneTreeAppModel::~PhoneTreeAppModel()
{
    if (m_pSocketThread) {
        delete m_pSocketThread;
        m_pSocketThread = nullptr;
    }
}

void PhoneTreeAppModel::appendData(const QVariant &data)
{
    auto appInfo = data.value<PhoneAppInfo>();

    //  防止 重复添加
    QString sPkgName = appInfo.strPacketName;
    auto items = this->findItems(sPkgName, Qt::MatchExactly, 4);
    if (items.size() > 0)
        return;

    int nRow = this->rowCount();

    QStandardItem *item = nullptr;

    item = new QStandardItem();
    item->setData(QVariant(false));
    this->setItem(nRow, 0, item);

    item = new QStandardItem(appInfo.strAppName);
    item->setData(appInfo.strAppName, Qt::DecorationRole); //设置数据包含图标
    this->setItem(nRow, 1, item);

    QString strSize = dataSizeToString(appInfo.nAppSize);
    item = new QStandardItem(strSize);
    item->setData(appInfo.nAppSize, ROLE_ITEM_FILE_SIZE);
    this->setItem(nRow, 2, item);

    item = new QStandardItem(appInfo.strAppVersion);
    this->setItem(nRow, 3, item);

    item = new QStandardItem(appInfo.strPacketName);
    this->setItem(nRow, 4, item);

    //  最后一列的操作, 导出\卸载 需要的 包名
    item = new QStandardItem();
    item->setData(appInfo.strPacketName, APP_PACKAGE_NAME);
    item->setData(appInfo.strAppName, APP_APK_NAME);
    this->setItem(nRow, 5, item);
}

void PhoneTreeAppModel::reloadModel()
{
    removeRows(0, rowCount());

    if (m_pDataTimer == nullptr) {
        m_pDataTimer = new QTimer(this);
        connect(m_pDataTimer, &QTimer::timeout, this, [=]() {
            m_pDataTimer->stop();

            if (m_pSocketThread == nullptr) {
                m_pSocketThread = new SocketThread;
                connect(m_pSocketThread, &SocketThread::sigAppInfoList, this, &PhoneTreeAppModel::slotDispResult); //  ->setPhoneInfo(m_strPhoneDevId, m_nDeviceType);
            }
            m_pSocketThread->setPhoneInfo(m_strPhoneDevId, m_nDeviceType);

            m_pSocketThread->start();
        });
    }
    m_pDataTimer->stop();
    m_pDataTimer->start(500);
}

//  数据加载
void PhoneTreeAppModel::slotDispResult(const QString &sDevid, const QList<PhoneAppInfo> &m_appInfoList)
{
    if (sDevid == m_strPhoneDevId) { // 过来的数据 是该设备
        foreach (auto app, m_appInfoList) {
            QVariant data;
            data.setValue(app);
            appendData(data);
        }
        emit sigLoadDataEnd();
    } else {
        if (m_pSocketThread == nullptr) {
            m_pSocketThread = new SocketThread;
            connect(m_pSocketThread, &SocketThread::sigAppInfoList, this, &PhoneTreeAppModel::slotDispResult); //  ->setPhoneInfo(m_strPhoneDevId, m_nDeviceType);
        }

        m_pSocketThread->setPhoneInfo(m_strPhoneDevId, m_nDeviceType);

        m_pSocketThread->start();
    }
}

void PhoneTreeAppModel::deleteRowByPath(const QString &, const QString &apkName)
{
    for (auto i = rowCount() - 1; i >= 0; i--) {
        QString strApkName = index(i, 5).data(APP_PACKAGE_NAME).toString();
        if (apkName.compare(strApkName) == 0) {
            removeRow(i);
            break;
        }
    }
}

void PhoneTreeAppModel::updateData(const QString &, const QVariant &)
{
}

qint64 PhoneTreeAppModel::getTotalSize()
{
    return m_nTotalSize;
}
