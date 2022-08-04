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
#include "SpinnerWidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <DApplicationHelper>
#include <DFontSizeManager>
#include <DHorizontalLine>
#include <DApplication>

#include "TrObject.h"

SpinnerWidget::SpinnerWidget(DWidget *parent)
    : DBlurEffectWidget(parent)
{
    setAutoFillBackground(true);
    setBackgroundRole(DPalette::Base);

    initUI();

    connect(DApplicationHelper::instance(), &DApplicationHelper::themeTypeChanged,
            this, &SpinnerWidget::slotSetBackground);
    slotSetBackground();
}

void SpinnerWidget::slotSetBackground()
{
    if (DApplicationHelper::instance()->themeType() == Dtk::Gui::DGuiApplicationHelper::ColorType::LightType) {
        setMaskColor(QColor(252, 252, 252));
    } else {
        setMaskColor(QColor(32, 32, 32));
    }
}

//void SpinnerWidget::fontChangeDisplay()
//{
//    auto fontSize = DFontSizeManager::instance()->get(DFontSizeManager::T7);
//    if (m_lblName->text().contains("Loading")) {
//        if (fontSize.pixelSize() < 13)
//            setFixedHeight(30);
//        else if (fontSize.pixelSize() < 14)
//            setFixedHeight(33);
//        else if (fontSize.pixelSize() < 15)
//            setFixedHeight(34);
//        else if (fontSize.pixelSize() < 17)
//            setFixedHeight(36);
//        else
//            setFixedHeight(39);
//    } else {
//        if (fontSize.pixelSize() < 19)
//            setFixedHeight(30);
//        else {
//            setFixedHeight(32);
//        }
//    }
//}

void SpinnerWidget::initUI()
{
    auto m_lblName = new DLabel(this);
    m_lblName->setText(TrObject::getInstance()->getLabelText(Loading));
    m_lblName->setForegroundRole(DPalette::TextTips);
    DFontSizeManager::instance()->bind(m_lblName, DFontSizeManager::T7);

    m_spinner = new DSpinner(this);
    m_spinner->setFixedSize(QSize(20, 20));

    DWidget *spinnerWidget = new DWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(spinnerWidget);
    layout->addStretch();
    layout->addWidget(m_spinner);
    layout->addWidget(m_lblName);
    layout->addStretch();
    layout->setMargin(0);

    QVBoxLayout *pMainLayout = new QVBoxLayout(this);
    pMainLayout->setContentsMargins(0, 0, 0, 0);

    m_sShowTip = new DLabel(this);
    m_sShowTip->setAlignment(Qt::AlignCenter);
    m_sShowTip->setForegroundRole(DPalette::TextTips);
    DFontSizeManager::instance()->bind(m_sShowTip, DFontSizeManager::T7);

    m_pStackWidget = new QStackedWidget(this);
    m_pStackWidget->addWidget(spinnerWidget);
    m_pStackWidget->addWidget(m_sShowTip);

    DHorizontalLine *HorizontalLine = new DHorizontalLine(this);
    pMainLayout->addWidget(HorizontalLine);
    pMainLayout->addStretch();
    pMainLayout->addWidget(m_pStackWidget, 0, Qt::AlignBottom);
    pMainLayout->addSpacing(6);
}

void SpinnerWidget::spinnerStart()
{
    //    fontChangeDisplay();
    //    connect(qApp, &DApplication::fontChanged, [=]() {
    //        fontChangeDisplay();
    //    });

    m_bIsHidden = false;
    m_spinner->show();
    //    m_spinner->setEnabled(true);
    m_spinner->start();

    m_pStackWidget->setCurrentIndex(0);
}

void SpinnerWidget::spinnerStop()
{
    m_bIsHidden = true;
    m_spinner->hide();
    //    m_lblName->clear();
    m_spinner->stop();
}

void SpinnerWidget::setShowText(const QString &strText)
{
    m_spinner->hide();
    m_spinner->stop();
    m_pStackWidget->setCurrentIndex(1);

    m_sShowTip->setText(strText);
}
