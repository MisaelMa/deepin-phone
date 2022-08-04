/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co., Ltd.
 *
 * Author:     yelei <yelei@uniontech.com>
 * Maintainer: yelei <yelei@uniontech.com>
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
#include "mainitemwidget.h"

#include <QBoxLayout>
#include <DSuggestButton>
#include <DLabel>
#include <DApplicationHelper>
#include <DFrame>
#include <QPaintEvent>
#include <QDebug>
#include <DProgressBar>
#include <DFontSizeManager>
#include <QThreadPool>

#include "TrObject.h"
#include "utils.h"

BaseThemeWgt::BaseThemeWgt(DWidget *parent)
    : DWidget(parent)
{
    connect(DApplicationHelper::instance(), &DApplicationHelper::themeTypeChanged, this, &BaseThemeWgt::onChangedTheme);
}

void BaseThemeWgt::refreshTheme()
{
    DGuiApplicationHelper::ColorType ct = DApplicationHelper::instance()->themeType();
    onChangedTheme(ct);
}

void BaseThemeWgt::appendPixmap(DLabel *lbl, QString strPixName)
{
    m_mapPix.insert(lbl, strPixName);
}

void BaseThemeWgt::onChangedTheme(DGuiApplicationHelper::ColorType themeType)
{
    QMap<DLabel *, QString>::iterator it;
    for (it = m_mapPix.begin(); it != m_mapPix.end(); ++it) {
        QString strPath = "://";
        strPath += ((themeType == DGuiApplicationHelper::LightType) ? "light/" : "dark/");
        strPath += it.value();
        QPixmap p = Utils::getResourcePixmap(strPath);
        it.key()->setPixmap(p);
    }
}

UnlockWgt::UnlockWgt(DWidget *parent)
    : BaseThemeWgt(parent)
{
    initUI();
    refreshTheme();
}

void UnlockWgt::initUI()
{
    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addStretch();

    DLabel *iconImage = new DLabel(this);
    appendPixmap(iconImage, "appleAuthorize.svg");
    centralLayout->addWidget(iconImage);
    DLabel *lbl_text = new DLabel(TrObject::getInstance()->getDeviceLabelText(Unlock_your_phone_and_tap_Trust), this);
    lbl_text->setAlignment(Qt::AlignCenter);
    centralLayout->addWidget(lbl_text);

    centralLayout->setAlignment(iconImage, Qt::AlignHCenter);
    centralLayout->addStretch();
    centralLayout->setSpacing(20);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(centralLayout);
}

DebugModeWidget::DebugModeWidget(DWidget *parent)
    : BaseThemeWgt(parent)
{
    initUI();
    refreshTheme();
}

void DebugModeWidget::initUI()
{
    QHBoxLayout *centralLayout = new QHBoxLayout;
    centralLayout->addStretch();

    DLabel *iconImage = new DLabel(this);
    appendPixmap(iconImage, "debugmode.svg");
    centralLayout->addWidget(iconImage);

    QVBoxLayout *l_right = new QVBoxLayout;
    DLabel *lbl_lvl01 = new DLabel(this);
    lbl_lvl01->setText(TrObject::getInstance()->getDeviceLabelText(Enable_USB_debugging_on_your_phone));
    DFontSizeManager::instance()->bind(lbl_lvl01, DFontSizeManager::T5, QFont::DemiBold);
    DLabel *lbl_lvl11 = new DLabel(this);
    lbl_lvl11->setText(TrObject::getInstance()->getDeviceLabelText(device_debug_1));
    DFontSizeManager::instance()->bind(lbl_lvl11, DFontSizeManager::T6, QFont::DemiBold);
    DLabel *lbl_lvl21 = new DLabel(this);
    lbl_lvl21->setWordWrap(true);
    lbl_lvl21->setText(TrObject::getInstance()->getDeviceLabelText(device_debug_2));

    DLabel *lbl_lvl12 = new DLabel(this);
    lbl_lvl12->setText(TrObject::getInstance()->getDeviceLabelText(device_debug_3));
    DFontSizeManager::instance()->bind(lbl_lvl12, DFontSizeManager::T6, QFont::DemiBold);
    DLabel *lbl_lvl22 = new DLabel(this);
    lbl_lvl22->setWordWrap(true);
    //    lbl_lvl22->setFixedWidth(250);
    lbl_lvl22->setText(TrObject::getInstance()->getDeviceLabelText(device_debug_4));

    l_right->addStretch();
    l_right->addWidget(lbl_lvl01);
    l_right->addSpacing(5);
    l_right->addWidget(lbl_lvl11);
    l_right->addSpacing(5);
    l_right->addWidget(lbl_lvl21);
    l_right->addSpacing(20);
    l_right->addWidget(lbl_lvl12);
    l_right->addSpacing(5);
    l_right->addWidget(lbl_lvl22);

    l_right->addStretch();
    l_right->setSpacing(0);
    l_right->setContentsMargins(0, 0, 0, 0);
    centralLayout->addLayout(l_right);
    centralLayout->addSpacing(100);
    centralLayout->addStretch();
    centralLayout->setSpacing(50);
    centralLayout->setContentsMargins(127, 0, 0, 0);
    setLayout(centralLayout);
}

UsbAuthorizeWidget::UsbAuthorizeWidget(DWidget *parent)
    : BaseThemeWgt(parent)
{
    initUI();
    refreshTheme();
}

void UsbAuthorizeWidget::initUI()
{
    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addStretch();

    DLabel *iconImage = new DLabel(this);
    appendPixmap(iconImage, "androidAuthorize.svg");
    centralLayout->addWidget(iconImage);
    centralLayout->setAlignment(iconImage, Qt::AlignHCenter);

    DLabel *tipLabel = new DLabel(this);
    tipLabel->setText(TrObject::getInstance()->getDeviceLabelText(device_authorize));
    tipLabel->setWordWrap(true);
    tipLabel->setFixedHeight(120);

    centralLayout->addWidget(tipLabel);
    centralLayout->setAlignment(tipLabel, Qt::AlignHCenter);
    centralLayout->addStretch();
    centralLayout->setSpacing(0);
    centralLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(centralLayout);
}

ApkInstallErrorWidget::ApkInstallErrorWidget(QWidget *parent)
    : BaseThemeWgt(parent)
{
    initUI();
    refreshTheme();
}

void ApkInstallErrorWidget::initUI()
{
    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addStretch();

    DLabel *iconImage = new DLabel(this);
    appendPixmap(iconImage, "apkinstallerror.svg");
    centralLayout->addWidget(iconImage);
    centralLayout->setAlignment(iconImage, Qt::AlignHCenter);

    DLabel *tipLabel = new DLabel(this);
    tipLabel->setText(TrObject::getInstance()->getDeviceLabelText(AppInstallError));
    tipLabel->setWordWrap(true);
    tipLabel->setFixedHeight(120);

    centralLayout->addWidget(tipLabel);
    centralLayout->setAlignment(tipLabel, Qt::AlignHCenter);
    centralLayout->addStretch();
    centralLayout->setSpacing(0);
    centralLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(centralLayout);
}

DisconnWgt::DisconnWgt(QWidget *parent)
    : BaseThemeWgt(parent)
{
    initUI();
    refreshTheme();
}

void DisconnWgt::initUI()
{
    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addStretch();

    DLabel *iconImage = new DLabel(this);
    appendPixmap(iconImage, "connect.svg");
    centralLayout->addWidget(iconImage);
    DLabel *lbl_text = new DLabel(TrObject::getInstance()->getDeviceLabelText(Connect_your_mobile_device), this);
    lbl_text->setAlignment(Qt::AlignCenter);
    centralLayout->addWidget(lbl_text);
    DSuggestButton *btn_conn = new DSuggestButton(TrObject::getInstance()->getDeviceLabelText(Connect_Now), this);
    btn_conn->setFixedSize(302, 36);
    centralLayout->addWidget(btn_conn);
    centralLayout->setAlignment(btn_conn, Qt::AlignCenter);

    centralLayout->setAlignment(iconImage, Qt::AlignHCenter);
    centralLayout->addStretch();
    centralLayout->setSpacing(20);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(centralLayout);

    connect(btn_conn, &DSuggestButton::clicked, [this]() { emit reconnect(); });
}

BatteryWgt::BatteryWgt(DWidget *parent)
    : BaseThemeWgt(parent)
{
    this->setFixedSize(26, 11);
    connect(DApplicationHelper::instance(), &DApplicationHelper::themeTypeChanged, this, &BatteryWgt::onChangedTheme);
    DGuiApplicationHelper::ColorType ct = DApplicationHelper::instance()->themeType();
    onChangedTheme(ct);
}

void BatteryWgt::setBatPct(int nPct)
{
    if (nPct > 100)
        nPct = 100;
    m_nBatPct = float(nPct) / float(100);
}

void BatteryWgt::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    //fix48263,绘制电池设置像素映射,抗锯齿
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    QRect rect = this->rect();
    rect.setY(1);
    painter.drawPixmap(rect, m_pixmap);
    QBrush brBat = QBrush(QColor("#09CD28"));
    painter.setBrush(brBat);
    painter.setPen(Qt::NoPen);

    const int startPos = 4;
    const int endPos = -8;

    int nTotal = this->width() - startPos - qAbs(endPos);
    int nSurplusStart = nTotal - int(m_nBatPct * nTotal);

    rect.adjust(startPos + nSurplusStart, 2, endPos, -2);
    painter.drawRect(rect);
}

void BatteryWgt::onChangedTheme(DGuiApplicationHelper::ColorType themeType)
{
    QString strTheme = ((themeType == DGuiApplicationHelper::LightType) ? "light/" : "dark/");
    m_pixmap = Utils::getResourcePixmap(QString("://%1/battery.svg").arg(strTheme));
}

VIconBtn::VIconBtn(const E_Widget_Type &type, DWidget *parent)
    : DPushButton(parent)
    , m_type(type)
{
    m_backgroundColor = DApplicationHelper::instance()->palette(this).itemBackground().color();
    //    this->setFocusPolicy(Qt::NoFocus);
    this->setFixedSize(130, 130);
    connect(this, &VIconBtn::clicked, this, &VIconBtn::onBtnClicked);

    DFontSizeManager::instance()->bind(this, DFontSizeManager::T8);

    auto bLayout = new QVBoxLayout(this);
    bLayout->setContentsMargins(0, 0, 0, 0);
    bLayout->setSpacing(10);

    m_pLabelPixmap = new DLabel(this);
    m_pLabelPixmap->setFixedSize(QSize(68, 68));

    bLayout->addStretch();
    bLayout->addWidget(m_pLabelPixmap, 0, Qt::AlignCenter);

    m_pLabelText = new DLabel(this);
    bLayout->addWidget(m_pLabelText, 0, Qt::AlignCenter);
    bLayout->addStretch();

    connect(DApplicationHelper::instance(), &DApplicationHelper::themeTypeChanged, this, &VIconBtn::onChangedTheme);
    DGuiApplicationHelper::ColorType ct = DApplicationHelper::instance()->themeType();
    onChangedTheme(ct);
}

bool VIconBtn::event(QEvent *e)
{
    switch (int(e->type())) {
    case QEvent::MouseButtonPress:
        m_backgroundColor = QColor(0, 0, 0, 26);
        break;
    case QEvent::MouseButtonRelease:
        m_backgroundColor = DApplicationHelper::instance()->palette(this).itemBackground().color();
        break;
    case QEvent::HoverEnter:
        m_backgroundColor = QColor(0, 0, 0, 13);
        break;
    case QEvent::HoverLeave:
        m_backgroundColor = DApplicationHelper::instance()->palette(this).itemBackground().color();
        break;
    }
    return DPushButton::event(e);
}

void VIconBtn::onBtnClicked()
{
    emit IconBtnClicked(m_type);
}

void VIconBtn::onChangedTheme(DGuiApplicationHelper::ColorType)
{
    QPixmap m_icon;
    QString m_text = "";
    switch (int(m_type)) {
    case E_Widget_App:
        m_icon = QIcon::fromTheme("uospm_item_app").pixmap(68, 68);
        m_text = TrObject::getInstance()->getListViewText(List_App);
        break;
    case E_Widget_Photo:
        m_icon = QIcon::fromTheme("uospm_item_picture").pixmap(68, 68);
        m_text = TrObject::getInstance()->getListViewText(List_Photo);
        break;
    case E_Widget_Video:
        m_icon = QIcon::fromTheme("uospm_item_video").pixmap(68, 68);
        m_text = TrObject::getInstance()->getListViewText(List_Video);
        break;
    case E_Widget_Music:
        m_icon = QIcon::fromTheme("uospm_item_music").pixmap(68, 68);
        m_text = TrObject::getInstance()->getListViewText(List_Song);
        break;
    case E_Widget_Book:
        m_icon = QIcon::fromTheme("uospm_item_ebook").pixmap(68, 68);
        m_text = TrObject::getInstance()->getListViewText(List_eBook);
        break;
    case E_Widget_File:
        m_icon = QIcon::fromTheme("uospm_item_file").pixmap(68, 68);
        m_text = TrObject::getInstance()->getListViewText(List_File);
        break;
    }

    m_pLabelPixmap->setPixmap(m_icon);
    m_pLabelText->setText(m_text);

    m_backgroundColor = DApplicationHelper::instance()->palette(this).itemBackground().color();
}

void VIconBtn::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);

    painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::Antialiasing);

    painter.save();
    painter.setBrush(m_backgroundColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(this->rect(), 18, 18);
    painter.restore();
}

ArrowPhoneInfo::ArrowPhoneInfo(DWidget *parent)
    : DArrowRectangle(DArrowRectangle::ArrowBottom, DArrowRectangle::FloatWindow, parent)
{
    //    setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowFlags(Qt::Popup);
    //    this->setMargin(0);
    //    this->setFixedSize(260, 300);
    this->setWindowOpacity(0.8);
    //    this->setShadowBlurRadius(80);
    this->setRadius(10);
    this->setFixedWidth(260);

    m_mainLayout = new QVBoxLayout;
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(10, 10, 10, 20);
    this->setLayout(m_mainLayout);
}

void ArrowPhoneInfo::refreshUI(PhoneInfo &info)
{
    DLabel *lbl_devInfo = new DLabel(TrObject::getInstance()->getArrowPhoneInfoText(device_info), this);
    DFontSizeManager::instance()->bind(lbl_devInfo, DFontSizeManager::T6);
    //    m_mainLayout->addSpacing(5);
    m_mainLayout->addWidget(lbl_devInfo, 0, Qt::AlignCenter);
    //    m_mainLayout->addSpacing(5);

    QGridLayout *layout = new QGridLayout;
    layout->setSpacing(10);
    layout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    layout->setMargin(10);
    DFrame *frm_main = new DFrame(this);
    frm_main->setLayout(layout);
    m_mainLayout->addWidget(frm_main, 1);
    //    m_mainLayout->addSpacing(10);

    int nIndex = 0;
    if (!info.strDevName.isEmpty()) {
        DLabel *lbl_text = converElideLbl(TrObject::getInstance()->getArrowPhoneInfoText(device_info_Brand));

        layout->addWidget(lbl_text, nIndex, 0);

        DLabel *infoLabel = converElideLbl(info.strBrand);
        layout->addWidget(infoLabel, nIndex, 1);
        ++nIndex;
    }
    if (!info.strProductType.isEmpty()) {
        DLabel *lbl_text = converElideLbl(TrObject::getInstance()->getArrowPhoneInfoText(device_info_Model));
        layout->addWidget(lbl_text, nIndex, 0);

        DLabel *infoLabel = converElideLbl(info.strProductType);
        layout->addWidget(infoLabel, nIndex, 1);
        ++nIndex;
    }
    if (!info.strSysName.isEmpty()) {
        DLabel *lbl_text = converElideLbl(TrObject::getInstance()->getArrowPhoneInfoText(device_info_system_name));

        layout->addWidget(lbl_text, nIndex, 0);

        DLabel *infoLabel = converElideLbl(info.strSysName);
        layout->addWidget(infoLabel, nIndex, 1);
        ++nIndex;
    }
    if (!info.strProductVersion.isEmpty()) {
        DLabel *lbl_text = converElideLbl(TrObject::getInstance()->getArrowPhoneInfoText(device_info_system_version));

        layout->addWidget(lbl_text, nIndex, 0);

        DLabel *infoLabel = converElideLbl(info.strProductVersion);
        layout->addWidget(infoLabel, nIndex, 1);
        ++nIndex;
    }
    if (!info.strRegionInfo.isEmpty()) {
        DLabel *lbl_text = converElideLbl(TrObject::getInstance()->getArrowPhoneInfoText(device_info_c_r));

        layout->addWidget(lbl_text, nIndex, 0);

        DLabel *infoLabel = converElideLbl(info.strRegionInfo);
        layout->addWidget(infoLabel, nIndex, 1);
        ++nIndex;
    }
    if (!info.strDiskFree.isEmpty()) {
        DLabel *lbl_text = converElideLbl(TrObject::getInstance()->getArrowPhoneInfoText(device_info_Storage));

        layout->addWidget(lbl_text, nIndex, 0);

        DLabel *infoLabel = converElideLbl(info.strDiskFree);
        layout->addWidget(infoLabel, nIndex, 1);
        ++nIndex;
    }
    if (info.nMemTotal != 0) {
        DLabel *lbl_text = converElideLbl(TrObject::getInstance()->getArrowPhoneInfoText(device_info_RAM));

        layout->addWidget(lbl_text, nIndex, 0);
        QString strText = QString("%1GB/%2GB")
                              .arg(double(info.nMemTotal - info.nMemFree) / 1000 / 1000, 0, 'g', 3)
                              .arg(double(info.nMemTotal) / 1000 / 1000, 0, 'g', 3);

        DLabel *infoLabel = converElideLbl(strText);
        layout->addWidget(infoLabel, nIndex, 1);
        ++nIndex;
    }
    adjustSize();
}

DLabel *ArrowPhoneInfo::converElideLbl(const QString &text)
{
    DLabel *infoLabel = new DLabel(this);
    DFontSizeManager::instance()->bind(infoLabel, DFontSizeManager::T8);

    QFontMetrics fontWidth(infoLabel->font()); //得到每个字符的宽度
    QString elideNote = fontWidth.elidedText(text, Qt::ElideRight, infoLabel->width());
    infoLabel->setText(elideNote); //显示省略好的字符串
    infoLabel->setToolTip(text); //设置tooltips
    return infoLabel;
}
