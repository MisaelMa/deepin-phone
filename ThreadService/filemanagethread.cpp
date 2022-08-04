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
#include "filemanagethread.h"
#include "defines.h"
#include "utils.h"

#include <QFile>
#include <QDir>
#include <QDebug>
#include <QDateTime>
#include <QProcess>
#include <QTextCodec>
#include <QImageReader>
#include <QUrl>

#include <iostream>
#include <fstream>
#include <QTemporaryFile>

#include <QApplication>
#include <stdio.h>

#include "Model.h"
#include "DataService.h"
#include "FileModel.h"

#include "MountService.h"
#include "FileUtils.h"

extern "C" {
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
}

#define MAX_BUF (1024 * 1024)

static QMutex g_mutex;
QWaitCondition g_fileProcWait;
extern QMutex g_enterDirMutex; //全局加载文件时读取目录信息用

FileManageThread::FileManageThread(const PhoneInfo &devInfo, QObject *parent)
    : PMThread(parent)
{
    m_devInfo = devInfo;

    m_notAsk = false;
    m_procCount = 0;
    m_newFileItem = false;
    m_process = nullptr;
    m_bLocked = false;
    m_isCanRun = true;
}

FileManageThread::~FileManageThread()
{
    qDebug() << "~FileManageThread()";

    if (m_process != nullptr) {
        if (m_process->isOpen())
            m_process->kill();
        delete m_process;
        m_process = nullptr;
    }

    //线程解锁
    m_bLocked = false;
    m_mutex_read.unlock();
}

void FileManageThread::run()
{
    if (m_rootPath.isEmpty())
        getRootPath();

    switch (int(m_optType)) {
    case FILE_DISPLAY:
        if (!m_path.isEmpty()) {
            getAllFileList(m_path);
        } else {
            getAllFileList(m_rootPath);
        }
        break;
    case FILE_IMPORT:
        qDebug() << __LINE__ << "import file start.";
        m_newFileItem = false;
        if (m_devInfo.type == Mount_Android) {
            bool isLowAndroid = Utils::isLowVersionAndroid(m_devInfo.strProductVersion);
            if (isLowAndroid) {
                if (m_fileType == FT_MUSIC) {
                    //importMusic();
                    importMusic_NoCheck();
                } else {
                    //importFile();
                    importFile_NoCheck();
                }
            } else {
                if (m_fileType == FT_MUSIC) {
                    //adbImportMusic();
                    adbImportMusic_NoCheck();
                } else {
                    //adbImportFile();
                    adbImportFile_NoCheck();
                }
            }
        } else {
            //copyFileOnly();
            copyFileOnly_NoCheck();
        }
        qDebug() << __LINE__ << "import file end.";
        break;
    case FILE_EXPORT:
        qDebug() << __LINE__ << "export file start.";
        m_newFileItem = false;
        if (m_devInfo.type == Mount_Android) {
            bool isLowAndroid = Utils::isLowVersionAndroid(m_devInfo.strProductVersion);
            if (isLowAndroid) {
                if (m_fileType == FT_MUSIC || m_fileType == FT_EBOOK) {
                    //copyFileOnly();
                    copyFileOnly_NoCheck();
                } else { //导出文件或文件夹
                    copyFile_r();
                }
            } else {
                if (m_fileType == FT_MUSIC || m_fileType == FT_EBOOK) {
                    //adbExportMusicEbook();
                    adbExportMusicEbook_NoCheck();
                } else { //导出文件或文件夹
                    adbExportFileOrDir();
                }
            }
        } else {
            copyFile_r();
        }
        qDebug() << __LINE__ << "export file end.";
        break;
    case FILE_PASTE:
        qDebug() << __LINE__ << "paste file start.";
        //copyFileOrDir(); //粘贴使用命令，可直接粘贴从系统复制的文件夹
        copyFileOrDir_NoCheck(); //粘贴使用命令，可直接粘贴从系统复制的文件夹
        qDebug() << __LINE__ << "paste file end.";
        break;
    case FILE_NEW:
        qDebug() << __LINE__ << "create file start.";
        createFile();
        qDebug() << __LINE__ << "create file end.";
        break;
    case FILE_DIRINFO:
        m_dirItemCount = 0;
        m_dirFileSize = 0;
        m_firstDirItem = true;
        m_pathTmpList = QStringList() << m_path;
        getDirFileInfo(m_path);
        //返回目录信息
        emit sigDirFileInfo(m_dirItemCount, m_dirFileSize, m_pathTmpList);
        break;
    case FILE_DIRSINFO:
        m_iDirsItemCount = 0;
        m_iDirsFileSize = 0;
        m_pathTmpList = m_fileList;
        for (auto listIt = m_fileList.begin(); listIt != m_fileList.end(); ++listIt) {
            if (!m_isCanRun)
                return;

            m_firstDirItem = true;
            getDirFileInfo(*listIt);
        }
        //返回目录信息
        emit sigDirFileInfo(m_iDirsItemCount, m_iDirsFileSize, m_fileList);
        break;
    case FILE_ICON:
        getAllFileIcon();
        break;
    }
}

void FileManageThread::createFile()
{
    QFileInfo file_info;
    int ret = 0;
    if (m_fileType == FT_DIR) {
        QDir dir(m_path);
        if (dir.exists()) {
            ret = -1;
        } else if (!dir.mkdir(m_path)) {
            ret = -1;
        }
    } else {
        QFile fileName(m_path);
        if (fileName.exists()) {
            ret = -1;
        } else if (!fileName.open(QIODevice::WriteOnly)) {
            ret = -1;
        } else {
            fileName.close();
        }
    }

    emit sigFileProcResult(FILE_NEW, ret, m_path, 1, file_info);
}

void FileManageThread::setPath(const QString &rootPath, const QString &path, const E_File_OptionType &optType,
                               const FileType &fileType)
{
    m_rootPath = rootPath;
    m_path = path;
    m_optType = optType;
    m_fileType = fileType;
}

void FileManageThread::setPathList(const QString &rootPath, const QStringList &pathList, const E_File_OptionType &optType,
                                   const FileType &fileType)
{
    m_rootPath = rootPath;
    m_fileList = pathList;
    m_optType = optType;
    m_fileType = fileType;
}

void FileManageThread::setCopyInfo(const QString &rootPath, const QStringList &srcFileList, const QStringList &replaceList, const QStringList &coexistList, const QString &dstDir,
                                   const E_File_OptionType &optType, const FileType &fileType)
{
    m_rootPath = rootPath;
    m_srcFileList = srcFileList;
    m_dstDirPath = dstDir;
    m_optType = optType;
    m_fileType = fileType;
    m_replaceList = replaceList;
    m_coexistList = coexistList;
}

void FileManageThread::setCopyInfo(const QString &rootPath, const QString &srcFile, const QString &dstFile,
                                   const E_File_OptionType &optType, const FileType &fileType)
{
    m_rootPath = rootPath;
    m_srcFile = srcFile;
    m_dstFile = dstFile;
    m_optType = optType;
    m_fileType = fileType;
}

void FileManageThread::getDirFileInfo(QString path)
{
    QDir dir(path);
    if (!dir.exists()) {
        return;
    }

    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();
    if (m_firstDirItem) {
        m_firstDirItem = false;
        if (m_optType == FILE_DIRSINFO) {
            m_iDirsItemCount += fileList.count();
            emit sigDirFileInfo(m_iDirsItemCount, m_iDirsFileSize, m_pathTmpList);
        } else {
            m_dirItemCount = fileList.count();
            emit sigDirFileInfo(m_dirItemCount, m_dirFileSize, m_pathTmpList);
        }
    }

    for (auto file : fileList) {
        if (!m_isCanRun)
            return;

        if (file.isDir()) {
            QString subDir = file.absoluteFilePath();
            getDirFileInfo(subDir);
        } else {
            if (m_optType == FILE_DIRSINFO) {
                m_iDirsFileSize += file.size();
                emit sigDirFileInfo(m_iDirsItemCount, m_iDirsFileSize, m_pathTmpList);
            } else {
                m_dirFileSize += file.size();
                emit sigDirFileInfo(m_dirItemCount, m_dirFileSize, m_pathTmpList);
            }
        }
    }
    return;
}

void FileManageThread::createFile(QString path)
{
    QFile fileName(path);
    if (fileName.exists()) {
        qDebug() << "createFile: File(" << path << ")exists.";
        return;
    }

    if (!fileName.open(QIODevice::WriteOnly)) {
        qDebug() << "Create File(" << path << ")Error.";
        return;
    } else {
        fileName.close();
    }

    return;
}

//void FileManageThread::deleteFile()
//{
//    //int nIndex = 0;
//    QFileInfo file_info;

//    foreach (QString strItemFile, m_fileList) {
//        if (!m_isCanRun)
//            break;

//        //        QFileInfo fi(strItemFile);
//        //        file_info.name = fi.fileName();

//        // int ret = deleteFunc_cmd(strItemFile);
//        int ret = remove(strItemFile.toLocal8Bit().data()); // C库函数快速删除
//        //大小为0的文件可能删除失败，使用下述方式再删一次
//        if (ret != 0) {
//            ret = deleteFunc_cmd(strItemFile);
//        }

//        emit sigFileProcResult(m_optType, ret, strItemFile, 0, file_info);
//        // send progress
//        //emit sigProgressRefresh(m_optType, ++nIndex);
//    }
//}

//int FileManageThread::deleteFunc_cmd(QString path)
//{
//    QString cmd = "rm";
//    QStringList args;
//    args << "-rf" << path;
//    return QProcess::execute(cmd, args);
//}

//bool FileManageThread::deleteFunc_qt(QString path)
//{
//    bool bRet;
//    QFileInfo fileInfo(path);
//    if (fileInfo.isDir()) {
//        QDir dir(path);
//        bRet = dir.removeRecursively();
//    } else {
//        QFile fileName(path);
//        bRet = fileName.remove();
//    }

//    return bRet;
//}

void FileManageThread::copyFileOrDir_NoCheck()
{
    qDebug() << __FUNCTION__ << " start.";

    int nIndex = 0;

    QString dstDir = m_dstDirPath;
    QDir dir(dstDir);
    if (!dir.exists()) {
        dir.mkdir(dstDir);
    }

    if (!dstDir.endsWith(QDir::separator())) {
        dstDir += QDir::separator();
    }

    m_process = new QProcess(); //不指定this，消除警告

    foreach (QString strItemFile, m_srcFileList) {
        if (!m_isCanRun)
            break;

        QString srcPath;
        if (strItemFile.startsWith("file://")) {
            // srcPath = strItemFile.mid(7);
            QUrl url(strItemFile);
            srcPath = url.toLocalFile();
        } else {
            srcPath = strItemFile;
        }

        QFileInfo fileInfo(srcPath);
        //源文件不存在，链接文件不支持导入，直接返回错误
        if (!fileInfo.exists() || fileInfo.isSymLink()) {
            //返回失败
            emit sigFileProcResult(m_optType, -1, srcPath, m_newFileItem, fileInfo);
            emit sigProgressRefresh(m_optType, ++m_procCount);
            continue;
        }

        QString fileName = fileInfo.fileName();
        QString dstPath = dstDir + fileName;
        emit sigProgressRefreshFileName(m_optType, fileName);

        m_newFileItem = true;
        if (m_replaceList.contains(strItemFile)) { //文件已存在，要替换
            m_newFileItem = false;
        } else if (m_coexistList.contains(strItemFile)) { //文件已存在，要共存
            //创建复本
            //生成文件名
            Utils::createFileName(dstPath, fileName);
            m_newFileItem = true;
        }

        //复制，替换
        copyFunc_cmd(srcPath, dstPath);

        // send progress
        emit sigProgressRefresh(m_optType, ++nIndex);
    }

    delete m_process;
    m_process = nullptr;

    qDebug() << __FUNCTION__ << " end.";
    return;
}

void FileManageThread::copyFileOrDir()
{
    qDebug() << __FUNCTION__ << " start.";

    int nIndex = 0;

    QString dstDir = m_dstDirPath;
    QDir dir(dstDir);
    if (!dir.exists()) {
        dir.mkdir(dstDir);
    }

    if (!dstDir.endsWith(QDir::separator())) {
        dstDir += QDir::separator();
    }

    m_process = new QProcess(); //不指定this，消除警告

    foreach (QString strItemFile, m_srcFileList) {
        if (!m_isCanRun)
            break;

        QString srcPath;
        if (strItemFile.startsWith("file://")) {
            // srcPath = strItemFile.mid(7);
            QUrl url(strItemFile);
            srcPath = url.toLocalFile();
        } else {
            srcPath = strItemFile;
        }

        QFileInfo fileInfo(srcPath);
        QString fileName = fileInfo.fileName();
        QString dstPath = dstDir + fileName;
        emit sigProgressRefreshFileName(m_optType, fileName);

        //        QString s = QString("find \"%1\" -name \"%2\"").arg(dstDir).arg(fileName);

        //        QProcess p;
        //        p.start(s);

        //        p.waitForStarted();
        //        p.waitForFinished();
        //        QString  sRes = QString::fromLocal8Bit(p.readAllStandardOutput());

        //        if (sRes != "") {
        m_newFileItem = true;
        if (QFile::exists(dstPath)) {
            //文件已存在，通知主界面弹出确认框
            //如果是同一目录下的复制，则不显示提示，以共存方式复制
            if (srcPath != dstPath) {
                if (!m_notAsk) {
                    m_fileRepeatType = -1;
                    emit sigFileRepeatConfirm(fileInfo.fileName(), fileInfo.absolutePath(), m_dstDirPath);
                    g_mutex.lock();
                    g_fileProcWait.wait(&g_mutex);
                    g_mutex.unlock();
                }

                if (m_fileRepeatType == FR_SKIP) { //跳过
                    emit sigProgressRefresh(m_optType, ++nIndex);
                    m_newFileItem = false;
                    continue;
                } else if (m_fileRepeatType == FR_COEXIST) { //共存
                    //创建复本
                    //生成文件名
                    Utils::createFileName(dstPath, fileName);
                    m_newFileItem = true;
                } else if (m_fileRepeatType == -1) {
                    //取消本次操作
                    break;
                } else if (m_fileRepeatType == FR_REPLACE) {
                    m_newFileItem = false;
                }
            } else {
                //创建复本
                //生成文件名
                Utils::createFileName(dstPath, fileName);
                m_newFileItem = true;
            }
        } else {
            m_newFileItem = true;
        }

        //复制，替换
        copyFunc_cmd(srcPath, dstPath);

        // send progress
        emit sigProgressRefresh(m_optType, ++nIndex);
    }

    delete m_process;
    m_process = nullptr;

    qDebug() << __FUNCTION__ << " end.";
    return;
}

//递归文件夹复制按文件
void FileManageThread::copyFile_r()
{
    qDebug() << "copyFile_r start.";

    //先统计目录下的所有文件个数
    int fileCount = 0;
    foreach (QString strItemFile, m_srcFileList) {
        if (!m_isCanRun)
            return;

        fileCount += getDirAllFielCount(strItemFile);
        fileCount++; //加上文件夹数
    }

    emit sigAllFileCount(fileCount);

    QString dstDir = m_dstDirPath;
    QDir dir(dstDir);
    if (!dir.exists()) {
        dir.mkdir(dstDir);
    }

    if (!dstDir.endsWith(QDir::separator())) {
        dstDir += QDir::separator();
    }

    foreach (QString strItemFile, m_srcFileList) {
        if (!m_isCanRun)
            break;

        QString srcPath;
        if (strItemFile.startsWith("file://")) {
            // srcPath = strItemFile.mid(7);
            QUrl url(strItemFile);
            srcPath = url.toLocalFile();
        } else {
            srcPath = strItemFile;
        }

        QFileInfo fileInfo(srcPath);
        QString fileName = fileInfo.fileName();
        // QString dstPath = dstDir + fileName;

        //copyFile_one(srcPath, dstDir);
        copyFile_one_NoCheck(srcPath, dstDir);
    }

    qDebug() << "copyFile_r end.";
    return;
}

/**
 * @brief 按单个文件拷贝，如果是文件夹则进入文件夹按文件复制
 * @param[in]
 * @param[out]
 * @return
 * @author zhangliangliang
 * @date 2020-07-11 10:47
 * @copyright (c) 2020
 */
void FileManageThread::copyFile_one(QString srcFile, QString dstDir)
{
    QDir dir(dstDir);
    if (!dir.exists()) {
        dir.mkdir(dstDir);
    }

    if (!dstDir.endsWith(QDir::separator())) {
        dstDir += QDir::separator();
    }

    QString srcPath;
    if (srcFile.startsWith("file://")) {
        QUrl url(srcFile);
        srcPath = url.toLocalFile();
    } else {
        srcPath = srcFile;
    }

    QFileInfo fileInfo(srcPath);
    QString fileName = fileInfo.fileName();
    QString dstPath = dstDir + fileName;

    emit sigProgressRefreshFileName(m_optType, fileName);

    m_newFileItem = true;

    if (QFile::exists(dstPath)) {
        //文件已存在，通知主界面弹出确认框
        //如果是同一目录下的复制，则不显示提示，以共存方式复制
        if (srcPath != dstPath) {
            if (!m_notAsk) {
                m_fileRepeatType = -1;
                emit sigFileRepeatConfirm(fileInfo.fileName(), fileInfo.absolutePath(), m_dstDirPath);
                g_mutex.lock();
                g_fileProcWait.wait(&g_mutex);
                g_mutex.unlock();
            }

            if (m_fileRepeatType == FR_SKIP) { //跳过
                emit sigProgressRefresh(m_optType, ++m_procCount);
                m_newFileItem = false;
                return;
            } else if (m_fileRepeatType == FR_REPLACE) { //替换
                //删除原文件
                FileUtils::deleteFunc_cmd(dstPath);
                m_newFileItem = false;
            } else if (m_fileRepeatType == FR_COEXIST) { //共存
                //创建复本
                //生成文件名
                Utils::createFileName(dstPath, fileName);
                m_newFileItem = true;
            } else if (m_fileRepeatType == -1) {
                //取消本次操作
                return;
            }
        } else {
            //创建复本
            //生成文件名
            Utils::createFileName(dstPath, fileName);
            m_newFileItem = true;
        }
    } else {
        m_newFileItem = true;
    }

    //文件夹则进入子目录按文件复制
    if (fileInfo.isDir()) {
        QDir copyDir(srcPath);
        copyDir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
        QFileInfoList subfileList = copyDir.entryInfoList();
        if (subfileList.size() > 0) {
            for (auto file : subfileList) {
                if (!m_isCanRun)
                    return;

                if (file.isDir()) {
                    QString subDir = file.absoluteFilePath();
                    copyFile_one(subDir, dstPath);
                } else {
                    QDir targetDir(dstPath);
                    if (!targetDir.exists()) {
                        targetDir.mkpath(dstPath);
                    }
                    QString fileName = file.absoluteFilePath();
                    QString targetFile = dstPath + "/" + file.fileName();
                    //复制，替换
                    //copyFunc_cmd(fileName, targetFile);
                    readWriteFile_C(fileName, targetFile);
                    emit sigProgressRefresh(m_optType, ++m_procCount);
                }
            }
        } else {
            //复制，替换
            //copyFunc_cmd(srcPath, dstPath);
            readWriteFile_C(srcPath, dstPath);
            emit sigProgressRefresh(m_optType, ++m_procCount);
        }
    } else {
        //复制，替换
        //copyFunc_cmd(srcPath, dstPath);
        readWriteFile_C(srcPath, dstPath);

        // send progress
        emit sigProgressRefresh(m_optType, ++m_procCount);
    }

    return;
}

/**
 * @brief 按单个文件拷贝，如果是文件夹则进入文件夹按文件复制
 * @param[in]
 * @param[out]
 * @return
 * @author zhangliangliang
 * @date 2020-07-11 10:47
 * @copyright (c) 2020
 */
void FileManageThread::copyFile_one_NoCheck(QString srcFile, QString dstDir)
{
    QDir dir(dstDir);
    if (!dir.exists()) {
        dir.mkdir(dstDir);
    }

    if (!dstDir.endsWith(QDir::separator())) {
        dstDir += QDir::separator();
    }

    QString srcPath;
    if (srcFile.startsWith("file://")) {
        QUrl url(srcFile);
        srcPath = url.toLocalFile();
    } else {
        srcPath = srcFile;
    }

    QFileInfo fileInfo(srcPath);
    //源文件不存在，链接文件不支持导入，直接返回错误
    if (!fileInfo.exists() || fileInfo.isSymLink()) {
        emit sigFileProcResult(m_optType, -1, srcPath, m_newFileItem, fileInfo);
        emit sigProgressRefresh(m_optType, ++m_procCount);
        return;
    }

    QString fileName = fileInfo.fileName();
    QString dstPath = dstDir + fileName;

    emit sigProgressRefreshFileName(m_optType, fileName);

    m_newFileItem = true;
    if (m_replaceList.contains(srcPath)) { //文件已存在，要替换
        //删除原文件
        FileUtils::deleteFunc_cmd(dstPath);
        m_newFileItem = false;
    } else if (m_coexistList.contains(srcPath)) { //文件已存在，要共存
        //创建复本
        //生成文件名
        Utils::createFileName(dstPath, fileName);
        m_newFileItem = true;
    }

    //文件夹则进入子目录按文件复制
    if (fileInfo.isDir()) {
        QDir copyDir(srcPath);
        copyDir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
        QFileInfoList subfileList = copyDir.entryInfoList();
        if (subfileList.size() > 0) {
            for (auto file : subfileList) {
                if (!m_isCanRun)
                    return;

                if (file.isDir()) {
                    QString subDir = file.absoluteFilePath();
                    copyFile_one_NoCheck(subDir, dstPath);
                } else {
                    QDir targetDir(dstPath);
                    if (!targetDir.exists()) {
                        targetDir.mkpath(dstPath);
                    }
                    QString fileName = file.absoluteFilePath();
                    QString targetFile = dstPath + "/" + file.fileName();
                    //复制，替换
                    //copyFunc_cmd(fileName, targetFile);
                    readWriteFile_C(fileName, targetFile);
                    emit sigProgressRefresh(m_optType, ++m_procCount);
                }
            }
        } else {
            //复制，替换
            //copyFunc_cmd(srcPath, dstPath);
            //readWriteFile_C(srcPath, dstPath);
            //当前是空目录
            QDir targetDir;
            bool bRet = targetDir.mkdir(dstPath);
            int ret = bRet ? 0 : 1;
            QFileInfo file_info;
            emit sigFileProcResult(m_optType, ret, dstPath, m_newFileItem, file_info);
            emit sigProgressRefresh(m_optType, ++m_procCount);
        }
    } else {
        //复制，替换
        //copyFunc_cmd(srcPath, dstPath);
        readWriteFile_C(srcPath, dstPath);

        // send progress
        emit sigProgressRefresh(m_optType, ++m_procCount);
    }

    return;
}

//复制文件内容生成新文件
void FileManageThread::readWriteFile_C(QString srcPath, QString dstPath)
{
    QFileInfo file_info;
    int ret = 0;

    FILE *srcfp = fopen(srcPath.toLocal8Bit().data(), "rb");
    if (srcfp == nullptr) {
        //文件打开失败，返回错误
        ret = -1;
        emit sigFileProcResult(m_optType, ret, srcPath, m_newFileItem, file_info);
        return;
    }

    FILE *dstfp = fopen(dstPath.toLocal8Bit().data(), "wb");
    if (dstfp == nullptr) {
        ret = -1;
        emit sigFileProcResult(m_optType, ret, srcPath, m_newFileItem, file_info);
        if (srcfp != nullptr)
            fclose(srcfp);
        return;
    }

    char buf[MAX_BUF] = {0};

    while (1) {
        if (!m_isCanRun) {
            ret = -2; //导入中止
            break;
        }
        memset(buf, 0, sizeof(buf));
        auto readSize = fread(buf, 1, MAX_BUF, srcfp);
        auto writeSize = fwrite(buf, 1, readSize, dstfp);
        if (readSize == 0) {
            break;
        }
        if (readSize != writeSize) {
            ret = -1;
            break;
        }
    }

    fclose(srcfp);
    fclose(dstfp);

    if (ret == 0) {
        file_info = QFileInfo(dstPath);
        //getFileInfo(dstPath, file_info);
    } else {
        remove(dstPath.toLocal8Bit().data()); //删除复制失败的文件
    }

    emit sigFileProcResult(m_optType, ret, dstPath, m_newFileItem, file_info);
}

//使用cp命令复制文件或目录
void FileManageThread::copyFunc_cmd(QString srcPath, QString dstPath)
{
    QString procPath;
    QString cmd = "cp";
    QStringList args;
    args << "-rf" << srcPath << dstPath;
    //    int ret = QProcess::execute(cmd, args);
    //    if (ret != 0) {
    //        qDebug() << "copy file error: " << srcPath;
    //    }

    if (m_process == nullptr)
        m_process = new QProcess(); //不指定this，消除警告

    m_process->start(cmd, args);
    m_process->waitForStarted();
    m_process->waitForFinished(-1);
    int exitCode = m_process->exitCode();

    if (m_optType == FILE_PASTE || m_optType == FILE_IMPORT) {
        procPath = dstPath;
    } else {
        procPath = srcPath;
    }

    QFileInfo file_info;
    if (exitCode == 0) {
        file_info = QFileInfo(procPath);
        //getFileInfo(procPath, file_info);
    } else {
        remove(dstPath.toLocal8Bit().data()); //删除复制失败的文件
    }

    if (exitCode != 9) { //9是被取消处理kill了，不处理返回
        emit sigFileProcResult(m_optType, exitCode, procPath, m_newFileItem, file_info);
    } else {
        if (m_optType == FILE_PASTE) {
            file_info = QFileInfo(procPath);
            if (file_info.isDir()) //复制目录时，取消操作可能有目录中的文件复制成功，所以要显示目录
                emit sigFileProcResult(m_optType, 0, procPath, m_newFileItem, file_info);
        }
    }
}

//低版本安卓使用，解决删除音乐时不能完全删除问题, 重名文件在线程外统一检查
void FileManageThread::importMusic_NoCheck()
{
    qDebug() << __FUNCTION__ << "start.";

    QString dstDir = m_dstDirPath;
    if (!dstDir.endsWith(QDir::separator())) {
        dstDir += QDir::separator();
    }

    int nIndex = 0;
    m_process = new QProcess(); //不指定this，消除警告

    foreach (QString strItemFile, m_srcFileList) {
        if (!m_isCanRun)
            break;

        QString srcPath = strItemFile;
        QFileInfo fileInfo(srcPath);
        QString fileName = fileInfo.fileName();
        //QString fileName = Utils::getFileNameByPath(strItemFile);
        QString dstPath = dstDir + fileName;
        emit sigProgressRefreshFileName(m_optType, fileName);

        m_newFileItem = true;
        if (m_replaceList.contains(strItemFile)) { //文件已存在，要替换
            m_newFileItem = false;
        } else if (m_coexistList.contains(strItemFile)) { //文件已存在，要共存
            //创建复本
            //生成文件名
            Utils::createFileName(dstPath, fileName);
            m_newFileItem = true;
        }

        //链接文件不支持导入，直接返回错误
        //QFileInfo fileInfo(srcPath);
        if (fileInfo.isSymLink()) {
            //返回失败
            emit sigFileProcResult(m_optType, -1, srcPath, m_newFileItem, fileInfo);
        } else {
            //复制，替换
            copyFunc_lowAndroid(srcPath, dstPath);
        }

        // send progress
        emit sigProgressRefresh(m_optType, ++nIndex);
    }

    delete m_process;
    m_process = nullptr;

    qDebug() << __FUNCTION__ << "end.";
    return;
}

//低版本安卓使用，解决删除音乐时不能完全删除问题
void FileManageThread::importMusic()
{
    qDebug() << __FUNCTION__ << "start.";

    QString dstDir = m_dstDirPath;
    if (!dstDir.endsWith(QDir::separator())) {
        dstDir += QDir::separator();
    }

    int nIndex = 0;
    m_process = new QProcess(); //不指定this，消除警告

    foreach (QString strItemFile, m_srcFileList) {
        if (!m_isCanRun)
            break;

        QString srcPath = strItemFile;
        QFileInfo fileInfo(srcPath);
        QString fileName = fileInfo.fileName();
        QString dstPath = dstDir + fileName;
        emit sigProgressRefreshFileName(m_optType, fileName);

        //PMS43852 有些音乐文件无法正常判断是否存在 改成ls 全部文件 ，再过滤指定文件名是否存在
        //不能直接ls 或 find文件名 QFile::exists ，存在已删除文件可以检出问题
        //QString s = QString("find \"%1\" -name \"%2\"").arg(dstDir).arg(fileName);
        QString cmd = QString("ls \"%1\"").arg(dstDir);
        QString sRes = Utils::execCmdNoResult(cmd);
        QStringList fileList = sRes.split("\n"); //分解出文件名

        m_newFileItem = true;

        if (fileList.contains(fileName)) {
            //文件已存在，通知主界面弹出确认框
            if (!m_notAsk) {
                m_fileRepeatType = -1;
                emit sigFileRepeatConfirm(fileInfo.fileName(), fileInfo.absolutePath(), m_dstDirPath);
                g_mutex.lock();
                g_fileProcWait.wait(&g_mutex);
                g_mutex.unlock();
            }

            if (m_fileRepeatType == FR_SKIP) { //跳过
                m_newFileItem = false;
                // send progress
                emit sigProgressRefresh(m_optType, ++nIndex);
                continue;
            } else if (m_fileRepeatType == FR_COEXIST) { //共存
                //创建复本
                //生成文件名
                Utils::createFileName(dstPath, fileName);
                m_newFileItem = true;
            } else if (m_fileRepeatType == -1) {
                //取消本次操作
                break;
            } else if (m_fileRepeatType == FR_REPLACE) {
                m_newFileItem = false;
            }
        } else {
            m_newFileItem = true;
        }

        //复制，替换
        copyFunc_lowAndroid(srcPath, dstPath);
        // send progress
        emit sigProgressRefresh(m_optType, ++nIndex);
    }

    delete m_process;
    m_process = nullptr;

    qDebug() << __FUNCTION__ << "end.";
    return;
}

//使用cp命令复制文件或目录 解决删除音乐时不能完全删除问题
void FileManageThread::copyFunc_lowAndroid(QString srcPath, QString dstPath)
{
    QFileInfo file_info;
    //处理文件复制过程中，源文件被删除问题
    QFileInfo srcFileInfo(srcPath);
    if (!srcFileInfo.exists()) {
        //文件不存在，返回错误
        emit sigFileProcResult(m_optType, -1, srcPath, m_newFileItem, file_info);
        return;
    }

    //  先 copy到临时文件， 临时文件 重命名
    QTemporaryFile tmpFile(dstPath);
    //    tmpFile.setAutoRemove(false);
    if (tmpFile.open()) {
        QString cmd = "cp";
        QStringList args;
        args << "-rf" << srcPath << tmpFile.fileName();

        //        int ret = QProcess::execute(cmd, args);
        //        if (ret == 0) {
        //            QString cmd = "mv";
        //            QStringList args;
        //            args << tmpFile.fileName() << dstPath;

        //            ret = QProcess::execute(cmd, args);
        //        }

        if (m_process == nullptr)
            m_process = new QProcess();

        m_process->start(cmd, args);
        m_process->waitForStarted();
        m_process->waitForFinished(-1);
        int exitCode = m_process->exitCode();
        if (exitCode == 0) {
            QString cmd = "mv";
            QStringList args;
            args << tmpFile.fileName() << dstPath;
            m_process->start(cmd, args);
            m_process->waitForStarted();
            m_process->waitForFinished(-1);
            exitCode = m_process->exitCode();
        }

        QString procPath;
        if (m_optType == FILE_PASTE || m_optType == FILE_IMPORT) {
            procPath = dstPath;
        } else {
            procPath = srcPath;
        }

        if (exitCode == 0) {
            file_info = QFileInfo(procPath);
            //getFileInfo(procPath, file_info);
        } else {
            remove(dstPath.toLocal8Bit().data()); //删除复制失败的文件
        }

        qDebug() << __LINE__ << __FUNCTION__ << srcPath << dstPath;
        if (exitCode != 9) { //9是被取消处理kill了，不处理返回
            emit sigFileProcResult(m_optType, exitCode, procPath, m_newFileItem, file_info);
        }
    }
}

//安卓低版本导入文件,特殊处理, 重名文件在线程外统一检查
void FileManageThread::importFile_NoCheck()
{
    qDebug() << __FUNCTION__ << "start.";

    QString dstDir = m_dstDirPath;
    if (!dstDir.endsWith(QDir::separator())) {
        dstDir += QDir::separator();
    }

    int nIndex = 0;
    m_process = new QProcess(); //不指定this，消除警告

    foreach (QString strItemFile, m_srcFileList) {
        if (!m_isCanRun)
            break;

        QString srcPath = strItemFile;
        QFileInfo fileInfo(srcPath);
        QString fileName = fileInfo.fileName();
        QString dstPath = dstDir + fileName;
        emit sigProgressRefreshFileName(m_optType, fileName);

        m_newFileItem = true;
        if (m_replaceList.contains(strItemFile)) { //文件已存在，要替换
            m_newFileItem = false;
        } else if (m_coexistList.contains(strItemFile)) { //文件已存在，要共存
            //创建复本
            //生成文件名
            Utils::createFileName(dstPath, fileName);
            m_newFileItem = true;
        }

        //链接文件不支持导入，直接返回错误
        if (fileInfo.isSymLink()) {
            //返回失败
            emit sigFileProcResult(m_optType, -1, srcPath, m_newFileItem, fileInfo);
        } else {
            //复制，替换
            //音乐文件存在删除后系统仍存在问题，特殊处理
            if (musicSuffix.contains(fileInfo.suffix())) {
                copyFunc_lowAndroid(srcPath, dstPath);
            } else {
                readWriteFile_C(srcPath, dstPath);
            }
        }

        // send progress
        emit sigProgressRefresh(m_optType, ++nIndex);
    }

    delete m_process;
    m_process = nullptr;

    qDebug() << __FUNCTION__ << "end.";
    return;
}

//安卓低版本导入文件,特殊处理
void FileManageThread::importFile()
{
    qDebug() << __FUNCTION__ << "start.";

    QString dstDir = m_dstDirPath;
    if (!dstDir.endsWith(QDir::separator())) {
        dstDir += QDir::separator();
    }

    int nIndex = 0;
    m_process = new QProcess(); //不指定this，消除警告

    foreach (QString strItemFile, m_srcFileList) {
        if (!m_isCanRun)
            break;

        QString srcPath = strItemFile;
        QFileInfo fileInfo(srcPath);
        QString fileName = fileInfo.fileName();
        QString dstPath = dstDir + fileName;
        emit sigProgressRefreshFileName(m_optType, fileName);

        //PMS43852 有些音乐文件无法正常判断是否存在 改成ls 全部文件 ，再过滤指定文件名是否存在
        //不能直接ls 或 find文件名 QFile::exists ，存在已删除文件可以检出问题
        //QString s = QString("find \"%1\" -name \"%2\"").arg(dstDir).arg(fileName);
        QString cmd = QString("ls \"%1\"").arg(dstDir);
        QString sRes = Utils::execCmdNoResult(cmd);
        QStringList fileList = sRes.split("\n"); //分解出文件名

        m_newFileItem = true;

        if (fileList.contains(fileName)) {
            //文件已存在，通知主界面弹出确认框
            if (!m_notAsk) {
                m_fileRepeatType = -1;
                emit sigFileRepeatConfirm(fileInfo.fileName(), fileInfo.absolutePath(), m_dstDirPath);
                g_mutex.lock();
                g_fileProcWait.wait(&g_mutex);
                g_mutex.unlock();
            }

            if (m_fileRepeatType == FR_SKIP) { //跳过
                m_newFileItem = false;
                // send progress
                emit sigProgressRefresh(m_optType, ++nIndex);
                continue;
            } else if (m_fileRepeatType == FR_COEXIST) { //共存
                //创建复本
                //生成文件名
                Utils::createFileName(dstPath, fileName);
                m_newFileItem = true;
            } else if (m_fileRepeatType == -1) {
                //取消本次操作
                break;
            } else if (m_fileRepeatType == FR_REPLACE) {
                m_newFileItem = false;
            }
        } else {
            m_newFileItem = true;
        }

        //复制，替换
        //音乐文件存在删除后系统仍存在问题，特殊处理
        if (musicSuffix.contains(fileInfo.suffix())) {
            copyFunc_lowAndroid(srcPath, dstPath);
        } else {
            readWriteFile_C(srcPath, dstPath);
        }
        // send progress
        emit sigProgressRefresh(m_optType, ++nIndex);
    }

    delete m_process;
    m_process = nullptr;

    qDebug() << __FUNCTION__ << "end.";
    return;
}

//只复制文件，不会复制文件夹及内部文件, 重名文件在线程外统一检查
void FileManageThread::copyFileOnly_NoCheck()
{
    qDebug() << __FUNCTION__ << " start.";

    QString dstDir = m_dstDirPath;

    if (!dstDir.endsWith(QDir::separator())) {
        dstDir += QDir::separator();
    }

    foreach (QString strItemFile, m_srcFileList) {
        if (!m_isCanRun)
            break;

        QString srcPath = strItemFile;
        QFileInfo fileInfo(srcPath);
        if (fileInfo.isDir())
            continue;

        //源文件不存在，链接文件不支持导入，直接返回错误
        if (!fileInfo.exists() || fileInfo.isSymLink()) {
            //返回失败
            emit sigFileProcResult(m_optType, -1, srcPath, m_newFileItem, fileInfo);
            emit sigProgressRefresh(m_optType, ++m_procCount);
            continue;
        }

        QString fileName = fileInfo.fileName();
        QString dstPath = dstDir + fileName;
        emit sigProgressRefreshFileName(m_optType, fileName);

        m_newFileItem = true;

        if (m_replaceList.contains(strItemFile)) { //文件已存在，要替换
            m_newFileItem = false;
        } else if (m_coexistList.contains(strItemFile)) { //文件已存在，要共存
            //创建复本
            //生成文件名
            Utils::createFileName(dstPath, fileName);
            m_newFileItem = true;
        }

        //链接文件不支持导入，直接返回错误
        if (fileInfo.isSymLink()) {
            //返回失败
            emit sigFileProcResult(m_optType, -1, srcPath, m_newFileItem, fileInfo);
        } else {
            //读写文件
            readWriteFile_C(srcPath, dstPath);
        }

        emit sigProgressRefresh(m_optType, ++m_procCount);
    }

    qDebug() << __FUNCTION__ << " end.";
    return;
}

//只复制文件，不会复制文件夹及内部文件
void FileManageThread::copyFileOnly()
{
    qDebug() << __FUNCTION__ << " start.";

    QString dstDir = m_dstDirPath;

    if (!dstDir.endsWith(QDir::separator())) {
        dstDir += QDir::separator();
    }

    foreach (QString strItemFile, m_srcFileList) {
        if (!m_isCanRun)
            break;

        QString srcPath = strItemFile;
        QFileInfo fileInfo(srcPath);
        if (fileInfo.isDir())
            continue;

        QString fileName = fileInfo.fileName();
        QString dstPath = dstDir + fileName;
        emit sigProgressRefreshFileName(m_optType, fileName);

        m_newFileItem = true;

        //PMS43852 有些音乐文件无法正常判断是否存在 改成ls 全部文件 ，再过滤指定文件名是否存在
        //不能直接ls 或 find文件名 QFile::exists ，存在已删除文件可以检出问题
        QString cmd = QString("ls \"%1\"").arg(dstDir);
        QString sRes = Utils::execCmdNoResult(cmd);
        QStringList fileList = sRes.split("\n"); //分解出文件名

        if (fileList.contains(fileName)) {
            //if (QFile::exists(dstPath)) {
            //文件已存在，通知主界面弹出确认框
            if (!m_notAsk) {
                m_fileRepeatType = -1;
                emit sigFileRepeatConfirm(fileInfo.fileName(), fileInfo.absolutePath(), m_dstDirPath);
                g_mutex.lock();
                g_fileProcWait.wait(&g_mutex);
                g_mutex.unlock();
            }

            if (m_fileRepeatType == FR_SKIP) { //跳过
                m_newFileItem = false;
                emit sigProgressRefresh(m_optType, ++m_procCount);
                continue;
            } else if (m_fileRepeatType == FR_COEXIST) { //共存
                //创建复本
                //生成文件名
                Utils::createFileName(dstPath, fileName);
                m_newFileItem = true;
            } else if (m_fileRepeatType == -1) {
                //取消本次操作
                return;
            } else if (m_fileRepeatType == FR_REPLACE) {
                m_newFileItem = false;
            }
        } else {
            m_newFileItem = true;
        }

        readWriteFile_C(srcPath, dstPath);
        emit sigProgressRefresh(m_optType, ++m_procCount);
    }

    qDebug() << __FUNCTION__ << " end.";
    return;
}

void FileManageThread::adbImportMusic_NoCheck()
{
    qDebug() << __FUNCTION__ << "       start.";

    int nIndex = 0;

    QString dstDir = m_dstDirPath;
    //    QDir dir(dstDir);
    //    if (!dir.exists()) {
    //        dir.mkdir(dstDir);
    //    }

    if (!dstDir.endsWith(QDir::separator())) {
        dstDir += QDir::separator();
    }

    //取得挂载路径下的第一层文件夹
    QString mountPath = Utils::getAndroidMountRootPath(m_devInfo.strPhoneID);

    m_process = new QProcess(); //不指定this，消除警告

    foreach (QString strItemFile, m_srcFileList) {
        if (!m_isCanRun)
            break;

        QString srcPath = strItemFile;
        QFileInfo fileInfo(srcPath);
        //链接文件不支持导入，文件不存在，直接返回错误
        if (!fileInfo.exists() || fileInfo.isSymLink()) {
            //返回失败
            emit sigFileProcResult(m_optType, -1, srcPath, m_newFileItem, fileInfo);
            // send progress
            emit sigProgressRefresh(m_optType, ++nIndex);
            continue; //继续下一个文件
        }

        QString fileName = fileInfo.fileName();
        //QString fileName = Utils::getFileNameByPath(strItemFile);
        QString dstPath = dstDir + fileName;
        emit sigProgressRefreshFileName(m_optType, fileName);

        m_newFileItem = true;

        //挂载路径转为adb用路径
        QString dstPath_adb = Utils::mountPathToAdbPath(mountPath, dstPath);

        if (m_replaceList.contains(strItemFile)) { //文件已存在，要替换
            m_newFileItem = false;
        } else if (m_coexistList.contains(strItemFile)) { //文件已存在，要共存
            //创建复本
            //生成文件名
            Utils::createFileName(dstPath, fileName);
            dstPath_adb = Utils::mountPathToAdbPath(mountPath, dstPath);
            m_newFileItem = true;
        }

        //链接文件不支持导入，直接返回错误
        //QFileInfo fileInfo(srcPath);
        if (fileInfo.isSymLink()) {
            //返回失败
            emit sigFileProcResult(m_optType, -1, srcPath, m_newFileItem, fileInfo);
            // send progress
            emit sigProgressRefresh(m_optType, ++nIndex);
            continue; //继续下一个文件
        }

        //大小为0的文件使用adb push到手机后，在挂载目录看不到
        //大小为0的文件用读取文件方式
        if (fileInfo.size() > 0) {
            // adb push上传文件
            int ret = adbPush(m_devInfo.strPhoneID, strItemFile, dstPath_adb);

            QFileInfo file_info;
            if (ret == 0) {
                file_info = QFileInfo(dstPath);
                //getFileInfo(dstPath, file_info);
            } else {
                remove(dstPath.toLocal8Bit().data()); //删除复制失败的文件
            }

            if (ret != 9) { //9是被取消处理kill了，不处理返回
                emit sigFileProcResult(m_optType, ret, dstPath, m_newFileItem, file_info);
                // send progress
                emit sigProgressRefresh(m_optType, ++nIndex);
            }
        } else {
            //大小为0 的文件单独处理
            readWriteFile_C(strItemFile, dstPath);
            emit sigProgressRefresh(m_optType, ++m_procCount);
        }
    }

    delete m_process;
    m_process = nullptr;

    qDebug() << __FUNCTION__ << "       end.";
}

void FileManageThread::adbImportMusic()
{
    qDebug() << __FUNCTION__ << "       start.";

    int nIndex = 0;

    QString dstDir = m_dstDirPath;
    //    QDir dir(dstDir);
    //    if (!dir.exists()) {
    //        dir.mkdir(dstDir);
    //    }

    if (!dstDir.endsWith(QDir::separator())) {
        dstDir += QDir::separator();
    }

    //取得挂载路径下的第一层文件夹
    QString mountPath = Utils::getAndroidMountRootPath(m_devInfo.strPhoneID);

    m_process = new QProcess(); //不指定this，消除警告

    foreach (QString strItemFile, m_srcFileList) {
        if (!m_isCanRun)
            break;

        QString srcPath = strItemFile;
        QFileInfo fileInfo(srcPath);
        QString fileName = fileInfo.fileName();
        QString dstPath = dstDir + fileName;
        emit sigProgressRefreshFileName(m_optType, fileName);

        //挂载路径转为adb用路径
        QString dstPath_adb = Utils::mountPathToAdbPath(mountPath, dstPath);
        //判断文件是否存在
        bool isExists = Utils::adbCheckFileExists(m_devInfo.strPhoneID, dstPath_adb);
        if (isExists) {
            //文件已存在，通知主界面弹出确认框
            if (!m_notAsk) {
                m_fileRepeatType = -1;
                emit sigFileRepeatConfirm(fileInfo.fileName(), fileInfo.absolutePath(), m_dstDirPath);
                g_mutex.lock();
                g_fileProcWait.wait(&g_mutex);
                g_mutex.unlock();
            }

            if (m_fileRepeatType == FR_SKIP) { //跳过
                emit sigProgressRefresh(m_optType, ++nIndex);
                m_newFileItem = false;
                continue;
            } else if (m_fileRepeatType == FR_COEXIST) { //共存
                //创建复本
                //生成文件名
                Utils::createFileName(dstPath, fileName);
                dstPath_adb = Utils::mountPathToAdbPath(mountPath, dstPath);
                m_newFileItem = true;
            } else if (m_fileRepeatType == -1) {
                //取消本次操作
                break;
            } else if (m_fileRepeatType == FR_REPLACE) {
                m_newFileItem = false;
            }
        } else {
            m_newFileItem = true;
        }

        //大小为0的文件使用adb push到手机后，在挂载目录看不到
        //大小为0的文件用读取文件方式单独处理
        if (fileInfo.size() > 0) {
            // adb push上传文件
            int ret = adbPush(m_devInfo.strPhoneID, strItemFile, dstPath_adb);

            QFileInfo file_info;
            if (ret == 0) {
                file_info = QFileInfo(dstPath);
                //getFileInfo(dstPath, file_info);
            } else {
                remove(dstPath.toLocal8Bit().data()); //删除复制失败的文件
            }

            if (ret != 9) { //9是被取消处理kill了，不处理返回
                emit sigFileProcResult(m_optType, ret, dstPath, m_newFileItem, file_info);
                // send progress
                emit sigProgressRefresh(m_optType, ++nIndex);
            }
        } else {
            //大小为0 的文件单独处理
            readWriteFile_C(strItemFile, dstPath);
            emit sigProgressRefresh(m_optType, ++m_procCount);
        }
    }

    delete m_process;
    m_process = nullptr;

    qDebug() << __FUNCTION__ << "       end.";
}

void FileManageThread::adbImportFile_NoCheck()
{
    qDebug() << __FUNCTION__ << "       start.";

    int nIndex = 0;

    QString dstDir = m_dstDirPath;
    //    QDir dir(dstDir);
    //    if (!dir.exists()) {
    //        dir.mkdir(dstDir);
    //    }

    if (!dstDir.endsWith(QDir::separator())) {
        dstDir += QDir::separator();
    }

    //取得挂载路径下的第一层文件夹
    QString mountPath = Utils::getAndroidMountRootPath(m_devInfo.strPhoneID);

    m_process = new QProcess(); //不指定this，消除警告

    foreach (QString strItemFile, m_srcFileList) {
        if (!m_isCanRun)
            break;

        QString srcPath = strItemFile;
        QFileInfo fileInfo(srcPath);
        //链接文件不支持导入，文件不存在，直接返回错误
        if (!fileInfo.exists() || fileInfo.isSymLink()) {
            //返回失败
            emit sigFileProcResult(m_optType, -1, srcPath, m_newFileItem, fileInfo);
            // send progress
            emit sigProgressRefresh(m_optType, ++nIndex);
            continue; //继续下一个文件
        }

        QString fileName = fileInfo.fileName();
        //QString fileName = Utils::getFileNameByPath(strItemFile);
        QString dstPath = dstDir + fileName;
        emit sigProgressRefreshFileName(m_optType, fileName);

        //挂载路径转为adb用路径
        QString dstPath_adb = Utils::mountPathToAdbPath(mountPath, dstPath);

        m_newFileItem = true;
        if (m_replaceList.contains(strItemFile)) { //文件已存在，要替换
            m_newFileItem = false;
        } else if (m_coexistList.contains(strItemFile)) { //文件已存在，要共存
            //创建复本
            //生成文件名
            Utils::createFileName(dstPath, fileName);
            dstPath_adb = Utils::mountPathToAdbPath(mountPath, dstPath);
            m_newFileItem = true;
        }

        //链接文件不支持导入，直接返回错误
        //QFileInfo fileInfo(srcPath);
        if (fileInfo.isSymLink()) {
            //返回失败
            emit sigFileProcResult(m_optType, -1, srcPath, m_newFileItem, fileInfo);
            // send progress
            emit sigProgressRefresh(m_optType, ++nIndex);
            continue; //继续下一个文件
        }

        //大小为0的文件使用adb push到手机后，在挂载目录看不到
        //大小为0的文件用读取文件方式单独处理
        if (fileInfo.size() > 0) {
            //adb push上传文件
            int ret = adbPush(m_devInfo.strPhoneID, strItemFile, dstPath_adb);

            QFileInfo file_info;
            if (ret == 0) {
                file_info = QFileInfo(dstPath);
                // getFileInfo(dstPath, file_info);
            } else {
                remove(dstPath.toLocal8Bit().data()); //删除复制失败的文件
            }

            if (ret != 9) { //9是被取消处理kill了，不处理返回
                emit sigFileProcResult(m_optType, ret, dstPath, m_newFileItem, file_info);
                // send progress
                emit sigProgressRefresh(m_optType, ++nIndex);
            }
        } else {
            //大小为0 的文件单独处理
            readWriteFile_C(strItemFile, dstPath);
            emit sigProgressRefresh(m_optType, ++m_procCount);
        }
    }

    delete m_process;
    m_process = nullptr;

    qDebug() << __FUNCTION__ << "       end.";
}

void FileManageThread::adbImportFile()
{
    qDebug() << __FUNCTION__ << "       start.";

    int nIndex = 0;

    QString dstDir = m_dstDirPath;
    //    QDir dir(dstDir);
    //    if (!dir.exists()) {
    //        dir.mkdir(dstDir);
    //    }

    if (!dstDir.endsWith(QDir::separator())) {
        dstDir += QDir::separator();
    }

    //取得挂载路径下的第一层文件夹
    QString mountPath = Utils::getAndroidMountRootPath(m_devInfo.strPhoneID);

    m_process = new QProcess(); //不指定this，消除警告

    foreach (QString strItemFile, m_srcFileList) {
        if (!m_isCanRun)
            break;

        QString srcPath = strItemFile;
        QFileInfo fileInfo(srcPath);
        QString fileName = fileInfo.fileName();
        QString dstPath = dstDir + fileName;
        emit sigProgressRefreshFileName(m_optType, fileName);

        //挂载路径转为adb用路径
        QString dstPath_adb = Utils::mountPathToAdbPath(mountPath, dstPath);
        //判断文件是否存在
        bool isExists = Utils::adbCheckFileExists(m_devInfo.strPhoneID, dstPath_adb);
        if (isExists) {
            //文件已存在，通知主界面弹出确认框
            if (!m_notAsk) {
                m_fileRepeatType = -1;
                emit sigFileRepeatConfirm(fileInfo.fileName(), fileInfo.absolutePath(), m_dstDirPath);
                g_mutex.lock();
                g_fileProcWait.wait(&g_mutex);
                g_mutex.unlock();
            }

            if (m_fileRepeatType == FR_SKIP) { //跳过
                emit sigProgressRefresh(m_optType, ++nIndex);
                m_newFileItem = false;
                continue;
            } else if (m_fileRepeatType == FR_COEXIST) { //共存
                //创建复本
                //生成文件名
                Utils::createFileName(dstPath, fileName);
                dstPath_adb = Utils::mountPathToAdbPath(mountPath, dstPath);
                m_newFileItem = true;
            } else if (m_fileRepeatType == -1) {
                //取消本次操作
                break;
            } else if (m_fileRepeatType == FR_REPLACE) {
                m_newFileItem = false;
            }
        } else {
            m_newFileItem = true;
        }

        //大小为0的文件使用adb push到手机后，在挂载目录看不到
        //大小为0的文件用读取文件方式单独处理
        if (fileInfo.size() > 0) {
            //adb push上传文件
            int ret = adbPush(m_devInfo.strPhoneID, strItemFile, dstPath_adb);

            QFileInfo file_info;
            if (ret == 0) {
                file_info = QFileInfo(dstPath);
                // getFileInfo(dstPath, file_info);
            } else {
                remove(dstPath.toLocal8Bit().data()); //删除复制失败的文件
            }

            if (ret != 9) { //9是被取消处理kill了，不处理返回
                emit sigFileProcResult(m_optType, ret, dstPath, m_newFileItem, file_info);
                // send progress
                emit sigProgressRefresh(m_optType, ++nIndex);
            }
        } else {
            //大小为0 的文件单独处理
            readWriteFile_C(strItemFile, dstPath);
            emit sigProgressRefresh(m_optType, ++m_procCount);
        }
    }

    delete m_process;
    m_process = nullptr;

    qDebug() << __FUNCTION__ << "       end.";
}

void FileManageThread::adbExportMusicEbook_NoCheck()
{
    qDebug() << __FUNCTION__ << "start.";

    int nIndex = 0;

    QString dstDir = m_dstDirPath;
    QDir dir(dstDir);
    if (!dir.exists()) {
        dir.mkdir(dstDir);
    }

    if (!dstDir.endsWith(QDir::separator())) {
        dstDir += QDir::separator();
    }

    //取得挂载路径下的第一层文件夹
    QString mountPath = Utils::getAndroidMountRootPath(m_devInfo.strPhoneID);

    m_process = new QProcess(); //不指定this，消除警告

    foreach (QString strItemFile, m_srcFileList) {
        if (!m_isCanRun)
            break;

        QFileInfo fileInfo(strItemFile);
        //源文件不存在，链接文件不支持导入，直接返回错误
        if (!fileInfo.exists() || fileInfo.isSymLink()) {
            //返回失败
            emit sigFileProcResult(m_optType, -1, strItemFile, m_newFileItem, fileInfo);
            emit sigProgressRefresh(m_optType, ++m_procCount);
            continue;
        }

        QString fileName = fileInfo.fileName();
        //QString fileName = Utils::getFileNameByPath(strItemFile);
        QString dstPath = dstDir + fileName;

        emit sigProgressRefreshFileName(m_optType, fileName);

        m_newFileItem = true;

        if (m_replaceList.contains(strItemFile)) { //文件已存在，要替换
            m_newFileItem = false;
        } else if (m_coexistList.contains(strItemFile)) { //文件已存在，要共存
            //创建复本
            //生成文件名
            Utils::createFileName(dstPath, fileName);
            m_newFileItem = true;
        }

        // adb pull下载文件
        //此处传入的路径可能是挂载路径，要转成adb路径/sdcard
        QString srcPath_adb = Utils::mountPathToAdbPath(mountPath, strItemFile);
        int ret = adbPull(m_devInfo.strPhoneID, srcPath_adb, dstPath);
        if (ret == 0) {
            qDebug() << __FUNCTION__ << "adb pull OKKKK>>>>>>>>>>>>>>>>";
        } else {
            // pms41669回避手机挂载目录下看到的文件名和实机文件名不一至问题 adb失败后用cp重试
            qDebug() << __FUNCTION__ << "adb pull ERROR>>>>cp retry.";
            QString cmd = "cp";
            QStringList args;
            args << "-rf" << strItemFile << dstPath;
            ret = QProcess::execute(cmd, args);
            if (ret != 0) {
                qDebug() << __FUNCTION__ << "copy file error: " << strItemFile;
            }
        }

        QFileInfo file_info;
        //        if(ret == 0){
        //            getFileInfo(dstPath, file_info);
        //        }
        if (ret != 0) {
            remove(dstPath.toLocal8Bit().data()); //删除复制失败的文件
        }

        if (ret != 9) { //9是被取消处理kill了，不处理返回
            emit sigFileProcResult(m_optType, ret, strItemFile, m_newFileItem, file_info);
            // send progress
            emit sigProgressRefresh(m_optType, ++nIndex);
        }
    }

    delete m_process;
    m_process = nullptr;

    qDebug() << __FUNCTION__ << "end.";
    return;
}

void FileManageThread::adbExportMusicEbook()
{
    qDebug() << __FUNCTION__ << "start.";

    int nIndex = 0;

    QString dstDir = m_dstDirPath;
    QDir dir(dstDir);
    if (!dir.exists()) {
        dir.mkdir(dstDir);
    }

    if (!dstDir.endsWith(QDir::separator())) {
        dstDir += QDir::separator();
    }

    //取得挂载路径下的第一层文件夹
    QString mountPath = Utils::getAndroidMountRootPath(m_devInfo.strPhoneID);

    m_process = new QProcess(); //不指定this，消除警告

    foreach (QString strItemFile, m_srcFileList) {
        if (!m_isCanRun)
            break;

        QFileInfo fileInfo(strItemFile);
        QString fileName = fileInfo.fileName();
        QString dstPath = dstDir + fileName;

        emit sigProgressRefreshFileName(m_optType, fileName);

        m_newFileItem = true;
        //判断文件是否存在
        if (QFile::exists(dstPath)) {
            //文件已存在，通知主界面弹出确认框
            if (!m_notAsk) {
                m_fileRepeatType = -1;
                emit sigFileRepeatConfirm(fileInfo.fileName(), fileInfo.absolutePath(), m_dstDirPath);
                g_mutex.lock();
                g_fileProcWait.wait(&g_mutex);
                g_mutex.unlock();
            }

            if (m_fileRepeatType == FR_SKIP) { //跳过
                emit sigProgressRefresh(m_optType, ++nIndex);
                m_newFileItem = false;
                continue;
            } else if (m_fileRepeatType == FR_COEXIST) { //共存
                //创建复本
                //生成文件名
                Utils::createFileName(dstPath, fileName);
                m_newFileItem = true;
            } else if (m_fileRepeatType == -1) {
                //取消本次操作
                break;
            } else if (m_fileRepeatType == FR_REPLACE) {
                m_newFileItem = false;
            }
        } else {
            m_newFileItem = true;
        }

        // adb pull下载文件
        //此处传入的路径可能是挂载路径，要转成adb路径/sdcard
        QString srcPath_adb = Utils::mountPathToAdbPath(mountPath, strItemFile);
        int ret = adbPull(m_devInfo.strPhoneID, srcPath_adb, dstPath);
        if (ret == 0) {
            qDebug() << __FUNCTION__ << "adb pull OKKKK>>>>>>>>>>>>>>>>";
        } else {
            // pms41669回避手机挂载目录下看到的文件名和实机文件名不一至问题 adb失败后用cp重试
            qDebug() << __FUNCTION__ << "adb pull ERROR>>>>cp retry.";
            QString cmd = "cp";
            QStringList args;
            args << "-rf" << strItemFile << dstPath;
            ret = QProcess::execute(cmd, args);
            if (ret != 0) {
                qDebug() << __FUNCTION__ << "copy file error: " << strItemFile;
            }
        }

        QFileInfo file_info;
        //        if(ret == 0){
        //            getFileInfo(dstPath, file_info);
        //        }
        if (ret != 0) {
            remove(dstPath.toLocal8Bit().data()); //删除复制失败的文件
        }

        if (ret != 9) { //9是被取消处理kill了，不处理返回
            emit sigFileProcResult(m_optType, ret, strItemFile, m_newFileItem, file_info);
            // send progress
            emit sigProgressRefresh(m_optType, ++nIndex);
        }
    }

    delete m_process;
    m_process = nullptr;

    qDebug() << __FUNCTION__ << "end.";
    return;
}

/**
 * @brief 删除文件特殊处理
 * @param[in]
 * @param[out]
 * @return
 * @author zhangliangliang
 * @date 2020-07-23 10:47
 * @copyright (c) 2020
 */
//void FileManageThread::adbDeleteFile()
//{
//    int nIndex = 0;

//    //取得挂载路径下的第一层文件夹
//    QString mountPath = Utils::getAndroidMountRootPath(m_devInfo.strPhoneID);

//    foreach (QString strItemFile, m_fileList) {
//        if (!m_isCanRun)
//            break;

//        //此处传入的路径可能是挂载路径，要转成adb路径/sdcard
//        QString adbFilePath = strItemFile;
//        adbFilePath = Utils::mountPathToAdbPath(mountPath, strItemFile);
//        int ret = Utils::adbDeleteFile(m_devInfo.strPhoneID, adbFilePath);

//        QFileInfo file_info;
//        emit sigFileProcResult(m_optType, ret, strItemFile, 0, file_info);
//        // send progress
//        emit sigProgressRefresh(m_optType, ++nIndex);
//    }
//}

void FileManageThread::adbImportFileOrDir()
{
    qDebug() << __FUNCTION__ << " start.";

    //先统计目录下的所有文件个数
    int fileCount = 0;
    foreach (QString strItemFile, m_srcFileList) {
        if (!m_isCanRun)
            return;

        fileCount += getDirAllFielCount(strItemFile);
    }

    emit sigAllFileCount(fileCount);

    QString dstDir = m_dstDirPath;
    QDir dir(dstDir);
    if (!dir.exists()) {
        dir.mkdir(dstDir);
    }

    if (!dstDir.endsWith(QDir::separator())) {
        dstDir += QDir::separator();
    }

    m_process = new QProcess(); //不指定this，消除警告

    foreach (QString strItemFile, m_srcFileList) {
        if (!m_isCanRun)
            break;

        QString srcPath;
        if (strItemFile.startsWith("file://")) {
            QUrl url(strItemFile);
            srcPath = url.toLocalFile();
        } else {
            srcPath = strItemFile;
        }

        QFileInfo fileInfo(srcPath);
        QString fileName = fileInfo.fileName();
        // QString dstPath = dstDir + fileName;

        //copyFile_one(srcPath, dstDir);
        adbImportFile_one(srcPath, dstDir);
    }

    delete m_process;
    m_process = nullptr;

    qDebug() << __FUNCTION__ << " end.";
    return;
}

/**
 * @brief 按单个文件拷贝，如果是文件夹则进入文件夹按文件复制
 * @param[in]
 * @param[out]
 * @return
 * @author zhangliangliang
 * @date 2020-07-11 10:47
 * @copyright (c) 2020
 */
void FileManageThread::adbImportFile_one(QString srcFile, QString dstDir)
{
    QDir dir(dstDir);
    if (!dir.exists()) {
        dir.mkdir(dstDir);
    }

    if (!dstDir.endsWith(QDir::separator())) {
        dstDir += QDir::separator();
    }

    QString srcPath = srcFile;
    //    if (srcFile.startsWith("file://")) {
    //        QUrl url(srcFile);
    //        srcPath = url.toLocalFile();
    //    } else {
    //        srcPath = srcFile;
    //    }

    QFileInfo fileInfo(srcPath);
    QString fileName = fileInfo.fileName();
    QString dstPath = dstDir + fileName;

    emit sigProgressRefreshFileName(m_optType, fileName);

    m_newFileItem = true;

    if (QFile::exists(dstPath)) {
        //文件已存在，通知主界面弹出确认框
        //如果是同一目录下的复制，则不显示提示，以共存方式复制
        if (srcPath != dstPath) {
            if (!m_notAsk) {
                m_fileRepeatType = -1;
                emit sigFileRepeatConfirm(fileInfo.fileName(), fileInfo.absolutePath(), m_dstDirPath);
                g_mutex.lock();
                g_fileProcWait.wait(&g_mutex);
                g_mutex.unlock();
            }

            if (m_fileRepeatType == FR_SKIP) { //跳过
                emit sigProgressRefresh(m_optType, ++m_procCount);
                m_newFileItem = false;
                return;
            } else if (m_fileRepeatType == FR_REPLACE) { //替换
                //删除原文件
                FileUtils::deleteFunc_cmd(dstPath);
                m_newFileItem = false;
            } else if (m_fileRepeatType == FR_COEXIST) { //共存
                //创建复本
                //生成文件名
                Utils::createFileName(dstPath, fileName);
                m_newFileItem = true;
            } else if (m_fileRepeatType == -1) {
                //取消本次操作
                return;
            }
        } else {
            //创建复本
            //生成文件名
            Utils::createFileName(dstPath, fileName);
            m_newFileItem = true;
        }
    } else {
        m_newFileItem = true;
    }

    //取得挂载路径下的第一层文件夹
    QString mountPath = Utils::getAndroidMountRootPath(m_devInfo.strPhoneID);

    //文件夹则进入子目录按文件复制
    if (fileInfo.isDir()) {
        QDir copyDir(srcPath);
        copyDir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
        QFileInfoList subfileList = copyDir.entryInfoList();
        if (subfileList.size() > 0) {
            for (auto file : subfileList) {
                if (!m_isCanRun)
                    return;

                if (file.isDir()) {
                    QString subDir = file.absoluteFilePath();
                    adbImportFile_one(subDir, dstPath);
                } else {
                    QDir targetDir(dstPath);
                    if (!targetDir.exists()) {
                        targetDir.mkpath(dstPath);
                    }
                    QString fileName = file.absoluteFilePath();
                    QString targetFile = dstPath + "/" + file.fileName();
                    //上传文件
                    //copyFunc_cmd(fileName, targetFile);
                    if (file.size() > 0)
                        adbPushFile(mountPath, fileName, targetFile);
                    else
                        readWriteFile_C(fileName, targetFile); //大小为0 的文件单独处理
                }
            }
        } else { //空文件夹
            //上传文件
            //copyFunc_cmd(srcPath, dstPath);
            adbPushFile(mountPath, srcPath, dstPath);
        }
    } else {
        //是一个文件
        //上传文件
        //copyFunc_cmd(srcPath, dstPath);
        if (fileInfo.size() > 0)
            adbPushFile(mountPath, srcPath, dstPath);
        else
            readWriteFile_C(srcPath, dstPath); //大小为0 的文件单独处理
    }

    return;
}

void FileManageThread::adbPushFile(QString mountPath, QString srcPath, QString dstPath)
{
    //挂载路径转为adb用路径
    QString dstPath_adb = Utils::mountPathToAdbPath(mountPath, dstPath);
    //adb push上传文件
    int ret = adbPush(m_devInfo.strPhoneID, srcPath, dstPath_adb);

    QFileInfo file_info;
    if (ret == 0) {
        file_info = QFileInfo(dstPath);
        //                getFileInfo(dstPath, file_info);
    } else {
        remove(dstPath.toLocal8Bit().data()); //删除复制失败的文件
    }

    if (ret != 9) { //9是被取消处理kill了，不处理返回
        emit sigFileProcResult(m_optType, ret, dstPath, m_newFileItem, file_info);
        // send progress
        emit sigProgressRefresh(m_optType, ++m_procCount);
    }
}

void FileManageThread::adbExportFileOrDir()
{
    qDebug() << __FUNCTION__ << " start.";

    //先统计目录下的所有文件个数
    int fileCount = 0;
    foreach (QString strItemFile, m_srcFileList) {
        if (!m_isCanRun)
            return;

        fileCount += getDirAllFielCount(strItemFile);
    }

    emit sigAllFileCount(fileCount);

    QString dstDir = m_dstDirPath;
    QDir dir(dstDir);
    if (!dir.exists()) {
        dir.mkdir(dstDir);
    }

    if (!dstDir.endsWith(QDir::separator())) {
        dstDir += QDir::separator();
    }

    m_process = new QProcess(); //不指定this，消除警告

    foreach (QString strItemFile, m_srcFileList) {
        if (!m_isCanRun)
            break;

        QString srcPath;
        if (strItemFile.startsWith("file://")) {
            QUrl url(strItemFile);
            srcPath = url.toLocalFile();
        } else {
            srcPath = strItemFile;
        }

        //QFileInfo fileInfo(srcPath);
        //QString fileName = fileInfo.fileName();
        // QString dstPath = dstDir + fileName;

        //copyFile_one(srcPath, dstDir);
        //adbExportFile_one(srcPath, dstDir);
        adbExportFile_one_NoCheck(srcPath, dstDir);
    }

    delete m_process;
    m_process = nullptr;

    qDebug() << __FUNCTION__ << " end.";
    return;
}

/**
 * @brief 按单个文件拷贝，如果是文件夹则进入文件夹按文件复制
 * @param[in]
 * @param[out]
 * @return
 * @author zhangliangliang
 * @date 2020-07-11 10:47
 * @copyright (c) 2020
 */
void FileManageThread::adbExportFile_one(QString srcFile, QString dstDir)
{
    QDir dir(dstDir);
    if (!dir.exists()) {
        dir.mkdir(dstDir);
    }

    if (!dstDir.endsWith(QDir::separator())) {
        dstDir += QDir::separator();
    }

    QString srcPath = srcFile;
    //    if (srcFile.startsWith("file://")) {
    //        QUrl url(srcFile);
    //        srcPath = url.toLocalFile();
    //    } else {
    //        srcPath = srcFile;
    //    }

    QFileInfo fileInfo(srcPath);
    QString fileName = fileInfo.fileName();
    QString dstPath = dstDir + fileName;

    emit sigProgressRefreshFileName(m_optType, fileName);

    m_newFileItem = true;

    if (QFile::exists(dstPath)) {
        //文件已存在，通知主界面弹出确认框
        if (!m_notAsk) {
            m_fileRepeatType = -1;
            emit sigFileRepeatConfirm(fileInfo.fileName(), fileInfo.absolutePath(), m_dstDirPath);
            g_mutex.lock();
            g_fileProcWait.wait(&g_mutex);
            g_mutex.unlock();
        }

        if (m_fileRepeatType == FR_SKIP) { //跳过
            emit sigProgressRefresh(m_optType, ++m_procCount);
            m_newFileItem = false;
            return;
        } else if (m_fileRepeatType == FR_REPLACE) { //替换
            //删除原文件
            FileUtils::deleteFunc_cmd(dstPath);

            m_newFileItem = false;
        } else if (m_fileRepeatType == FR_COEXIST) { //共存
            //创建复本
            //生成文件名
            Utils::createFileName(dstPath, fileName);
            m_newFileItem = true;
        } else if (m_fileRepeatType == -1) {
            //取消本次操作
            return;
        }
    } else {
        m_newFileItem = true;
    }

    //取得挂载路径下的第一层文件夹
    QString mountPath = Utils::getAndroidMountRootPath(m_devInfo.strPhoneID);

    //文件夹则进入子目录按文件复制
    if (fileInfo.isDir()) {
        QDir copyDir(srcPath);
        copyDir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
        QFileInfoList subfileList = copyDir.entryInfoList();
        if (subfileList.size() > 0) {
            for (auto file : subfileList) {
                if (!m_isCanRun)
                    return;

                if (file.isDir()) {
                    QString subDir = file.absoluteFilePath();
                    adbExportFile_one(subDir, dstPath);
                } else {
                    QDir targetDir(dstPath);
                    if (!targetDir.exists()) {
                        targetDir.mkpath(dstPath);
                    }
                    QString fileName = file.absoluteFilePath();
                    QString targetFile = dstPath + "/" + file.fileName();
                    //导出文件
                    //copyFunc_cmd(fileName, targetFile);
                    adbPullFile(mountPath, fileName, targetFile);
                }
            }
        } else {
            //导出文件
            //copyFunc_cmd(srcPath, dstPath);
            adbPullFile(mountPath, srcPath, dstPath);
        }
    } else {
        //导出文件
        //copyFunc_cmd(srcPath, dstPath);
        adbPullFile(mountPath, srcPath, dstPath);
    }

    return;
}

/**
 * @brief 按单个文件拷贝，如果是文件夹则进入文件夹按文件复制
 * @param[in]
 * @param[out]
 * @return
 * @author zhangliangliang
 * @date 2020-07-11 10:47
 * @copyright (c) 2020
 */
void FileManageThread::adbExportFile_one_NoCheck(QString srcFile, QString dstDir)
{
    QDir dir(dstDir);
    if (!dir.exists()) {
        dir.mkdir(dstDir);
    }

    if (!dstDir.endsWith(QDir::separator())) {
        dstDir += QDir::separator();
    }

    QString srcPath = srcFile;
    //    if (srcFile.startsWith("file://")) {
    //        QUrl url(srcFile);
    //        srcPath = url.toLocalFile();
    //    } else {
    //        srcPath = srcFile;
    //    }

    QFileInfo fileInfo(srcPath);
    //源文件不存在，链接文件不支持导入，直接返回错误
    if (!fileInfo.exists() || fileInfo.isSymLink()) {
        //返回失败
        emit sigFileProcResult(m_optType, -1, srcPath, m_newFileItem, fileInfo);
        emit sigProgressRefresh(m_optType, ++m_procCount);
        return;
    }

    QString fileName = fileInfo.fileName();
    QString dstPath = dstDir + fileName;

    emit sigProgressRefreshFileName(m_optType, fileName);

    m_newFileItem = true;
    if (m_replaceList.contains(srcPath)) { //文件已存在，要替换
        //删除原文件
        //deleteFunc_cmd(dstPath);
        FileUtils::deleteFunc_cmd(dstPath);
        m_newFileItem = false;
    } else if (m_coexistList.contains(srcPath)) { //文件已存在，要共存
        //创建复本
        //生成文件名
        Utils::createFileName(dstPath, fileName);
        m_newFileItem = true;
    }

    //取得挂载路径下的第一层文件夹
    QString mountPath = Utils::getAndroidMountRootPath(m_devInfo.strPhoneID);

    //文件夹则进入子目录按文件复制
    if (fileInfo.isDir()) {
        QDir copyDir(srcPath);
        copyDir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
        QFileInfoList subfileList = copyDir.entryInfoList();
        if (subfileList.size() > 0) {
            for (auto file : subfileList) {
                if (!m_isCanRun)
                    return;

                if (file.isDir()) {
                    QString subDir = file.absoluteFilePath();
                    adbExportFile_one_NoCheck(subDir, dstPath);
                } else {
                    QDir targetDir(dstPath);
                    if (!targetDir.exists()) {
                        targetDir.mkpath(dstPath);
                    }
                    QString fileName = file.absoluteFilePath();
                    QString targetFile = dstPath + "/" + file.fileName();
                    //导出文件
                    //copyFunc_cmd(fileName, targetFile);
                    adbPullFile(mountPath, fileName, targetFile);
                }
            }
        } else {
            //导出文件
            //copyFunc_cmd(srcPath, dstPath);
            adbPullFile(mountPath, srcPath, dstPath);
        }
    } else {
        //导出文件
        //copyFunc_cmd(srcPath, dstPath);
        adbPullFile(mountPath, srcPath, dstPath);
    }

    return;
}

void FileManageThread::adbPullFile(QString mountPath, QString srcPath, QString dstPath)
{
    //挂载路径转为adb用路径
    QString srcPath_adb = Utils::mountPathToAdbPath(mountPath, srcPath);
    //adb pull文件
    int ret = adbPull(m_devInfo.strPhoneID, srcPath_adb, dstPath);
    if (ret == 0) {
        qDebug() << __FUNCTION__ << "adb pull OKKKK>>>>>>>>>>>>>>>>";
    } else {
        // pms41669回避手机挂载目录下看到的文件名和实机文件名不一至问题 adb失败后用cp重试
        qDebug() << __FUNCTION__ << "adb pull ERROR>>>>cp retry.";
        QString cmd = "cp";
        QStringList args;
        args << "-rf" << srcPath << dstPath;
        ret = QProcess::execute(cmd, args);
        if (ret != 0) {
            qDebug() << __FUNCTION__ << "copy file error: " << srcPath;
        }
    }

    QFileInfo file_info;
    //    if (ret == 0) {
    //        getFileInfo(dstPath, file_info);
    //    }

    if (ret != 0) {
        remove(dstPath.toLocal8Bit().data()); //删除复制失败的文件
    }

    if (ret != 9) { //9是被取消处理kill了，不处理返回
        emit sigFileProcResult(m_optType, ret, srcPath, m_newFileItem, file_info);
        // send progress
        emit sigProgressRefresh(m_optType, ++m_procCount);
    }
}

/**
 * @brief adb方式向手机传输文件
 * @param[in] devId: 设备ID
 * @param[in] srcPath: 本地路径，要上传的文件路径
 * @param[in] dstPath_adb：手机中的adb路径
 * @author zhangliangliang
 */
int FileManageThread::adbPush(QString devId, QString srcPath, QString dstPath_adb)
{
    QString cmd = QString("adb -s %1 push \"%2\" \"%3\"").arg(devId).arg(srcPath).arg(dstPath_adb);
    qDebug() << __FUNCTION__ << cmd;

    if (m_process == nullptr)
        m_process = new QProcess();

    m_process->start(cmd);
    m_process->waitForStarted();
    m_process->waitForFinished(-1);
    int exitCode = m_process->exitCode();
    qDebug() << __FUNCTION__ << "adb push >>" << exitCode;

    return exitCode;
}

/**
 * @brief adb方式从手机下载文件
 * @param[in] devId: 设备ID
 * @param[in] srcPath_adb: 手机中adb文件路径
 * @param[in] dstPath：本地路径
 * @author zhangliangliang
 */
int FileManageThread::adbPull(QString devId, QString srcPath_adb, QString dstPath)
{
    QString cmd = QString("adb -s %1 pull \"%2\" \"%3\"").arg(devId).arg(srcPath_adb).arg(dstPath);
    qDebug() << __FUNCTION__ << cmd;

    if (m_process == nullptr)
        m_process = new QProcess();

    m_process->start(cmd);
    m_process->waitForStarted();
    m_process->waitForFinished(-1);
    int exitCode = m_process->exitCode();
    qDebug() << __FUNCTION__ << "adb push >>" << exitCode;

    return exitCode;
}

//QPixmap FileManageThread::readPicture(QString path)
//{
//    QPixmap pixmap;
//    QImageReader reader;
//    reader.setFileName(path);
//    reader.setFormat(Utils::DetectImageFormat(path).toLatin1());
//    reader.setAutoTransform(true);
//    QSize size = reader.size();

//    if (reader.canRead()) {
//        const qreal ratio = qApp->devicePixelRatio();
//        reader.setScaledSize(size * ratio);
//        pixmap = QPixmap::fromImage(reader.read());
//        pixmap.setDevicePixelRatio(ratio);
//    } else {
//        pixmap.load(path);
//    }

//    if (pixmap.isNull())
//        return pixmap;

//    return resizePicture(pixmap);
//}

//QPixmap FileManageThread::resizePicture(const QPixmap &icon)
//{
//    if (icon.isNull())
//        return icon;

//    QPixmap newIcon;

//    if (icon.width() > icon.height()) {
//        newIcon = icon.scaled(QSize(96, 64), Qt::KeepAspectRatio);
//    } else {
//        newIcon = icon.scaled(QSize(64, 96), Qt::KeepAspectRatio);
//    }

//    return newIcon;
//}

int FileManageThread::getDirAllFielCount(QString path)
{
    int fileCount = 0;

    QDir dir(path);
    if (path.isEmpty() || !dir.exists()) {
        return fileCount;
    }

    dir.setFilter(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    QFileInfoList fileList = dir.entryInfoList();
    for (auto file : fileList) {
        if (!m_isCanRun)
            return fileCount;

        if (file.isDir()) {
            QString subDir = file.absoluteFilePath();
            fileCount += getDirAllFielCount(subDir);
        } else {
            fileCount++;
        }
    }

    return fileCount;
}

void FileManageThread::setExit()
{
    m_isCanRun = false; //结束运行

    //杀掉可能在运行的进程
    if (m_process != nullptr && m_process->isOpen()) {
        m_process->kill();
    }
}

/**
 * @brief 获取文件类型图标 保存在DataService中
 * @author zhangliangliang
 */
void FileManageThread::getAllFileIcon()
{
    QStringList suffixList;
    suffixList << musicSuffix << ebookSuffix << photoSuffix << videoSuffix;

    foreach (auto suffix, suffixList) {
        QTemporaryFile tmpFile;
        if (tmpFile.open()) {
            tmpFile.rename(tmpFile.fileName() + "." + suffix);
            //qDebug() << __FUNCTION__ << suffix << tmpFile.fileName();
            Model::getInstance()->getIconBySuffix(suffix, tmpFile.fileName());
        }
    }
}

// 标记线程是否要加锁
bool FileManageThread::lock()
{
    if (m_bLocked)
        return true;

    m_bLocked = true; //true时线程要加锁
    return true;
}

// 标记线程是否要解锁
bool FileManageThread::unlock()
{
    if (!m_bLocked)
        return true;

    m_bLocked = false; //标识线程不用加锁
    m_mutex_read.unlock(); //解锁线程
    return true;
}

//检查线程是否可继续运行
//使用Mutex自锁停住执行
void FileManageThread::checkRunabled()
{
    //判断是否要暂停
    if (m_bLocked) {
        //通过两次加锁，让自己锁住，线程暂停
        m_mutex_read.lock();
        m_mutex_read.lock();
    } else {
        //解锁
        m_mutex_read.unlock();
    }
}

//读取目录下的文件列表
void FileManageThread::getAllFileList(QString path)
{
    if (!m_isCanRun)
        return;

    QDir dir(path);
    if (!dir.exists()) {
        return;
    }

    qDebug() << __FUNCTION__ << " entryInfoList in.";
    g_enterDirMutex.lock(); //加锁
    QFileInfoList fileList = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot, QDir::DirsFirst);
    g_enterDirMutex.unlock(); //解锁
    qDebug() << __FUNCTION__ << " entryInfoList out.";
    for (auto file : fileList) {
        if (!m_isCanRun)
            break;

        //读取文件信息
        FILE_INFO file_Info;
        file_Info.fileInfo = file;

        if (photoSuffix.contains(file.suffix(), Qt::CaseInsensitive)) {
            //读取图片缩略图
            QPixmap pixmap = Utils::readDiskPicture(file.absoluteFilePath());
            file_Info.icon = Utils::resizeFilePicture(pixmap);
        } else if (videoSuffix.contains(file.suffix(), Qt::CaseInsensitive)) {
            int nTime = 0;
            QPixmap pixmap;
            //读取视频缩略图
            Utils::_readVideo_ffmpeg_one(file.absoluteFilePath().toLocal8Bit().data(), pixmap, nTime);
            file_Info.icon = pixmap;
        } else {
            file_Info.icon = Model::getInstance()->getIconBySuffix(file.suffix(), file);
        }

        //获取图片视频的缩略图失败
        if (file_Info.icon.isNull()) {
            //读取默认图标
            file_Info.icon = Model::getInstance()->getIconBySuffix(file.suffix(), file);
        }

        //通知界面
        emit sigFileInfo(file_Info);
    }
}

//读取挂载目录
void FileManageThread::getRootPath()
{
    //回避手机挂载慢的导致获取目录失败问题，尝试10次后仍不成功返回失败
    int retryCount = 10; //解决有些机器挂载慢问题，尝试10秒
    while (retryCount > 0 && m_isCanRun) {
        retryCount--;

        m_rootPath = MountService::getService()->GetMountPath(m_devInfo.type, m_devInfo.strPhoneID);

        if (m_rootPath.isEmpty()) {
            QThread::sleep(1);
        } else {
            break;
        }
    }

    if (m_isCanRun) {
        if (m_rootPath.isEmpty()) {
            emit sigRootPathNotFound(); //没有挂载目录
        } else {
            emit sigRootPath(m_rootPath); //返回挂载目录
        }
    }
}
