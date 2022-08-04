/*
* Copyright (C) 2019 ~ 2020 Deepin Technology Co., Ltd.
*
* Author:     yelei <yelei@uniontech.com>
* Maintainer: yelei <yelei@uniontech.com>
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
#ifndef PHONELISTPHOTOMODEL_H
#define PHONELISTPHOTOMODEL_H

#include "base/BaseItemModel.h"

class PhoneListPhotoModel : public BaseItemModel
{
    Q_OBJECT
public:
    explicit PhoneListPhotoModel(QObject *parent = nullptr);

protected:
    void deleteRowByPath(const QString &, const QString &path) override;
    void appendData(const QVariant &data) override;
    void updateDataPixmap(const QJsonObject &, const QPixmap &) override;
    void updateData(const QString &, const QVariant &) override;
    qint64 getTotalSize() override;
};

#endif // PHONELISTPHOTOMODEL_H
