/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co., Ltd.
 *
 * Author:     huangyonga<huangyonga@uniontech.com>
 * Maintainer: huangyonga<huangyonga@uniontech.com>
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
#include "BaseTreeViewItemDelegate.h"

#include <DApplication>
#include <DApplicationHelper>
#include <DPalette>
#include <DStyle>
#include <DStyleHelper>
#include <QDebug>
#include <QModelIndex>
#include <QPainter>
#include <QStyleOptionViewItem>
#include <QPainterPath>
#include <QMouseEvent>

#include "GlobalDefine.h"

DWIDGET_USE_NAMESPACE

using namespace PhoneAssistantModel;

BaseTreeViewItemDelegate::BaseTreeViewItemDelegate(QAbstractItemView *parent)
    : DStyledItemDelegate(parent)
{
    m_pItemView = static_cast<BaseTreeView *>(parent);
}

BaseTreeViewItemDelegate::~BaseTreeViewItemDelegate()
{
}

//  类型
void BaseTreeViewItemDelegate::setTreeType(const E_Widget_Type &type)
{
    m_treeType = type;
}

void BaseTreeViewItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid()) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    if (m_treeType != E_Widget_File) {
        if (index.column() == 0) {
            drawCheckBox(painter, option, index);
            return;
        }
    }

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setOpacity(1);

    //  背景色
    drawBkgColor(painter, option, index);

    //  文字
    drawItemText(painter, option, index);

    painter->restore();
}

QSize BaseTreeViewItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QSize size = QStyledItemDelegate::sizeHint(option, index);
    size.setHeight(37); //std::max(36, size.height()));
    return size;
}

void BaseTreeViewItemDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    option->showDecorationSelected = true;
    bool ok = false;
    if (index.data(Qt::TextAlignmentRole).isValid()) {
        uint value = index.data(Qt::TextAlignmentRole).toUInt(&ok);
        option->displayAlignment = static_cast<Qt::Alignment>(value);
    }
    if (!ok)
        option->displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;
    option->textElideMode = Qt::ElideRight;
    option->features = QStyleOptionViewItem::HasDisplay;
    if (index.row() % 2 == 0)
        option->features |= QStyleOptionViewItem::Alternate;
    if (index.data(Qt::DisplayRole).isValid())
        option->text = index.data().toString();
}

//  画 图标、文字
void BaseTreeViewItemDelegate::drawPixmapText(QPainter *painter, const QStyleOptionViewItem &option,
                                              const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    QRect textRect = opt.rect;
    textRect.setHeight(textRect.height() - 1);

    QFontMetrics fm(opt.font);

    DStyle *style = dynamic_cast<DStyle *>(DApplication::style());
    int margin = style->pixelMetric(DStyle::PM_ContentsMargins, &option);
    auto spacing = style->pixelMetric(DStyle::PM_ContentsSpacing, &option);
    auto iconSize = style->pixelMetric(DStyle::PM_ListViewIconSize, &option);

    QString text = "";

    auto icon = index.data(Qt::DecorationRole).value<QIcon>();
    if (icon.isNull() == false) {
        QRect iconRect = textRect;
        iconRect.setX(textRect.x() + margin);
        iconRect.setWidth(iconSize);

        auto diff = (iconRect.height() - iconSize) / 2;
        if (m_treeType == E_Widget_File) {
            icon.paint(painter, iconRect.adjusted(margin, diff, margin, -diff));

            textRect.setX(textRect.x() + iconSize + margin * 2 + spacing);
        } else {
            icon.paint(painter, iconRect.adjusted(0, diff, 0, -diff));

            textRect.setX(textRect.x() + iconSize + spacing + margin);
        }
    } else {
        textRect.setX(textRect.x() + margin);
    }

    textRect.setWidth(textRect.width() - margin);
    text = fm.elidedText(opt.text, Qt::ElideRight, textRect.width());
    //    }
    if (text != "") {
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text);
    }
}

//  画 checkBox
void BaseTreeViewItemDelegate::drawCheckBox(QPainter *painter, const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    bool data = m_pItemView->selectionModel()->isSelected(index);
    DStyleOptionButton checkBoxStyle;
    checkBoxStyle.initFrom(m_pItemView);

    checkBoxStyle.state = data ? DStyle::State_On : DStyle::State_Off;
    checkBoxStyle.rect =
        QRect(option.rect.topLeft() + QPoint((opt.rect.width() - 16) / 2, (opt.rect.height() - 17) / 2), QSize(16, 16));
    DApplication::style()->drawPrimitive(DStyle::PE_IndicatorCheckBox, &checkBoxStyle, painter);

    painter->restore();
}

//  画 文字
void BaseTreeViewItemDelegate::drawItemText(QPainter *painter, const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    if (m_treeType == E_Widget_File) {
        drawPixmapText(painter, option, index);
    } else if (opt.viewItemPosition != QStyleOptionViewItem::Beginning) {
        drawPixmapText(painter, option, index);
    }
}

//  画背景色
void BaseTreeViewItemDelegate::drawBkgColor(QPainter *painter, const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    DPalette::ColorGroup cg;
    if (!(opt.state & DStyle::State_Enabled)) {
        cg = DPalette::Disabled;
    } else {
        bool rl = m_pItemView->isActiveWindow();
        if (!rl) {
            cg = DPalette::Inactive;
        } else {
            cg = DPalette::Active;
        }
    }
    DApplicationHelper *dAppHelper = DApplicationHelper::instance();
    DPalette palette = dAppHelper->applicationPalette();
    QBrush background;
    if (opt.features & QStyleOptionViewItem::Alternate) {
        //        background = palette.color(cg, DPalette::Base);
    } else {
        background = palette.color(cg, DPalette::AlternateBase);
    }

    QPen forground;
    //  文管的第一列 不是checkbox，  第一列 采用 text， 其余 采用 TextTips   设计师确认
    if (m_treeType == E_Widget_File) {
        if (index.column() == 0) {
            forground.setColor(palette.color(cg, DPalette::Text));
        } else
            forground.setColor(palette.color(cg, DPalette::TextTips));
    } else {
        if (index.column() == 1) {
            forground.setColor(palette.color(cg, DPalette::Text));
        } else {
            forground.setColor(palette.color(cg, DPalette::TextTips));
        }
    }

    if (opt.state & DStyle::State_Enabled) {
        if (opt.state & DStyle::State_Selected) {
            background = palette.color(cg, DPalette::Highlight); // add by Airy
            forground.setColor(palette.color(cg, DPalette::HighlightedText));
        }
    }

    QPainterPath path = getBkPath(painter, option, index);

    painter->setPen(forground);
    painter->fillPath(path, background);

    painter->save();

    if (opt.state & DStyle::State_Enabled) {
        if (opt.state & DStyle::State_Selected) {
        } else {
            if (opt.state & DStyle::State_MouseOver) { //  滑过的颜色绘制
                painter->setBrush(QBrush(palette.color(DPalette::Light)));
                painter->fillPath(path, painter->brush());
            }
        }
    }

    painter->restore();
}

QPainterPath BaseTreeViewItemDelegate::getBkPath(QPainter *painter, const QStyleOptionViewItem &option,
                                                 const QModelIndex &index) const
{
    DStyle *style = dynamic_cast<DStyle *>(DApplication::style());

    int radius = style->pixelMetric(DStyle::PM_FrameRadius, &option);
    int margin = style->pixelMetric(DStyle::PM_ContentsMargins, &option);

    QRect rect = option.rect;
    rect.setHeight(rect.height() - 1);

    QPainterPath path;

    switch (option.viewItemPosition) {
    case QStyleOptionViewItem::Beginning: {
        if (m_treeType == E_Widget_File) {
            rect.setX(rect.x() + 10);
        } else {
            rect.setX(rect.x());
        }

        QPainterPath rectPath, roundedPath;
        roundedPath.addRoundedRect(rect.x(), rect.y(), rect.width() * 2, rect.height(), radius, radius);
        rectPath.addRect(rect.x() + rect.width(), rect.y(), rect.width(), rect.height());
        QPainterPath clipPath = roundedPath.subtracted(rectPath);
        painter->setClipPath(clipPath);
        path.addRect(rect);
    } break;
    case QStyleOptionViewItem::Middle: {
        QPainterPath rectPath, roundedPath;

        if (index.column() == 1 && m_treeType != E_Widget_File) {
            roundedPath.addRoundedRect(rect.x(), rect.y(), rect.width() * 2 + margin, rect.height(), radius, radius);
        } else {
            roundedPath.addRoundedRect(rect.x(), rect.y(), rect.width() * 2 + margin, rect.height(), 0, 0);
        }

        rectPath.addRect(rect.x() + rect.width(), rect.y(), rect.width(), rect.height());
        QPainterPath clipPath = roundedPath.subtracted(rectPath);
        painter->setClipPath(clipPath);
        path.addRect(rect);

    } break;
    case QStyleOptionViewItem::End: {
        rect.setWidth(rect.width() - 10); //  pms 37224 wzx
        QPainterPath rectPath, roundedPath;
        roundedPath.addRoundedRect(rect.x() - rect.width(), rect.y(), rect.width() * 2, rect.height(), radius, radius);
        rectPath.addRect(rect.x() - rect.width(), rect.y(), rect.width(), rect.height());
        QPainterPath clipPath = roundedPath.subtracted(rectPath);
        painter->setClipPath(clipPath);
        path.addRect(rect);
    } break;
    case QStyleOptionViewItem::OnlyOne: {
        rect.setX(rect.x() + margin); // left margin
        rect.setWidth(rect.width() - margin); // right margin
        path.addRoundedRect(rect, radius, radius);
    } break;
    default: {
        QStyledItemDelegate::paint(painter, option, index);
        break;
    }
    }

    return path;
}
