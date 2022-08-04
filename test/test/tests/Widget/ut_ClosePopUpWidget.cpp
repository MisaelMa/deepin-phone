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
#include "widget/closepopupwidget.h"
#include "ut_mainHead.h"

class ut_ClosePopUpWidget : public ut_mainHead
{
public:
    virtual void SetUp()
    {
        closepopWidget = new ClosePopUpWidget;
    }
    virtual void TearDown()
    {
        delete closepopWidget;
    }
    ClosePopUpWidget *closepopWidget = nullptr;
};

TEST_F(ut_ClosePopUpWidget, ClosePopUpWidgetInit)
{
}

/**
* @brief GUI testing using QtTest
*        测试关闭界面的按钮点击事件，最小化，退出，不在询问
* @param[in] TestClosePopUpWidget 测试类名  testIncButton 测试用例名
*/
TEST_F(ut_ClosePopUpWidget, testIncButton)
{
    /**
    * @brief  测试按钮前需要先将界面显示出来
    */
    ASSERT_FALSE(closepopWidget == nullptr) << "取消" << endl;
    closepopWidget->show();

    for (int i = 0; i < 2; i++) {
        QTest::mouseClick(closepopWidget->getMinRadioBtn(), Qt::LeftButton, Qt::NoModifier, QPoint(10, 10), 250);
        QTest::mouseClick(closepopWidget->getExtRadioBtn(), Qt::LeftButton, Qt::NoModifier, QPoint(10, 10), 250);
        QTest::mouseClick(closepopWidget->getRememberCheckBox(), Qt::LeftButton, Qt::NoModifier, QPoint(10, 10), 250);
    }
}

/**
* @brief 测试关闭界面的按钮点击事件，取消关闭界面，确认关闭按钮
*
* @param[in] TestClosePopUpWidget 测试类名  testCloseButton 测试用例名
*/
TEST_F(ut_ClosePopUpWidget, testCloseButton)
{
    ASSERT_FALSE(closepopWidget == nullptr) << "已存在" << endl;
    /**
    * @brief GUI testing using QtTest
    *        测试关闭界面的按钮点击事件，最小化，退出，不在询问
    * @param[in] ut_ClosePopUpWidget 测试类名  testIncButton 测试用例名
    */
    closepopWidget->show();
}

/**
* @brief  测试关闭界面显示10S
*
* @param[in] ut_ClosePopUpWidget 测试类名  Show 测试用例名
*/
TEST_F(ut_ClosePopUpWidget, Show)
{
    ASSERT_FALSE(closepopWidget == nullptr) << "已存在" << endl;
    closepopWidget->show();
}

/**
* @brief  测试关闭界面关闭
*
* @param[in] ut_ClosePopUpWidget 测试类名  Close 测试用例名
*/
TEST_F(ut_ClosePopUpWidget, Close)
{
    ASSERT_FALSE(closepopWidget == nullptr) << "已存在" << endl;
    closepopWidget->close();
}

/**
* @brief  测试调出关闭界面函数
*
* @param[in] ut_ClosePopUpWidget 测试类名  execEx 测试用例名
*/
TEST_F(ut_ClosePopUpWidget, execEx)
{
    ASSERT_NE(closepopWidget->execEx(), 1);
    closepopWidget->close();
}
