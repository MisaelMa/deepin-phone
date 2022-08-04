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

#include "MainWidget.h"
#include "phoneui/phonelistview.h"

#include <QBoxLayout>
#include <QVariant>
#include <QDebug>
#include <DFrame>
#include <QTimer>

#include "phonethread.h"
//#include "adbcmdthread.h"
#include "TitleWidget.h"
#include "MainRightWidget.h"
#include "MountService.h"
//#include "../phoneui/PhotoVideoGridWidget.h"
#include "GetDefaultFileIconTask.h"
#include "ThreadService.h"

#define ITEM_PHONE_ROLE (Qt::UserRole + 67)
#define ITEM_SHOW_ROLE (Qt::UserRole + 68)

MainWidget::MainWidget(DWidget *parent)
    : DWidget(parent)
{
    qDebug() << __FUNCTION__ << "       begin";

    initUI();

    qDebug() << __FUNCTION__ << "       end";
}

void MainWidget::addDev(const DEV_CONN_INFO &devInfo)
{
    PhoneInfo info;
    info.strPhoneID = devInfo.strDeviceId;
    info.type = devInfo.deviceType;
    info.usbType = devInfo.usbType; // add by zhangliangliang

    switch (int(devInfo.deviceType)) {
    case Mount_Ios:
        info.strDevName = "iPhone";
        if (!devInfo.bAuthorize) {
            // 未解锁
            info.status = STATUS_UNLOCK;
            break;
        }
        info.status = STATUS_LOADING;
        break;
    case Mount_Android:
        info.strDevName = "Android";
        if (!devInfo.bUSBDebug) {
            // 未开启调试模式
            info.status = STATUS_UNOPEN_DEBUGMODE;
            break;
        } else if (!devInfo.bAuthorize) {
            // 未开启USB授权
            info.status = STATUS_UNAUTHORIZE;
            break;
        }

        //手机连接方式不支持 //add by zhangliangliang
        if ((devInfo.bAuthorize && devInfo.usbType == USB_OTHER) || devInfo.usbType == USB_MIDI) {
            info.status = STATUS_UNOPEN_DEBUGMODE;
            break;
        }

        info.status = STATUS_LOADING;
        break;
    }
    if (info.status == STATUS_LOADING) {
        readPhoneInfo(info);
    }
    m_pPhoneListView->appendPhoneInfo(info);

#ifdef QT_DEBUG
    qDebug() << __LINE__ << __FUNCTION__ << devInfo.deviceType << devInfo.strDeviceId << info.status
             << devInfo.bAuthorize;
#endif
}

void MainWidget::delDev(const DEV_CONN_INFO &devInfo)
{
    m_pPhoneListView->removePhoneInfo(devInfo.strDeviceId);

    m_pMainRightWidget->onDeviceDisconnect(devInfo.strDeviceId);
}

void MainWidget::refreshDev(const DEV_CONN_INFO &devInfo)
{
    PhoneInfo info;
    info.strPhoneID = devInfo.strDeviceId;
    info.type = devInfo.deviceType;
    readPhoneInfo(info);
}

void MainWidget::initUI()
{
    QHBoxLayout *hMainLayout = new QHBoxLayout();
    hMainLayout->setMargin(0);
    hMainLayout->setSpacing(0);
    //左侧导航区
    DFrame *frm_left = new DFrame(this);
    frm_left->setFrameStyle(QFrame::NoFrame);
    frm_left->setLineWidth(0);
    frm_left->setFrameRounded(false);
    frm_left->setAutoFillBackground(true);

    QVBoxLayout *vLeftLayout = new QVBoxLayout(frm_left);
    vLeftLayout->setMargin(0);
    vLeftLayout->setSpacing(0);
    vLeftLayout->addSpacing(10);
    m_pPhoneListView = new PhoneListView(this);
    vLeftLayout->addWidget(m_pPhoneListView);

    hMainLayout->addWidget(frm_left);

    m_pMainRightWidget = new MainRightWidget(this);

    connect(m_pPhoneListView, &PhoneListView::mainItemChanged, m_pMainRightWidget,
            &MainRightWidget::slotMainItemChanged);
    connect(m_pPhoneListView, &PhoneListView::itemChanged, m_pMainRightWidget, &MainRightWidget::slotItemChanged);
    connect(m_pPhoneListView->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &MainWidget::slotListCurrentChanged);

    connect(m_pMainRightWidget, &MainRightWidget::sigDeviceReconnect, m_pPhoneListView,
            &PhoneListView::slotDeviceReconnect);
    connect(m_pMainRightWidget, &MainRightWidget::sigShowChanged, m_pPhoneListView, &PhoneListView::slotShowChanged);
    connect(m_pMainRightWidget, &MainRightWidget::sigUpdateBattery, this, &MainWidget::slotUpdateBattery);

    hMainLayout->addWidget(m_pMainRightWidget, 1);
    setLayout(hMainLayout);

    //启动设备连接检测线程
    QTimer::singleShot(500, this, SLOT(startDeviceConnectCheck()));
    //启动获取默认文件图标线程
    QTimer::singleShot(2000, this, SLOT(getDefaultFileIcon()));
}

void MainWidget::startDeviceConnectCheck()
{
    auto mount = MountService::getService();
    connect(mount, &MountService::sigDeviceListChanged, this, &MainWidget::slotDeviceListUpdate);
    connect(mount, &MountService::sigDeviceAuthorezeChanged, this, &MainWidget::slotDeviceAuthorizeUpdate);
    connect(mount, &MountService::sigNoDeviceList, this, &MainWidget::sigNoDeviceConnected);

    mount->startDeviceConnectCheck();
}

void MainWidget::getDefaultFileIcon()
{
    auto task = new GetDefaultFileIconTask(qApp);
    ThreadService::getService()->startTask(ThreadService::E_File_Icon_Task, task);
}

void MainWidget::slotDeviceListUpdate(const DEV_CONN_INFO &devInfo)
{
    emit sigDeviceListUpdate();

    if (devInfo.operate == OPT_ADD) {
        m_pMainRightWidget->insetDevUsbType(devInfo.strDeviceId, devInfo.usbType);

        addDev(devInfo);
    } else {
        m_pMainRightWidget->removeDevUsbType(devInfo.strDeviceId);

        delDev(devInfo);
    }
}

void MainWidget::slotDeviceAuthorizeUpdate(const DEV_CONN_INFO &devInfo)
{
    qDebug() << __LINE__ << __FUNCTION__ << devInfo.deviceType << "Authorize:" << devInfo.bAuthorize;

    PhoneInfo info;
    bool bFind = m_pPhoneListView->getPhoneInfo(devInfo.strDeviceId, info);
    if (!bFind)
        return;

    m_pMainRightWidget->insetDevUsbType(devInfo.strDeviceId, devInfo.usbType);
    //    m_devUsbType.insert(devInfo.strDeviceId, devInfo.usbType);

    if (devInfo.bAuthorize) {
        //已授权
        info.status = STATUS_LOADING;
    } else {
        if (devInfo.deviceType == Mount_Android) {
            //未授权
            info.status = STATUS_UNAUTHORIZE;
        } else {
            //未授权
            info.status = STATUS_UNLOCK;
        }
    }

    if (devInfo.deviceType == Mount_Android) {
        if (devInfo.usbType == USB_MIDI) {
            info.status = STATUS_UNOPEN_DEBUGMODE;
        } else {
            //已授权但是连接方式不是MTP或PTP，则显示连接指导界面
            if (devInfo.bAuthorize) {
                if (devInfo.usbType == USB_OTHER) {
                    info.status = STATUS_UNOPEN_DEBUGMODE;
                } else {
                    //                    AdbCmdThread *cmdThred = new AdbCmdThread(devInfo.strDeviceId, CMD_START_APP);
                    //                    connect(cmdThred, &AdbCmdThread::finished, cmdThred,
                    //                    &AdbCmdThread::deleteLater); cmdThred->start();

                    refreshDev(devInfo);
                }
            }
        }
    } else {
        if (info.status == STATUS_LOADING) {
            refreshDev(devInfo);
        }
    }

    m_pPhoneListView->resetPhoneInfo(info);

    PhoneInfo currInfo;
    m_pPhoneListView->getCurPhoneInfo(currInfo);

    if (info.strPhoneID == currInfo.strPhoneID) {
        currInfo.usbType = m_pMainRightWidget->getDevUsbType(info.strPhoneID);
        emit m_pPhoneListView->mainItemChanged(currInfo);
    }

    m_pMainRightWidget->onDeviceDisconnect(devInfo.strDeviceId);
}

void MainWidget::readPhoneInfo(const PhoneInfo &info)
{
    //  read info
    PhoneThread *t = new PhoneThread();
    connect(t, &PhoneThread::sigFindPhoneInfo, this, &MainWidget::onFindPhoneInfo);
    connect(t, &PhoneThread::finished, t, &PhoneThread::deleteLater);

    //  本地 安卓 版本
    t->setStrApkVersion(m_strLocalApkVersion);

    t->readPhoneInfo(info.strPhoneID, info.type);
    t->start();
}

//  左侧选项改变了
void MainWidget::slotListCurrentChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(deselected);

    auto indexList = selected.indexes();
    if (indexList.size() > 0) {
        auto index = indexList.at(0);

        auto curItem = static_cast<QStandardItemModel *>(m_pPhoneListView->model())->itemFromIndex(index);
        if (curItem) {
            auto parentItem = curItem->parent();

            PhoneInfo info;

            if (parentItem == nullptr) {
                info = curItem->data(ITEM_PHONE_ROLE).value<PhoneInfo>();

                emit m_pPhoneListView->mainItemChanged(info);

                m_pPhoneListView->setFocus();
            } else {
                // 子界面
                info = parentItem->data(ITEM_PHONE_ROLE).value<PhoneInfo>();
                E_Widget_Type type = E_Widget_Type(curItem->data(ITEM_SHOW_ROLE).toInt());

                emit m_pPhoneListView->itemChanged(info, type);
            }
        }
    }
}

void MainWidget::onFindPhoneInfo(const PhoneInfo &info)
{
    qDebug() << __LINE__ << __FUNCTION__ << info.strDevName;
    m_pPhoneListView->resetPhoneInfo(info);
}

void MainWidget::setLocalApkVersion(const QString &strInput)
{
    m_strLocalApkVersion = strInput;
}

void MainWidget::slotUpdateBattery(QString phoneId, int value)
{
    m_pPhoneListView->updatePhoneBattery(phoneId, value);
}
