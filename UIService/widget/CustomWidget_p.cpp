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
#include "CustomWidget_p.h"

#include <QDebug>

#include "CustomWidget.h"
#include "filemanagethread.h"
#include "progresswidget.h"
#include "TrObject.h"

CustomWidgetPrivate::CustomWidgetPrivate(QObject *parent)
    : QObject(parent)
{
    q_ptr = static_cast<CustomWidget *>(parent);
}

//void CustomWidgetPrivate::showImportProgress()
//{
//    Q_Q(CustomWidget);

//    m_pImportProgress = new ImportProgressWgt(q);
//    connect(m_pImportProgress, &ImportProgressWgt::sigCloseBtnClicked, this, &CustomWidgetPrivate::slotCancelImport);
//    moveImportDlg();
//    m_pImportProgress->show();
//}

//void CustomWidgetPrivate::sendExportMessage(const E_Widget_Type &type)
//{
//    QString info = "";
//    if (!m_exportCancel) {
//        //正常结束导出
//        if (m_exportOkCount > 0 && m_exportErrCount == 0) {
//            info = TrObject::getInstance()->getImportExportText(Export_success);
//        } else if (m_exportOkCount > 0) {
//            if (type == E_Widget_Book) {
//                info = TrObject::getInstance()->getEBookImportExportText(eBooks_export_count).arg(m_exportOkCount);
//            } else if (type == E_Widget_File) {
//                info = TrObject::getInstance()->getImportExportText(Export_success_count).arg(m_exportOkCount);
//            } else if (type == E_Widget_Music) {
//                info = TrObject::getInstance()->getMusicImportExportText(song_export_count).arg(m_exportOkCount);
//            }
//        }
//        qDebug() << __LINE__ << __FUNCTION__ << "export end.";
//    } else {
//        //用户取消了导出
//        if (m_exportOkCount > 0) {
//            m_exportOkCount += 1; //取消导出时，由于线程信号的问题，可能成功的数比实际导出的数少1个
//            if (type == E_Widget_Book) {
//                info = TrObject::getInstance()->getEBookImportExportText(eBooks_export_stop_count).arg(m_exportOkCount);
//            } else if (type == E_Widget_File) {
//                if (m_exportOkCount > 1)
//                    info = TrObject::getInstance()->getImportExportText(file_Export_stopped).arg(m_exportOkCount);
//                else if (m_exportOkCount == 1)
//                    info = TrObject::getInstance()->getMessageText(Export_stopped_one_file);
//            } else if (type == E_Widget_Music) {
//                info = TrObject::getInstance()->getMusicImportExportText(song_export_stop_count).arg(m_exportOkCount);
//            }
//        }
//        qDebug() << __LINE__ << __FUNCTION__ << "export cancel.";
//    }

//    if (info != "") {
//        Q_Q(CustomWidget);
//        q->sendOkMessage(info);
//    }
//}

//void CustomWidgetPrivate::closeExportProgress(const E_Widget_Type &type)
//{
//    QThread::msleep(300);

//    qDebug() << __LINE__ << __FUNCTION__;
//    if (m_pExportProgressDlg != nullptr) {
//        qDebug() << __LINE__ << __FUNCTION__;
//        if (m_exportFileThrd != nullptr) {
//            if (m_exportFileThrd->isRunning()) {
//                qDebug() << __LINE__ << __FUNCTION__;
//                m_exportFileThrd->setExit();
//                m_exportFileThrd->quit();
//                m_exportFileThrd->wait();
//                qDebug() << __LINE__ << __FUNCTION__;
//            }

//            //线程结束时析构
//            delete m_exportFileThrd;
//            m_exportFileThrd = nullptr;
//            qDebug() << __LINE__ << __FUNCTION__;
//        }

//        m_pExportProgressDlg->close();

//        delete m_pExportProgressDlg;
//        m_pExportProgressDlg = nullptr;

//        sendExportMessage(type);
//    }
//}

//void CustomWidgetPrivate::slotCancelImport()
//{
//    qDebug() << __LINE__ << __FUNCTION__;
//    m_importCancel = true;
//    closeImportProgress();
//}

//void CustomWidgetPrivate::closeImportProgress()
//{
//    if (m_importFileThrd != nullptr) {
//        m_importFileThrd->setExit();
//        m_importFileThrd->quit();
//        m_importFileThrd->wait();
//        //线程结束时析构
//        delete m_importFileThrd;
//        m_importFileThrd = nullptr;
//        qDebug() << __LINE__ << __FUNCTION__;
//    }

//    if (m_pImportProgress != nullptr) {
//        m_pImportProgress->close();
//        delete m_pImportProgress;
//        m_pImportProgress = nullptr;
//    }
//    qDebug() << __LINE__ << __FUNCTION__;
//}

void CustomWidgetPrivate::moveImportDlg()
{
    if (m_pImportProgress) {
        Q_Q(CustomWidget);
        m_pImportProgress->move((q->width() - m_pImportProgress->width()) / 2, q->height() - 60);
    }
}

//void CustomWidgetPrivate::slotUpdateImportFileName(int optType, QString fileName)
//{
//    if (optType == FILE_IMPORT) {
//        if (m_pImportProgress != nullptr) {
//            m_pImportProgress->updateProgressText(TrObject::getInstance()->getImportExportText(Importing) + "  " + fileName);
//        }
//    }
//}

//void CustomWidgetPrivate::slotUpdateExportProgressValue(int optType, int curValue)
//{
//    if (optType == FILE_EXPORT) {
//        if (m_pExportProgressDlg != nullptr)
//            m_pExportProgressDlg->updateProgressBarValue(curValue);
//    }
//}

//void CustomWidgetPrivate::setProgressDlgInfo(E_File_OptionType optType)
//{
//    m_optType = optType;
//}

void CustomWidgetPrivate::closeImportExportProgress(const E_Widget_Type &type)
{
    qDebug() << __LINE__ << __FUNCTION__;
    if (m_pImportExportProgressDlg != nullptr) {
        qDebug() << __LINE__ << __FUNCTION__;
        if (m_importExportFileThrd != nullptr) {
            if (m_importExportFileThrd->isRunning()) {
                qDebug() << __LINE__ << __FUNCTION__;
                m_importExportFileThrd->setExit();
                m_importExportFileThrd->quit();
                m_importExportFileThrd->wait();
                qDebug() << __LINE__ << __FUNCTION__;
            }

            //线程结束时析构
            delete m_importExportFileThrd;
            m_importExportFileThrd = nullptr;
            qDebug() << __LINE__ << __FUNCTION__;
        }

        m_pImportExportProgressDlg->close();

        delete m_pImportExportProgressDlg;
        m_pImportExportProgressDlg = nullptr;

        sendImportExportMessage(type);
    }
}

void CustomWidgetPrivate::slotUpdateImportExportProgressValue(int optType, int curValue)
{
    Q_UNUSED(optType);
    if (m_pImportExportProgressDlg != nullptr)
        m_pImportExportProgressDlg->updateProgressBarValue(curValue);
}

void CustomWidgetPrivate::sendImportExportMessage(const E_Widget_Type &type)
{
    QString info = "";
    if (!m_importExportCancel) {
        //正常结束
        if (m_optType == FILE_IMPORT) {
            if (m_OkCount > 0) {
                if (type == E_Widget_Book) {
                    info = TrObject::getInstance()->getEBookImportExportText(eBooks_import_count).arg(m_OkCount).arg(EBOOK_DIR);
                } else if (type == E_Widget_File) {
                    info = TrObject::getInstance()->getImportExportText(file_Import_success).arg(m_OkCount);
                } else if (type == E_Widget_Music) {
                    info = TrObject::getInstance()->getMusicImportExportText(song_import_count).arg(m_OkCount).arg(MUSIC_DIR);
                }
            }
        } else if (m_optType == FILE_EXPORT) {
            if (m_OkCount > 0 && m_ErrCount == 0) {
                info = TrObject::getInstance()->getImportExportText(Export_success);
            } else if (m_OkCount > 0) {
                if (type == E_Widget_Book) {
                    info = TrObject::getInstance()->getEBookImportExportText(eBooks_export_count).arg(m_OkCount);
                } else if (type == E_Widget_File) {
                    info = TrObject::getInstance()->getImportExportText(Export_success_count).arg(m_OkCount);
                } else if (type == E_Widget_Music) {
                    info = TrObject::getInstance()->getMusicImportExportText(song_export_count).arg(m_OkCount);
                }
            }
        }

        qDebug() << __LINE__ << __FUNCTION__ << " end.";
    } else {
        //用户取消了
        if (m_optType == FILE_IMPORT) {
            if (m_OkCount > 0) {
                //m_OkCount += 1; //取消导出时，由于线程信号的问题，可能成功的数比实际导出的数少1个
                if (type == E_Widget_Book) {
                    info = TrObject::getInstance()->getEBookImportExportText(eBooks_import_stop).arg(m_OkCount).arg(EBOOK_DIR);
                } else if (type == E_Widget_File) {
                    info = TrObject::getInstance()->getImportExportText(file_Import_stopped).arg(m_OkCount);
                } else if (type == E_Widget_Music) {
                    info = TrObject::getInstance()->getMusicImportExportText(song_import_stop).arg(m_OkCount).arg(MUSIC_DIR);
                }
            }
        } else if (m_optType == FILE_EXPORT) {
            if (m_OkCount > 0) {
                m_OkCount += 1; //取消导出时，由于线程信号的问题，可能成功的数比实际导出的数少1个
                if (type == E_Widget_Book) {
                    info = TrObject::getInstance()->getEBookImportExportText(eBooks_export_stop_count).arg(m_OkCount);
                } else if (type == E_Widget_File) {
                    if (m_OkCount > 1)
                        info = TrObject::getInstance()->getImportExportText(file_Export_stopped).arg(m_OkCount);
                    else if (m_OkCount == 1)
                        info = TrObject::getInstance()->getMessageText(Export_stopped_one_file);
                } else if (type == E_Widget_Music) {
                    info = TrObject::getInstance()->getMusicImportExportText(song_export_stop_count).arg(m_OkCount);
                }
            }
        }

        qDebug() << __LINE__ << __FUNCTION__ << " cancel.";
    }

    if (info != "") {
        Q_Q(CustomWidget);
        q->sendOkMessage(info);
    }
}
