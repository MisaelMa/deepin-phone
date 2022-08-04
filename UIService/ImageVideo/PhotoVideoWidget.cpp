/*
* Copyright (C) 2019 ~ 2020 Deepin Technology Co., Ltd.
*
* Author:     yelei <yelei@uniontech.com>
* Maintainer: yelei <yelei@uniontech.com>
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

#include "PhotoVideoWidget.h"

#include <QDebug>
#include <QHeaderView>
#include <QDir>
#include <DFileDialog>
#include <DStandardPaths>
#include <DApplicationHelper>

#include "FileOperate/phonefilethread.h"
#include "FileOperate/DeletePhoneFileThread.h"
#include "FileOperate/CopyFileThread.h"

#include "PhotoTreeFrame.h"
#include "PhotoListFrame.h"
#include "PhoneTreePhotoModel.h"

#include "widget/SpinnerWidget.h"
#include "widget/progresswidget.h"
#include "widget/TitleWidget.h"
#include "widget/RightTitleWidget.h"
#include "widget/CustomWidget_p.h"

#include "UIService.h"
#include "TrObject.h"
#include "utils.h"
#include "FileUtils.h"
#include "FileOperate/ImageVideoManager.h"

DCORE_USE_NAMESPACE

#define ITEM_SIZE_PIC 116
#define ITEM_SIZE_VIDEO 156
#define ITEM_SIZE_ALBUM 184 // 184 150 相册
#define ITEM_SIZE_ALBUM_NAME 28 // 20 名称高度

extern QMutex g_enterDirMutex; //全局加载文件时读取目录信息用

PhotoVideoWidget::PhotoVideoWidget(const RunMode &mode, QWidget *parent)
    : CustomWidget(parent)
{
    setObjectName("PhotoVideoWidget");
    m_runMode = mode;
    m_bIsAlbum = true;

    m_pWidgetType = m_runMode == Photo ? E_Widget_Photo : E_Widget_Video;

    initUI();
    initConn();

    setUIModel();
}

PhotoVideoWidget::~PhotoVideoWidget()
{
    g_enterDirMutex.unlock(); //程序退出析构时，解锁防止异常情况下锁住不能退出问题

    if (m_tRead) {
        m_tRead->quitImmediately();
        m_tRead->wait();
    }

    //    if (d_ptr->m_tPhotoVideoImport) {
    //        d_ptr->m_tPhotoVideoImport->quitImmediately();
    //        d_ptr->m_tPhotoVideoImport->wait();
    //        d_ptr->m_tPhotoVideoImport->deleteLater();
    //}
    //    if (d_ptr->m_tPhotoVideoExport) {
    //        d_ptr->m_tPhotoVideoExport->quitImmediately();
    //        d_ptr->m_tPhotoVideoExport->wait();
    //        d_ptr->m_tPhotoVideoExport->deleteLater();
    //}
}

void PhotoVideoWidget::updateWidgetInfo(const PhoneInfo &info)
{
    qDebug() << __LINE__ << __FUNCTION__;
    if (info == m_phoneInfo) {
        // 正在读取
        if (m_tRead != nullptr && m_tRead->isRunning()) {
            refreshUI();
            return;
        }
        // 已经读取到数据 不做刷新
        if (!isEmpty()) {
            refreshUI();
            return;
        }
    }
    qDebug() << __LINE__ << __FUNCTION__;
    m_strLastPath.clear();
    m_phoneInfo = info;
    m_bIsAlbum = true;
    refreshData();
    qDebug() << __LINE__ << __FUNCTION__;
}

//void PhotoVideoWidget::initCache(const PhoneInfo &info)
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

void PhotoVideoWidget::setWidgetBtnState()
{
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Left_Btn, true);
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Right_Btn, true);
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_New_Folder_Btn, false);
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Export_Btn, true);
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Delete_Btn, true);

    int nIndex = m_pStackViewLayout->currentIndex();

    slotStackedCurrentChanged(nIndex);
}

void PhotoVideoWidget::clearWidgetInfo(const QString &devInfo)
{
    clearCache(devInfo);
    if (devInfo == m_phoneInfo.strPhoneID) {
        m_phoneInfo.strPhoneID.clear();
        //        clear(); // 会导致卡死
        //        if (isReading()) {
        //            m_tRead->quitImmediately();
        //        }
    }
}

bool PhotoVideoWidget::isEmpty()
{
    return getListFrame()->isEmpty();
}

void PhotoVideoWidget::refreshWidgetData()
{
    //清空缓存
    if (m_bIsAlbum) {
        clearCache();
    }
    refreshData(m_strLastPath);
}

void PhotoVideoWidget::slotTitleWidgetBtnClicked(const int &nId)
{
    if (this->isVisible() == false)
        return;

    if (nId == TitleWidget::E_Left_Btn) {
        if (m_bIsAlbum)
            return;
        m_bIsAlbum = true;
        refreshData();
    } else if (nId == TitleWidget::E_Right_Btn) {
        if (!m_bIsAlbum)
            return;
        if (m_strLastPath.isEmpty())
            return;
        m_bIsAlbum = false;
        refreshData(m_strLastPath);
    } else if (nId == RightTitleWidget::E_Icon_View_Btn) {
        onGridBtnClicked();
    } else if (nId == RightTitleWidget::E_List_View_Btn) {
        onListBtnClicked();
    } else if (nId == TitleWidget::E_Delete_Btn) {
        onDeleteBtnClicked();
    } else if (nId == TitleWidget::E_Import_Btn) {
        onImportBtnClicked();
    } else if (nId == TitleWidget::E_Export_Btn) {
        onExportBtnClicked();
    }
}

bool PhotoVideoWidget::event(QEvent *e)
{
    if (e->type() == QEvent::MouseMove) {
        return true;
    }
    return CustomWidget::event(e);
}

bool PhotoVideoWidget::isReading()
{
    if (m_tRead && m_tRead->isRunning()) {
        return true;
    }

    return false;
}

bool PhotoVideoWidget::pauseRead()
{
    return m_tRead->lock();
}

bool PhotoVideoWidget::continueRead()
{
    return m_tRead->unlock();
}

void PhotoVideoWidget::initUI()
{
    // 网格
    m_pListViewFrame = new PhotoListFrame(m_pWidgetType, this);

    // 列表
    m_pTreeViewFrame = new PhotoTreeFrame(m_pWidgetType, this);
    auto tempWidget = m_pTreeViewFrame->getParent(); //getTreeWidget(this); // 排版m_pTreeViewFrame

    // 增加布局
    // top
    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->setContentsMargins(15, 10, 30, 0);
    // title
    lbl_title = new DLabel(this);
    //    lbl_title->setAutoFillBackground(true);
    //    DPalette palette = DApplicationHelper::instance()->palette(lbl_title);
    //    palette.setColor(QPalette::WindowText, palette.brightText().color());
    //    DApplicationHelper::instance()->setPalette(lbl_title, palette);
    DFontSizeManager::instance()->bind(lbl_title, DFontSizeManager::T3, QFont::DemiBold);

    topLayout->addWidget(lbl_title);
    topLayout->addStretch();
    m_btnChoose = new DCommandLinkButton(TrObject::getInstance()->getDlgBtnText(Select_All));
    topLayout->addWidget(m_btnChoose);

    //stacked
    m_pStackViewLayout = new QStackedLayout();
    m_pStackViewLayout->setMargin(0);
    m_pStackViewLayout->setSpacing(0);
    m_pStackViewLayout->insertWidget(VIEW_ICON_MODE, m_pListViewFrame);
    m_pStackViewLayout->insertWidget(VIEW_LIST_MODE, tempWidget);
    m_pStackViewLayout->setCurrentIndex(VIEW_ICON_MODE);

    m_pMmainLayout = new QVBoxLayout(this);
    m_pMmainLayout->setMargin(0);
    m_pMmainLayout->setSpacing(0);
    m_pMmainLayout->addLayout(topLayout);
    m_pMmainLayout->addLayout(m_pStackViewLayout);
    m_pMmainLayout->addWidget(m_pSpinnerWidget, 0, Qt::AlignBottom);

    d_ptr->m_pImportExportProgressDlg = new ImportExportProgressDlg("", this);
    d_ptr->m_pImportExportProgressDlg->hide();

    d_ptr->m_pImportProgress = new ImportProgressWgt(this);
    d_ptr->m_pImportProgress->hide();
}

void PhotoVideoWidget::initConn()
{
    connect(getListFrame(), &BaseListView::sigLoadDirData, this, &PhotoVideoWidget::onEnterFolder);
    connect(getTreeFrame(), &BaseTreeView::sigSelectAll, this, &PhotoVideoWidget::onTreeWgtSelectAll);
    connect(getListFrame(), &PhotoListFrame::sigFileSelectChanged, this, &PhotoVideoWidget::onItemSelect);
    connect(getTreeFrame(), &PhotoTreeFrame::sigFileSelectChanged, this, &PhotoVideoWidget::onItemSelect);

    connect(m_pStackViewLayout, &QStackedLayout::currentChanged, this, &PhotoVideoWidget::slotStackedCurrentChanged);
    connect(m_btnChoose, &DCommandLinkButton::clicked, this, &PhotoVideoWidget::onChooseBtnClicked);
    connect(d_ptr->m_pImportExportProgressDlg, &ImportExportProgressDlg::sigCloseBtnClicked, this, &PhotoVideoWidget::onImportExportProgressClose);
    connect(d_ptr->m_pImportProgress, &ImportProgressWgt::sigCloseBtnClicked, this, &PhotoVideoWidget::onImportExportProgressClose);

    connect(qobject_cast<QGuiApplication *>(qApp), &QGuiApplication::fontChanged, this, &PhotoVideoWidget::onFontChanged);
}

PhotoListFrame *PhotoVideoWidget::getListFrame()
{
    return qobject_cast<PhotoListFrame *>(m_pListViewFrame);
}

PhotoTreeFrame *PhotoVideoWidget::getTreeFrame()
{
    return qobject_cast<PhotoTreeFrame *>(m_pTreeViewFrame);
}

void PhotoVideoWidget::refreshData(QString strPath)
{
    clear();
    spinnerStart();
    refreshUI();

    if (m_tRead != nullptr && m_tRead->isRunning()) {
        disconnect(m_tRead, &PhoneFileThread::sigFindPicture, this, &PhotoVideoWidget::onFindPicture);
        disconnect(m_tRead, &PhoneFileThread::error, this, &PhotoVideoWidget::onFileError);
        m_tRead->quitImmediately();
        m_tRead = nullptr;
    }

    m_tRead = new PhoneFileThread(this);
    connect(m_tRead, &PhoneFileThread::sigFindPicture, this, &PhotoVideoWidget::onFindPicture);
    connect(m_tRead, &PhoneFileThread::error, this, &PhotoVideoWidget::onFileError);
    connect(m_tRead, &PhoneFileThread::finished, this, &PhotoVideoWidget::onReadFinished);

    QSize sItem; // list item大小
    QSize sIcon; // item中icon大小
    int nSpace = 0; //间距
    switch (int(m_runMode)) {
    case Photo:
        if (m_bIsAlbum) {
            sItem = QSize(ITEM_SIZE_ALBUM, ITEM_SIZE_ALBUM + ITEM_SIZE_ALBUM_NAME);
            sIcon = QSize(ITEM_SIZE_ALBUM - 34, ITEM_SIZE_ALBUM - 34); //边框
            lbl_title->setText(TrObject::getInstance()->getListViewText(List_Photo));
            lbl_title->setProperty("formertext", TrObject::getInstance()->getListViewText(List_Photo));

            m_tRead->readPhoto(strPath, m_phoneInfo.strPhoneID, sIcon, m_phoneInfo.type, true);
            nSpace = 18;
        } else {
            sItem = QSize(ITEM_SIZE_PIC, ITEM_SIZE_PIC);
            sIcon = QSize(ITEM_SIZE_PIC - 16, ITEM_SIZE_PIC - 16); //边框

            QString tempTitle = strPath.section('/', -1);
            QFontMetrics fontWidth(lbl_title->font()); //得到每个字符的宽度
            QString elideNote = fontWidth.elidedText(tempTitle, Qt::ElideRight, 600);
            lbl_title->setText(elideNote); //显示省略好的字符串
            lbl_title->setProperty("formertext", tempTitle);

            m_tRead->readPhoto(strPath, m_phoneInfo.strPhoneID, sIcon, m_phoneInfo.type);
            nSpace = 2; //3
        }
        break;
    case Video:
        if (m_bIsAlbum) {
            sItem = QSize(ITEM_SIZE_ALBUM, ITEM_SIZE_ALBUM + ITEM_SIZE_ALBUM_NAME);
            sIcon = QSize(ITEM_SIZE_ALBUM - 34, ITEM_SIZE_ALBUM - 34); //边框
            lbl_title->setText(TrObject::getInstance()->getListViewText(List_Video));
            lbl_title->setProperty("formertext", TrObject::getInstance()->getListViewText(List_Video));

            m_tRead->readVideo(strPath, m_phoneInfo.strPhoneID, sIcon, m_phoneInfo.type, true);
            nSpace = 18;
        } else {
            sItem = QSize(ITEM_SIZE_VIDEO, ITEM_SIZE_VIDEO);
            sIcon = QSize(ITEM_SIZE_VIDEO - 16, ITEM_SIZE_VIDEO - 16); //边框

            QString tempTitle = strPath.section('/', -1);
            QFontMetrics fontWidth(lbl_title->font()); //得到每个字符的宽度
            QString elideNote = fontWidth.elidedText(tempTitle, Qt::ElideRight, 600);
            lbl_title->setText(elideNote); //显示省略好的字符串
            lbl_title->setProperty("formertext", tempTitle);

            m_tRead->readVideo(strPath, m_phoneInfo.strPhoneID, sIcon, m_phoneInfo.type);
            nSpace = 8; //14
        }
        break;
    }
    getListFrame()->setSpacing(nSpace);
    getListFrame()->setContentsMargins(0, 0, 0, 0);

    getListFrame()->setItemSize(sItem);
    getListFrame()->setIconSize(sIcon);
    getTreeFrame()->resetHeaderState();
    m_tRead->start(QThread::HighestPriority);
}

void PhotoVideoWidget::restoreAll()
{
    m_bIsAlbum = true;
    m_strLastPath.clear();
    refreshData();
}

void PhotoVideoWidget::refreshUI()
{
    if (m_bIsAlbum) {
        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Left_Btn, false);

        RightTitleWidget::getInstance()->setWidgetBtnVisible(RightTitleWidget::E_Icon_View_Btn, false);
        RightTitleWidget::getInstance()->setWidgetBtnVisible(RightTitleWidget::E_List_View_Btn, false);

        TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Import_Btn, false);

        m_pStackViewLayout->setCurrentIndex(VIEW_ICON_MODE);
    } else {
        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Left_Btn, true);

        RightTitleWidget::getInstance()->setWidgetBtnVisible(RightTitleWidget::E_Icon_View_Btn, true);
        RightTitleWidget::getInstance()->setWidgetBtnVisible(RightTitleWidget::E_List_View_Btn, true);

        TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Import_Btn, true);
        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Import_Btn, true);
    }
    if (m_phoneInfo.type == Mount_Ios) { // ios 不让导入
        TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Import_Btn, false);
    }

    refreshTitleBtn();

    refreshPromptDlg();
}

void PhotoVideoWidget::refreshTitleBtn()
{
    if (!this->isVisible()) {
        return;
    }
    if (!m_strLastPath.isEmpty() && m_bIsAlbum) {
        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Right_Btn, true);
    } else {
        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Right_Btn, false);
    }

    bool bHasSelected = getListFrame()->selectionModel()->hasSelection();
    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Export_Btn, bHasSelected);
    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Delete_Btn, bHasSelected);

    if (bHasSelected && getListFrame()->isSelectAll()) { //全选
        m_btnChoose->setText(TrObject::getInstance()->getDlgBtnText(Unselect_All));
    } else {
        m_btnChoose->setText(TrObject::getInstance()->getDlgBtnText(Select_All));
    }

    calculateSelectCountSize();
}

void PhotoVideoWidget::calculateSelectCountSize()
{
    QString strText = "";

    int nRowCount = getListFrame()->getSourceModel()->rowCount();
    if (getListFrame()->selectionModel()->hasSelection()) {
        int nSelectCount = getListFrame()->selectionModel()->selectedRows(0).size();

        quint64 nFileSize = getListFrame()->getSelectSize();
        strText = TrObject::getInstance()
                      ->getFileOperationText(Total_item_selected_size)
                      .arg(nRowCount)
                      .arg(nSelectCount)
                      .arg(Utils::dataSizeToString(nFileSize));
    } else if (nRowCount > 0) {
        qint64 nFileSize = getListFrame()->getSourceModel()->getTotalSize();
        strText = TrObject::getInstance()
                      ->getFileOperationText(items_size)
                      .arg(nRowCount)
                      .arg(Utils::dataSizeToString(quint64(nFileSize)));

    } else {
        strText = TrObject::getInstance()->getFileOperationText(items_count).arg(0);
    }

    setShowText(strText);
}

void PhotoVideoWidget::refreshPromptDlg()
{
    d_ptr->m_pImportProgress->raise();
}

void PhotoVideoWidget::clear()
{
    m_pUIModelManager->clearModel();
    m_pTreeViewFrame->setHeaderViewState(false);
}

void PhotoVideoWidget::clearCache(const QString &strPhoneID)
{
    QString strPendingPhoneID;
    strPendingPhoneID = strPhoneID.isEmpty() ? m_phoneInfo.strPhoneID : strPhoneID;
    qDebug() << __LINE__ << __FUNCTION__ << strPendingPhoneID;
    switch (int(m_runMode)) {
    case Photo:
        ImageVideoManager::clearCache(PhoneFileThread::ReadPhoto, strPendingPhoneID);
        break;
    case Video:
        ImageVideoManager::clearCache(PhoneFileThread::ReadVideo, strPendingPhoneID);
        break;
    }
}

void PhotoVideoWidget::loadImage(const QStringList &listFilePath)
{
    Q_UNUSED(listFilePath)
}

void PhotoVideoWidget::delItem(QString strPath)
{
    getListFrame()->deleteModelByPath("", strPath);
    getTreeFrame()->delItem(strPath);

    if (m_bIsAlbum) {
        // 删除为返回文件路径
        if (m_strLastPath == strPath) {
            m_strLastPath.clear();
        }
    } else if (isEmpty() && !m_bIsAlbum) { //全部删完
        restoreAll();
    }
}

void PhotoVideoWidget::selectAll(bool bSelected, bool bOnlyList)
{
    if (isEmpty())
        return;

    if (bOnlyList) {
        getListFrame()->selectAllEx(bSelected);
    } else {
        getListFrame()->selectAllEx(bSelected);
        getTreeFrame()->selectAllEx(bSelected);
    }

    //    refreshTitleBtn(); //会刷新select按钮
    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Export_Btn, bSelected);
    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Delete_Btn, bSelected);
    if (bSelected) { //全选
        m_btnChoose->setText(TrObject::getInstance()->getDlgBtnText(Unselect_All));
    } else {
        m_btnChoose->setText(TrObject::getInstance()->getDlgBtnText(Select_All));
    }

    calculateSelectCountSize();
}

QString PhotoVideoWidget::getCurFilters()
{
    QString strRet;
    QStringList listFilters;

    switch (int(m_runMode)) {
    case Photo:
        strRet = "Images";
        listFilters = FileUtils::getImageFilters();
        break;
    case Video:
        strRet = "Videos";
        listFilters = FileUtils::getVideoFilters();
        break;
    }

    strRet += "(";
    foreach (QString item, listFilters) {
        strRet += item;
        strRet += " ";
    }
    strRet += ")";
    return strRet;
}

void PhotoVideoWidget::checkFileRepeat(QStringList &listSrcFile, QString strDesPath)
{
    if (listSrcFile.isEmpty())
        return;

    QStringList list;
    //导入
    if (strDesPath == m_strLastPath) {
        //从内存加载
        foreach (PhoneFileInfo itemInfo, getListFrame()->getAllFile()) {
            //防止过滤到目录子级
            if (m_strLastPath == itemInfo.path.section("/", 0, -2))
                list.append(itemInfo.path);
        }
    } else { //导出
        QDir dir;
        dir.setPath(strDesPath);
        list = dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
    }

    bool bShowDlg = true;
    int ret = 0;
    int procCount = 0; //要替换或共存的个数
    for (int i = 0; i < list.size(); ++i) {
        //        QFileInfo fileInfo = list.at(i);
        //        QString strFileName = fileInfo.fileName();
        QString strFileName = list.at(i).section("/", -1);
        QStringList listSrcPath;
        if (fileListContains(listSrcFile, strFileName, listSrcPath)) {
            foreach (QString strSrcPath, listSrcPath) {
                // 有重复 开始判断
                if (bShowDlg) {
                    FileRepeatConfirmDlg confirmDlg(strFileName, strSrcPath, strDesPath, true, this, false);
                    //最后一个文件时，不显示应用到全部
                    if (listSrcFile.count() == 1 || listSrcFile.count() - procCount == 1)
                        confirmDlg.hideCheckBox();
                    ret = confirmDlg.exec();
                    bShowDlg = !confirmDlg.getAskCheckBoxValue();
                }
                if (ret == -1) { // 关闭
                    bShowDlg = false;
                    ret = 0;
                    listSrcFile.removeOne(strSrcPath);
                } else if (ret == 0) { //跳过
                    listSrcFile.removeOne(strSrcPath);
                } else if (ret == 1) { //替换
                    procCount++;
                    //                if (bRecount) // onProgressResult 会增加 所以先去掉
                    //                    --m_nCount;
                } else if (ret == 2) { //共存
                    procCount++;
                    //                if (bRecount)
                    //                    ++m_nCount;
                }
            }
        }
    }
}

bool PhotoVideoWidget::fileListContains(const QStringList &listSrcFile, const QString &strFile, QStringList &listSrcAbsolutePath)
{
    foreach (QString strItemFile, listSrcFile) {
        if (strItemFile.endsWith("/" + strFile)) {
            listSrcAbsolutePath.append(strItemFile);
        }
    }
    if (listSrcAbsolutePath.isEmpty())
        return false;
    return true;
}

bool PhotoVideoWidget::contains(PhoneFileInfo &info)
{
    foreach (PhoneFileInfo item, getListFrame()->getAllFile()) {
        if (m_bIsAlbum) {
            if (item.dirPath == info.dirPath) {
                return true;
            }
        } else {
            if (item == info) {
                return true;
            }
        }
    }
    return false;
}

void PhotoVideoWidget::onDeleteBtnClicked()
{
    if (!d_ptr->m_pImportProgress->isHidden()) {
        sendWarnMessage(TrObject::getInstance()->getFileOperationText(Processing_data));
        return;
    }
    d_ptr->m_pImportProgress->setProperty("type", "delete");
    d_ptr->m_pImportProgress->setProperty("strPhoneID", m_phoneInfo.strPhoneID);
    d_ptr->m_pImportProgress->setProperty("strDevName", m_phoneInfo.strDevName);

    m_nDeletePendingData.clear();
    // delete 固定从网格视图获取
    m_nDeletePendingData.append(getListFrame()->getSelectFilePath());
    qDebug() << __LINE__ << __FUNCTION__ << m_nDeletePendingData;

    if (m_nDeletePendingData.isEmpty()) {
        sendWarnMessage(TrObject::getInstance()->getFileOperationText(Please_select_a_file));
        return;
    }
    // confirm dialog
    QString strTitle;
    if (m_nDeletePendingData.count() == 1) {
        strTitle = TrObject::getInstance()->getFileOperationText(Delete_it);
    } else {
        strTitle = TrObject::getInstance()->getFileOperationText(Delete_items);
    }
    bool bRet = execMsgBox(strTitle);
    if (!bRet) { //选择了取消
        m_nDeletePendingData.clear();
        return;
    }

    // delete
    m_pDeleteThread = new DeletePhoneFileThread(this);
    connect(m_pDeleteThread, &PMThread::finished, &PMThread::deleteLater);
    connect(m_pDeleteThread, &PMThread::error, this, &PhotoVideoWidget::slotDeleteFileError);
    connect(static_cast<DeletePhoneFileThread *>(m_pDeleteThread), &DeletePhoneFileThread::sigDeleteProgress, this, &PhotoVideoWidget::slotDeleteProgress);
    connect(static_cast<DeletePhoneFileThread *>(m_pDeleteThread), &DeletePhoneFileThread::sigDeleteResult, this, &PhotoVideoWidget::slotDeleteProgressResult);

    PhoneFileType type;
    if (m_runMode == Photo) {
        type = IMAGE;
    } else {
        type = VIDEO;
    }
    static_cast<DeletePhoneFileThread *>(m_pDeleteThread)->delFile(m_nDeletePendingData, type);
    m_pDeleteThread->start();
}

//  copy 操作
void PhotoVideoWidget::onCopyFile(QStringList &listPendingData, const QString &strResPath, const int &copyType)
{
    checkFileRepeat(listPendingData, strResPath);

    if (listPendingData.size() > 0) {
        // copy
        m_tPhotoVideoCopy = new CopyFileThread(this);
        connect(m_tPhotoVideoCopy, &PMThread::finished, &PMThread::deleteLater);
        connect(m_tPhotoVideoCopy, &PMThread::error, this, &PhotoVideoWidget::slotCopyFileError);
        connect(static_cast<CopyFileThread *>(m_tPhotoVideoCopy), &CopyFileThread::sigCopyProgress, this, &PhotoVideoWidget::slotCopyProgress);
        connect(static_cast<CopyFileThread *>(m_tPhotoVideoCopy), &CopyFileThread::sigCopyResult, this, &PhotoVideoWidget::slotCopyProgressResult);
        PhoneFileType type;
        if (m_runMode == Photo) {
            type = IMAGE;
        } else {
            type = VIDEO;
        }
        QString strRootPath;
        if (m_phoneInfo.type == Mount_Android)
            strRootPath = Utils::getAndroidMountRootPath(m_phoneInfo.strPhoneID);
        static_cast<CopyFileThread *>(m_tPhotoVideoCopy)->copyFile(listPendingData, strResPath, copyType, type, m_phoneInfo.strProductVersion, strRootPath, m_phoneInfo.strPhoneID);
        m_tPhotoVideoCopy->start();
    }
}

void PhotoVideoWidget::onExportBtnClicked()
{
    d_ptr->m_pImportExportProgressDlg->setTitle(TrObject::getInstance()->getDlgTitle(dlg_Exporting));
    d_ptr->m_pImportExportProgressDlg->setProperty("type", "export");

    if (!SpinnerIsVisible()) {
        sendWarnMessage(TrObject::getInstance()->getFileOperationText(Processing_data));
        return;
    }

    //目前 m_exportWgt是模态 不会遇到这个问题
    if (!d_ptr->m_pImportExportProgressDlg->isHidden()) {
        sendWarnMessage(TrObject::getInstance()->getFileOperationText(Processing_data));
        return;
    }
    // export 是否增加取消选择 固定从
    QStringList listPendingData = getListFrame()->getSelectFilePath();
    if (listPendingData.isEmpty()) {
        sendWarnMessage(TrObject::getInstance()->getFileOperationText(Please_select_a_file));
        return;
    }

    QString strResPath = getExportPath();
    if (strResPath.isEmpty()) {
        return;
    }

    onCopyFile(listPendingData, strResPath, CopyFileThread::ExportFile);
}

void PhotoVideoWidget::onImportBtnClicked()
{
    d_ptr->m_pImportExportProgressDlg->setTitle(TrObject::getInstance()->getDlgTitle(dlg_Importing));
    d_ptr->m_pImportExportProgressDlg->setProperty("type", "import");

    if (!SpinnerIsVisible()) {
        sendWarnMessage(TrObject::getInstance()->getFileOperationText(Processing_data));
        return;
    }
    // import
    QStringList listPendingData = DFileDialog::getOpenFileNames(
        this, "", DStandardPaths::writableLocation(QStandardPaths::DesktopLocation), getCurFilters());
    if (listPendingData.isEmpty())
        return;

    onCopyFile(listPendingData, m_strLastPath, CopyFileThread::ImportFile);
}

void PhotoVideoWidget::onChooseBtnClicked()
{
    bool bIsAllChecked = getListFrame()->isSelectAll();
    this->selectAll(!bIsAllChecked);
}

void PhotoVideoWidget::onGridBtnClicked()
{
    m_currFileViewMode = VIEW_ICON_MODE;
    m_pStackViewLayout->setCurrentIndex(m_currFileViewMode);
}

void PhotoVideoWidget::onListBtnClicked()
{
    m_currFileViewMode = VIEW_LIST_MODE;
    m_pStackViewLayout->setCurrentIndex(m_currFileViewMode);
}

void PhotoVideoWidget::slotStackedCurrentChanged(int nIndex)
{
    if (nIndex == 0) { //  网格
        RightTitleWidget::getInstance()->setWidgetBtnChecked(RightTitleWidget::E_Icon_View_Btn, true);

        m_btnChoose->show();
    } else { // 列表
        RightTitleWidget::getInstance()->setWidgetBtnChecked(RightTitleWidget::E_List_View_Btn, true);

        m_btnChoose->hide();
    }

    refreshPromptDlg();
    //        refreshTitleBtn();
}

void PhotoVideoWidget::onFileError(int nErrorCode, QString strErrorMsg)
{
    if (nErrorCode == PFERROR_NOTFIND_DCIM) {
        spinnerStop();
    } else if (nErrorCode == PEERROR_NO_FILE) {
        spinnerStop();
        //        calcTotal();
        calculateSelectCountSize();
        return;
    } else if (nErrorCode == PEERROR_END) {
        spinnerStop();
        //        calcTotal();
        calculateSelectCountSize();
        //        loadImage(sender()->property("filelist").toStringList());

        return;
    }
    sendWarnMessage(strErrorMsg);
}

void PhotoVideoWidget::slotCopyFileError(QString strErrorMsg)
{
    sendWarnMessage(strErrorMsg);
}

void PhotoVideoWidget::slotDeleteFileError(QString strErrorMsg)
{
    sendWarnMessage(strErrorMsg);
}

void PhotoVideoWidget::onFindPicture(PhoneFileInfo &info)
{
    if (m_bIsAlbum != info.bIsDir)
        return;

    // 验重 fix: 49079
    if (m_bIsAlbum && contains(info)) {
        return;
    }
    m_pUIModelManager->appendData(QVariant::fromValue(info));

    // UI
    m_btnChoose->setText(TrObject::getInstance()->getDlgBtnText(Select_All));
    m_pTreeViewFrame->setHeaderViewState(false);
    //    refreshStatusBar();//目前加载过程中不显示状态栏
}

void PhotoVideoWidget::onImportExportProgressClose()
{
    if (m_tPhotoVideoCopy == nullptr)
        return;

    m_tPhotoVideoCopy->setThreadStop();
}

//void PhotoVideoWidget::onImportProgressClose()
//{
//    if (m_tPhotoVideoCopy == nullptr)
//        return;

//    m_tPhotoVideoCopy->setThreadStop();
//    //    m_tPhotoVideoCopy->deleteLater();
//    //    m_tPhotoVideoCopy = nullptr;
//}

//void PhotoVideoWidget::onExportProgressClose()
//{
//    if (m_tPhotoVideoCopy == nullptr)
//        return;
//    m_tPhotoVideoCopy->setThreadStop();
//    //    m_tPhotoVideoCopy->deleteLater();
//    //    m_tPhotoVideoCopy = nullptr;
//}

void PhotoVideoWidget::onEnterFolder(const QString &strPath)
{
    RightTitleWidget::getInstance()->setWidgetBtnChecked(RightTitleWidget::E_Icon_View_Btn, true);
    RightTitleWidget::getInstance()->setWidgetBtnChecked(RightTitleWidget::E_List_View_Btn, false);

    m_bIsAlbum = false;
    m_strLastPath = strPath;
    refreshData(strPath);
}

void PhotoVideoWidget::slotCopyProgress(int nCur, int nTotal, QString strFileName)
{
    qDebug() << __FUNCTION__ << "   " << nCur << nTotal << strFileName;

    d_ptr->m_pImportExportProgressDlg->updateProgressBarValue(0);
    if (d_ptr->m_pImportExportProgressDlg->isHidden()) {
        d_ptr->m_pImportExportProgressDlg->exec();
    }
    d_ptr->m_pImportExportProgressDlg->setProgressBarRange(0, nTotal);
    d_ptr->m_pImportExportProgressDlg->updateProgressBarValue(nCur);
}

void PhotoVideoWidget::slotCopyProgressResult(int nRunMode, int nCount, int nSuccess, int nFailed, bool bIsStop)
{
    qDebug() << __LINE__ << m_runMode << nRunMode << nCount << nSuccess << bIsStop;
    Q_UNUSED(nCount)
    Q_UNUSED(nFailed)
    switch (nRunMode) {
    case CopyFileThread::ImportFile:
        d_ptr->m_pImportExportProgressDlg->hide();
        if (nSuccess > 0) { //防止没导入也刷新
            refreshWidgetData();
            if (bIsStop) {
                if (m_runMode == Photo) {
                    sendOkMessage(TrObject::getInstance()->getPhotoImportExportText(photo_import_stop).arg(nSuccess));
                } else if (m_runMode == Video) {
                    sendOkMessage(TrObject::getInstance()->getVideoImportExportText(video_import_stop).arg(nSuccess));
                }
            } else {
                if (m_runMode == Photo) {
                    sendOkMessage(
                        TrObject::getInstance()->getPhotoImportExportText(photo_import_success).arg(nSuccess));
                } else if (m_runMode == Video) {
                    sendOkMessage(
                        TrObject::getInstance()->getVideoImportExportText(video_import_success).arg(nSuccess));
                }
            }
        }
        break;
    case CopyFileThread::ExportFile:
        d_ptr->m_pImportExportProgressDlg->hide();
        if (nSuccess > 0) { //防止没导入也刷新
            if (bIsStop) {
                if (m_runMode == Photo) {
                    sendOkMessage(
                        TrObject::getInstance()->getPhotoImportExportText(photo_export_stop_count).arg(nSuccess));
                } else if (m_runMode == Video) {
                    sendOkMessage(
                        TrObject::getInstance()->getVideoImportExportText(video_export_stop_count).arg(nSuccess));
                }
            } else {
                if (m_runMode == Photo) {
                    sendOkMessage(
                        TrObject::getInstance()->getPhotoImportExportText(photo_export_success).arg(nSuccess));
                } else if (m_runMode == Video) {
                    sendOkMessage(
                        TrObject::getInstance()->getVideoImportExportText(video_export_success).arg(nSuccess));
                }
            }
        }
        break;
    }
    calculateSelectCountSize();
}

void PhotoVideoWidget::slotDeleteProgress(QString strFileName)
{
    if (d_ptr->m_pImportProgress->isHidden()) {
        d_ptr->m_pImportProgress->setCloseBtnHidden(true);
        d_ptr->m_pImportProgress->show();
        d_ptr->m_pImportProgress->raise();
    }
    d_ptr->m_pImportProgress->updateProgressText("[" + d_ptr->m_pImportProgress->property("strDevName").toString() + "]" + TrObject::getInstance()->getFileOperationText(Deleting) + "  " + strFileName);
}

void PhotoVideoWidget::slotDeleteProgressResult(int nFailed, bool bIsStop)
{
    d_ptr->m_pImportProgress->hide();
    // 清空缓存
    clearCache(d_ptr->m_pImportProgress->property("strPhoneID").toString());

    //可能会删除失败
    if (nFailed > 0 || bIsStop) {
        refreshData(m_strLastPath);
        m_nDeletePendingData.clear();
    }

    foreach (QString strItemPath, m_nDeletePendingData) {
        delItem(strItemPath);
    }
    m_nDeletePendingData.clear();

    //  结束之后 设置按钮的相应状态
    refreshTitleBtn();
}

void PhotoVideoWidget::onTreeWgtSelectAll(bool bSelected)
{
    this->selectAll(bSelected, true);
}

void PhotoVideoWidget::onItemSelect(QString sPath, bool bSelected)
{
    // 网格->列表
    if (sender() == getListFrame()) {
        // 列表模式下没有文件夹
        if (!m_bIsAlbum) {
            getTreeFrame()->setItemSelect(sPath, bSelected);
        }
    } else {
        // 列表->网格
        getListFrame()->setItemSelect(sPath, bSelected);
    }
    refreshTitleBtn(); //会刷新select按钮
}

void PhotoVideoWidget::onFontChanged()
{
    QString tempTitle = lbl_title->property("formertext").toString();
    QFontMetrics fontWidth(lbl_title->font()); //得到每个字符的宽度
    QString elideNote = fontWidth.elidedText(tempTitle, Qt::ElideRight, 600);
    lbl_title->setText(elideNote); //显示省略好的字符串
}

void PhotoVideoWidget::onReadFinished()
{
    QThread *sender_t = qobject_cast<QThread *>(sender());
    if (sender_t == nullptr) {
        return;
    }

    if (sender_t == m_tRead) {
        m_tRead = nullptr;
    }

    sender_t->deleteLater();
}
