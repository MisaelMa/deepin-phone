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
#ifndef UISERVICE_H
#define UISERVICE_H

#include "uiservice_global.h"

#include "widget/MainWindow.h"
/**/
class UISERVICESHARED_EXPORT UIService : public QObject
{
    Q_OBJECT
private:
    UIService();

public:
    ~UIService();
    static void InitService();
    static UIService *getService();

    void showMainWindow();
    //    void sendOkMessage(const QString &);
    //    void sendWarnMessage(const QString &);

    MainWindow *getQspMainWnd() const;

private:
    void onNewProcessInstance(qint64 pid, const QStringList &arguments);

private:
    static UIService *g_instance;

    QScopedPointer<MainWindow> m_qspMainWnd {nullptr};
};

#endif // UISERVICE_H
