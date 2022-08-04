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
#include "DeletePhoneFileThread.h"

#include <QDirIterator>

#include "FileUtils.h"
#include "TrObject.h"

DeletePhoneFileThread::DeletePhoneFileThread(QObject *parent)
    : PMThread(parent)
{
}

void DeletePhoneFileThread::run()
{
    m_isCanRun = true;

    _delFile();
}

void DeletePhoneFileThread::delFile(const QStringList &listFileNames, const PhoneFileType &type)
{
    if (listFileNames.isEmpty())
        return;

    m_listFileNames = listFileNames;
    if (type == IMAGE) {
        m_listFilters << FileUtils::getImageFilters();
    } else if (type == VIDEO) {
        m_listFilters << FileUtils::getVideoFilters();
    }
}

void DeletePhoneFileThread::_delFile()
{
    int nCount = m_listFileNames.count();
    int nTotal = 0; //总数
    int nError = 0; //失败数
    foreach (QString strItemFile, m_listFileNames) {
        if (!m_isCanRun) {
            break;
        }
        QFileInfo fileInfo;
        fileInfo.setFile(strItemFile);
        // send progress
        emit sigDeleteProgress(fileInfo.fileName());

        if (fileInfo.isDir()) {
            QDirIterator dir_iterator(strItemFile, m_listFilters, QDir::Files | QDir::NoSymLinks,
                                      QDirIterator::Subdirectories);
            while (dir_iterator.hasNext()) {
                if (!m_isCanRun) {
                    break;
                }
                dir_iterator.next();
                QFileInfo file_info = dir_iterator.fileInfo();
                if (file_info.isFile()) {
                    ++nTotal;
                    bool bRet = FileUtils::_delFile_one(file_info.absoluteFilePath().toLocal8Bit().data());
                    if (!bRet) {
                        ++nError;
                        emit error(TrObject::getInstance()->getFileOperationText(Deletion_failed) + file_info.fileName()
                                   + ". \n" + TrObject::getInstance()->getFileOperationText(delete_failed));
                    }
                }
            }
        } else if (fileInfo.isFile()) {
            ++nTotal;
            bool bRet = FileUtils::_delFile_one(strItemFile.toLocal8Bit().data());
            if (!bRet) {
                ++nError;
                emit error(TrObject::getInstance()->getFileOperationText(Deletion_failed)
                           + strItemFile.section('/', -1) + ". \n"
                           + TrObject::getInstance()->getFileOperationText(delete_failed));
            }
        }
    }

    // send progress
    if (nCount != 0) {
        emit sigDeleteProgress("");
    }

    emit sigDeleteResult(nError, !m_isCanRun);
}
