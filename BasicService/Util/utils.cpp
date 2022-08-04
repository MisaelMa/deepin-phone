/*
 * Copyright (C) 2017 ~ 2018 Uniontech Software Technology Co., Ltd.
 *
 * Author:     rekols <rekols@foxmail.com>
 * Maintainer: rekols <rekols@foxmail.com>
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

#include "utils.h"
//#include "defines.h"

#include <QUrl>
#include <QDir>
//#include <QFile>
//#include <QDebug>
//#include <QFileInfo>
//#include <QFontInfo>
//#include <QMimeType>
//#include <QApplication>
//#include <QMimeDatabase>
#include <QStandardPaths>
//#include <QImageReader>
//#include <QPixmap>
//#include <QFile>
//#include <QFontDatabase>
//#include <QTextCodec>
//#include <QProcess>
#include <QDesktopServices>
#include <QTextCodec>
#include <QImageReader>
#include <DApplication>
#include <QProcess>
#include <QFont>
#include <DWidget>
#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <taglib/taglib.h>
#include <taglib/tpropertymap.h>

#include <unistd.h>
//#include <math.h>

#include <DDialog>
#include <QDebug>

//#include "GlobalDefine.h"
#include "TrObject.h"

DWIDGET_USE_NAMESPACE

//QHash<QString, QPixmap> Utils::m_imgCacheHash;
//QHash<QString, QString> Utils::m_fontNameCache;

extern "C" {
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
}

Utils::Utils()
{
}

//QString Utils::getQssContent(const QString &filePath)
//{
//    QFile file(filePath);
//    QString qss;

//    if (file.open(QIODevice::ReadOnly)) {
//        qss = file.readAll();
//    }

//    return qss;
//}

//QString Utils::getConfigPath()
//{
//    QDir dir(QDir(QStandardPaths::standardLocations(QStandardPaths::ConfigLocation).first())
//             .filePath(qApp->organizationName()));

//    return dir.filePath(qApp->applicationName());
//}

//bool Utils::isFontMimeType(const QString &filePath)
//{
//    const QString mimeName = QMimeDatabase().mimeTypeForFile(filePath).name();

//    if (mimeName.startsWith("font/") || mimeName.startsWith("application/x-font")) {
//        return true;
//    }

//    return false;
//}

//QString Utils::suffixList()
//{
//    return QString("Font Files (*.ttf *.ttc *.otf)");
//}

//QPixmap Utils::renderSVG(const QString &filePath, const QSize &size)
//{
//    if (m_imgCacheHash.contains(filePath)) {
//        return m_imgCacheHash.value(filePath);
//    }

//    QImageReader reader;
//    QPixmap pixmap;

//    reader.setFileName(filePath);

//    if (reader.canRead()) {
//        const qreal ratio = qApp->devicePixelRatio();
//        reader.setScaledSize(size * ratio);
//        pixmap = QPixmap::fromImage(reader.read());
//        pixmap.setDevicePixelRatio(ratio);
//    } else {
//        pixmap.load(filePath);
//    }

//    m_imgCacheHash.insert(filePath, pixmap);

//    return pixmap;
//}

//QString Utils::loadFontFamilyByType(FontType fontType)
//{
//    QString fontFileName = "";
//    switch (fontType) {
//    case SourceHanSansMedium:
//        fontFileName = ":/font/SourceHanSansCN-Medium.ttf";
//        break;
//    case SourceHanSansNormal:
//        fontFileName = ":/font/SourceHanSansCN-Normal.ttf";
//        break;
//    case DefautFont:
//        QFont font;
//        return font.family();
//    }

//    if (m_fontNameCache.contains(fontFileName)) {
//        return m_fontNameCache.value(fontFileName);
//    }

//    QString fontFamilyName = "";
//    QFile fontFile(fontFileName);
//    if (!fontFile.open(QIODevice::ReadOnly)) {
//        qDebug() << "Open font file error";
//        return fontFamilyName;
//    }

//    int loadedFontID = QFontDatabase::addApplicationFontFromData(fontFile.readAll());
//    QStringList loadedFontFamilies = QFontDatabase::applicationFontFamilies(loadedFontID);
//    if (!loadedFontFamilies.empty()) {
//        fontFamilyName = loadedFontFamilies.at(0);
//    }
//    fontFile.close();

//    m_fontNameCache.insert(fontFileName, fontFamilyName);
//    return fontFamilyName;
//}

//QFont Utils::loadFontBySizeAndWeight(QString fontFamily, int fontSize, int fontWeight)
//{
//    Q_UNUSED(fontSize)

//    QFont font(fontFamily);
//    font.setWeight(fontWeight);

//    return font;
//}

//void Utils::bindFontBySizeAndWeight(DWidget *widget, QString fontFamily, int fontSize, int fontWeight)
//{
//    QFont font = loadFontBySizeAndWeight(fontFamily, fontSize, fontWeight);
//    widget->setFont(font);

//    DFontSizeManager::SizeType sizeType = DFontSizeManager::T6;
//    switch (fontSize) {
//    case 10: {
//        sizeType = DFontSizeManager::T10;
//    } break;
//    case 11: {
//        sizeType = DFontSizeManager::T9;
//    } break;
//    case 12: {
//        sizeType = DFontSizeManager::T8;
//    } break;
//    case 13: {
//        sizeType = DFontSizeManager::T7;
//    } break;
//    case 14: {
//        sizeType = DFontSizeManager::T6;
//    } break;
//    case 17: {
//        sizeType = DFontSizeManager::T5;
//    } break;
//    case 20: {
//        sizeType = DFontSizeManager::T4;
//    } break;
//    case 24: {
//        sizeType = DFontSizeManager::T3;
//    } break;
//    case 30: {
//        sizeType = DFontSizeManager::T2;
//    } break;
//    case 40: {
//        sizeType = DFontSizeManager::T1;
//    } break;
//    }

//    DFontSizeManager *fontManager = DFontSizeManager::instance();
//    fontManager->bind(widget, sizeType, fontWeight);
//}

//int Utils::returnfileIsempty(QString strfilepath, QString strfilename)
//{
//    QDir dir(strfilepath);
//    QString filename = strfilename + ".postinst";
//    do {
//        if (!dir.exists()) {
//            qDebug() << "文件夹不存在";
//            return 0;
//        }
//        dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
//        QFileInfoList list = dir.entryInfoList();
//        int file_count = list.count();
//        qDebug() << "file_count                 " << file_count;
//        if (file_count <= 0) {
//            qDebug() << "当前文件夹为空";
//            return 0;
//        }
//        QStringList string_list;
//        for (int i = 0; i < list.count(); i++) {
//            QFileInfo file_info = list.at(i);
//            if (file_info.fileName() == filename) {
//                qDebug() << "文件路径：  " << file_info.path() << "           "
//                         << "文件名：  " << file_info.fileName();
//                break;
//            }
//        }
//    } while (0);
//    return 1;
//}

//QString Utils::fromSpecialEncoding(const QString &inputStr)
//{
//    bool bFlag = inputStr.contains(QRegExp("[\\x4e00-\\x9fa5]+"));
//    if (bFlag) {
//        return inputStr;
//    }

//    QTextCodec *codec = QTextCodec::codecForName("utf-8");
//    if (codec) {
//        QString unicodeStr = codec->toUnicode(inputStr.toLatin1());
//        return unicodeStr;
//    } else {
//        return inputStr;
//    }
//}

//bool Utils::File_transfer(QString Sourcefilepath, QString Targetfilepath, QString strfilename)
//{
//    QDir dir(Targetfilepath);
//    QString filename = strfilename + ".postinst";
//    QString File_transfer_Action1 = "";
//    QString File_transfer_Action2 = "";

//    File_transfer_Action1 = "mkdir " + Targetfilepath;
//    qDebug() << "创建文件夹：" << File_transfer_Action1;
//    system(File_transfer_Action1.toStdString().c_str());
//    File_transfer_Action2 = "cp " + Sourcefilepath + "/" + filename + " " + Targetfilepath;
//    system(File_transfer_Action2.toStdString().c_str());
//    qDebug() << "文件复制转移：" << File_transfer_Action2;
//    return true;
//}

//bool Utils::Modify_transferfile(QString Targetfilepath, QString strfilename)
//{
//    QDir dir(Targetfilepath);
//    QString filename = strfilename + ".postinst";
//    QString File_modify_Action = "";
//    File_modify_Action = "sed -i '1,$s/su /#su /g' " + Targetfilepath + "/" + filename;
//    qDebug() << "修改文件内容：" << File_modify_Action;
//    system(File_modify_Action.toStdString().c_str());
//    return true;
//}

//bool Utils::Return_Digital_Verify(QString strfilepath, QString strfilename)
//{
//    QDir dir(strfilepath);
//    if (!dir.exists()) {
//        qDebug() << "文件夹不存在";
//        return false;
//    }
//    dir.setFilter(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);
//    QFileInfoList list = dir.entryInfoList();
//    int file_count = list.count();
//    qDebug() << "file_count                 " << file_count;
//    if (file_count <= 0) {
//        qDebug() << "当前文件夹为空";
//        return false;
//    }
//    QStringList string_list;
//    for (int i = 0; i < list.count(); i++) {
//        QFileInfo file_info = list.at(i);
//        if (file_info.fileName() == strfilename) {
//            qDebug() << "文件路径：  " << file_info.path() << "           "
//                     << "文件名：  " << file_info.fileName();
//            return true;
//        }
//    }
//    return false;
//}
//bool Utils::Digital_Verify(QString filepath_name)
//{
//    QString verifyfilepath = "/usr/bin/";
//    QString verifyfilename = "deepin-deb-verify";
//    bool result_verify_file = Return_Digital_Verify(verifyfilepath, verifyfilename);
//    qDebug() << "result_verify_file" << result_verify_file;
//    if (result_verify_file) {
//        QProcess proc;
//        QString program = "/usr/bin/deepin-deb-verify ";
//        program = program + filepath_name;
//        proc.start(program);
//        qDebug() << "program:" << program;
//        proc.waitForFinished();
//        const QString output = proc.readAllStandardOutput();
//        const QString output1 = proc.readAllStandardError();
//        qDebug() << output1;
//        for (const auto &item : output1.split('\n'))
//            if (item.toLatin1() == "[INFO] signature verified!") {
//                qDebug() << "item:" << item;
//                return true;
//            }
//    }
//    return false;
//}

//QString Utils::holdTextInRect(const QFont &font, QString srcText, const QSize &size)
//{
//    bool bContainsChinese = srcText.contains(QRegExp("[\\x4e00-\\x9fa5]+"));

//    QString text;
//    QString tempText;
//    int totalHeight = size.height();
//    int lineWidth = size.width() - font.pixelSize();

//    int offset = bContainsChinese ? font.pixelSize() : 0;

//    QFontMetrics fm(font);

//    int calcHeight = 0;
//    int lineHeight = fm.height();
//    int lineSpace = 0;
//    int lineCount = (totalHeight - lineSpace) / lineHeight;
//    int prevLineCharIndex = 0;
//    for (int charIndex = 0; charIndex < srcText.size() && lineCount >= 0; ++charIndex) {
//        int fmWidth = fm.horizontalAdvance(tempText);
//        if (fmWidth > lineWidth - offset) {
//            calcHeight += lineHeight + 3;
//            if (calcHeight + lineHeight > totalHeight) {
//                QString endString = srcText.mid(prevLineCharIndex);
//                const QString &endText = fm.elidedText(endString, Qt::ElideRight, size.width());
//                text += endText;

//                lineCount = 0;
//                break;
//            }

//            if (!bContainsChinese) {
//                QChar currChar = tempText.at(tempText.length() - 1);
//                QChar nextChar = srcText.at(srcText.indexOf(tempText) + tempText.length());
//                if (currChar.isLetter() && nextChar.isLetter()) {
//                    tempText += '-';
//                }
//                fmWidth = fm.horizontalAdvance(tempText);
//                if (fmWidth > size.width()) {
//                    --charIndex;
//                    --prevLineCharIndex;
//                    tempText = tempText.remove(tempText.length() - 2, 1);
//                }
//            }
//            text += tempText;

//            --lineCount;
//            if (lineCount > 0) {
//                text += "\n";
//            }
//            tempText = srcText.at(charIndex);

//            prevLineCharIndex = charIndex;
//        } else {
//            tempText += srcText.at(charIndex);
//        }
//    }

//    if (lineCount > 0) {
//        text += tempText;
//    }

//    return text;
//}

QString Utils::mountGvfsPath()
{
    QString path = QString("/run/user/%1/gvfs").arg(getuid());
    return path;
}

/*
 * 获取Android挂载目录，挂载目录下有多个目录则返回挂载路径的根目录
 * 例：/run/user/1001/gvfs/mtp:host=HUAWEI_VTR-AL00_SJE5T17A19005084/
 * 若挂载路径下只有多个子文件夹，则返回该文件夹路径以/结尾
 * 若获取失败返回空串
*/
//QString Utils::androidDevMountPath(QString devId)
//{
//    QString devMntPath;
//    QString gvfsPath = mountGvfsPath();
//    QDir dir(gvfsPath);
//    if (!dir.exists()) {
//        return devMntPath;
//    }

//    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
//    dir.setSorting(QDir::DirsFirst);
//    QFileInfoList list = dir.entryInfoList();
//    for (int i = 0; i < list.size(); i++) {
//        QFileInfo fileInfo = list.at(i);
//        if (fileInfo.isDir()) {
//            QString fileName = fileInfo.fileName();
//            if (fileName.contains(devId, Qt::CaseInsensitive)) {
//                if (fileName.startsWith("gphoto2:")) {
//                    devMntPath = fileInfo.absoluteFilePath() + QDir::separator();
//                } else {
//                    QString subDir = fileInfo.absoluteFilePath();
//                    //判断根目录下的文件夹个数，如有多个文件夹，则返回根目录 + /
//                    QDir strPath(subDir);
//                    auto fileList = strPath.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
//                    if (fileList.size() == 1) {
//                        devMntPath = subDir;
//                    } else if (fileList.size() > 1) {
//                        devMntPath = subDir + QDir::separator();
//                    }
//                }
//                //devMntPath = fileInfo.absoluteFilePath();
//                break;
//            }
//        }
//    }

//    return devMntPath;
//}

//QString Utils::iOSDevMountPathDoc(QString devId)
//{
//    QString devMntPath;
//    QString gvfsPath = mountGvfsPath();
//    QDir dir(gvfsPath);
//    if (!dir.exists()) {
//        return devMntPath;
//    }

//    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
//    dir.setSorting(QDir::DirsFirst);
//    QFileInfoList list = dir.entryInfoList();
//    for (int i = 0; i < list.size(); i++) {
//        QFileInfo fileInfo = list.at(i);
//        if (fileInfo.isDir()) {
//            QString fileName = fileInfo.fileName();
//            if (fileName.contains(devId, Qt::CaseInsensitive)) {
//                if (fileName.startsWith("afc:")) {
//                    devMntPath = fileInfo.absoluteFilePath();
//                }
//                break;
//            }
//        }
//    }

//    return devMntPath;
//}

//QString Utils::iOSDevMountPathPhoto(QString devId)
//{
//    QString devMntPath;
//    QString gvfsPath = mountGvfsPath();
//    QDir dir(gvfsPath);
//    if (!dir.exists()) {
//        return devMntPath;
//    }

//    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
//    dir.setSorting(QDir::DirsFirst);
//    QFileInfoList list = dir.entryInfoList();
//    for (int i = 0; i < list.size(); i++) {
//        QFileInfo fileInfo = list.at(i);
//        if (fileInfo.isDir()) {
//            QString fileName = fileInfo.fileName();
//            if (fileName.contains(devId, Qt::CaseInsensitive)) {
//                if (fileName.startsWith("gphoto2:")) {
//                    devMntPath = fileInfo.absoluteFilePath();
//                    //进入DCIM
//                    //                    QDir strPath(devMntPath);
//                    //                    auto fileList = strPath.entryInfoList(QDir::Dirs | QDir::Files |
//                    //                    QDir::NoDotAndDotDot); if (fileList.size() == 1) {
//                    //                        devMntPath = fileList.at(0).absoluteFilePath();
//                    //                    }
//                }
//                break;
//            }
//        }
//    }

//    return devMntPath;
//}

//文件大小转换成可读的字符串，后加B,KB,....
QString Utils::dataSizeToString(const quint64 filesize)
{
    QStringList units;
    units << "B"
          << "KB"
          << "MB"
          << "GB"
          << "TB"
          << "PB";
    double mod = 1024.0;
    double size = filesize;
    int i = 0;

    while (size >= mod && i < units.count() - 1) {
        size /= mod;
        i++;
    }

    QString szResult = QString::number(size, 'd', 1);
    szResult += " " + units[i];
    return szResult;
}

//bool Utils::openUrlFile(QString &file)
//{
//    return QDesktopServices::openUrl(QUrl::fromLocalFile(file));

//    //    QString filePath = QString("file:///") + file;
//    //    filePath = filePath.replace("%", "%25");
//    //    filePath = filePath.replace("+", "%2B");
//    //    filePath = filePath.replace("?", "%3F");
//    //    filePath = filePath.replace("#", "%23");
//    //    filePath = filePath.replace("&", "%26");
//    //    return QDesktopServices::openUrl(QUrl(filePath, QUrl::TolerantMode));
//}

//bool Utils::isGBK(unsigned char *data, int len)
//{
//    int i = 0;
//    while (i < len) {
//        if (data[i] <= 0x7f) {
//            //编码小于等于127,只有一个字节的编码，兼容ASCII
//            i++;
//            continue;
//        } else {
//            //大于127的使用双字节编码
//            if (data[i] >= 0x81 && data[i] <= 0xfe && data[i + 1] >= 0x40 && data[i + 1] <= 0xfe
//                && data[i + 1] != 0xf7) {
//                i += 2;
//                continue;
//            } else {
//                return false;
//            }
//        }
//    }
//    return true;
//}

//int Utils::preNUm(unsigned char byte)
//{
//    unsigned char mask = 0x80;
//    int num = 0;
//    for (int i = 0; i < 8; i++) {
//        if ((byte & mask) == mask) {
//            mask = mask >> 1;
//            num++;
//        } else {
//            break;
//        }
//    }
//    return num;
//}

//bool Utils::isUtf8(unsigned char *data, int len)
//{
//    int num = 0;
//    int i = 0;
//    while (i < len) {
//        if ((data[i] & 0x80) == 0x00) {
//            // 0XXX_XXXX
//            i++;
//            continue;
//        } else if ((num = preNUm(data[i])) > 2) {
//            // 110X_XXXX 10XX_XXXX
//            // 1110_XXXX 10XX_XXXX 10XX_XXXX
//            // 1111_0XXX 10XX_XXXX 10XX_XXXX 10XX_XXXX
//            // 1111_10XX 10XX_XXXX 10XX_XXXX 10XX_XXXX 10XX_XXXX
//            // 1111_110X 10XX_XXXX 10XX_XXXX 10XX_XXXX 10XX_XXXX 10XX_XXXX
//            // preNUm() 返回首个字节8个bits中首�?0bit前面1bit的个数，该数量也是该字符所使用的字节数
//            i++;
//            for (int j = 0; j < num - 1; j++) {
//                //判断后面num - 1 个字节是不是都是10开
//                if ((data[i] & 0xc0) != 0x80) {
//                    return false;
//                }
//                i++;
//            }
//        } else {
//            //其他情况说明不是utf-8
//            return false;
//        }
//    }
//    return true;
//}

QString Utils::lengthString(qint64 length)
{
    length = length / 1000;
    int hour = static_cast<int>(length / 3600);

    QString mmStr = QString("%1").arg(length % 3600 / 60, 2, 10, QLatin1Char('0'));
    QString ssStr = QString("%1").arg(length % 60, 2, 10, QLatin1Char('0'));

    if (hour > 0) {
        return QString("%1:%2:%3").arg(hour).arg(mmStr).arg(ssStr);
    } else {
        return QString("%1:%2").arg(mmStr).arg(ssStr);
    }
}

void Utils::getMusicArtist(const QString &file, QString &artist, QString &length)
{
    Q_UNUSED(artist);
    //    //获取音乐 演唱者和时长
    TagLib::FileRef f(file.toStdString().c_str());
    //    TagLib::Tag *tag = f.tag();
    //    if (tag != nullptr) {
    //        artist = TStringToQString(tag->artist());
    //        //转换编码
    //        const char *cstr = artist.toStdString().c_str();
    //        char *str = const_cast<char *>(cstr);
    //        uchar *ucstrArtist = reinterpret_cast<uchar *>(str);

    //        if (isUtf8(ucstrArtist, artist.size())) {
    //            // UTF-8编码不用转
    //        } else {
    //            uchar *strgbk = new uchar[tag->artist().size()];
    //            uint strSize = tag->artist().size();
    //            memcpy(strgbk, tag->artist().toCString(), size_t(strSize));
    //            if (isGBK(strgbk, int(strSize))) {
    //                QTextCodec *codec = QTextCodec::codecForName("GB18030");
    //                if (codec != nullptr) {
    //                    artist = codec->toUnicode(f.tag()->artist().toCString());
    //                }
    //            }
    //            delete[] strgbk;
    //        }
    //    } else {
    //        artist = "";
    //    }

    TagLib::AudioProperties *t_audioProperties = f.audioProperties();
    if (t_audioProperties != nullptr) {
        length = lengthString(t_audioProperties->length() * 1000);
    } else { //  读取不到音频信息    aac
        AVFormatContext *pFormatCtx = avformat_alloc_context();
        avformat_open_input(&pFormatCtx, file.toStdString().c_str(), nullptr, nullptr);
        if (pFormatCtx) {
            avformat_find_stream_info(pFormatCtx, nullptr);
            int64_t duration = pFormatCtx->duration / 1000;
            if (duration > 0) {
                length = lengthString(duration);
            }

            avformat_close_input(&pFormatCtx);
            avformat_free_context(pFormatCtx);
        }
    }
}

//  pms 36879, modify by wzx, 2020-7-7
//bool Utils::execMsgBox(const QString &strText, DWidget *p, bool bShowCancel)
//{
//    bool rl = false;
//    DDialog *dlg = new DDialog(strText, "", p);
//    dlg->setIcon(QIcon(":images/drive-harddisk.svg"));
//    //dlg->setIcon(QIcon::fromTheme(PhoneAssistantModel::g_app_icon_name));
//    //    dlg->setWindowFlags(dlg.windowFlags() | Qt::WindowStaysOnTopHint);
//    if (bShowCancel) //add by zhangliangliang
//        dlg->addButton(TrObject::getInstance()->getDlgBtnText(Dlg_Btn_cancel), false, DDialog::ButtonNormal);

//    dlg->addButton(TrObject::getInstance()->getDlgBtnText(Dlg_Btn_Delete), true, DDialog::ButtonWarning);
//    int nRet = dlg->exec();
//    if (nRet == QDialog::Accepted) //选择了取消
//        rl = true;

//    delete dlg;

//    return rl;
//}

/**
 * @brief 打开文件、图片、视频
 * @param strPath
 */
void Utils::execOpenPath(const QString &strPath)
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(strPath));
}

void Utils::createFileName(QString &path, QString &name)
{
    QFileInfo file(path);
    QString newPath = path;
    QString baseName;
    QString basePath = file.absolutePath();
    QString nameExp = TrObject::getInstance()->getFileOperationText(file_copy);

    if (file.isDir()) {
        baseName = file.fileName();
    } else {
        QString fileName = file.fileName();
        if (fileName.contains("."))
            baseName = fileName.left(fileName.lastIndexOf("."));
        else
            baseName = file.baseName();
    }

    QString newName = baseName;

    uint i = 0;
    while (true && i < 1025) {
        //判断文件是否存在
        QFileInfo fileInfo(newPath);
        if (fileInfo.exists()) {
            QString suffix = fileInfo.suffix();
            //文件存在，重命名
            if (i > 0) {
                if (fileInfo.isDir()) {
                    newName = QString("%1(%2%3)").arg(baseName).arg(nameExp).arg(i);
                } else {
                    if (suffix.isEmpty())
                        newName = QString("%1(%2%3)").arg(baseName).arg(nameExp).arg(i);
                    else
                        newName = QString("%1(%2%3).%4").arg(baseName).arg(nameExp).arg(i).arg(suffix);
                }
            } else {
                if (fileInfo.isDir()) {
                    newName = QString("%1(%2)").arg(baseName).arg(nameExp);
                } else {
                    if (suffix.isEmpty())
                        newName = QString("%1(%2)").arg(baseName).arg(nameExp);
                    else
                        newName = QString("%1(%2).%3").arg(baseName).arg(nameExp).arg(suffix);
                }
            }

            i++;
            newPath = basePath + "/" + newName;

        } else {
            break;
        }
    }

    path = newPath;
    name = newName;

    return;
}

//void Utils::RemovePathPrefix(QString &path, int devType, QString devId)
//{
//    QString tmpPath;
//    if (devType == Mount_Ios) {
//        tmpPath = path.mid(path.indexOf(",port=") + 1);
//        path = tmpPath.mid(tmpPath.indexOf("/"));
//    } else {
//        QStringList pathLst = path.split(devId);
//        if (pathLst.size() > 1)
//            path = pathLst.at(1);
//    }
//}

QString Utils::ElideText(QFont font, int width, QString strInfo)
{
    QFontMetrics fontMetrics(font);
    //如果当前字体下，字符串长度大于指定宽度
    if (fontMetrics.width(strInfo) > width) {
        strInfo = QFontMetrics(font).elidedText(strInfo, Qt::ElideMiddle, width);
    }
    return strInfo;
}

//DebApplicationHelper *DebApplicationHelper::instance()
//{
//    return qobject_cast<DebApplicationHelper *>(DGuiApplicationHelper::instance());
//}

//#define CAST_INT static_cast<int>

//static QColor light_qpalette[QPalette::NColorRoles] {
//    QColor("#414d68"), // WindowText
//    QColor("#e5e5e5"), // Button
//    QColor("#e6e6e6"), // Light
//    QColor("#e5e5e5"), // Midlight
//    QColor("#001A2E"), // Dark  -- changed origin is #e3e3e3
//    QColor("#e4e4e4"), // Mid
//    QColor("#414d68"), // Text
//    Qt::black, // BrightText
//    QColor("#414d68"), // ButtonText
//    Qt::white, // Base
//    QColor("#f8f8f8"), // Window
//    QColor(0, 0, 0, CAST_INT(0.05 * 255)), // Shadow
//    QColor("#0081ff"), // Highlight
//    QColor(0, 45, 255, CAST_INT(0.5 * 255)), // HighlightedText   //old Qt::white
//    QColor("#0082fa"), // Link
//    QColor("#ad4579"), // LinkVisited
//    QColor(0, 0, 0, CAST_INT(0.03 * 255)), // AlternateBase
//    Qt::white, // NoRole
//    QColor(255, 255, 255, CAST_INT(0.8 * 255)), // ToolTipBase
//    QColor("#526A7F") // ToolTipText -- changed origin is Qt::black
//};

//static QColor dark_qpalette[QPalette::NColorRoles] {
//    QColor("#c0c6d4"), // WindowText
//    QColor("#444444"), // Button
//    QColor("#484848"), // Light
//    QColor("#474747"), // Midlight
//    QColor("#C0C6D4"), // Dark      -- changed origin is #414141
//    QColor("#434343"), // Mid
//    QColor("#c0c6d4"), // Text
//    Qt::white, // BrightText
//    QColor("#c0c6d4"), // ButtonText
//    QColor(255, 255, 255, CAST_INT(0.05 * 255)), // Base --- changed origin is #282828
//    QColor("#252525"), // Window
//    QColor(0, 0, 0, CAST_INT(0.05 * 255)), // Shadow
//    QColor("#095EFF"), // Highlight         //old : #0081ff
//    QColor("#0059D2"), // HighlightedText   //old: b8d3ff
//    QColor("#0082fa"), // Link
//    QColor("#ad4579"), // LinkVisited
//    QColor(0, 0, 0, CAST_INT(0.05 * 255)), // AlternateBase
//    Qt::black, // NoRole
//    QColor(45, 45, 45, CAST_INT(0.8 * 255)), // ToolTipBase
//    QColor("#6D7C88") // ToolTipText -- changed origin is #c0c6d4
//};

//static QColor light_dpalette[DPalette::NColorTypes] {
//    QColor(), // NoType
//    QColor(0, 0, 0, CAST_INT(255 * 0.03)), // ItemBackground
//    QColor("#414d68"), // TextTitle
//    QColor("#609DC8"), // TextTips --- changed origin is #526A7F
//    QColor("#FF5A5A"), // TextWarning -- changed origin is #FF5736
//    QColor("#7C7C7C"), // TextLively  -- changed origin is #0082FA
//    QColor("#417505"), // LightLively -- changed origin is #25b7ff
//    QColor("#47790C"), // DarkLively -- changed origin is #0098ff
//    QColor(0, 0, 0, CAST_INT(0.03 * 255)) // FrameBorder
//};

//static QColor dark_dpalette[DPalette::NColorTypes] {
//    QColor(), // NoType
//    QColor(255, 255, 255, CAST_INT(255 * 0.05)), // ItemBackground
//    QColor("#c0c6d4"), // TextTitle
//    QColor("#6D7C88"), // TextTips
//    QColor("#9A2F2F"), // TextWarning -- changed origin is #FF5736
//    QColor("#7C7C7C"), // TextLively -- changed origin is #0082FA
//    QColor("#417505"), // LightLively -- changed origin is #0056c1
//    QColor("#47790C"), // DarkLively  -- changed origin is #004c9c
//    QColor(0, 0, 0, CAST_INT(0.08 * 255)) // FrameBorder
//};

//DPalette DebApplicationHelper::standardPalette(DGuiApplicationHelper::ColorType type) const
//{
//    DPalette *pa;
//    const QColor *qcolor_list, *dcolor_list;

//    if (type == DarkType) {
//        pa = new DPalette();
//        qcolor_list = dark_qpalette;
//        dcolor_list = dark_dpalette;
//    } else {
//        pa = new DPalette();
//        qcolor_list = light_qpalette;
//        dcolor_list = light_dpalette;
//    }

//    for (int i = 0; i < DPalette::NColorRoles; ++i) {
//        QPalette::ColorRole role = static_cast<QPalette::ColorRole>(i);

//        QColor color = qcolor_list[i];
//        pa->setColor(DPalette::Active, role, color);
//        generatePaletteColor(*pa, role, type);
//    }

//    for (int i = 0; i < DPalette::NColorTypes; ++i) {
//        DPalette::ColorType role = static_cast<DPalette::ColorType>(i);

//        QColor color = dcolor_list[i];
//        pa->setColor(DPalette::Active, role, color);
//        generatePaletteColor(*pa, role, type);
//    }

//    return *const_cast<const DPalette *>(pa);
//}

//DPalette DebApplicationHelper::palette(const DWidget *widget, const QPalette &base) const
//{
//    Q_UNUSED(base)

//    DPalette palette;

//    do {
//        // 存在自定义palette时应该根据其自定义的palette获取对应色调的DPalette
//        const QPalette &wp = widget->palette();

//        palette = standardPalette(toColorType(wp));

//        // 关注控件palette改变的事件
//        const_cast<DWidget *>(widget)->installEventFilter(const_cast<DebApplicationHelper *>(this));
//    } while (false);

//    return palette;
//}

//void DebApplicationHelper::setPalette(DWidget *widget, const DPalette &palette)
//{
//    // 记录此控件被设置过palette
//    widget->setProperty("_d_set_palette", true);
//    widget->setPalette(palette);
//}

//void DebApplicationHelper::resetPalette(DWidget *widget)
//{
//    widget->setProperty("_d_set_palette", QVariant());
//    widget->setAttribute(Qt::WA_SetPalette, false);
//}

//DebApplicationHelper::DebApplicationHelper()
//{
//}

//DebApplicationHelper::~DebApplicationHelper()
//{
//}

//bool DebApplicationHelper::eventFilter(QObject *watched, QEvent *event)
//{
//    return DGuiApplicationHelper::eventFilter(watched, event);
//}

QPixmap Utils::readDiskPicture(const QString &path)
{
    QPixmap pixmap;
    QImageReader reader;
    reader.setFileName(path);
    reader.setFormat(Utils::DetectImageFormat(path).toLatin1());
    reader.setAutoTransform(true);
    QSize size = reader.size();

    if (reader.canRead()) {
        const qreal ratio = qApp->devicePixelRatio();
        reader.setScaledSize(size * ratio);
        pixmap = QPixmap::fromImage(reader.read());
        pixmap.setDevicePixelRatio(ratio);
    } else {
        pixmap.load(path);
    }

    return pixmap;
}

QPixmap Utils::resizeFilePicture(const QPixmap &icon)
{
    if (icon.isNull())
        return icon;

    QPixmap newIcon;

    if (icon.width() > icon.height()) {
        newIcon = icon.scaled(QSize(96, 64), Qt::KeepAspectRatio);
    } else {
        newIcon = icon.scaled(QSize(64, 96), Qt::KeepAspectRatio);
    }

    return newIcon;
}

QPixmap Utils::getResourcePixmap(const QString &filename)
{
    QPixmap pixmap;
    qreal devicePixelRatio = qApp->devicePixelRatio();

    qreal ratio = 1.0;
    //    qreal ratio = 0.5;
    if (!qFuzzyCompare(ratio, devicePixelRatio)) {
        QImageReader reader;
        reader.setFileName(qt_findAtNxFile(filename, devicePixelRatio, &ratio));
        if (reader.canRead()) {
            reader.setScaledSize(reader.size() * (devicePixelRatio / ratio));
            pixmap = QPixmap::fromImage(reader.read());
            pixmap.setDevicePixelRatio(devicePixelRatio);
        }
    } else {
        pixmap.load(filename);
    }

    return pixmap;
}

QString Utils::DetectImageFormat(const QString &filepath)
{
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly)) {
        return "";
    }

    //    const QByteArray data = file.read(1024);
    const QByteArray data = file.read(32);
    file.close();

    // Check bmp file.
    if (data.startsWith("BM")) {
        return "bmp";
    }

    // Check dds file.
    if (data.startsWith("DDS")) {
        return "dds";
    }

    // Check gif file.
    if (data.startsWith("GIF8")) {
        return "gif";
    }

    // Check Max OS icons file.
    if (data.startsWith("icns")) {
        return "icns";
    }

    // Check jpeg file.
    if (data.startsWith("\xff\xd8")) {
        return "jpg";
    }

    // Check mng file.
    if (data.startsWith("\x8a\x4d\x4e\x47\x0d\x0a\x1a\x0a")) {
        return "mng";
    }

    // Check net pbm file (BitMap).
    if (data.startsWith("P1") || data.startsWith("P4")) {
        return "pbm";
    }

    // Check pgm file (GrayMap).
    if (data.startsWith("P2") || data.startsWith("P5")) {
        return "pgm";
    }

    // Check ppm file (PixMap).
    if (data.startsWith("P3") || data.startsWith("P6")) {
        return "ppm";
    }

    // Check png file.
    if (data.startsWith("\x89PNG\x0d\x0a\x1a\x0a")) {
        return "png";
    }

    // Check svg file.
    if (data.indexOf("<svg") > -1) {
        return "svg";
    }

    // TODO(xushaohua): tga file is not supported yet.

    // Check tiff file.
    if (data.startsWith("MM\x00\x2a") || data.startsWith("II\x2a\x00")) {
        // big-endian, little-endian.
        return "tiff";
    }

    // TODO(xushaohua): Support wbmp file.

    // Check webp file.
    if (data.startsWith("RIFFr\x00\x00\x00WEBPVP")) {
        return "webp";
    }
    if (data.startsWith("RIFF\x00V\x00\x00WEBPVP8")) {
        return "webp";
    }

    // Check xbm file.
    if (data.indexOf("#define max_width ") > -1 && data.indexOf("#define max_height ") > -1) {
        return "xbm";
    }

    // Check xpm file.
    if (data.startsWith("/* XPM */")) {
        return "xpm";
    }

    return "";
}

/**
 * @brief 处理adb命令中文件名的特殊字符
 * @param[in]
 * @param[out]
 * @return
 * @author zhangliangliang
 * @date 2020-07-25
 * @copyright (c) 2020
 */
void Utils::adbSpecialCharProc(QString &str)
{
    str.replace(' ', "\\ ");
    str.replace('(', "\\(");
    str.replace(')', "\\)");
    str.replace('&', "\\&");
    str.replace(';', "\\;");
    str.replace("'", "\\'");
}

QString Utils::execCmdNoResult(const QString &cmd, const int &msecs)
{
    QProcess p;
    p.setProcessChannelMode(QProcess::MergedChannels);
    p.start(cmd);
    p.waitForStarted();
    p.waitForFinished(msecs);
    return QString::fromLocal8Bit(p.readAllStandardOutput()).trimmed();
}

int Utils::writeCommand(const QString &strCommand, QString &strOutMsg, QString &strErrMsg, const int &msecs)
{
    QProcess p;
    p.start(strCommand);
    p.waitForStarted();
    p.closeWriteChannel(); //关闭写通道 ，解决未响应问题
    if (p.waitForFinished(msecs) == false) {
        strErrMsg = p.errorString();
    } else {
        strOutMsg = QString::fromLocal8Bit(p.readAllStandardOutput()).trimmed();
    }
    int nExitCode = p.exitCode();
    return nExitCode;
}

//直接从网上复制的该函数有问题，不能直接使用，例文件大小为5223639B时转换是错误的
/**
 * @brief 网上搜， 整数  转 文件大小
 * @param inSize
 * @return
 */
//QString Utils::dataSizeToString(const quint64 &inSize)
//{
//    QStringList units = QStringList() << "B"
//                                      << "KB"
//                                      << "MB"
//                                      << "GB"
//                                      << "TB"
//                                      << "PB";
//    double mod = 1024.0;
//    double size = inSize;
//    //qDebug() << size;
//    int i = 0;
//    long rest = 0;
//    while (size >= mod && i < units.count() - 1) {
//        rest = static_cast<long>(size) % static_cast<long>(mod);
//        size /= mod;
//        i++;
//    }
//    QString szResult = QString::number(floor(size));
//    if (rest > 0) {
//        szResult += QString(".") + QString::number(rest).left(2);
//    }
//    szResult += " " + units[i];
//    return szResult;
//}

/**
 * @brief 获取Android挂载路径，含首层文件夹
 * @author zhangliangliang
 */
//获取Android挂载路径，含首层文件夹
//例：/run/user/1001/gvfs/mtp:host=HUAWEI_HLK-AL00_C7YVB20423006594/内部存储空间
// /run/user/1001/gvfs/gphoto2:host=Xiaomi_TRINKET-IDP__SN%3A09974841_1898d22f
QString Utils::getAndroidMountRootPath(QString devId)
{
    QString devMntPath;
    QString gvfsPath = mountGvfsPath();
    QDir dir(gvfsPath);
    if (!dir.exists()) {
        return devMntPath;
    }

    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::DirsFirst);
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); i++) {
        QFileInfo fileInfo = list.at(i);
        if (fileInfo.isDir()) {
            QString fileName = fileInfo.fileName();
            if (fileName.contains(devId, Qt::CaseInsensitive)) {
                if (fileName.startsWith("gphoto2:")) {
                    devMntPath = fileInfo.absoluteFilePath();
                } else {
                    QString subDir = fileInfo.absoluteFilePath();
                    //如果挂载目录下有多个文件夹，则选择一个返回
                    QDir strPath(subDir);
                    auto fileList = strPath.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
                    if (fileList.size() == 1) {
                        devMntPath = fileList.at(0).absoluteFilePath();
                    } else if (fileList.size() > 1) {
                        for (int i = 0; i < fileList.size(); i++) {
                            if (!fileList[i].fileName().startsWith("sdcard", Qt::CaseInsensitive)) {
                                devMntPath = fileList[i].absoluteFilePath();
                                break;
                            }
                        }
                    }
                }
                break;
            }
        }
    }

    return devMntPath;
}

/**
 * @brief 把挂载路径转成adb用形式路径
 * @author zhangliangliang
 */
QString Utils::mountPathToAdbPath(QString mountPath, QString filePath)
{
    //把挂载形式路径转为adb用路径
    //例：/run/user/1001/gvfs/mtp:host=Xiaomi_TRINKET-IDP__SN%3A09974841_1898d22f/内部存储设备/1/a.txt
    //转为：/sdcard/1/a.txt

    QString adbFilePath = filePath;
    adbFilePath = adbFilePath.replace(mountPath, "/sdcard");
    qDebug() << __FUNCTION__ << adbFilePath;
    return adbFilePath;
}

/**
 * @brief 判断Android版本是否低于8，低于8返回true,否则返回false
 * @author zhangliangliang
 */
bool Utils::isLowVersionAndroid(QString version)
{
    //判断安卓系统版本分别处理 8.0以下版本不使用ADB
    bool isLowAndroid = false;
    QString sysVer = version.mid(version.indexOf(' ') + 1).trimmed();
    qDebug() << __FUNCTION__ << version;
    QString majorVer = sysVer.split('.').at(0);
    if (majorVer.toInt() < 8)
        isLowAndroid = true; //8.0以下版本，不使用ADB

    return isLowAndroid;
}

/**
 * @brief adb命令检查文件是否存在，存在返回true,不存在返回false
 * @param[in] devId: 设备ID
 * @param[in]adbFilePath: 手机中adb文件路径
 * @return
 * @author zhangliangliang
 */
bool Utils::adbCheckFileExists(QString devId, QString adbFilePath)
{
    //处理adb路径中的特殊字符
    Utils::adbSpecialCharProc(adbFilePath);

    QString cmd = QString("adb -s %1 shell ls \"%2\"").arg(devId).arg(adbFilePath);
    qDebug() << __FUNCTION__ << cmd;

    QString cmdResult = execCmdNoResult(cmd);
    //如返回的字符串有“No such file or directory”则认为文件不存在
    if (cmdResult.endsWith("No such file or directory")) {
        return false;
    }

    return true;
}

/**
 * @brief adb方式向手机传输文件
 * @param[in] devId: 设备ID
 * @param[in] srcPath: 本地路径，要上传的文件路径
 * @param[in] dstPath_adb：手机中的adb路径
 * @author zhangliangliang
 */
int Utils::adbPush(QString devId, QString srcPath, QString dstPath_adb)
{
    QString cmd = QString("adb -s %1 push \"%2\" \"%3\"").arg(devId).arg(srcPath).arg(dstPath_adb);
    qDebug() << __FUNCTION__ << cmd;

    int ret = QProcess::execute(cmd);
    if (ret == 0) {
        qDebug() << __FUNCTION__ << "adb push OKKKK>>>>>>>>>>>>>>>>";
    } else {
        qDebug() << __FUNCTION__ << "adb push ERROR>>>>>>>>>>>>>>>>";
    }

    return ret;
}

/**
 * @brief adb方式从手机下载文件
 * @param[in] devId: 设备ID
 * @param[in] srcPath_adb: 手机中adb文件路径
 * @param[in] dstPath：本地路径
 * @author zhangliangliang
 */
int Utils::adbPull(QString devId, QString srcPath_adb, QString dstPath)
{
    QString cmd = QString("adb -s %1 pull \"%2\" \"%3\"").arg(devId).arg(srcPath_adb).arg(dstPath);
    qDebug() << __FUNCTION__ << cmd;

    int ret = QProcess::execute(cmd);
    if (ret == 0) {
        qDebug() << __FUNCTION__ << "adb pull OKKKK>>>>>>>>>>>>>>>>";
    } else {
        qDebug() << __FUNCTION__ << "adb pull ERROR>>>>>>>>>>>>>>>>";
    }

    return ret;
}

/**
 * @brief adb方式从手机删除文件
 * @param[in] devId: 设备ID
 * @param[in] path: 手机中adb文件路径
 * @author zhangliangliang
 */
int Utils::adbDeleteFile(QString devId, QString adbFilepath)
{
    //处理特殊字符
    Utils::adbSpecialCharProc(adbFilepath);

    QString cmd = QString("adb -s %1 shell rm -f \"%2\"").arg(devId).arg(adbFilepath);
    qDebug() << __FUNCTION__ << cmd;

    int ret = QProcess::execute(cmd);
    if (ret == 0) {
        qDebug() << __FUNCTION__ << "adb shell rm OKKKK>>>>>>>>>>>>>>>>";
    } else {
        qDebug() << __FUNCTION__ << "adb shell rm ERROR>>>>>>>>>>>>>>>>";
    }

    return ret;
}

/**
 * @brief adb方式在手机新建文件夹
 * @param[in] devId: 设备ID
 * @param[in] path: 手机中adb文件路径
 * @author zhangliangliang
 */
int Utils::adbMakeDir(QString devId, QString adbFilepath)
{
    //处理特殊字符
    Utils::adbSpecialCharProc(adbFilepath);

    QString cmd = QString("adb -s %1 shell mkdir -p \"%2\"").arg(devId).arg(adbFilepath);
    qDebug() << __FUNCTION__ << cmd;

    int ret = QProcess::execute(cmd);
    if (ret == 0) {
        qDebug() << __FUNCTION__ << "adb shell mkdir OKKKK>>>>>>>>>>>>>>>>";
    } else {
        qDebug() << __FUNCTION__ << "adb shell mkdir ERROR>>>>>>>>>>>>>>>>";
    }

    return ret;
}

QString Utils::getValueFromMsg(const QString &strMsg, const QString strKey, const QString &separator)
{
    QString retVal = "";
    QStringList list_msg = strMsg.split(QRegExp(separator), QString::SkipEmptyParts);
    foreach (QString item_msg, list_msg) {
        if (!item_msg.contains(strKey))
            continue;
        item_msg = item_msg.trimmed();
        retVal = item_msg.right(item_msg.length() - strKey.length() - 1).trimmed();
        break;
    }

    return retVal;
}

bool Utils::_readVideo_ffmpeg_one(const char *input, QPixmap &pixmap, int &nTime)
{
    // 1.注册所有组件
    //    av_register_all();        wzx 不用也可以
    //封装格式上下文，统领全局的结构体，保存了视频文件封装格式的相关信息
    AVFormatContext *pFormatCtx = nullptr; // avformat_alloc_context();
    //        AVFormatContext *pFormatCtx = NULL;
    // 2.打开输入视频文件
    //    printf("fileName:%s\n", input);
    int nRet = avformat_open_input(&pFormatCtx, input, nullptr, nullptr);
    if (nRet != 0) {
        qDebug("%s", "无法打开输入视频文件");
        char buf[1024] = "";
        av_strerror(nRet, buf, 1024);
        printf("Couldn't open file %s: %d(%s)\n", input, nRet, buf);
        return false;
    }
    // 3.获取视频文件信息
    if (avformat_find_stream_info(pFormatCtx, nullptr) < 0) {
        qDebug("%s", "无法获取视频文件信息");
        if (pFormatCtx != nullptr) {
            avformat_close_input(&pFormatCtx); //关闭
            avformat_free_context(pFormatCtx); //释放内存
        }
        return false;
    }
    //获取视频流的索引位置
    //遍历所有类型的流（音频流、视频流、字幕流），找到视频流
    int v_stream_idx = -1;
    unsigned int i = 0;
    // number of streams
    for (; i < pFormatCtx->nb_streams; i++) {
        //流的类型
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            v_stream_idx = int(i);
            break;
        }
    }
    if (v_stream_idx == -1) {
        qDebug("%s", "找不到视频流");
        if (pFormatCtx != nullptr) {
            avformat_close_input(&pFormatCtx); //关闭
            avformat_free_context(pFormatCtx); //释放内存
        }
        return false;
    }
    //只有知道视频的编码方式，才能够根据编码方式去找到解码器
    //获取视频流中的编解码上下文
    AVCodecParameters *pCodecPara = pFormatCtx->streams[v_stream_idx]->codecpar;
    // 4.根据编解码上下文中的编码id查找对应的解码
    AVCodec *pCodec = avcodec_find_decoder(pCodecPara->codec_id);
    if (pCodec == nullptr) {
        qDebug("%s", "找不到解码器");
        if (pFormatCtx != nullptr) {
            avformat_close_input(&pFormatCtx); //关闭
            avformat_free_context(pFormatCtx); //释放内存
        }
        return false;
    }

    AVCodecContext *pCodecCtx = avcodec_alloc_context3(pCodec); //需要使用avcodec_free_context释放

    //事实上codecpar包含了大部分解码器相关的信息，这里是直接从AVCodecParameters复制到AVCodecContext
    avcodec_parameters_to_context(pCodecCtx, pCodecPara);

    // 5.打开解码器
    if (avcodec_open2(pCodecCtx, pCodec, nullptr) < 0) {
        qDebug("%s", "解码器无法打开");
        if (pFormatCtx != nullptr) {
            avformat_close_input(&pFormatCtx); //关闭
            avformat_free_context(pFormatCtx); //释放内存
        }
        if (pCodecCtx != nullptr) {
            avcodec_free_context(&pCodecCtx); //释放内存
        }
        return false;
    }
    //输出视频信息
    //            qDebug("视频的文件格式：%s",pFormatCtx->iformat->name);
    //            qDebug("视频时长：%d", (pFormatCtx->duration)/1000000);//s
    nTime = int((pFormatCtx->duration) / 1000000);
    //            qDebug("视频的宽高：%d,%d",pCodecCtx->width,pCodecCtx->height);
    //            qDebug("解码器的名称：%s",pCodec->name);
    //准备读取
    // AVPacket用于存储一帧一帧的压缩数据（H264）
    //缓冲区，开辟空间
    AVPacket *packet = reinterpret_cast<AVPacket *>(av_malloc(sizeof(AVPacket)));
    // AVFrame用于存储解码后的像素数据(YUV)
    //内存分配
    AVFrame *pFrame = av_frame_alloc();
    // YUV420
    AVFrame *pFrameYUV = av_frame_alloc();
    //只有指定了AVFrame的像素格式、画面大小才能真正分配内存
    //缓冲区分配内存
    uint8_t *out_buffer = static_cast<uint8_t *>(av_malloc(
        static_cast<size_t>(av_image_get_buffer_size(AV_PIX_FMT_RGB32, pCodecCtx->width, pCodecCtx->height, 1))));
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, out_buffer, AV_PIX_FMT_RGB32, pCodecCtx->width,
                         pCodecCtx->height, 1);

    //用于转码（缩放）的参数，转之前的宽高，转之后的宽高，格式等
    struct SwsContext *sws_ctx =
        sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
                       AV_PIX_FMT_BGRA, SWS_BICUBIC, nullptr, nullptr, nullptr);
    int frame_count = 0; // 帧
    //    6.一帧一帧的读取压缩数据

    while (av_read_frame(pFormatCtx, packet) >= 0) {
        //只要视频压缩数据（根据流的索引位置判断）
        if (packet->stream_index == v_stream_idx) {
            // 7.解码一帧视频压缩数据，得到视频像素数据
            int ret = avcodec_send_packet(pCodecCtx, packet);
            if (ret == 0) {
                ret = avcodec_receive_frame(pCodecCtx, pFrame);
            }
            if (ret < 0) {
                //                qDebug("%s", "解码错误");
                continue;
            }

            //为0说明解码完成，非0正在解码
            // AVFrame转为像素格式YUV420，宽高
            // 2 6输入、输出数据
            // 3 7输入、输出画面一行的数据的大小 AVFrame 转换是一行一行转换的
            // 4 输入数据第一列要转码的位置 从0开始
            // 5 输入画面的高度
            sws_scale(sws_ctx, pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data,
                      pFrameYUV->linesize);

            //把这个RGB数据 用QImage加载
            QImage image(static_cast<uchar *>(out_buffer), pCodecCtx->width, pCodecCtx->height, QImage::Format_RGB32);
            image = image.scaled(512, 512);

            QString tempFile = QString("/tmp/phone-assistant-video.png");
            if (image.save(tempFile)) { //   保存为临时文件， 之后在读取

                pixmap = QPixmap(tempFile);

                QFile::remove(tempFile); //   读取之后删除
            } else {
                qDebug() << "save video png failed";
            }
            frame_count++;
        }

        if (frame_count == 1)
            break;
    }

    sws_freeContext(sws_ctx);

    av_free(out_buffer);
    av_frame_free(&pFrameYUV);
    av_frame_free(&pFrame);
    av_free(packet);

    avcodec_free_context(&pCodecCtx);
    avcodec_close(pCodecCtx);

    avformat_close_input(&pFormatCtx);
    avformat_free_context(pFormatCtx);
    return true;
}

/**
 * @brief 从文件路径中取出文件名
 * @param[in] path: 文件路径
 * @param[out] fileName: 文件名
 * @author zhangliangliang
 */
QString Utils::getFileNameByPath(QString path)
{
    QString fileName;
    if (path.contains("/")) {
        if (path.endsWith("/")) {
            fileName = "";
        } else {
            fileName = path.mid(path.lastIndexOf("/") + 1);
        }
    } else {
        fileName = path;
    }
    return fileName;
}
