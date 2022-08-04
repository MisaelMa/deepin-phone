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
#ifndef MAINITEMWIDGET_H
#define MAINITEMWIDGET_H

#include "defines.h"

#include <DWidget>
#include <DPushButton>
#include <DArrowRectangle>
#include <DFloatingButton>
#include <DApplicationHelper>
#include <DLabel>
#include <DFrame>
#include <DProgressBar>
#include <DIconButton>
DWIDGET_USE_NAMESPACE

//class ArrowPhoneInfo;
class BatteryWgt;
class VIconBtn;

// base widget
class BaseThemeWgt : public DWidget
{
    Q_OBJECT
public:
    explicit BaseThemeWgt(DWidget *parent = nullptr);
    void refreshTheme();
    void appendPixmap(DLabel *lbl, QString strPixName);
protected slots:
    void onChangedTheme(DApplicationHelper::ColorType themeType);

private:
    QMap<DLabel *, QString> m_mapPix;
};

// 未解锁 ios
class UnlockWgt : public BaseThemeWgt
{
    Q_OBJECT
public:
    explicit UnlockWgt(DWidget *parent = nullptr);

private:
    void initUI();
};

// 调试模式
class DebugModeWidget : public BaseThemeWgt
{
    Q_OBJECT
public:
    explicit DebugModeWidget(DWidget *parent = nullptr);

private:
    void initUI();
};

// USB授权
class UsbAuthorizeWidget : public BaseThemeWgt
{
    Q_OBJECT
public:
    explicit UsbAuthorizeWidget(QWidget *parent = nullptr);

private:
    void initUI();
};

//APK安装失败
class ApkInstallErrorWidget : public BaseThemeWgt
{
    Q_OBJECT
public:
    explicit ApkInstallErrorWidget(QWidget *parent = nullptr);

private:
    void initUI();
};

// 断开连接
class DisconnWgt : public BaseThemeWgt
{
    Q_OBJECT
public:
    explicit DisconnWgt(DWidget *parent = nullptr);

signals:
    void reconnect();

private:
    void initUI();
};

// 电池
class BatteryWgt : public BaseThemeWgt
{
    Q_OBJECT
public:
    explicit BatteryWgt(DWidget *parent = nullptr);
    void setBatPct(int nPct);
    virtual void paintEvent(QPaintEvent *event);
private slots:
    void onChangedTheme(DApplicationHelper::ColorType themeType);

private:
    QPixmap m_pixmap;
    float m_nBatPct;
};

// 垂直图标按钮
class VIconBtn : public DPushButton
{
    Q_OBJECT
public:
    VIconBtn(const E_Widget_Type &type, DWidget *parent = nullptr);
signals:
    void IconBtnClicked(const E_Widget_Type &type);

protected:
    bool event(QEvent *e) override;

private slots:
    void onBtnClicked();
    void onChangedTheme(DGuiApplicationHelper::ColorType);

private:
    void paintEvent(QPaintEvent *event) override;

private:
    E_Widget_Type m_type;
    QColor m_backgroundColor;
    DLabel *m_pLabelPixmap = nullptr;
    DLabel *m_pLabelText = nullptr;
};

// 手机信息弹框
class ArrowPhoneInfo : public DArrowRectangle
{
    Q_OBJECT
public:
    explicit ArrowPhoneInfo(DWidget *parent = nullptr);

    void refreshUI(PhoneInfo &info);
    DLabel *converElideLbl(const QString &text);

private:
    QVBoxLayout *m_mainLayout;
};
#endif // MAINITEMWIDGET_H
