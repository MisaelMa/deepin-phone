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

#include "PhoneListPhotoDelegate.h"
#include "base/BaseItemModel.h"

#include <QPainter>
#include <DApplication>
#include <DApplicationHelper>
#include <DStyle>
#include <QDebug>
#include <QPainterPath>

PhoneListPhotoDelegate::PhoneListPhotoDelegate(QAbstractItemView *parent)
    : BaseListViewItemDelegate(parent)
{
}

void PhoneListPhotoDelegate::setItemSize(const QSize &size)
{
    m_sItemSize = size;
}

void PhoneListPhotoDelegate::setIconSize(const QSize &size)
{
    m_sIconSize = size;
}

void PhoneListPhotoDelegate::paint(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    p->save();
    p->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, true);

    DGuiApplicationHelper::ColorType ct = DApplicationHelper::instance()->themeType();
    QString strTheme = ((ct == DGuiApplicationHelper::LightType) ? "light" : "dark");
    PhoneFileInfo info = index.data(ROLE_ITEM_DATA).value<PhoneFileInfo>();

    if (info.bIsDir) {
        QRect rect = drawDirImage(p, option, strTheme);
        drawImage(p, rect, info, strTheme, true);
        drawDirName(p, option, info);
    } else {
        if (option.state & QStyle::State_Selected) {
            // 画图片背景色
            drawSelectBkg(p, option, strTheme);
        }
        drawImage(p, option.rect, info, strTheme);
    }

    if (option.state & QStyle::State_Selected) {
        // 绘制勾选图标
        drawCheck(p, option.rect);
    }

    p->restore();
}

QSize PhoneListPhotoDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const
{
    return m_sItemSize;
}

QRect PhoneListPhotoDelegate::drawDirImage(QPainter *p, const QStyleOptionViewItem &option, const QString &strTheme) const
{
    bool bSelected = option.state & QStyle::State_Selected;
    QPixmap pixBG;
    if (bSelected) {
        pixBG = QPixmap(QString("://%1/album_bg_checked.svg").arg(strTheme));
    } else {
        pixBG = QPixmap(QString("://%1/album_bg.svg").arg(strTheme));
    }

    QRect rPixmap = option.rect;
    rPixmap.setSize(pixBG.size());
    p->drawPixmap(rPixmap, pixBG); //背景

    return rPixmap;
}

void PhoneListPhotoDelegate::drawDirName(QPainter *p, const QStyleOptionViewItem &option, const PhoneFileInfo &info) const
{
    p->save();

    QFontMetrics fontMetrics(p->font());
    QString strName = fontMetrics.elidedText(info.dirPath.section("/", -1), Qt::ElideRight, 80);
    QString strCount = QString("(%1)").arg(info.fileCount);

    QRect rFont = option.rect.adjusted(0, 0, -QFontMetrics(option.font).width(strCount), 0);
    // name
    QRect rAfter;
    p->drawText(rFont, int(Qt::AlignBottom | Qt::AlignHCenter), strName, &rAfter);

    // count
    rFont.setX(rAfter.right());
    rFont.setRight(option.rect.right());
    p->setPen(getColor(DPalette::TextTips));
    p->drawText(rFont, int(Qt::AlignBottom | Qt::AlignLeft), strCount);

    p->restore();
}

void PhoneListPhotoDelegate::drawImage(QPainter *p, const QRect &rParent, const PhoneFileInfo &info, const QString &strTheme, bool bBaseColor) const
{
    p->save();

    QPixmap pix = info.scaleIcon.pixmap(m_sIconSize).scaled(m_sIconSize); // setDevicePixelRatio(devicePixelRatioF());
    //    pix.setDevicePixelRatio(devicePixelRatioF());
    //    pix = pix.scaled(m_pIconSize.width() * devicePixelRatioF(),
    //                     m_pIconSize.height() * devicePixelRatioF()); // setDevicePixelRatio(devicePixelRatioF());

    QRect rDraw = pix.rect();
    rDraw.moveCenter(rParent.center());

    if (info.damaged) {
        // setClipPath 会导致线被裁一半
        drawErrorImage(p, rDraw, info, strTheme);
    } else {
        int nRadius = 8;
        QPainterPath path;
        path.addRoundedRect(rDraw, nRadius, nRadius);
        p->setClipPath(path);
        // 绘制底色
        if (bBaseColor) {
            p->fillPath(path, DApplicationHelper::instance()->applicationPalette().base());
        }
        // 绘制图片
        p->drawPixmap(rDraw, pix);

        // 绘制时间
        if (!info.bIsDir && info.type == VIDEO) {
            drawVideoTime(p, rDraw, info.time);
        }

        p->restore();

        // 绘制边框
        drawFrame(p, rDraw, nRadius, strTheme);
        return;
    }

    p->restore();
}

void PhoneListPhotoDelegate::drawSelectBkg(QPainter *p, const QStyleOptionViewItem &option, const QString &strTheme) const
{
    Q_UNUSED(strTheme)

    p->save();

    p->setPen(Qt::NoPen);
    p->setBrush(getColor(DPalette::ObviousBackground));
    p->drawRoundedRect(option.rect, 16, 16);

    p->restore();
}

void PhoneListPhotoDelegate::drawErrorImage(QPainter *p, const QRect &rect, const PhoneFileInfo &info, const QString &strTheme) const
{
    p->save();

    QBrush bgDamaged = ((strTheme == "light") ? Qt::white : QColor("#191919"));
    p->setBrush(bgDamaged);
    drawFrame(p, rect, 8, strTheme);

    QPixmap pixDamaged;
    if (info.type == IMAGE) {
        pixDamaged = QPixmap(QString("://%1/picture_damaged.svg").arg(strTheme));
    } else {
        pixDamaged = QPixmap(QString("://%1/video_damaged.svg").arg(strTheme));
    }
    QRect rDamaged = pixDamaged.rect();
    rDamaged.moveCenter(rect.center());
    p->drawPixmap(rDamaged, pixDamaged); //背景

    p->restore();
}

void PhoneListPhotoDelegate::drawFrame(QPainter *p, const QRect &rect, const int &nRnd, const QString &strTheme) const
{
    p->save();

    //增加边框 待封装
    if (strTheme == "light") {
        p->setPen(QPen(QColor(0, 0, 0, 25), 1));
    } else {
        p->setPen(QPen(QColor(255, 255, 255, 50), 1));
    }
    p->drawRoundedRect(rect, nRnd, nRnd);

    p->restore();
}

void PhoneListPhotoDelegate::drawVideoTime(QPainter *p, const QRect &rect, const int &nTime) const
{
    p->save();

    const int nHeight = 36; // 时间状态栏高度
    const int nYPos = rect.y() + rect.height() - nHeight; // 状态栏的y坐标

    QPainterPath rectPath;
    rectPath.addRect(rect.x(), nYPos, rect.width(), 36);
    p->fillPath(rectPath, QColor::fromRgb(0, 0, 0, 80));

    QPixmap pixmapCamera = QPixmap("://light/camera.svg");
    p->drawPixmap(rect.x() + 10, nYPos + 5, pixmapCamera);
    // time text
    p->setPen(Qt::white);
    QString strTime = textFromTime(nTime);
    p->drawText(rect.x() + 12 + pixmapCamera.width(), nYPos + 23, strTime);

    p->restore();
}

void PhoneListPhotoDelegate::drawCheck(QPainter *p, const QRect &rect) const
{
    QRect rc = rect;
    rc.setSize({20, 20});
    rc.moveTopRight(QPoint(rect.right() - 5, rect.top() + 5));

    int x = rc.x() + 1;
    int y = rc.y() + 1;
    int w = rc.width() - 2;
    int h = rc.height() - 2;

    QPainterPath rectPath;

    rectPath.addEllipse(x, y, w, h);

    p->fillPath(rectPath, QColor(255, 255, 255));

    DStyleOptionButton check;
    check.state = DStyle::State_On;
    check.rect = rc;

    DApplication::style()->drawPrimitive(DStyle::PE_IndicatorItemViewItemCheck, &check, p);
}

QString PhoneListPhotoDelegate::textFromTime(int nTime) const
{
    QString retTime;

    //天数计算
    int day = (nTime) / (24 * 3600);
    //小时计算
    int hour = (nTime) % (24 * 3600) / 3600;
    //分钟计算
    int minute = (nTime) % 3600 / 60;
    //秒计算
    int second = (nTime) % 60;

    if (day != 0) {
        retTime = QString("%1:").arg(day, 2, 10, QLatin1Char('0'));
    }
    if (hour != 0) {
        retTime = QString("%1%2:").arg(retTime).arg(hour, 2, 10, QLatin1Char('0'));
    }
    retTime = QString("%1%2:").arg(retTime).arg(minute, 2, 10, QLatin1Char('0'));
    retTime = QString("%1%2").arg(retTime).arg(second, 2, 10, QLatin1Char('0'));

    return retTime;
}

QColor PhoneListPhotoDelegate::getColor(DPalette::ColorType ct, QPalette::ColorGroup cg) const
{
    DApplicationHelper *dAppHelper = DApplicationHelper::instance();
    DPalette palette = dAppHelper->applicationPalette();
    //    cg = DPalette::Active;
    QColor cRet = palette.color(cg, ct);
    return cRet;
}
