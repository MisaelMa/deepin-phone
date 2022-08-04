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
#ifndef RIGHTTITLEWIDGET_H
#define RIGHTTITLEWIDGET_H

#include <DWidget>

#include <DToolButton>
#include <DLabel>
#include <DButtonBox>

#include <QHBoxLayout>

DWIDGET_USE_NAMESPACE

/**
   标题栏
 * @brief The TitleWidget class
 */

class RightTitleWidget : public DWidget
{
    Q_OBJECT
private:
    explicit RightTitleWidget(DWidget *parent = nullptr);

public:
    static RightTitleWidget *getInstance();

    enum E_TitleWidget_Btn {
        E_Icon_View_Btn = 10,
        E_List_View_Btn
    };

signals:
    void sigBtnClicked(const int &); // 只发出一个信号， 即按钮点击的信号

public:
    void setWidgetBtnVisible(const int &, const bool &bVis);
    void setWidgetBtnEnable(const int &, const bool &bEnable);
    void setWidgetBtnChecked(const int &, const bool &bCheck);

private:
    void InitToolViewBtn();

private:
    static RightTitleWidget *g_instance;

    QHBoxLayout *m_pMainLayout = nullptr;

    QMap<int, QAbstractButton *> m_nIdAndBtnMap;
};

#endif // RIGHTTITLEWIDGET_H
