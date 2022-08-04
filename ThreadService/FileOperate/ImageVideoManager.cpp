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
#include "ImageVideoManager.h"

QVector<PhoneFileInfo> ImageVideoManager::m_mapDirFileInfo_photo;
QVector<PhoneFileInfo> ImageVideoManager::m_mapDirFileInfo_video;

ImageVideoManager::ImageVideoManager()
{
}

void ImageVideoManager::clearCache(const int &mode, const QString &strPhoneID)
{
    if (mode == 0) {
        QVector<PhoneFileInfo>::iterator itr = m_mapDirFileInfo_photo.begin();
        while (itr != m_mapDirFileInfo_photo.end()) {
            PhoneFileInfo &item = *itr;
            if (item.dirPath.contains(strPhoneID)) {
                itr = m_mapDirFileInfo_photo.erase(itr);
            } else {
                itr++;
            }
        }
    } else {
        QVector<PhoneFileInfo>::iterator itr = m_mapDirFileInfo_video.begin();
        while (itr != m_mapDirFileInfo_video.end()) {
            PhoneFileInfo &item = *itr;
            if (item.dirPath.contains(strPhoneID)) {
                itr = m_mapDirFileInfo_video.erase(itr);
            } else {
                itr++;
            }
        }
    }
}
