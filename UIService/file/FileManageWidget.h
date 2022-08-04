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
#ifndef FILEMANAGEWIDGET_H
#define FILEMANAGEWIDGET_H

#include "defines.h"

#include <DLabel>
#include <DFrame>
#include <DFloatingWidget>
#include <QItemSelection>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QTimer>

#include "widget/CustomWidget.h"

DWIDGET_USE_NAMESPACE

class FileManageThread;
class BaseEngineThread;
class FileManageWidget : public CustomWidget
{
    Q_OBJECT

public:
    explicit FileManageWidget(DWidget *parent = nullptr);
    ~FileManageWidget() override;

    void updateWidgetInfo(const PhoneInfo &info) override;

signals:
    void sigNameEdit(QString name);

    // CustomWidget interface
public:
    void clearWidgetInfo(const QString &devInfo) override;
    void setWidgetBtnState() override;

public slots:
    void slotSingleFileProcFinish(int optType, int result, QString file, bool newFileItem, QFileInfo file_info);

    // CustomWidget interface
protected:
    void refreshWidgetData() override;

    // 是否开启读取线程 QThread::isRunning
    virtual bool isReading() override;
    // 暂停读取
    virtual bool pauseRead() override;
    // 继续读取
    virtual bool continueRead() override;

private slots:
    void slotCustomContextMenuRequested(const QPoint &) override;

    void slotFileItemEditFinish(QString name);

    //void slotDispFileInfo(const QFileInfo &fi);
    void slotDispFileInfo(FILE_INFO file_info); //文件显示
    void slotFileThreadFinish();

    void slotCreateNewFile();
    void slotExportBtnClicked();
    void slotImportBtnClicked();
    void slotDeleteBtnClicked();
    void slotIconViewBtnClicked();
    void slotListViewBtnClicked();
    //    void slotDTreeViewItemDoubleClick(const QModelIndex &index);
    //    void slotDListViewItemDoubleClick(const QModelIndex &index);
    void slotDListViewItemClicked(const QModelIndex &index);
    void slotDTreeViewItemClicked(const QModelIndex &index);

    void slotCleanSelectItem();

    void slotCopyFile();
    void slotPasteFile();
    void slotDeleteFile();
    void slotRefresh();

    void slotFileRepeatConfirm(QString fileName, QString srcPath, QString dstPath);

    void slotNameEdit(QString name);
    void slotDirFileInfo(int itemCount, qint64 fileSize, QStringList pathList);
    void SlotDirsFileInfo(int itemCount, qint64 fileSize, QStringList pathList);
    void slotRootPath(QString path);
    void slotRootPathNotFound();

    /**
     * @brief 标题栏 按钮点击
     */
    void slotTitleWidgetBtnClicked(const int &) override;

    void slotSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void slotUpdateSelectData(const bool &);
    void slotItemRefresh();

    void getDirFile(const QString &path);
    /**
     * @brief 删除文件
     * @param result
     * @param filePath
     */
    void slotDeleteFileFinish(const int &result, const QString &filePath);
    void slotUpdateProcessBar();
    void slotUpdateImage(const QString &path, const QPixmap &data);
    void slotGetImageThreadFinish();
    void slotCreateFileThreadFinish();
    void slotGetDirFileCount();

private:
    void initUI();
    void initConnection();

    void initRootIconViewUI();
    void initRootListViewUI();
    void initEmptyWidget();

    void dispRootPathData();

    void startSpinner();
    void stopSpinner();
    void createNewFileOnDevice(QString path);
    void createNewFileItem(QString path, QFileInfo file_info, QString fileName = QString());
    void forwardBtnBoxClicked();
    void backwardBtnBoxClicked();
    void navigationBtnEnabled();
    void createNewFile(QString name = QString());

    void deleteFile();
    void exportFile();
    void importFile();
    void deleteItemFromUI(const QString &name, const QString &path);

    void getFileListToDisp(QString path);
    void getFileListFromDevice(int type, QString path);
    void addPathToHistoryList(QString path);
    void delPathFromHistoryList(QString path);
    void getDirFileInfo(QString path);

    void createFile(QString fileName);

    //    void dispFileItemIconModel(const QFileInfo &file);
    //    void dispFileItemListModel(const QFileInfo &file);
    void setFileViewBtnIcon();

    void setTitleBtnEnble(bool value);
    void setTitleBtnEnble();
    void setNewDelBtnEnable();

    bool checkCanLoadFile();
    void calculateSelectCountSize() override;

    void GetDirsFileInfo();
    void updateItemInfoByFile(const QString &);
    void updateSelectFileInfo();

    void setAllTitleBtnEnabled(bool value);
    void startImportExportFile(const E_File_OptionType &optType, const QStringList &fileList, const QStringList &replaceList, const QStringList &coexistList, const QString &dstPath);

    /**
     * @brief 开启获取图片任务
     */
    void startGetItemImageTask();
    void getFileIconAdvance(); //通过线程预先获取文件图标
    void startFakeProcessBarUpdateProc();

    void checkFileRepeatProc(const QString &dstPath, QStringList &srcFileList, QStringList &replaceList, QStringList &coexistList);
    void checkFileRepeatProc_QFile(const QString &dstPath, QStringList &srcFileList, QStringList &replaceList, QStringList &coexistList);

private:
    QString m_devId;
    PhoneInfo m_devInfo;

    QString m_currPath;
    QStringList m_currPathList;
    int m_currType;
    QList<QString> m_pHistoryPathList;
    int m_currentPathIndex;

    QModelIndex m_editNameIndex;

    FileManageThread *m_copyFileThrd;
    QString m_rootPath;

    bool m_newNameRepeat;

    QStringList m_pImageList;
    QStringList m_pVideoList;
    QString m_lastPasteFile;

    QTimer *m_timer;
    int m_processBarValue;
    BaseEngineThread *m_imageFileThrd = nullptr;
    BaseEngineThread *m_videoImgFileThrd = nullptr;
    bool m_importExportOneFile;
    QTimer *m_dirFileTimer;
    QTime m_lastSelChgTime;
    QString m_currSelPath;
    FileManageThread *m_dispFileThrd; //加载文件线程
};

#endif // FILEMANAGEWIDGET_H
