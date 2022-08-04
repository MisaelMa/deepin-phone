/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co., Ltd.
 *
 * Author:     huangyonga<huangyonga@uniontech.com>
 * Maintainer: huangyonga<huangyonga@uniontech.com>
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
#include "closepopupwidget.h"

#include <DCheckBox>
#include <DFrame>
#include <DLabel>
#include <DMainWindow>
#include <DRadioButton>
#include <DSettingsOption>
#include <DApplication>

#include <QDir>
#include <QStandardPaths>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QVBoxLayout>
#include <qsettingbackend.h>
#include <QStandardPaths>
#include <QThread>

#include "utils.h"
#include "GlobalDefine.h"
#include "TrObject.h"

DCORE_USE_NAMESPACE

ClosePopUpWidget::ClosePopUpWidget(DWidget *parent)
    : DDialog(parent)
{
    InitUi();
}

ClosePopUpWidget::~ClosePopUpWidget()
{
}

int ClosePopUpWidget::execEx()
{
    int nRes = -1;

    QString m_configPath = QString("%1/%2/%3/config.conf").arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)).arg(PhoneAssistantModel::g_organization_name).arg(PhoneAssistantModel::g_application_name);

    auto m_backend = new QSettingBackend(m_configPath, this);

    bool nAskId = m_backend->getOption("base.Closemainwindow.askmealways").toBool(); //
    bool bMin = m_backend->getOption("base.Closemainwindow.radiogroupclose").toBool();
    nRes = bMin ? -1 : 1;

    if (nAskId) {
        m_remember->setChecked(false);

        if (bMin) {
            m_minRadioBtn->setChecked(true);
        } else {
            m_extRadioBtn->setChecked(true);
        }

        if (exec() == DDialog::Accepted) {
            auto m_pDSettings = DSettings::fromJsonFile(":/resources/settings.json");
            m_pDSettings->setBackend(m_backend);

            bool bMinCheck = m_minRadioBtn->isChecked();
            if (false == bMinCheck) {
                emit m_backend->setOption("base.Closemainwindow.radiogroupclose", false);
            } else {
                emit m_backend->setOption("base.Closemainwindow.radiogroupclose", true);
            }

            bool bNoAsk = m_remember->isChecked();
            if (bNoAsk) {
                emit m_backend->setOption("base.Closemainwindow.askmealways", false);
            }

            m_pDSettings->sync();
            qApp->sync();

            nRes = bMinCheck ? -1 : 1;
        } else {
            nRes = 0;
        }
    }
    delete m_backend;

    return nRes;
}

void ClosePopUpWidget::InitUi()
{
    setTitle(TrObject::getInstance()->getDlgTitle(dlg_ClosePopUpWidget));

    m_extRadioBtn = new DRadioButton(TrObject::getInstance()->getDlgBtnText(Dlg_Btn_Exit));
    m_extRadioBtn->setObjectName("CloseConfirmDialogExit");
    m_minRadioBtn = new DRadioButton(TrObject::getInstance()->getDlgBtnText(Minimize_to_system_tray));
    m_minRadioBtn->setObjectName("CloseConfirmDialogMini");
    m_remember = new DCheckBox(TrObject::getInstance()->getCheckBoxText(Do_not_ask_again));
    m_remember->setObjectName("CloseConfirmDialogAsk");
    DFontSizeManager::instance()->bind(m_extRadioBtn, DFontSizeManager::T6);
    DFontSizeManager::instance()->bind(m_minRadioBtn, DFontSizeManager::T6);
    DFontSizeManager::instance()->bind(m_remember, DFontSizeManager::T6);

    setIcon(QIcon::fromTheme(PhoneAssistantModel::g_app_icon_name));
    this->addContent(m_extRadioBtn);
    this->addContent(m_minRadioBtn);
    this->addContent(m_remember);

    addButton(TrObject::getInstance()->getDlgBtnText(Dlg_Btn_cancel), false, ButtonNormal);
    addButton(TrObject::getInstance()->getDlgBtnText(Dlg_Btn_Confirm), true, ButtonRecommend);

    this->setFixedWidth(380);
}
