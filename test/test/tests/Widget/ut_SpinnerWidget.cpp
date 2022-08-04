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
#include "widget/SpinnerWidget.h"
#include "ut_mainHead.h"

class ut_SpinnerWidget : public ut_mainHead
{
public:
    virtual void SetUp()
    {
        spinnerWidget = new SpinnerWidget;
    }
    virtual void TearDown()
    {
        delete spinnerWidget;
    }
    SpinnerWidget *spinnerWidget = nullptr;
};

/**
* @brief  旋转等待显示框显示
*/
TEST_F(ut_SpinnerWidget, Show)
{
    spinnerWidget->show();
    QTest::qWait(200); //等待10S,此时可手动操作界面按钮
}

/**
* @brief  旋转等待显示框关闭
*/
TEST_F(ut_SpinnerWidget, Close)
{
    spinnerWidget->close();
}

/**
* @brief  旋转等待显示框开始
*/
TEST_F(ut_SpinnerWidget, spinnerStart)
{
    spinnerWidget->spinnerStart();
}

/**
* @brief  旋转等待显示框停止
*/
TEST_F(ut_SpinnerWidget, spinnerStop)
{
    spinnerWidget->spinnerStop();
}

/**
* @brief  旋转等待显示框，设置显示字符串
*/
TEST_F(ut_SpinnerWidget, setShowText)
{
    spinnerWidget->setShowText("hello world");
}

/**
* @brief  旋转等待显示框，是否显示
*/
TEST_F(ut_SpinnerWidget, SpinnerIsVisible)
{
    ASSERT_TRUE(spinnerWidget->SpinnerIsVisible());
}

/**
* @brief  旋转等待显示框，字体改变，设置弹框高度
*/
TEST_F(ut_SpinnerWidget, fontChangeDisplay)
{
    spinnerWidget->fontChangeDisplay();
}
