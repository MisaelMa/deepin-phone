
#include "widget/MainWindow.h"
#include "ut_mainHead.h"

class ut_MainWindow : public ut_mainHead
{
public:
    virtual void SetUp()
    {
        mainWindow = new MainWindow();
    }
    virtual void TearDown()
    {
        delete mainWindow;
    }
    MainWindow *mainWindow;
};

TEST_F(ut_MainWindow, mainWindowInit)
{
}

//TEST_F(ut_MainWindow, Show)
//{
//    ASSERT_FALSE(mainWindow == nullptr) << "已存在" << endl;
//    mainWindow->show();
//}

//TEST_F(ut_MainWindow, Close)
//{
//    ASSERT_FALSE(mainWindow == nullptr) << "已存在" << endl;
//    mainWindow->close();
//}
