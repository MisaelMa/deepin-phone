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
#include "MusicInfoTask.h"

#include <taglib/fileref.h>

extern "C" {
#include "libavformat/avformat.h"
}

MusicInfoTask::MusicInfoTask(QObject *parent)
    : FileDisplayTask(parent)
{
}

void MusicInfoTask::setData(const QStringList &inData)
{
    m_strFilePathList = inData;
}

void MusicInfoTask::run()
{
    if (m_strFilePathList.size() > 0)
        getMusicLength();

    //线程结束
    emit sigTaskFinished();
}

void MusicInfoTask::getMusicLength()
{
    QString length = "";
    foreach (auto s, m_strFilePathList) {
        if (!m_bsTaskCanRun)
            return;

        //判断是否要暂停
        checkRunabled();

        //    //获取音乐 演唱者和时长
        TagLib::FileRef f(s.toLocal8Bit().data());

        TagLib::AudioProperties *t_audioProperties = f.audioProperties();
        if (t_audioProperties != nullptr) {
            length = lengthString(t_audioProperties->length() * 1000);
        } else { //  读取不到音频信息    aac
            AVFormatContext *pFormatCtx = avformat_alloc_context();
            avformat_open_input(&pFormatCtx, s.toLocal8Bit().data(), nullptr, nullptr);
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

        emit sigMusicInfoLoaded(s, length);
    }
}

QString MusicInfoTask::lengthString(const qint64 &length)
{
    qint64 temp = length;
    temp = temp / 1000;
    int hour = static_cast<int>(temp / 3600);

    QString mmStr = QString("%1").arg(temp % 3600 / 60, 2, 10, QLatin1Char('0'));
    QString ssStr = QString("%1").arg(temp % 60, 2, 10, QLatin1Char('0'));

    if (hour > 0) {
        return QString("%1:%2:%3").arg(hour).arg(mmStr).arg(ssStr);
    } else {
        return QString("%1:%2").arg(mmStr).arg(ssStr);
    }
}
