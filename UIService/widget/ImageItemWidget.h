///*
// * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// *
// * Author:     wangzhixuan <wangzhixuan@uniontech.com>
// *
// * Maintainer: wangzhixuan <wangzhixuan@uniontech.com>
// *
// * This program is free software: you can redistribute it and/or modify
// * it under the terms of the GNU General Public License as published by
// * the Free Software Foundation, either version 3 of the License, or
// * any later version.
// *
// * This program is distributed in the hope that it will be useful,
// * but WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// * GNU General Public License for more details.
// *
// * You should have received a copy of the GNU General Public License
// * along with this program.  If not, see <http://www.gnu.org/licenses/>.
// */
//#ifndef IMAGEITEMWIDGET_H
//#define IMAGEITEMWIDGET_H

//#include <DWidget>
//#include <DBlurEffectWidget>
//#include <DFloatingButton>

//#include "defines.h"

///**
// *  @brief 图片、视频 绘制
// */

//DWIDGET_USE_NAMESPACE

//class ImageEngineTask;

//class ImageItemWidget : public DBlurEffectWidget
//{
//    Q_OBJECT
//public:
//    explicit ImageItemWidget(const QSize &widgetSize, const QSize &iconSize, DWidget *parent = nullptr);
//    virtual ~ImageItemWidget() override;

//    void setPhoneFileInfo(PhoneFileInfo &info);
//    bool isChecked();
//    void check(bool bSendSig = false);
//    void uncheck(bool bSendSig = false);
//    PhoneFileInfo getFileInfo();

//protected:
//    void paintEvent(QPaintEvent *event) override;
//    void mousePressEvent(QMouseEvent *event) override;
//    void mouseDoubleClickEvent(QMouseEvent *event) override;

//signals:
//    void sigEnterFolder(const QString &strPath);
//    void sigSelectChanged();
//    void sigReloadImage(const QString &path, const QPixmap &data);

//private:
//    void initUI();
//    void select(bool bSendSig = false); // 选择当前,发送改变状态信号
//    QString textFromTime(int nTime);
//    void reloadImage();

//    void drawDirImage(QPainter &p, const QString &strTheme);
//    void drawCheckBkg(QPainter &p);
//    void drawImage(QPainter &p);
//    void drawErrorImage(QPainter &p);
//    // 绘制勾选图表
//    void drawCheck(QPainter *p, const QRect &rect) const;

//    /**
//     * @brief drawVideoTime
//     *  画 视频的时间区域
//     */
//    void drawVideoTime(QPainter &);
//    void autoUpdate();

//private:
//    bool m_bChecked;
//    QPoint m_pos;
//    QSize m_pIconSize;

//    int m_nBorderSize;
//    DFloatingButton *m_checkButton;
//    PhoneFileInfo m_phoneFileInfo;

//    QTimer *m_pClickTimer = nullptr;
//    ImageEngineTask *m_task;
//};
//#endif // IMAGEITEMWIDGET_H
