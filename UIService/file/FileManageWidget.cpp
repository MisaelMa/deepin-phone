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
#include "FileManageWidget.h"

#include "widget/SpinnerWidget.h"
#include "utils.h"
#include "widget/progresswidget.h"

#include <DFileDialog>
#include <QClipboard>
#include <DStandardPaths>
#include <DFileIconProvider>
#include <DMenu>
#include <DApplicationHelper>
#include <DApplication>

#include <QDebug>
#include <QMimeData>
#include <QSortFilterProxyModel>
#include <QJsonObject>

#include "GlobalDefine.h"
#include "widget/TitleWidget.h"
#include "widget/RightTitleWidget.h"
#include "Model.h"
#include "DataService.h"

#include "TrObject.h"
#include "FileModel.h"

#include "base/SortFilterProxyModel.h"
#include "file/PhoneTreeFileModel.h"
#include "file/FileTreeView.h"
#include "file/FileListView.h"

#include "ThreadService.h"
#include "widget/CustomWidget_p.h"

#include "FileOperate/DeleteFileTask.h"
#include "ThreadService.h"
#include "FileOperate/PhoneFileTask.h"
#include "FileOperate/phonefilethread.h"

#include "filemanagethread.h"
#include "src/ImageEngineThread.h"
#include "src/VideoEngineThread.h"
#include "PhoneIconFileModel.h"
#include "PhoneTreeFileModel.h"

const int LIST_NAMECOL_WIDTH = 365;
const int LIST_DATECOL_WIDTH = 180;
const int LIST_TYPECOL_WIDTH = 80;

extern QWaitCondition g_fileProcWait;
extern QMutex g_enterDirMutex; //??????????????????????????????????????????

DCORE_USE_NAMESPACE

FileManageWidget::FileManageWidget(DWidget *parent)
    : CustomWidget(parent)
//    , m_bCtrlPress(false)
//    , m_bShiftPress(false)
{
    setObjectName("FileManageWidget");

    qRegisterMetaType<FILE_INFO>("FILE_INFO");

    m_currentPathIndex = 0;

    m_copyFileThrd = nullptr;
    m_timer = nullptr;
    m_imageFileThrd = nullptr;
    m_videoImgFileThrd = nullptr;
    m_processBarValue = 0;
    m_dirFileTimer = nullptr;
    m_loadTask = nullptr;
    m_dispFileThrd = nullptr;

    m_currFileViewMode = VIEW_ICON_MODE;
    m_pWidgetType = E_Widget_File;

    initUI();
    initConnection();

    setUIModel();
    //getFileIconAdvance(); //???????????????????????????????????? ?????????DataService???
}

FileManageWidget::~FileManageWidget()
{
    g_enterDirMutex.unlock(); //??????

    qDebug() << __FUNCTION__ << "           " << this;
    if (m_timer != nullptr) {
        m_timer->stop();
        delete m_timer;
        m_timer = nullptr;
    }

    //??????
    if (m_loadTask != nullptr) {
        m_loadTask->unlock();
        m_loadTask->setTaskStop();
    }

    //    //???????????????????????????
    //    if (m_imageFileThrd != nullptr) {
    //        m_imageFileThrd->setThreadExit();
    //        m_imageFileThrd->quit();
    //        m_imageFileThrd->wait();
    //    }

    //???????????????????????????
    //    if (m_videoImgFileThrd != nullptr)
    //        m_videoImgFileThrd->unlock();

    //????????????
    ThreadService::getService()->stopTask(ThreadService::E_File_Display_Task);
    //??????????????????
    ThreadService::getService()->stopTask(ThreadService::E_File_Delete_Task);

    //    ThreadService::getService()->stopTask(ThreadService::E_Image_Render_Task);
    //    ThreadService::getService()->stopTask(ThreadService::E_Video_Render_Task);

    if (m_imageFileThrd != nullptr) {
        delete m_imageFileThrd;
        m_imageFileThrd = nullptr;
    }
    if (m_videoImgFileThrd != nullptr) {
        delete m_videoImgFileThrd;
        m_videoImgFileThrd = nullptr;
    }

    if (m_copyFileThrd != nullptr) {
        m_copyFileThrd->setExit(); //??????????????????
        m_copyFileThrd->quit();
    }

    if (d_ptr->m_importExportFileThrd != nullptr) {
        d_ptr->m_importExportFileThrd->setExit();
        d_ptr->m_importExportFileThrd->quit();
    }

    if (m_dispFileThrd != nullptr) {
        if (m_dispFileThrd->isRunning()) {
            //?????????????????????????????????????????????????????????????????????????????????
            m_dispFileThrd->setExit();
            m_dispFileThrd->quit();
        }
        //????????????????????????
        m_dispFileThrd = nullptr;
    }
}

void FileManageWidget::initUI()
{
    m_pMmainLayout = new QVBoxLayout();

    m_pStackViewLayout = new QStackedLayout();
    m_pStackViewLayout->setMargin(0);
    m_pStackViewLayout->setSpacing(0);

    initRootIconViewUI();
    initRootListViewUI();
    initEmptyWidget();

    m_pMmainLayout->addLayout(m_pStackViewLayout);

    //?????????????????????
    m_pMmainLayout->addWidget(m_pSpinnerWidget, 0, Qt::AlignBottom);

    m_pMmainLayout->setSpacing(0);
    m_pMmainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(m_pMmainLayout);

    m_pHistoryPathList.clear();
}

void FileManageWidget::initRootIconViewUI()
{
    //?????????????????????????????????
    auto iconWidget = new DWidget(this);

    QHBoxLayout *hLayout = new QHBoxLayout(iconWidget);
    hLayout->setContentsMargins(14, 10, 0, 0);

    m_pListViewFrame = new FileListView(iconWidget);
    hLayout->addWidget(m_pListViewFrame);

    m_pStackViewLayout->insertWidget(VIEW_ICON_MODE, iconWidget);

    connect(m_pListViewFrame, &BaseListView::sigLoadDirData, this, &FileManageWidget::getDirFile);
    //    connect(listView, &BaseListView::doubleClicked, this, &FileManageWidget::slotDListViewItemDoubleClick);
    connect(m_pListViewFrame, &BaseListView::clicked, this, &FileManageWidget::slotDListViewItemClicked);
    connect(m_pListViewFrame->selectionModel(), &QItemSelectionModel::selectionChanged, this, &FileManageWidget::slotSelectionChanged);

    connect(static_cast<FileListView *>(m_pListViewFrame), &FileListView::sigCleanSelectItem, this, &FileManageWidget::slotCleanSelectItem);
    connect(static_cast<FileListView *>(m_pListViewFrame), &FileListView::sigFileNewFolder, this, &FileManageWidget::slotFileItemEditFinish);
    connect(static_cast<FileListView *>(m_pListViewFrame), &FileListView::sigItemRefresh, this, &FileManageWidget::slotItemRefresh);
}

void FileManageWidget::initRootListViewUI()
{
    m_pTreeViewFrame = new FileTreeView(this);

    auto tempWidget = m_pTreeViewFrame->getParent(); //getTreeWidget(this);
    m_pStackViewLayout->insertWidget(VIEW_LIST_MODE, tempWidget);

    //    connect(treeView, &BaseTreeView::sigSelectAll, this, &FileManageWidget::slotUpdateSelectData);
    connect(m_pTreeViewFrame, &BaseTreeView::sigLoadDirData, this, &FileManageWidget::getDirFile);
    connect(m_pTreeViewFrame, &BaseTreeView::sigUpdateSelectData, this, &FileManageWidget::slotUpdateSelectData);
    connect(static_cast<FileTreeView *>(m_pTreeViewFrame), &FileTreeView::sigFileNewFolder, this, &FileManageWidget::slotFileItemEditFinish);

    m_pTreeViewFrame->setSelectionMode(QAbstractItemView::SingleSelection);
    m_pTreeViewFrame->sortByColumn(LIST_NAMECOL_WIDTH, Qt::AscendingOrder);

    m_pTreeViewFrame->setColumnWidth(FILE_NAME_COLUMN, LIST_NAMECOL_WIDTH);
    m_pTreeViewFrame->setColumnWidth(FILE_DATE_COLUMN, LIST_DATECOL_WIDTH);
    m_pTreeViewFrame->setColumnWidth(FILE_TYPE_COLUMN, LIST_TYPECOL_WIDTH);
}

void FileManageWidget::initEmptyWidget()
{
    auto widget = new DWidget(this);

    auto layout = new QVBoxLayout(widget);

    layout->addStretch();

    //?????????????????????
    auto m_emptyDirInfo = new DLabel(TrObject::getInstance()->getLabelText(No_files), widget);
    m_emptyDirInfo->setAlignment(Qt::AlignCenter);
    DFontSizeManager::instance()->bind(m_emptyDirInfo, DFontSizeManager::T6);
    DPalette pa = DApplicationHelper::instance()->palette(m_emptyDirInfo);
    pa.setBrush(DPalette::WindowText, pa.color(DPalette::PlaceholderText));
    DApplicationHelper::instance()->setPalette(m_emptyDirInfo, pa);

    layout->addWidget(m_emptyDirInfo, 0, Qt::AlignCenter);
    layout->addStretch();

    m_pStackViewLayout->insertWidget(VIEW_EMPTY, widget);
}

void FileManageWidget::initConnection()
{
    connect(this, &FileManageWidget::sigNameEdit, this, &FileManageWidget::slotNameEdit, Qt::QueuedConnection);
}

void FileManageWidget::updateWidgetInfo(const PhoneInfo &info)
{
    qDebug() << __LINE__ << __FUNCTION__ << "File Data: " << info.strPhoneID;

    m_devInfo = info;

    navigationBtnEnabled();
    setFileViewBtnIcon();
    setTitleBtnEnble();
    setNewDelBtnEnable();

    //????????????????????????????????????????????????????????? //modify by zll pms 37140
    //????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
    if (m_devInfo.strPhoneID == m_devId)
        return;

    m_devId = m_devInfo.strPhoneID;

    m_pHistoryPathList.clear();
    m_currentPathIndex = 0;

    dispRootPathData();
}

void FileManageWidget::dispRootPathData()
{
    qDebug() << __FUNCTION__;
    QString path = ""; //?????????Path??????"",??????????????????????????????
    m_rootPath = path;
    m_currPath = path;
    m_currType = E_Widget_File;

    getFileListFromDevice(E_Widget_File, path);

    m_pHistoryPathList.append(path);
    m_currentPathIndex = m_pHistoryPathList.size() - 1;

    navigationBtnEnabled();

    m_pStackViewLayout->setCurrentIndex(m_currFileViewMode);

    setFileViewBtnIcon();
    setNewDelBtnEnable();
}

void FileManageWidget::getFileListFromDevice(int type, QString path)
{
    ThreadService::getService()->stopTask(ThreadService::E_File_Display_Task);

    //    ThreadService::getService()->stopTask(ThreadService::E_Image_Render_Task);
    //    ThreadService::getService()->stopTask(ThreadService::E_Video_Render_Task);

    if (m_imageFileThrd != nullptr) {
        disconnect(m_imageFileThrd, &BaseEngineThread::sigImageBackLoaded, this, &FileManageWidget::slotUpdateImage);
        delete m_imageFileThrd;
        m_imageFileThrd = nullptr;
    }
    if (m_videoImgFileThrd != nullptr) {
        disconnect(m_videoImgFileThrd, &BaseEngineThread::sigImageBackLoaded, this, &FileManageWidget::slotUpdateImage);
        delete m_videoImgFileThrd;
        m_videoImgFileThrd = nullptr;
    }

    m_pUIModelManager->clearModel();
    m_pTreeViewFrame->setHeaderViewState(false);

    if (!path.isEmpty()) {
        QFile filePath(path);
        if (!filePath.exists()) {
            sendWarnMessage(TrObject::getInstance()->getLabelText(path_not_exist));
            calculateSelectCountSize();
            //emit sigFindCount(m_devId, m_count);
            return;
        }
    }

    m_pImageList.clear();
    m_pVideoList.clear();

    d_ptr->m_optType = FILE_DISPLAY;
    d_ptr->m_bIsLoading = true;

    //    if (m_loadTask == nullptr) {
    //        m_loadTask = new PhoneFileTask(qApp);

    //        connect(m_loadTask, &PhoneFileTask::sigFileInfo, this, &FileManageWidget::slotDispFileInfo);
    //        connect(m_loadTask, &PhoneFileTask::sigTaskFinished, this, &FileManageWidget::slotFileThreadFinish);
    //        connect(m_loadTask, &PhoneFileTask::sigRootPath, this, &FileManageWidget::slotRootPath);
    //        connect(m_loadTask, &PhoneFileTask::sigRootPathNotFound, this, &FileManageWidget::slotRootPathNotFound);
    //    }

    //    m_loadTask->setPathAndType(path, m_devInfo.type, m_devInfo.strPhoneID);
    //    ThreadService::getService()->startTask(ThreadService::E_File_Display_Task, m_loadTask);

    //PhoneFileTask ???????????????????????????????????????????????????????????????????????????????????????
    //PhoneFileTask ??????????????????????????????????????????????????????????????????????????????QThread???????????????????????????
    if (m_dispFileThrd != nullptr) {
        if (m_dispFileThrd->isRunning()) {
            //?????????????????????????????????????????????????????????????????????????????????
            g_enterDirMutex.unlock(); //??????
            m_dispFileThrd->setExit();
            m_dispFileThrd->quit();
        }
        //????????????????????????
        m_dispFileThrd = nullptr;
    }

    //????????????????????????
    m_dispFileThrd = new FileManageThread(m_devInfo);
    m_dispFileThrd->setPath(m_rootPath, path, FILE_DISPLAY, FT_ALL);
    connect(m_dispFileThrd, &FileManageThread::sigFileInfo, this, &FileManageWidget::slotDispFileInfo);
    connect(m_dispFileThrd, &FileManageThread::finished, this, &FileManageWidget::slotFileThreadFinish);
    connect(m_dispFileThrd, &FileManageThread::sigRootPath, this, &FileManageWidget::slotRootPath);
    connect(m_dispFileThrd, &FileManageThread::sigRootPathNotFound, this, &FileManageWidget::slotRootPathNotFound);
    connect(m_dispFileThrd, &FileManageThread::finished, m_dispFileThrd, &FileManageThread::deleteLater);

    m_dispFileThrd->start();

    m_currPath = path;
    m_currType = type;

    startSpinner();
}

//??????FILE_INFO ????????????????????????
void FileManageWidget::slotDispFileInfo(FILE_INFO file_info)
{
    //?????????????????????????????????????????????????????????????????????
    QString parentPath;
    if (m_currPath.isEmpty()) {
        //????????????????????????
        if (m_rootPath.endsWith(QDir::separator()))
            parentPath = file_info.fileInfo.absolutePath() + QDir::separator();
        else
            parentPath = file_info.fileInfo.absolutePath();

        if (parentPath != m_rootPath)
            return;
    } else {
        if (m_currPath.endsWith(QDir::separator()))
            parentPath = file_info.fileInfo.absolutePath() + QDir::separator();
        else
            parentPath = file_info.fileInfo.absolutePath();

        if (parentPath != m_currPath)
            return;
    }

    PhoneIconFileModel *iconModel = dynamic_cast<PhoneIconFileModel *>(m_pListViewFrame->getSourceModel());
    if (iconModel != nullptr)
        iconModel->appendData_FI(file_info);

    PhoneTreeFileModel *treeModel = dynamic_cast<PhoneTreeFileModel *>(m_pTreeViewFrame->getSourceModel());
    if (treeModel != nullptr)
        treeModel->appendData_FI(file_info);
}

//void FileManageWidget::slotDispFileInfo(const QFileInfo &fi)
//{
//    //?????????????????????????????????????????????????????????????????????
//    QString parentPath;
//    if (m_currPath.isEmpty()) {
//        //????????????????????????
//        if (m_rootPath.endsWith(QDir::separator()))
//            parentPath = fi.absolutePath() + QDir::separator();
//        else
//            parentPath = fi.absolutePath();

//        if (parentPath != m_rootPath)
//            return;
//    } else {
//        if (m_currPath.endsWith(QDir::separator()))
//            parentPath = fi.absolutePath() + QDir::separator();
//        else
//            parentPath = fi.absolutePath();

//        if (parentPath != m_currPath)
//            return;
//    }

//    if (photoSuffix.contains(fi.suffix(), Qt::CaseInsensitive)) {
//        m_pImageList.append(fi.absoluteFilePath());
//    } else if (videoSuffix.contains(fi.suffix(), Qt::CaseInsensitive)) {
//        m_pVideoList.append(fi.absoluteFilePath());
//    }

//    QVariant var;
//    var.setValue(fi);

//    m_pListViewFrame->getSourceModel()->appendData(var);
//    m_pTreeViewFrame->getSourceModel()->appendData(var);
//}

void FileManageWidget::startGetItemImageTask()
{
    //    {
    //        QStringList tempList = m_pImageList;
    //        if (tempList.size() > 0) {
    //            auto task = new ImageEngineTask(qApp);
    //            task->setData(tempList);
    //            connect(task, &ImageEngineTask::sigImageBackLoaded, this, [=](const QString &path, const QPixmap &data) {
    //                QJsonObject obj;
    //                obj.insert("path", path);

    //                if (data.isNull())
    //                    return;
    //                m_pListViewFrame->getSourceModel()->updateDataPixmap(obj, data);
    //                m_pTreeViewFrame->getSourceModel()->updateDataPixmap(obj, data);
    //            });

    //            ThreadService::getService()->startTask(ThreadService::E_Image_Render_Task, task);
    //        }
    //        m_pImageList.clear();
    //    }

    //    {
    //        auto tempList = m_pVideoList;
    //        if (tempList.size() > 0) {
    //            auto task = new VideoEngineTask(qApp);
    //            task->setData(tempList);
    //            connect(task, &VideoEngineTask::sigVideoBackLoaded, this, [=](const QString &path, const int &nTime, const QPixmap &data) {
    //                Q_UNUSED(nTime);

    //                QJsonObject obj;
    //                obj.insert("path", path);

    //                m_pListViewFrame->getSourceModel()->updateDataPixmap(obj, data);
    //                m_pTreeViewFrame->getSourceModel()->updateDataPixmap(obj, data);
    //            });

    //            ThreadService::getService()->startTask(ThreadService::E_Video_Render_Task, task);
    //        }

    //        m_pVideoList.clear();
    //    }

    qDebug() << __FUNCTION__ << "start.";
    if (m_pImageList.size() > 0) {
        m_imageFileThrd = new ImageEngineThread();
        connect(m_imageFileThrd, &ImageEngineThread::sigImageBackLoaded, this, &FileManageWidget::slotUpdateImage);
        connect(m_imageFileThrd, &FileManageThread::finished, this, &FileManageWidget::slotGetImageThreadFinish);
        m_imageFileThrd->setImagePathList(m_pImageList);
        m_imageFileThrd->start();
    }

    if (m_pVideoList.size() > 0) {
        m_videoImgFileThrd = new VideoEngineThread();
        connect(m_videoImgFileThrd, &VideoEngineThread::sigImageBackLoaded, this, &FileManageWidget::slotUpdateImage);
        connect(m_videoImgFileThrd, &FileManageThread::finished, this, &FileManageWidget::slotGetImageThreadFinish);
        m_videoImgFileThrd->setImagePathList(m_pVideoList);
        m_videoImgFileThrd->start();
    }
}

void FileManageWidget::slotFileThreadFinish()
{
    qDebug() << __LINE__ << __FUNCTION__;

    //????????????????????????????????????????????????????????????????????????????????????????????????
    //????????????????????????????????????????????????????????????????????????????????????????????????????????????
    if (d_ptr->m_optType == FILE_DISPLAY) {
        if (m_dispFileThrd != nullptr && m_dispFileThrd->isRunning()) {
            return;
        }
    }

    stopSpinner();
    setTitleBtnEnble();

    if (d_ptr->m_optType == FILE_DISPLAY) {
        //  ????????????????????????
        //startGetItemImageTask();
        d_ptr->m_bIsLoading = false;
        if (m_dispFileThrd != nullptr) {
            //????????????????????????
            m_dispFileThrd = nullptr;
        }

        qDebug()
            << __LINE__ << __FUNCTION__ << "disp file end.";
    } else if (d_ptr->m_optType == FILE_IMPORT) {
        //        d_ptr->closeImportProgress();
        qDebug() << __LINE__ << __FUNCTION__ << "import end.";

        //        if (d_ptr->m_importOkCount > 0) {
        //            //  ????????????????????????
        //            startGetItemImageTask();

        //            if (!d_ptr->m_importCancel) {
        //                sendOkMessage(TrObject::getInstance()->getImportExportText(file_Import_success).arg(d_ptr->m_importOkCount));
        //            } else {
        //                sendOkMessage(TrObject::getInstance()->getImportExportText(file_Import_stopped).arg(d_ptr->m_importOkCount));
        //            }
        //        }
        if (d_ptr->m_OkCount > 0) {
            //  ????????????????????????
            startGetItemImageTask();
        }
        if (d_ptr->m_pImportExportProgressDlg != nullptr) {
            d_ptr->m_importExportCancel = false;
            d_ptr->m_pImportExportProgressDlg->setProgressBarFinish();
            d_ptr->m_pImportExportProgressDlg->hide();
            qDebug() << __LINE__ << __FUNCTION__;
        }
    } else if (d_ptr->m_optType == FILE_EXPORT) {
        qDebug() << __LINE__ << __FUNCTION__ << "export end.";
        //        if (d_ptr->m_pExportProgressDlg != nullptr) {
        //            d_ptr->m_exportCancel = false;
        //            d_ptr->m_pExportProgressDlg->hide();
        //            qDebug() << __LINE__ << __FUNCTION__;
        //        }
        if (d_ptr->m_pImportExportProgressDlg != nullptr) {
            d_ptr->m_importExportCancel = false;
            d_ptr->m_pImportExportProgressDlg->setProgressBarFinish();
            d_ptr->m_pImportExportProgressDlg->hide();
            qDebug() << __LINE__ << __FUNCTION__;
        }
    } else if (d_ptr->m_optType == FILE_PASTE) {
        //  ????????????????????????
        startGetItemImageTask();

        //        if (m_copyFileThrd != nullptr) {
        //            m_copyFileThrd->quit();
        //            m_copyFileThrd->wait();
        //            delete m_copyFileThrd;
        //            m_copyFileThrd = nullptr;
        //        }

        if (d_ptr->m_pImportExportProgressDlg != nullptr) {
            d_ptr->m_importExportCancel = false;
            d_ptr->m_pImportExportProgressDlg->setProgressBarFinish();
            d_ptr->m_pImportExportProgressDlg->hide();
            qDebug() << __LINE__ << __FUNCTION__;
        }

        //?????????????????????????????????
        if (!m_lastPasteFile.isEmpty()) {
            if (m_currFileViewMode == VIEW_ICON_MODE)
                m_pListViewFrame->setItemSelect(m_lastPasteFile, true);
            else if (m_currFileViewMode == VIEW_LIST_MODE)
                m_pTreeViewFrame->setItemSelect(m_lastPasteFile, true);
        }

        qDebug() << __LINE__ << __FUNCTION__ << "copy file end.";
    }

    //d_ptr->m_optType = File_Undefine;

    if (m_currFileViewMode == VIEW_EMPTY) {
        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Export_Btn, false);
        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Delete_Btn, false);
    }
    setNewDelBtnEnable();

    //    if (m_count < 0)
    //        m_count = 0;

    //emit sigFindCount(m_devId, m_count);

    calculateSelectCountSize();
    updateSelectFileInfo();

    //??????????????????
    QItemSelection itemSel_dumy;
    slotSelectionChanged(itemSel_dumy, itemSel_dumy);

    if (m_pListViewFrame->getSourceModel()->rowCount() == 0) {
        m_pStackViewLayout->setCurrentIndex(VIEW_EMPTY);
    } else {
        //????????????
        m_pStackViewLayout->setCurrentIndex(m_currFileViewMode);
    }
}

void FileManageWidget::startSpinner()
{
    m_pStackViewLayout->setCurrentIndex(m_currFileViewMode);

    spinnerStart();
}

void FileManageWidget::stopSpinner()
{
    spinnerStop();
    //    if (m_pListViewFrame->getSourceModel()->rowCount() == 0) {
    //        m_pStackViewLayout->setCurrentIndex(VIEW_EMPTY);
    //    }
}

void FileManageWidget::getDirFile(const QString &path)
{
    if (!checkCanLoadFile()) {
        //???????????????????????? ??????
        return;
    }

    getFileListFromDevice(E_Widget_File, path);

    addPathToHistoryList(path);
    navigationBtnEnabled();
    setNewDelBtnEnable();
}

void FileManageWidget::slotDeleteFileFinish(const int &result, const QString &filePath)
{
    QFileInfo fi = QFileInfo(filePath);
    if (result == 0) {
        deleteItemFromUI(fi.fileName(), filePath);
        delPathFromHistoryList(filePath);

        navigationBtnEnabled();
    } else {
        QString info = TrObject::getInstance()->getFileOperationText(Deletion_failed_count).arg(fi.fileName());
        sendWarnMessage(info);
    }
}

void FileManageWidget::addPathToHistoryList(QString path)
{
    if (m_currentPathIndex < m_pHistoryPathList.size() - 1) {
        if (path != m_pHistoryPathList.at(m_currentPathIndex + 1)) {
            for (auto i = m_pHistoryPathList.size() - 1; i > m_currentPathIndex; i--) {
                m_pHistoryPathList.removeLast();
            }
            m_pHistoryPathList.append(path);
            m_currentPathIndex = m_pHistoryPathList.size() - 1;
        } else {
            m_currentPathIndex++;
        }
    } else {
        m_pHistoryPathList.append(path);
        m_currentPathIndex = m_pHistoryPathList.size() - 1;
    }
}

void FileManageWidget::delPathFromHistoryList(QString path)
{
    if (!m_pHistoryPathList.contains(path)) {
        return;
    } else {
        int index = m_pHistoryPathList.indexOf(path);
        m_pHistoryPathList.removeOne(path);

        if (index <= m_currentPathIndex) {
            m_currentPathIndex--;
        }
    }
}

void FileManageWidget::forwardBtnBoxClicked()
{
    if (!checkCanLoadFile()) {
        //???????????????????????? ??????
        return;
    }

    if (m_currentPathIndex < m_pHistoryPathList.size() - 1) {
        m_currentPathIndex++;

        navigationBtnEnabled();

        QString path = m_pHistoryPathList.at(m_currentPathIndex);
        getFileListFromDevice(E_Widget_File, path);

        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_New_Folder_Btn, true);
        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Import_Btn, true);

        setTitleBtnEnble();
        setNewDelBtnEnable();
    }
}

void FileManageWidget::backwardBtnBoxClicked()
{
    if (!checkCanLoadFile()) {
        //???????????????????????? ??????
        return;
    }

    if (m_currentPathIndex > 0) {
        m_currentPathIndex--;

        navigationBtnEnabled();

        QString path = m_pHistoryPathList.at(m_currentPathIndex);
        getFileListFromDevice(E_Widget_File, path);

        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_New_Folder_Btn, true);
        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Export_Btn, true);
        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Import_Btn, true);
        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Delete_Btn, true);
        setNewDelBtnEnable();
    }
}

void FileManageWidget::navigationBtnEnabled()
{
    if (this->isVisible() == false)
        return;

    if (m_currentPathIndex == 0)
        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Left_Btn, false);
    else
        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Left_Btn, true);

    if (m_currentPathIndex == m_pHistoryPathList.size() - 1)
        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Right_Btn, false);
    else
        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Right_Btn, true);
}

void FileManageWidget::slotCreateNewFile()
{
    if (this->isVisible() && !checkOperating()) {
        m_newNameRepeat = false;
        createNewFile();
        setAllTitleBtnEnabled(false);
    }
}

void FileManageWidget::slotExportBtnClicked()
{
    if (!this->isVisible() || checkOperating()) {
        return;
    }

    exportFile();
}

void FileManageWidget::slotImportBtnClicked()
{
    if (!this->isVisible() || checkOperating()) {
        return;
    }

    importFile();
}

void FileManageWidget::slotDeleteBtnClicked()
{
    if (!this->isVisible() || checkOperating()) {
        return;
    }

    deleteFile();
}

void FileManageWidget::slotIconViewBtnClicked()
{
    if (m_currFileViewMode == VIEW_ICON_MODE)
        return;

    qDebug() << __FUNCTION__;
    m_currFileViewMode = VIEW_ICON_MODE;
    auto model = m_pListViewFrame->getSourceModel();
    if (model->rowCount() > 0) {
        m_pStackViewLayout->setCurrentIndex(m_currFileViewMode);

        auto indexEs = m_pTreeViewFrame->selectionModel()->selectedIndexes();
        foreach (auto index, indexEs) {
            QFileInfo tempInfo = index.data(ROLE_ITEM_DATA).value<QFileInfo>();
            //            QString sName = info.name;
            QString sPath = tempInfo.absoluteFilePath();

            //??????ICONMODEL????????????
            for (auto i = 0; i < model->rowCount(); i++) {
                QFileInfo info = model->item(i, 0)->data(ROLE_ITEM_DATA).value<QFileInfo>();
                if (info.absoluteFilePath().contains('/') && info.absoluteFilePath().compare(sPath) == 0) {
                    m_pListViewFrame->setCurrentIndex(model->index(i, 0));
                    break;
                }
            }
        }
    } else {
        if (!SpinnerIsVisible()) {
            m_pStackViewLayout->setCurrentIndex(m_currFileViewMode);
        } else {
            m_pStackViewLayout->setCurrentIndex(VIEW_EMPTY);
        }
    }

    setTitleBtnEnble();
    setNewDelBtnEnable();

    //    if (d_ptr->m_pImportProgress && d_ptr->m_pImportProgress->isVisible()) {
    //        d_ptr->m_pImportProgress->raise();
    //    }
}

void FileManageWidget::slotListViewBtnClicked()
{
    if (m_currFileViewMode == VIEW_LIST_MODE)
        return;

    qDebug() << __FUNCTION__;
    m_currFileViewMode = VIEW_LIST_MODE;

    auto model = m_pTreeViewFrame->getSourceModel();
    if (model->rowCount() > 0) {
        m_pStackViewLayout->setCurrentIndex(m_currFileViewMode);

        auto indexEs = m_pListViewFrame->selectionModel()->selectedIndexes();

        //        auto pModel = m_pTreeViewFrame->pViewModel();

        foreach (auto index, indexEs) {
            QString path = index.data(ROLE_ITEM_DATA).value<QFileInfo>().absoluteFilePath();
            //??????LISTMODEL????????????
            m_pTreeViewFrame->setItemSelect(path, true);
        }
    } else {
        if (!SpinnerIsVisible()) {
            m_pStackViewLayout->setCurrentIndex(VIEW_LIST_MODE);
        } else {
            m_pStackViewLayout->setCurrentIndex(VIEW_EMPTY);
        }
    }

    setTitleBtnEnble();
    setNewDelBtnEnable();

    //    if (d_ptr->m_pImportProgress && d_ptr->m_pImportProgress->isVisible()) {
    //        d_ptr->m_pImportProgress->raise();
    //    }
}

void FileManageWidget::createNewFile(QString name)
{
    QString fileName = name;
    //    //??????????????????????????????" < > : ' " \ | / ? "
    //    fileName.replace('<', "");
    //    fileName.replace('>', "");
    //    fileName.replace(':', "");
    //    fileName.replace("'", "");
    //    fileName.replace('"', "");
    //    fileName.replace("\\", "");
    //    fileName.replace('|', "");
    //    fileName.replace('/', "");
    //    fileName.replace('?', "");
    //    //????????????????????????????????????????????????????????????????????????????????????????????????????????? //add by zhangll PMS40279
    //    if (name.trimmed().isEmpty() || fileName.trimmed().isEmpty())
    //        fileName = TrObject::getInstance()->getTreeHeaderText(Tree_New_folder);
    //    else
    //        fileName = name;

    QString path = m_currPath + "/" + fileName;
    //??????????????????"."???????????????????????????????????????
    if (fileName.startsWith(".")) {
        //?????????????????????
        fileName = TrObject::getInstance()->getTreeHeaderText(Tree_New_folder);
        path = m_currPath + "/" + fileName;
        Utils::createFileName(path, fileName);
    } else {
        //???????????????
        Utils::createFileName(path, fileName);
        //?????????????????????????????????copy?????????????????? //PMS39275 //add by zhangll
        QString fileCopyOnly = QString("(%1)").arg(TrObject::getInstance()->getFileOperationText(file_copy));
        if (fileName.isEmpty() || fileName == fileCopyOnly) {
            //?????????????????????
            fileName = TrObject::getInstance()->getTreeHeaderText(Tree_New_folder);
            path = m_currPath + "/" + fileName;
            Utils::createFileName(path, fileName);
        }
    }

    QFileInfo fi(path);
    //    file.name = fileName;
    //    file.path = path;
    //    file.isDir = true;

    //????????????
    DFileIconProvider iiicon;
    QFileInfo fileII("/etc");
    QIcon icon = iiicon.icon(fileII);

    //    QFileInfo fi = QFileInfo(file.path);

    QVariant var;
    var.setValue(fi);

    if (m_currFileViewMode == VIEW_ICON_MODE) {
        DStandardItem *iconItemName = new DStandardItem(fileName);
        iconItemName->setIcon(icon);
        iconItemName->setData(var, ROLE_ITEM_DATA);
        //pms47535???????????????????????????????????????????????????????????????????????????
        //????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????????
        //????????????????????????????????????????????????????????????????????????????????????
        iconItemName->setData(QVariant(true), ROLE_NEW_DIR); //???????????????????????????????????????????????????????????????//true????????????????????????

        auto model = m_pListViewFrame->getSourceModel();
        model->appendRow(iconItemName);

        m_editNameIndex = model->indexFromItem(iconItemName);
        m_pListViewFrame->setCurrentIndex(m_editNameIndex);
        m_pListViewFrame->edit(model->indexFromItem(iconItemName));

        m_pStackViewLayout->setCurrentIndex(VIEW_ICON_MODE);
    } else {
        DStandardItem *listItemName = new DStandardItem(fileName);
        listItemName->setIcon(icon);
        listItemName->setData(var, ROLE_ITEM_DATA);
        listItemName->setData("NewItem", Qt::UserRole + 1);
        listItemName->setData(QVariant(true), ROLE_NEW_DIR); //???????????????????????????????????????????????????????????????,//true????????????????????????

        QString strSize;
        QVariant usrVarSize;
        //        if (fi.isDir()) {
        //            strSize = "-";
        //            usrVarSize.setValue(0);
        //        } else {
        //            strSize = Utils::dataSizeToString(quint64(fi.size()));
        //            usrVarSize.setValue(fi.size());
        //        }
        //???????????????????????????????????????????????????QFileInfo????????? isDir?????????????????????????????????????????????????????? ???????????????????????????-???
        DStandardItem *itemSize = new DStandardItem("-");
        itemSize->setData(usrVarSize);
        QString date = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
        DStandardItem *itemDate = new DStandardItem(date);
        //        QString type = fi.isDir() ? TrObject::getInstance()->getTreeHeaderText(Tree_Directory) : TrObject::getInstance()->getTreeHeaderText(Tree_File);
        //        if (!fi.isDir() && !fi.suffix().isEmpty()) {
        //            type = fi.suffix();
        //        }
        //????????????????????????
        DStandardItem *itemType = new DStandardItem(TrObject::getInstance()->getTreeHeaderText(Tree_Directory));

        QList<QStandardItem *> rowItem;
        rowItem << listItemName << itemSize << itemDate << itemType;
        auto model = m_pTreeViewFrame->getSourceModel();
        model->appendRow(rowItem);
        m_editNameIndex = model->indexFromItem(listItemName);
        m_pTreeViewFrame->reset();

        auto pSortModel = m_pTreeViewFrame->getSortFilterProxyModel();
        for (auto rowIndex = 0; rowIndex < pSortModel->rowCount(); rowIndex++) {
            if (pSortModel->index(rowIndex, 0).data(Qt::UserRole + 1).toString() == "NewItem") {
                QModelIndex NameIndex = pSortModel->index(rowIndex, 0);
                pSortModel->setData(NameIndex, "", Qt::UserRole + 1);
                m_pTreeViewFrame->setCurrentIndex(NameIndex);
                m_pTreeViewFrame->edit(NameIndex);
                break;
            }
        }
        m_pStackViewLayout->setCurrentIndex(VIEW_LIST_MODE);
    }
    //    m_createNewFile = true;
}

void FileManageWidget::createFile(QString fileName)
{
    QString newPath = m_currPath + "/" + fileName;

    QFileInfo fi(newPath);
    //    file.name = fileName;
    //    file.path = newPath;
    //    file.isDir = true;

    //??????????????????
    DFileIconProvider iiicon;
    QFileInfo fileII("/etc");
    QIcon icon = iiicon.icon(fileII);

    //    QFileInfo fi = QFileInfo(file.path);
    QVariant var;
    var.setValue(fi);

    if (m_currFileViewMode == VIEW_ICON_MODE) {
        DStandardItem *listItemName = new DStandardItem(fileName);
        listItemName->setIcon(icon);
        listItemName->setData(var, ROLE_ITEM_DATA);

        QString strSize;
        QVariant usrVarSize;
        //???????????????????????????????????????????????????QFileInfo????????? isDir?????????????????????????????????????????????????????? ???????????????????????????-???
        strSize = "-";
        usrVarSize.setValue(0);
        DStandardItem *itemSize = new DStandardItem(strSize);
        QString date = QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss");
        DStandardItem *itemDate = new DStandardItem(date);
        QString type = TrObject::getInstance()->getTreeHeaderText(Tree_Directory);
        DStandardItem *itemType = new DStandardItem(type);

        //        if (fi.isDir()) {
        //            strSize = "-";
        //            usrVarSize.setValue(0);
        //        } else {
        //            strSize = Utils::dataSizeToString(quint64(fi.size()));
        //            usrVarSize.setValue(fi.size());
        //        }

        //        DStandardItem *itemSize = new DStandardItem(strSize);
        //        QString date = fi.lastModified().toString("yyyy/MM/dd hh:mm:ss");
        //        DStandardItem *itemDate = new DStandardItem(date);
        //        QString type = fi.isDir() ? TrObject::getInstance()->getTreeHeaderText(Tree_Directory) : TrObject::getInstance()->getTreeHeaderText(Tree_File);
        //        if (!fi.isDir() && !fi.suffix().isEmpty()) {
        //            type = fi.suffix();
        //        }

        QList<QStandardItem *> rowItem;
        rowItem << listItemName << itemSize << itemDate << itemType;
        auto model = m_pTreeViewFrame->getSourceModel();
        model->appendRow(rowItem);

        m_pListViewFrame->getSourceModel()->setData(m_editNameIndex, var, ROLE_ITEM_DATA);
    } else if (m_currFileViewMode == VIEW_LIST_MODE) {
        DStandardItem *iconItemName = new DStandardItem(fileName);
        iconItemName->setIcon(icon); //, Qt::DecorationRole);
        iconItemName->setData(var, ROLE_ITEM_DATA);
        m_pListViewFrame->getSourceModel()->appendRow(iconItemName);
        auto model = m_pTreeViewFrame->getSourceModel();
        model->setData(m_editNameIndex, var, ROLE_ITEM_DATA);
    }

    createNewFileOnDevice(newPath);
}

void FileManageWidget::createNewFileOnDevice(QString path)
{
    d_ptr->m_optType = FILE_NEW;
    FileManageThread *fileThrd = new FileManageThread(m_devInfo);
    fileThrd->setPath(m_rootPath, path, FILE_NEW, FT_DIR);
    connect(fileThrd, &FileManageThread::finished, this, &FileManageWidget::slotCreateFileThreadFinish);
    connect(fileThrd, &FileManageThread::sigFileProcResult, this, &FileManageWidget::slotSingleFileProcFinish);
    connect(fileThrd, &FileManageThread::finished, fileThrd, &FileManageThread::deleteLater);

    qDebug() << __LINE__ << __FUNCTION__ << path;

    fileThrd->start();
    startSpinner();
}

void FileManageWidget::deleteFile()
{
    QStringList selectFileList;

    if (m_currFileViewMode == VIEW_ICON_MODE) {
        if (m_pListViewFrame->currentIndex().row() >= 0) {
            QModelIndexList listIndexs = m_pListViewFrame->selectionModel()->selectedIndexes();
            for (auto listIt = listIndexs.begin(); listIt != listIndexs.end(); ++listIt) {
                if (!listIt->data(ROLE_ITEM_DATA).value<QFileInfo>().absoluteFilePath().isEmpty()) {
                    selectFileList << listIt->data(ROLE_ITEM_DATA).value<QFileInfo>().absoluteFilePath();
                }
            }
        }
    } else {
        if (m_pTreeViewFrame->currentIndex().row() >= 0) {
            QModelIndexList listIndexs = m_pTreeViewFrame->selectionModel()->selectedIndexes();
            for (auto listIt = listIndexs.begin(); listIt != listIndexs.end(); ++listIt) {
                if (!listIt->data(ROLE_ITEM_DATA).value<QFileInfo>().absoluteFilePath().isEmpty()) {
                    selectFileList << listIt->data(ROLE_ITEM_DATA).value<QFileInfo>().absoluteFilePath();
                }
            }
        }
    }

    //    if (fileInfo.path.isEmpty())
    //        return;

    if (selectFileList.isEmpty()) {
        sendWarnMessage(TrObject::getInstance()->getFileOperationText(Please_select_a_file));
        return;
    }

    bool bRet = execMsgBox(TrObject::getInstance()->getFileOperationText(Delete_it));
    if (!bRet) //???????????????
        return;

    //    FileType optType;
    //    if (fileInfo.isDir)
    //    optType = FT_DIR;
    //    else
    //        optType = FT_FILE;

    d_ptr->m_bIsLoading = false;
    d_ptr->m_optType = FILE_DEL;

    auto task = new DeleteFileTask(qApp);
    task->setPathList(selectFileList);

    connect(task, &DeleteFileTask::sigTaskFinished, this, &FileManageWidget::slotFileThreadFinish);
    connect(task, &DeleteFileTask::sigDeleteFileResult, this, &FileManageWidget::slotDeleteFileFinish);
    ThreadService::getService()->startTask(ThreadService::E_File_Delete_Task, task);

    //    FileType optType = FT_FILE;

    //    FileManageThread *fileThrd = new FileManageThread(m_devInfo);
    //    fileThrd->setPathList(m_rootPath, selectFileList, FILE_DEL, optType);
    //    connect(fileThrd, &FileManageThread::finished, this, &FileManageWidget::slotFileThreadFinish);
    //    connect(fileThrd, &FileManageThread::sigFileProcResult, this, &FileManageWidget::slotSingleFileProcFinish);
    //    connect(fileThrd, &FileManageThread::finished, fileThrd, &FileManageThread::deleteLater);

    //    qDebug() << __LINE__ << __FUNCTION__;

    //    fileThrd->start();
    startSpinner();

    // delPathFromHistoryList(fileInfo.path);
    // deleteItemFromUI(fileInfo.path);
}

void FileManageWidget::deleteItemFromUI(const QString &name, const QString &path)
{
    qDebug() << __LINE__ << __FUNCTION__ << path;

    //??????LISTMODEL????????????
    m_pListViewFrame->deleteModelByPath(name, path);
    //    m_pListViewFrame->pCustomListView()->reset();

    m_pTreeViewFrame->deleteModelByPath(name, path);
    //    m_pTreeViewFrame->pCustomTreeView()->reset();
}

void FileManageWidget::exportFile()
{
    if (!this->isVisible()) {
        return;
    }

    if (m_pListViewFrame->currentIndex().row() < 0 && m_pTreeViewFrame->currentIndex().row() < 0) {
        //?????????????????????
        sendWarnMessage(TrObject::getInstance()->getFileOperationText(Please_select_a_file));
        return;
    }

    QFileInfo fi;

    if (m_currFileViewMode == VIEW_ICON_MODE) {
        fi = m_pListViewFrame->currentIndex().data(ROLE_ITEM_DATA).value<QFileInfo>();
    } else if (m_currFileViewMode == VIEW_LIST_MODE) {
        auto pSortModel = m_pTreeViewFrame->getSortFilterProxyModel();
        fi = pSortModel->index(m_pTreeViewFrame->currentIndex().row(), 0).data(ROLE_ITEM_DATA).value<QFileInfo>();
    }
    QString srcPath = fi.absoluteFilePath();

    qDebug() << __LINE__ << __FUNCTION__ << srcPath;

    //?????????????????????
    QString exportDstPath = getExportPath();
    if (exportDstPath.isEmpty() == false) {
        //        QFileInfo fi = QFileInfo(fileInfo.path);

        QStringList selectFileList;
        selectFileList.append(srcPath);
        //        FileType optType;
        //        if (fi.isDir())
        //            optType = FT_DIR;
        //        else
        //            optType = FT_FILE;

        //???????????????????????????
        QStringList replaceFile; //????????????????????????
        QStringList coexistFile; //????????????????????????
        checkFileRepeatProc_QFile(exportDstPath, selectFileList, replaceFile, coexistFile);

        if (selectFileList.isEmpty())
            return;

        //????????????
        startImportExportFile(FILE_EXPORT, selectFileList, replaceFile, coexistFile, exportDstPath);
    }
}

void FileManageWidget::importFile()
{
    if (!this->isVisible()) {
        return;
    }

    QFile filePath(m_currPath);
    if (!filePath.exists()) {
        sendWarnMessage(TrObject::getInstance()->getMountText(Failed_mount_the_directories));
        return;
    }

    QStringList selectFileList =
        DFileDialog::getOpenFileNames(this, "", DStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    if (selectFileList.isEmpty())
        return;

    //???????????????????????????
    QStringList replaceFile; //????????????????????????
    QStringList coexistFile; //????????????????????????
    checkFileRepeatProc(m_currPath, selectFileList, replaceFile, coexistFile);

    if (selectFileList.isEmpty())
        return;

    //?????????????????????????????????????????????????????????????????????????????????????????????
    if (m_pListViewFrame->getSourceModel()->rowCount() == 0) {
        m_pStackViewLayout->setCurrentIndex(m_currFileViewMode);
    }

    //????????????
    startImportExportFile(FILE_IMPORT, selectFileList, replaceFile, coexistFile, m_currPath);

    qDebug() << __LINE__ << __FUNCTION__;
}

void FileManageWidget::startImportExportFile(const E_File_OptionType &optType, const QStringList &fileList, const QStringList &replaceList, const QStringList &coexistList, const QString &dstPath)
{
    if (fileList.size() == 1)
        m_importExportOneFile = true;
    else
        m_importExportOneFile = false;

    d_ptr->m_OkCount = 0;
    d_ptr->m_optType = optType;
    d_ptr->m_importExportCancel = true; //?????????true,???????????????????????????false

    d_ptr->m_importExportFileThrd = new FileManageThread(m_devInfo);
    d_ptr->m_importExportFileThrd->setCopyInfo(m_rootPath, fileList, replaceList, coexistList, dstPath, d_ptr->m_optType, FT_FILE);

    connect(d_ptr->m_importExportFileThrd, &FileManageThread::finished, this, &FileManageWidget::slotFileThreadFinish);
    connect(d_ptr->m_importExportFileThrd, &FileManageThread::sigFileProcResult, this, &FileManageWidget::slotSingleFileProcFinish);
    connect(d_ptr->m_importExportFileThrd, &FileManageThread::sigFileRepeatConfirm, this, &FileManageWidget::slotFileRepeatConfirm);
    connect(d_ptr->m_importExportFileThrd, &FileManageThread::sigProgressRefresh, d_ptr, &CustomWidgetPrivate::slotUpdateImportExportProgressValue);

    qDebug() << __FUNCTION__ << " start.";
    d_ptr->m_importExportFileThrd->start();

    QString titleinfo;
    if (optType == FILE_IMPORT)
        titleinfo = TrObject::getInstance()->getDlgTitle(dlg_Importing);
    else if (optType == FILE_EXPORT)
        titleinfo = TrObject::getInstance()->getDlgTitle(dlg_Exporting);
    else if (optType == FILE_PASTE)
        titleinfo = TrObject::getInstance()->getDlgTitle(dlg_Copying);

    d_ptr->m_pImportExportProgressDlg = new ImportExportProgressDlg(titleinfo, this);
    connect(d_ptr->m_importExportFileThrd, &FileManageThread::sigAllFileCount, this, [=](int count) {
        if (d_ptr->m_pImportExportProgressDlg != nullptr) {
            d_ptr->m_pImportExportProgressDlg->setProgressBarRange(0, count);
        }
    });

    if (fileList.count() > 1) {
        d_ptr->m_pImportExportProgressDlg->setProgressBarRange(0, fileList.count());
    } else if (fileList.count() == 1) {
        QFileInfo fileInfo(fileList.at(0));
        if (fileInfo.isFile()) {
            startFakeProcessBarUpdateProc(); //??????????????????????????????????????????
        }
    }

    d_ptr->m_pImportExportProgressDlg->exec();
    d_ptr->closeImportExportProgress(E_Widget_File);

    if (m_timer != nullptr && m_timer->isActive()) {
        m_timer->stop();
        delete m_timer;
        m_timer = nullptr;
    }

    qDebug() << __FUNCTION__ << " end.";
}

void FileManageWidget::createNewFileItem(QString path, QFileInfo file_info, QString fileName)
{
    Q_UNUSED(file_info);

    QFileInfo fi = QFileInfo(path);
    qDebug() << __LINE__ << __FUNCTION__ << path;
    QString tmp_fileName = fileName.isEmpty() ? fi.fileName() : fileName;
    //    file_info.name = tmp_fileName;
    //     file_info.path = m_currPath + "/" + tmp_fileName;

    QFileInfo tempFile(m_currPath + "/" + tmp_fileName);

    //    QFileInfo fi = QFileInfo(file_info.path);

    if (photoSuffix.contains(tempFile.suffix(), Qt::CaseInsensitive)) {
        m_pImageList.append(tempFile.absoluteFilePath());
    } else if (videoSuffix.contains(tempFile.suffix(), Qt::CaseInsensitive)) {
        m_pVideoList.append(tempFile.absoluteFilePath());
    }

    QVariant var;
    var.setValue(tempFile);
    m_pUIModelManager->appendData(var);

    m_pStackViewLayout->setCurrentIndex(m_currFileViewMode);
}

void FileManageWidget::slotDListViewItemClicked(const QModelIndex &index)
{
    if (m_currFileViewMode != VIEW_ICON_MODE)
        return;

    if (index.isValid())
        setTitleBtnEnble(true);

    setNewDelBtnEnable();
}

void FileManageWidget::slotDTreeViewItemClicked(const QModelIndex &index)
{
    if (m_currFileViewMode != VIEW_LIST_MODE)
        return;

    Q_UNUSED(index);

    qDebug() << __LINE__ << __FUNCTION__;

    if (index.isValid())
        setTitleBtnEnble(true);

    setNewDelBtnEnable();

    //??????????????????
    QString path = index.data(ROLE_ITEM_DATA).value<QFileInfo>().absoluteFilePath();

    getDirFileInfo(path); //????????????????????????

    //??????ICONMODEL????????????
    auto model = m_pListViewFrame->getSourceModel();
    for (auto i = 0; i < model->rowCount(); i++) {
        QFileInfo info = model->item(i, 0)->data(ROLE_ITEM_DATA).value<QFileInfo>();
        if (info.absoluteFilePath().contains('/') && info.absoluteFilePath().compare(path) == 0) {
            m_pListViewFrame->setCurrentIndex(model->index(i, 0));
            break;
        }
    }

    //    qDebug() << __LINE__ << __FUNCTION__;
}

void FileManageWidget::slotCopyFile()
{
    QString copyPath = "";
    if (m_currFileViewMode == VIEW_ICON_MODE) {
        if (m_pListViewFrame->currentIndex().row() >= 0) {
            QFileInfo fileInfo = m_pListViewFrame->currentIndex().data(ROLE_ITEM_DATA).value<QFileInfo>();
            copyPath = fileInfo.absoluteFilePath();
            qDebug() << "ICON_MODE:" << __FUNCTION__ << copyPath;
        }
    } else if (m_currFileViewMode == VIEW_LIST_MODE) {
        if (m_pTreeViewFrame->currentIndex().row() >= 0) {
            //?????????0?????????????????????
            auto pSortModel = m_pTreeViewFrame->getSortFilterProxyModel();
            QFileInfo fileInfo = pSortModel->index(m_pTreeViewFrame->currentIndex().row(), 0).data(ROLE_ITEM_DATA).value<QFileInfo>(); //??????????????????
            copyPath = fileInfo.absoluteFilePath(); //??????????????????
            qDebug() << "LIST_MODE:" << __FUNCTION__ << copyPath; //??????????????????
        }
    }

    qDebug() << "FileManageWidget::slotCopyFile()" << copyPath;

    if (!copyPath.isEmpty()) {
        QByteArray gnomeFormat = QByteArray("copy\n");
        QString text;
        QList<QUrl> dataUrls;
        text += copyPath + "\n";
        dataUrls << QUrl(QFileInfo(copyPath).absoluteFilePath());
        gnomeFormat.append(QUrl::fromLocalFile(copyPath).toEncoded()).append("\n");
        QByteArray uriList(QUrl::fromLocalFile(copyPath).toEncoded());

        //setText  need to be called, or you can't clip the context to clipboard.
        QMimeData *newMimeData = new QMimeData();
        newMimeData->setText(text);
        newMimeData->setUrls(dataUrls);
        gnomeFormat.remove(gnomeFormat.length() - 1, 1);
        newMimeData->setData("x-special/gnome-copied-files", gnomeFormat);
        newMimeData->setData("text/uri-list", uriList);
        // Set the mimedata
        QClipboard *cb = QApplication::clipboard();
        cb->setMimeData(newMimeData);
    }
}

void FileManageWidget::slotPasteFile()
{
    if (!this->isVisible() || checkOperating()) {
        return;
    }

    QFile filePath(m_currPath);
    if (!filePath.exists()) {
        sendWarnMessage(TrObject::getInstance()->getMountText(Failed_mount_the_directories));
        return;
    }

    const QClipboard *clipboard = QApplication::clipboard(); //?????????????????????

    //????????????????????????????????????????????????MIME?????????????????????
    //???????????????????????????????????????????????????????????????
    const QMimeData *mimeData = clipboard->mimeData();

    qDebug() << "FileManageWidget::slotPasteFile()";

    if (mimeData->hasUrls()) {
        QString mimePath = mimeData->text();
        QStringList mimePathList = mimePath.split("\n", QString::SkipEmptyParts);

        qDebug() << __FUNCTION__ << mimePathList; //??????????????????

        //        m_lastPasteFile = "";
        //        d_ptr->m_optType = FILE_PASTE;

        //        m_copyFileThrd = new FileManageThread(m_devInfo);
        //        m_copyFileThrd->setCopyInfo(m_rootPath, mimePathList, m_currPath, d_ptr->m_optType, FT_FILE);

        //        connect(m_copyFileThrd, &FileManageThread::finished, this, &FileManageWidget::slotFileThreadFinish);
        //        connect(m_copyFileThrd, &FileManageThread::sigFileProcResult, this,
        //                &FileManageWidget::slotSingleFileProcFinish);
        //        connect(m_copyFileThrd, &FileManageThread::sigFileRepeatConfirm, this,
        //                &FileManageWidget::slotFileRepeatConfirm);

        //        qDebug() << "FileManageWidget::slotPasteFile() start.";
        //        m_copyFileThrd->start();
        //        startSpinner();

        m_lastPasteFile = "";

        //???????????????????????????
        QStringList replaceFile; //????????????????????????
        QStringList coexistFile; //????????????????????????
        checkFileRepeatProc_QFile(m_currPath, mimePathList, replaceFile, coexistFile);

        if (mimePathList.isEmpty())
            return;

        //????????????
        startImportExportFile(FILE_PASTE, mimePathList, replaceFile, coexistFile, m_currPath);

        //?????????????????????????????????????????????????????????????????????????????????????????????
        if (m_pListViewFrame->getSourceModel()->rowCount() == 0) {
            m_pStackViewLayout->setCurrentIndex(m_currFileViewMode);
        }
    }
}

void FileManageWidget::slotDeleteFile()
{
    if (checkOperating())
        return;

    deleteFile();
}

void FileManageWidget::slotRefresh()
{
    if (m_rootPath.isEmpty()) {
        //????????????????????????????????????????????????????????????????????????
        dispRootPathData();
        return;
    }

    getFileListFromDevice(m_currType, m_currPath);
}

void FileManageWidget::slotFileItemEditFinish(QString name)
{
    QString errInfo;
    QString fileName;
    name = name.trimmed();
    if (name.isEmpty()) {
        errInfo = TrObject::getInstance()->getMessageText(Input_a_folder_name_please); //?????????????????????
    } else if (name.startsWith(".")) {
        //pms43347 //?????????????????????.????????????????????????????????????????????????
        errInfo = TrObject::getInstance()->getMessageText(folder_name_dot_start);
    } else {
        QString filePath = m_currPath + "/" + name;
        QFile file(filePath);

        //??????????????????????????????" < > : ' " \ | / ? "
        QRegExp rege("[<>:'\"\\|/?]");

        if (name.contains(rege) || name.contains('\\')) {
            errInfo = TrObject::getInstance()->getMessageText(folder_name_illage); //??????????????????????????????
            //?????????????????????
            fileName = TrObject::getInstance()->getTreeHeaderText(Tree_New_folder);
            QString path = m_currPath + "/" + fileName;
            //???????????????
            Utils::createFileName(path, fileName);
        } else if (file.exists()) {
            DLabel infoLabel;
            infoLabel.setFixedWidth(150);
            QString text = name;
            text = Utils::ElideText(infoLabel.font(), infoLabel.width(), text);
            errInfo = TrObject::getInstance()->getMessageText(folder_name_exist).arg(text); //??????????????????
        }
    }

    if (!errInfo.isEmpty()) {
        DDialog *pDDialog = new DDialog(QString(""), errInfo, this);
        pDDialog->setIcon(QIcon::fromTheme(PhoneAssistantModel::g_app_icon_name));
        pDDialog->addButton(TrObject::getInstance()->getDlgBtnText(Dlg_Btn_ok), true, DDialog::ButtonRecommend);
        pDDialog->setMinimumHeight(175);
        pDDialog->exec();
        delete pDDialog;

        if (m_currFileViewMode == VIEW_ICON_MODE) {
            m_pListViewFrame->getSourceModel()->removeRow(m_editNameIndex.row());
        } else {
            auto model = m_pTreeViewFrame->getSourceModel();
            model->removeRow(m_editNameIndex.row());
        }

        m_pListViewFrame->reset();
        m_pTreeViewFrame->reset();

        //m_newNameRepeat = true; //mod 40407 ?????????????????????zll
        if (fileName.isEmpty())
            fileName = name;

        qDebug() << __LINE__ << __FUNCTION__ << fileName;
        emit sigNameEdit(fileName);

        return;
    }

    createFile(name);
    qDebug() << __LINE__ << __FUNCTION__;
}

void FileManageWidget::setFileViewBtnIcon()
{
    if (m_currFileViewMode != VIEW_ICON_MODE) {
        RightTitleWidget::getInstance()->setWidgetBtnChecked(RightTitleWidget::E_List_View_Btn, true);
    } else {
        RightTitleWidget::getInstance()->setWidgetBtnChecked(RightTitleWidget::E_Icon_View_Btn, true);
    }
}

void FileManageWidget::slotSingleFileProcFinish(int optType, int result, QString file, bool newFileItem, QFileInfo file_info)
{
    QFileInfo fileInfo(file);
    QString info;
    if (optType == FILE_IMPORT) {
        if (result == 0) {
            //?????????????????????????????????
            if (newFileItem) {
                createNewFileItem(file, file_info);
                bool isUpdateCount = true;
                if (m_currFileViewMode == VIEW_ICON_MODE) {
                    if (m_pListViewFrame->selectionModel()->hasSelection())
                        isUpdateCount = false; //????????????????????????????????????
                } else if (m_currFileViewMode == VIEW_LIST_MODE) {
                    if (m_pTreeViewFrame->selectionModel()->hasSelection())
                        isUpdateCount = false; //????????????????????????????????????
                }
                if (isUpdateCount)
                    calculateSelectCountSize(); //??????????????????
            } else {
                updateItemInfoByFile(file);
            }
            d_ptr->m_OkCount++;
        } else {
            if (result != -2) //-2???????????????????????????????????????
                info = TrObject::getInstance()->getImportExportText(Import_failed_count).arg(fileInfo.fileName());
        }

    } else if (optType == FILE_EXPORT) {
        if (result == 0) {
            d_ptr->m_OkCount++;
        } else {
            if (result != -2) { //-2???????????????????????????????????????
                d_ptr->m_ErrCount++;
                info = TrObject::getInstance()->getImportExportText(Export_failed_count).arg(fileInfo.fileName());
            }
        }
    } /*else if (optType == FILE_DEL) {
        if (result == 0) {
            QFileInfo fi = QFileInfo(file);
            deleteItemFromUI(fi.fileName(), file);
            delPathFromHistoryList(file);
            //            m_count--;
            navigationBtnEnabled();
        } else {
            info = TrObject::getInstance()->getFileOperationText(Deletion_failed_count).arg(fileInfo.fileName());
        }
    } */
    else if (optType == FILE_PASTE) {
        if (result == 0) {
            //?????????????????????????????????
            if (newFileItem) {
                createNewFileItem(file, file_info, fileInfo.fileName());
                m_lastPasteFile = fileInfo.absoluteFilePath();
                calculateSelectCountSize(); //??????????????????
                //                m_count++;
            }
        } else {
            if (result != -2) //-2???????????????????????????????????????
                info = TrObject::getInstance()->getFileOperationText(copy_failed_count).arg(fileInfo.fileName());
        }
    } else if (optType == FILE_NEW) {
        if (result == 0) {
            //            m_count++;
            //??????????????????????????????????????????????????????????????????????????????
            if (m_newNameRepeat) {
                m_newNameRepeat = false;
                slotRefresh();
            }

            if (m_pListViewFrame->getSourceModel()->rowCount() == 0 && !d_ptr->m_bIsLoading) {
                m_pStackViewLayout->setCurrentIndex(VIEW_EMPTY);
            }
        } else {
            info = TrObject::getInstance()->getMessageText(folder_create_failed).arg(fileInfo.fileName());
        }

        //  ??????????????????????????????????????????
        setAllTitleBtnEnabled(true);
        navigationBtnEnabled(); //????????????????????????????????????
    }

    if (result != 0 && result != -2)
        sendWarnMessage(info);
}

void FileManageWidget::refreshWidgetData()
{
    if (m_rootPath.isEmpty()) {
        //????????????????????????????????????????????????????????????????????????
        dispRootPathData();
        return;
    }

    getFileListFromDevice(m_currType, m_currPath);
}

//????????????????????????
bool FileManageWidget::isReading()
{
    //???????????????????????????
    if (d_ptr->m_bIsLoading || m_imageFileThrd != nullptr || m_videoImgFileThrd != nullptr) {
        return true; //???????????????
    }

    return false;
}

// ????????????
bool FileManageWidget::pauseRead()
{
    if (m_loadTask != nullptr) {
        return m_loadTask->lock(); //?????????????????????
    }
    //    if (m_imageFileThrd != nullptr) {
    //        return m_imageFileThrd->lock(); //????????????????????? ??????
    //    }
    //    if (m_videoImgFileThrd != nullptr) {
    //        return m_videoImgFileThrd->lock(); //??????????????????????????? ??????
    //    }

    return false;
}

// ????????????
bool FileManageWidget::continueRead()
{
    if (m_loadTask != nullptr) {
        return m_loadTask->unlock(); //?????????????????????
    }
    //    if (m_imageFileThrd != nullptr) {
    //        return m_imageFileThrd->unlock(); //????????????????????? ??????
    //    }
    //    if (m_videoImgFileThrd != nullptr) {
    //        return m_videoImgFileThrd->unlock(); //??????????????????????????? ??????
    //    }

    return true;
}

void FileManageWidget::slotCustomContextMenuRequested(const QPoint &)
{
    //    if (SpinnerIsVisible() == false || (d_ptr->m_pImportProgress != nullptr && d_ptr->m_pImportProgress->isVisible()))
    //        return;
    if (SpinnerIsVisible() == false)
        return;

    DMenu *m_cMenu = new DMenu(this);

    auto m_copyAction = m_cMenu->addAction(TrObject::getInstance()->getMenuActionText(Menu_Copy));
    auto m_pasteAction = m_cMenu->addAction(TrObject::getInstance()->getMenuActionText(Menu_Paste));
    auto m_deleteAction = m_cMenu->addAction(TrObject::getInstance()->getDlgBtnText(Dlg_Btn_Delete));
    m_cMenu->addSeparator();
    auto m_refreshAction = m_cMenu->addAction(TrObject::getInstance()->getMenuActionText(Menu_Refresh));

    connect(m_copyAction, &QAction::triggered, this, &FileManageWidget::slotCopyFile);
    connect(m_pasteAction, &QAction::triggered, this, &FileManageWidget::slotPasteFile);
    connect(m_deleteAction, &QAction::triggered, this, &FileManageWidget::slotDeleteFile);
    connect(m_refreshAction, &QAction::triggered, this, &FileManageWidget::slotRefresh);

    bool isSelectItem = false;
    if (m_currFileViewMode == VIEW_ICON_MODE) {
        if (!(m_pListViewFrame->selectionModel()->selectedIndexes()).isEmpty())
            isSelectItem = true;
    } else if (m_currFileViewMode == VIEW_LIST_MODE) {
        if (!(m_pTreeViewFrame->selectionModel()->selectedIndexes()).isEmpty())
            isSelectItem = true;
    }

    if (isSelectItem) {
        m_copyAction->setEnabled(true);
        m_deleteAction->setEnabled(true);
    } else {
        m_copyAction->setEnabled(false);
        m_deleteAction->setEnabled(false);
    }

    const QClipboard *clipboard = QApplication::clipboard(); //?????????????????????
    //????????????????????????????????????????????????MIME?????????????????????
    //???????????????????????????????????????????????????????????????
    const QMimeData *mimeData = clipboard->mimeData();
    if (mimeData->hasUrls()) {
        m_pasteAction->setEnabled(true);
    } else {
        m_pasteAction->setEnabled(false);
    }

    bool pastBtn = m_pasteAction->isEnabled();
    bool delBtn = m_deleteAction->isEnabled();
    if (m_devInfo.type == Mount_Ios && m_rootPath == m_currPath) {
        m_pasteAction->setEnabled(false);
        m_deleteAction->setEnabled(false);
    } else {
        m_pasteAction->setEnabled(pastBtn);
        m_deleteAction->setEnabled(delBtn);

        //        //???????????????????????????????????????????????????????????????????????????
        //        if (m_devInfo.usbType == USB_PTP &&  m_currentPathIndex == 1) {
        //            m_pasteAction->setEnabled(false);
        //            m_deleteAction->setEnabled(false);
        //        }

        //        //????????????????????????????????? ?????? ???
        //        if (m_currentPathIndex == 0) {
        //            m_pasteAction->setEnabled(false);
        //            m_deleteAction->setEnabled(false);
        //        }

        bool rootSubDirOnly = true; //?????????????????????????????????????????????
        if (m_rootPath.endsWith(QDir::separator())) {
            //??????????????????
            rootSubDirOnly = false;
        }

        //???????????????????????????????????????????????????????????????????????????, ????????????//????????????????????????????????? ?????? ???
        if (m_devInfo.usbType == USB_PTP) {
            if (rootSubDirOnly) {
                if (m_currentPathIndex <= 1) {
                    m_pasteAction->setEnabled(false);
                    m_deleteAction->setEnabled(false);
                }
            } else {
                if (m_currentPathIndex == 0) {
                    m_pasteAction->setEnabled(false);
                    m_deleteAction->setEnabled(false);
                }
            }
        } else {
            //????????????????????????????????? ?????? ???
            if (m_currentPathIndex == 0 || m_rootPath == m_currPath) {
                m_pasteAction->setEnabled(false);
                m_deleteAction->setEnabled(false);
            }
        }
    }

    if (m_cMenu != nullptr)
        m_cMenu->exec(QCursor::pos()); //???????????????????????????
}

void FileManageWidget::setTitleBtnEnble(bool value)
{
    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Export_Btn, value);
    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Delete_Btn, value);
}

void FileManageWidget::setTitleBtnEnble()
{
    if (!this->isVisible())
        return;

    if (m_currFileViewMode == VIEW_LIST_MODE)
        setTitleBtnEnble(m_pTreeViewFrame->selectionModel()->hasSelection());
    else {
        setTitleBtnEnble(m_pListViewFrame->selectionModel()->hasSelection());
    }
}

void FileManageWidget::setNewDelBtnEnable()
{
    if (m_devInfo.type == Mount_Ios && m_rootPath == m_currPath) {
        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_New_Folder_Btn, false);
        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Import_Btn, false);
        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Delete_Btn, false);
    } else {
        setTitleBtnEnble();

        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_New_Folder_Btn, true);
        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Import_Btn, true);

        bool rootSubDirOnly = true; //?????????????????????????????????????????????
        if (m_rootPath.endsWith(QDir::separator())) {
            //??????????????????
            rootSubDirOnly = false;
        }

        //???????????????????????????????????????????????????????????????????????????, ????????????//????????????????????????????????? ?????? ???
        if (m_devInfo.usbType == USB_PTP) {
            if (rootSubDirOnly) {
                if (m_currentPathIndex <= 1) {
                    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_New_Folder_Btn, false);
                    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Import_Btn, false);
                    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Delete_Btn, false);
                }
            } else {
                if (m_currentPathIndex == 0) {
                    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_New_Folder_Btn, false);
                    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Import_Btn, false);
                    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Delete_Btn, false);
                }
            }
        } else {
            //????????????????????????????????? ?????? ???
            if (m_currentPathIndex == 0 || m_rootPath == m_currPath) {
                TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_New_Folder_Btn, false);
                TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Import_Btn, false);
                TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Delete_Btn, false);
            }
        }
    }
}

void FileManageWidget::slotFileRepeatConfirm(QString fileName, QString srcPath, QString dstPath)
{
    FileManageThread *fileThread = nullptr;
    if (d_ptr->m_optType == FILE_EXPORT || d_ptr->m_optType == FILE_IMPORT) {
        fileThread = d_ptr->m_importExportFileThrd;
        RemovePathPrefix(srcPath, m_devInfo.type, m_devId);
        //    } else if (d_ptr->m_optType == FILE_IMPORT) {
        //        fileThread = d_ptr->m_importExportFileThrd;
        //        Utils::RemovePathPrefix(dstPath, m_devInfo.type, m_devId);
    } else if (d_ptr->m_optType == FILE_PASTE) {
        //fileThread = m_copyFileThrd;
        fileThread = d_ptr->m_importExportFileThrd;
        RemovePathPrefix(dstPath, m_devInfo.type, m_devId);
    }

    //????????????????????????
    bool timerRestart = false;
    if (m_timer != nullptr && m_timer->isActive()) {
        m_timer->stop();
        timerRestart = true;
    }

    FileRepeatConfirmDlg *confirmDlg = new FileRepeatConfirmDlg(fileName, srcPath, dstPath, false, this, false);
    if (m_importExportOneFile)
        confirmDlg->hideCheckBox();

    int ret = confirmDlg->exec();
    bool notAsk = confirmDlg->getAskCheckBoxValue();
    fileThread->setFileRepeatType(ret, notAsk);
    delete confirmDlg;

    g_fileProcWait.wakeAll();

    //????????????????????????
    if (timerRestart && m_timer != nullptr) {
        m_timer->start();
    }
}

//  ?????? ????????????
void FileManageWidget::slotUpdateSelectData(const bool &rl)
{
    if (m_currFileViewMode != VIEW_LIST_MODE)
        return;

    //  ?????????
    if (rl) {
        //  ????????????
        auto indexList = m_pTreeViewFrame->selectionModel()->selectedRows();

        auto index = indexList.at(0);

        if (index.isValid())
            setTitleBtnEnble(true);

        //getDirFileInfo(path); //????????????????????????
        QFileInfo fileInfo = index.data(ROLE_ITEM_DATA).value<QFileInfo>(); //??????????????????
        //??????????????????
        m_currSelPath = fileInfo.absoluteFilePath();
        m_currPathList << m_currSelPath; //????????????????????????
        if (fileInfo.isFile()) {
            //????????????????????????
            QString strSize = Utils::dataSizeToString(quint64(fileInfo.size()));
            QString txt = TrObject::getInstance()->getFileSelectText(file_select_szie).arg(1).arg(strSize);
            setShowText(txt);
        } else {
            //?????????????????????????????????????????????????????????????????????????????????
            if (m_dirFileTimer == nullptr) {
                m_dirFileTimer = new QTimer(this);
                connect(m_dirFileTimer, &QTimer::timeout, this, &FileManageWidget::slotGetDirFileCount);
            }
            //???????????????????????????
            m_lastSelChgTime.restart();
            m_dirFileTimer->start(1000);
        }
    } else { //  ????????????
        slotCleanSelectItem();
    }

    setNewDelBtnEnable();
}

//  ??????????????????
//void FileManageWidget::slotSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
//{
//    Q_UNUSED(selected);
//    Q_UNUSED(deselected);

//    if (m_pTreeView) {
//        bool rl = m_pTreeView->selectionModel()->hasSelection();

//        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Delete_Btn, rl);
//        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Export_Btn, rl);
//    }
//}

//????????????????????????????????? ???????????????????????????????????????
void FileManageWidget::slotSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected);
    Q_UNUSED(deselected);

    bool rl = m_pListViewFrame->selectionModel()->hasSelection();
    //    //  ?????????
    if (rl) {
        //        //  ????????????
        //        auto indexList = m_pTreeView->selectionModel()->selectedRows();

        //        auto index = indexList.at(0);

        //        slotDTreeViewItemClicked(index);
        //    } else { //  ????????????
        //        slotCleanSelectItem();

        //PMS41076
        auto indexList = m_pListViewFrame->selectionModel()->selectedRows();
        auto index = indexList.at(0);
        QFileInfo fileInfo = index.data(ROLE_ITEM_DATA).value<QFileInfo>(); //??????????????????
        m_currPathList << fileInfo.absoluteFilePath(); //????????????????????????
        if (fileInfo.isFile()) {
            QString strSize = Utils::dataSizeToString(quint64(fileInfo.size()));
            QString txt = TrObject::getInstance()->getFileSelectText(file_select_szie).arg(1).arg(strSize);
            setShowText(txt);
        } else {
            //GetDirsFileInfo(); //????????????????????????
            //?????????????????????????????????????????????????????????????????????????????????
            if (m_dirFileTimer == nullptr) {
                m_dirFileTimer = new QTimer(this);
                connect(m_dirFileTimer, &QTimer::timeout, this, &FileManageWidget::slotGetDirFileCount);
            }
            m_lastSelChgTime.restart();
            m_dirFileTimer->start(1000);
        }
    } else {
        slotCleanSelectItem();
    }

    setNewDelBtnEnable();
}

/*
 * @brief FileManageWidget::checkCanLoadFile()
 * ???????????????????????????????????????TRUE:???????????????FALSE???????????????
 */
bool FileManageWidget::checkCanLoadFile()
{
    //??????????????????????????????????????????????????????
    if (d_ptr->m_bIsLoading == true) {
        return true;
    }

    if (m_dispFileThrd != nullptr && m_dispFileThrd->isRunning()) {
        return true;
    }

    //?????????????????????????????????????????????????????? ?????? FALSE
    bool othRuning = checkOperating();
    return !othRuning;
}

void FileManageWidget::slotCleanSelectItem()
{
    if (m_currFileViewMode == VIEW_ICON_MODE) {
        m_pTreeViewFrame->setCurrentIndex(QModelIndex());
    } else {
        m_pListViewFrame->setCurrentIndex(QModelIndex());
    }

    setTitleBtnEnble();

    calculateSelectCountSize();
}

void FileManageWidget::slotNameEdit(QString name)
{
    createNewFile(name);

    setAllTitleBtnEnabled(false);
}

void FileManageWidget::getDirFileInfo(QString path)
{
    //    QFileInfo fileInfo(path);
    //    if (fileInfo.isFile()) {
    //        QString strSize = Utils::dataSizeToString(quint64(fileInfo.size()));
    //        QString txt = TrObject::getInstance()->getFileSelectText(file_select_szie).arg(1).arg(strSize);
    //        setShowText(txt);
    //        return;
    //    }

    m_currPathList = QStringList() << path;

    FileManageThread *fileThrd = new FileManageThread(m_devInfo);
    fileThrd->setPath(m_rootPath, path, FILE_DIRINFO, FT_ALL);
    connect(fileThrd, &FileManageThread::sigDirFileInfo, this, &FileManageWidget::slotDirFileInfo);
    //connect(fileThrd, &FileManageThread::finished, this, &FileManageWidget::slotFileThreadFinish);
    connect(fileThrd, &FileManageThread::finished, fileThrd, &FileManageThread::deleteLater);

    fileThrd->start();
}

void FileManageWidget::slotDirFileInfo(int itemCount, qint64 fileSize, QStringList pathList)
{
    if (!m_pTreeViewFrame->currentIndex().isValid() && !m_pListViewFrame->currentIndex().isValid())
        return;

    if (pathList != m_currPathList)
        return;

    QString strSize = Utils::dataSizeToString(quint64(fileSize));

    QString txt = "";
    if (itemCount > 0) {
        if (itemCount > 1) {
            txt = TrObject::getInstance()->getFileSelectText(folder_select_contains_items_szie).arg(itemCount).arg(strSize);
        } else {
            txt = TrObject::getInstance()->getFileSelectText(folder_select_contains_item_szie).arg(itemCount).arg(strSize);
        }
    } else {
        txt = TrObject::getInstance()->getFileSelectText(folder_select_contains_items_szie).arg(itemCount).arg(strSize);
    }

    if (m_pTreeViewFrame->currentIndex().isValid()) //????????????????????????
        setShowText(txt);
}

void FileManageWidget::SlotDirsFileInfo(int itemCount, qint64 fileSize, QStringList pathList)
{
    if (!m_pTreeViewFrame->currentIndex().isValid() && !m_pListViewFrame->currentIndex().isValid())
        return;

    if (pathList != m_currPathList)
        return;

    QString strSize = Utils::dataSizeToString(quint64(fileSize));

    QString txt = "";
    int nSize = m_pListViewFrame->selectionModel()->selectedIndexes().size();
    if (nSize == 1) {
        if (itemCount <= 1) {
            txt = TrObject::getInstance()->getFileSelectText(folder_select_contains_item_szie).arg(itemCount).arg(strSize);
        } else {
            txt = TrObject::getInstance()->getFileSelectText(folder_select_contains_items_szie).arg(itemCount).arg(strSize);
        }
    } else {
        txt = TrObject::getInstance()->getFileSelectText(folders_selected_contains_items_size).arg(nSize).arg(itemCount).arg(strSize);
    }

    if (nSize > 0) //???????????????????????????
        setShowText(txt);
}

void FileManageWidget::calculateSelectCountSize()
{
    int nCount = m_pListViewFrame->model()->rowCount();

    QString txt = TrObject::getInstance()->getFileOperationText(items_count).arg(nCount);

    setShowText(txt);
}

//bool FileManageWidget::eventFilter(QObject *pObj, QEvent *pEvent)
//{
//    if (pEvent->type() == QEvent::KeyPress) {
//        if (pObj == m_pCustomListView) {
//            QKeyEvent *pKeyEvent = dynamic_cast<QKeyEvent *>(pEvent);
//            if (pKeyEvent->key() == Qt::Key_Delete) {
//                slotDeleteFile();
//            } else if (pKeyEvent->key() == Qt::Key_Control) {
//                m_bCtrlPress = true;
//            } else if (pKeyEvent->key() == Qt::Key_Shift) {
//                m_bShiftPress = true;
//            }
//        }
//    } else if (pEvent->type() == QEvent::KeyRelease) {
//        if (pObj == m_pCustomListView) {
//            QKeyEvent *pKeyEvent = dynamic_cast<QKeyEvent *>(pEvent);
//            if (pKeyEvent->key() == Qt::Key_Control) {
//                m_bCtrlPress = false;
//            } else if (pKeyEvent->key() == Qt::Key_Shift) {
//                m_bShiftPress = false;
//            }
//        }
//    }
//    return false;
//}

/**
 * @brief ??????????????????
 */
void FileManageWidget::setWidgetBtnState()
{
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Left_Btn, true);
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Right_Btn, true);
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_New_Folder_Btn, true);
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Export_Btn, true);
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Import_Btn, true);
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Delete_Btn, true);
    RightTitleWidget::getInstance()->setWidgetBtnVisible(RightTitleWidget::E_Icon_View_Btn, true);
    RightTitleWidget::getInstance()->setWidgetBtnVisible(RightTitleWidget::E_List_View_Btn, true);

    if (m_currFileViewMode == VIEW_ICON_MODE) {
        RightTitleWidget::getInstance()->setWidgetBtnChecked(RightTitleWidget::E_Icon_View_Btn, true);
        RightTitleWidget::getInstance()->setWidgetBtnChecked(RightTitleWidget::E_List_View_Btn, false);
    } else {
        RightTitleWidget::getInstance()->setWidgetBtnChecked(RightTitleWidget::E_Icon_View_Btn, false);
        RightTitleWidget::getInstance()->setWidgetBtnChecked(RightTitleWidget::E_List_View_Btn, true);
    }
}

void FileManageWidget::slotRootPath(QString path)
{
    if (m_rootPath.isEmpty()) {
        m_rootPath = path;
        m_currPath = path;
    }

    qDebug() << __LINE__ << __FUNCTION__ << "   RootPath:   " << path;
}

void FileManageWidget::slotRootPathNotFound()
{
    m_rootPath = "";
    sendWarnMessage(TrObject::getInstance()->getMountText(Failed_mount_the_directories));

    calculateSelectCountSize();
    setTitleBtnEnble();
    setAllTitleBtnEnabled(false);
}

void FileManageWidget::slotTitleWidgetBtnClicked(const int &nId)
{
    if (this->isVisible() == false) {
        return;
    }

    if (nId == TitleWidget::E_Left_Btn) {
        backwardBtnBoxClicked();
    } else if (nId == TitleWidget::E_Right_Btn) {
        forwardBtnBoxClicked();
    } else if (nId == TitleWidget::E_New_Folder_Btn) {
        slotCreateNewFile();
    } else if (nId == RightTitleWidget::E_Icon_View_Btn) {
        slotIconViewBtnClicked();
    } else if (nId == RightTitleWidget::E_List_View_Btn) {
        slotListViewBtnClicked();
    } else if (nId == TitleWidget::E_Delete_Btn) {
        slotDeleteBtnClicked();
    } else if (nId == TitleWidget::E_Import_Btn) {
        slotImportBtnClicked();
    } else if (nId == TitleWidget::E_Export_Btn) {
        slotExportBtnClicked();
    }
}

void FileManageWidget::clearWidgetInfo(const QString &devId)
{
    if (devId == m_devId) {
        m_rootPath = "";
        m_devId = "";

        //?????????????????????????????????????????????????????????Model??????
        //m_pListViewFrame->clearModel();
        //m_pTreeViewFrame->clearModel();

        m_currentPathIndex = 0;
        m_pHistoryPathList.clear();
    }
}

void FileManageWidget::GetDirsFileInfo()
{
    QStringList strPaths;
    QModelIndexList listIndexs = m_pListViewFrame->selectionModel()->selectedIndexes();
    for (auto listIt = listIndexs.begin(); listIt != listIndexs.end(); ++listIt) {
        strPaths << listIt->data(ROLE_ITEM_DATA).value<QFileInfo>().absoluteFilePath();
    }
    m_currPathList = strPaths;

    FileManageThread *fileThrd = new FileManageThread(m_devInfo);
    fileThrd->setPathList(m_rootPath, strPaths, FILE_DIRSINFO, FT_ALL);
    connect(fileThrd, &FileManageThread::sigDirFileInfo, this, &FileManageWidget::SlotDirsFileInfo);
    connect(fileThrd, &FileManageThread::finished, fileThrd, &FileManageThread::deleteLater);

    fileThrd->start();
}

/**
 * @brief ?????? ??????
 * @param path
 */
void FileManageWidget::updateItemInfoByFile(const QString &path)
{
    //    bool bIsPicture = false; //???????????????

    //????????????
    //    QPixmap picIcon;
    QIcon icon;
    QFileInfo file_info(path);
    QString suffix = file_info.suffix();
    QString filePath = file_info.absoluteFilePath();
    QStringList photoSuffixList = photoSuffix;
    QStringList videoSuffixList = videoSuffix;

    if (photoSuffixList.contains(suffix, Qt::CaseInsensitive)) {
        QPixmap pixmap = Utils::readDiskPicture(filePath);
        icon = Utils::resizeFilePicture(pixmap);
        //        icon = picIcon;
        //        bIsPicture = true;
    } else if (videoSuffixList.contains(suffix, Qt::CaseInsensitive)) {
        int timeLen;

        QPixmap pixmap;
        Utils::_readVideo_ffmpeg_one(filePath.toLocal8Bit(), pixmap, timeLen);

        icon = pixmap;
    } else {
        icon = Model::getInstance()->getIconBySuffix(suffix, file_info);
    }

    if (icon.isNull()) {
        icon = Model::getInstance()->getIconBySuffix(suffix, file_info);
    }

    QString tmp_fileName = file_info.fileName();

    //    QFileInfo file;
    //    file.name = tmp_fileName;
    QString sTempPath = m_currPath + "/" + tmp_fileName;
    //    file.isDir = file_info.isDir();
    //    file.size = file_info.size();
    //    file.suffix = file_info.suffix();

    QFileInfo tempFi = QFileInfo(sTempPath);

    auto items = m_pListViewFrame->getSourceModel()->findItems(tmp_fileName);
    foreach (auto item, items) {
        auto fi = item->data(ROLE_ITEM_DATA).value<QFileInfo>();
        if (fi.absoluteFilePath() == path) {
            QVariant var;
            var.setValue(tempFi);
            item->setData(var, ROLE_ITEM_DATA);

            item->setIcon(icon);

            //break;
        }
    }

    auto model = m_pTreeViewFrame->getSourceModel();
    auto listItems = model->findItems(tmp_fileName);
    foreach (auto item, listItems) {
        auto fi = item->data(ROLE_ITEM_DATA).value<QFileInfo>();
        if (fi.absoluteFilePath() == path) {
            int nRow = item->row();

            QVariant var;
            var.setValue(tempFi);
            item->setData(var, ROLE_ITEM_DATA);

            item->setIcon(icon);

            QString strSize;
            if (tempFi.isDir()) {
                strSize = "-";
            } else {
                strSize = Utils::dataSizeToString(quint64(tempFi.size()));
            }

            model->setItem(nRow, 1, new DStandardItem(strSize));

            QString date = tempFi.lastModified().toString("yyyy/MM/dd hh:mm:ss");
            model->setItem(nRow, 2, new DStandardItem(date));

            QString type = tempFi.isDir() ? TrObject::getInstance()->getTreeHeaderText(Tree_Directory) : TrObject::getInstance()->getTreeHeaderText(Tree_File);
            if (!tempFi.isDir() && !tempFi.suffix().isEmpty()) {
                type = tempFi.suffix();
            }

            model->setItem(nRow, 3, new DStandardItem(type));
            //break; //?????????????????????????????????????????????????????????????????????
        }
    }

    //?????????????????????????????????????????????????????????????????????
    if (m_pTreeViewFrame->selectionModel()->hasSelection()) {
        m_pTreeViewFrame->setCurrentIndex(m_pTreeViewFrame->currentIndex());
    }

    m_pStackViewLayout->setCurrentIndex(m_currFileViewMode);
}

//??????????????????????????????????????????????????????????????????????????????????????????????????????????????????
void FileManageWidget::slotItemRefresh()
{
    qDebug() << __LINE__ << __FUNCTION__;
    QThread::sleep(1);
    slotRefresh();
}

void FileManageWidget::updateSelectFileInfo()
{
    if (m_currFileViewMode == VIEW_ICON_MODE) {
        auto indexList = m_pListViewFrame->selectionModel()->selectedRows();
        if (indexList.size() > 0) {
            auto index = indexList.at(0);
            slotDListViewItemClicked(index);
        }
    } else if (m_currFileViewMode == VIEW_LIST_MODE) {
        auto indexList = m_pTreeViewFrame->selectionModel()->selectedRows();
        if (indexList.size() > 0) {
            auto index = indexList.at(0);
            slotDTreeViewItemClicked(index);
        }
    }
}

void FileManageWidget::setAllTitleBtnEnabled(bool value)
{
    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Left_Btn, value);
    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Right_Btn, value);
    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Import_Btn, value);
    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Export_Btn, value);
    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Delete_Btn, value);
    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_New_Folder_Btn, value);
}

/**
 * @brief ???????????????????????????????????? ?????????DataService???
 * @param void
 */
void FileManageWidget::getFileIconAdvance()
{
    FileManageThread *fileThrd = new FileManageThread(m_devInfo);
    fileThrd->setPath(m_rootPath, "", FILE_ICON, FT_ALL);
    connect(fileThrd, &FileManageThread::finished, fileThrd, &FileManageThread::deleteLater);

    fileThrd->start();
}

/**
 * @brief ?????????????????????????????????????????????????????????????????????????????????????????????
 * @param void
 */
void FileManageWidget::startFakeProcessBarUpdateProc()
{
    m_processBarValue = 0;
    d_ptr->m_pImportExportProgressDlg->setProgressBarRange(0, 100);

    if (m_timer == nullptr) {
        m_timer = new QTimer(this);
    }

    connect(m_timer, &QTimer::timeout, this, &FileManageWidget::slotUpdateProcessBar);
    m_timer->setInterval(1000);
    m_timer->start();
}

void FileManageWidget::slotUpdateProcessBar()
{
    if (m_processBarValue < 60) {
        m_processBarValue += 5;
    } else if (m_processBarValue < 70) {
        m_processBarValue += 3;
    } else if (m_processBarValue < 80) {
        m_processBarValue += 2;
    } else if (m_processBarValue < 95) {
        m_processBarValue += 1;
    }
    if (m_processBarValue > 60) {
        m_timer->stop();
        m_timer->start(3000);
    }
    d_ptr->slotUpdateImportExportProgressValue(0, m_processBarValue);
}

void FileManageWidget::slotUpdateImage(const QString &path, const QPixmap &data)
{
    QJsonObject obj;
    obj.insert("path", path);

    if (data.isNull())
        return;

    m_pListViewFrame->getSourceModel()->updateDataPixmap(obj, data);
    m_pTreeViewFrame->getSourceModel()->updateDataPixmap(obj, data);
}

void FileManageWidget::slotGetImageThreadFinish()
{
    if (m_imageFileThrd != nullptr && m_imageFileThrd->isFinished()) {
        delete m_imageFileThrd;
        m_imageFileThrd = nullptr;
        m_pImageList.clear();
        qDebug() << __FUNCTION__ << " image thread end.";
    }
    if (m_videoImgFileThrd != nullptr && m_videoImgFileThrd->isFinished()) {
        delete m_videoImgFileThrd;
        m_videoImgFileThrd = nullptr;
        m_pVideoList.clear();
        qDebug() << __FUNCTION__ << " video image thread end.";
    }
}

void FileManageWidget::slotCreateFileThreadFinish()
{
    if (m_currFileViewMode == VIEW_ICON_MODE) {
        if (m_pListViewFrame->getSourceModel()->rowCount() > 0) {
            m_pStackViewLayout->setCurrentIndex(m_currFileViewMode);
        }
    } else {
        if (m_pTreeViewFrame->getSourceModel()->rowCount() > 0) {
            m_pStackViewLayout->setCurrentIndex(m_currFileViewMode);
        }
    }

    stopSpinner();

    setTitleBtnEnble();

    if (m_currFileViewMode == VIEW_LIST_MODE) { //????????????
        //PMS48378??????????????????????????????????????????
        //????????????????????????????????????????????????????????????????????????????????????????????????
        auto selIndex = m_pTreeViewFrame->currentIndex(); //????????????????????????
        m_pTreeViewFrame->reset(); //??????????????????
        if (selIndex.isValid()) {
            //????????????????????????
            m_pTreeViewFrame->setCurrentIndex(selIndex);
        }
    }

    if (m_currFileViewMode == VIEW_EMPTY) {
        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Export_Btn, false);
        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Delete_Btn, false);
    }

    setNewDelBtnEnable();
    calculateSelectCountSize();
    updateSelectFileInfo();
}

void FileManageWidget::checkFileRepeatProc(const QString &dstPath, QStringList &srcFileList, QStringList &replaceList, QStringList &coexistList)
{
    replaceList.clear();
    coexistList.clear();

    int confirmRust = -1;
    bool notAsk = false;

    //PMS43852 ???????????????????????????????????????????????? ??????ls ???????????? ???????????????????????????????????????
    //????????????ls ??? find????????? QFile::exists ??????????????????????????????????????????
    //QString s = QString("find \"%1\" -name \"%2\"").arg(dstDir).arg(fileName);
    QString cmd = QString("ls \"%1\"").arg(dstPath);
    QString sRes = Utils::execCmdNoResult(cmd);
    QStringList fileList = sRes.split("\n"); //??????????????????

    foreach (QString strItemFile, srcFileList) {
        QString fileName = Utils::getFileNameByPath(strItemFile);
        if (fileList.contains(fileName)) {
            //????????????
            if (!notAsk) {
                FileRepeatConfirmDlg *confirmDlg = new FileRepeatConfirmDlg(fileName, "", "", false, this, false);
                //????????????????????????????????????????????????
                if (srcFileList.size() == 1 || (srcFileList.size() - replaceList.size() - coexistList.size() == 1))
                    confirmDlg->hideCheckBox();

                confirmRust = confirmDlg->exec();
                notAsk = confirmDlg->getAskCheckBoxValue();
                delete confirmDlg;
            }

            if (confirmRust == 0) { //??????
                //????????????????????????
                srcFileList.removeOne(strItemFile);
            } else if (confirmRust == 1) { //??????
                replaceList.append(strItemFile);
            } else if (confirmRust == 2) { //??????
                coexistList.append(strItemFile);
            } else if (confirmRust == -1) { //???????????????
                //???????????? ??????????????????????????????????????????
                //????????????????????????
                srcFileList.removeOne(strItemFile);
            }
        }
    }

    //    //??????????????????????????????????????????????????????????????????????????????????????????????????????????????????
    //    //???????????????????????????8.0????????????????????????????????????????????????????????????
    //    bool isLowAndroid = Utils::isLowVersionAndroid(m_devInfo.strProductVersion);
    //    if (isLowAndroid) {
    //        foreach (QString strItemFile, srcFileList) {
    //            //QFileInfo fileInfo(strItemFile);
    //            //QString fileName = fileInfo.fileName();
    //            QString fileName = Utils::getFileNameByPath(strItemFile);

    //            //PMS43852 ???????????????????????????????????????????????? ??????ls ???????????? ???????????????????????????????????????
    //            //????????????ls ??? find????????? QFile::exists ??????????????????????????????????????????
    //            //QString s = QString("find \"%1\" -name \"%2\"").arg(dstDir).arg(fileName);
    //            QString cmd = QString("ls \"%1\"").arg(dstPath);
    //            QString sRes = Utils::execCmdNoResult(cmd);
    //            QStringList fileList = sRes.split("\n"); //??????????????????

    //            if (fileList.contains(fileName)) {
    //                //????????????
    //                if (!notAsk) {
    //                    FileRepeatConfirmDlg *confirmDlg = new FileRepeatConfirmDlg(fileName, "", "", false, this, false);
    //                    if (srcFileList.size() == 1)
    //                        confirmDlg->hideCheckBox();

    //                    confirmRust = confirmDlg->exec();
    //                    notAsk = confirmDlg->getAskCheckBoxValue();
    //                    delete confirmDlg;
    //                }

    //                if (confirmRust == 0) { //??????
    //                    //????????????????????????
    //                    srcFileList.removeOne(strItemFile);
    //                } else if (confirmRust == 1) { //??????
    //                    replaceList.append(strItemFile);
    //                } else if (confirmRust == 2) { //??????
    //                    coexistList.append(strItemFile);
    //                }
    //            }
    //        }

    //    } else {
    //        //???????????????adb??????
    //        //??????????????????????????????????????????
    //        QString mountPath = Utils::getAndroidMountRootPath(m_devInfo.strPhoneID);

    //        foreach (QString strItemFile, srcFileList) {
    //            //QFileInfo fileInfo(strItemFile);
    //            //QString fileName = fileInfo.fileName();
    //            QString fileName = Utils::getFileNameByPath(strItemFile);
    //            QString dstFile = dstPath + "/" + fileName;

    //            //??????????????????adb?????????
    //            QString dstFile_adb = Utils::mountPathToAdbPath(mountPath, dstFile);
    //            //????????????????????????
    //            bool isExists = Utils::adbCheckFileExists(m_devInfo.strPhoneID, dstFile_adb);
    //            if (isExists) {
    //                //????????????
    //                if (!notAsk) {
    //                    FileRepeatConfirmDlg *confirmDlg = new FileRepeatConfirmDlg(fileName, "", "", false, this, false);
    //                    if (srcFileList.size() == 1)
    //                        confirmDlg->hideCheckBox();

    //                    confirmRust = confirmDlg->exec();
    //                    notAsk = confirmDlg->getAskCheckBoxValue();
    //                    delete confirmDlg;
    //                }

    //                if (confirmRust == 0) { //??????
    //                    //????????????????????????
    //                    srcFileList.removeOne(strItemFile);
    //                } else if (confirmRust == 1) { //??????
    //                    replaceList.append(strItemFile);
    //                } else if (confirmRust == 2) { //??????
    //                    coexistList.append(strItemFile);
    //                }
    //            }
    //        }
    //    }
}

void FileManageWidget::checkFileRepeatProc_QFile(const QString &dstPath, QStringList &srcFileList, QStringList &replaceList, QStringList &coexistList)
{
    //?????????????????????????????????QFile?????????

    replaceList.clear();
    coexistList.clear();

    int confirmRust = -1;
    bool notAsk = false;

    foreach (QString strItemFile, srcFileList) {
        QFileInfo fileInfo(strItemFile);
        QString fileName = fileInfo.fileName();
        QString dstFile = dstPath + "/" + fileName;

        QString srcFile;
        if (strItemFile.startsWith("file://")) {
            // srcPath = strItemFile.mid(7);
            QUrl url(strItemFile);
            srcFile = url.toLocalFile();
        } else {
            srcFile = strItemFile;
        }

        if (QFile::exists(dstFile)) {
            //????????????
            //??????????????????????????????????????????????????????????????????????????????
            if (srcFile != dstFile) {
                if (!notAsk) {
                    FileRepeatConfirmDlg *confirmDlg = new FileRepeatConfirmDlg(fileName, "", "", false, this, false);
                    //????????????????????????????????????????????????
                    if (srcFileList.size() == 1 || (srcFileList.size() - replaceList.size() - coexistList.size() == 1))
                        confirmDlg->hideCheckBox();

                    confirmRust = confirmDlg->exec();
                    notAsk = confirmDlg->getAskCheckBoxValue();
                    delete confirmDlg;
                }

                if (confirmRust == 0) { //??????
                    //????????????????????????
                    srcFileList.removeOne(strItemFile);
                } else if (confirmRust == 1) { //??????
                    replaceList.append(strItemFile);
                } else if (confirmRust == 2) { //??????
                    coexistList.append(strItemFile);
                } else if (confirmRust == -1) { //???????????????
                    //???????????? ??????????????????????????????????????????
                    //????????????????????????
                    srcFileList.removeOne(strItemFile);
                }
            } else {
                //?????????????????????????????????????????????
                coexistList.append(strItemFile);
            }
        }
    }
}

void FileManageWidget::slotGetDirFileCount()
{
    m_dirFileTimer->stop();
    if (m_lastSelChgTime.elapsed() >= 900) {
        //???????????????????????????????????????????????????????????????????????????
        if (m_currFileViewMode == VIEW_ICON_MODE) {
            //??????????????????
            auto indexList = m_pListViewFrame->selectionModel()->selectedRows();
            if (indexList.size() > 0) {
                auto index = indexList.at(0);
                QFileInfo fileInfo = index.data(ROLE_ITEM_DATA).value<QFileInfo>();
                if (fileInfo.isFile()) {
                    //???????????????????????????
                    return;
                }
                GetDirsFileInfo(); //????????????????????????
            }
        } else {
            //??????????????????
            auto indexList = m_pTreeViewFrame->selectionModel()->selectedRows();
            if (indexList.size() > 0) {
                auto index = indexList.at(0);
                QFileInfo fileInfo = index.data(ROLE_ITEM_DATA).value<QFileInfo>();
                if (fileInfo.isFile()) {
                    //???????????????????????????
                    return;
                }
                getDirFileInfo(m_currSelPath); //????????????????????????
            }
        }
    }
}
