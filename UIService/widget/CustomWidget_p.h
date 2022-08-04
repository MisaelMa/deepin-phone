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
#ifndef CUSTOMWIDGET_P_H
#define CUSTOMWIDGET_P_H

#include <QObject>

#include "defines.h"

//class PhoneFileThread;
class ImportProgressWgt;
class FileManageThread;
//class ExportProgressDlg;
class ImportExportProgressDlg;

class CustomWidget;
class CustomWidgetPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(CustomWidget)
public:
    explicit CustomWidgetPrivate(QObject *parent = nullptr);

public:
    //    void setProgressDlgInfo(E_File_OptionType optType);
    //    void showImportProgress();
    //    void closeImportProgress();

    //    void closeExportProgress(const E_Widget_Type &);
    void closeImportExportProgress(const E_Widget_Type &);

public slots:
    //    void slotUpdateImportFileName(int optType, QString fileName);
    //    void slotUpdateExportProgressValue(int optType, int curValue);
    void slotUpdateImportExportProgressValue(int optType, int curValue);

private slots:
    //    void slotCancelImport();

private:
    void moveImportDlg();
    //    void sendExportMessage(const E_Widget_Type &);
    void sendImportExportMessage(const E_Widget_Type &);

public:
    //    bool m_importCancel = false;
    //    int m_importOkCount = 0;
    ImportProgressWgt *m_pImportProgress = nullptr;
    //    FileManageThread *m_importFileThrd = nullptr;
    ImportExportProgressDlg *m_pImportExportProgressDlg = nullptr;
    FileManageThread *m_importExportFileThrd = nullptr;

    //    int m_exportOkCount = 0;
    //    int m_exportErrCount = 0;
    int m_OkCount = 0;
    int m_ErrCount = 0;
    //    bool m_exportCancel = false;
    bool m_importExportCancel = false;
    //    FileManageThread *m_exportFileThrd = nullptr;
    //    ExportProgressDlg *m_pExportProgressDlg = nullptr;

    E_File_OptionType m_optType = File_Undefine;

    //  数据是否正在查询 加载
    bool m_bIsLoading = false;

private:
    CustomWidget *q_ptr;
};

#endif // CUSTOMWIDGET_P_H
