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
#include "TreeHeaderView.h"
#include <DPalette>
#include <DApplicationHelper>
#include <DStyle>
#include <DApplication>
#include <QPainterPath>

static const int kSpacingMargin = 4;
// recommend size
// static const QSize kDropDownSize {10, 6};
static const QSize kDropDownSize {11, 10};

TreeHeaderView::TreeHeaderView(int checkColumnIndex, Qt::Orientation orientation, DWidget *parent)
    : DHeaderView(orientation, parent)
{
    setStretchLastSection(true);
    setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    setFixedHeight(36);

    this->isChecked = false;
    this->m_checkColIdx = checkColumnIndex;
    viewport()->setAutoFillBackground(false);
    //    m_checkBoxEnabled = true;
}

void TreeHeaderView::drawIndexTextSort(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    DPalette::ColorGroup cg;
    QStyleOption opt;
    opt.initFrom(this);

    QWidget *wnd = DApplication::activeWindow();
    if (!(opt.state & DStyle::State_Enabled) || (!wnd)) {
        cg = DPalette::Disabled;
    } else {
#ifdef ENABLE_INACTIVE_DISPLAY
        if (!wnd) {
            cg = DPalette::Inactive;
        } else {
            if (wnd->isModal()) {
                cg = DPalette::Inactive;
            } else {
                cg = DPalette::Active;
            }
        }
#else
        cg = DPalette::Active;
//        cg = DPalette::Current;
#endif
    }

    DApplicationHelper *dAppHelper = DApplicationHelper::instance();
    DPalette palette = dAppHelper->applicationPalette();
    //    DPalette palette = dAppHelper->palette(this);

    DStyle *style = dynamic_cast<DStyle *>(DApplication::style());

    QStyleOptionHeader option;
    initStyleOption(&option);
    int margin = style->pixelMetric(DStyle::PM_ContentsMargins, &option);

    // title
    QRect contentRect(rect.x(), rect.y(), rect.width(), rect.height() - 1);
    QRect hSpacingRect(rect.x(), contentRect.height(), rect.width(),
                       rect.height() - contentRect.height());

    QBrush contentBrush(palette.color(cg, DPalette::Base));
    // horizontal spacing
    QBrush hSpacingBrush(palette.color(cg, DPalette::FrameBorder));
    //    QBrush hSpacingBrush(
    //        DApplicationHelper::instance()->palette(this).color(DPalette::FrameBorder));
    // vertical spacing
    QBrush vSpacingBrush(palette.color(cg, DPalette::FrameBorder));
    QRectF vSpacingRect(rect.x(), rect.y() + kSpacingMargin, 1,
                        rect.height() - kSpacingMargin * 2);
    QBrush clearBrush(palette.color(cg, DPalette::Window));

    painter->fillRect(hSpacingRect, clearBrush);
    painter->fillRect(hSpacingRect, hSpacingBrush);

    if (visualIndex(logicalIndex) > 0) {
        painter->fillRect(vSpacingRect, clearBrush);
        painter->fillRect(vSpacingRect, vSpacingBrush);
    }

    QPen forground;
    forground.setColor(palette.color(cg, DPalette::Text));
    QRect textRect;
    if (sortIndicatorSection() == logicalIndex) {
        textRect = {contentRect.x() + margin, contentRect.y(),
                    contentRect.width() - margin * 3 - kDropDownSize.width(), contentRect.height()};
    } else {
        textRect = {contentRect.x() + margin, contentRect.y(), contentRect.width() - margin,
                    contentRect.height()};
    }
    QString title = model()->headerData(logicalIndex, orientation(), Qt::DisplayRole).toString();
    painter->setPen(forground);
    painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, title);

    if ((logicalIndex == 0 && m_checkColIdx == 0) || (logicalIndex == 5 && m_treeType == E_Widget_App)) {
    } else {
        if (isSortIndicatorShown() && logicalIndex == sortIndicatorSection()) {
            QRect sortIndicator(textRect.x() + textRect.width() + margin,
                                textRect.y() + qCeil((textRect.height() - kDropDownSize.height()) / 2.),
                                kDropDownSize.width(), kDropDownSize.height());
            option.rect = sortIndicator;
            if (sortIndicatorOrder() == Qt::DescendingOrder) {
                style->drawPrimitive(DStyle::PE_IndicatorArrowDown, &option, painter);
            } else if (sortIndicatorOrder() == Qt::AscendingOrder) {
                style->drawPrimitive(DStyle::PE_IndicatorArrowUp, &option, painter);
            }
        }
    }
}

void TreeHeaderView::paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setOpacity(1);

    drawIndexTextSort(painter, rect, logicalIndex);

    drawCheckBox(painter, rect, logicalIndex);

    painter->restore();
}

void TreeHeaderView::drawCheckBox(QPainter *painter, const QRect &rect, int logicalIndex) const
{
    if (m_treeType == E_Widget_File)
        return;

    if (logicalIndex == m_checkColIdx) {
        painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

        QStyleOptionButton option;
        option.initFrom(this);

        option.rect = QRect(QPoint((rect.width() - 16) / 2, (rect.height() - 16) / 2), QSize(16, 16));
        option.state = isChecked ? QStyle::State_On : QStyle::State_Off;

        DApplication::style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &option, painter);
    }
}

void TreeHeaderView::mousePressEvent(QMouseEvent *event)
{
    if (m_checkColIdx != -1) {
        if (visualIndexAt(event->pos().x()) == m_checkColIdx) { // && m_checkBoxEnabled) {
            if (isChecked)
                isChecked = false;
            else
                isChecked = true;
            this->updateSection(m_checkColIdx);
            emit checkStausChanged(isChecked);
        }
    }

    DHeaderView::mousePressEvent(event);
}

void TreeHeaderView::mouseReleaseEvent(QMouseEvent *event)
{
    if (visualIndexAt(event->pos().x()) == m_checkColIdx) {
        return;
    }

    if (((visualIndexAt(event->pos().x()) == 5) || (visualIndexAt(event->pos().x()) == 3)) && m_treeType == E_Widget_App) {
        setSortIndicatorShown(false);
        emit sigSortIndicatorShown(false);
    } else {
        setSortIndicatorShown(true);
        emit sigSortIndicatorShown(true);
    }

    DHeaderView::mouseReleaseEvent(event);
}

void TreeHeaderView::rowsInserted(const QModelIndex &parent, int start, int end)
{
    //    setSortIndicatorShown(false);
    DHeaderView::rowsInserted(parent, start, end);
}

void TreeHeaderView::slotCancelAllChecked()
{
    this->isChecked = false;
    this->updateSection(m_checkColIdx);
}

void TreeHeaderView::slotSetChecked(bool bChecked)
{
    this->isChecked = bChecked;
    this->updateSection(m_checkColIdx);
}

bool TreeHeaderView::getIsChecked() const
{
    return isChecked;
}

void TreeHeaderView::setTreeType(const E_Widget_Type &type)
{
    m_treeType = type;

    if (m_treeType == E_Widget_File) {
        m_checkColIdx = -1;
    }
}

//void TreeHeaderView::setCheckBoxEnabled(bool value)
//{
//    this->setEnabled(value);
//    m_checkBoxEnabled = value;
//}

void TreeHeaderView::paintEvent(QPaintEvent *event)
{ // painter object for headerview's viewport widget
    QPainter painter(viewport());
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(1);

    DPalette::ColorGroup cg;
#ifdef ENABLE_INACTIVE_DISPLAY
    QWidget *wnd = DApplication::activeWindow();
    if (!wnd) {
        cg = DPalette::Inactive;
    } else {
        cg = DPalette::Active;
    }
#else
    cg = DPalette::Active;
//    cg = DPalette::Current;
#endif

    // global palette
    auto palette = DApplicationHelper::instance()->applicationPalette();
    // global style
    auto *style = dynamic_cast<DStyle *>(DApplication::style());

    QStyleOption opt;
    opt.initFrom(this);
    // base background brush
    QBrush bgBrush(palette.color(cg, DPalette::Base));
    // highlight brush, if widget is disabled, we set highlight brush to transparent
    QBrush hlBrush = Qt::transparent;
    // highlighted area's bounding rect
    QRect hlRect;

    // background in enabled state
    //    if ((opt.state & DStyle::State_Enabled) && m_hover != -1) {
    //        auto baseColor = bgBrush.color();
    //        if (m_pressed >= 0) {
    //            auto actColor = palette.color(cg, DPalette::Highlight);
    //            actColor.setAlphaF(0.1);
    //            auto newColor = style->adjustColor(baseColor, 0, 0, -20, 0, 0, 20, 0);
    //            // change highlight brush to pressed color
    //            hlBrush = style->blendColor(newColor, actColor);
    //        } else {
    //            // change hightlight brush to hovered color
    //            hlBrush = style->adjustColor(baseColor, 0, 0, -10);
    //        }
    //        // get hovered section's x axis
    //        auto spos = sectionPosition(m_hover);
    //        // get hovered section's width
    //        auto sw = sectionSize(m_hover);
    //        // calculate highlighted area's rect
    //        hlRect = {
    //            // while viewport scrolls horizontally, we need substract the scroll offset from hovered section's axis
    //            m_hover > 0 ? spos + 1 - offset() : spos - offset(),
    //            0,
    //            m_hover > 0 ? sw - 1 : sw,
    //            height()};
    //    }

    QStyleOptionHeader option;
    initStyleOption(&option);
    // header view corner radius
    auto radius = style->pixelMetric(DStyle::PM_FrameRadius, &option);

    // draw background, create a rounded rect 2 times viewport's height, then clip
    // out half part below
    QRect rect = viewport()->rect();
    QRectF clipRect(rect.x(), rect.y(), rect.width(), rect.height() * 2);
    QRectF subRect(rect.x(), rect.y() + rect.height(), rect.width(), rect.height());
    QPainterPath clipPath, subPath;
    clipPath.addRoundedRect(clipRect, radius, radius);
    subPath.addRect(subRect);
    clipPath = clipPath.subtracted(subPath);

    // draw clipped path
    painter.fillPath(clipPath, bgBrush);
    QPainterPath hlPath;
    // draw highlighted section if any
    hlPath.addRect(hlRect);
    hlPath = hlPath.intersected(clipPath);
    painter.fillPath(hlPath, hlBrush);

    // propagate paint event to base class
    DHeaderView::paintEvent(event);
    // restore painter state
    painter.restore();

    // draw focus
    if (hasFocus()) {
        QStyleOptionFocusRect o;
        o.QStyleOption::operator=(option);
        // need take scroll offset into consideration
        QRect focusRect {rect.x() - offset(), rect.y(), length() - sectionPosition(0), rect.height()};
        o.rect = style->visualRect(layoutDirection(), rect, focusRect);
        style->drawPrimitive(DStyle::PE_FrameFocusRect, &o, &painter);
    }
}
