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
#ifndef MAINRIGHTWIDGET_H
#define MAINRIGHTWIDGET_H

#include <DWidget>
#include <DStackedWidget>

#include "defines.h"

DWIDGET_USE_NAMESPACE

class PhoneInfoWidget;
class UnlockWgt;
class DebugModeWidget;
class UsbAuthorizeWidget;
class DisconnWgt;
class DeviceSearchWidget;
class ApkInstallErrorWidget;

class CustomWidget;

class MainRightWidget : public DStackedWidget
{
    Q_OBJECT
public:
    explicit MainRightWidget(DWidget *p = nullptr);

signals:
    void sigDeviceReconnect();
    void sigShowChanged(const E_Widget_Type &);
    void SignalUpdata();
    void sigUpdateBattery(QString phoneId, int value);

public:
    /**
     * @brief MainRightWidget::onDeviceDisconnect   设备断开链接， 需要将相应的数据清空
     * @param devInfo 断开的设备信息搜索
     */
    void onDeviceDisconnect(const QString &);

public:
    void insetDevUsbType(const QString &, const UsbConnType &);
    void removeDevUsbType(const QString &);
    UsbConnType getDevUsbType(const QString &);

public slots:
    void slotMainItemChanged(const PhoneInfo &info);
    void slotItemChanged(PhoneInfo &info, const E_Widget_Type &type);
    void slotCurrentChanged(int);

private slots:
    void initUI();

private:
    void showDeviceInfoWidget(const PhoneInfo &info);
    void showAppWidget(const PhoneInfo &info);
    void showPhotoWidget(const PhoneInfo &info);
    void showVideoWidget(const PhoneInfo &info);
    void showMusicWidget(PhoneInfo &info);
    void showBookWidget(PhoneInfo &info);
    void showFileWidget(PhoneInfo &info);

private:
    QMap<QString, UsbConnType> m_devUsbType;

    CustomWidget *m_pAndriodAppWidget = nullptr;
    CustomWidget *m_pIosAppWidget = nullptr; // add by Airy

    CustomWidget *m_ebookWgt = nullptr;
    CustomWidget *m_musicWgt = nullptr;
    CustomWidget *m_pFileManagerWgt = nullptr;

    PhoneInfoWidget *m_pPhoneInfoWgt = nullptr;
    UnlockWgt *m_pUnlockWgt = nullptr;
    DebugModeWidget *m_pDebugModeWgt = nullptr;
    DisconnWgt *m_pDisconnWgt = nullptr;
    DeviceSearchWidget *m_pDevSearchWgt = nullptr;

    CustomWidget *m_pPhotoGridWgt = nullptr;
    CustomWidget *m_pVideoWidget = nullptr;

    UsbAuthorizeWidget *m_pUsbAuthorizeWgt = nullptr;
    ApkInstallErrorWidget *m_pApkInstallErrorWgt = nullptr;

    PhoneInfo m_phoneInfo;
};

#endif // MAINRIGHTWIDGET_H
