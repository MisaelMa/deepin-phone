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
#ifndef SPINNERWIDGET_H
#define SPINNERWIDGET_H

#include <DSpinner>
#include <DWidget>
#include <DBlurEffectWidget>
#include <QStackedWidget>
#include <DLabel>

DWIDGET_USE_NAMESPACE

/**
 * @brief load界面, 显示数据信息
 */

class SpinnerWidget : public DBlurEffectWidget
{
    Q_OBJECT
public:
    explicit SpinnerWidget(DWidget *parent = nullptr);

public:
    void spinnerStart();
    void spinnerStop();
    void setShowText(const QString &strText);
    bool SpinnerIsVisible() { return m_bIsHidden; }
    //    void fontChangeDisplay();

private:
    void initUI();

private slots:
    void slotSetBackground();

private:
    QStackedWidget *m_pStackWidget = nullptr;

    DSpinner *m_spinner = nullptr;
    //    DLabel *m_lblName = nullptr;
    DLabel *m_sShowTip = nullptr;
    bool m_bIsHidden = true;
};

#endif // SPINNERWIDGET_H
