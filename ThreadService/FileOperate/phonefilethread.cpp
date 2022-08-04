/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co., Ltd.
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
#include "phonefilethread.h"
#include "utils.h"

#include <QDesktopServices>
#include <QFile>
#include <QDir>
#include <QMimeDatabase>
#include <QImageReader>
#include <QApplication>
#include <QDirIterator>
#include <QDebug>
#include <QtConcurrent>
#include <QMutex>

#include <iostream>
#include <fstream>
#include <stdio.h>

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavfilter/avfilter.h"
#include "libswscale/swscale.h"
#include "libavutil/frame.h"
#include "libavutil/error.h"
#include "libavutil/imgutils.h"
}

#include "Ios/iphonemountcontrol.h"
#include "GlobalDefine.h"
#include "TrObject.h"
#include "FileUtils.h"
#include "ImageVideoManager.h"

//QVector<PhoneFileInfo> PhoneFileThread::m_mapDirFileInfo_photo;
//QVector<PhoneFileInfo> PhoneFileThread::m_mapDirFileInfo_video;
extern QMutex g_enterDirMutex; //全局加载文件时读取目录信息用

QMutex PhoneFileThread::m_sg_mutex_photo;
QMutex PhoneFileThread::m_sg_mutex_video;

PhoneFileThread::PhoneFileThread(QObject *parent)
    : PMThread(parent)
{
    //    qRegisterMetaType<PhoneFileInfo>("PhoneFileInfo");
    m_bLocked = false;
}

PhoneFileThread::~PhoneFileThread()
{
    //stopImmediately();
    quitImmediately();

    //  基类析构 已经调用下面两个函数
    //    quit();
    //    wait();
    qDebug() << __LINE__ << __FUNCTION__;
}

bool PhoneFileThread::readPhoto(const QString &strPath, const QString &strPhoneID, const QSize &sMaxSize,
                                DEVICE_TYPE type, bool bOnlyDir, bool bKeepAspectRatio)
{
    m_listFileNames.clear();
    this->m_mode = ReadPhoto;
    this->m_sMaxSize = sMaxSize;
    this->m_bKeepAspectRatio = bKeepAspectRatio;
    this->m_strDesPath = strPath;
    this->m_strPhoneID = strPhoneID;
    this->m_bIsDir = bOnlyDir;
    this->m_phoneType = type;

    return true;
}

bool PhoneFileThread::readVideo(const QString &strPath, const QString &strPhoneID, const QSize &sMaxSize,
                                DEVICE_TYPE type, bool bOnlyDir, bool bKeepAspectRatio)
{
    m_listFileNames.clear();
    this->m_mode = ReadVideo;
    this->m_sMaxSize = sMaxSize;
    this->m_bKeepAspectRatio = bKeepAspectRatio;
    this->m_strDesPath = strPath;
    this->m_strPhoneID = strPhoneID;
    this->m_bIsDir = bOnlyDir;
    this->m_phoneType = type;

    return true;
}

void PhoneFileThread::quitImmediately()
{
    m_isCanRun = false;
    m_bLocked = false;
    m_mutex_read.unlock();
}

void PhoneFileThread::stopImmediately()
{
    if (this->isRunning())
        emit error(PEERROR_END, ""); // 界面停止spinner
}

void PhoneFileThread::clear()
{
    //    m_mapDirToFileCount.clear();
    //    m_mapDirToFileSize.clear();
    m_mapFileToDir.clear();
    m_listFileNames.clear();
}

//void PhoneFileThread::clearCache(const QString &strPhoneID)
//{
//    clearCache(PhoneFileThread::ReadPhoto, strPhoneID);
//    clearCache(PhoneFileThread::ReadVideo, strPhoneID);
//}

//void PhoneFileThread::clearCache(const PhoneFileThread::RunMode &mode, const QString &strPhoneID)
//{
//    if (mode == ReadPhoto) {
//        QVector<PhoneFileInfo>::iterator itr = m_mapDirFileInfo_photo.begin();
//        while (itr != m_mapDirFileInfo_photo.end()) {
//            PhoneFileInfo &item = *itr;
//            if (item.dirPath.contains(strPhoneID)) {
//                itr = m_mapDirFileInfo_photo.erase(itr);
//            } else {
//                itr++;
//            }
//        }
//    } else if (mode == ReadVideo) {
//        QVector<PhoneFileInfo>::iterator itr = m_mapDirFileInfo_video.begin();
//        while (itr != m_mapDirFileInfo_video.end()) {
//            PhoneFileInfo &item = *itr;
//            if (item.dirPath.contains(strPhoneID)) {
//                itr = m_mapDirFileInfo_video.erase(itr);
//            } else {
//                itr++;
//            }
//        }
//    }
//}

bool PhoneFileThread::lock()
{
    if (m_bLocked)
        return true;

    m_bLocked = true;
    return true;
}

bool PhoneFileThread::unlock()
{
    if (!m_bLocked)
        return true;

    m_bLocked = false;
    m_mutex_read.unlock();
    return true;
}

void PhoneFileThread::run()
{
    m_isCanRun = true;
    switch (m_mode) {
    case ReadPhoto:
        _readPhoto();
        break;
    case ReadVideo:
        _readVideo_ffmpeg();
        break;
    }
}

PhoneFileInfo PhoneFileThread::onVideoFindPixmap(QPixmap &icon, QString path, int nTime)
{
    PhoneFileInfo info;
    if (!m_isCanRun) {
        return info;
    }
    info.type = VIDEO;
    info.phoneID = m_strPhoneID;
    //    info.icon = QPixmap::fromImage(icon).scaled(m_sMaxSize, Qt::IgnoreAspectRatio);
    info.damaged = icon.isNull();
    //    info.originalIcon = icon.scaled(300, 300);
    info.scaleIcon = resizePicture(icon.scaled(m_sMaxSize));
    info.time = nTime;
    info.name = path.right(path.length() - path.lastIndexOf('/') - 1);
    info.path = path;
    //    info.scaleIcon = info.originalIcon.scaled(m_sMaxSize);
    if (m_bIsDir) {
        info.bIsDir = true;
        info.dirPath = m_mapFileToDir.value(path);
        info.fileCount = m_mapDirToFileCount.value(info.dirPath);
        info.size = m_mapDirToFileSize.value(info.dirPath);
        if (!ImageVideoManager::m_mapDirFileInfo_video.contains(info)) {
            ImageVideoManager::m_mapDirFileInfo_video.append(info);
        }
    } else {
        info.bIsDir = false;
        readFileInfo(info);
    }
    if (m_isCanRun) {
        emit sigFindPicture(info);
    }
    if (info.damaged) {
        qDebug() << __LINE__ << "read video error:time:" << nTime;
    }
    return info;
}

bool PhoneFileThread::_readPhoto()
{
    QMutexLocker locker(&m_sg_mutex_photo);
    if (processCache()) {
        return true;
    }
    if (!autoFilterSet()) {
        return false;
    }
    if (!m_isCanRun) {
        return false;
    }

    if (m_listFileNames.isEmpty()) {
        emit error(PEERROR_NO_FILE, "");
        return false;
    }

    foreach (auto strItemPath, m_listFileNames) {
        if (m_bLocked) {
            m_mutex_read.lock();
            m_mutex_read.lock();
        } else {
            m_mutex_read.unlock();
        }

        //如果是目录获取其中一张
        if (m_bIsDir) {
            QString strDirItemPath;
            if (!setDir(strItemPath, strDirItemPath)) { // 中断了
                ImageVideoManager::clearCache(m_mode, m_strPhoneID);
                return false;
            }
            if (strDirItemPath.isEmpty()) {
                continue;
            }
            strItemPath = strDirItemPath;
        }

        if (m_isCanRun) {
            _readPhoto_one(strItemPath);
        } else {
            if (m_bIsDir) {
                ImageVideoManager::clearCache(m_mode, m_strPhoneID);
            }
            return false;
        }
    }

    //    this->setProperty("filelist", m_listFileNames);
    if (m_isCanRun) {
        emit error(PEERROR_END, ""); // 界面停止spinner
    }

    return true;
}

bool PhoneFileThread::_readPhoto_one(const QString &path)
{
    PhoneFileInfo info;

    QPixmap pixmap = Utils::readDiskPicture(path);
    info.damaged = pixmap.isNull();

    info.type = IMAGE;
    info.phoneID = m_strPhoneID;
    info.scaleIcon = resizePicture(pixmap);
    info.time = 0;
    info.name = path.right(path.length() - path.lastIndexOf('/') - 1);
    info.path = path;
    //    info.scaleIcon = info.originalIcon.scaled(m_sMaxSize);
    if (m_bIsDir) {
        info.bIsDir = true;
        info.dirPath = m_mapFileToDir.value(path);
        info.fileCount = m_mapDirToFileCount.value(info.dirPath);
        info.size = m_mapDirToFileSize.value(info.dirPath);
        if (!ImageVideoManager::m_mapDirFileInfo_photo.contains(info)) {
            ImageVideoManager::m_mapDirFileInfo_photo.append(info);
        }
    } else {
        info.bIsDir = false;
        readFileInfo(info);
    }
    if (m_isCanRun) {
        emit sigFindPicture(info);
    }
    return true;
}

bool PhoneFileThread::_readVideo_ffmpeg()
{
    QMutexLocker locker(&m_sg_mutex_video);
    if (processCache()) {
        return true;
    }
    if (!autoFilterSet()) {
        return false;
    }
    if (!m_isCanRun) {
        return false;
    }

    if (m_listFileNames.isEmpty()) {
        emit error(PEERROR_NO_FILE, "");
        return false;
    }

    foreach (auto strItemPath, m_listFileNames) {
        if (m_bLocked) {
            m_mutex_read.lock();
            m_mutex_read.lock();
        } else {
            m_mutex_read.unlock();
        }

        //如果是目录获取其中一张
        if (m_bIsDir) {
            QString strDirItemPath;
            if (!setDir(strItemPath, strDirItemPath)) {
                ImageVideoManager::clearCache(m_mode, m_strPhoneID);
                return false;
            }
            if (strDirItemPath.isEmpty()) {
                continue;
            }
            strItemPath = strDirItemPath;
        }

        if (m_isCanRun) {
            QPixmap pixmap;
            int nTime;
            bool bRet = Utils::_readVideo_ffmpeg_one(strItemPath.toLocal8Bit(), pixmap, nTime);
            if (!bRet) {
                // 读取错误目前会用占位图表示
                // emit error(-1, tr("Read video icon error!"));
            }
            onVideoFindPixmap(pixmap, strItemPath, nTime);
        } else {
            if (m_bIsDir) {
                ImageVideoManager::clearCache(m_mode, m_strPhoneID);
            }
            return false;
        }
    }

    //    this->setProperty("filelist", m_listFileNames);
    if (m_isCanRun) {
        emit error(PEERROR_END, ""); // 界面停止spinner
    }

    return true;
}

bool PhoneFileThread::refreshDirInfo(const RunMode &runMode, const QString &strDesPath, const int &nFileCount, const quint64 &nFileSize)
{
    if (strDesPath.isEmpty()) {
        return false;
    }

    QVector<PhoneFileInfo>::iterator itr_begin;
    QVector<PhoneFileInfo>::iterator itr_end;
    if (runMode == ReadPhoto) {
        itr_begin = ImageVideoManager::m_mapDirFileInfo_photo.begin();
        itr_end = ImageVideoManager::m_mapDirFileInfo_photo.end();
    } else {
        itr_begin = ImageVideoManager::m_mapDirFileInfo_video.begin();
        itr_end = ImageVideoManager::m_mapDirFileInfo_video.end();
    }

    while (itr_begin != itr_end) {
        PhoneFileInfo &item = *itr_begin;
        if (item.dirPath == strDesPath) {
            item.fileCount = nFileCount;
            item.size = nFileSize;
            break;
        }
        ++itr_begin;
    }
    return true;
}

bool PhoneFileThread::setDir(const QString &strDesPath, QString &strDesItemPath)
{
    //    QDir dir;
    //    if (!dir.exists(strDesPath))
    //        return false;
    QStringList filters = getFileFilters();

    QList<QFileInfo> list_fileInfo;
    quint64 nSize = 0;
    //    qDebug() << __LINE__ << strDesPath;
    // 数量大小缓存
    bool bReaded = false;
    if (m_mapDirToFileCount.contains(strDesPath)) {
        bReaded = true;
    }

    QMutexLocker locker(&g_enterDirMutex);
    QDirIterator dir_iterator(strDesPath, filters, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while (dir_iterator.hasNext()) {
        if (!m_isCanRun) {
            return false;
        }
        dir_iterator.next();
        QFileInfo file_info = dir_iterator.fileInfo();
        if (!file_info.isFile()) {
            continue;
        }
        list_fileInfo.append(file_info);
        nSize += quint64(file_info.size());

        if (list_fileInfo.count() == 1) {
            strDesItemPath = file_info.absoluteFilePath();
            m_mapFileToDir.insert(file_info.absoluteFilePath(), strDesPath);
        }
        if (bReaded) {
            break;
        }
    }
    if (!bReaded && m_isCanRun) {
        m_mapDirToFileCount.insert(strDesPath, list_fileInfo.count());
        m_mapDirToFileSize.insert(strDesPath, nSize);
    }

    //    if (list_fileInfo.isEmpty())
    //        return false;

    return true;
}

bool PhoneFileThread::autoFilterSet()
{
    qDebug() << __LINE__ << __FUNCTION__ << m_strPhoneID;
    if (m_bIsDir) {
        //        QStringList listDirPaths;
        if (!m_strPhoneID.isEmpty()) {
            int nTimes = 10; // 10s //解决有些机器挂载慢问题，尝试10秒
            bool bRet = false;
            for (int i = 0; i < nTimes; ++i) {
                bRet = getPhotoPath(m_strPhoneID);
                if (!m_isCanRun) {
                    return false;
                }
                if (bRet) {
                    break;
                }

                sleep(1);
            }
            if (!bRet) {
                emit error(PFERROR_NOTFIND_DCIM, TrObject::getInstance()->getMountText(Failed_mount_the_directories));
                return false;
            }
        }
    } else {
        setFile(m_strDesPath);
    }
    return true;
}

bool PhoneFileThread::getPhotoPath(QString strPhoneID)
{
    QMutexLocker locker(&g_enterDirMutex);
    if (!m_isCanRun) {
        return false;
    }

    m_listFileNames.clear();

    if (m_phoneType == Mount_Ios) {
        QDir dir;
        dir.setPath(IPhoneMountControl::getMountAbsolutePath(strPhoneID));
        if (dir.exists()) {
            if (dir.cd("DCIM")) {
                m_listFileNames.append(dir.absolutePath());
                return true;
            }
        }
        return false;
    }

    QDir dir(Utils::mountGvfsPath());
    qDebug() << __LINE__ << __FUNCTION__ << Utils::mountGvfsPath();
    QFileInfoList list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    qDebug() << __LINE__ << __FUNCTION__ << list.count();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        QString file_path = fileInfo.fileName();
        if (file_path.contains("_" + strPhoneID)) {
            if (dir.cd(file_path)) {
                QStringList listChildrenDir = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
                if (listChildrenDir.count() >= 1) { //多个sd卡 || 照片模式
                    qDebug() << __LINE__ << QString("挂载文件夹数量[%1]:").arg(strPhoneID) << listChildrenDir.count();
                    if (!m_isCanRun) {
                        return false;
                    }

                    QString strCurPath = dir.absolutePath();

                    bool bIsPhotoMode = false;
                    if (listChildrenDir.contains("DCIM"))
                        bIsPhotoMode = true;
                    foreach (QString strItemChildPath, listChildrenDir) {
                        if (!m_isCanRun) {
                            return false;
                        }

                        dir.setPath(strCurPath + QDir::separator() + strItemChildPath);
                        qDebug() << __LINE__ << __FUNCTION__ << dir.absolutePath();
                        if (file_path.contains("iPhone")) { // iphone
                            if (!dir.cd("DCIM")) {
                                //                                return false;
                            }
                            return true;
                        } else { // Android
                            if (bIsPhotoMode) {
                                m_listFileNames.append(dir.absolutePath());
                            } else {
                                //
                                if (!m_isCanRun) {
                                    return false;
                                }
                                getAndroidPhotoPath(dir.absolutePath());
                            }
                        }
                    }
                    //                    if (listPath.isEmpty()) { //照片模式 getAndroidPhotoPath判断是否存在
                    //                        qDebug() << __LINE__ << dir.cd("..");
                    //                        listPath.append(getAndroidPhotoPath(dir.absolutePath()));
                    //                        qDebug() << __LINE__ << listPath.count();
                    //                    }
                    return true;
                }
            }
        }
    }
    return false;
}

void PhoneFileThread::getAndroidPhotoPath(QString strBasePath)
{
    QStringList listPendingPath;
    QDir dir(strBasePath);
    //下载
    listPendingPath.append(dir.absolutePath() + "/Download"); // 不能用arg
    // QQ
    listPendingPath.append(dir.absolutePath() + "/tencent/QQ_Images");
    listPendingPath.append(dir.absolutePath() + "/Tencent/QQ_Images");
    //微信
    listPendingPath.append(dir.absolutePath() + "/tencent/MicroMsg/WeiXin");
    listPendingPath.append(dir.absolutePath() + "/Tencent/MicroMsg/WeiXin");
    //                        //淘宝
    //                        listPendingPath.append(dir.absolutePath() + "/Pictures/taobao");
    //                        //苏宁
    //                        listPendingPath.append(dir.absolutePath() + "/Pictures/suning");
    //微博
    listPendingPath.append(dir.absolutePath() + "/sina/weibo/storage/photoalbum_save/weibo");
    listPendingPath.append(dir.absolutePath() + "/sina/weibo/weibo");
    // QQ音乐
    listPendingPath.append(dir.absolutePath() + "/qqmusic/mv");
    //网易云
    listPendingPath.append(dir.absolutePath() + "/netease/cloudmusic/MV");
    //酷狗
    listPendingPath.append(dir.absolutePath() + "/kugou/mv");
    if (dir.cd(strBasePath + "/DCIM")) {
        foreach (QFileInfo itemFileInfo, dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            if (!m_isCanRun) {
                break;
            }
            m_listFileNames.append(itemFileInfo.absoluteFilePath());
        }
    }

    if (dir.cd(strBasePath + "/Pictures")) {
        foreach (QFileInfo itemFileInfo, dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)) {
            if (!m_isCanRun) {
                break;
            }
            m_listFileNames.append(itemFileInfo.absoluteFilePath());
        }
    }

    foreach (QString strPath, listPendingPath) {
        if (!m_isCanRun) {
            break;
        }
        if (dir.exists(strPath)) {
            m_listFileNames.append(strPath);
        }
    }
}

// PhoneFileType PhoneFileThread::getFileType(QString strFilePath)
//{
//    QMimeDatabase db;
//    QMimeType mime = db.mimeTypeForFile(strFilePath);
//    //    qDebug() << __LINE__ << mime.name();
//    if (mime.name().startsWith("image/"))
//        return IMAGE;
//    else if (mime.name().startsWith("video/")) {
//        return VIDEO;
//    }
//    return UNKNOW;
//}

void PhoneFileThread::setFile(QString strDesPath)
{
    QMutexLocker locker(&g_enterDirMutex);
    QStringList filters = getFileFilters();

    quint64 size = 0;
    QDirIterator dir_iterator(strDesPath, filters, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);
    while (dir_iterator.hasNext() && m_isCanRun) {
        dir_iterator.next();
        QFileInfo file_info = dir_iterator.fileInfo();
        QString absPath = file_info.absoluteFilePath();
        size += quint64(file_info.size());

        m_listFileNames.append(absPath);
    }

    if (m_isCanRun) {
        refreshDirInfo(m_mode, strDesPath, m_listFileNames.count(), size);
    }
}

QPixmap PhoneFileThread::resizePicture(const QPixmap &icon)
{
    if (icon.isNull()) {
        //读取失败
        QPixmap pix(m_sMaxSize);
        pix.fill(Qt::transparent); //用透明色填充
        return pix;
    }
    QPixmap newIcon;
    if (m_bKeepAspectRatio) {
        newIcon = icon.scaled(m_sMaxSize, Qt::KeepAspectRatio);
        return newIcon;
    }

    if (icon.width() > icon.height()) {
        newIcon = icon.copy(0, 0, icon.height(), icon.height());
    } else {
        newIcon = icon.copy(0, 0, icon.width(), icon.width());
    }

    newIcon = newIcon.scaled(m_sMaxSize);
    //    newIcon = newIcon.scaled(150, 150);

    return newIcon;
}

QStringList PhoneFileThread::getFileFilters()
{
    /*
    通讯录格式：csv vcf xml
    图片格式：bmp gif jpeg jpg png wbmp webp
    视频格式：3gp avi f4v flv mkv mov mp4 rmvb wmv
    音乐格式：aac amr ape flac m4a mid mp3 ogg wav wma xmp
    电子书格式:chm doc ebk2 epub fb2 jar mobi ndb ndz pdb pdf pmlz rb rtf tcr txt zip
    */
    QStringList filters;
    if (m_mode == ReadPhoto) {
        filters << FileUtils::getImageFilters();
    } else if (m_mode == ReadVideo) {
        filters << FileUtils::getVideoFilters();
    }
    return filters;
}

bool PhoneFileThread::processCache()
{
    if (!m_bIsDir)
        return false;

    QStringList listFileNames;
    int nCount = 0;

    if (m_mode == ReadPhoto) {
        foreach (PhoneFileInfo item, ImageVideoManager::m_mapDirFileInfo_photo) {
            if (m_strPhoneID != item.phoneID)
                continue;
            if (!m_isCanRun) {
                return true;
            }
            listFileNames.append(item.path);
            emit sigFindPicture(item);
            ++nCount;
        }
    } else if (m_mode == ReadVideo) {
        foreach (PhoneFileInfo item, ImageVideoManager::m_mapDirFileInfo_video) {
            if (m_strPhoneID != item.phoneID)
                continue;
            if (!m_isCanRun) {
                return true;
            }
            listFileNames.append(item.path);
            emit sigFindPicture(item);
            ++nCount;
        }
    }

    if (nCount == 0)
        return false;

    //    this->setProperty("filelist", listFileNames);
    //缓存处理了 不用外面处理
    emit error(PEERROR_END, ""); // 界面停止spinner
    return true;
}

bool PhoneFileThread::readFileInfo(PhoneFileInfo &info)
{
    QFileInfo fileInfo;
    fileInfo.setFile(info.path);
    if (!fileInfo.exists()) {
        info.size = 0;
        //        info.lastModified = "";
        return false;
    }
    info.size = quint64(fileInfo.size());

    return true;
}
