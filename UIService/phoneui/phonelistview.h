/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co., Ltd.
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
#ifndef PHONETREEVIEW_H
#define PHONETREEVIEW_H

#include "defines.h"

#include <DApplicationHelper>
#include <DTreeView>
#include <QStandardItemModel>

DWIDGET_USE_NAMESPACE

class PhoneListView : public DTreeView
{
    Q_OBJECT
public:
    explicit PhoneListView(DWidget *parent = nullptr);
    void appendPhoneInfo(const PhoneInfo &info);
    void removePhoneInfo(const QString &devId);
    void resetPhoneInfo(const PhoneInfo &info);
    bool getCurPhoneInfo(PhoneInfo &info);
    bool changeItem(const E_Widget_Type &type);
    //    void setCount(QString strPhoneID, E_Widget_Type type, int nCount);
    bool getPhoneInfo(QString phoneId, PhoneInfo &info);
    void updatePhoneBattery(QString phoneId, int value);

private:
    void initUI();
    void refreshTheme();
    void setDefaultSelect();
    void setCustomFont(QStandardItem *item);
    QList<QStandardItem *> getChildrenItem(const PhoneInfo &info);
    bool isExist(const PhoneInfo &info); //判断是否存在

public slots:
    void slotDeviceReconnect();
    void slotShowChanged(const E_Widget_Type &);

protected slots:
    void onChangedTheme(Dtk::Widget::DApplicationHelper::ColorType themeType);
    void onItemExpanded(QModelIndex idx);
    void onItemClicked(QModelIndex idx);

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void mainItemChanged(const PhoneInfo &info);
    void itemChanged(PhoneInfo &info, const E_Widget_Type &);
    //    void SignalUpdata();

private:
    QStandardItemModel *m_pModel = nullptr;
    QString icon = "";
    bool bItemChange;
};

#endif // PHONETREEVIEW_H
