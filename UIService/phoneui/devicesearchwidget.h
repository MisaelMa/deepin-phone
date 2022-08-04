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
#ifndef DEVICESEARCHWIDGET_H
#define DEVICESEARCHWIDGET_H

//#include "spinnerwidget.h"

#include <DWidget>
#include <DLabel>
#include <DSpinner>
#include <DGuiApplicationHelper>

DWIDGET_USE_NAMESPACE

class DeviceSearchWidget : public DWidget
{
public:
    explicit DeviceSearchWidget(DWidget *parent = nullptr);
    void SpinnerWidgetStart();
    void SpinnerWidgetStop();
    void onChangedTheme(DGuiApplicationHelper::ColorType themeType);
    void setState(bool isInstalling);

private:
    void initUI();

private:
    DLabel *m_iconImage;
    DLabel *m_tipLabel;
    DSpinner *m_spinner;
};

#endif // DEVICESEARCHWIDGET_H
