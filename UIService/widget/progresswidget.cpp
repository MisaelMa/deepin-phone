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
#include "progresswidget.h"
#include "utils.h"

#include <DLabel>
#include <DProgressBar>
#include <DSpinner>
#include <DIconButton>
#include <QCloseEvent>

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>
#include <QThread>

#include "GlobalDefine.h"
#include "TrObject.h"
#include "UIService.h"

//ExportProgressDlg::ExportProgressDlg(DWidget *parent)
//    : DDialog(parent)
//{
//    setIcon(QIcon::fromTheme(PhoneAssistantModel::g_app_icon_name));
//    setTitle(TrObject::getInstance()->getDlgTitle(dlg_Exporting));

//    m_pExportProgressBar = new DProgressBar(this);
//    m_pExportProgressBar->setTextVisible(false);
//    m_pExportProgressBar->setMaximumHeight(8);
//    m_pExportProgressBar->setRange(0, 100);

//    addSpacing(6);
//    addContent(m_pExportProgressBar);
//    addButton(TrObject::getInstance()->getDlgBtnText(Dlg_Btn_cancel), false, DDialog::ButtonNormal);
//}

//void ExportProgressDlg::setProgressBarRange(int minValue, int maxValue)
//{
//    if (m_pExportProgressBar != nullptr) {
//        if (maxValue > minValue)
//            m_pExportProgressBar->setRange(minValue, maxValue);
//    }
//}

//void ExportProgressDlg::updateProgressBarValue(int curValue)
//{
//    if (m_pExportProgressBar != nullptr) {
//        m_pExportProgressBar->setValue(curValue);
//        //        update();
//    }
//}

//void ExportProgressDlg::hideEvent(QHideEvent *event)
//{
//    Q_UNUSED(event)
//    emit sigCloseBtnClicked();
//}

//void ExportProgressDlg::resizeEvent(QResizeEvent *event)
//{
//    UIService::getService()->getQspMainWnd()->width();
//    UIService::getService()->getQspMainWnd()->height();

//    this->move((UIService::getService()->getQspMainWnd()->width() - this->width()) / 2 + UIService::getService()->getQspMainWnd()->x(),
//               ((UIService::getService()->getQspMainWnd()->height() - this->height())) / 2 + UIService::getService()->getQspMainWnd()->y());

//    DDialog::resizeEvent(event);
//}

ImportProgressWgt::ImportProgressWgt(DWidget *parent)
    : DFloatingWidget(parent)
{
    setFixedSize(QSize(600, 60));

    QHBoxLayout *pHBoxLayoutDFloat = new QHBoxLayout();
    pHBoxLayoutDFloat->setContentsMargins(11, 6, 6, 6);
    pHBoxLayoutDFloat->setSpacing(10);

    DSpinner *spinner = new DSpinner(this);
    spinner->setFixedSize(QSize(20, 20));
    spinner->start();
    pHBoxLayoutDFloat->addWidget(spinner);

    //    DLabel *pDLabel = new DLabel(TrObject::getInstance()->getDlgTitle(dlg_Importing));
    //    pHBoxLayoutDFloat->addWidget(pDLabel);

    m_txtInfo = new DLabel();
    m_txtInfo->setFixedWidth(520);
    pHBoxLayoutDFloat->addWidget(m_txtInfo);
    pHBoxLayoutDFloat->addStretch();

    DIconButton *pIconBtn = new DIconButton(DStyle::SP_CloseButton);
    pIconBtn->setAccessibleName("closeButton");
    pIconBtn->setIconSize(QSize(25, 25));
    pIconBtn->setFlat(true);
    connect(pIconBtn, &DIconButton::clicked, this, &ImportProgressWgt::sigCloseBtnClicked);
    pHBoxLayoutDFloat->addWidget(pIconBtn);
    m_pIconBtn = pIconBtn;

    setLayout(pHBoxLayoutDFloat);
    move(parent->geometry().x() + 100, parent->geometry().bottom() - 64);
}

void ImportProgressWgt::updateProgressText(QString text)
{
    text = Utils::ElideText(m_txtInfo->font(), m_txtInfo->width(), text);
    m_txtInfo->setText(text);
}

void ImportProgressWgt::setCloseBtnHidden(bool bHidden)
{
    m_pIconBtn->setHidden(bHidden);
}

FileRepeatConfirmDlg::FileRepeatConfirmDlg(QString fileName, QString srcPath, QString dstPath, bool hideCoexistBtn,
                                           DWidget *parent, bool hideCheckBox)
    : DDialog(TrObject::getInstance()->getDlgTitle(dlg_FileRepeatConfirmDlg), fileName, parent)
{
    Q_UNUSED(srcPath)
    Q_UNUSED(dstPath)

    //setIcon(QIcon::fromTheme("uospm_warning"));
    setIcon(QIcon::fromTheme("dialog-warning"));
    setWindowFlags(Qt::WindowTitleHint | Qt::Tool); //不显示X
    setMinimumSize(380, 140);
    setWordWrapTitle(true);

    m_labSrcPath = new DLabel(this);
    m_labSrcPath->setFixedWidth(this->width() - 30);
    m_labSrcPath->setAlignment(Qt::AlignHCenter);
    QString text = fileName;
    text = Utils::ElideText(m_labSrcPath->font(), m_labSrcPath->width(), text);
    setMessage(text);
    m_checkBox = new DCheckBox(TrObject::getInstance()->getCheckBoxText(Apply_to_all), this);

    if (!hideCheckBox)
        addContent(m_checkBox, Qt::AlignHCenter);

    addButton(TrObject::getInstance()->getDlgBtnText(Dlg_Btn_Skip), false, DDialog::ButtonNormal);
    addButton(TrObject::getInstance()->getDlgBtnText(Dlg_Btn_Replace), false, DDialog::ButtonNormal);
    if (!hideCoexistBtn)
        addButton(TrObject::getInstance()->getDlgBtnText(Dlg_Btn_Keep_both), false, DDialog::ButtonRecommend);

    setModal(true);
}

void FileRepeatConfirmDlg::hideCheckBox()
{
    m_checkBox->hide();
}

void FileRepeatConfirmDlg::closeEvent(QCloseEvent *event)
{
    // 屏蔽关闭信号
    event->ignore();
}

void FileRepeatConfirmDlg::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        //进行界面退出，重写Esc键，否则重写reject()方法 屏蔽关闭信号
    case Qt::Key_Escape:
        //回调关闭信号
        this->close();
        break;

    default:
        QDialog::keyPressEvent(event);
    }
}

ImportExportProgressDlg::ImportExportProgressDlg(QString title, DWidget *parent)
    : DDialog(parent)
{
    setIcon(QIcon::fromTheme(PhoneAssistantModel::g_app_icon_name));
    setTitle(title);

    m_pProgressBar = new DProgressBar(this);
    m_pProgressBar->setTextVisible(false);
    m_pProgressBar->setMaximumHeight(8);
    m_pProgressBar->setRange(0, 100);

    addSpacing(6);
    addContent(m_pProgressBar);
    addButton(TrObject::getInstance()->getDlgBtnText(Dlg_Btn_cancel), false, DDialog::ButtonNormal);
}

void ImportExportProgressDlg::setProgressBarRange(int minValue, int maxValue)
{
    if (m_pProgressBar != nullptr) {
        if (maxValue > minValue)
            m_pProgressBar->setRange(minValue, maxValue);
    }
}

void ImportExportProgressDlg::updateProgressBarValue(int curValue)
{
    if (m_pProgressBar != nullptr) {
        m_pProgressBar->setValue(curValue);
        //        update();
    }
}

void ImportExportProgressDlg::setProgressBarFinish()
{
    if (m_pProgressBar != nullptr) {
        m_pProgressBar->setValue(m_pProgressBar->maximum());
    }
    QThread::msleep(300);
}

void ImportExportProgressDlg::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event)
    emit sigCloseBtnClicked();
}

void ImportExportProgressDlg::resizeEvent(QResizeEvent *event)
{
    UIService::getService()->getQspMainWnd()->width();
    UIService::getService()->getQspMainWnd()->height();

    this->move((UIService::getService()->getQspMainWnd()->width() - this->width()) / 2 + UIService::getService()->getQspMainWnd()->x(),
               ((UIService::getService()->getQspMainWnd()->height() - this->height())) / 2 + UIService::getService()->getQspMainWnd()->y());

    DDialog::resizeEvent(event);
}
