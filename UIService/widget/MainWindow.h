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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <DMainWindow>
#include <DWidget>
#include <QStackedLayout>
#include <QScreen>
#include <QSystemTrayIcon>

DWIDGET_USE_NAMESPACE

class NoDeviceWidget;
class MainWidget;
//class ClosePopUpWidget;

class MainWindow : public DMainWindow
{
    Q_OBJECT
    Q_DISABLE_COPY(MainWindow)
public:
    MainWindow(DWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    //    void timerEvent(QTimerEvent *) override;
    void closeEvent(QCloseEvent *event) override;
    //    void resizeEvent(QResizeEvent *event) override;

private slots:
    void slotNoDeviceConnected();

    void slotDeviceListUpdate();

    void onNewProcessInstance(qint64 pid, const QStringList &arguments);

    /**
     * @brief 设置界面退出
     */
    void slotActionTriggred();

    void initMainWindowUI();

private:
    void addSettingsWidget();

    void initUI(const QString &);
    void postInitUI();

    void initLocalApk();

private:
    QStackedLayout *m_centralLayout = nullptr;
    NoDeviceWidget *m_noDevWidget = nullptr;
    MainWidget *m_mainWidget = nullptr;

    QSystemTrayIcon *m_pTrayIcon = nullptr; //  托盘图标
};

#endif // MAINWINDOW_H
