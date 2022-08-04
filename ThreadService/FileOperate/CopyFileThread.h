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
#ifndef COPYFILETHREAD_H
#define COPYFILETHREAD_H

#include "PMThread.h"
#include "defines.h"

#include <QFileInfo>

class CopyFileThread : public PMThread
{
    Q_OBJECT
public:
    explicit CopyFileThread(QObject *parent = nullptr);
    //        DelFile = 0, //删除文件
    enum E_Copy_Type {
        ExportFile, //导出文件
        ImportFile, //导入文件
    };

signals:
    void sigCopyProgress(int nCur, int nTotal, QString strFileName); // 操作进度(拷贝)
    void sigCopyResult(int nRunMode, int nCount, int nSuccess, int nFailed, bool bIsStop); // 结果

public:
    void copyFile(const QStringList &listFileNames, const QString &strDesPath, const int &mode,
                  const PhoneFileType &type, const QString &strSysVersion, const QString &strRootPath,
                  const QString strPhoneID); //导入/导出
    // QThread interface
protected:
    void run() override;

private:
    void _copyFile();
    bool _copyFile_one_auto(const QFileInfo &source, const QString &desPath);
    bool _copyFile_one_qfile(const QString &source, const QString &destination);
    bool _copyFile_one_iof(const QString &source, const QString &destination);
    bool _copyFile_one_f(const QString &source, const QString &destination);
    bool _copyFile_one_adb(const QString &source, const QString &destination);

private:
    E_Copy_Type m_nType = ExportFile;

    QStringList m_listFileNames; //操作多个文件
    QStringList m_listFilters;

    QString m_strSysVersion = ""; //手机系统版本
    QString m_strDesPath = ""; //多个文件指向目录
    QString m_strRootPath = ""; // 安卓用 rootpath
    QString m_strPhoneID = ""; //查询某个手机里的数据
};

#endif // COPYFILETHREAD_H
