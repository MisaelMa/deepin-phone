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
#include "PMFileIconProvider.h"

#include "FileModel.h"

#include <QDebug>
#include <QImageReader>
#include <QDir>
#include <QApplication>

PMFileIconProvider::PMFileIconProvider()
{
}

QIcon PMFileIconProvider::icon(const QFileInfo &info) const
{
    if (photoSuffix.contains(info.suffix(), Qt::CaseInsensitive)) {
        auto image = getResourcePixmap(info.filePath());
        if (image.isNull())
            return DFileIconProvider::icon(info);
        else
            return QIcon(image);
    }

    return DFileIconProvider::icon(info);
}

QPixmap PMFileIconProvider::getResourcePixmap(const QString &filename) const
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
