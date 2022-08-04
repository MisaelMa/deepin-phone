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
#ifndef PHONEFILETHREAD_H
#define PHONEFILETHREAD_H

#include <QPixmap>
#include <QMap>
#include <QFileInfo>
#include <QMutex>

#include "defines.h"
#include "PMThread.h"

#define PFERROR_NOTFIND_DCIM 1001
#define PEERROR_NO_FILE 1003
#define PEERROR_END 2001 //标识位

class PhoneFileThread : public PMThread
{
    Q_OBJECT
public:
    PhoneFileThread(QObject *parent = nullptr);
    virtual ~PhoneFileThread() override;

    enum RunMode {
        ReadPhoto, //获取照片
        ReadVideo //获取视频
    };

    bool readPhoto(const QString &strPath, const QString &strPhoneID, const QSize &sMaxSize, DEVICE_TYPE type,
                   bool bOnlyDir = false, bool bKeepAspectRatio = false);
    bool readVideo(const QString &strPath, const QString &strPhoneID, const QSize &sMaxSize, DEVICE_TYPE type,
                   bool bOnlyDir = false, bool bKeepAspectRatio = false);
    // 停止线程
    void quitImmediately();
    void stopImmediately();
    //清空数据
    void clear();
    //    static void clearCache(const QString &strPhoneID);
    //    static void clearCache(const RunMode &mode, const QString &strPhoneID);
    //线程锁
    bool lock();
    bool unlock();

public:
    //    static bool _readVideo_ffmpeg_one(const char *input, QIcon &pixmap, int &nTime);
    static bool refreshDirInfo(const RunMode &runMode, const QString &strDesPath,
                               const int &nFileCount, const quint64 &nFileSize); //刷新缓存信息

private:
    //    static QVector<PhoneFileInfo> m_mapDirFileInfo_photo; //照片文件夹信息缓存
    //    static QVector<PhoneFileInfo> m_mapDirFileInfo_video; //视频文件夹信息缓存

signals:
    void sigProgress(int nRunMode, int nCur, int nTotal, QString strFileName); // 操作进度(拷贝)
    void sigFindPicture(PhoneFileInfo &info); // 图标查找
    void sigResult(int nRunMode, int nCount, int nSuccess, int nFailed, bool bIsStop); // 结果
    void error(int nErrorCode, QString strErrorMsg);

protected:
    void run() override;

private:
    PhoneFileInfo onVideoFindPixmap(QPixmap &icon, QString path, int nTime);

    bool getPhotoPath(QString strPhoneID);
    void getAndroidPhotoPath(QString strBasePath);

    bool _readPhoto();
    bool _readPhoto_one(const QString &path);
    bool _readVideo_ffmpeg();

    // 根据路径自动筛选文件
    void setFile(QString strDesPath);
    //根据目录获取其中一张图片
    bool setDir(const QString &strDesPath, QString &strDesItemPath);
    // 自动判断应该查找的文件
    bool autoFilterSet();
    // 根据指定大小resize图片
    QPixmap resizePicture(const QPixmap &icon);
    // 获取过滤条件
    QStringList getFileFilters();
    // 缓存处理
    bool processCache();
    // 读取文件信息
    bool readFileInfo(PhoneFileInfo &info);

protected:
    static QMutex m_sg_mutex_photo;
    static QMutex m_sg_mutex_video;
    QMutex m_mutex_read;
    bool m_bLocked;

    QString m_strSrcFileName; //源路径
    QString m_strDesFileName; //指定路径
    QStringList m_listFileNames; //操作多个文件
    QString m_strDesPath; //多个文件指向目录
    QString m_strRootPath; // 安卓用 rootpath
    QString m_strPhoneID; //查询某个手机里的数据
    QString m_strSysVersion; //手机系统版本
    DEVICE_TYPE m_phoneType; //查询手机的类型
    RunMode m_mode;
    QSize m_sMaxSize; //图片最大尺寸
    bool m_bKeepAspectRatio; //是否保持原图比例 共用此参数会导致混乱
    bool m_bIsDir; //文件夹
    QMap<QString, QString> m_mapFileToDir; //文件对应的目录
    QMap<QString, int> m_mapDirToFileCount;
    QMap<QString, quint64> m_mapDirToFileSize;
    QStringList m_listFilters;
};

#endif // PHONEFILETHREAD_H
