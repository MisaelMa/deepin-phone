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
#include "MusicWidget.h"
#include "filemanagethread.h"
#include "utils.h"

#include <QVBoxLayout>
#include <DStandardItem>
#include <DMessageManager>
#include <DFileDialog>
#include <DFrame>
#include <DTitlebar>
#include <DStandardPaths>
#include <QJsonObject>

#include "DataService.h"
#include "TrObject.h"
#include "FileModel.h"
#include "Model.h"

#include "MusicBookTreeView.h"

#include "PhoneTreeMusicModel.h"

#include "widget/TitleWidget.h"
#include "widget/RightTitleWidget.h"
#include "widget/progresswidget.h"
#include "widget/SpinnerWidget.h"
#include "widget/CustomWidget_p.h"

#include "FileOperate/MusicTask.h"
#include "FileOperate/DeleteFileTask.h"
#include "FileOperate/MusicInfoTask.h"
#include "ThreadService.h"

#define CHECK_COL 0 //CheckBox
#define NAME_COL 1 // 文件名

//#define MARTIST_COL 2 // 演唱者
#define MLENGTH_COL 2 // 时长
#define MSIZE_COL 3 // 文件大小
#define MTYPE_COL 4 // 类型
#define MDATE_COL 5 // 修改时间

const int LIST_CHECKCOL_WIDTH = 50;
const int LIST_COMMON_WIDTH = 70;
const int LIST_MNAMECOL_WIDTH = 300;
const int LIST_MDATECOL_WIDTH = 100;
//const int LIST_MARTIST_WIDTH = 120;
const int LIST_MTYPE_WIDTH = 65;

extern QWaitCondition g_fileProcWait;
extern QMutex g_enterDirMutex; //全局加载文件时读取目录信息用

DCORE_USE_NAMESPACE

MusicWidget::MusicWidget(DWidget *parent)
    : CustomWidget(parent)
{
    setObjectName("MusicWidget");

    m_pWidgetType = E_Widget_Music;
    m_timer = nullptr;
    m_processBarValue = 0;
    m_loadTask = nullptr;
    m_infoTask = nullptr;

    initUI();
    initConnect();

    setUIModel();
}

MusicWidget::~MusicWidget()
{
    g_enterDirMutex.unlock(); //程序退出析构时，解锁防止异常情况下锁住不能退出问题

    qDebug() << __FUNCTION__ << "       " << this;
    if (m_timer != nullptr) {
        m_timer->stop();
        delete m_timer;
        m_timer = nullptr;
    }

    if (d_ptr->m_importExportFileThrd != nullptr) {
        d_ptr->m_importExportFileThrd->setExit();
        d_ptr->m_importExportFileThrd->quit();
    }

    //解锁
    if (m_loadTask != nullptr) {
        m_loadTask->unlock();
        m_loadTask->setTaskStop();
    }

    //解锁音乐时长任务
    if (m_infoTask != nullptr)
        m_infoTask->unlock();

    //结束任务
    ThreadService::getService()->stopTask(ThreadService::E_Music_Display_Task);
    ThreadService::getService()->stopTask(ThreadService::E_Music_Info_Task);
    //结束删除
    ThreadService::getService()->stopTask(ThreadService::E_Music_Delete_Task);
}

void MusicWidget::initUI()
{
    m_pTreeViewFrame = new MusicBookTreeView(m_pWidgetType, this);

    connect(m_pTreeViewFrame, &BaseTreeView::sigSelectAll, this, &MusicWidget::slotSelectAll);
    connect(m_pTreeViewFrame, &BaseTreeView::sigUpdateSelectData, this, &MusicWidget::slotUpdateSelectData);

    m_pTreeViewFrame->setColumnWidth(CHECK_COL, LIST_CHECKCOL_WIDTH);
    m_pTreeViewFrame->setColumnWidth(NAME_COL, LIST_MNAMECOL_WIDTH);
    //        m_treeViewList->setColumnWidth(MARTIST_COL, LIST_MARTIST_WIDTH);
    m_pTreeViewFrame->setColumnWidth(MLENGTH_COL, LIST_COMMON_WIDTH + 10);
    m_pTreeViewFrame->setColumnWidth(MSIZE_COL, LIST_COMMON_WIDTH + 10);
    m_pTreeViewFrame->setColumnWidth(MTYPE_COL, LIST_MTYPE_WIDTH + 10);
    m_pTreeViewFrame->setColumnWidth(MDATE_COL, LIST_MDATECOL_WIDTH);

    m_pTreeViewFrame->header()->setSortIndicator(MDATE_COL, Qt::DescendingOrder);
    m_pTreeViewFrame->header()->setSortIndicatorShown(false);

    initMainLayout();
}

void MusicWidget::initConnect()
{
}

//判断是否在加载中
bool MusicWidget::isReading()
{
    if (d_ptr->m_bIsLoading) {
        return true; //正在加载中
    }

    if (m_infoTask != nullptr)
        return true; //正在读取时长

    return false;
}

// 暂停读取
bool MusicWidget::pauseRead()
{
    if (m_loadTask != nullptr)
        return m_loadTask->lock(); //加锁，线程暂停

    if (m_infoTask != nullptr)
        m_infoTask->lock(); //读时长任务加锁，线程暂停

    return false;
}

// 继续读取
bool MusicWidget::continueRead()
{
    if (m_loadTask != nullptr)
        return m_loadTask->unlock(); //解锁，线程继续

    if (m_infoTask != nullptr)
        m_infoTask->unlock(); //读时长任务解锁，线程继续

    return true;
}

void MusicWidget::updateWidgetInfo(const PhoneInfo &info)
{
    qDebug() << __LINE__ << __FUNCTION__ << "Music Data: " << info.strPhoneID;
    m_devInfo = info;

    //    emit sigFindCount(m_devId, m_count);

    calculateSelectCountSize();

    //    m_pSpinnerWidget->fontChangeDisplay(); //解决PTP下内容为空时状态栏过高问题

    if (info.strPhoneID == m_devId) {
        if (m_devInfo.usbType == USB_PTP) {
            spinnerStop();
        }
        calculateSelectCountSize();
        return;
    }

    m_devId = info.strPhoneID;
    m_rootPath = "";

    m_pUIModelManager->clearModel();
    m_pTreeViewFrame->setHeaderViewState(false);

    //传输照片模式下不支持音乐电子书，显示为空
    if (m_devInfo.usbType == USB_PTP) {
        spinnerStop();

        auto model = m_pUIModelManager->getTreeModel();
        m_pTreeViewFrame->setHeaderViewEnable(model->rowCount());

        bool rl = m_pTreeViewFrame->selectionModel()->hasSelection();

        slotUpdateSelectData(rl);
        return;
    }

    //获取根目录下的所有文件
    getDataFromDevice(m_rootPath);
}

void MusicWidget::getDataFromDevice(const QString &path)
{
    qDebug() << __LINE__ << __FUNCTION__ << path;

    d_ptr->m_optType = FILE_DISPLAY;

    ThreadService::getService()->stopTask(ThreadService::E_Music_Display_Task);
    ThreadService::getService()->stopTask(ThreadService::E_Music_Info_Task);

    if (m_loadTask == nullptr) {
        m_loadTask = new MusicTask(qApp);

        connect(m_loadTask, &MusicTask::sigFileInfo, this, &MusicWidget::slotDispFileInfo);
        connect(m_loadTask, &MusicTask::sigTaskFinished, this, &MusicWidget::slotFileThreadFinish);
        connect(m_loadTask, &MusicTask::sigRootPath, this, &MusicWidget::slotRootPath);
        connect(m_loadTask, &MusicTask::sigRootPathNotFound, this, &MusicWidget::slotRootPathNotFound);
    }
    m_loadTask->setPathAndType(path, m_devInfo.type, m_devInfo.strPhoneID);
    ThreadService::getService()->startTask(ThreadService::E_Music_Display_Task, m_loadTask);

    d_ptr->m_bIsLoading = true;

    spinnerStart();
}

void MusicWidget::slotDispFileInfo(const QFileInfo &fi)
{
    //    QFileInfo fi = QFileInfo(file.path);

    //解决在加载过程中切换手机导致的文件显示不对问题
    QString parentPath;
    if (m_rootPath.endsWith(QDir::separator()))
        parentPath = fi.absolutePath() + QDir::separator();
    else
        parentPath = fi.absolutePath();

    if (m_rootPath.isEmpty() || !parentPath.startsWith(m_rootPath))
        return;

    m_pPathList.append(fi.absoluteFilePath());

    QVariant data;
    data.setValue(fi);
    m_pUIModelManager->appendData(data);

    //pms40949 音乐电子书加载过程中当有内容显示时可以全选
    auto model = m_pUIModelManager->getTreeModel();
    if (model->rowCount() > 0) {
        m_pTreeViewFrame->setHeaderViewEnable(true);
        // m_treeHeadView->setCheckBoxEnabled(true);
    }
    if (m_pTreeViewFrame->getIsHeaderChecked()) {
        m_pTreeViewFrame->selectAll();
        // m_pTreeViewFrame->pCustomTreeView()->selectAll();
    }
}

void MusicWidget::slotFileThreadFinish()
{
    //多个手机切换时不显示加载转圈问题特殊处理 //ZLL
    if (d_ptr->m_optType != FILE_DISPLAY || d_ptr->m_bIsLoading) {
        spinnerStop();
    }

    qDebug() << __LINE__ << __FUNCTION__;

    setTitleBtnEnble(isItemSelected());

    m_deleteOpt = false;

    if (d_ptr->m_optType == FILE_IMPORT) {
        //d_ptr->closeImportProgress();
        qDebug() << __LINE__ << __FUNCTION__;

        auto selectRows = m_pTreeViewFrame->selectionModel()->selectedRows();
        auto rowCount = m_pTreeViewFrame->model()->rowCount();
        if (rowCount > 0 && selectRows.count() == rowCount) {
            m_pTreeViewFrame->setHeaderViewState(true); //  清除信息， check 为false
            // m_treeHeadView->slotSetChecked(true);
        } else {
            if (!m_deleteOpt)
                m_pTreeViewFrame->setHeaderViewState(false); //  清除信息， check 为false
            // m_treeHeadView->slotSetChecked(false);
        }

        //        if (d_ptr->m_importOkCount > 0) {
        //            startGetInfoTask();

        //            if (!d_ptr->m_importCancel) {
        //                sendOkMessage(TrObject::getInstance()->getMusicImportExportText(song_import_count).arg(d_ptr->m_importOkCount));
        //            } else {
        //                sendOkMessage(TrObject::getInstance()->getMusicImportExportText(song_import_stop).arg(d_ptr->m_importOkCount));
        //            }
        //        }

        if (d_ptr->m_OkCount > 0)
            startGetInfoTask();

        if (d_ptr->m_pImportExportProgressDlg != nullptr) {
            d_ptr->m_importExportCancel = false;
            d_ptr->m_pImportExportProgressDlg->setProgressBarFinish();
            d_ptr->m_pImportExportProgressDlg->hide();
            qDebug() << __LINE__ << __FUNCTION__;
        }

    } else if (d_ptr->m_optType == FILE_DEL) {
        m_pTreeViewFrame->setHeaderViewState(false); //  清除信息， check 为false
        //        m_treeHeadView->slotSetChecked(false);
    } else if (d_ptr->m_optType == FILE_EXPORT) {
        qDebug() << __LINE__ << __FUNCTION__;
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
    } else if (d_ptr->m_optType == FILE_DISPLAY) {
        if (d_ptr->m_bIsLoading)
            spinnerStop();

        d_ptr->m_bIsLoading = false;

        startGetInfoTask();
    }

    auto model = m_pUIModelManager->getTreeModel();
    m_pTreeViewFrame->setHeaderViewEnable(model->rowCount());

    bool rl = m_pTreeViewFrame->selectionModel()->hasSelection();

    slotUpdateSelectData(rl);
}

void MusicWidget::slotDeleteBtnClicked()
{
    qDebug() << __LINE__ << __FUNCTION__;

    if (!this->isVisible() || checkOperating()) {
        return;
    }

    QStringList selectFileList;
    auto selectRows = m_pTreeViewFrame->selectionModel()->selectedRows(NAME_COL);
    foreach (auto item, selectRows) {
        QFileInfo fileInfo = item.data(ROLE_ITEM_DATA).value<QFileInfo>();
        selectFileList.append(fileInfo.absoluteFilePath());
    }

    if (selectFileList.isEmpty()) {
        sendWarnMessage(TrObject::getInstance()->getFileOperationText(Please_select_a_file));
        return;
    }

    QString info;
    if (selectFileList.size() > 1)
        info = TrObject::getInstance()->getFileOperationText(Delete_items);
    else
        info = TrObject::getInstance()->getFileOperationText(Delete_it);

    //弹出删除确认对话框
    bool bRet = execMsgBox(info);
    if (!bRet) //选择了取消
        return;

    //删除过程中禁用全选
    m_pTreeViewFrame->setHeaderViewEnable(false);

    //删除选中的文件
    d_ptr->m_optType = FILE_DEL;

    //  删除任务
    auto task = new DeleteFileTask(qApp);
    task->setPathList(selectFileList);

    connect(task, &DeleteFileTask::sigTaskFinished, this, &MusicWidget::slotFileThreadFinish);
    connect(task, &DeleteFileTask::sigDeleteFileResult, this, &MusicWidget::slotDeleteFileFinish);
    ThreadService::getService()->startTask(ThreadService::E_Music_Delete_Task, task);

    //    FileManageThread *fileThrd = new FileManageThread(m_devInfo);
    //    fileThrd->setPathList(m_rootPath, selectFileList, d_ptr->m_optType, FT_MUSIC);

    //    connect(fileThrd, &FileManageThread::finished, this, &MusicWidget::slotFileThreadFinish);
    //    connect(fileThrd, &FileManageThread::sigFileProcResult, this, &MusicWidget::slotSingleFileProcFinish);
    //    connect(fileThrd, &FileManageThread::finished, fileThrd, &FileManageThread::deleteLater);

    //    fileThrd->start();
    spinnerStart();
    m_deleteOpt = true;

    //m_treeHeadView->slotCancelAllChecked();
}

void MusicWidget::slotDeleteFileFinish(const int &result, const QString &filePath)
{
    if (result == 0) {
        m_pUIModelManager->deleteModelByPath("", filePath);

        bool rl = m_pTreeViewFrame->selectionModel()->hasSelection();
        slotUpdateSelectData(rl);
    } else {
        QFileInfo fi(filePath);
        QString info = TrObject::getInstance()->getFileOperationText(Deletion_failed_count).arg(fi.fileName());

        sendWarnMessage(info);
    }
}

void MusicWidget::slotExportBtnClicked()
{
    if (!this->isVisible() || checkOperating()) {
        return;
    }

    QStringList selectFileList;
    auto selectRows = m_pTreeViewFrame->selectionModel()->selectedRows(NAME_COL);
    foreach (auto item, selectRows) {
        QFileInfo fileInfo = item.data(ROLE_ITEM_DATA).value<QFileInfo>();
        selectFileList.append(fileInfo.absoluteFilePath());
    }

    if (selectFileList.isEmpty()) {
        sendWarnMessage(TrObject::getInstance()->getFileOperationText(Please_select_a_file));
        return;
    }

    //导出选择对话框
    QString exportDstPath = getExportPath();
    if (!exportDstPath.isEmpty()) {
        //检查是否有文件重名
        QStringList replaceFile; //要替换的文件列表
        QStringList coexistFile; //要共存的文件列表
        checkFileRepeatProc_QFile(exportDstPath, selectFileList, replaceFile, coexistFile);

        if (selectFileList.isEmpty())
            return;

        //导出选中的文件
        startImportExportFile(FILE_EXPORT, selectFileList, replaceFile, coexistFile, exportDstPath);
    }
}

void MusicWidget::slotImportBtnClicked()
{
    qDebug() << __LINE__ << __FUNCTION__;

    if (!this->isVisible() || checkOperating()) {
        return;
    }

    //PTP连接方式下，不能导入
    if (m_devInfo.usbType == USB_PTP) {
        sendWarnMessage(TrObject::getInstance()->getMessageText(You_are_using_PTP_connection));
        return;
    }

    QFile filePath(m_rootPath);
    if (!filePath.exists()) {
        sendWarnMessage(TrObject::getInstance()->getMountText(Failed_mount_the_directories));
        return;
    }

    //挂载的根路径下有多个子文件夹，默认导入到第一个子文件夹中，新建PhoneAssistant文件夹
    QDir rootDir(m_rootPath);
    auto fileList = rootDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    if (fileList.isEmpty()) {
        sendWarnMessage(TrObject::getInstance()->getMountText(Failed_mount_the_directories));
        return;
    }

    QString subPath = fileList.at(0).absoluteFilePath();
    m_importDir = subPath + "/" + MUSIC_DIR;
    //创建导入目录 /根目录
    QDir dir(m_importDir);
    if (!dir.exists()) {
        if (!dir.mkpath(m_importDir)) {
            sendWarnMessage(TrObject::getInstance()->getImportExportText(Import_failed));
            qDebug() << __LINE__ << __FUNCTION__ << "mkpath error:" << m_importDir;
            return;
        }
    }

    QString filterStr = FileModel::getMusicFilterStr();
    QStringList selectFileList = DFileDialog::getOpenFileNames(this, "", DStandardPaths::writableLocation(QStandardPaths::DesktopLocation), filterStr);
    if (selectFileList.isEmpty())
        return;

    //检查是否有文件重名
    QStringList replaceFile; //要替换的文件列表
    QStringList coexistFile; //要共存的文件列表
    checkFileRepeatProc(m_importDir, selectFileList, replaceFile, coexistFile);

    if (selectFileList.isEmpty())
        return;

    startImportExportFile(FILE_IMPORT, selectFileList, replaceFile, coexistFile, m_importDir);
}

void MusicWidget::startImportExportFile(const E_File_OptionType &optType, const QStringList &fileList, const QStringList &replaceList, const QStringList &coexistList, const QString &dstPath)
{
    if (fileList.size() == 1)
        m_importExportOneFile = true;
    else
        m_importExportOneFile = false;

    d_ptr->m_OkCount = 0;
    d_ptr->m_ErrCount = 0;
    d_ptr->m_importExportCancel = true; //默认为true,正常结束情况会设为false
    d_ptr->m_optType = optType;

    qDebug() << __LINE__ << __FUNCTION__ << " start.";

    d_ptr->m_importExportFileThrd = new FileManageThread(m_devInfo);
    d_ptr->m_importExportFileThrd->setCopyInfo(m_rootPath, fileList, replaceList, coexistList, dstPath, d_ptr->m_optType, FT_MUSIC);

    connect(d_ptr->m_importExportFileThrd, &FileManageThread::finished, this, &MusicWidget::slotFileThreadFinish);
    connect(d_ptr->m_importExportFileThrd, &FileManageThread::sigFileProcResult, this, &MusicWidget::slotSingleFileProcFinish);
    connect(d_ptr->m_importExportFileThrd, &FileManageThread::sigFileRepeatConfirm, this, &MusicWidget::slotFileRepeatConfirm);
    connect(d_ptr->m_importExportFileThrd, &FileManageThread::sigProgressRefresh, d_ptr, &CustomWidgetPrivate::slotUpdateImportExportProgressValue);

    d_ptr->m_importExportFileThrd->start();

    QString titleinfo;
    if (optType == FILE_IMPORT)
        titleinfo = TrObject::getInstance()->getDlgTitle(dlg_Importing);
    else if (optType == FILE_EXPORT)
        titleinfo = TrObject::getInstance()->getDlgTitle(dlg_Exporting);

    d_ptr->m_pImportExportProgressDlg = new ImportExportProgressDlg(titleinfo, this);
    if (fileList.count() > 1)
        d_ptr->m_pImportExportProgressDlg->setProgressBarRange(0, fileList.count());
    else
        startFakeProcessBarUpdateProc(); //只有一个文件时，使用假的进度

    d_ptr->m_pImportExportProgressDlg->exec();
    d_ptr->closeImportExportProgress(E_Widget_Music);

    if (m_timer != nullptr && m_timer->isActive()) {
        m_timer->stop();
        delete m_timer;
        m_timer = nullptr;
    }

    qDebug() << __LINE__ << __FUNCTION__ << " end.";
}

void MusicWidget::createNewFileItem(QString path, QFileInfo file_info, QString fileName)
{
    Q_UNUSED(fileName);
    Q_UNUSED(file_info);
    qDebug() << __LINE__ << __FUNCTION__ << path;

    QFileInfo file(path);
    //    file_info.name = file.completeBaseName();
    QFileInfo tempFi(m_importDir + "/" + file.fileName());

    m_pPathList.append(tempFi.absoluteFilePath());

    QVariant var;
    var.setValue(tempFi);

    m_pUIModelManager->appendData(var);
}

void MusicWidget::slotSingleFileProcFinish(int optType, int result, QString file, bool newFileItem, QFileInfo file_info)
{
    QFileInfo fileInfo(file);
    QString info;
    if (optType == FILE_IMPORT) {
        if (result == 0) {
            //在当前界面新建文件显示
            if (newFileItem) {
                createNewFileItem(file, file_info);
                calculateSelectCountSize(); //更新总数
                //m_count++;
                //m_totalSize += fileInfo.size();
                m_pTreeViewFrame->setHeaderViewState(false);
                //                m_treeHeadView->slotSetChecked(false);
            } else {
                updateItemInfoByFile(file);
            }
            d_ptr->m_OkCount++;
        } else {
            if (result != -2) { //-2是导入中止，不显示导入失败
                info = TrObject::getInstance()->getImportExportText(Import_failed_count).arg(fileInfo.fileName());
            }
        }

    } else if (optType == FILE_EXPORT) {
        if (result == 0) {
            d_ptr->m_OkCount++;
        } else {
            if (result != -2) { //-2是导入中止，不显示导入失败
                d_ptr->m_ErrCount++;
                info = TrObject::getInstance()->getImportExportText(Export_failed_count).arg(fileInfo.fileName());
            }
        }
    }

    if (result != 0 && result != -2)
        sendWarnMessage(info);
}

void MusicWidget::setTitleBtnEnble(bool value)
{
    if (!this->isVisible())
        return;

    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Export_Btn, value);
    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Delete_Btn, value);
}

bool MusicWidget::isItemSelected()
{
    return m_pTreeViewFrame->selectionModel()->hasSelection();
}

void MusicWidget::slotFileRepeatConfirm(QString fileName, QString srcPath, QString dstPath)
{
    FileManageThread *fileThread = nullptr;
    if (d_ptr->m_optType == FILE_EXPORT || d_ptr->m_optType == FILE_IMPORT) {
        fileThread = d_ptr->m_importExportFileThrd;
        RemovePathPrefix(srcPath, Mount_Android, m_devId);
    }
    //    } else if (d_ptr->m_optType == FILE_IMPORT) {
    //        fileThread = d_ptr->m_importExportFileThrd;
    //        Utils::RemovePathPrefix(dstPath, Mount_Android, m_devId);
    //    }

    //暂停更新假进度条
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

    //开始更新假进度条
    if (timerRestart && m_timer != nullptr) {
        m_timer->start();
    }
}

void MusicWidget::calculateSelectCountSize()
{
    auto model = m_pUIModelManager->getTreeModel();
    int count = model->rowCount();
    QString txt = "";

    if (m_pTreeViewFrame->selectionModel()->hasSelection()) {
        int nSelectCount = 0;
        quint64 selectSize = static_cast<MusicBookTreeView *>(m_pTreeViewFrame)->getSelectSize(nSelectCount);

        QString strSize = Utils::dataSizeToString(quint64(selectSize));
        txt = TrObject::getInstance()->getFileOperationText(Total_item_selected_size).arg(count).arg(nSelectCount).arg(strSize);
    } else {
        QString strSize = Utils::dataSizeToString(quint64(model->getTotalSize()));
        txt = TrObject::getInstance()->getFileOperationText(items_size).arg(count).arg(strSize);
    }
    setShowText(txt);
}

void MusicWidget::startGetInfoTask()
{
    auto task = new MusicInfoTask(qApp);
    task->setData(m_pPathList);
    connect(task, &MusicInfoTask::sigMusicInfoLoaded, this, [=](const QString &path, const QString &length) {
        QJsonObject obj;
        obj.insert("path", path);
        obj.insert("length", length);

        m_pUIModelManager->updateDataPixmap(obj, QPixmap());
    });

    //任务结束处理信号
    connect(task, &MusicInfoTask::sigTaskFinished, this, [=]() {
        m_infoTask = nullptr; //结束把指针设为空
    });

    ThreadService::getService()->startTask(ThreadService::E_Music_Info_Task, task);
    m_infoTask = task; //保存指针

    m_pPathList.clear();
}

void MusicWidget::setWidgetBtnState()
{
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Left_Btn, false);
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Right_Btn, false);
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_New_Folder_Btn, false);
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Export_Btn, true);
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Import_Btn, true);
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Delete_Btn, true);
    RightTitleWidget::getInstance()->setWidgetBtnVisible(RightTitleWidget::E_Icon_View_Btn, false);
    RightTitleWidget::getInstance()->setWidgetBtnVisible(RightTitleWidget::E_List_View_Btn, false);

    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Import_Btn, true);

    bool rl = m_pTreeViewFrame->selectionModel()->hasSelection();
    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Export_Btn, rl);
    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Delete_Btn, rl);
}

void MusicWidget::clearWidgetInfo(const QString &devInfo)
{
    if (devInfo == m_devId) {
        m_rootPath = "";
        m_devId = "";

        //拔手机时可能存在崩溃问题，此处不再删除Model数据
        //m_pTreeViewFrame->clearModel();
    }
}

void MusicWidget::refreshWidgetData()
{
    //    m_treeHeadView->slotCancelAllChecked();
    if (checkOperating())
        return;

    m_pUIModelManager->clearModel();
    m_pTreeViewFrame->setHeaderViewState(false);

    getDataFromDevice(m_rootPath);
}

void MusicWidget::slotRootPath(QString path)
{
    if (m_rootPath.isEmpty()) {
        m_rootPath = path;
    }

    qDebug() << __LINE__ << __FUNCTION__ << "RootPath:" << path;
}

void MusicWidget::slotRootPathNotFound()
{
    m_rootPath = "";
    sendWarnMessage(TrObject::getInstance()->getMountText(Failed_mount_the_directories));
    //emit sigFindCount(m_devId, m_count);
    calculateSelectCountSize();
}

void MusicWidget::slotTitleWidgetBtnClicked(const int &nId)
{
    if (this->isVisible() == false)
        return;

    if (nId == TitleWidget::E_Export_Btn) {
        slotExportBtnClicked();
    } else if (nId == TitleWidget::E_Import_Btn) {
        slotImportBtnClicked();
    } else if (nId == TitleWidget::E_Delete_Btn) {
        slotDeleteBtnClicked();
    }
}

void MusicWidget::slotUpdateSelectData(const bool &rl)
{
    if (this->isVisible() == true) {
        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Delete_Btn, rl);
        TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Export_Btn, rl);
    }

    auto selectRows = m_pTreeViewFrame->selectionModel()->selectedRows();
    auto rowCount = m_pTreeViewFrame->model()->rowCount();
    if (rowCount > 0 && selectRows.count() == rowCount) {
        m_pTreeViewFrame->setHeaderViewState(true);
        // m_treeHeadView->slotSetChecked(true);
    } else {
        if (!m_deleteOpt)
            m_pTreeViewFrame->setHeaderViewState(false);
        // m_treeHeadView->slotSetChecked(false);
    }

    calculateSelectCountSize();
}

void MusicWidget::slotSelectAll(const bool &rl)
{
    slotUpdateSelectData(rl);
}

/**
 * @brief 导入导出只有一个文件时，进度条不显示进度，使用假数据更新进度条
 * @param void
 */
void MusicWidget::startFakeProcessBarUpdateProc()
{
    m_processBarValue = 0;
    d_ptr->m_pImportExportProgressDlg->setProgressBarRange(0, 100);

    if (m_timer == nullptr) {
        m_timer = new QTimer(this);
    }

    connect(m_timer, &QTimer::timeout, this, &MusicWidget::slotUpdateProcessBar);
    m_timer->setInterval(1000);
    m_timer->start();
}

void MusicWidget::slotUpdateProcessBar()
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

void MusicWidget::checkFileRepeatProc(const QString &dstPath, QStringList &srcFileList, QStringList &replaceList, QStringList &coexistList)
{
    replaceList.clear();
    coexistList.clear();

    int confirmRust = -1;
    bool notAsk = false;

    //PMS43852 有些音乐文件无法正常判断是否存在 改成ls 全部文件 ，再过滤指定文件名是否存在
    //不能直接ls 或 find文件名 QFile::exists ，存在已删除文件可以检出问题
    //QString s = QString("find \"%1\" -name \"%2\"").arg(dstDir).arg(fileName);
    QString cmd = QString("ls \"%1\"").arg(dstPath);
    QString sRes = Utils::execCmdNoResult(cmd);
    QStringList fileList = sRes.split("\n"); //分解出文件名

    foreach (QString strItemFile, srcFileList) {
        QString fileName = Utils::getFileNameByPath(strItemFile);
        if (fileList.contains(fileName)) {
            //文件存在
            if (!notAsk) {
                FileRepeatConfirmDlg *confirmDlg = new FileRepeatConfirmDlg(fileName, "", "", false, this, false);
                //最后一个文件时，不显示应用到全部
                if (srcFileList.size() == 1 || (srcFileList.size() - replaceList.size() - coexistList.size() == 1))
                    confirmDlg->hideCheckBox();

                confirmRust = confirmDlg->exec();
                notAsk = confirmDlg->getAskCheckBoxValue();
                delete confirmDlg;
            }

            if (confirmRust == 0) { //跳过
                //从文件列表中移除
                srcFileList.removeOne(strItemFile);
            } else if (confirmRust == 1) { //替换
                replaceList.append(strItemFile);
            } else if (confirmRust == 2) { //共存
                coexistList.append(strItemFile);
            } else if (confirmRust == -1) { //关闭对话框
                //用户直接 关闭确认框时按该文件跳过处理
                //从文件列表中移除
                srcFileList.removeOne(strItemFile);
            }
        }
    }
}

void MusicWidget::checkFileRepeatProc_QFile(const QString &dstPath, QStringList &srcFileList, QStringList &replaceList, QStringList &coexistList)
{
    //检查文件重复处理，使用QFile的方式

    replaceList.clear();
    coexistList.clear();

    int confirmRust = -1;
    bool notAsk = false;

    foreach (QString strItemFile, srcFileList) {
        QFileInfo fileInfo(strItemFile);
        QString fileName = fileInfo.fileName();
        QString dstFile = dstPath + "/" + fileName;

        if (QFile::exists(dstFile)) {
            //文件存在
            if (!notAsk) {
                FileRepeatConfirmDlg *confirmDlg = new FileRepeatConfirmDlg(fileName, "", "", false, this, false);
                //最后一个文件时，不显示应用到全部
                if (srcFileList.size() == 1 || (srcFileList.size() - replaceList.size() - coexistList.size() == 1))
                    confirmDlg->hideCheckBox();

                confirmRust = confirmDlg->exec();
                notAsk = confirmDlg->getAskCheckBoxValue();
                delete confirmDlg;
            }

            if (confirmRust == 0) { //跳过
                //从文件列表中移除
                srcFileList.removeOne(strItemFile);
            } else if (confirmRust == 1) { //替换
                replaceList.append(strItemFile);
            } else if (confirmRust == 2) { //共存
                coexistList.append(strItemFile);
            } else if (confirmRust == -1) { //关闭对话框
                //用户直接 关闭确认框时按该文件跳过处理
                //从文件列表中移除
                srcFileList.removeOne(strItemFile);
            }
        }
    }
}

void MusicWidget::updateItemInfoByFile(const QString &path)
{
    //获得图标
    //    QPixmap picIcon;
    QFileInfo file_info(path);

    QString tmp_fileName = file_info.fileName();

    QString sTempPath = m_importDir + "/" + tmp_fileName;

    QFileInfo tempFi = QFileInfo(sTempPath);
    QVariant var;
    var.setValue(tempFi);
    m_pUIModelManager->updateData(tmp_fileName, var);

    //解决列表模式下选中行，在导入替换时选中截断问题
    if (m_pTreeViewFrame->selectionModel()->hasSelection()) {
        m_pTreeViewFrame->setCurrentIndex(m_pTreeViewFrame->currentIndex());
    }
}
