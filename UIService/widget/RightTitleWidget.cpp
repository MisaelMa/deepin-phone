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
#include "RightTitleWidget.h"

#include <DApplicationHelper>
#include <QButtonGroup>
#include <QMutex>

#include "TrObject.h"

RightTitleWidget *RightTitleWidget::g_instance = nullptr;

RightTitleWidget::RightTitleWidget(DWidget *parent)
    : DWidget(parent)
{
    setAccessibleName("RightTitleWidget");
    //  水平布局
    m_pMainLayout = new QHBoxLayout(this);
    m_pMainLayout->setContentsMargins(0, 0, 0, 0);

    InitToolViewBtn();
}

RightTitleWidget *RightTitleWidget::getInstance()
{
    if (g_instance == nullptr) {
        QMutex mutex;
        QMutexLocker locker(&mutex);
        if (g_instance == nullptr) {
            g_instance = new RightTitleWidget;
        }
    }
    return g_instance;
}

//  按钮显示、隐藏 控制
void RightTitleWidget::setWidgetBtnVisible(const int &nId, const bool &bVis)
{
    auto it = m_nIdAndBtnMap.find(nId);
    while (it != m_nIdAndBtnMap.end()) {
        it.value()->setVisible(bVis);
        break;
    }
}

//  按钮 enable 控制
void RightTitleWidget::setWidgetBtnEnable(const int &nId, const bool &bEnable)
{
    auto it = m_nIdAndBtnMap.find(nId);
    while (it != m_nIdAndBtnMap.end()) {
        it.value()->setEnabled(bEnable);
        break;
    }
}

void RightTitleWidget::setWidgetBtnChecked(const int &nId, const bool &bCheck)
{
    auto it = m_nIdAndBtnMap.find(nId);
    while (it != m_nIdAndBtnMap.end()) {
        if (it.value()->isCheckable()) {
            it.value()->setChecked(bCheck);
        }
        break;
    }
}

void RightTitleWidget::InitToolViewBtn()
{
    auto onlyBtn = new QButtonGroup(this);
    {
        auto btn = new DToolButton(this);
        btn->setAccessibleName("iconview");
        btn->setBackgroundRole(DPalette::Button);
        connect(btn, &DToolButton::clicked, this, [=]() {
            emit sigBtnClicked(E_Icon_View_Btn);
        });

        btn->setIcon(QIcon::fromTheme("uospm_iconview"));

        btn->setCheckable(true);
        btn->setFixedSize(QSize(36, 36));
        btn->setIconSize(QSize(36, 36));
        btn->hide();
        m_pMainLayout->addWidget(btn);
        onlyBtn->addButton(btn);

        m_nIdAndBtnMap.insert(E_Icon_View_Btn, btn);
    }
    {
        auto btn = new DToolButton(this);
        btn->setAccessibleName("listview");
        btn->setBackgroundRole(DPalette::Button);
        connect(btn, &DToolButton::clicked, this, [=]() {
            emit sigBtnClicked(E_List_View_Btn);
        });
        btn->setCheckable(true);

        btn->setIcon(QIcon::fromTheme("uospm_listview"));
        btn->setFixedSize(QSize(36, 36));
        btn->setIconSize(QSize(36, 36));
        btn->hide();

        m_nIdAndBtnMap.insert(E_List_View_Btn, btn);

        onlyBtn->addButton(btn);

        m_pMainLayout->addWidget(btn);
    }
}
