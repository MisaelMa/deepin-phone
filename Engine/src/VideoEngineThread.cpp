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
#include "VideoEngineThread.h"

#include <QDebug>
#include <QPixmap>

#include "utils.h"

VideoEngineThread::VideoEngineThread(QObject *parent)
    : BaseEngineThread(parent)
{
}

void VideoEngineThread::getVideoImage()
{
    foreach (auto s, m_pFileList) {
        if (m_bIsExit == false) {
            return;
        }

        int nTime = 0;
        QPixmap pixmap;
        bool rl = Utils::_readVideo_ffmpeg_one(s.toLocal8Bit().data(), pixmap, nTime);
        if (rl) {
            emit sigImageBackLoaded(s, pixmap);
        }
    }
}

void VideoEngineThread::run()
{
    getVideoImage();
}
