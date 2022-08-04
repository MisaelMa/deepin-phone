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
#include "AppTreeViewItemDelegate.h"

#include <QMouseEvent>

#include <DApplication>
#include <DPalette>
#include <DStyle>
#include <QPainter>

#include "AppTreeView.h"
#include "GlobalDefine.h"
using namespace PhoneAssistantModel;

AppTreeViewItemDelegate::AppTreeViewItemDelegate(QAbstractItemView *parent)
    : BaseTreeViewItemDelegate(parent)
{
    m_treeType = E_Widget_App;
}

void AppTreeViewItemDelegate::setIsCanExport(const bool &rl)
{
    m_bIsCanExport = rl;
}

void AppTreeViewItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    BaseTreeViewItemDelegate::paint(painter, option, index);

    drawItemDeleteBtn(painter, option, index);
}

bool AppTreeViewItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
    //  只有左键单击才可以
    if (mouseEvent->type() == QEvent::MouseButtonPress && mouseEvent->button() == Qt::LeftButton) {
        if (index.column() == model->columnCount() - 1) {
            QRect decorationRect_d =
                QRect(option.rect.topLeft() + QPoint(10, (option.rect.height() - 30) / 2), QSize(30, 30)); // add by Airy

            QRect export_d =
                QRect(option.rect.topLeft() + QPoint(10 + 30, (option.rect.height() - 30) / 2), QSize(30, 30)); // add by Airy

            if (decorationRect_d.contains(mouseEvent->pos())) {
                emit sigDeleteData(index);
                return true; //  pms 37242, wzx
            } else if (export_d.contains(mouseEvent->pos()) && m_bIsCanExport) {
                emit sigExportData(index);
                return true;
            }
        }
    }

    return BaseTreeViewItemDelegate::editorEvent(event, model, option, index);
}

//  鼠标滑过、 并且是最后一列
void AppTreeViewItemDelegate::drawItemDeleteBtn(QPainter *painter, const QStyleOptionViewItem &option,
                                                const QModelIndex &index) const
{
    Q_UNUSED(index);
    //    QStyleOptionViewItem opt = option;
    //    initStyleOption(&opt, index);

    if (m_treeType == E_Widget_App) {
        if (option.viewItemPosition == (QStyleOptionViewItem::End)) {
            QStyleOptionButton button;
            button.iconSize = QSize(30, 30);
            if (option.state & DStyle::State_MouseOver) {
                if (option.state & DStyle::State_Selected) {
                    button.icon = QIcon::fromTheme(g_app_short_name + "app_select_delete");
                } else {
                    button.icon = QIcon::fromTheme(g_app_short_name + "app_delete");
                }
            }
            button.rect = QRect(option.rect.topLeft() + QPoint(10, (option.rect.height() - 31) / 2), QSize(30, 30));
            button.features = QStyleOptionButton::Flat;

            DApplication::style()->drawControl(QStyle::CE_PushButton, &button, painter);

            //  苹果手机 处理
            if (m_bIsCanExport) {
                QStyleOptionButton exportButton;
                exportButton.iconSize = QSize(30, 30);
                if (option.state & DStyle::State_MouseOver) {
                    if (option.state & DStyle::State_Selected) {
                        exportButton.icon = QIcon::fromTheme(g_app_short_name + "app_select_export");
                    } else {
                        exportButton.icon = QIcon::fromTheme(g_app_short_name + "app_export");
                    }
                }
                exportButton.rect = QRect(option.rect.topLeft() + QPoint(10 + 30, (option.rect.height() - 31) / 2), QSize(30, 30));
                exportButton.features = QStyleOptionButton::Flat;

                DApplication::style()->drawControl(QStyle::CE_PushButton, &exportButton, painter);
            }
        }
    }
}
