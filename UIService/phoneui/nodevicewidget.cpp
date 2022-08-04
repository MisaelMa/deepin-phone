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
#include "nodevicewidget.h"

#include <DFontSizeManager>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

#include "GlobalDefine.h"
#include "TrObject.h"

NoDeviceWidget::NoDeviceWidget(DWidget *parent)
    : DWidget(parent)
{
    initUI();
    initConnection();

    DGuiApplicationHelper::ColorType ct = DApplicationHelper::instance()->themeType();
    onChangedTheme(ct);
}

void NoDeviceWidget::initUI()
{
    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addStretch();

    m_iconImage = new DLabel;
    m_iconImage->setFixedSize(187, 134);
    //m_iconImage->setPixmap(QPixmap(phoneImg));
    centralLayout->addWidget(m_iconImage);
    centralLayout->setAlignment(m_iconImage, Qt::AlignHCenter);

    DLabel *tipNoDevLabel = new DLabel(this);
    tipNoDevLabel->setFixedHeight(30);
    tipNoDevLabel->setAlignment(Qt::AlignTop);
    tipNoDevLabel->setText(TrObject::getInstance()->getLabelText(No_device_connected)); //未检测到设备连接
    centralLayout->addSpacing(8);
    centralLayout->addWidget(tipNoDevLabel);
    centralLayout->setAlignment(tipNoDevLabel, Qt::AlignHCenter);

    DLabel *tipConnLabel = new DLabel(this);
    tipConnLabel->setFixedHeight(30);
    tipConnLabel->setAlignment(Qt::AlignTop);
    tipConnLabel->setText(TrObject::getInstance()->getLabelText(Connect_your_device_to_PC_with_USB_cables)); //请将移动设备用数据线连接电脑
    DFontSizeManager::instance()->bind(tipConnLabel, DFontSizeManager::T8);
    DPalette pa = DApplicationHelper::instance()->palette(tipConnLabel);
    pa.setBrush(DPalette::WindowText, pa.color(DPalette::TextTips));
    DApplicationHelper::instance()->setPalette(tipConnLabel, pa);

    centralLayout->addWidget(tipConnLabel);
    centralLayout->setAlignment(tipConnLabel, Qt::AlignHCenter);

    DLabel *tipHelpLabel = new DLabel(this);
    tipHelpLabel->setFixedHeight(30);
    tipHelpLabel->setAlignment(Qt::AlignTop);
    tipHelpLabel->setText(TrObject::getInstance()->getLabelText(Connected_but_not_recognized_Click_below_buttons)); //已连接设备但无法识别？请点这里
    DFontSizeManager::instance()->bind(tipHelpLabel, DFontSizeManager::T8);
    pa = DApplicationHelper::instance()->palette(tipConnLabel);
    pa.setBrush(DPalette::WindowText, pa.color(DPalette::TextTips));
    DApplicationHelper::instance()->setPalette(tipHelpLabel, pa);

    centralLayout->addWidget(tipHelpLabel);
    centralLayout->setAlignment(tipHelpLabel, Qt::AlignHCenter);

    QVBoxLayout *vIosLayout = new QVBoxLayout;
    m_iosButton = new DFloatingButton(this);
    m_iosButton->setAccessibleName("iosButton");
    m_iosButton->setIcon(QIcon::fromTheme("uospm_ios_icon"));
    m_iosButton->setFixedSize(54, 54);
    //    pa = DApplicationHelper::instance()->palette(m_iosButton);
    //    QColor color = QColor("#252525");
    //    pa.setColor(QPalette::Background, color);
    //    m_iosButton->setPalette(pa);
    m_iosButton->setBackgroundRole(DPalette::Background);
    //m_iosButton->setIcon(QIcon(QIcon(iconPath + "apple.svg")));
    m_iosButton->setIconSize(QSize(24, 24));
    vIosLayout->addWidget(m_iosButton);
    vIosLayout->setAlignment(vIosLayout, Qt::AlignHCenter);
    DLabel *iosLabel = new DLabel("iOS");
    iosLabel->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(iosLabel, DFontSizeManager::T8);
    pa = DApplicationHelper::instance()->palette(iosLabel);
    pa.setBrush(DPalette::WindowText, pa.color(DPalette::TextTips));
    DApplicationHelper::instance()->setPalette(iosLabel, pa);

    vIosLayout->addWidget(iosLabel);
    vIosLayout->setAlignment(iosLabel, Qt::AlignHCenter);
    vIosLayout->setSpacing(0);

    QVBoxLayout *vAndroidLayout = new QVBoxLayout;
    m_androidButton = new DFloatingButton(this);
    m_androidButton->setAccessibleName("androidButton");
    m_androidButton->setIcon(QIcon::fromTheme("uospm_android_icon"));

    m_androidButton->setFixedSize(54, 54);
    //    m_androidButton->setPalette(pa);
    m_androidButton->setBackgroundRole(DPalette::Background);
    //m_androidButton->setIcon(QIcon(iconPath + "android.svg"));
    m_androidButton->setIconSize(QSize(24, 24));
    QHBoxLayout *BtnLayout = new QHBoxLayout;
    BtnLayout->addWidget(m_androidButton);
    vAndroidLayout->addLayout(BtnLayout);
    vAndroidLayout->setAlignment(vAndroidLayout, Qt::AlignHCenter);
    DLabel *androidLabel = new DLabel("Android");
    androidLabel->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(androidLabel, DFontSizeManager::T8);
    pa = DApplicationHelper::instance()->palette(androidLabel);
    pa.setBrush(DPalette::WindowText, pa.color(DPalette::TextTips));
    DApplicationHelper::instance()->setPalette(androidLabel, pa);

    vAndroidLayout->addWidget(androidLabel);
    vAndroidLayout->setAlignment(androidLabel, Qt::AlignHCenter);
    vAndroidLayout->setSpacing(0);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->addLayout(vIosLayout);
    hLayout->setSpacing(30);
    hLayout->addLayout(vAndroidLayout);
    centralLayout->addLayout(hLayout);
    centralLayout->setAlignment(hLayout, Qt::AlignHCenter);

    centralLayout->addStretch();
    centralLayout->setSpacing(0);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(centralLayout);
}

void NoDeviceWidget::initConnection()
{
    connect(m_iosButton, &DFloatingButton::clicked, this, &NoDeviceWidget::slotIOSButtonClicked);
    connect(m_androidButton, &DFloatingButton::clicked, this, &NoDeviceWidget::slotAndroidButtonClicked);
    connect(DApplicationHelper::instance(), &DApplicationHelper::themeTypeChanged, this,
            &NoDeviceWidget::onChangedTheme);
}

void NoDeviceWidget::slotIOSButtonClicked()
{
    //    if (m_iosHelpDlg == nullptr) {
    auto m_iosHelpDlg = new DDialog(this);
    m_iosHelpDlg->setIcon(QIcon::fromTheme(PhoneAssistantModel::g_app_icon_name));
    m_iosHelpDlg->setMinimumSize(600, 430);

    QVBoxLayout *pVLayout = new QVBoxLayout();

    DLabel *labelTitle = new DLabel(this);
    labelTitle->setWordWrap(true);
    DFontSizeManager::instance()->bind(labelTitle, DFontSizeManager::T5, QFont::DemiBold);
    labelTitle->setText(TrObject::getInstance()->getLabelText(ios_Occasion_1));
    DLabel *labelInfo = new DLabel(this);
    labelInfo->setWordWrap(true);
    DFontSizeManager::instance()->bind(labelInfo, DFontSizeManager::T6);
    labelInfo->setText(TrObject::getInstance()->getLabelText(ios_Occasion_1_1));
    DPalette paLaybelDate = DApplicationHelper::instance()->palette(labelInfo);
    pVLayout->addWidget(labelTitle);
    pVLayout->addWidget(labelInfo);

    DLabel *labelTitle2 = new DLabel(this);
    labelTitle2->setWordWrap(true);
    DFontSizeManager::instance()->bind(labelTitle2, DFontSizeManager::T5, QFont::DemiBold);
    labelTitle2->setText(TrObject::getInstance()->getLabelText(ios_Occasion_2));
    DLabel *labelInfo2 = new DLabel(this);
    DFontSizeManager::instance()->bind(labelInfo2, DFontSizeManager::T6);
    labelInfo2->setWordWrap(true);
    labelInfo2->setText(TrObject::getInstance()->getLabelText(ios_Occasion_2_1));
    pVLayout->addWidget(labelTitle2);
    pVLayout->addWidget(labelInfo2);

    pVLayout->setSpacing(10);
    pVLayout->addStretch();

    DWidget *pWidget = new DWidget(this);
    pWidget->setLayout(pVLayout);

    m_iosHelpDlg->addContent(pWidget);

    m_iosHelpDlg->exec();
}

void NoDeviceWidget::slotAndroidButtonClicked()
{
    //    if (m_androidHelpDlg == nullptr) {
    auto m_androidHelpDlg = new DDialog(this);
    m_androidHelpDlg->setIcon(QIcon::fromTheme(PhoneAssistantModel::g_app_icon_name));
    m_androidHelpDlg->setMinimumSize(650, 510);

    QVBoxLayout *pVLayout = new QVBoxLayout();

    DLabel *labelTitle = new DLabel(this);
    labelTitle->setText(TrObject::getInstance()->getLabelText(android_1));
    labelTitle->setWordWrap(true);
    DFontSizeManager::instance()->bind(labelTitle, DFontSizeManager::T5, QFont::DemiBold);
    pVLayout->addWidget(labelTitle);
    DLabel *labelInfo = new DLabel(this);
    labelInfo->setWordWrap(true);
    DFontSizeManager::instance()->bind(labelInfo, DFontSizeManager::T6);
    labelInfo->setText(TrObject::getInstance()->getLabelText(android_2));
    pVLayout->addWidget(labelInfo);
    DLabel *labelInfo2 = new DLabel(this);
    labelInfo2->setWordWrap(true);
    DFontSizeManager::instance()->bind(labelInfo2, DFontSizeManager::T6);
    labelInfo2->setText(TrObject::getInstance()->getLabelText(android_3));
    pVLayout->addWidget(labelInfo2);
    DLabel *labelInfo3 = new DLabel(this);
    labelInfo3->setWordWrap(true);
    DFontSizeManager::instance()->bind(labelInfo3, DFontSizeManager::T6);
    labelInfo3->setText(TrObject::getInstance()->getLabelText(android_4));
    pVLayout->addWidget(labelInfo3);

    DLabel *labelTitle2 = new DLabel(this);
    labelTitle2->setWordWrap(true);
    DFontSizeManager::instance()->bind(labelTitle2, DFontSizeManager::T5, QFont::DemiBold);
    labelTitle2->setText(TrObject::getInstance()->getLabelText(android_5));
    pVLayout->addWidget(labelTitle2);
    DLabel *labelInfo21 = new DLabel(this);
    labelInfo21->setText(TrObject::getInstance()->getLabelText(android_6));
    labelInfo21->setWordWrap(true);
    DFontSizeManager::instance()->bind(labelInfo21, DFontSizeManager::T6);
    pVLayout->addWidget(labelInfo21);
    DLabel *labelInfo22 = new DLabel(this);
    labelInfo22->setText(TrObject::getInstance()->getLabelText(android_7));
    labelInfo22->setWordWrap(true);
    DFontSizeManager::instance()->bind(labelInfo22, DFontSizeManager::T6);
    pVLayout->addWidget(labelInfo22);
    DLabel *labelInfo23 = new DLabel(this);
    labelInfo23->setWordWrap(true);
    DFontSizeManager::instance()->bind(labelInfo23, DFontSizeManager::T6);
    labelInfo23->setText(TrObject::getInstance()->getLabelText(android_8));
    pVLayout->addWidget(labelInfo23);
    DLabel *labelInfo24 = new DLabel(this);
    labelInfo24->setWordWrap(true);
    DFontSizeManager::instance()->bind(labelInfo24, DFontSizeManager::T6);
    labelInfo24->setText(TrObject::getInstance()->getLabelText(android_9));
    pVLayout->addWidget(labelInfo24);
    DLabel *labelInfo25 = new DLabel(this);
    labelInfo25->setWordWrap(true);
    DFontSizeManager::instance()->bind(labelInfo25, DFontSizeManager::T6);
    labelInfo25->setText(TrObject::getInstance()->getLabelText(android_10));
    pVLayout->addWidget(labelInfo25);

    pVLayout->setSpacing(10);
    pVLayout->addStretch();
    DWidget *pWidget = new DWidget(this);
    pWidget->setLayout(pVLayout);

    m_androidHelpDlg->addContent(pWidget);
    //    }
    m_androidHelpDlg->exec();
}

//void NoDeviceWidget::closeHelpDialog()
//{
//    if (m_iosHelpDlg != nullptr && m_iosHelpDlg->isVisible()) {
//        m_iosHelpDlg->close();
//    }
//    if (m_androidHelpDlg != nullptr && m_androidHelpDlg->isVisible()) {
//        m_androidHelpDlg->close();
//    }
//}

void NoDeviceWidget::onChangedTheme(DGuiApplicationHelper::ColorType themeType)
{
    QString iconPath = "://";
    iconPath += ((themeType == DGuiApplicationHelper::LightType) ? "light/" : "dark/");

    m_iconImage->setPixmap(QPixmap(iconPath + "unconnected.svg"));
}
