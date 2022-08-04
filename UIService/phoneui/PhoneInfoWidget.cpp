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
#include "PhoneInfoWidget.h"

#include <DFontSizeManager>

#include "BatteryTask.h"
#include "PhoneUseTask.h"
#include "ThreadService.h"
#include "TrObject.h"

PhoneInfoWidget::PhoneInfoWidget(DWidget *parent)
    : BaseThemeWgt(parent)
{
    //    m_pArrowPhoneWgt = new ArrowPhoneInfo;
    initUI();
    refreshTheme();

    setBatteryTask();
}

void PhoneInfoWidget::hidePopWidget()
{
    auto list = this->findChildren<ArrowPhoneInfo *>();
    foreach (auto it, list) {
        delete it;
        it = nullptr;
    }
}

void PhoneInfoWidget::setPhoneInfo(const PhoneInfo &info)
{
    bool bSame = true;
    if (m_phoneInfo.strPhoneID != info.strPhoneID) {
        bSame = false;
    }
    m_phoneInfo = info;

    if (bSame == false) {
        lbl_SysInfo->setText("");
        m_pProgressBar->setValue(0);

        if (m_pBatteryTask) {
            m_pBatteryTask->setStrPhoneDeviceId(m_phoneInfo.strPhoneID, m_phoneInfo.type);
        }
        if (m_pPhoneUseTask) {
            m_pIconBtn->setEnabled(false);
            m_pPhoneUseTask->setStrPhoneDeviceId(m_phoneInfo.strPhoneID, m_phoneInfo.type);
        }
    }

    lbl_devName->setText(m_phoneInfo.strProductType);
    lbl_Sys->setText(QString("%1 %2").arg(m_phoneInfo.strSysName).arg(m_phoneInfo.strProductVersion));
    lbl_bat->setText(QString("%1%").arg(m_phoneInfo.battery));
    batWgt->setBatPct(m_phoneInfo.battery);

    if (info.type == Mount_Ios) {
        btn_app->setFixedSize(150, 150);
        btn_photo->setFixedSize(150, 150);
        btn_video->setFixedSize(150, 150);
        btn_file->setFixedSize(150, 150);
        rightTopLayout->addWidget(btn_app, 0, 0);
        rightTopLayout->addWidget(btn_photo, 0, 1);
        rightTopLayout->addWidget(btn_video, 1, 0);
        //        rightTopLayout->addWidget(btn_music, 1, 0);
        //        rightTopLayout->addWidget(btn_ebook, 1, 1);
        rightTopLayout->addWidget(btn_file, 1, 1);

        btn_music->hide();
        btn_ebook->hide();

        lbl_SysInfo->hide();
        //        lbl_SysText->hide();
        //        m_pProgressBar->hide();
        //        m_pIconBtn->hide();
    } else if (info.type == Mount_Android) {
        btn_app->setFixedSize(130, 130);
        btn_photo->setFixedSize(130, 130);
        btn_video->setFixedSize(130, 130);
        btn_file->setFixedSize(130, 130);
        rightTopLayout->addWidget(btn_app, 0, 0);
        rightTopLayout->addWidget(btn_photo, 0, 1);
        rightTopLayout->addWidget(btn_video, 0, 2);
        rightTopLayout->addWidget(btn_music, 1, 0);
        rightTopLayout->addWidget(btn_ebook, 1, 1);
        rightTopLayout->addWidget(btn_file, 1, 2);

        btn_music->show();
        btn_ebook->show();

        lbl_SysInfo->show();
    }
}

void PhoneInfoWidget::setUseTotalPer(const quint64 &nSysTotal, const quint64 &nSysFree, const QString &sPer)
{
    QString strText = QString("%1：%2GB/%3GB")
                          .arg(TrObject::getInstance()->getDeviceLabelText(Used))
                          .arg(double(nSysFree) / 1024 / 1024, 0, 'g', 3)
                          .arg(double(nSysTotal) / 1000 / 1000, 0, 'g', 3);
    lbl_SysInfo->setText(strText);
    lbl_SysText->setText(TrObject::getInstance()->getDeviceLabelText(Internal_Storage));
    if (nSysTotal != 0) {
        QString sProgesss = sPer;
        sProgesss = sProgesss.replace("%", "");
        int iValue = sProgesss.toInt();
        m_pProgressBar->setValue(iValue);
    } else {
        m_pProgressBar->setValue(0);
    }
}

void PhoneInfoWidget::setBatteryTask()
{
    m_pBatteryTask = new BatteryTask(qApp);
    connect(m_pBatteryTask, &BatteryTask::sigBatteryValue, this, &PhoneInfoWidget::slotSetPhoneBattery);
    ThreadService::getService()->startTask(ThreadService::E_Battery_Task, m_pBatteryTask);

    m_pPhoneUseTask = new PhoneUseTask(qApp);
    connect(m_pPhoneUseTask, &PhoneUseTask::sigPhoneUse, this,
            [=](const quint64 &nSysTotal, const quint64 &nSysFree, const QString &sUsePer) {
                setUseTotalPer(nSysTotal, nSysFree, sUsePer);

                m_pIconBtn->setEnabled(true);

                m_pPhoneUseTask->setStrPhoneDeviceId("", Mount_OTHER);
            });
    ThreadService::getService()->startTask(ThreadService::E_Phone_Use_Size_Task, m_pPhoneUseTask);
}

//  电量
void PhoneInfoWidget::slotSetPhoneBattery(const QString &phoneId, const int &v)
{
    if (phoneId == m_phoneInfo.strPhoneID) {
        lbl_bat->setText(QString("%1%").arg(v));
        batWgt->setBatPct(v);

        //保存到手机信息中
        emit sigUpdatePhoneBattery(m_phoneInfo.strPhoneID, v);
    }
}

void PhoneInfoWidget::restartBtnClicked()
{
    m_pIconBtn->setEnabled(false);

    m_pProgressBar->setValue(0);

    QString strText = QString("%1：0GB/0GB").arg(TrObject::getInstance()->getDeviceLabelText(Used));
    lbl_SysInfo->setText(strText);

    m_pPhoneUseTask->setStrPhoneDeviceId(m_phoneInfo.strPhoneID, m_phoneInfo.type);
}

void PhoneInfoWidget::InitLeftUi()
{
    frm_left = new DFrame(this);
    frm_left->setLineWidth(0);

    auto leftLayout = new QVBoxLayout;
    leftLayout->setMargin(0);
    leftLayout->setSpacing(0);
    leftLayout->addStretch();
    // dev
    lbl_devName = new DLabel(m_phoneInfo.strProductType, frm_left);
    DFontSizeManager::instance()->bind(lbl_devName, DFontSizeManager::T6, QFont::DemiBold);
    leftLayout->addWidget(lbl_devName);
    leftLayout->setAlignment(lbl_devName, Qt::AlignCenter);
    DLabel *lbl_phone = new DLabel(frm_left);
    appendPixmap(lbl_phone, "phone_main.svg");
    leftLayout->addWidget(lbl_phone);
    leftLayout->setAlignment(lbl_phone, Qt::AlignCenter);
    leftLayout->addSpacing(5);
    // text
    DLabel *lbl_phoneText = new DLabel(TrObject::getInstance()->getDeviceLabelText(device_Connected), frm_left);
    QVBoxLayout *phoneLayout = new QVBoxLayout(lbl_phone);
    phoneLayout->addStretch(2);
    phoneLayout->addWidget(lbl_phoneText, 5, Qt::AlignCenter);

    // Battery
    QGridLayout *batLayout = new QGridLayout;
    batLayout->setVerticalSpacing(3);
    batLayout->setHorizontalSpacing(3);
    batLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    //系统版本信息
    lbl_Sys = new DLabel(frm_left);
    DFontSizeManager::instance()->bind(lbl_Sys, DFontSizeManager::T8);

    //电量值
    lbl_bat = new DLabel(frm_left);
    DFontSizeManager::instance()->bind(lbl_bat, DFontSizeManager::T8);

    batWgt = new BatteryWgt(frm_left);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(lbl_bat);

    lbl_bat->setText(QString("100%"));
    batWgt->setBatPct(100);

    layout->addSpacing(5);
    layout->addWidget(batWgt);
    layout->addStretch();
    QVBoxLayout *BatVlayout = new QVBoxLayout();
    BatVlayout->addLayout(layout);
    batLayout->addWidget(lbl_Sys, 0, 0, 1, 3, Qt::AlignLeft | Qt::AlignBottom);
    batLayout->addLayout(BatVlayout, 1, 0, 1, 3, Qt::AlignLeft | Qt::AlignTop);
    auto m_pBtn_select = new DFloatingButton(DStyle::SP_SelectElement, frm_left);
    //    m_pBtn_select->setFocusPolicy(Qt::NoFocus);
    m_pBtn_select->setFixedSize(40, 40);
    m_pBtn_select->setBackgroundRole(QPalette::Button); //需要制定背景颜色角色才能生效
    DPalette pa = DApplicationHelper::instance()->palette(m_pBtn_select);
    QColor color = pa.frameBorder().color();
    if (DApplicationHelper::instance()->themeType() != Dtk::Gui::DGuiApplicationHelper::ColorType::LightType) {
        color.setAlpha(13);
    }
    pa.setColor(QPalette::Button, color);
    m_pBtn_select->setPalette(pa);
    batLayout->addWidget(m_pBtn_select, 0, 3, 2, 2);

    connect(DApplicationHelper::instance(), &DApplicationHelper::themeTypeChanged, this, [=]() {
        DPalette palette = DApplicationHelper::instance()->palette(m_pBtn_select);
        QColor color = palette.frameBorder().color();
        if (DApplicationHelper::instance()->themeType() != Dtk::Gui::DGuiApplicationHelper::ColorType::LightType) {
            color.setAlpha(13);
        }
        palette.setColor(QPalette::Button, color);
        m_pBtn_select->setPalette(palette);
    });

    connect(m_pBtn_select, &DFloatingButton::clicked, [=]() {
        auto m_pArrowPhoneInfo = new ArrowPhoneInfo(this);

        m_pArrowPhoneInfo->refreshUI(m_phoneInfo);
        QPoint p = mapToGlobal(m_pBtn_select->pos());
        m_pArrowPhoneInfo->show(p.x() + m_pBtn_select->width() / 2 + 9, p.y());
    });
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addStretch();
    hLayout->addLayout(batLayout);
    hLayout->addStretch();

    leftLayout->addLayout(hLayout);
    leftLayout->addStretch();

    frm_left->setLayout(leftLayout);
}

void PhoneInfoWidget::initUI()
{
    InitLeftUi();

    rightLayout = new QVBoxLayout;

    InitIconWidget();

    InitPhoneUseWidget();

    QHBoxLayout *centralLayout = new QHBoxLayout;
    centralLayout->addWidget(frm_left, 3);
    centralLayout->addLayout(rightLayout, 5);
    centralLayout->setSpacing(10);
    centralLayout->setMargin(10);
    setLayout(centralLayout);
}

void PhoneInfoWidget::InitIconWidget()
{
    rightTopLayout = new QGridLayout;
    rightTopLayout->setSpacing(30);

    QHBoxLayout *rightHLayout = new QHBoxLayout;
    rightHLayout->addStretch();
    rightHLayout->addLayout(rightTopLayout);
    rightHLayout->addStretch();

    QVBoxLayout *rightVLayout = new QVBoxLayout;
    rightVLayout->addStretch();
    rightVLayout->addLayout(rightHLayout);
    rightVLayout->addStretch();

    auto frm_rightTop = new DFrame(this);
    frm_rightTop->setLineWidth(0);
    frm_rightTop->setLayout(rightVLayout);
    rightLayout->addWidget(frm_rightTop, 4);

    btn_app = new VIconBtn(E_Widget_App, frm_rightTop);
    btn_app->setObjectName("E_Widget_App");
    btn_photo = new VIconBtn(E_Widget_Photo, frm_rightTop);
    btn_photo->setObjectName("E_Widget_Photo");
    btn_video = new VIconBtn(E_Widget_Video, frm_rightTop);
    btn_video->setObjectName("E_Widget_Video");
    btn_music = new VIconBtn(E_Widget_Music, frm_rightTop);
    btn_music->setObjectName("E_Widget_Music");
    btn_ebook = new VIconBtn(E_Widget_Book, frm_rightTop);
    btn_ebook->setObjectName("E_Widget_Book");
    btn_file = new VIconBtn(E_Widget_File, frm_rightTop);
    btn_file->setObjectName("E_Widget_File");

    connect(btn_app, &VIconBtn::IconBtnClicked, this, &PhoneInfoWidget::showChanged);
    connect(btn_photo, &VIconBtn::IconBtnClicked, this, &PhoneInfoWidget::showChanged);
    connect(btn_video, &VIconBtn::IconBtnClicked, this, &PhoneInfoWidget::showChanged);
    connect(btn_music, &VIconBtn::IconBtnClicked, this, &PhoneInfoWidget::showChanged);
    connect(btn_ebook, &VIconBtn::IconBtnClicked, this, &PhoneInfoWidget::showChanged);
    connect(btn_file, &VIconBtn::IconBtnClicked, this, &PhoneInfoWidget::showChanged);
}

void PhoneInfoWidget::InitPhoneUseWidget()
{
    DFrame *frm_rightBtm = new DFrame(this);
    frm_rightBtm->setLineWidth(0);

    QHBoxLayout *pRightBtmTopLayout = new QHBoxLayout;
    lbl_SysText = new DLabel(frm_rightBtm);

    DFontSizeManager::instance()->bind(lbl_SysText, DFontSizeManager::T6, QFont::DemiBold);
    pRightBtmTopLayout->addWidget(lbl_SysText);
    pRightBtmTopLayout->addStretch();

    lbl_SysInfo = new DLabel(frm_rightBtm);
    DFontSizeManager::instance()->bind(lbl_SysInfo, DFontSizeManager::T8);
    pRightBtmTopLayout->addWidget(lbl_SysInfo);

    m_pIconBtn = new DIconButton(frm_rightBtm);
    m_pIconBtn->setAccessibleName("system-restart-panel");
    m_pIconBtn->setFlat(true);
    m_pIconBtn->setIcon(QIcon::fromTheme("system-restart-panel"));
    connect(m_pIconBtn, &DIconButton::clicked, this, &PhoneInfoWidget::restartBtnClicked);

    pRightBtmTopLayout->addWidget(m_pIconBtn);
    pRightBtmTopLayout->setSpacing(8);

    m_pProgressBar = new DProgressBar(frm_rightBtm);
    m_pProgressBar->setFixedHeight(10);

    QVBoxLayout *pRightBtmMainLayout = new QVBoxLayout;
    pRightBtmMainLayout->setContentsMargins(22, 0, 22, 0);
    pRightBtmMainLayout->setSpacing(8);
    pRightBtmMainLayout->addStretch();
    pRightBtmMainLayout->addLayout(pRightBtmTopLayout);
    pRightBtmMainLayout->addWidget(m_pProgressBar);
    pRightBtmMainLayout->addStretch();

    frm_rightBtm->setLayout(pRightBtmMainLayout);
    rightLayout->addWidget(frm_rightBtm, 1);
}
