/*
 * Copyright (C) 2017 ~ 2018 Uniontech Software Technology Co., Ltd.
 *
 * Author:     rekols <rekols@foxmail.com>
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

#ifndef UTILS_H
#define UTILS_H

#include <QObject>
//#include <QHash>
//#include <DWidget>

//#include <DPalette>
//#include <DApplicationHelper>
//#include <DFontSizeManager>
//#include <QDir>
//#include <DDialog>
//#include <QPushButton>
//#include <DPushButton>
//#define dApp (static_cast<DApplication *>(QCoreApplication::instance()))

//DWIDGET_USE_NAMESPACE

//class DebApplicationHelper : public DGuiApplicationHelper
//{
//    Q_OBJECT

//public:
//    static DebApplicationHelper *instance();

//    DPalette standardPalette(DGuiApplicationHelper::ColorType type) const;
//    DPalette palette(const DWidget *widget, const QPalette &base = QPalette()) const;
//    void setPalette(DWidget *widget, const DPalette &palette);
//    void resetPalette(DWidget *widget);

//private:
//    DebApplicationHelper();
//    ~DebApplicationHelper() override;

//    bool eventFilter(QObject *watched, QEvent *event) override;
//};

class Utils
{
public:
    Utils();

    //    enum FontType { SourceHanSansMedium,
    //                    SourceHanSansNormal,
    //                    DefautFont
    //                  };

    //    static QHash<QString, QPixmap> m_imgCacheHash;
    //    static QHash<QString, QString> m_fontNameCache;

    //    static QString getQssContent(const QString &filePath);
    //    static QString getConfigPath();
    //    static bool isFontMimeType(const QString &filePath);
    //    static QString suffixList();
    //    static QPixmap renderSVG(const QString &filePath, const QSize &size);
    //    static QString loadFontFamilyByType(FontType fontType);
    //    static QFont loadFontBySizeAndWeight(QString fontFamily, int fontSize, int fontWeight);
    //    static void bindFontBySizeAndWeight(DWidget *widget, QString fontFamily, int fontSize, int fontWeight);
    //    static QString fromSpecialEncoding(const QString &inputStr);
    //    static QString holdTextInRect(const QFont &font, QString srcText, const QSize &size);
    //    static int returnfileIsempty(QString strfilepath, QString strfilename);
    //    static bool Modify_transferfile(QString Targetfilepath, QString strfilename);
    //    static bool File_transfer(QString Sourcefilepath, QString Targetfilepath, QString strfilename);
    //    static bool Digital_Verify(QString filepath_name);
    //    static bool Return_Digital_Verify(QString strfilepath, QString strfilename);
    static QString mountGvfsPath();
    //    static QString androidDevMountPath(QString devId);
    //    static QString iOSDevMountPathDoc(QString devId);
    //    static QString iOSDevMountPathPhoto(QString devId);
    static QString dataSizeToString(quint64 filesize); // size to M/MB/GB
    static void getMusicArtist(const QString &file, QString &artist, QString &length);
    //    static bool execMsgBox(const QString &strText, DWidget *p, bool bShowCancel = true); //?????? true??????
    static void execOpenPath(const QString &strPath); //  ?????? ????????????????????????
    static void createFileName(QString &path, QString &name);
    //    static void RemovePathPrefix(QString &path, int devType, QString devId);
    static QString ElideText(QFont font, int width, QString strInfo);
    static QString lengthString(qint64 length);
    //    static bool isUtf8(unsigned char *data, int len);
    //    static int preNUm(unsigned char byte);
    //    static bool isGBK(unsigned char *data, int len);

    static QPixmap readDiskPicture(const QString &path);
    static QPixmap resizeFilePicture(const QPixmap &icon);

    static QPixmap getResourcePixmap(const QString &filename);
    static QString DetectImageFormat(const QString &filepath);

    static QString execCmdNoResult(const QString &cmd, const int &msecs = 30000);
    static int writeCommand(const QString &strCommand, QString &strOutMsg, QString &strErrMsg, const int &msecs = 30000);

    //static QString dataSizeToString(const quint64 &size);

    static QString getAndroidMountRootPath(QString devId); //????????????????????????????????????
    static QString mountPathToAdbPath(QString mountPath, QString filePath); //?????????????????????adb???????????????
    static void adbSpecialCharProc(QString &str); //??????adb ??????????????????????????? //add by zhangll
    static bool isLowVersionAndroid(QString version); //??????Android??????????????????8?????????8??????true,????????????false
    static bool adbCheckFileExists(QString devId, QString adbFilePath); //adb?????????????????????????????? //add by zhangll
    static int adbPush(QString devId, QString srcPath, QString dstPath); //adb???????????????????????????
    static int adbPull(QString devId, QString srcPath_adb, QString dstPath); //adb???????????????????????????
    static int adbDeleteFile(QString devId, QString adbFilepath); //adb???????????????????????????
    static int adbMakeDir(QString devId, QString adbFilepath); //adb?????????????????????

    static QString getValueFromMsg(const QString &strMsg, const QString strKey, const QString &separator = "[\r\n]");
    static bool _readVideo_ffmpeg_one(const char *input, QPixmap &pixmap, int &nTime);

    static QString getFileNameByPath(QString path);
};

#endif
