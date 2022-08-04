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
#include "phoneconnectthread.h"

#include <QTimer>
#include <QProcess>
#include <QMap>

#include "MountService.h"
#include "Ios/iphonemountcontrol.h"
#include "Android/devmountcontrol.h"

PhoneConnectThread::PhoneConnectThread(QObject *parent)
    : QThread(parent)
{
    m_bHaveDevice = false;
    m_adbCheck = true;

    m_androidDevCheckNum = 0;

    //挂载安卓手机 修复PMS46633 系统文管没开时，插拔手机不会挂载问题
    m_devMountCtrl = new DevMountControl();
}

PhoneConnectThread::~PhoneConnectThread()
{
    if (m_devMountCtrl != nullptr)
        delete m_devMountCtrl;

    //全部取消挂载
    IPhoneMountControl::unmountAll();

    m_bIsCanRun = false;
    quit();
    wait();
}

void PhoneConnectThread::run()
{
    while (1) {
        if (m_bIsCanRun == false)
            break;

        iosDeviceList();
        checkUsbAndroidDevice();
        androidDeviceList();
        checkAllDeviceNum();

        QThread::msleep(500);
    }

    //    QTimer *devCheckTimer = new QTimer(this);
    //    devCheckTimer->setInterval(500);
    //    connect(devCheckTimer, &QTimer::timeout, this, &PhoneConnectThread::slotDeviceCheck);
    //    devCheckTimer->start();
    //    exec();

    //    devCheckTimer->stop();
    //    delete devCheckTimer;
}

//void PhoneConnectThread::slotDeviceCheck()
//{
//    iosDeviceList();
//    checkUsbAndroidDevice();
//    androidDeviceList();
//    checkAllDeviceNum();
//}

//void PhoneConnectThread::slotNewDeviceFind(QString devId)
//{
//    if (!m_mountFindDevIDList.contains(devId)) {
//        m_mountFindDevIDList.append(devId);

//        if (!m_androidDevIDList.contains(devId)) {
//            //没有打开USB调试
//            DEV_CONN_INFO devConnInfo;
//            devConnInfo.operate = OPT_ADD;
//            devConnInfo.deviceType = Mount_Android;
//            devConnInfo.strDeviceId = devId;
//            devConnInfo.bUSBDebug = false;
//            devConnInfo.bAuthorize = false;
//            emit sigDeviceListChanged(devConnInfo);
//        }

//        m_bHaveDevice = true;
//        qDebug() << __LINE__ << __FUNCTION__ << " DeviceID:" << devId;
//    }
//}

//void PhoneConnectThread::slotMountDeviceDel(QString devId)
//{
//    if (m_mountFindDevIDList.contains(devId)) {
//        m_mountFindDevIDList.removeOne(devId);

//        if (!m_androidDevIDList.contains(devId)) {
//            DEV_CONN_INFO devConnInfo;
//            devConnInfo.operate = OPT_DEL;
//            devConnInfo.deviceType = Mount_Android;
//            devConnInfo.strDeviceId = devId;
//            emit sigDeviceListChanged(devConnInfo);
//        }

//        qDebug() << __LINE__ << __FUNCTION__ << " DeviceID:" << devId;
//    }
//}

void PhoneConnectThread::androidDeviceList()
{
    //    if (!m_adbCheck) //不需要进入adb操作
    //        return;

    QString cmdResult = MountService::getService()->getDevices(Mount_Android);

    if (!cmdResult.trimmed().startsWith("List of devices attached")) {
        //命令结果异常
        qDebug() << "adb devices error.";
        return;
    }

    if (cmdResult.contains("daemon not running")) {
        MountService::getService()->runAdbServer();
        return;
    }

    // Android设备 连接ID如下：
    //$ adb devices
    // List of devices attached
    // 88AKBML223HV    device         //88AKBML223HV 为设备标识ID

    QVector<QString> devIdTmp;
    auto strList = cmdResult.split('\n', QString::SkipEmptyParts);

    //没有Android设备
    if (strList.size() <= 1) {
        //所有设备已断开
        if (!m_androidDevIDList.isEmpty()) {
            qDebug() << __LINE__ << __FUNCTION__ << "Delect all android devices.";
            foreach (auto id, m_androidDevIDList) {
                delAndroidDevice(id);
            }
        }
        checkAllDeviceNum();
        return;
    }

    //bool allDevAuthorized = true; //标记是否所有设备都已授权

    for (int i = 1; i < strList.size(); i++) {
        auto devInfo = strList[i].split('\t', QString::SkipEmptyParts);
        if (devInfo.size() < 2)
            continue;

        QString devId = devInfo[0]; //取出设备标识ID
        if (devId.startsWith("emulator")) {
            //屏蔽安卓模拟器容器 ，只显示在USB列表中的安卓设备
            continue;
        }
        devIdTmp.append(devId);
        QString strAuthorize = devInfo[1].trimmed(); //取出设备标识，是否有权限

        DEV_CONN_INFO devConnInfo;
        devConnInfo.strDeviceId = devId;
        devConnInfo.operate = OPT_ADD;
        devConnInfo.deviceType = Mount_Android;
        devConnInfo.bUSBDebug = true; //usb调试已打开
        //devConnInfo.bAuthorize = false;

        //判断有无授权
        if (strAuthorize.compare("device") == 0) {
            devConnInfo.bAuthorize = true; //已授权
        } else {
            devConnInfo.bAuthorize = false;
        }

        //获取手机连接方式 //MTP，PTP
        QString adbResult = MountService::getService()->getAdbUsbState(devId);

        QString usbTypeStr;
        if (adbResult.contains("mtp", Qt::CaseInsensitive)) {
            devConnInfo.usbType = USB_MTP;
            usbTypeStr = "USB Type(mtp)";
        } else if (adbResult.contains("ptp", Qt::CaseInsensitive)) {
            devConnInfo.usbType = USB_PTP;
            usbTypeStr = "USB Type(ptp)";
        } else if (adbResult.contains("midi", Qt::CaseInsensitive)) {
            devConnInfo.usbType = USB_MIDI;
            usbTypeStr = "USB Type(midi)";
        } else {
            devConnInfo.usbType = USB_OTHER;
            usbTypeStr = "USB Type(other)";
        }

        if (strAuthorize.startsWith("no permissions")) { //魅族手机X8 MIDI的时候
            devConnInfo.usbType = USB_MIDI;
            devConnInfo.bUSBDebug = false;
            usbTypeStr = "USB Type(midi)";
        }

        //        bool isUpdate = false;
        //        //MIDI方式下取消授权时
        //        if (strAuthorize.contains("unauthorized") && adbResult.contains("unauthorized")) {
        //            devConnInfo.usbType = USB_MIDI;
        //            usbTypeStr = "USB Type(midi)";
        //            isUpdate = true;
        //        }

        //        if (strAuthorize.compare("device") == 0) {
        //            devConnInfo.bUSBDebug = true;
        //            devConnInfo.bAuthorize = true;
        //        } else if (strAuthorize.compare("unauthorized") == 0) {
        //            devConnInfo.bUSBDebug = true;
        //            devConnInfo.bAuthorize = false;
        //        } else if (strAuthorize.startsWith("no permissions")) { //魅族手机X8 选充电或MIDI的时候
        //            devConnInfo.bUSBDebug = false;
        //            devConnInfo.bAuthorize = false;
        //        } else if (strAuthorize.startsWith("offline")) { //honor 可能存在的情况
        //            devConnInfo.bUSBDebug = true;
        //            devConnInfo.bAuthorize = false;
        //        }

        //判断有没有挂载成功
        //        QString path = Utils::androidDevMountPath(devId);
        //        if (path.isEmpty()) {
        //            //没有挂载路径
        //            devConnInfo.usbType = USB_OTHER;
        //        }

        //发送信息通知主界面
        if (!m_androidDevIDList.contains(devId)) {
            m_androidDevIDList.append(devId);
            m_devUsbInfoList.insert(devId, devConnInfo);
            m_bHaveDevice = true;

            //新发现设备 若不在USB列表中，发送添加信号，否发送更新权限信号
            if (m_usbAndroidList.contains(devId)) {
                emit sigDeviceAuthorezeChanged(devConnInfo);
            } else {
                emit sigDeviceListChanged(devConnInfo);
            }
            qDebug() << __LINE__ << __FUNCTION__ << "Adb Android DeviceID:" << devId << " is connected, authorized(" << devConnInfo.bAuthorize << "), " << usbTypeStr;
        } else {
            //判断USB连接方式，授权是否变化
            DEV_CONN_INFO oldDevSubInfo = m_devUsbInfoList.value(devId);
            if (oldDevSubInfo.usbType != devConnInfo.usbType || oldDevSubInfo.bAuthorize != devConnInfo.bAuthorize) {
                emit sigDeviceAuthorezeChanged(devConnInfo);
                m_devUsbInfoList.insert(devId, devConnInfo);
                qDebug() << __LINE__ << __FUNCTION__ << "Adb Android DeviceID:" << devId << " is connected, authorized(" << devConnInfo.bAuthorize << "), " << usbTypeStr;
            }
        }

        //adb devices 一直运行可能会影响其它adb命令执行，此处改为手机正常连接后不再执行此循环检查
        //if (!devConnInfo.bUSBDebug || (devConnInfo.usbType != USB_MTP || devConnInfo.usbType != USB_PTP)) {
        //    if (!devConnInfo.bAuthorize)
        //        allDevAuthorized = false;
        //}

        //        if (!m_androidDevIDList.contains(devId)) {
        //            m_androidDevIDList.append(devId);
        //            m_devAuthorizeList.insert(devId, devConnInfo.bAuthorize);
        //            if (m_mountFindDevIDList.contains(devId)) {
        //                emit sigDeviceAuthorezeChanged(devConnInfo);
        //            } else {
        //                emit sigDeviceListChanged(devConnInfo);
        //            }
        //            m_bHaveDevice = true;
        //            qDebug() << __LINE__ << __FUNCTION__ << "Adb Android DeviceID:" << devId << " is connected, authorized(" << devConnInfo.bAuthorize << ").";
        //        }

        //        //授权变化，发出信号
        //        if (m_devAuthorizeList.value(devId) != devConnInfo.bAuthorize) {
        //            m_devAuthorizeList.insert(devId, devConnInfo.bAuthorize);
        //            emit sigDeviceAuthorezeChanged(devConnInfo);
        //            qDebug() << __LINE__ << __FUNCTION__ << "Android DeviceID:" << devId << " is connected, authorized(" << devConnInfo.bAuthorize << ").";
        //        }
    }

    //所有设备检查完后判断是否有未授权设备，若全都授权则不再调用此函数，否则继续调用
    //if (allDevAuthorized)
    //    m_adbCheck = false;

    //检查是否有设备拔出
    if (!m_androidDevIDList.isEmpty()) {
        foreach (auto id, m_androidDevIDList) {
            if (!devIdTmp.contains(id)) {
                delAndroidDevice(id);
            }
        }
    }
}

void PhoneConnectThread::iosDeviceList()
{
    QString cmdResult = MountService::getService()->getDevices(Mount_Ios);

    // IOS设备 连接ID如下：
    //$ idevice_id -l
    // 428e0bbc3fe62ecd147d602eff558bd07b339214   //设备UUID

    //没有IOS设备
    // ERROR: Unable to retrieve device list!
    if (cmdResult.startsWith("ERROR:", Qt::CaseInsensitive)) {
        //所有设备已断开
        if (!m_iosDevIDList.isEmpty()) {
            qDebug() << __LINE__ << __FUNCTION__ << "Delect all ios devices.";
            foreach (auto id, m_iosDevIDList) {
                delIOSDevice(id);
            }
        }
        checkAllDeviceNum();
        return;
    }

    QVector<QString> devIdTmp;
    auto strList = cmdResult.split('\n', QString::SkipEmptyParts);

    for (int i = 0; i < strList.size(); i++) {
        QString devId = strList[i]; //取出设备标识ID
        devIdTmp.append(devId);

        //  只处理 唯一的苹果设备id
        if (m_strIosDeviceId == "" || m_strIosDeviceId == devId) {
            m_strIosDeviceId = devId;

            DEV_CONN_INFO devConnInfo;
            devConnInfo.strDeviceId = devId;
            devConnInfo.deviceType = Mount_Ios;
            devConnInfo.operate = OPT_ADD;
            devConnInfo.bUSBDebug = true;
            devConnInfo.bAuthorize = true;
            devConnInfo.usbType = Usb_ios;

            //获取设备类型信息
            QString devType = "";
            cmdResult = MountService::getService()->getProductType(devId);
            devType = cmdResult.trimmed();
            if (devType.startsWith("No device found")) {
                continue;
            } else if (devType.startsWith("ERROR: Could not connect to lockdownd, error code -21")) {
                //没有权限，可能需要信任，解锁
                devConnInfo.bAuthorize = false;
            }

            // mount
            //防止手机运行中取消授权
            if (devConnInfo.bAuthorize) {
                //只挂载一次
                if (!m_iosMountList.contains(devId)) {
                    //挂载
                    if (IPhoneMountControl::mount(devId)) {
                        m_iosMountList.append(devId);
                    } else {
                        //没有权限，可能需要信任，解锁
                        devConnInfo.bAuthorize = false;
                    }
                }
            }

            if (!m_iosDevIDList.contains(devId)) {
                m_iosDevIDList.append(devId);
                m_devAuthorizeList.insert(devId, devConnInfo.bAuthorize);
                emit sigDeviceListChanged(devConnInfo);
                m_bHaveDevice = true;
                qDebug() << __LINE__ << __FUNCTION__ << "IOS DeviceID:" << devId << " is connected, authorized(" << devConnInfo.bAuthorize << ").";
            }

            //授权变化，发出信号
            if (m_devAuthorizeList.value(devId) != devConnInfo.bAuthorize) {
                m_devAuthorizeList.insert(devId, devConnInfo.bAuthorize);
                emit sigDeviceAuthorezeChanged(devConnInfo);
                qDebug() << __LINE__ << __FUNCTION__ << "IOS DeviceID:" << devId << " is connected, authorized(" << devConnInfo.bAuthorize << ").";
            }
        }
    }

    //检查是否有设备拔出
    if (!m_iosDevIDList.isEmpty()) {
        foreach (auto id, m_iosDevIDList) {
            if (!devIdTmp.contains(id)) {
                delIOSDevice(id);
            }
        }
    }
}

void PhoneConnectThread::delIOSDevice(QString id)
{
    if (m_strIosDeviceId == id) {
        m_strIosDeviceId = "";
    }

    qDebug() << "IOS DeviceID:" << id << " is disconnected.";
    m_iosDevIDList.removeOne(id);
    m_devAuthorizeList.remove(id);
    m_devUsbInfoList.remove(id);

    DEV_CONN_INFO devConnInfo;
    devConnInfo.strDeviceId = id;
    devConnInfo.deviceType = Mount_Ios;
    devConnInfo.operate = OPT_DEL;
    devConnInfo.usbType = Usb_ios;
    emit sigDeviceListChanged(devConnInfo);

    //unmount
    IPhoneMountControl::unmount(id);
    m_iosMountList.removeOne(id);
}

void PhoneConnectThread::delAndroidDevice(QString id)
{
    qDebug() << "Android DeviceID:" << id << " is disconnected.";
    m_androidDevIDList.removeOne(id);
    m_devAuthorizeList.remove(id);
    //m_mountFindDevIDList.removeOne(id);
    m_devUsbInfoList.remove(id);

    DEV_CONN_INFO devConnInfo;
    devConnInfo.strDeviceId = id;
    devConnInfo.operate = OPT_DEL;
    devConnInfo.deviceType = Mount_Android;
    emit sigDeviceListChanged(devConnInfo);

    m_usbAndroidList.removeOne(id);
}

//读取所有USB设备
bool PhoneConnectThread::loadHwinfoUSBInfo()
{
    QProcess proc;
    proc.start("hwinfo --usb");
    proc.waitForStarted();
    proc.waitForFinished(60000); //1分钟超时

    QString hwOut = QString::fromLocal8Bit(proc.readAllStandardOutput()).trimmed();
    if (hwOut.size() < 1) {
        return false;
    }

    m_usbInfoMap.clear();

    // hciconfig
    //DatabaseMap hwinfoDatabase_;
    QStringList secondOrder;

    QMap<QString, QString> DeviceInfoMap;
    QString deviceName;
    int startIndex = 0;

    for (int i = 0; i < hwOut.size(); ++i) {
        if (hwOut[i] != '\n' && i != hwOut.size() - 1) {
            continue;
        }

        QString line = hwOut.mid(startIndex, i - startIndex);
        startIndex = i + 1;

        if (i == hwOut.size() - 1 || line.trimmed().isEmpty()) {
            if (deviceName.isEmpty() == false) {
                m_usbInfoMap[deviceName] = DeviceInfoMap;
                secondOrder.push_back(deviceName);
            }

            DeviceInfoMap.clear();
            deviceName = "";
            continue;
        }

        if (line.startsWith(Devicetype_HwInfo_Fourspace)) {
            int index = line.indexOf(": ");
            if (index > 0) {
                if (line.trimmed().contains(Devicetype_HwInfo_Resolution)) {
                    if (DeviceInfoMap.contains(Devicetype_HwInfo_Currentresolution)) {
                        //DeviceInfoMap[Devicetype_HwInfo_Currentresolution] += ", ";
                        //DeviceInfoMap[Devicetype_HwInfo_Currentresolution] +=line.mid(index+1).trimmed();
                    } else {
                        DeviceInfoMap[Devicetype_HwInfo_Currentresolution] = line.mid(index + 1).trimmed();
                    }

                    continue;
                }
                if (false == DeviceInfoMap.contains(line.mid(0, index).trimmed())) {
                    DeviceInfoMap[line.mid(0, index).trimmed()] = line.mid(index + 1).trimmed();
                }
            }
            continue;
        }

        if (line.startsWith(Devicetype_HwInfo_Twospace)) {
            int index = line.indexOf(": ");
            if (index > 0) {
                if (line.contains(Devicetype_HwInfo_Resolution)) {
                    if (DeviceInfoMap.contains(Devicetype_HwInfo_ResolutionList)) {
                        DeviceInfoMap[Devicetype_HwInfo_ResolutionList] += ", ";
                        DeviceInfoMap[Devicetype_HwInfo_ResolutionList] += line.mid(index + 1).trimmed();
                    } else {
                        DeviceInfoMap[Devicetype_HwInfo_ResolutionList] = line.mid(index + 1).trimmed();
                    }

                    continue;
                }

                DeviceInfoMap[line.mid(0, index).trimmed()] = line.mid(index + 1).trimmed();
            }
            continue;
        }

        deviceName = line.trimmed();
    }

    //toolDatabase_["USB"] = hwinfoDatabase_;
    secondOrder.removeDuplicates();
    //toolDatabaseSecondOrder_["USB"] = secondOrder;
    return true;
}

//检测出USB安卓设备
void PhoneConnectThread::checkUsbAndroidDevice()
{
    if (!loadHwinfoUSBInfo()) {
        return;
    }

    QVector<QString> devIdTmp;
    for (auto it = m_usbInfoMap.begin(); it != m_usbInfoMap.end(); it++) {
        if (it.key().endsWith("device")) {
            QString model = it.value().value("Model");
            model.replace('"', "");
            QString driver = it.value().value("Driver");
            driver.replace('"', "");
            if (!model.startsWith("Apple") && driver == "usbfs") {
                QString serialID = it.value().value("Serial ID");
                serialID.replace('"', "");
                if (!serialID.isEmpty()) {
                    devIdTmp.append(serialID);
                    if (!m_usbAndroidList.contains(serialID)) {
                        m_usbAndroidList.append(serialID);

                        if (!m_androidDevIDList.contains(serialID)) {
                            //没有打开USB调试
                            DEV_CONN_INFO devConnInfo;
                            devConnInfo.operate = OPT_ADD;
                            devConnInfo.deviceType = Mount_Android;
                            devConnInfo.strDeviceId = serialID;
                            devConnInfo.bUSBDebug = false;
                            devConnInfo.bAuthorize = false;
                            emit sigDeviceListChanged(devConnInfo);
                        }

                        m_adbCheck = true; //进行adb检查
                        m_bHaveDevice = true;
                        qDebug() << __LINE__ << __FUNCTION__ << "model:" << model << " driver:" << driver << "serialID:" << serialID;
                    }
                }
            }
        }
    }

    //检查是否有设备拔出
    if (!m_usbAndroidList.isEmpty()) {
        foreach (auto id, m_usbAndroidList) {
            if (!devIdTmp.contains(id)) {
                //sleep(1);
                delAndroidDevice(id);
            }
        }
    }
}

//检查所连接的设备个数，为0时，通过界面显示没有设备连接界面
void PhoneConnectThread::checkAllDeviceNum()
{
    if (m_bHaveDevice && m_usbAndroidList.isEmpty() && m_androidDevIDList.isEmpty() && m_iosDevIDList.isEmpty()) {
        //没有设备连接
        emit sigNoDeviceList();
        m_bHaveDevice = false;
    }
}
