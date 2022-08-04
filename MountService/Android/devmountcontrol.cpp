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
#include "devmountcontrol.h"

#include <dgiovolumemanager.h>
#include <dgiofile.h>
#include <dgiofileinfo.h>
#include <dgiovolume.h>
#include <dgiovolumemanager.h>

#include <QRegularExpression>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QThread>

#include "TrObject.h"

namespace {
const int ITEM_SPACING = 0;
const int LEFT_VIEW_WIDTH = 180;
const int LEFT_VIEW_LISTITEM_WIDTH = 140;
const int LEFT_VIEW_LISTITEM_HEIGHT = 40;
const int OPE_MODE_ADDNEWALBUM = 0;
const int OPE_MODE_RENAMEALBUM = 1;
const int OPE_MODE_ADDRENAMEALBUM = 2;
const QString BUTTON_STR_RECOVERY = "恢复";
const QString BUTTON_STR_DETELE = "删除";
const QString BUTTON_STR_DETELEALL = "全部删除";
const int RIGHT_VIEW_IMPORT = 0;
const int RIGHT_VIEW_THUMBNAIL_LIST = 1;
const int RIGHT_VIEW_TRASH_LIST = 2;
const int RIGHT_VIEW_FAVORITE_LIST = 3;
const int RIGHT_VIEW_SEARCH = 4;
const int RIGHT_VIEW_PHONE = 5;
const int RIGHT_VIEW_TIMELINE_IMPORT = 6;
const int VIEW_MAINWINDOW_ALBUM = 2;

static QMap<QString, const char *> i18nMap {{"data", "Data Disk"}};
const QString ddeI18nSym = QStringLiteral("_dde_");

static std::initializer_list<std::pair<QString, QString>> opticalmediakeys {{"optical", "Optical"},
                                                                            {"optical_cd", "CD-ROM"},
                                                                            {"optical_cd_r", "CD-R"},
                                                                            {"optical_cd_rw", "CD-RW"},
                                                                            {"optical_dvd", "DVD-ROM"},
                                                                            {"optical_dvd_r", "DVD-R"},
                                                                            {"optical_dvd_rw", "DVD-RW"},
                                                                            {"optical_dvd_ram", "DVD-RAM"},
                                                                            {"optical_dvd_plus_r", "DVD+R"},
                                                                            {"optical_dvd_plus_rw", "DVD+RW"},
                                                                            {"optical_dvd_plus_r_dl", "DVD+R/DL"},
                                                                            {"optical_dvd_plus_rw_dl", "DVD+RW/DL"},
                                                                            {"optical_bd", "BD-ROM"},
                                                                            {"optical_bd_r", "BD-R"},
                                                                            {"optical_bd_re", "BD-RE"},
                                                                            {"optical_hddvd", "HD DVD-ROM"},
                                                                            {"optical_hddvd_r", "HD DVD-R"},
                                                                            {"optical_hddvd_rw", "HD DVD-RW"},
                                                                            {"optical_mo", "MO"}};
static QVector<std::pair<QString, QString>> opticalmediakv(opticalmediakeys);
static QMap<QString, QString> opticalmediamap(opticalmediakeys);

} // namespace

QString sizeString(const QString &str)
{
    int begin_pos = str.indexOf('.');

    if (begin_pos < 0)
        return str;

    QString size = str;

    while (size.count() - 1 > begin_pos) {
        if (!size.endsWith('0'))
            return size;

        size = size.left(size.count() - 1);
    }

    return size.left(size.count() - 1);
}

QString formatSize(quint64 num, bool withUnitVisible = true, int precision = 1, int forceUnit = -1,
                   QStringList unitList = QStringList())
{
    if (num <= 0) {
        qWarning() << "Negative number passed to formatSize():" << num;
        num = 0;
    }

    bool isForceUnit = (forceUnit >= 0);
    QStringList list;
    qreal fileSize(num);

    if (unitList.size() == 0) {
        list << " B"
             << " KB"
             << " MB"
             << " GB"
             << " TB"; // should we use KiB since we use 1024 here?
    } else {
        list = unitList;
    }

    QStringListIterator i(list);
    QString unit = i.hasNext() ? i.next() : QStringLiteral(" B");

    int index = 0;
    while (i.hasNext()) {
        if (fileSize < 1024 && !isForceUnit) {
            break;
        }

        if (isForceUnit && index == forceUnit) {
            break;
        }

        unit = i.next();
        fileSize /= 1024;
        index++;
    }
    QString unitString = withUnitVisible ? unit : QString();
    return QString("%1%2").arg(sizeString(QString::number(fileSize, 'f', precision)), unitString);
}

DevMountControl::DevMountControl(QObject *parent)
    : QObject(parent)
{
    m_durlAndNameMap.clear();

    m_vfsManager = new DGioVolumeManager;
    //    m_diskManager = new DDiskManager(this);
    //    m_diskManager->setWatchChanges(true);

    //    m_mounts = getVfsMountList();

    initConnection();
}

//析构处理，释放内存
DevMountControl::~DevMountControl()
{
    if (m_vfsManager != nullptr)
        delete m_vfsManager; //释放 m_vfsManager
    //    if (m_diskManager != nullptr)
    //        delete m_diskManager; //释放 m_diskManager
}

void DevMountControl::initConnection()
{
    //    connect(m_vfsManager, &DGioVolumeManager::mountAdded, this, &DevMountControl::onVfsMountChangedAdd);
    //    connect(m_vfsManager, &DGioVolumeManager::mountRemoved, this, &DevMountControl::onVfsMountChangedRemove);
    connect(m_vfsManager, &DGioVolumeManager::volumeAdded, [](QExplicitlySharedDataPointer<DGioVolume> vol) {
        if (vol->volumeMonitorName().contains(QRegularExpression("(MTP|GPhoto2|Afc)$"))) {
            vol->mount();
        }
    });

    //    connect(m_diskManager, &DDiskManager::fileSystemAdded, this, [=](const QString &dbusPath) {
    //        DBlockDevice *blDev = DDiskManager::createBlockDevice(dbusPath);
    //        blDev->mount({});
    //    });
    //    connect(m_diskManager, &DDiskManager::blockDeviceAdded, this, [=](const QString &blks) {
    //        QSharedPointer<DBlockDevice> blk(DDiskManager::createBlockDevice(blks));
    //        QScopedPointer<DDiskDevice> drv(DDiskManager::createDiskDevice(blk->drive()));

    //        if (!blk->hasFileSystem() && !drv->mediaCompatibility().join(" ").contains("optical") && !blk->isEncrypted()) {
    //            return;
    //        }
    //        if ((blk->hintIgnore() && !blk->isEncrypted()) || blk->cryptoBackingDevice().length() > 1) {
    //            return;
    //        }
    //        DBlockDevice *pblk = blk.data();
    //        QByteArrayList mps = blk->mountPoints();
    //        qulonglong size = blk->size();
    //        QString label = blk->idLabel();
    //        QString fs = blk->idType();
    //        QString udispname = "";
    //        if (label.startsWith(ddeI18nSym)) {
    //            QString i18nKey = label.mid(ddeI18nSym.size(), label.size() - ddeI18nSym.size());
    //            udispname = qApp->translate("DeepinStorage", i18nMap.value(i18nKey, i18nKey.toUtf8().constData()));
    //            goto runend;
    //        }

    //        if (mps.contains(QByteArray("/\0", 2))) {
    //            udispname = QCoreApplication::translate("PathManager", "System Disk");
    //            goto runend;
    //        }
    //        if (label.length() == 0) {
    //            QScopedPointer<DDiskDevice> drv(DDiskManager::createDiskDevice(pblk->drive()));
    //            if (!drv->mediaAvailable() && drv->mediaCompatibility().join(" ").contains("optical")) {
    //                QString maxmediacompat;
    //                for (auto i = opticalmediakv.rbegin(); i != opticalmediakv.rend(); ++i) {
    //                    if (drv->mediaCompatibility().contains(i->first)) {
    //                        maxmediacompat = i->second;
    //                        break;
    //                    }
    //                }
    //                udispname = QCoreApplication::translate("DeepinStorage", "%1 Drive").arg(maxmediacompat);
    //                goto runend;
    //            }
    //            if (drv->opticalBlank()) {
    //                udispname =
    //                    QCoreApplication::translate("DeepinStorage", "Blank %1 Disc").arg(opticalmediamap[drv->media()]);
    //                goto runend;
    //            }
    //            if (pblk->isEncrypted() && !blk) {
    //                udispname = QCoreApplication::translate("DeepinStorage", "%1 Encrypted").arg(formatSize(size));
    //                goto runend;
    //            }
    //            udispname = QCoreApplication::translate("DeepinStorage", "%1 Volume").arg(formatSize(size));
    //            goto runend;
    //        }
    //        udispname = label;

    //    runend:
    //        blk->mount({});
    //        QByteArrayList qbl = blk->mountPoints();
    //        QString mountPoint = "file://";
    //        for (QByteArray qb : qbl) {
    //            mountPoint += qb;
    //        }
    //        QUrl qurl(mountPoint);
    //        m_durlAndNameMap[qurl] = udispname;
    //        return;
    //    });
}

void DevMountControl::onVfsMountChangedAdd(QExplicitlySharedDataPointer<DGioMount> mount)
{
    qDebug() << "onVfsMountChangedAdd() name:" << mount->name();
    Q_UNUSED(mount);
    // TODO:
    // Support android phone, iPhone, and usb devices. Not support ftp, smb mount, non removeable disk now
    QString uri = mount->getRootFile()->uri();
    QString path = mount->getRootFile()->path();
    QString scheme = QUrl(uri).scheme();
    qDebug() << "scheme:" << scheme << ", path:" << path;

    if ((scheme == "file" && mount->canEject()) || // usb device
        (scheme == "gphoto2") || // phone photo
        //            (scheme == "afc") ||                    //iPhone document
        (scheme == "mtp")) { // android file
        qDebug() << "mount.name" << mount->name() << " scheme type:" << scheme;

        for (auto mountLoop : m_mounts) {
            QString uriLoop = mountLoop->getRootFile()->uri();
            qDebug() << "uri:" << uriLoop;
            if (uri == uriLoop) {
                qDebug() << "Already has this device in mount list. uri:" << uriLoop;
                return;
            }
        }

        QExplicitlySharedDataPointer<DGioFile> LocationFile = mount->getDefaultLocationFile();
        QString strPath = LocationFile->path();
        if (strPath.isEmpty()) {
            qDebug() << "onVfsMountChangedAdd() strPath.isEmpty()";
            return;
        }

        QString rename = "";
        rename = m_durlAndNameMap[QUrl(mount->getRootFile()->uri())];
        if ("" == rename) {
            rename = mount->name();
        }

        //针对android设备获取挂载的设备Id,通知设备检测线程发现新设备，ios设备不用此处理
        QString devId;
        if (scheme == "gphoto2") {
            //只处理安卓
            if (!path.contains("Apple_Inc")) {
                devId = path.mid(path.lastIndexOf('_') + 1);
            }
        } else if (scheme == "mtp") {
            devId = path.mid(path.lastIndexOf('_') + 1);
        }

        //        if (!devId.isEmpty()) {
        //            emit sigNewDeviceAdd(devId);
        //            qDebug() << "New Android device find. ID:" << devId;
        //        }

        //判断路径是否存在
        QDir dir(strPath);
        if (!dir.exists()) {
            qDebug() << "onLoadMountImagesStart() !dir.exists()";
            return;
        }

        updateExternalDevice(mount);
    }
}

void DevMountControl::onVfsMountChangedRemove(QExplicitlySharedDataPointer<DGioMount> mount)
{
    Q_UNUSED(mount);

    QString uri = mount->getRootFile()->uri();
    for (auto mountLoop : m_mounts) {
        QString uriLoop = mountLoop->getRootFile()->uri();
        if (uri == uriLoop) {
            m_mounts.removeOne(mountLoop);
        }
    }

    //    QString path = mount->getRootFile()->path();
    //    QString devId = path.mid(path.lastIndexOf('_') + 1);
    //    if (!devId.isEmpty()) {
    //        QThread::sleep(1);
    //        emit sigMountDeviceDel(devId);
    //        qDebug() << "onVfsMountChangedRemove() unmount device id:" << devId;
    //    }
}

void DevMountControl::getAllDeviceName()
{
    QVariantMap options;
    QStringList blDevList = m_diskManager->blockDevices(options);
    for (const QString &blks : blDevList) {
        QSharedPointer<DBlockDevice> blk(DDiskManager::createBlockDevice(blks));
        QScopedPointer<DDiskDevice> drv(DDiskManager::createDiskDevice(blk->drive()));

        if (!blk->hasFileSystem() && !drv->mediaCompatibility().join(" ").contains("optical") && !blk->isEncrypted()) {
            continue;
        }
        if ((blk->hintIgnore() && !blk->isEncrypted()) || blk->cryptoBackingDevice().length() > 1) {
            continue;
        }
        DBlockDevice *pblk = blk.data();
        QByteArrayList mps = blk->mountPoints();
        qulonglong size = blk->size();
        QString label = blk->idLabel();
        QString fs = blk->idType();
        QString udispname = "";

        if (label.startsWith(ddeI18nSym)) {
            QString i18nKey = label.mid(ddeI18nSym.size(), label.size() - ddeI18nSym.size());
            udispname = qApp->translate("DeepinStorage", i18nMap.value(i18nKey, i18nKey.toUtf8().constData()));
            goto runend1;
        }

        if (mps.contains(QByteArray("/\0", 2))) {
            udispname = TrObject::getInstance()->getMountText(System_Disk);
            goto runend1;
        }
        if (label.length() == 0) {
            QScopedPointer<DDiskDevice> drv(DDiskManager::createDiskDevice(pblk->drive()));
            if (!drv->mediaAvailable() && drv->mediaCompatibility().join(" ").contains("optical")) {
                QString maxmediacompat;
                for (auto i = opticalmediakv.rbegin(); i != opticalmediakv.rend(); ++i) {
                    if (drv->mediaCompatibility().contains(i->first)) {
                        maxmediacompat = i->second;
                        break;
                    }
                }
                udispname = QCoreApplication::translate("DeepinStorage", "%1 Drive").arg(maxmediacompat);
                goto runend1;
            }
            if (drv->opticalBlank()) {
                udispname =
                    QCoreApplication::translate("DeepinStorage", "Blank %1 Disc").arg(opticalmediamap[drv->media()]);
                goto runend1;
            }
            if (pblk->isEncrypted() && !blk) {
                udispname = QCoreApplication::translate("DeepinStorage", "%1 Encrypted").arg(formatSize(size));
                goto runend1;
            }
            udispname = QCoreApplication::translate("DeepinStorage", "%1 Volume").arg(formatSize(size));
            goto runend1;
        }
        udispname = label;

    runend1:
        blk->mount({});
        QByteArrayList qbl = blk->mountPoints();
        QString mountPoint = "file://";
        for (QByteArray qb : qbl) {
            mountPoint += qb;
        }
        qDebug() << "mountPoint:" << mountPoint;
        QUrl qurl(mountPoint);
        m_durlAndNameMap[qurl] = udispname;
    }
}

const QList<QExplicitlySharedDataPointer<DGioMount>> DevMountControl::getVfsMountList()
{
    getAllDeviceName();
    QList<QExplicitlySharedDataPointer<DGioMount>> result;
    const QList<QExplicitlySharedDataPointer<DGioMount>> mounts = m_vfsManager->getMounts();

    for (auto mount : mounts) {
        // TODO:
        // Support android phone, iPhone, and usb devices. Not support ftp, smb, non removeable disk now
        QString scheme = QUrl(mount->getRootFile()->uri()).scheme();

        if ((scheme == "file" && mount->canEject()) || // usb device
            (scheme == "gphoto2") || // phone photo
            //            (scheme == "afc") ||                    //iPhone document
            (scheme == "mtp")) { // android file
            qDebug() << "getVfsMountList() mount.name" << mount->name() << " scheme type:" << scheme;
            result.append(mount);
        } else {
            qDebug() << mount->name() << " scheme type:" << scheme << "is not supported by album.";
        }
    }

    return result;
}

void DevMountControl::updateExternalDevice(QExplicitlySharedDataPointer<DGioMount> mount)
{
    QExplicitlySharedDataPointer<DGioFile> LocationFile = mount->getDefaultLocationFile();
    QString strPath = LocationFile->path();

    QString rename = "";
    rename = m_durlAndNameMap[QUrl(mount->getRootFile()->uri())];
    if ("" == rename) {
        rename = mount->name();
    }

    m_mounts.append(mount);
}
