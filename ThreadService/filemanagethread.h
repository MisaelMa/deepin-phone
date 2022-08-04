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
#ifndef FILEMANAGETHREAD_H
#define FILEMANAGETHREAD_H

#include "defines.h"

#include "PMThread.h"
#include <QDir>
#include <QWaitCondition>
#include <QMutex>
#include <QProcess>

class FileManageThread : public PMThread
{
    Q_OBJECT

public:
    FileManageThread(const PhoneInfo &devInfo, QObject *parent = nullptr);
    virtual ~FileManageThread() override;

    enum FileRepeatType {
        FR_SKIP,
        FR_REPLACE,
        FR_COEXIST,
        FR_NOTFIND
    };

    void setPath(const QString &rootPath, const QString &path, const E_File_OptionType &optType, const FileType &fileType);
    void setPathList(const QString &rootPath, const QStringList &pathList, const E_File_OptionType &optType, const FileType &fileType);
    void setCopyInfo(const QString &rootPath, const QStringList &srcFileList, const QStringList &replaceList, const QStringList &coexistList, const QString &dstDir, const E_File_OptionType &optType, const FileType &fileType);
    void setCopyInfo(const QString &rootPath, const QString &srcFile, const QString &dstFile, const E_File_OptionType &optType, const FileType &fileType);

    void setExit();

    void setFileRepeatType(int type, bool notAsk)
    {
        m_fileRepeatType = type;
        m_notAsk = notAsk;
    }

    //线程锁住
    bool lock(); //线程锁住
    bool unlock(); //线程解锁
    //检查线程是否可继续运行
    void checkRunabled();

protected:
    void getDirFileInfo(QString path);
    void getAllFileList(QString path); //读取目录下文件列表

    void createFile(QString path);
    void createFile();

    void copyFileOrDir(); //复制文件或目录
    void copyFile_r();
    void copyFile_one(QString srcFile, QString dstDir); //复制一个文件
    void copyFunc_cmd(QString srcPath, QString dstPath); //使用cp命令复制文件或目录
    void copyFunc_lowAndroid(QString srcPath, QString dstPath);
    void readWriteFile_C(QString srcFile, QString dstDir); //读定心文件内容方式生成新文件

    void copyFileOnly(); //只复制文件
    void importMusic();
    void importFile();
    void importMusic_NoCheck();
    void importFile_NoCheck();
    void copyFileOnly_NoCheck();
    void copyFile_one_NoCheck(QString srcFile, QString dstDir); //复制一个文件
    void copyFileOrDir_NoCheck(); //复制文件或目录

    void adbImportMusic();
    void adbImportFile();
    void adbExportMusicEbook();
    //    void adbDeleteFile();
    void adbImportFileOrDir();
    void adbImportFile_one(QString srcFile, QString dstDir);
    void adbPushFile(QString mountPath, QString srcPath, QString dstPath);
    void adbExportFileOrDir();
    void adbExportFile_one(QString srcFile, QString dstDir);
    void adbPullFile(QString mountPath, QString srcPath, QString dstPath);
    int adbPush(QString devId, QString srcPath, QString dstPath_adb);
    int adbPull(QString devId, QString srcPath_adb, QString dstPath);
    void adbImportMusic_NoCheck();
    void adbImportFile_NoCheck();
    void adbExportMusicEbook_NoCheck();
    void adbExportFile_one_NoCheck(QString srcFile, QString dstDir);

    int getDirAllFielCount(QString path); //获取一文件夹下的所有文件数
    void getAllFileIcon(); //获取文件类型图标
    void getRootPath(); //获取根目录

signals:
    void sigProgressRefresh(int optType, int curValue);
    void sigFileProcResult(int optType, int result, QString file, bool newItem, QFileInfo file_info);
    void sigProgressRefreshFileName(int optMode, QString fileName);
    void sigFileRepeatConfirm(QString fileName, QString srcPath, QString dstPath);
    void sigDirFileInfo(int itemCount, qint64 fileSize, QStringList pathList);
    void sigAllFileCount(int count);
    void sigFileInfo(FILE_INFO file_info);
    void sigRootPath(QString path); //返回挂载目录
    void sigRootPathNotFound(); //挂载目录没找到

protected:
    void run() override;

protected:
    PhoneInfo m_devInfo;
    QString m_rootPath;
    QString m_path;
    QString m_cmd;
    QStringList m_cmdArgs;
    QStringList m_srcFileList;
    QStringList m_replaceList;
    QStringList m_coexistList;
    QString m_dstDirPath;
    E_File_OptionType m_optType;
    FileType m_fileType;
    QStringList m_fileList;
    QString m_srcFile;
    QString m_dstFile;
    int m_fileRepeatType;
    bool m_notAsk; //不再询问
    bool m_firstDirItem;
    int m_dirItemCount;
    qint64 m_dirFileSize;
    int m_iDirsItemCount;
    qint64 m_iDirsFileSize;
    int m_procCount;
    bool m_newFileItem;
    QStringList m_pathTmpList;
    QProcess *m_process;
    QMutex m_mutex_read; //是否可加载文件锁
    bool m_bLocked; //是否要加锁
};

#endif // FILEMANAGETHREAD_H
