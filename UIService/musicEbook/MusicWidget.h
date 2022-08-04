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
#ifndef MUSICWIDGET_H
#define MUSICWIDGET_H

#include "defines.h"

#include <DWidget>
#include <DLabel>

#include <QFileInfo>

#include "widget/CustomWidget.h"

DWIDGET_USE_NAMESPACE

class MusicInfoTask;

class MusicWidget : public CustomWidget
{
    Q_OBJECT
public:
    explicit MusicWidget(DWidget *parent = nullptr);
    ~MusicWidget() override;

    void updateWidgetInfo(const PhoneInfo &info) override;

    // CustomWidget interface
public:
    void setWidgetBtnState() override;
    void clearWidgetInfo(const QString &devInfo) override;

protected:
    void refreshWidgetData() override;

    // 是否开启读取线程 QThread::isRunning
    virtual bool isReading() override;
    // 暂停读取
    virtual bool pauseRead() override;
    // 继续读取
    virtual bool continueRead() override;

private slots:
    void slotDispFileInfo(const QFileInfo &file);
    void slotFileThreadFinish();
    void slotExportBtnClicked();
    void slotImportBtnClicked();
    void slotDeleteBtnClicked();

    /**
     * @brief 删除文件
     * @param result
     * @param filePath
     */
    void slotDeleteFileFinish(const int &result, const QString &filePath);
    void slotSingleFileProcFinish(int optType, int result, QString file, bool newFileItem, QFileInfo file_info);
    void slotFileRepeatConfirm(QString fileName, QString srcPath, QString dstPath);
    void slotRootPath(QString path);
    void slotRootPathNotFound();

    void slotTitleWidgetBtnClicked(const int &) override;

    /**
     * @brief tree 列表选中状态变化处理
     */
    void slotUpdateSelectData(const bool &);

    void slotSelectAll(const bool &);
    void slotUpdateProcessBar();

private:
    void initUI();
    void initConnect();

    void getDataFromDevice(const QString &path);
    void createNewFileItem(QString path, QFileInfo file_info, QString fileName = QString());
    void updateItemInfoByFile(const QString &path);

    void setTitleBtnEnble(bool value);
    bool isItemSelected();

    void calculateSelectCountSize() override;
    void startImportExportFile(const E_File_OptionType &optType, const QStringList &fileList, const QStringList &replaceList, const QStringList &coexistList, const QString &dstPath);
    void startFakeProcessBarUpdateProc();

    /**
     * @brief 开启音乐 时间获取任务
     */
    void startGetInfoTask();

    void checkFileRepeatProc(const QString &dstPath, QStringList &srcFileList, QStringList &replaceList, QStringList &coexistList);
    void checkFileRepeatProc_QFile(const QString &dstPath, QStringList &srcFileList, QStringList &replaceList, QStringList &coexistList);

private:
    QString m_rootPath;
    QString m_importDir;
    QString m_devId;
    PhoneInfo m_devInfo;

    bool m_deleteOpt = false;
    QTimer *m_timer;
    int m_processBarValue;

    QStringList m_pPathList;
    bool m_importExportOneFile;
    MusicInfoTask *m_infoTask; //音乐时长
};

#endif // MUSICWIDGET_H
