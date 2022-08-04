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
#ifndef TREEHEADERVIEW_H
#define TREEHEADERVIEW_H

#include <QtGui>
#include <DHeaderView>
#include <QStyleOptionButton>
#include <QStyle>
#include <QDebug>

#include "defines.h"

DWIDGET_USE_NAMESPACE

class TreeHeaderView : public DHeaderView
{
    Q_OBJECT

public:
    TreeHeaderView(int checkColumnIndex, Qt::Orientation orientation, DWidget *parent = nullptr);
    //    void setCheckBoxEnabled(bool value);

    bool getIsChecked() const;
    void setTreeType(const E_Widget_Type &);

signals:
    void checkStausChanged(const bool &);
    void sigSortIndicatorShown(const bool &bSortShown);

protected:
    void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void rowsInserted(const QModelIndex &parent, int start, int end) override;
    void paintEvent(QPaintEvent *event) override;

public slots:
    void slotCancelAllChecked();
    void slotSetChecked(bool bChecked);

private:
    void drawCheckBox(QPainter *painter, const QRect &rect, int logicalIndex) const;
    void drawIndexTextSort(QPainter *painter, const QRect &rect, int logicalIndex) const;

private:
    bool isChecked;
    int m_checkColIdx;
    //    bool m_checkBoxEnabled;

    E_Widget_Type m_treeType = E_No_Define;
};

#endif // TREEHEADERVIEW_H
