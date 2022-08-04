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
#include "CustomWidget.h"

#include <QHBoxLayout>
#include <DFontSizeManager>
#include <DMessageManager>
#include <DMenu>
#include <DDialog>
#include <DFileDialog>
#include <DStandardPaths>
#include <QDebug>
#include <DApplication>

#include "widget/SpinnerWidget.h"
#include "widget/TitleWidget.h"
#include "widget/RightTitleWidget.h"
#include "widget/progresswidget.h"

#include "base/BaseTreeView.h"

#include "TrObject.h"
#include "UIService.h"
#include "CustomWidget_p.h"

DCORE_USE_NAMESPACE

CustomWidget::CustomWidget(DWidget *p)
    : DWidget(p)
    , d_ptr(new CustomWidgetPrivate(this))
{
    //    initTreeViews();
    initSpinnerWidget();

    initConnection();
}

CustomWidget::~CustomWidget()
{
    Q_D(CustomWidget);
    delete d;
}

void CustomWidget::spinnerStart()
{
    m_pSpinnerWidget->spinnerStart();
    m_pSpinnerWidget->setEnabled(true); //解决可能的置灰问题
}

void CustomWidget::spinnerStop()
{
    m_pSpinnerWidget->spinnerStop();
    m_pSpinnerWidget->setEnabled(true); //解决可能的置灰问题
}

void CustomWidget::initMainLayout()
{
    m_pMmainLayout = new QVBoxLayout(this);
    m_pMmainLayout->setMargin(0);
    m_pMmainLayout->setSpacing(0);

    auto tempWidget = m_pTreeViewFrame->getParent(); // getTreeWidget(this);
    m_pMmainLayout->addWidget(tempWidget);
    m_pMmainLayout->addWidget(m_pSpinnerWidget, 0, Qt::AlignBottom);
}

void CustomWidget::setShowText(const QString &strText)
{
    if (SpinnerIsVisible() == true) {
        m_pSpinnerWidget->setShowText(strText);
    }
}

bool CustomWidget::SpinnerIsVisible()
{
    return m_pSpinnerWidget->SpinnerIsVisible();
}

//  pms 36879, modify by wzx, 2020-7-7
bool CustomWidget::execMsgBox(const QString &strText, bool bShowCancel)
{
    bool rl = false;
    DDialog *dlg = new DDialog(strText, "", this);
    dlg->setIcon(QIcon::fromTheme("dialog-warning"));
    if (bShowCancel) //add by zhangliangliang
        dlg->addButton(TrObject::getInstance()->getDlgBtnText(Dlg_Btn_cancel), false, DDialog::ButtonNormal);

    dlg->addButton(TrObject::getInstance()->getDlgBtnText(Dlg_Btn_Delete), true, DDialog::ButtonWarning);
    int nRet = dlg->exec();
    if (nRet == DDialog::Accepted) //选择了取消
        rl = true;

    delete dlg;

    return rl;
}

//  导出 的 路径选择对话框
QString CustomWidget::getExportPath()
{
    QString sPath = "";

    auto w = new DFileDialog(this);
    w->setLabelText(QFileDialog::Accept, TrObject::getInstance()->getDlgBtnText(Dlg_Btn_Save));
    w->setLabelText(QFileDialog::Reject, TrObject::getInstance()->getDlgBtnText(Dlg_Btn_cancel));
    w->setAcceptMode(QFileDialog::AcceptOpen);
    w->setFileMode(QFileDialog::DirectoryOnly);
    w->setOption(QFileDialog::ReadOnly, true);
    w->setDirectory(DStandardPaths::writableLocation(QStandardPaths::DesktopLocation));

    if (w->exec() == DFileDialog::Accepted) {
        sPath = w->directory().path();
    }

    delete w;

    return sPath;
}

void CustomWidget::sendWarnMessage(const QString &sMsg)
{
    DMessageManager::instance()->sendMessage(UIService::getService()->getQspMainWnd(), QIcon::fromTheme("uospm_message_warning"), sMsg);
}

//  统一的右键菜单
void CustomWidget::slotCustomContextMenuRequested(const QPoint &)
{
    Q_D(CustomWidget);

    //  正在加载, 或者 正在导入
    if (SpinnerIsVisible() == false || (d->m_pImportProgress != nullptr && d->m_pImportProgress->isVisible()))
        return;

    auto menu = new DMenu(this);
    QAction *refresh = new QAction(TrObject::getInstance()->getMenuActionText(Menu_Refresh), this);

    connect(refresh, &QAction::triggered, this, [=]() {
        refreshWidgetData();
    });

    menu->addAction(refresh);
    menu->exec(QCursor::pos());

    delete menu;
}

void CustomWidget::sendOkMessage(const QString &sMsg)
{
    DMessageManager::instance()->sendMessage(UIService::getService()->getQspMainWnd(), QIcon::fromTheme("uospm_message_ok"), sMsg); //pms46417切换到列表时消息框被档
}

//  loading
void CustomWidget::initSpinnerWidget()
{
    m_pSpinnerWidget = new SpinnerWidget(this);
}

/**
 * @brief 初始化 通用信号
 */
void CustomWidget::initConnection()
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &DWidget::customContextMenuRequested, this, &CustomWidget::slotCustomContextMenuRequested);

    connect(TitleWidget::getInstance(), &TitleWidget::sigBtnClicked, this, &CustomWidget::slotTitleWidgetBtnClicked);
    connect(RightTitleWidget::getInstance(), &RightTitleWidget::sigBtnClicked, this, &CustomWidget::slotTitleWidgetBtnClicked);
}

void CustomWidget::resizeEvent(QResizeEvent *event)
{
    Q_D(CustomWidget);
    d->moveImportDlg();

    DWidget::resizeEvent(event);
}

bool CustomWidget::checkOperating()
{
    bool isOpting = false;
    if (SpinnerIsVisible() == false)
        isOpting = true;
    if (d_ptr->m_pImportProgress != nullptr && d_ptr->m_pImportProgress->isVisible())
        isOpting = true;
    //    if (d_ptr->m_pExportProgressDlg != nullptr && d_ptr->m_pExportProgressDlg->isVisible())
    //        isOpting = true;

    if (isOpting)
        sendWarnMessage(TrObject::getInstance()->getFileOperationText(Processing_data));

    return isOpting;
}

void CustomWidget::RemovePathPrefix(QString &path, int devType, QString devId)
{
    QString tmpPath;
    if (devType == Mount_Ios) {
        tmpPath = path.mid(path.indexOf(",port=") + 1);
        path = tmpPath.mid(tmpPath.indexOf("/"));
    } else {
        QStringList pathLst = path.split(devId);
        if (pathLst.size() > 1)
            path = pathLst.at(1);
    }
}

void CustomWidget::setUIModel()
{
    BaseItemModel *treeModel = nullptr;

    if (m_pTreeViewFrame) {
        treeModel = m_pTreeViewFrame->getSourceModel();
    }

    BaseItemModel *listModel = nullptr;

    if (m_pListViewFrame) {
        listModel = m_pListViewFrame->getSourceModel();
    }

    m_pUIModelManager = new UIModelManager(this);
    m_pUIModelManager->setUIModel(treeModel, listModel);
}
