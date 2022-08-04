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
#ifndef NODEVICEWIDGET_H
#define NODEVICEWIDGET_H

#include <DWidget>
#include <DLabel>
#include <DFloatingButton>
#include <DDialog>
#include <DApplicationHelper>

DWIDGET_USE_NAMESPACE

class NoDeviceWidget : public DWidget
{
    Q_OBJECT
public:
    explicit NoDeviceWidget(DWidget *parent = nullptr);
    //    void closeHelpDialog();

private slots:
    void slotIOSButtonClicked();
    void slotAndroidButtonClicked();
    void onChangedTheme(DGuiApplicationHelper::ColorType themeType);

private:
    void initUI();
    void initConnection();

private:
    DLabel *m_iconImage = nullptr;
    DFloatingButton *m_iosButton = nullptr;
    DFloatingButton *m_androidButton = nullptr;
    //    DDialog *m_iosHelpDlg = nullptr;
    //    DDialog *m_androidHelpDlg = nullptr;
};

#endif // NODEVICEWIDGET_H
