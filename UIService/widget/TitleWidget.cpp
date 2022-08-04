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
#include "TitleWidget.h"

#include <DApplicationHelper>
#include <QDebug>
#include <QButtonGroup>
#include <QMutex>

#include "TrObject.h"

TitleWidget *TitleWidget::g_instance = nullptr;

TitleWidget::TitleWidget(DWidget *parent)
    : DWidget(parent)
{
    //  水平布局
    m_pMainLayout = new QHBoxLayout(this);
    m_pMainLayout->setContentsMargins(0, 0, 0, 0);

    InitLeftWidget();

    m_pMainLayout->addStretch();
}

TitleWidget *TitleWidget::getInstance()
{
    if (g_instance == nullptr) {
        QMutex mutex;
        QMutexLocker locker(&mutex);
        if (g_instance == nullptr) {
            g_instance = new TitleWidget;
        }
    }
    return g_instance;
}

//  按钮显示、隐藏 控制
void TitleWidget::setWidgetBtnVisible(const int &nId, const bool &bVis)
{
    auto it = m_nIdAndBtnMap.find(nId);
    while (it != m_nIdAndBtnMap.end()) {
        it.value()->setVisible(bVis);
        break;
    }
}

//  按钮 enable 控制
void TitleWidget::setWidgetBtnEnable(const int &nId, const bool &bEnable)
{
    auto it = m_nIdAndBtnMap.find(nId);
    while (it != m_nIdAndBtnMap.end()) {
        it.value()->setEnabled(bEnable);
        break;
    }
}

void TitleWidget::setWidgetBtnChecked(const int &nId, const bool &bCheck)
{
    auto it = m_nIdAndBtnMap.find(nId);
    while (it != m_nIdAndBtnMap.end()) {
        if (it.value()->isCheckable()) {
            it.value()->setChecked(bCheck);
        }
        break;
    }
}

/**
 * @brief TitleWidget::InitLeftWidget
 *  应用标题 左侧 widget 初始化
 */
void TitleWidget::InitLeftWidget()
{
    InitButtonBoxBtn();

    InitNewFolderBtn();

    InitExportBtn();

    InitImportBtn();

    InitDeleteBtn();

    m_pMainLayout->addStretch();
}

void TitleWidget::InitButtonBoxBtn()
{
    auto btnBox = new DButtonBox(this);
    QList<DButtonBoxButton *> listBtnBox;
    DButtonBoxButton *pBoxBtnLeft = new DButtonBoxButton(QStyle::SP_ArrowLeft, nullptr, this);
    pBoxBtnLeft->setAccessibleName("LeftButton");
    pBoxBtnLeft->hide();
    connect(pBoxBtnLeft, &DButtonBoxButton::clicked, this, [=]() {
        emit sigBtnClicked(E_Left_Btn);
    });
    m_nIdAndBtnMap.insert(E_Left_Btn, pBoxBtnLeft);

    DButtonBoxButton *pBoxBtnRight = new DButtonBoxButton(QStyle::SP_ArrowRight, nullptr, this);
    pBoxBtnRight->setAccessibleName("RightButton");
    pBoxBtnRight->hide();
    connect(pBoxBtnRight, &DButtonBoxButton::clicked, this, [=]() {
        emit sigBtnClicked(E_Right_Btn);
    });
    m_nIdAndBtnMap.insert(E_Right_Btn, pBoxBtnRight);

    //    pBoxBtnLeft->setObjectName("backwardbtn");
    //    pBoxBtnRight->setObjectName("forwardbtn");
    listBtnBox.append(pBoxBtnLeft);
    listBtnBox.append(pBoxBtnRight);
    btnBox->setButtonList(listBtnBox, false);
    //    btnBox->show();

    m_pMainLayout->addWidget(btnBox);
}

void TitleWidget::InitNewFolderBtn()
{
    auto btn = new DIconButton(this);
    btn->setAccessibleName("NewFolder");

    connect(btn, &DIconButton::clicked, this, [=]() {
        emit sigBtnClicked(E_New_Folder_Btn);
    });

    btn->setIcon(QIcon::fromTheme("uospm_title_new_folder"));
    btn->setFixedSize(QSize(36, 36));
    btn->setIconSize(QSize(36, 36));
    btn->setToolTip(TrObject::getInstance()->getDlgBtnText(New_Folder)); // add by Airy
    btn->hide();

    m_nIdAndBtnMap.insert(E_New_Folder_Btn, btn);

    m_pMainLayout->addWidget(btn);
}

void TitleWidget::InitExportBtn()
{
    auto btn = new DIconButton(this);
    btn->setAccessibleName("Export");

    connect(btn, &DIconButton::clicked, this, [=]() {
        emit sigBtnClicked(E_Export_Btn);
    });

    btn->setIcon(QIcon::fromTheme("uospm_title_export"));
    btn->setFixedSize(QSize(36, 36));
    btn->setIconSize(QSize(36, 36));
    btn->setToolTip(TrObject::getInstance()->getDlgBtnText(Btn_Export)); // add by Airy
    btn->hide();

    m_nIdAndBtnMap.insert(E_Export_Btn, btn);

    m_pMainLayout->addWidget(btn);
}

void TitleWidget::InitImportBtn()
{
    auto btn = new DIconButton(this);
    btn->setAccessibleName("Import");

    connect(btn, &DIconButton::clicked, this, [=]() {
        emit sigBtnClicked(E_Import_Btn);
    });

    btn->setIcon(QIcon::fromTheme("uospm_title_import"));
    btn->setFixedSize(QSize(36, 36));
    btn->setIconSize(QSize(36, 36));
    btn->setToolTip(TrObject::getInstance()->getDlgBtnText(Btn_Import)); // add by Airy
    btn->hide();

    m_nIdAndBtnMap.insert(E_Import_Btn, btn);

    m_pMainLayout->addWidget(btn);
}

void TitleWidget::InitDeleteBtn()
{
    auto btn = new DIconButton(this);
    btn->setAccessibleName("Delete");

    connect(btn, &DIconButton::clicked, this, [=]() {
        emit sigBtnClicked(E_Delete_Btn);
    });

    btn->setIcon(QIcon::fromTheme("uospm_title_delete"));
    btn->setFixedSize(QSize(36, 36));
    btn->setIconSize(QSize(36, 36));
    btn->setToolTip(TrObject::getInstance()->getDlgBtnText(Dlg_Btn_Delete)); // add by Airy
    btn->hide();

    m_nIdAndBtnMap.insert(E_Delete_Btn, btn);

    m_pMainLayout->addWidget(btn);
}
