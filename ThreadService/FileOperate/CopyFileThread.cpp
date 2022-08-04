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
#include "CopyFileThread.h"

#include <QDirIterator>
#include <QDebug>
#include <QThreadPool>
#include <QtConcurrent/QtConcurrent>

#include <iostream>
#include <fstream>
#include <stdio.h>

#include "utils.h"
#include "FileUtils.h"
#include "TrObject.h"

CopyFileThread::CopyFileThread(QObject *parnet)
    : PMThread(parnet)
{
}

void CopyFileThread::copyFile(const QStringList &listFileNames, const QString &strDesPath, const int &mode,
                              const PhoneFileType &type, const QString &strSysVersion, const QString &strRootPath,
                              const QString strPhoneID)
{
    if (strDesPath.isEmpty())
        return;

    this->m_nType = static_cast<E_Copy_Type>(mode);

    m_listFileNames = listFileNames;
    m_strDesPath = strDesPath;
    if (type == IMAGE) {
        m_listFilters << FileUtils::getImageFilters();
    } else if (type == VIDEO) {
        m_listFilters << FileUtils::getVideoFilters();
    }
    m_strSysVersion = strSysVersion;
    m_strRootPath = strRootPath;
    m_strPhoneID = strPhoneID;
}

void CopyFileThread::_copyFile()
{
    int nIndex = 0;
    int nCount = m_listFileNames.count();
    QDir dir;
    dir.mkpath(m_strDesPath);
    if (!m_strDesPath.endsWith(QDir::separator())) {
        m_strDesPath += QDir::separator();
    }
    int nTotal = 0; //总数
    int nError = 0; //失败数
    foreach (QString strItemFile, m_listFileNames) {
        if (!m_isCanRun) {
            break;
        }
        QFileInfo fileInfo;
        fileInfo.setFile(strItemFile);
        // send progress
        emit sigCopyProgress(nIndex++, nCount, fileInfo.fileName());

        if (fileInfo.isDir()) {
            QString strNewPath = m_strDesPath + fileInfo.fileName() + QDir::separator();
            dir.mkpath(strNewPath);
            QDirIterator dir_iterator(strItemFile, m_listFilters, QDir::Files | QDir::NoSymLinks,
                                      QDirIterator::Subdirectories);
            while (dir_iterator.hasNext()) {
                if (!m_isCanRun) {
                    break;
                }
                dir_iterator.next();
                QFileInfo file_info = dir_iterator.fileInfo();
                if (file_info.isFile()) {
                    //                    bool bRet = QFile::copy(file_info.absoluteFilePath(), strNewPath +
                    //                    file_info.fileName());
                    bool bRet = _copyFile_one_auto(file_info, strNewPath);
                    ++nTotal;
                    if (!bRet) {
                        ++nError;
                        //                        emit error(-1,
                        //                        TrObject::getInstance()->getFileOperationText(copy_failed) +
                        //                        file_info.absoluteFilePath());
                    }
                }
            }
        } else if (fileInfo.isFile()) {
            bool bRet = _copyFile_one_auto(strItemFile, m_strDesPath);
            ++nTotal;
            if (!bRet) {
                ++nError;
                //                emit error(-1, TrObject::getInstance()->getFileOperationText(copy_failed) +
                //                strItemFile);
            }
        }
    }

    // send progress
    if (nCount != 0) {
        emit sigCopyProgress(nIndex, nCount, "");
    }

    emit sigCopyResult(m_nType, nTotal, nTotal - nError, nError, !m_isCanRun);
}

bool CopyFileThread::_copyFile_one_auto(const QFileInfo &source, const QString &desPath)
{
    //    if (m_mode == PhoneFileThread::ImportFile) {
    //        导入时，可能会替换，删除缓存
    //        PhoneFileThread::m_imgCacheHash.remove(pszFileCoper);
    //    }

    // 软链接不拷贝
    if (source.isSymLink()) {
        emit error(TrObject::getInstance()->getFileOperationText(copy_failed) + source.fileName());
        return false;
    }

    bool bRet = false;
    if (source.size() / 1024 / 1024 /*MB*/ < 120) {
        if (m_strSysVersion.contains("Android") && !Utils::isLowVersionAndroid(m_strSysVersion)) {
            // 考虑使用adb
            bRet = _copyFile_one_adb(source.absoluteFilePath().toLocal8Bit().data(),
                                     (desPath + source.fileName()).toLocal8Bit().data());
        } else {
            bRet = _copyFile_one_f(source.absoluteFilePath().toLocal8Bit().data(),
                                   (desPath + source.fileName()).toLocal8Bit().data());
        }
    } else {
        bRet = _copyFile_one_iof(source.absoluteFilePath().toLocal8Bit().data(),
                                 (desPath + source.fileName()).toLocal8Bit().data());
    }
    return bRet;
}

bool CopyFileThread::_copyFile_one_qfile(const QString &source, const QString &destination)
{
    QFile srcFile(source);
    if (!srcFile.open(QFile::ReadOnly)) {
        qDebug() << __LINE__ << source;
        qDebug() << __LINE__ << srcFile.errorString();
        return false;
    }

    QString strDesTemp = destination + "_tmp";
    QFile desFile(strDesTemp);
    if (!desFile.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
        qDebug() << __LINE__ << destination;
        qDebug() << __LINE__ << desFile.errorString();
        return false;
    }

    const qint64 maxSize = 4096;
    qint64 size = -1;

    QScopedArrayPointer<char> buffer(new char[maxSize]);

    bool bRes = true;
    do {
        if (!m_isCanRun) {
            bRes = false;
            qDebug() << __LINE__ << __FUNCTION__;
            break;
        }

        if ((size = srcFile.read(buffer.data(), maxSize)) < 0) {
            bRes = false;
            qDebug() << __LINE__ << __FUNCTION__;
            break;
        }

        if (desFile.write(buffer.data(), size) < 0) {
            bRes = false;
            qDebug() << __LINE__ << __FUNCTION__;
            break;
        }
    } while (size > 0);

    if (bRes) {
        QFile::remove(destination);
        QFile::rename(strDesTemp, destination);
    } else {
        if (QThreadPool::globalInstance()->activeThreadCount() >= QThreadPool::globalInstance()->maxThreadCount()) {
            // 线程池满了
            qDebug() << __LINE__ << QThreadPool::globalInstance()->activeThreadCount()
                     << QThreadPool::globalInstance()->maxThreadCount();
            FileUtils::_delFile_one(strDesTemp);
        } else {
            QtConcurrent::run(&FileUtils::_delFile_one, strDesTemp);
        }
    }

    return bRes;
}

bool CopyFileThread::_copyFile_one_iof(const QString &source, const QString &destination)
{
    using namespace std;
    QString strDesTemp = destination + "_tmp";
    ifstream in(source.toLocal8Bit(), ios::binary);
    ofstream out(strDesTemp.toLocal8Bit(), ios::binary | ios::trunc);
    if (!in.is_open()) {
        qDebug() << "error open file " << source << endl;
        return false;
    }
    if (!out.is_open()) {
        qDebug() << "error open file " << destination << endl;
        return false;
    }

    bool bRes = true;
    const qint64 maxSize = 2048;
    char buf[maxSize];
    long long totalBytes = 0;
    while (in) {
        if (!m_isCanRun) {
            bRes = false;
            break;
        }

        // read从in流中读取2048字节，放入buf数组中，同时文件指针向后移动2048字节
        //若不足2048字节遇到文件结尾，则以实际提取字节读取。
        in.read(buf, maxSize);
        // gcount()用来提取读取的字节数，write将buf中的内容写入out流。
        out.write(buf, in.gcount());
        out.flush();
        totalBytes += in.gcount();
    }
    in.close();
    out.close();

    if (bRes) {
        FileUtils::_delFile_one(destination);
        QFile::rename(strDesTemp, destination);
    } else {
        //        msleep(100);
        if (QThreadPool::globalInstance()->activeThreadCount() >= QThreadPool::globalInstance()->maxThreadCount()) {
            // 线程池满了
            qDebug() << __LINE__ << QThreadPool::globalInstance()->activeThreadCount()
                     << QThreadPool::globalInstance()->maxThreadCount();
            FileUtils::_delFile_one(strDesTemp);
        } else {
            qDebug() << __LINE__;
            QtConcurrent::run(&FileUtils::_delFile_one, strDesTemp);
        }
    }
    return bRes;
}

bool CopyFileThread::_copyFile_one_f(const QString &source, const QString &destination)
{
    using namespace std;

    fstream fsCopee(source.toLocal8Bit(), ios::binary | ios::in);
    fstream fsCoper(destination.toLocal8Bit(), ios::binary | ios::out);
    fsCoper << fsCopee.rdbuf();
    return true;
}

bool CopyFileThread::_copyFile_one_adb(const QString &source, const QString &destination)
{
    qDebug() << __LINE__ << __FUNCTION__;
    if (m_nType == ImportFile) {
        QString strAdbPath = Utils::mountPathToAdbPath(m_strRootPath, destination);
        if (Utils::adbPush(m_strPhoneID, source, strAdbPath) != 0)
            return false;
    } else if (m_nType == ExportFile) {
        QString strAdbPath = Utils::mountPathToAdbPath(m_strRootPath, source);
        if (Utils::adbPull(m_strPhoneID, strAdbPath, destination) != 0)
            return false;
    }
    return true;
}

void CopyFileThread::run()
{
    m_isCanRun = true;

    _copyFile();
}
