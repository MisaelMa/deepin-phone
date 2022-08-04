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
#include "MainRightWidget.h"

#include <QDebug>

#include "phoneui/mainitemwidget.h"
#include "phoneui/devicesearchwidget.h"
#include "phoneui/PhoneInfoWidget.h"
#include "TitleWidget.h"
#include "RightTitleWidget.h"
#include "musicEbook/EBookWidget.h"
#include "musicEbook/MusicWidget.h"
#include "file/FileManageWidget.h"
#include "ImageVideo/PhotoVideoWidget.h"
#include "app/PhoneAppWidget.h"

MainRightWidget::MainRightWidget(DWidget *p)
    : DStackedWidget(p)
{
    qDebug() << __FUNCTION__ << "           begin";

    initUI();

    qDebug() << __FUNCTION__ << "           end";
}

/**
 * @brief MainRightWidget::onDeviceDisconnect   设备断开链接， 需要将相应的数据清空
 * @param devInfo 断开的设备信息搜索
 */
void MainRightWidget::onDeviceDisconnect(const QString &devInfo)
{
    if (m_pPhoneInfoWgt) {
        m_pPhoneInfoWgt->hidePopWidget();
    }

    if (m_pPhotoGridWgt) {
        m_pPhotoGridWgt->clearWidgetInfo(devInfo);
    }

    if (m_pVideoWidget) {
        m_pVideoWidget->clearWidgetInfo(devInfo);
    }

    if (m_ebookWgt != nullptr)
        m_ebookWgt->clearWidgetInfo(devInfo);

    if (m_musicWgt != nullptr)
        m_musicWgt->clearWidgetInfo(devInfo);

    if (m_pFileManagerWgt != nullptr)
        m_pFileManagerWgt->clearWidgetInfo(devInfo);

    if (m_pAndriodAppWidget != nullptr)
        m_pAndriodAppWidget->clearWidgetInfo(devInfo);

    if (m_pIosAppWidget != nullptr)
        m_pIosAppWidget->clearWidgetInfo(devInfo);
}

void MainRightWidget::insetDevUsbType(const QString &k, const UsbConnType &v)
{
    m_devUsbType.insert(k, v);
}

void MainRightWidget::removeDevUsbType(const QString &k)
{
    m_devUsbType.remove(k);
}

UsbConnType MainRightWidget::getDevUsbType(const QString &k)
{
    return m_devUsbType.value(k);
}

void MainRightWidget::slotMainItemChanged(const PhoneInfo &info)
{
    //    m_pPhoneInfoWgt->hidePopWidget();

    showDeviceInfoWidget(info);
}

/**
 * @brief MainRightWidget::showAppWidget    显示设备的应用信息
 * @param info
 */
void MainRightWidget::showAppWidget(const PhoneInfo &info)
{
    //    bool bSame = m_preType == info.type;
    if (info.type == Mount_Android) {
        if (m_pAndriodAppWidget == nullptr) {
            m_pAndriodAppWidget = new PhoneAppWidget(this);

            addWidget(m_pAndriodAppWidget);
        }

        m_pAndriodAppWidget->updateWidgetInfo(info);

        setCurrentWidget(m_pAndriodAppWidget);
    } else if (info.type == Mount_Ios) {
        if (m_pIosAppWidget == nullptr) {
            m_pIosAppWidget = new PhoneAppWidget(this); // modified by Airy

            addWidget(m_pIosAppWidget);
        }
        m_pIosAppWidget->updateWidgetInfo(info);

        setCurrentWidget(m_pIosAppWidget);
    }
    m_phoneInfo = info;
}

/**
 * @brief 显示选中的设备信息
 * @param info
 */
void MainRightWidget::showDeviceInfoWidget(const PhoneInfo &info)
{
    switch (int(info.status)) {
    case STATUS_CONNECTED:
        m_pPhoneInfoWgt->setPhoneInfo(info);
        setCurrentWidget(m_pPhoneInfoWgt);
        break;
    case STATUS_DISCONNECTED:
        if (m_pDisconnWgt == nullptr) {
            m_pDisconnWgt = new DisconnWgt(this);
            connect(m_pDisconnWgt, &DisconnWgt::reconnect, this, &MainRightWidget::sigDeviceReconnect);
            addWidget(m_pDisconnWgt);
        }
        setCurrentWidget(m_pDisconnWgt);
        break;
    case STATUS_LOADING:
        m_pDevSearchWgt->setState(false);
        setCurrentWidget(m_pDevSearchWgt);
        break;
    case STATUS_APK_INSTALLING:
        m_pDevSearchWgt->setState(true);
        setCurrentWidget(m_pDevSearchWgt);
        break;
    case STATUS_UNOPEN_DEBUGMODE:
        if (m_pDebugModeWgt == nullptr) {
            m_pDebugModeWgt = new DebugModeWidget(this);
            addWidget(m_pDebugModeWgt);
        }
        setCurrentWidget(m_pDebugModeWgt);
        break;
    case STATUS_UNAUTHORIZE:
        if (m_pUsbAuthorizeWgt == nullptr) {
            m_pUsbAuthorizeWgt = new UsbAuthorizeWidget(this);
            addWidget(m_pUsbAuthorizeWgt);
        }
        setCurrentWidget(m_pUsbAuthorizeWgt);
        break;
    case STATUS_UNLOCK:
        if (m_pUnlockWgt == nullptr) {
            m_pUnlockWgt = new UnlockWgt(this);
            addWidget(m_pUnlockWgt);
        }
        setCurrentWidget(m_pUnlockWgt);
        break;
    case STATUS_APKINSTALL_ERROR:

        if (m_pApkInstallErrorWgt == nullptr) {
            m_pApkInstallErrorWgt = new ApkInstallErrorWidget(this);
            addWidget(m_pApkInstallErrorWgt);
        }

        setCurrentWidget(m_pApkInstallErrorWgt);
        break;
    }

    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Left_Btn, false);
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Right_Btn, false);
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_New_Folder_Btn, false);
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Export_Btn, false);
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Import_Btn, false);
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Delete_Btn, false);
    RightTitleWidget::getInstance()->setWidgetBtnVisible(RightTitleWidget::E_Icon_View_Btn, false);
    RightTitleWidget::getInstance()->setWidgetBtnVisible(RightTitleWidget::E_List_View_Btn, false);
}

/**
 * @brief 显示选中的图片信息
 * @param info
 */
void MainRightWidget::showPhotoWidget(const PhoneInfo &info)
{
    if (m_pPhotoGridWgt == nullptr) {
        m_pPhotoGridWgt = new PhotoVideoWidget(PhotoVideoWidget::Photo, this);

        addWidget(m_pPhotoGridWgt);
    }
    setCurrentWidget(m_pPhotoGridWgt);
    m_pPhotoGridWgt->updateWidgetInfo(info);
}

/**
 * @brief 显示选中的视频
 * @param info
 */
void MainRightWidget::showVideoWidget(const PhoneInfo &info)
{
    if (m_pVideoWidget == nullptr) {
        m_pVideoWidget = new PhotoVideoWidget(PhotoVideoWidget::Video, this);

        addWidget(m_pVideoWidget);
    }

    setCurrentWidget(m_pVideoWidget);
    m_pVideoWidget->updateWidgetInfo(info);
}

/**
 * @brief 显示选中的音乐信息
 * @param info
 */
void MainRightWidget::showMusicWidget(PhoneInfo &info)
{
    if (m_musicWgt == nullptr) {
        m_musicWgt = new MusicWidget(this);
        //        connect(m_musicWgt, &EBookWidget::sigFindCount, this, &MainRightWidget::onFindCount);

        addWidget(m_musicWgt);
    }
    setCurrentWidget(m_musicWgt);
    info.usbType = m_devUsbType.value(info.strPhoneID);
    m_musicWgt->updateWidgetInfo(info);
}

/**
 * @brief 显示选中的电子书 数据
 * @param info
 */
void MainRightWidget::showBookWidget(PhoneInfo &info)
{
    if (m_ebookWgt == nullptr) {
        m_ebookWgt = new EBookWidget(this);

        addWidget(m_ebookWgt);
    }
    info.usbType = m_devUsbType.value(info.strPhoneID);
    m_ebookWgt->updateWidgetInfo(info);
    setCurrentWidget(m_ebookWgt);
}

/**
 * @brief 显示选中的文件信息
 * @param info
 */
void MainRightWidget::showFileWidget(PhoneInfo &info)
{
    if (m_pFileManagerWgt == nullptr) {
        m_pFileManagerWgt = new FileManageWidget(this);

        addWidget(m_pFileManagerWgt);
    }
    setCurrentWidget(m_pFileManagerWgt);
    info.usbType = m_devUsbType.value(info.strPhoneID);
    m_pFileManagerWgt->updateWidgetInfo(info);
}

void MainRightWidget::slotItemChanged(PhoneInfo &info, const E_Widget_Type &type)
{
    //    m_pPhoneInfoWgt->hidePopWidget();
    switch (int(type)) {
    case E_Widget_App:
        showAppWidget(info);
        break;
    case E_Widget_Photo:
        showPhotoWidget(info);
        break;
    case E_Widget_Video:
        showVideoWidget(info);
        break;
    case E_Widget_Music:
        showMusicWidget(info);
        break;
    case E_Widget_Book:
        showBookWidget(info);
        break;
    case E_Widget_File:
        showFileWidget(info);
        break;
    }
}

void MainRightWidget::slotCurrentChanged(int iIndex)
{
    auto w = widget(iIndex);
    if (w) {
        if (w->objectName() == "")
            return;

        auto cw = static_cast<CustomWidget *>(w);
        if (cw) {
            cw->setWidgetBtnState();
        }
    }
}

void MainRightWidget::initUI()
{
    m_pPhoneInfoWgt = new PhoneInfoWidget(this);
    addWidget(m_pPhoneInfoWgt);
    connect(m_pPhoneInfoWgt, &PhoneInfoWidget::showChanged, this, &MainRightWidget::sigShowChanged);
    connect(m_pPhoneInfoWgt, &PhoneInfoWidget::SignalUpdata, this, &MainRightWidget::SignalUpdata);
    connect(m_pPhoneInfoWgt, &PhoneInfoWidget::sigUpdatePhoneBattery, this, &MainRightWidget::sigUpdateBattery);

    m_pDevSearchWgt = new DeviceSearchWidget(this);
    addWidget(m_pDevSearchWgt);

    connect(this, &DStackedWidget::currentChanged, this, &MainRightWidget::slotCurrentChanged);
}
