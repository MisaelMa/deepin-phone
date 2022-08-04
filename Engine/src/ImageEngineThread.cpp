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
#include "ImageEngineThread.h"

#include <QDebug>
#include <QImageReader>
#include <QFileInfo>
#include <QPixmap>

#include "utils.h"

ImageEngineThread::ImageEngineThread(QObject *parent)
    : BaseEngineThread(parent)
{
}

void ImageEngineThread::getImage()
{
    foreach (auto s, m_pFileList) {
        if (m_bIsExit == false)
            return;

        QImage tImg;

        QString format = Utils::DetectImageFormat(s);
        if (format.isEmpty()) {
            QImageReader reader(s);
            reader.setAutoTransform(true);
            if (reader.canRead()) {
                tImg = reader.read();
            }
        } else {
            if (m_bIsExit == false)
                return;
            QImageReader readerF(s, format.toLatin1());
            readerF.setAutoTransform(true);
            if (readerF.canRead()) {
                tImg = readerF.read();
            } else {
                qWarning() << "can't read image:" << readerF.errorString() << format;
                tImg = QImage(s);
            }
        }

        if (tImg.isNull()) {
            QImageReader readerG(s, QFileInfo(s).suffix().toLatin1());
            if (readerG.canRead())
                tImg = readerG.read();
        }

        if (m_bIsExit == false)
            return;

        QPixmap pixmap = QPixmap::fromImage(tImg);
        if (0 != pixmap.height() && 0 != pixmap.width() && (pixmap.height() / pixmap.width()) < 10
            && (pixmap.width() / pixmap.height()) < 10) {
            if (pixmap.height() != 100 && pixmap.width() != 100) {
                if (pixmap.height() >= pixmap.width()) {
                    pixmap = pixmap.scaledToWidth(100, Qt::FastTransformation);
                } else if (pixmap.height() <= pixmap.width()) {
                    pixmap = pixmap.scaledToHeight(100, Qt::FastTransformation);
                }
            }
        }
        if (pixmap.isNull()) {
            pixmap = QPixmap::fromImage(tImg);
        }

        if (m_bIsExit == false) {
            return;
        }

        emit sigImageBackLoaded(s, pixmap);
    }
}

void ImageEngineThread::run()
{
    getImage();
}
