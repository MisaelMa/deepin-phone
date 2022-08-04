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
#include "PhoneViewItemDelegate.h"

#include <DApplication>
#include <DApplicationHelper>
#include <DStyle>
#include <QImageReader>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>
#include <QPainterPath>

#include "utils.h"
#include "defines.h"
#include "phonelistview.h"

#define ICON_DATA (Qt::UserRole + 99)
#define ITEM_PHONE_ROLE (Qt::UserRole + 67)

PhoneViewItemDelegate::PhoneViewItemDelegate(QAbstractItemView *parent)
    : DStyledItemDelegate(parent)
{
    m_view = static_cast<PhoneListView *>(parent);
}

PhoneViewItemDelegate::~PhoneViewItemDelegate()
{
    //    qDeleteAll(m_btns.begin(), m_btns.end());
}

void PhoneViewItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    //    QStyledItemDelegate::paint(painter, option, index);
    paint_newStyle(painter, option, index);

    //    PhoneInfo info = index.data(ITEM_PHONE_ROLE).value<PhoneInfo>();
    //    if (info.status == STATUS_CONNECTED) {
    //一期暂时不提供断开连接功能 //modify zhangll
    //        QStyleOptionButton *button = m_btns.value(index);
    //        if (!button) {
    //            button = new QStyleOptionButton();
    //            button->iconSize = QSize(20, 20);
    //            button->state |= QStyle::State_Enabled; //可以换色

    //            (const_cast<PhoneViewItemDelegate *>(this))->m_btns.insert(index, button);
    //        }
    //选中
    //        DGuiApplicationHelper::ColorType ct = DApplicationHelper::instance()->themeType();
    //        QString img_path = "://images/";
    //        img_path += ((ct == DGuiApplicationHelper::LightType) ? "light/" : "dark/");

    //        if (option.state & QStyle::State_Selected) {
    //            img_path += "quit_white.svg";
    //        } else {
    //            img_path += "quit_black.svg";
    //        }
    //        button->icon = QIcon(img_path);
    //        button->rect = option.rect.adjusted(TREE_WIDTH - 10 - button->iconSize.width() - 10 /*190*/, 4, -4, -4);
    //        QApplication::style()->drawControl(QStyle::CE_PushButtonLabel, button, painter);
    //    }
}

void PhoneViewItemDelegate::paint_newStyle(QPainter *painter, const QStyleOptionViewItem &option,
                                           const QModelIndex &index) const
{
    painter->setRenderHint(QPainter::Antialiasing);

    DApplicationHelper *dAppHelper = DApplicationHelper::instance();
    DPalette palette = dAppHelper->applicationPalette();

    DStyle *style = dynamic_cast<DStyle *>(DApplication::style());
    int radius = style->pixelMetric(DStyle::PM_FrameRadius, &option);
    int margin = style->pixelMetric(DStyle::PM_ContentsMargins, &option);
    margin = 10;

    //    DWidget *wnd = DApplication::activeWindow();
    DPalette::ColorGroup cg;
    if (!(option.state & DStyle::State_Enabled)) {
        cg = DPalette::Disabled;
    } else {
        //        if (!wnd) {
        //            //            cg = DPalette::Inactive;
        //            cg = DPalette::Active; //一直高亮
        //        } else {
        cg = DPalette::Active;
        //        }
    }

    // bg
    QRect rBg = option.rect;
    rBg.setX(0);
    painter->fillRect(rBg, palette.base());
    QColor colorBg;
    QColor colorFont;
    QColor colorCount;
    if (option.state & QStyle::State_Selected) {
        colorBg = palette.color(cg, DPalette::Highlight);
        colorFont = Qt::white;
        colorCount = Qt::white;
    } else if (option.state & QStyle::State_MouseOver) {
        //colorBg = palette.color(cg, DPalette::Base);
        colorBg = palette.color(cg, DPalette::Light);
        colorFont = palette.color(cg, DPalette::Text);
        colorCount = palette.color(cg, DPalette::TextTips);
    } else {
        colorBg = palette.color(cg, DPalette::Base);
        colorFont = palette.color(cg, DPalette::Text);
        colorCount = palette.color(cg, DPalette::TextTips);
    }
    QPainterPath path;
    rBg.setX(0 + margin); // left margin
    rBg.setWidth(rBg.width() - margin); // right margin
    path.addRoundedRect(rBg, radius, radius);
    //    if (option.state & QStyle::State_MouseOver) {
    //        painter->setBrush(QBrush(palette.color(DPalette::Light)));
    //        painter->fillPath(path, painter->brush());
    //    } else {
    painter->fillPath(path, colorBg);
    //    }

    //    int nCurPosX = option.rect.x() + margin;
    int nCurPosX = margin - 3;
    //    decorated
    QStandardItemModel *m_model = qobject_cast<QStandardItemModel *>(m_view->model());
    if (m_model->itemFromIndex(index)->hasChildren()) {
        DGuiApplicationHelper::ColorType ct = DApplicationHelper::instance()->themeType();
        QString img_path = "://";
        img_path += ((ct == DGuiApplicationHelper::LightType) ? "light/" : "dark/");

        bool bExpanded = false;
        if (m_view->isExpanded(index)) {
            bExpanded = true;
        }
        if (option.state & QStyle::State_Selected) {
            img_path.replace("dark", "light");
            if (bExpanded)
                img_path += "arrow_down_checked.svg";
            else
                img_path += "arrow_right_checked.svg";
        } else if (option.state & QStyle::State_MouseOver) {
            if (bExpanded)
                img_path += "arrow_down.svg";
            else
                img_path += "arrow_right.svg";
        } else {
            if (bExpanded)
                img_path += "arrow_down.svg";
            else
                img_path += "arrow_right.svg";
        }
        QPixmap img = Utils::getResourcePixmap(img_path); // width:9

        QRect targetRect = option.rect;
        targetRect.setSize(QSize(9, 9));
        QPoint c = option.rect.center();
        c.setX(nCurPosX + img.width() / 2 + 8 /*spacing*/);
        targetRect.moveCenter(c);
        painter->drawPixmap(targetRect, img, img.rect());
    }
    //    PhoneInfo info = index.data(ITEM_PHONE_ROLE).value<PhoneInfo>();
    //    if (!info.strPhoneID.isEmpty()) {
    //        nCurPosX += (9 + 8);
    //    } else {
    //        nCurPosX = 36 - 5;
    //    }

    // icon
    drawTreeIcon(painter, option, index);

    //    // text
    //    drawTreeTitle(painter, option, index);

    //    painter->setPen(colorFont);
    //    QFontMetrics fontMetrics(painter->font());
    //    QRect rFont = option.rect;
    //    rFont.setX(60);
    //    QString msg =
    //        fontMetrics.elidedText(index.data().toString(), Qt::ElideRight, option.rect.width() - (nCurPosX + 6 + 30));
    //    QApplication::style()->drawItemText(painter, rFont, int(option.displayAlignment), palette, true, msg);

    // count
    //    painter->save();
    //    painter->setPen(colorCount);
    //    painter->setFont(DFontSizeManager::instance()->t8());
    //    int nCount = index.data(ITEM_COUNT_ROLE).toInt();
    //    if (nCount >= 0) {
    //        QRect rCount = option.rect.adjusted(0, 0, -20, 0);
    //        painter->drawText(rCount, int(Qt::AlignVCenter | Qt::AlignRight), QString::number(nCount));
    //    }
    //    painter->restore();
}

bool PhoneViewItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option,
                                        const QModelIndex &index)
{
    //    if (event->type() == QEvent::MouseButtonPress) {
    //        QMouseEvent *e = static_cast<QMouseEvent *>(event);
    //        if (option.rect.adjusted(190, 4, -4, -4).contains(e->x(), e->y()) && m_btns.contains(index)) {
    //            m_btns.value(index)->state |= QStyle::State_Sunken;
    //            m_lastIdx = index;
    //            return true;
    //        }
    //    }
    //    if (event->type() == QEvent::MouseButtonRelease) {
    //        QMouseEvent *e = static_cast<QMouseEvent *>(event);
    //        if (m_lastIdx.isValid() && m_lastIdx != index) {
    //            m_btns.value(m_lastIdx)->state &= (~QStyle::State_Sunken);
    //            return true;
    //        }
    //        if (option.rect.adjusted(190, 4, -4, -4).contains(e->x(), e->y()) && m_btns.contains(index)) {
    //            m_btns.value(index)->state &= (~QStyle::State_Sunken);
    //            emit itemDisconn(index);
    //            return true;
    //        } else {
    //            if (m_btns.contains(index)) {
    //                m_btns.value(index)->state &= (~QStyle::State_Sunken);
    //                return true;
    //            }
    //        }
    //    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

void PhoneViewItemDelegate::drawTreeIcon(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    if (opt.state & DStyle::State_Enabled) {
        QString sIconPath = index.data(ICON_DATA).toString();
        if (QFile::exists(sIconPath)) {
            if (opt.state & DStyle::State_Selected) {
                if (sIconPath.endsWith("_checked.svg") == false) {
                    sIconPath = sIconPath.replace(".svg", "_checked.svg");
                }
            } else {
                if (sIconPath.endsWith("_checked.svg") == true) {
                    sIconPath = sIconPath.replace("_checked.svg", ".svg");
                }
            }

            int nCurPosX = 7;
            PhoneInfo info = index.data(ITEM_PHONE_ROLE).value<PhoneInfo>();
            if (!info.strPhoneID.isEmpty()) {
                nCurPosX += (9 + 8);
            } else {
                nCurPosX = 36 - 5;
            }

            QPixmap pixmap = Utils::getResourcePixmap(sIconPath);
            QRect targetRect = option.rect;
            targetRect.setSize(QSize(16, 16));
            QPoint c = option.rect.center();
            c.setX(nCurPosX + pixmap.width() / 2 + 4 /*spacing*/);

            nCurPosX += pixmap.width() + 4;

            targetRect.moveCenter(c);
            p->drawPixmap(targetRect, pixmap); //, pixmap.rect());

            drawTreeTitle(p, option, index, nCurPosX);
        }
    }
}

void PhoneViewItemDelegate::drawTreeTitle(QPainter *p, const QStyleOptionViewItem &option, const QModelIndex &index, const int &nCurPosX) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    DApplicationHelper *dAppHelper = DApplicationHelper::instance();
    DPalette palette = dAppHelper->applicationPalette();

    DWidget *wnd = DApplication::activeWindow();
    DPalette::ColorGroup cg;
    if (!(option.state & DStyle::State_Enabled)) {
        cg = DPalette::Disabled;
    } else {
        if (!wnd) {
            cg = DPalette::Active; //一直高亮
        } else {
            cg = DPalette::Active;
        }
    }

    QColor colorFont, colorCount;
    if (option.state & QStyle::State_Selected) {
        colorFont = palette.color(cg, DPalette::HighlightedText);
        colorCount = palette.color(cg, DPalette::HighlightedText);
    } else {
        colorFont = palette.color(cg, DPalette::Text);
        colorCount = palette.color(cg, DPalette::TextTips);
    }

    p->setPen(colorFont);
    QFontMetrics fontMetrics(p->font());
    QRect rFont = option.rect;

    rFont.setX(nCurPosX + 6);

    QString msg = fontMetrics.elidedText(index.data().toString(), Qt::ElideRight, option.rect.width() - 100);
    DApplication::style()->drawItemText(p, rFont, Qt::AlignVCenter | Qt::AlignLeft, palette, true, msg);
}
