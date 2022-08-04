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
#ifndef PHONEINFOWIDGET_H
#define PHONEINFOWIDGET_H

#include "mainitemwidget.h"

// 手机界面
class BatteryTask;
class PhoneUseTask;

class PhoneInfoWidget : public BaseThemeWgt
{
    Q_OBJECT
public:
    explicit PhoneInfoWidget(DWidget *parent = nullptr);
    void hidePopWidget();
    void setPhoneInfo(const PhoneInfo &info);
signals:
    void showChanged(const E_Widget_Type &type);
    void SignalUpdata();
    void sigUpdatePhoneBattery(QString phoneId, int value);

private slots:
    void restartBtnClicked();
    void slotSetPhoneBattery(const QString &phoneId, const int &);

private:
    void initUI();
    void InitLeftUi();
    /**
     * @brief 初始化 按钮 布局widget
     */
    void InitIconWidget();

    /**
     * @brief 初始化 手机使用空间weidget
     */
    void InitPhoneUseWidget();
    void setUseTotalPer(const quint64 &nSysTotal, const quint64 &nSysFree, const QString &);
    void setBatteryTask();

private:
    PhoneInfo m_phoneInfo;

    DLabel *lbl_devName;
    DLabel *lbl_Sys;
    DLabel *lbl_bat;
    DLabel *lbl_SysInfo; // add by Airy
    DLabel *lbl_SysText; //add by Airy
    DProgressBar *m_pProgressBar; //add by hx
    DIconButton *m_pIconBtn; //add by hx
    BatteryWgt *batWgt;
    QGridLayout *rightTopLayout;
    VIconBtn *btn_app;
    VIconBtn *btn_photo;
    VIconBtn *btn_video;
    VIconBtn *btn_music;
    VIconBtn *btn_ebook;
    VIconBtn *btn_file;

    DFrame *frm_left = nullptr;
    QVBoxLayout *rightLayout = nullptr;

    PhoneUseTask *m_pPhoneUseTask = nullptr;
    BatteryTask *m_pBatteryTask = nullptr;
};

#endif // PHONEINFOWIDGET_H
