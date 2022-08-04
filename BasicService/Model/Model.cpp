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
#include "Model.h"

#include "DataService.h"

Model *Model::g_instance = nullptr;

Model::Model(QObject *p)
    : QObject(p)
{
    m_pDataService = new DataService(this);
}

void Model::InitInstance(QObject *p)
{
    g_instance = new Model(p);
}

Model *Model::getInstance()
{
    return g_instance;
}

QIcon Model::getIconBySuffix(const QString &k, const QFileInfo &info)
{
    return m_pDataService->getIconBySuffix(k, info);
}

//void Model::intsertPathAndSize(const QString &path, const qint64 &size)
//{
//    m_pDataService->intsertPathAndSize(path, size);
//}

//qint64 Model::getFileSizeByPath(const QString &path)
//{
//    return m_pDataService->getFileSizeByPath(path);
//}
