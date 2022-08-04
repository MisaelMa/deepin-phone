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
#ifndef PHONELISTPHOTODELEGATE_H
#define PHONELISTPHOTODELEGATE_H

#include "defines.h"
#include "base/BaseListViewItemDelegate.h"

class PhoneListPhotoDelegate : public BaseListViewItemDelegate
{
    Q_OBJECT
public:
    explicit PhoneListPhotoDelegate(QAbstractItemView *parent = nullptr);

    // item大小
    void setItemSize(const QSize &size);
    // 图标大小
    void setIconSize(const QSize &size);

protected:
    void paint(QPainter *p,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const Q_DECL_OVERRIDE;

private:
    // 绘制文件夹
    QRect drawDirImage(QPainter *p, const QStyleOptionViewItem &option, const QString &strTheme) const;
    // 绘制文件夹名称
    void drawDirName(QPainter *p, const QStyleOptionViewItem &option, const PhoneFileInfo &info) const;
    // 绘制实际图片
    void drawImage(QPainter *p, const QRect &rParent, const PhoneFileInfo &info, const QString &strTheme, bool bBaseColor = false) const;
    // 绘制图片选中边框
    void drawSelectBkg(QPainter *p, const QStyleOptionViewItem &option, const QString &strTheme) const;
    // 绘制错误图片
    void drawErrorImage(QPainter *p, const QRect &rect, const PhoneFileInfo &info, const QString &strTheme) const;
    // 绘制图片边框
    void drawFrame(QPainter *p, const QRect &rect, const int &nRnd, const QString &strTheme) const;
    // 绘制视频时间
    void drawVideoTime(QPainter *p, const QRect &rect, const int &nTime) const;
    // 绘制勾选图表
    void drawCheck(QPainter *p, const QRect &rect) const;

    // 时间转字符串
    QString textFromTime(int nTime) const;
    // 获取颜色
    QColor getColor(DPalette::ColorType ct, DPalette::ColorGroup cg = DPalette::Active) const;

private:
    QSize m_sItemSize;
    QSize m_sIconSize;
};

#endif // PHONELISTPHOTODELEGATE_H
