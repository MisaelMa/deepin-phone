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
#include "MainWindow.h"

#include <DTitlebar>
#include <QStandardPaths>
#include <QCloseEvent>
#include <qsettingbackend.h>
#include <QSystemTrayIcon>
#include <QTimer>
#include <DWidgetUtil>

#include "TitleWidget.h"
#include "RightTitleWidget.h"
#include "GlobalDefine.h"
#include "SettingDialog.h"
#include "TrObject.h"
#include "LocalApkVersionTask.h"
#include "ThreadService.h"
#include "MountService.h"
#include "MainWidget.h"
#include "phoneui/nodevicewidget.h"
#include "closepopupwidget.h"
#include "widget/SpinnerWidget.h"

DCORE_USE_NAMESPACE

MainWindow::MainWindow(DWidget *parent)
    : DMainWindow(parent)
{
    initLocalApk();

    //    qDebug() << __FUNCTION__ << "       begin";
    //    QTime startTime = QTime::currentTime();

    setMinimumSize(1000, 700);
    resize(1070, 700);
    initMainWindowUI();
    //设置菜单
    addSettingsWidget();

    QTimer::singleShot(500, this, [=]() {
        //        initUI();
        postInitUI();
    });

    Dtk::Widget::moveToCenter(this);

    //修改 PMS46955非窗口特效下，最小化窗口无法唤起窗口问题 把窗口激活放到MainWindow处理
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::newProcessInstance, this, &MainWindow::onNewProcessInstance);

    //主界面运行需要时长
    //    QTime stopTime = QTime::currentTime();
    //    int elapsed = startTime.msecsTo(stopTime);
    //    qDebug() << "_________________"
    //             << "QTime.currentTime =" << elapsed << "ms";

    qDebug() << __FUNCTION__ << "       end";
}

MainWindow::~MainWindow()
{
    if (m_pTrayIcon) {
        m_pTrayIcon->hide();
        delete m_pTrayIcon; //删除NEW出的对象
    }

    qDebug() << __FUNCTION__;
}

//void MainWindow::timerEvent(QTimerEvent *pEvent)
//{
//    //    delete closeDlg;
//    killTimer(pEvent->timerId());

//    //  手动退出 应用程序
//    qApp->quit();
//}

void MainWindow::initUI(const QString &sVer)
{
    m_mainWidget = new MainWidget(this);
    m_mainWidget->setLocalApkVersion(sVer);
    m_centralLayout->addWidget(m_mainWidget);
    connect(m_mainWidget, &MainWidget::sigDeviceListUpdate, this, &MainWindow::slotDeviceListUpdate);
    connect(m_mainWidget, &MainWidget::sigNoDeviceConnected, this, &MainWindow::slotNoDeviceConnected);
}

void MainWindow::initMainWindowUI()
{
    titlebar()->setIcon(QIcon::fromTheme(PhoneAssistantModel::g_app_icon_name));
    titlebar()->setTitle(TrObject::getInstance()->getDlgTitle(dlg_app_title)); //  应用标题
    titlebar()->addWidget(TitleWidget::getInstance(), Qt::AlignLeft); //  应用 标题栏 左侧按钮
    titlebar()->addWidget(RightTitleWidget::getInstance(), Qt::AlignRight); //  应用 标题

    m_centralLayout = new QStackedLayout;

    m_noDevWidget = new NoDeviceWidget(this);
    m_centralLayout->addWidget(m_noDevWidget);

    m_centralLayout->setContentsMargins(0, 0, 0, 0);
    m_centralLayout->setSpacing(0);

    DWidget *centralWidget = new DWidget(this);
    centralWidget->setLayout(m_centralLayout);
    this->setCentralWidget(centralWidget);
}

//未检测到设备
void MainWindow::slotNoDeviceConnected()
{
    if (m_noDevWidget == nullptr) {
        m_noDevWidget = new NoDeviceWidget(this);
        m_centralLayout->addWidget(m_noDevWidget);
    }
    m_centralLayout->setCurrentWidget(m_noDevWidget);
    m_noDevWidget->setFocus();

    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Left_Btn, false);
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Right_Btn, false);
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_New_Folder_Btn, false);
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Export_Btn, false);
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Import_Btn, false);
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Delete_Btn, false);
    RightTitleWidget::getInstance()->setWidgetBtnVisible(RightTitleWidget::E_Icon_View_Btn, false);
    RightTitleWidget::getInstance()->setWidgetBtnVisible(RightTitleWidget::E_List_View_Btn, false);
}

//更新设备列表
void MainWindow::slotDeviceListUpdate()
{
    if (m_noDevWidget) {
        delete m_noDevWidget;
        m_noDevWidget = nullptr;
    }
    //    m_noDevWidget->closeHelpDialog();
    m_centralLayout->setCurrentWidget(m_mainWidget);
}

/**
 * @brief 创建设置界面
 */
void MainWindow::addSettingsWidget()
{
    /***添加设置菜单*****/
    auto m_MainwindowMenu = new DMenu(this);
    auto mSetAction = new QAction(TrObject::getInstance()->getMenuActionText(Menu_Settings), this);
    m_MainwindowMenu->addAction(mSetAction);
    titlebar()->setMenu(m_MainwindowMenu);
    QString m_configPath = QString("%1/%2/%3/config.conf").arg(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)).arg(PhoneAssistantModel::g_organization_name).arg(PhoneAssistantModel::g_application_name);
    QFile fileconfig(m_configPath);
    if (!fileconfig.exists()) {
        auto dlg = new SettingDialog(this);
        dlg->hide();
        dlg->deleteLater();
        //        dlg->exec();
    };

    connect(mSetAction, &QAction::triggered, this, &MainWindow::slotActionTriggred);
}

/**
 * @brief 设置界面退出
 */
void MainWindow::slotActionTriggred()
{
    auto dlg = new SettingDialog(this);
    dlg->exec();
}

/**
 * @brief  关闭事件，主要判断是退出还是最小化系统托盘
 * @param[in] event
 */
void MainWindow::closeEvent(QCloseEvent *event)
{
    event->ignore();

    auto closeDlg = new ClosePopUpWidget(this);
    int nRes = closeDlg->execEx();
    delete closeDlg;

    if (nRes == 1) { //  ok
        event->accept();
    } else if (nRes == 0) {
    } else if (nRes == -1) { //  minisize
        this->showMinimized();
    }
}

/**
 * @brief 创建 托盘 图标
 */
void MainWindow::postInitUI()
{
    auto quitAction = new QAction(TrObject::getInstance()->getDlgBtnText(Dlg_Btn_Exit), this);
    //    connect(quitAction, &QAction::triggered, this, [=]() {
    //        exit(0);
    //    });
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

    auto trayIconMenu = new DMenu(this);
    trayIconMenu->addAction(quitAction);

    m_pTrayIcon = new QSystemTrayIcon(this);
    m_pTrayIcon->setIcon(QIcon::fromTheme(PhoneAssistantModel::g_app_icon_name));
    m_pTrayIcon->setToolTip(TrObject::getInstance()->getDlgTitle(dlg_app_title));
    m_pTrayIcon->setContextMenu(trayIconMenu);
    m_pTrayIcon->show();

    //  任务栏点击, 窗口的显示隐藏, 和 邮箱代码一致
    connect(m_pTrayIcon, &QSystemTrayIcon::activated, this, [=](QSystemTrayIcon::ActivationReason reason) {
        if (QSystemTrayIcon::Trigger == reason) {
            if (isVisible()) {
                if (isMinimized()) {
                    if (isMaximized()) {
                        hide();
                        showMaximized();
                    } else if (isFullScreen()) {
                        hide();
                        showFullScreen();
                    } else {
                        hide();
                        this->titlebar()->setFocus();
                        showNormal();
                    }
                } else {
                    showMinimized();
                }
            } else {
                onNewProcessInstance(0, QStringList());
            }
        }
    });
}

//void MainWindow::resizeEvent(QResizeEvent *event)
//{
//    QScreen *screen = QGuiApplication::primaryScreen();
//    QRect rect = screen->availableGeometry();
//    if (event->oldSize() == QSize(rect.width(), rect.height())) {
//        setGeometry(m_rect);
//    } else {
//        if (event->size() != QSize(rect.width(), rect.height()))
//            m_rect = frameGeometry();
//    }
//}

void MainWindow::initLocalApk()
{
    auto task = new LocalApkVersionTask(qApp);
    connect(task, &LocalApkVersionTask::sigFlagRes, this, [=](const QString &sVer) {
        initUI(sVer);
    });
    ThreadService::getService()->startTask(ThreadService::E_Locate_App_Version_Task, task);
}

/**
 * @brief 激活显示窗口
 * @param pid
 * @param arguments
 */
void MainWindow::onNewProcessInstance(qint64 pid, const QStringList &arguments)
{
    Q_UNUSED(pid);
    Q_UNUSED(arguments);

    activateWindow();

    this->show();

    this->titlebar()->setFocus();
}
