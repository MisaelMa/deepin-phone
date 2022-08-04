#include <gtest/gtest.h>
#include <QDebug>

#include <QApplication>

#include "ut_environment.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    auto environment = new ut_environment;
    ::testing::AddGlobalTestEnvironment(environment);

    qDebug() << "start deepin-phone-master test cases ..............";
    ::testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    qDebug() << "end deepin-phone-master test cases ..............";
    return ret;
}
