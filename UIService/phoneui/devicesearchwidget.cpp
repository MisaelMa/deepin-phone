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
#include "devicesearchwidget.h"

#include <DSpinner>
#include <QVBoxLayout>
#include <QDebug>
#include <DApplicationHelper>

#include "TrObject.h"

DeviceSearchWidget::DeviceSearchWidget(DWidget *parent)
    : DWidget(parent)
{
    initUI();

    connect(DApplicationHelper::instance(), &DApplicationHelper::themeTypeChanged, this,
            &DeviceSearchWidget::onChangedTheme);

    DGuiApplicationHelper::ColorType ct = DApplicationHelper::instance()->themeType();
    onChangedTheme(ct);
}

void DeviceSearchWidget::initUI()
{
    m_iconImage = new DLabel;
    m_iconImage->setFixedSize(85, 245);

    m_spinner = new DSpinner(this);
    m_spinner->setFixedSize(QSize(20, 20));
    m_spinner->start();
    QVBoxLayout *vLayout = new QVBoxLayout();
    vLayout->addWidget(m_spinner, 0, Qt::AlignCenter);
    DWidget *layoutWidget = new DWidget();
    layoutWidget->setMinimumHeight(90);
    vLayout->addWidget(layoutWidget);
    m_iconImage->setLayout(vLayout);

    m_tipLabel = new DLabel(this);
    m_tipLabel->setFixedHeight(60);
    m_tipLabel->setWordWrap(true);
    m_tipLabel->setAlignment(Qt::AlignTop);
    m_tipLabel->setText(TrObject::getInstance()->getLabelText(Detecting)); //检测设备中...

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addStretch();
    centralLayout->addWidget(m_iconImage);
    centralLayout->setAlignment(m_iconImage, Qt::AlignHCenter);
    centralLayout->addWidget(m_tipLabel);
    centralLayout->setAlignment(m_tipLabel, Qt::AlignHCenter);
    centralLayout->addStretch();
    centralLayout->setSpacing(0);
    centralLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(centralLayout);
}

void DeviceSearchWidget::SpinnerWidgetStart()
{
    m_spinner->start();
    m_spinner->show();
}
void DeviceSearchWidget::SpinnerWidgetStop()
{
    m_spinner->stop();
    this->hide();
}

void DeviceSearchWidget::onChangedTheme(DGuiApplicationHelper::ColorType themeType)
{
    QString iconPath = "://";
    iconPath += ((themeType == DGuiApplicationHelper::LightType) ? "light/" : "dark/");
    iconPath += "phone.svg";

    m_iconImage->setPixmap(QPixmap(iconPath).scaled(84, 244));
}

void DeviceSearchWidget::setState(bool isInstalling)
{
    if (isInstalling)
        m_tipLabel->setText(TrObject::getInstance()->getLabelText(ApkInstalling));
    else
        m_tipLabel->setText(TrObject::getInstance()->getLabelText(Detecting));
}
