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

#include <DApplicationSettings>
#include <DGuiApplicationHelper>
#include <DLog>
#include <QPixmapCache>

#include <signal.h>
#include <execinfo.h>

#include "defines.h"
#include "Application.h"
#include "UIService.h"

#include "accessible.h"

DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

void phone_assistant_signal_handler(int s)
{
    if (s != SIGSEGV) {
        return;
    }

    static bool sigSegvServed = false;
    if (sigSegvServed) {
        abort();
    }
    sigSegvServed = true;

    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + QString("/deepin/deepin-phone-assistant");

    //    qDebug() << "deepin-phone-assistant: Crashed :( Saving backtrace in " << qPrintable(path) << "/crashlog ...";

    void *array[100];
    int size = backtrace(array, 100);
    char **strings = backtrace_symbols(array, size);

    if (size < 0 || !strings) {
        qDebug() << "Cannot get backtrace!";
        abort();
    }

    QDir dir(path);
    if (!dir.exists()) {
        qDebug() << qPrintable(path) << " does not exist";
        abort();
    }

    if (!dir.cd("crashlog")) {
        if (!dir.mkdir("crashlog")) {
            qDebug() << "Cannot create " << qPrintable(path) << "crashlog directory!";
            abort();
        }

        dir.cd("crashlog");
    }

    const QDateTime currentDateTime = QDateTime::currentDateTime();

    QFile file(dir.absoluteFilePath("Crash-" + currentDateTime.toString("yyyy-MM-dd_hh-mm-ss") + ".txt")); //PMS45811文件名不要有冒号
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        qDebug() << "Cannot open file " << qPrintable(file.fileName()) << " for writing!";
        abort();
    }

    QTextStream stream(&file);
    stream << "Time: " << currentDateTime.toString() << endl;
    stream << endl;
    stream << "============== BACKTRACE ==============" << endl;

    for (int i = 0; i < size; ++i) {
        stream << "#" << i << ": " << strings[i] << endl;
    }

    file.close();

    //    qDebug() << "Backtrace successfully saved in " << qPrintable(dir.absoluteFilePath(file.fileName()));
}

void handleForeQuitSignal(int s)
{
    qDebug() << "signal = " << s;
    qApp->exit();
}

int main(int argc, char *argv[])
{
    signal(SIGSEGV, phone_assistant_signal_handler);
    signal(SIGTERM, handleForeQuitSignal);

    qRegisterMetaType<QFileInfo>("QFileInfo");
    qRegisterMetaType<PhoneFileInfo>("PhoneFileInfo");
    qRegisterMetaType<PhoneFileInfo>("PhoneFileInfo&");
    qRegisterMetaType<PhoneInfo>("PhoneInfo");
    qRegisterMetaType<PhoneInfo>("PhoneInfo&");
    qRegisterMetaType<QList<PhoneAppInfo>>("QList<PhoneAppInfo> &");
    qRegisterMetaType<DEV_CONN_INFO>("DEV_CONN_INFO");

    //    Application::loadDXcbPlugin();
    Application a(argc, argv);

    qputenv("DTK_USE_SEMAPHORE_SINGLEINSTANCE", "1");

    if (!DGuiApplicationHelper::instance()->setSingleInstance(
            a.applicationName(),
            DGuiApplicationHelper::UserScope)) {
        return 0;
    }

    DApplicationSettings saveTheme;

    QPixmapCache::setCacheLimit(1);

    QThreadPool::globalInstance()->setMaxThreadCount(20);

    QAccessible::installFactory(accessibleFactory);

    //DApplicationSettings settings;
    DLogManager::registerConsoleAppender();
    DLogManager::registerFileAppender();

    UIService::getService()->showMainWindow();

    return a.exec();
}
