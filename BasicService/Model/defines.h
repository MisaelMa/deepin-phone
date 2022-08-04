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
#ifndef DEFINES_H
#define DEFINES_H

#include <QPixmap>
#include <QIcon>
#include <QString>
#include <QList>
#include <QDateTime>
#include <QMetaType>
#include <QFileInfo>

#define EBOOK_DIR "PhoneAssistant/EBook"
#define MUSIC_DIR "PhoneAssistant/Music"

//设备类型
enum DEVICE_TYPE { Mount_Ios = 0,
                   Mount_Android,
                   Mount_OTHER = 999
};

//设备连接或删除
enum OPERATE_TYPE { OPT_ADD = 0,
                    OPT_DEL
};

//设备USB连接方式
enum UsbConnType { USB_MTP = 0, //传文件（MTP）方式
                   USB_PTP, //传照片（PTP）方式
                   USB_MIDI, //MIDI设备
                   USB_OTHER, //其它
                   Usb_ios
};

//设备连接信息
typedef struct tagDevConn {
    DEVICE_TYPE deviceType; //设备类型 IOS设备、Android设备等
    QString strDeviceId; //设备连接后的唯一标识
    int operate; //操作类型 连接、断开
    bool bAuthorize; //是否授权
    bool bUSBDebug; //是否开启USB调试
    UsbConnType usbType = USB_OTHER; //Android Usb连接方式
} DEV_CONN_INFO;

Q_DECLARE_METATYPE(DEV_CONN_INFO)

//设备状态
enum DEVICE_STATUS {
    STATUS_DISCONNECTED, //断开连接
    STATUS_CONNECTED, //已连接上
    STATUS_LOADING, //正在加载
    STATUS_UNOPEN_DEBUGMODE, //未开启调试模式
    STATUS_UNAUTHORIZE, //未开启USB调试授权
    STATUS_UNLOCK, //未解锁
    STATUS_OTHER = 999,
    STATUS_APKINSTALL_ERROR,
    STATUS_APK_INSTALLING
};

//手机应用信息
struct PhoneAppInfo {
    QString strAppName = ""; //应用名
    QString strPacketName = ""; //安装包名称
    //    QString strIconPath = ""; //图表路径
    quint64 nAppSize = 0; //应用大小
    QString strAppVersion = ""; //应用版本
};
Q_DECLARE_METATYPE(PhoneAppInfo)

//手机信息
struct PhoneInfo {
    DEVICE_TYPE type = Mount_OTHER;
    DEVICE_STATUS status = STATUS_OTHER;
    QString strPhoneID = "";
    QString strDevName = "";
    QString strBrand = ""; //品牌
    QString strProductType = ""; //手机型号
    QString strSysName = ""; //系统名称
    QString strProductVersion = ""; //系统版本
    QString strRegionInfo = ""; //销售地区
    QString strBatteryCycleCount = ""; //充电次数
    int battery = 0; //电量
    quint64 nMemTotal = 0; //运行内存 总量 kB
    quint64 nMemFree = 0; //运行内存 可用 kB
    //    quint64 nSysTotal = 0; //系统存储空间 总量
    //    quint64 nSysFree = 0; // 系统存储空间 可用
    QString strDiskFree = ""; //数据内存
    //    QList<PhoneAppInfo> listApp; //应用
    UsbConnType usbType = USB_OTHER; //Android Usb：连接方式

    bool operator==(const PhoneInfo &other) const
    {
        return this->strPhoneID == other.strPhoneID;
    }
};

Q_DECLARE_METATYPE(PhoneInfo)

//struct PhoneSysInfo {
//    quint64 sysTotalSize = 0; //手机系统存储大小
//    quint64 sysAvailSize = 0; //手机系统可用大小
//    quint64 sysMemSize = 0; //手机内存大小
//    quint64 sysMemAvailSize = 0; //手机内存可用大小
//    QString sysPhoneMode = ""; //手机型号
//};

////设备信息展示
//enum E_Widget_Type {
//    SHOW_MAIN = 0, //主界面
//    E_Widget_App, //应用
//    E_Widget_Photo, //照片
//    E_Widget_Video, //视频
//    E_Widget_Music, //音乐
//    E_Widget_Book, //电子书
//    E_Widget_File //文件管理
//};

//enum ADB_CMD_TYPE { //CMD_START_APP = 0,
//    CMD_COPY_ICON = 0, //拷贝路径
//    CMD_DELETE_ICON, //删除路径
//    CMD_UNINSTALL_APP, //卸载APP
//    CMD_EXPORT_APP, //导出APP
//    //    CMD_GET_BATTERY //获取电量
//};

//enum ADB_CMD_ERRNO { CMD_ERR_NORMAL = 0,
//                     CMD_ERR_ERROR
//};

enum FileType {
    FT_No_Define,
    FT_DIR,
    FT_FILE,
    FT_MUSIC,
    FT_EBOOK,
    FT_ALL
};

//  widget 类别
enum E_Widget_Type {
    E_No_Define,
    //    E_Widget_Main,
    E_Widget_App,
    E_Widget_Photo,
    E_Widget_Music,
    E_Widget_Video,
    E_Widget_File,
    E_Widget_Book
};

//文件信息
struct FILE_INFO {
    QFileInfo fileInfo; //文件信息
    QIcon icon; //文件图标
};

Q_DECLARE_METATYPE(FILE_INFO);

enum PhoneFileType {
    UNKNOW = 1, //未知
    DEFAULT = 2, //默认
    IMAGE = 4, //图片
    VIDEO = 8, //视频
};

struct PhoneFileInfo {
    PhoneFileType type; //类型
    QString phoneID; //手机关联
    QString name; //名称
    QString dirPath; //所属文件夹路径
    QString path; //路径
    QIcon scaleIcon; //缩小图标
    //    QPixmap originalIcon; //原图图标
    bool damaged; //图标损坏
    bool bIsDir; //文件夹
    int time; //视频时间 seconds
    int fileCount; //如果是文件夹 记录文件数量
    quint64 size; //文件大小
    //    quint64 lastModifiedTime_t;
    //    QString lastModified; //最后修改时间

    bool operator==(const PhoneFileInfo &other) const
    {
        return this->path == other.path;
    }
};
Q_DECLARE_METATYPE(PhoneFileInfo);

enum E_File_OptionType {
    File_Undefine,
    //        FILE_CMD,
    FILE_NEW,
    FILE_DEL,
    FILE_PASTE,
    FILE_IMPORT,
    FILE_EXPORT,
    FILE_DISPLAY,
    FILE_DIRINFO,
    //    FILE_GETPATH,
    FILE_DIRSINFO,
    FILE_ICON,
    FILE_IMAGE,
    FILE_VIDEOIMG
};

#endif // DEFINES_H
