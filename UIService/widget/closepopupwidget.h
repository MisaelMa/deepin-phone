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
#ifndef CLOSEPOPUPWIDGET_H
#define CLOSEPOPUPWIDGET_H

#include <DCheckBox>
#include <DDialog>
#include <DRadioButton>

#include <DWidget>
#include <QDebug>
#include <DSettings>
#include <DFontSizeManager>
#include <qsettingbackend.h>

DCORE_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

class ClosePopUpWidget : public DDialog
{
    Q_OBJECT

public:
    explicit ClosePopUpWidget(DWidget *parent = nullptr);
    ~ClosePopUpWidget();

    int execEx();

    DRadioButton *getMinRadioBtn() { return m_minRadioBtn; }
    DRadioButton *getExtRadioBtn() { return m_extRadioBtn; }
    DCheckBox *getRememberCheckBox() { return m_remember; }

private:
    void InitUi();
    /**
    * @brief  最小化按钮
    */
    DRadioButton *m_minRadioBtn;
    /**
    * @brief  退出按钮
    */
    DRadioButton *m_extRadioBtn;
    /**
    * @brief  不再询问按钮
    */
    DCheckBox *m_remember;
    QString strConfig;
};

#endif // CLOSEPOPUPWIDGET_H
