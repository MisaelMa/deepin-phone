///*
// * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co., Ltd.
// *
// * Author:     yelei <yelei@uniontech.com>
// * Maintainer: yelei <yelei@uniontech.com>
// * This program is free software: you can redistribute it and/or modify
// * it under the terms of the GNU General Public License as published by
// * the Free Software Foundation, either version 3 of the License, or
// * any later version.
// * This program is distributed in the hope that it will be useful,
// * but WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// * GNU General Public License for more details.
// * You should have received a copy of the GNU General Public License
// * along with this program.  If not, see <http://www.gnu.org/licenses/>.
// */

//#include "PhotoVideoGridWidget.h"
//#include "utils.h"
//#include "widget/SpinnerWidget.h"
//#include "widget/progresswidget.h"
//#include <QApplication>
//#include <QPainter>
//#include <DPalette>
//#include <DApplicationHelper>
//#include <QStyleOptionButton>
//#include <QBitmap>
//#include <DScrollArea>
//#include <QTimer>
//#include <DMessageManager>
//#include <DFileDialog>
//#include <DFontSizeManager>
//#include <QMouseEvent>
//#include <QDebug>
//#include <DStandardPaths>
//#include <QHeaderView>

//#include <DFrame>

//#include "widget/TitleWidget.h"
//#include "widget/RightTitleWidget.h"
//#include "widget/ImageItemWidget.h"
//#include "widget/CustomWidget_p.h"
//#include "TrObject.h"

//#include "ImageVideo/PhotoTreeFrame.h"
//#include "ItemModel/PhoneTreePhotoModel.h"
//#include "UIService.h"
//#include "interface/BaseTreeView.h"

//#define ITEM_SIZE_PIC 116
//#define ITEM_SIZE_VIDEO 164
//#define ITEM_SIZE_ALBUM 184 // 184 150 相册

//DCORE_USE_NAMESPACE

//PhotoVideoGridWidget::PhotoVideoGridWidget(const RunMode &mode, DWidget *parent)
//    : CustomWidget(parent)
//{
//    setObjectName("PhotoVideoGridWidget");
//    m_runMode = mode;
//    m_bIsAlbum = true;
//    m_tRead = nullptr;

//    if (m_runMode == Photo) {
//        m_pWidgetType = E_Widget_Photo;
//    } else if (m_runMode == Video) {
//        m_pWidgetType = E_Widget_Video;
//    }

//    initUI();
//    initConn();
//}

//PhotoVideoGridWidget::~PhotoVideoGridWidget()
//{
//    if (m_tRead) {
//        m_tRead->quitImmediately();
//        m_tRead->quit();
//        //        m_tRead->wait();
//    }
//    if (d_ptr->m_tPhotoVideoImport) {
//        d_ptr->m_tPhotoVideoImport->quitImmediately();
//        d_ptr->m_tPhotoVideoImport->wait();
//        d_ptr->m_tPhotoVideoImport->deleteLater();
//    }
//    if (d_ptr->m_tPhotoVideoExport) {
//        d_ptr->m_tPhotoVideoImport->quitImmediately();
//        d_ptr->m_tPhotoVideoExport->wait();
//        d_ptr->m_tPhotoVideoExport->deleteLater();
//    }
//    qDebug() << __FUNCTION__ << "       " << m_runMode << this;
//}

//void PhotoVideoGridWidget::initCache(const PhoneInfo &info)
//{
//    qDebug() << __LINE__ << __FUNCTION__ << info.strPhoneID << info.type;
//    QSize sIcon = QSize(ITEM_SIZE_ALBUM - 34, ITEM_SIZE_ALBUM - 34); //边框

//    PhoneFileThread *tRead_photo = new PhoneFileThread(UIService::getService()->getQspMainWnd());
//    connect(tRead_photo, &PhoneFileThread::finished, tRead_photo, &PhoneFileThread::deleteLater);
//    tRead_photo->readPhoto("", info.strPhoneID, sIcon, info.type, true);
//    tRead_photo->start();

//    PhoneFileThread *tRead_video = new PhoneFileThread(UIService::getService()->getQspMainWnd());
//    connect(tRead_video, &PhoneFileThread::finished, tRead_video, &PhoneFileThread::deleteLater);
//    tRead_video->readVideo("", info.strPhoneID, sIcon, info.type, true);
//    tRead_video->start();
//}

//void PhotoVideoGridWidget::updatePhoneInfo(const PhoneInfo &info)
//{
//    qDebug() << __LINE__ << __FUNCTION__;
//    if (info == m_phoneInfo && !SpinnerIsVisible()) {
//        if (m_tRead != nullptr && m_tRead->isRunning()) {
//            refreshUI();
//            return;
//        }
//    }
//    qDebug() << __LINE__ << __FUNCTION__;
//    if (info == m_phoneInfo && !m_listPhotoItem.isEmpty()) {
//        refreshUI();
//        calcTotal();
//        return;
//    }
//    qDebug() << __LINE__ << __FUNCTION__;
//    m_strLastPath.clear();
//    m_phoneInfo = info;
//    m_bIsAlbum = true;
//    refreshMode();
//}

///**
// * @brief PhotoVideoGridWidget::setWidgetBtnState    设置 顶部标题栏 按钮状态
// */
//void PhotoVideoGridWidget::setWidgetBtnState()
//{
//    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Left_Btn, true);
//    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Right_Btn, true);
//    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_New_Folder_Btn, false);
//    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Export_Btn, true);
//    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Delete_Btn, true);

//    int nIndex = m_pStackViewLayout->currentIndex();

//    slotStackedCurrentChanged(nIndex);
//}

//void PhotoVideoGridWidget::clearWidgetInfo(const QString &devInfo)
//{
//    clearCache(devInfo);
//    if (devInfo == m_phoneInfo.strPhoneID) {
//        m_phoneInfo.strPhoneID.clear();
//        clear();
//    }
//}

//void PhotoVideoGridWidget::refreshWidgetData()
//{
//    //导入中不让刷新
//    //    if (!d_ptr->m_pImportProgress->isHidden()) {
//    //        sendWarnMessage(TrObject::getInstance()->getFileOperationText(Processing_data));
//    //        return;
//    //    }
//    //清空缓存
//    if (m_bIsAlbum) {
//        clearCache();
//    }
//    refreshMode(m_strLastPath);
//}

//void PhotoVideoGridWidget::onFileError(int nErrorCode, QString strErrorMsg)
//{
//    if (nErrorCode == PFERROR_DELETE_FILE) {
//    } else if (nErrorCode == PFERROR_NOTFIND_DCIM) {
//        //        strErrorMsg = QString("[%1]%2").arg(m_phoneInfo.strDevName).arg(strErrorMsg);
//        spinnerStop();
//    } else if (nErrorCode == PEERROR_NO_FILE) {
//        spinnerStop();
//        calcTotal();
//        return;
//    } else if (nErrorCode == PEERROR_END) {
//        spinnerStop();
//        calcTotal();
//        loadImage(sender()->property("filelist").toStringList());

//        return;
//    }
//    sendWarnMessage(strErrorMsg);
//}

//void PhotoVideoGridWidget::onFindPicture(PhoneFileInfo &info)
//{
//    if (m_bIsAlbum != info.bIsDir)
//        return;

//    if (!info.bIsDir) {
//        reloadFileInfo(info);
//    } else {
//        //验重
//        foreach (ImageItemWidget *item, m_listPhotoItem) {
//            if (item->getFileInfo().dirPath == info.dirPath) {
//                return;
//            }
//        }
//    }

//    auto itemWidget = new ImageItemWidget(m_sItem, m_sIcon, this);
//    m_listPhotoItem.append(itemWidget);
//    connect(itemWidget, &ImageItemWidget::sigEnterFolder, this, &PhotoVideoGridWidget::onEnterFolder);
//    connect(itemWidget, &ImageItemWidget::sigSelectChanged, this, &PhotoVideoGridWidget::onItemSelectChanged);
//    connect(itemWidget, &ImageItemWidget::sigReloadImage, this, &PhotoVideoGridWidget::onReloadImage);
//    itemWidget->setPhoneFileInfo(info);

//    DWidget *wgt = new DWidget(this);
//    QVBoxLayout *vlayout = new QVBoxLayout(wgt);
//    vlayout->setSpacing(0);
//    vlayout->setMargin(0);
//    vlayout->addWidget(itemWidget);
//    if (info.bIsDir) {
//        DLabel *lbl_name = new DLabel(wgt);

//        QString strName =
//            QString("%1").arg(info.dirPath.right(info.dirPath.length() - info.dirPath.lastIndexOf('/') - 1));

//        QFontMetrics fontWidth(lbl_name->font()); //得到每个字符的宽度
//        QString elideNote = fontWidth.elidedText(strName, Qt::ElideRight, 80);
//        lbl_name->setText(elideNote); //显示省略好的字符串

//        QString strCount = QString("(%1)").arg(info.fileCount);

//        DLabel *lbl_count = new DLabel(strCount, wgt);
//        lbl_count->setAutoFillBackground(true);
//        DPalette pa = DApplicationHelper::instance()->palette(lbl_count);
//        pa.setColor(DPalette::WindowText, pa.textTips().color());
//        DApplicationHelper::instance()->setPalette(lbl_count, pa);

//        QHBoxLayout *hLayout = new QHBoxLayout();
//        hLayout->addStretch();
//        hLayout->addWidget(lbl_name);
//        hLayout->addWidget(lbl_count);
//        hLayout->addStretch();
//        vlayout->addLayout(hLayout);
//    }

//    m_mainLayout->addWidget(wgt);

//    m_btnChoose->setText(TrObject::getInstance()->getDlgBtnText(Select_All));
//    m_pTreeViewFrame->setHeaderViewState(false);
//    refreshStatusBar();

//    auto model = m_pTreeViewFrame->getSourceModel();
//    QVariant data;
//    data.setValue(info);
//    model->appendData(data);
//}

//void PhotoVideoGridWidget::onReloadImage(const QString &path, const QPixmap &data)
//{
//    qobject_cast<PhotoTreeFrame *>(m_pTreeViewFrame)->reloadImage(path, data);
//}

//void PhotoVideoGridWidget::reloadFileInfo(PhoneFileInfo &info)
//{
//    QFileInfo fileInfo;
//    fileInfo.setFile(info.path);
//    if (!fileInfo.exists()) {
//        info.size = 0;
//        //        info.lastModified = "";
//        return;
//    }

//    info.size = quint64(fileInfo.size());
//    //    info.lastModifiedTime_t = fileInfo.lastModified().toTime_t();
//    //    info.lastModified = fileInfo.lastModified().toString("yyyy-MM-dd hh:mm:ss");
//}

///**
// * @brief PhotoVideoGridWidget::onEnterFolder    进入文件夹
// * @param strPath   文件夹名称
// */
//void PhotoVideoGridWidget::onEnterFolder(const QString &strPath)
//{
//    RightTitleWidget::getInstance()->setWidgetBtnChecked(RightTitleWidget::E_Icon_View_Btn, true);
//    RightTitleWidget::getInstance()->setWidgetBtnChecked(RightTitleWidget::E_List_View_Btn, false);

//    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Delete_Btn, false);

//    m_bIsAlbum = false;
//    m_strLastPath = strPath;
//    refreshMode(strPath);
//}

//void PhotoVideoGridWidget::onItemSelectChanged()
//{
//    auto item = static_cast<ImageItemWidget *>(QObject::sender());
//    if (item) {
//        auto info = item->getFileInfo();
//        if (!info.bIsDir) {
//            // wzx 双击进入文件夹， 不应直接对该文件夹 进行选中
//            static_cast<PhotoTreeFrame *>(m_pTreeViewFrame)->setItemSelect(info.path, item->isChecked());
//        }
//    }

//    // 双击也会调用到这里
//    setTitleBtnEnble();
//    refreshStatusBar();
//}

//void PhotoVideoGridWidget::onDeleteBtnClicked()
//{
//    if (!this->isVisible()) {
//        return;
//    }
//    if (!d_ptr->m_pImportProgress->isHidden()) {
//        sendWarnMessage(TrObject::getInstance()->getFileOperationText(Processing_data));
//        return;
//    }
//    m_nDeletePendingData.clear();
//    // delete
//    if (m_currFileViewMode == VIEW_ICON_MODE) {
//        foreach (auto item_wgt, getCheckedItems()) {
//            if (item_wgt->getFileInfo().bIsDir) {
//                m_nDeletePendingData.append(item_wgt->getFileInfo().dirPath);
//            } else {
//                m_nDeletePendingData.append(item_wgt->getFileInfo().path);
//            }
//        }
//    } else
//        m_nDeletePendingData.append(static_cast<PhotoTreeFrame *>(m_pTreeViewFrame)->getSelectFilePath());

//    if (m_nDeletePendingData.isEmpty()) {
//        sendWarnMessage(TrObject::getInstance()->getFileOperationText(Please_select_a_file));
//        return;
//    }
//    // confirm dialog
//    QString strTitle;
//    if (m_nDeletePendingData.count() == 1) {
//        strTitle = TrObject::getInstance()->getFileOperationText(Delete_it);
//    } else {
//        strTitle = TrObject::getInstance()->getFileOperationText(Delete_items);
//    }
//    bool bRet = execMsgBox(strTitle);
//    if (!bRet) { //选择了取消
//        m_nDeletePendingData.clear();
//        return;
//    }

//    // delete
//    if (d_ptr->m_tPhotoVideoImport != nullptr) {
//        d_ptr->m_tPhotoVideoImport->deleteLater();
//        d_ptr->m_tPhotoVideoImport = nullptr;
//    }
//    d_ptr->m_tPhotoVideoImport = new PhoneFileThread(this);
//    //    connect(m_tImport, &PhoneFileThread::finished, &PhoneFileThread::deleteLater);
//    connect(d_ptr->m_tPhotoVideoImport, &PhoneFileThread::error, this, &PhotoVideoGridWidget::onFileError);
//    connect(d_ptr->m_tPhotoVideoImport, &PhoneFileThread::sigProgress, this, &PhotoVideoGridWidget::onProgress);
//    connect(d_ptr->m_tPhotoVideoImport, &PhoneFileThread::sigResult, this, &PhotoVideoGridWidget::onProgressResult);
//    PhoneFileType type;
//    if (m_runMode == Photo) {
//        type = IMAGE;
//    } else {
//        type = VIDEO;
//    }
//    d_ptr->m_tPhotoVideoImport->delFile(m_nDeletePendingData, type);
//    d_ptr->m_tPhotoVideoImport->start();
//}

//void PhotoVideoGridWidget::onExportBtnClicked()
//{
//    if (!this->isVisible()) {
//        return;
//    }
//    //目前 m_exportWgt是模态 不会遇到这个问题
//    if (!d_ptr->m_pExportProgressDlg->isHidden()) {
//        sendWarnMessage(TrObject::getInstance()->getFileOperationText(Processing_data));
//        return;
//    }
//    // export 是否增加取消选择
//    QStringList listPendingData;
//    if (m_currFileViewMode == VIEW_ICON_MODE) {
//        foreach (auto item_wgt, getCheckedItems()) {
//            if (item_wgt->getFileInfo().bIsDir) {
//                listPendingData.append(item_wgt->getFileInfo().dirPath);
//            } else {
//                listPendingData.append(item_wgt->getFileInfo().path);
//            }
//        }
//    } else
//        listPendingData.append(static_cast<PhotoTreeFrame *>(m_pTreeViewFrame)->getSelectFilePath());

//    if (listPendingData.isEmpty()) {
//        sendWarnMessage(TrObject::getInstance()->getFileOperationText(Please_select_a_file));
//        return;
//    }

//    QString strResPath = getExportPath();
//    if (strResPath.isEmpty()) {
//        return;
//    }

//    //验重
//    checkFileRepeat(listPendingData, strResPath);

//    // copy
//    if (d_ptr->m_tPhotoVideoExport != nullptr) {
//        d_ptr->m_tPhotoVideoExport->deleteLater();
//        d_ptr->m_tPhotoVideoExport = nullptr;
//    }
//    d_ptr->m_tPhotoVideoExport = new PhoneFileThread();
//    //    connect(m_tEmport, &PhoneFileThread::finished, &PhoneFileThread::deleteLater);
//    connect(d_ptr->m_tPhotoVideoExport, &PhoneFileThread::error, this, &PhotoVideoGridWidget::onFileError);
//    connect(d_ptr->m_tPhotoVideoExport, &PhoneFileThread::sigProgress, this, &PhotoVideoGridWidget::onProgress);
//    connect(d_ptr->m_tPhotoVideoExport, &PhoneFileThread::sigResult, this, &PhotoVideoGridWidget::onProgressResult);
//    PhoneFileType type;
//    if (m_runMode == Photo) {
//        type = IMAGE;
//    } else {
//        type = VIDEO;
//    }
//    QString strRootPath;
//    if (m_phoneInfo.type == Mount_Android)
//        strRootPath = Utils::getAndroidMountRootPath(m_phoneInfo.strPhoneID);
//    d_ptr->m_tPhotoVideoExport->copyFile(listPendingData, strResPath, PhoneFileThread::ExportFile, type,
//                                         m_phoneInfo.strProductVersion, strRootPath, m_phoneInfo.strPhoneID);
//    d_ptr->m_tPhotoVideoExport->start();
//}

//void PhotoVideoGridWidget::onImportBtnClicked()
//{
//    if (!this->isVisible()) {
//        return;
//    }
//    if (!SpinnerIsVisible()) {
//        sendWarnMessage(TrObject::getInstance()->getFileOperationText(Processing_data));
//        return;
//    }
//    // import
//    QStringList listPendingData = DFileDialog::getOpenFileNames(
//        this, "", DStandardPaths::writableLocation(QStandardPaths::DesktopLocation), getCurFilters());
//    if (listPendingData.isEmpty())
//        return;
//    //验重
//    checkFileRepeat(listPendingData, m_strLastPath, true);

//    // copy
//    if (d_ptr->m_tPhotoVideoImport != nullptr) {
//        d_ptr->m_tPhotoVideoImport->deleteLater();
//        d_ptr->m_tPhotoVideoImport = nullptr;
//    }
//    d_ptr->m_tPhotoVideoImport = new PhoneFileThread();
//    //    connect(m_tImport, &PhoneFileThread::finished, &PhoneFileThread::deleteLater);
//    connect(d_ptr->m_tPhotoVideoImport, &PhoneFileThread::error, this, &PhotoVideoGridWidget::onFileError);
//    connect(d_ptr->m_tPhotoVideoImport, &PhoneFileThread::sigProgress, this, &PhotoVideoGridWidget::onProgress);
//    connect(d_ptr->m_tPhotoVideoImport, &PhoneFileThread::sigResult, this, &PhotoVideoGridWidget::onProgressResult);
//    PhoneFileType type;
//    if (m_runMode == Photo) {
//        type = IMAGE;
//    } else {
//        type = VIDEO;
//    }
//    QString strRootPath;
//    if (m_phoneInfo.type == Mount_Android)
//        strRootPath = Utils::getAndroidMountRootPath(m_phoneInfo.strPhoneID);
//    d_ptr->m_tPhotoVideoImport->copyFile(listPendingData, m_strLastPath, PhoneFileThread::ImportFile, type,
//                                         m_phoneInfo.strProductVersion, strRootPath, m_phoneInfo.strPhoneID);
//    d_ptr->m_tPhotoVideoImport->start();
//}

//void PhotoVideoGridWidget::onGridBtnClicked()
//{
//    m_currFileViewMode = VIEW_ICON_MODE;
//    m_gridWgt->setContentsMargins(0, 0, 0, 0);
//    m_pStackViewLayout->setCurrentIndex(m_currFileViewMode);
//    //    if (d_ptr->m_pImportProgress && d_ptr->m_pImportProgress->isVisible()) {
//    //        d_ptr->m_pImportProgress->raise();
//    //    }
//}

//void PhotoVideoGridWidget::onListBtnClicked()
//{
//    m_currFileViewMode = VIEW_LIST_MODE;
//    m_pStackViewLayout->setContentsMargins(0, 0, 0, 0);
//    m_pStackViewLayout->setCurrentIndex(m_currFileViewMode);
//    //    if (d_ptr->m_pImportProgress && d_ptr->m_pImportProgress->isVisible()) {
//    //        d_ptr->m_pImportProgress->raise();
//    //    }
//}

//void PhotoVideoGridWidget::onProgress(int nRunMode, int nCur, int nTotal, QString strFileName)
//{
//    qDebug() << nRunMode << nCur << nTotal << strFileName;
//    switch (nRunMode) {
//    case PhoneFileThread::ImportFile:
//        //        if (d_ptr->m_pImportProgress->isHidden()) {
//        //            d_ptr->m_pImportProgress->setCloseBtnHidden(false);
//        //            d_ptr->m_pImportProgress->show();
//        //            d_ptr->m_pImportProgress->raise();
//        //        }
//        //        d_ptr->m_pImportProgress->updateProgressText(TrObject::getInstance()->getImportExportText(Importing) + "  " + strFileName);
//        //弹窗
//        if (d_ptr->m_pImportExportProgressDlg->isHidden()) {
//            d_ptr->m_pImportExportProgressDlg->exec();
//        }
//        d_ptr->m_pImportExportProgressDlg->setProgressBarRange(0, nTotal);
//        d_ptr->m_pImportExportProgressDlg->updateProgressBarValue(nCur);
//        break;
//    case PhoneFileThread::ExportFile:
//        //弹窗
//        if (d_ptr->m_pExportProgressDlg->isHidden()) {
//            d_ptr->m_pExportProgressDlg->exec();
//        }
//        d_ptr->m_pExportProgressDlg->setProgressBarRange(0, nTotal);
//        d_ptr->m_pExportProgressDlg->updateProgressBarValue(nCur);
//        break;
//    case PhoneFileThread::DelFile:
//        if (d_ptr->m_pImportProgress->isHidden()) {
//            d_ptr->m_pImportProgress->setCloseBtnHidden(true);
//            d_ptr->m_pImportProgress->show();
//            d_ptr->m_pImportProgress->raise();
//        }
//        d_ptr->m_pImportProgress->updateProgressText(TrObject::getInstance()->getFileOperationText(Deleting) + "  " + strFileName);
//        //后续移植到这里刷新界面
//        break;
//    }
//}

//void PhotoVideoGridWidget::onProgressResult(int nRunMode, int nCount, int nSuccess, int nFailed, bool bIsStop)
//{
//    qDebug() << __LINE__ << m_runMode << nRunMode << nCount << nSuccess << bIsStop;
//    Q_UNUSED(nCount)
//    Q_UNUSED(nFailed)
//    switch (nRunMode) {
//    case PhoneFileThread::ImportFile:
//        d_ptr->m_pImportExportProgressDlg->hide();
//        if (nSuccess > 0) { //防止没导入也刷新
//            refreshWidgetData();
//            if (bIsStop) {
//                if (m_runMode == Photo) {
//                    sendOkMessage(TrObject::getInstance()->getPhotoImportExportText(photo_import_stop).arg(nSuccess));
//                } else if (m_runMode == Video) {
//                    sendOkMessage(TrObject::getInstance()->getVideoImportExportText(video_import_stop).arg(nSuccess));
//                }
//            } else {
//                if (m_runMode == Photo) {
//                    sendOkMessage(
//                        TrObject::getInstance()->getPhotoImportExportText(photo_import_success).arg(nSuccess));
//                } else if (m_runMode == Video) {
//                    sendOkMessage(
//                        TrObject::getInstance()->getVideoImportExportText(video_import_success).arg(nSuccess));
//                }
//            }
//        }
//        break;
//    case PhoneFileThread::ExportFile:
//        d_ptr->m_pExportProgressDlg->hide();
//        if (nSuccess > 0) { //防止没导入也刷新
//            if (bIsStop) {
//                if (m_runMode == Photo) {
//                    sendOkMessage(
//                        TrObject::getInstance()->getPhotoImportExportText(photo_export_stop_count).arg(nSuccess));
//                } else if (m_runMode == Video) {
//                    sendOkMessage(
//                        TrObject::getInstance()->getVideoImportExportText(video_export_stop_count).arg(nSuccess));
//                }
//            } else {
//                if (m_runMode == Photo) {
//                    sendOkMessage(
//                        TrObject::getInstance()->getPhotoImportExportText(photo_export_success).arg(nSuccess));
//                } else if (m_runMode == Video) {
//                    sendOkMessage(
//                        TrObject::getInstance()->getVideoImportExportText(video_export_success).arg(nSuccess));
//                }
//            }
//        }
//        break;
//    case PhoneFileThread::DelFile:
//        clearCache();

//        d_ptr->m_pImportProgress->hide();
//        foreach (QString strItemPath, m_nDeletePendingData) {
//            delItem(strItemPath, (nFailed > 0));
//        }
//        m_nDeletePendingData.clear();

//        //可能会删除失败
//        if (nFailed > 0 || bIsStop)
//            refreshMode(m_strLastPath);

//        //  结束之后 设置按钮的相应状态
//        setTitleBtnEnble();

//        break;
//    }
//    refreshStatusBar();
//}

//void PhotoVideoGridWidget::onListWgtFileSelectChanged(QString strFilePath, bool bSelected)
//{
//    foreach (auto item_wgt, m_listPhotoItem) {
//        if (item_wgt->getFileInfo().path == strFilePath) {
//            if (bSelected)
//                item_wgt->check();
//            else
//                item_wgt->uncheck();

//            break;
//        }
//    }
//    refreshStatusBar();
//}

//void PhotoVideoGridWidget::onListWgtSelectAll(bool bSelected)
//{
//    selectAll(bSelected);
//}

//void PhotoVideoGridWidget::onImportProgressClose()
//{
//    d_ptr->m_tPhotoVideoImport->deleteLater();
//    d_ptr->m_tPhotoVideoImport = nullptr;
//}

//void PhotoVideoGridWidget::onExportProgressClose()
//{
//    d_ptr->m_tPhotoVideoExport->deleteLater();
//    d_ptr->m_tPhotoVideoExport = nullptr;
//}

//void PhotoVideoGridWidget::loadImage(const QStringList &listFilePath)
//{
//    Q_UNUSED(listFilePath)
//    //    qDebug() << __LINE__ << listFilePath;
//}

//void PhotoVideoGridWidget::slotTitleWidgetBtnClicked(const int &nId)
//{
//    if (this->isVisible() == false)
//        return;

//    if (nId == TitleWidget::E_Left_Btn) {
//        if (m_bIsAlbum)
//            return;
//        m_bIsAlbum = true;
//        refreshMode();
//    } else if (nId == TitleWidget::E_Right_Btn) {
//        if (!m_bIsAlbum)
//            return;
//        if (m_strLastPath.isEmpty())
//            return;
//        m_bIsAlbum = false;
//        refreshMode(m_strLastPath);
//    } else if (nId == RightTitleWidget::E_Icon_View_Btn) {
//        onGridBtnClicked();
//    } else if (nId == RightTitleWidget::E_List_View_Btn) {
//        onListBtnClicked();
//    } else if (nId == TitleWidget::E_Delete_Btn) {
//        onDeleteBtnClicked();
//    } else if (nId == TitleWidget::E_Import_Btn) {
//        onImportBtnClicked();
//    } else if (nId == TitleWidget::E_Export_Btn) {
//        onExportBtnClicked();
//    }
//}

////  判断 statck 当前的显示 进行按钮的check 状态设置
//void PhotoVideoGridWidget::slotStackedCurrentChanged(int iIndex)
//{
//    if (iIndex == 0) { //  网格
//        RightTitleWidget::getInstance()->setWidgetBtnChecked(RightTitleWidget::E_Icon_View_Btn, true);

//        if (m_btnChoose) {
//            m_btnChoose->show();
//        }
//    } else { // 列表
//        RightTitleWidget::getInstance()->setWidgetBtnChecked(RightTitleWidget::E_List_View_Btn, true);

//        if (m_btnChoose) {
//            m_btnChoose->hide();
//        }
//    }

//    setTitleBtnEnble();
//}

//void PhotoVideoGridWidget::initUI()
//{
//    //  m_pStackViewLayout  父类 已经申明好了， 直接使用即可
//    m_gridWgt = new QScrollArea(this);
//    m_gridWgt->setLineWidth(0);

//    m_pTreeViewFrame = new PhotoTreeFrame(m_pWidgetType, this);
//    m_pTreeViewFrame->pCustomTreeView()->setHeaderView();

//    m_pTreeViewFrame->pCustomTreeView()->setColumnWidth(0, 50);
//    m_pTreeViewFrame->pCustomTreeView()->setColumnWidth(1, 350);
//    m_pTreeViewFrame->pCustomTreeView()->setColumnWidth(3, 150);

//    m_pTreeViewFrame->pCustomTreeView()->header()->setSortIndicator(3, Qt::DescendingOrder);
//    m_pTreeViewFrame->pCustomTreeView()->header()->setSortIndicatorShown(false);

//    //    m_pTreeViewFrame->setContentsMargins(10, 0, 10, 10);

//    auto tempWidget = getTreeWidget(this);

//    m_pStackViewLayout = new QStackedLayout();
//    connect(m_pStackViewLayout, &QStackedLayout::currentChanged, this,
//            &PhotoVideoGridWidget::slotStackedCurrentChanged);
//    m_pStackViewLayout->setMargin(0);
//    m_pStackViewLayout->setSpacing(0);
//    m_pStackViewLayout->insertWidget(VIEW_ICON_MODE, m_gridWgt);
//    m_pStackViewLayout->insertWidget(VIEW_LIST_MODE, tempWidget);
//    m_pStackViewLayout->setCurrentIndex(VIEW_ICON_MODE);

//    QHBoxLayout *topLayout = new QHBoxLayout;
//    topLayout->setContentsMargins(15, 10, 30, 10);
//    lbl_title = new DLabel();
//    DFontSizeManager::instance()->bind(lbl_title, DFontSizeManager::T3, QFont::DemiBold);

//    topLayout->addWidget(lbl_title);
//    topLayout->addStretch();
//    m_btnChoose = new DCommandLinkButton(TrObject::getInstance()->getDlgBtnText(Select_All));
//    connect(m_btnChoose, &DCommandLinkButton::clicked, [this]() {
//        bool bIsAllChecked = isSelectedAll();
//        this->selectAll(!bIsAllChecked);
//        static_cast<PhotoTreeFrame *>(m_pTreeViewFrame)->selectAllEx(!bIsAllChecked);
//    });
//    topLayout->addWidget(m_btnChoose);

//    m_mainLayout = new DFlowLayout();
//    m_mainLayout->setMargin(0);
//    m_mainLayout->setSpacing(0);

//    m_gridWgt->setFrameShape(QFrame::NoFrame);
//    m_gridWgt->setWidgetResizable(true);
//    m_gridWgt->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
//    DWidget *wgt = new DWidget(m_gridWgt);
//    wgt->setLayout(m_mainLayout);
//    m_gridWgt->setWidget(wgt);

//    m_pMmainLayout = new QVBoxLayout(this);
//    m_pMmainLayout->setMargin(0);
//    m_pMmainLayout->setSpacing(0);
//    m_pMmainLayout->addLayout(topLayout);
//    m_pMmainLayout->addLayout(m_pStackViewLayout);
//    m_pMmainLayout->addWidget(m_pSpinnerWidget);

//    d_ptr->m_pImportProgress = new ImportProgressWgt(this);
//    connect(d_ptr->m_pImportProgress, &ImportProgressWgt::sigCloseBtnClicked, this, &PhotoVideoGridWidget::onImportProgressClose);
//    d_ptr->m_pImportProgress->hide();

//    d_ptr->m_pImportExportProgressDlg = new ImportExportProgressDlg(TrObject::getInstance()->getDlgTitle(dlg_Importing), this);
//    connect(d_ptr->m_pImportExportProgressDlg, &ImportExportProgressDlg::sigCloseBtnClicked, this, &PhotoVideoGridWidget::onImportProgressClose);
//    d_ptr->m_pImportExportProgressDlg->hide();

//    d_ptr->m_pExportProgressDlg = new ExportProgressDlg(this);
//    connect(d_ptr->m_pExportProgressDlg, &ExportProgressDlg::sigCloseBtnClicked, this, &PhotoVideoGridWidget::onExportProgressClose);
//    d_ptr->m_pExportProgressDlg->hide();
//}

//void PhotoVideoGridWidget::initConn()
//{
//    connect(qobject_cast<PhotoTreeFrame *>(m_pTreeViewFrame), &PhotoTreeFrame::sigFileSelectChanged, this,
//            &PhotoVideoGridWidget::onListWgtFileSelectChanged);
//    connect(m_pTreeViewFrame->pCustomTreeView(), &BaseTreeView::sigSelectAll, this,
//            &PhotoVideoGridWidget::onListWgtSelectAll);
//}

//void PhotoVideoGridWidget::clear()
//{
//    for (int i = 0; i < m_mainLayout->count(); ++i) {
//        m_mainLayout->itemAt(i)->widget()->deleteLater();
//    }
//    m_listPhotoItem.clear();

//    m_pTreeViewFrame->pCustomTreeView()->clearModel();
//    //    m_pTreeViewFrame->pCustomTreeView()->setHeaderViewState(false);
//    //    static_cast<PhotoTreeFrame *>(m_pTreeViewFrame)->clear();
//    //    m_listWgt->clear();
//}

//void PhotoVideoGridWidget::delItem(QString strPath, bool bHasFailed)
//{
//    static_cast<PhotoTreeFrame *>(m_pTreeViewFrame)->delItem(strPath);
//    //    m_listWgt->delItem(strPath);
//    foreach (auto item_wgt, m_listPhotoItem) {
//        QString strItemPath;
//        if (item_wgt->getFileInfo().bIsDir) {
//            strItemPath = item_wgt->getFileInfo().dirPath;
//        } else {
//            strItemPath = item_wgt->getFileInfo().path;
//        }
//        if (strItemPath == strPath) {
//            item_wgt->parent()->deleteLater();
//            m_listPhotoItem.removeOne(item_wgt);
//            break;
//        }
//    }
//    if (m_listPhotoItem.isEmpty() && !m_bIsAlbum && !bHasFailed) { //全部删完
//        restoreAll();
//    }
//}
//#include "ThreadService.h"
//void PhotoVideoGridWidget::refreshMode(QString strPath)
//{
//    spinnerStart();
//    clear();

//    //    if (m_runMode == Photo)
//    //        ThreadService::getService()->stopTask(ThreadService::E_Image_Render_Task);
//    //    else
//    //        ThreadService::getService()->stopTask(ThreadService::E_Video_Render_Task);

//    if (m_tRead != nullptr && m_tRead->isRunning()) {
//        disconnect(m_tRead, &PhoneFileThread::sigFindPicture, this, &PhotoVideoGridWidget::onFindPicture);
//        disconnect(m_tRead, &PhoneFileThread::error, this, &PhotoVideoGridWidget::onFileError);
//        m_tRead->quitImmediately();
//        m_tRead->stopImmediately();
//        m_tRead = nullptr;
//    }

//    m_tRead = new PhoneFileThread();
//    connect(m_tRead, &PhoneFileThread::sigFindPicture, this, &PhotoVideoGridWidget::onFindPicture);
//    connect(m_tRead, &PhoneFileThread::error, this, &PhotoVideoGridWidget::onFileError);
//    connect(m_tRead, &PhoneFileThread::finished, m_tRead, [this]() {
//        PhoneFileThread *sender_t = qobject_cast<PhoneFileThread *>(sender());
//        if (sender_t == nullptr)
//            return;
//        sender_t->deleteLater();

//        if (sender_t->currentThreadId() == m_tRead->currentThreadId())
//            m_tRead = nullptr;
//    });

//    refreshUI();

//    switch (int(m_runMode)) {
//    case Photo:
//        if (m_bIsAlbum) {
//            m_sItem = QSize(ITEM_SIZE_ALBUM, ITEM_SIZE_ALBUM);
//            m_sIcon = QSize(ITEM_SIZE_ALBUM - 34, ITEM_SIZE_ALBUM - 34); //边框
//            lbl_title->setText(TrObject::getInstance()->getListViewText(List_Photo));
//            m_tRead->readPhoto(strPath, m_phoneInfo.strPhoneID, m_sIcon, m_phoneInfo.type, true);
//            m_mainLayout->setContentsMargins(30, 0, 29, 0);
//            m_mainLayout->setSpacing(18);
//        } else {
//            m_sItem = QSize(ITEM_SIZE_PIC, ITEM_SIZE_PIC);
//            m_sIcon = QSize(ITEM_SIZE_PIC - 16, ITEM_SIZE_PIC - 16); //边框

//            QString tempTitle = strPath.section('/', -1);

//            QFontMetrics fontWidth(lbl_title->font()); //得到每个字符的宽度
//            QString elideNote = fontWidth.elidedText(tempTitle, Qt::ElideRight, 600);
//            lbl_title->setText(elideNote); //显示省略好的字符串

//            //            lbl_title->setText(strPath.section('/', -1));
//            m_tRead->readPhoto(strPath, m_phoneInfo.strPhoneID, m_sIcon, m_phoneInfo.type);
//            m_mainLayout->setContentsMargins(7, 0, 6, 0);
//            m_mainLayout->setSpacing(3);
//        }
//        break;
//    case Video:
//        if (m_bIsAlbum) {
//            m_sItem = QSize(ITEM_SIZE_ALBUM, ITEM_SIZE_ALBUM);
//            m_sIcon = QSize(ITEM_SIZE_ALBUM - 34, ITEM_SIZE_ALBUM - 34); //边框
//            lbl_title->setText(TrObject::getInstance()->getListViewText(List_Video));
//            m_tRead->readVideo(strPath, m_phoneInfo.strPhoneID, m_sIcon, m_phoneInfo.type, true);
//            m_mainLayout->setContentsMargins(30, 0, 29, 0);
//            m_mainLayout->setSpacing(18);
//        } else {
//            m_sItem = QSize(ITEM_SIZE_VIDEO, ITEM_SIZE_VIDEO);
//            m_sIcon = QSize(ITEM_SIZE_VIDEO - 16, ITEM_SIZE_VIDEO - 16); //边框

//            QString tempTitle = strPath.section('/', -1);
//            QFontMetrics fontWidth(lbl_title->font()); //得到每个字符的宽度
//            QString elideNote = fontWidth.elidedText(tempTitle, Qt::ElideRight, 600);
//            lbl_title->setText(elideNote); //显示省略好的字符串

//            //            lbl_title->setText(strPath.section('/', -1));
//            m_tRead->readVideo(strPath, m_phoneInfo.strPhoneID, m_sIcon, m_phoneInfo.type);
//            m_mainLayout->setContentsMargins(7, 0, 6, 0);
//            m_mainLayout->setSpacing(3);
//        }
//        break;
//    }

//    m_tRead->start();

//    static_cast<PhotoTreeFrame *>(m_pTreeViewFrame)->resetHeaderState();
//    //    m_listWgt->resetHeaderState();
//}

//void PhotoVideoGridWidget::refreshUI()
//{
//    if (m_bIsAlbum) {
//        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Left_Btn, false);
//        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Right_Btn, false);

//        if (!m_strLastPath.isEmpty()) {
//            TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Right_Btn, true);
//        }

//        RightTitleWidget::getInstance()->setWidgetBtnVisible(RightTitleWidget::E_Icon_View_Btn, false);
//        RightTitleWidget::getInstance()->setWidgetBtnVisible(RightTitleWidget::E_List_View_Btn, false);

//        TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Import_Btn, false);

//        m_pStackViewLayout->setCurrentIndex(VIEW_ICON_MODE);
//        d_ptr->m_pImportProgress->raise();
//    } else {
//        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Left_Btn, true);
//        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Right_Btn, false);

//        RightTitleWidget::getInstance()->setWidgetBtnVisible(RightTitleWidget::E_Icon_View_Btn, true);
//        RightTitleWidget::getInstance()->setWidgetBtnVisible(RightTitleWidget::E_List_View_Btn, true);

//        TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Import_Btn, true);
//        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Import_Btn, true);
//    }
//    if (m_phoneInfo.type == Mount_Ios) {
//        TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Import_Btn, false);
//    }
//    setTitleBtnEnble();
//    refreshStatusBar();
//}

//void PhotoVideoGridWidget::restoreAll()
//{
//    m_bIsAlbum = true;
//    m_strLastPath.clear();
//    refreshMode();
//}

//void PhotoVideoGridWidget::selectAll(bool bSelected, bool bSendSig)
//{
//    if (m_listPhotoItem.isEmpty())
//        return;

//    foreach (auto itemPhoto, m_listPhotoItem) {
//        if (bSelected) {
//            itemPhoto->check(bSendSig);
//        } else {
//            itemPhoto->uncheck(bSendSig);
//        }
//    }
//    setTitleBtnEnble(); //会刷新select按钮
//    refreshStatusBar();
//}

//bool PhotoVideoGridWidget::isSelectedAll()
//{
//    if (m_btnChoose->text() == TrObject::getInstance()->getDlgBtnText(Select_All)) {
//        return false;
//    }
//    return true;
//    //    auto list_checked = getCheckedItems();
//    //    if (list_checked.count() == m_listPhotoItem.count() && list_checked.count() != 0)
//    //        return true;
//    //    return false;
//}

//bool PhotoVideoGridWidget::fileListContains(QStringList &listSrcFile, QString &strFile, QString &strSrcAbsolutePath)
//{
//    foreach (QString strItemFile, listSrcFile) {
//        if (strItemFile.endsWith("/" + strFile)) {
//            strSrcAbsolutePath = strItemFile;
//            return true;
//        }
//    }
//    return false;
//}

//void PhotoVideoGridWidget::checkFileRepeat(QStringList &listSrcFile, QString strDesPath, bool bRecount)
//{
//    Q_UNUSED(bRecount);

//    if (listSrcFile.isEmpty())
//        return;

//    QStringList list;
//    //导入
//    if (strDesPath == m_strLastPath) {
//        //从内存加载
//        foreach (ImageItemWidget *item, m_listPhotoItem) {
//            //防止过滤到目录子级
//            if (m_strLastPath == item->getFileInfo().path.section("/", 0, -2))
//                list.append(item->getFileInfo().path);
//        }
//    } else { //导出
//        QDir dir;
//        dir.setPath(strDesPath);
//        list = dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
//    }

//    bool bShowDlg = true;
//    int ret = 0;
//    for (int i = 0; i < list.size(); ++i) {
//        //        QFileInfo fileInfo = list.at(i);
//        //        QString strFileName = fileInfo.fileName();
//        QString strFileName = list.at(i).section("/", -1);
//        QString strSrcPath;
//        if (fileListContains(listSrcFile, strFileName, strSrcPath)) {
//            // 有重复 开始判断
//            if (bShowDlg) {
//                FileRepeatConfirmDlg confirmDlg(strFileName, strSrcPath, strDesPath, true, this, false);
//                ret = confirmDlg.exec();
//                bShowDlg = !confirmDlg.getAskCheckBoxValue();
//            }
//            if (ret == -1) { // 关闭
//                bShowDlg = false;
//                ret = 0;
//                listSrcFile.removeOne(strSrcPath);
//            } else if (ret == 0) { //跳过
//                listSrcFile.removeOne(strSrcPath);
//            } else if (ret == 1) { //替换
//                //                if (bRecount) // onProgressResult 会增加 所以先去掉
//                //                    --m_nCount;
//            } else if (ret == 2) { //共存
//                //                if (bRecount)
//                //                    ++m_nCount;
//            }
//        }
//    }
//}

//QList<ImageItemWidget *> PhotoVideoGridWidget::getCheckedItems()
//{
//    QList<ImageItemWidget *> listItems;

//    foreach (auto itemPhoto, m_listPhotoItem) {
//        if (itemPhoto->isChecked()) {
//            listItems.append(itemPhoto);
//        }
//    }
//    return listItems;
//}

//QString PhotoVideoGridWidget::getCurFilters()
//{
//    QString strRet;
//    QStringList listFilters;

//    switch (int(m_runMode)) {
//    case Photo:
//        strRet = "Images";
//        listFilters = PhoneFileThread::getImageFilters();
//        break;
//    case Video:
//        strRet = "Videos";
//        listFilters = PhoneFileThread::getVideoFilters();
//        break;
//    }

//    strRet += "(";
//    foreach (QString item, listFilters) {
//        strRet += item;
//        strRet += " ";
//    }
//    strRet += ")";
//    return strRet;
//}

//void PhotoVideoGridWidget::setTitleBtnEnble()
//{
//    auto list_checked = getCheckedItems();
//    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Export_Btn, !list_checked.isEmpty());
//    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Delete_Btn, !list_checked.isEmpty());

//    if (m_btnChoose) {
//        if (list_checked.count() == m_listPhotoItem.count() && list_checked.count() != 0) { //全选
//            m_btnChoose->setText(TrObject::getInstance()->getDlgBtnText(Unselect_All));
//        } else {
//            m_btnChoose->setText(TrObject::getInstance()->getDlgBtnText(Select_All));
//        }
//    }
//}

//void PhotoVideoGridWidget::refreshStatusBar()
//{
//    QString strText;
//    QList<ImageItemWidget *> list_checked = getCheckedItems();
//    if (!list_checked.isEmpty()) {
//        quint64 nFileSize = 0;
//        foreach (auto item_checked, list_checked) {
//            PhoneFileInfo item_info = item_checked->getFileInfo();
//            nFileSize += item_info.size;
//        }

//        strText = TrObject::getInstance()
//                      ->getFileOperationText(Total_item_selected_size)
//                      .arg(m_listPhotoItem.count())
//                      .arg(list_checked.count())
//                      .arg(Utils::dataSizeToString(nFileSize));
//    } else if (!m_listPhotoItem.isEmpty()) {
//        quint64 nFileSize = 0;
//        foreach (auto item_checked, m_listPhotoItem) {
//            PhoneFileInfo item_info = item_checked->getFileInfo();
//            nFileSize += item_info.size;
//        }

//        strText = TrObject::getInstance()
//                      ->getFileOperationText(items_size)
//                      .arg(m_listPhotoItem.count())
//                      .arg(Utils::dataSizeToString(nFileSize));
//    } else {
//        strText = TrObject::getInstance()->getFileOperationText(items_count).arg(0);
//    }

//    setShowText(strText);
//}

//void PhotoVideoGridWidget::clearCache(const QString &strPhoneID)
//{
//    QString strPendingPhoneID;
//    strPendingPhoneID = strPhoneID.isEmpty() ? m_phoneInfo.strPhoneID : strPhoneID;
//    qDebug() << __LINE__ << __FUNCTION__ << strPendingPhoneID;
//    switch (int(m_runMode)) {
//    case Photo:
//        PhoneFileThread::clearCache(PhoneFileThread::ReadPhoto, strPendingPhoneID);
//        break;
//    case Video:
//        PhoneFileThread::clearCache(PhoneFileThread::ReadVideo, strPendingPhoneID);
//        break;
//    }
//}

//void PhotoVideoGridWidget::PhotoVideoGridWidget::calcTotal()
//{
//    refreshStatusBar();
//}
