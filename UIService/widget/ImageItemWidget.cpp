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
//#include "ImageItemWidget.h"

//#include <DApplicationHelper>

//#include <QPainter>
//#include <QTimer>
//#include <QMouseEvent>
//#include <QDebug>
//#include <DApplication>
//#include <DStyleOptionButton>

//#include "defines.h"
//#include "utils.h"
//#include "ImageEngineTask.h"
//#include "VideoEngineTask.h"
//#include "ThreadService.h"
//#include "UIService.h"

//ImageItemWidget::ImageItemWidget(const QSize &widgetSize, const QSize &iconSize, DWidget *parent)
//    : DBlurEffectWidget(parent)
//{
//    this->setFixedSize(widgetSize);
//    m_task = nullptr;
//    m_pIconSize = iconSize;

//    m_pos.setX((this->width() - iconSize.width()) / 2);
//    m_pos.setY((this->height() - iconSize.height()) / 2);
//    m_nBorderSize = (this->width() - iconSize.width()) / 2;

//    m_bChecked = false;

//    initUI();
//}

//ImageItemWidget::~ImageItemWidget()
//{
//    if (m_task)
//        m_task->setTaskStop();
//}

//void ImageItemWidget::setPhoneFileInfo(PhoneFileInfo &info)
//{
//    m_phoneFileInfo = info;
//    //    QTimer::singleShot(1, [this]() { reloadImage(); });
//}

//bool ImageItemWidget::isChecked()
//{
//    return m_bChecked;
//}

//void ImageItemWidget::check(bool bSendSig)
//{
//    if (!m_bChecked) {
//        select(bSendSig);
//    }
//}

//void ImageItemWidget::uncheck(bool bSendSig)
//{
//    if (m_bChecked) {
//        select(bSendSig);
//    }
//}

//PhoneFileInfo ImageItemWidget::getFileInfo()
//{
//    return m_phoneFileInfo;
//}

//void ImageItemWidget::initUI()
//{
//}

//void ImageItemWidget::select(bool bSendSig)
//{
//    m_bChecked = !m_bChecked;
//    autoUpdate();
//    if (bSendSig) {
//        //通知
//        emit sigSelectChanged();
//    }
//}

//void ImageItemWidget::paintEvent(QPaintEvent *event)
//{
//    Q_UNUSED(event);

//    DGuiApplicationHelper::ColorType ct = DApplicationHelper::instance()->themeType();
//    QString strTheme = ((ct == DGuiApplicationHelper::LightType) ? "light" : "dark");

//    QPainter p(this);
//    p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, true);

//    p.save();
//    if (m_phoneFileInfo.bIsDir) {
//        drawDirImage(p, strTheme);
//        if (m_phoneFileInfo.damaged) { // load error
//            drawErrorImage(p);
//        }
//    } else {
//        //  画图片背景色
//        drawCheckBkg(p);
//        if (m_phoneFileInfo.damaged) { // load error
//            drawErrorImage(p);
//        } else {
//            // 画实际图片
//            drawImage(p);
//        }
//    }

//    p.restore();
//    if (m_bChecked) {
//        // 绘制勾选图标
//        drawCheck(&p, this->rect());
//    }

//    //    DWidget::paintEvent(event);
//}

////  点击事件， 修改为 200毫秒定时器
//void ImageItemWidget::mousePressEvent(QMouseEvent *event)
//{
//    if (Qt::LeftButton == event->button()) {
//        if (m_pClickTimer == nullptr) {
//            m_pClickTimer = new QTimer(this);
//            m_pClickTimer->setSingleShot(true);
//            m_pClickTimer->start(200);

//            connect(m_pClickTimer, &QTimer::timeout, this, [=]() {
//                select(true);

//                delete m_pClickTimer;
//                m_pClickTimer = nullptr;
//            });
//        }
//        //        select(true);
//    }
//    DWidget::mousePressEvent(event);
//}

//void ImageItemWidget::mouseDoubleClickEvent(QMouseEvent *event)
//{
//    if (m_phoneFileInfo.bIsDir) {
//        if (m_pClickTimer != nullptr) {
//            delete m_pClickTimer;
//            m_pClickTimer = nullptr;
//        }
//        emit sigEnterFolder(m_phoneFileInfo.dirPath);
//    } else {
//        if (m_phoneFileInfo.type == VIDEO) { // 视频可以打开， 暂不做图片打开
//            Utils::execOpenPath(m_phoneFileInfo.path);
//        }
//    }
//    DWidget::mouseDoubleClickEvent(event);
//}

//void ImageItemWidget::drawVideoTime(QPainter &p)
//{
//    QRect rect = this->rect();

//    QPainterPath rectPath;
//    rectPath.addRect(rect.x(), rect.height() - 44, rect.width(), 36);

//    p.fillPath(rectPath, QColor::fromRgb(0, 0, 0, 80));

//    QPixmap pixmapCamera = QPixmap("://light/camera.svg");
//    p.drawPixmap(m_nBorderSize + 10, this->height() - m_nBorderSize - 36 + 5, pixmapCamera);
//    p.setPen(Qt::white);
//    QString strTime = textFromTime(m_phoneFileInfo.time);
//    p.drawText(m_nBorderSize + 10 + pixmapCamera.width(), this->height() - 36 - m_nBorderSize + 23, strTime);
//}

//void ImageItemWidget::autoUpdate()
//{
//    if (qAbs(mapFromGlobal(this->pos()).y()) > UIService::getService()->getQspMainWnd()->height()) {
//        return;
//    }
//    update();
//}

////  dir 图片
//void ImageItemWidget::drawDirImage(QPainter &p, const QString &strTheme)
//{
//    QPixmap pixBG;
//    if (m_bChecked) {
//        pixBG = QPixmap(QString("://%1/album_bg_checked.svg").arg(strTheme));
//    } else {
//        pixBG = QPixmap(QString("://%1/album_bg.svg").arg(strTheme));
//    }
//    p.drawPixmap(QPoint(0, 0), pixBG); //背景

//    //    图片
//    p.save();

//    QPixmap pix = m_phoneFileInfo.scaleIcon.pixmap(m_pIconSize); // setDevicePixelRatio(devicePixelRatioF());
//    pix.setDevicePixelRatio(devicePixelRatioF());
//    pix = pix.scaled(m_pIconSize.width() * devicePixelRatioF(),
//                     m_pIconSize.height() * devicePixelRatioF()); // setDevicePixelRatio(devicePixelRatioF());

//    QPainterPath path;
//    path.addRoundedRect(m_pos.x(), m_pos.y(), m_pIconSize.width(), m_pIconSize.height(), 10, 10);
//    p.setClipPath(path);
//    p.drawPixmap(m_pos, pix);
//    p.restore();
//    //增加边框
//    if (DApplicationHelper::instance()->themeType() == Dtk::Gui::DGuiApplicationHelper::ColorType::LightType) {
//        p.setPen(QColor(0, 0, 0, 26));
//    } else {
//        p.setPen(QColor(255, 255, 255, 50));
//    }
//    p.drawRoundedRect(m_pos.x(), m_pos.y(), m_pIconSize.width(), m_pIconSize.height(), 10, 10);
//}

////  check 的背景
//void ImageItemWidget::drawCheckBkg(QPainter &p)
//{
//    if (m_bChecked) {
//        DApplicationHelper *dAppHelper = DApplicationHelper::instance();
//        DPalette palette = dAppHelper->applicationPalette();
//        DPalette::ColorGroup cg;
//        cg = DPalette::Active;
//        p.setPen(Qt::NoPen);
//        p.setBrush(palette.color(cg, DPalette::ObviousBackground));
//        p.drawRoundedRect(0, 0, this->width(), this->height(), 16, 16);
//    }
//}

//void ImageItemWidget::drawImage(QPainter &p)
//{
//    QPainterPath path;
//    path.addRoundedRect(m_pos.x(), m_pos.y(), m_pIconSize.width(), m_pIconSize.height(), 8, 8);
//    p.setClipPath(path);

//    QPixmap pix =
//        m_phoneFileInfo.scaleIcon.pixmap(this->width(), this->height()); // setDevicePixelRatio(devicePixelRatioF());
//    pix.setDevicePixelRatio(devicePixelRatioF());
//    pix = pix.scaled(this->width() * devicePixelRatioF(),
//                     this->height() * devicePixelRatioF()); // setDevicePixelRatio(devicePixelRatioF());
//    p.drawPixmap(0, 0, pix);

//    //  如果是视频， 还需要画上时间
//    if (m_phoneFileInfo.type == VIDEO) { // 绘制时间
//        drawVideoTime(p);
//    }

//    if (m_bChecked == false) {
//        if (DApplicationHelper::instance()->themeType() == Dtk::Gui::DGuiApplicationHelper::ColorType::LightType) {
//            p.setPen(QColor(0, 0, 0, 26));
//        } else {
//            p.setPen(QColor(255, 255, 255, 50));
//        }

//        p.drawPath(path);
//    }
//}

///**
// * @brief 读取 图片、视频失败， 画 error 图
// * @param p
// */
//void ImageItemWidget::drawErrorImage(QPainter &p)
//{
//    DGuiApplicationHelper::ColorType ct = DApplicationHelper::instance()->themeType();
//    QString strTheme = ((ct == DGuiApplicationHelper::LightType) ? "light" : "dark");

//    QBrush bgDamaged = ((ct == DGuiApplicationHelper::LightType) ? Qt::white : QColor("#191919"));
//    p.setBrush(bgDamaged);
//    p.setPen(QPen(QColor(0, 0, 0, 35), 0.4));
//    p.drawRoundedRect(m_pos.x(), m_pos.y(), m_pIconSize.width(), m_pIconSize.height(), 8, 8);

//    QPixmap pixDamaged;
//    if (m_phoneFileInfo.type == IMAGE) {
//        pixDamaged = QPixmap(QString("://%1/picture_damaged.svg").arg(strTheme));
//    } else {
//        pixDamaged = QPixmap(QString("://%1/video_damaged.svg").arg(strTheme));
//    }
//    QRect rDamaged = pixDamaged.rect();
//    rDamaged.moveCenter(this->rect().center());
//    p.drawPixmap(rDamaged.topLeft(), pixDamaged); //背景
//}

//void ImageItemWidget::drawCheck(QPainter *p, const QRect &rect) const
//{
//    QRect rc = rect;
//    rc.setSize({20, 20});
//    rc.moveTopRight(QPoint(rect.right() - 5, rect.top() + 5));

//    int x = rc.x() + 1;
//    int y = rc.y() + 1;
//    int w = rc.width() - 2;
//    int h = rc.height() - 2;
//    QPainterPath rectPath;
//    rectPath.addEllipse(x, y, w, h);
//    p->fillPath(rectPath, QColor(255, 255, 255));

//    DStyleOptionButton check;
//    check.state = DStyle::State_On;
//    check.rect = rc;

//    DApplication::style()->drawPrimitive(DStyle::PE_IndicatorItemViewItemCheck, &check, p);
//}

//QString ImageItemWidget::textFromTime(int nTime)
//{
//    QString retTime;

//    //天数计算
//    int day = (nTime) / (24 * 3600);
//    //小时计算
//    int hour = (nTime) % (24 * 3600) / 3600;
//    //分钟计算
//    int minute = (nTime) % 3600 / 60;
//    //秒计算
//    int second = (nTime) % 60;

//    if (day != 0) {
//        retTime = QString("%1:").arg(day, 2, 10, QLatin1Char('0'));
//    }
//    if (hour != 0) {
//        retTime = QString("%1%2:").arg(retTime).arg(hour, 2, 10, QLatin1Char('0'));
//    }
//    retTime = QString("%1%2:").arg(retTime).arg(minute, 2, 10, QLatin1Char('0'));
//    retTime = QString("%1%2").arg(retTime).arg(second, 2, 10, QLatin1Char('0'));

//    return retTime;
//}

//void ImageItemWidget::reloadImage()
//{
//    if (m_phoneFileInfo.bIsDir)
//        return;

//    static QThreadPool p;
//    p.setMaxThreadCount(3);

//    if (m_phoneFileInfo.type == IMAGE) {
//        m_task = new ImageEngineTask();
//        m_task->setData(QStringList() << m_phoneFileInfo.path);
//        connect(m_task, &ImageEngineTask::sigImageBackLoaded, this, [this](const QString &path, const QPixmap &data) {
//            emit sigReloadImage(path, data);
//            m_task = nullptr;
//            m_phoneFileInfo.scaleIcon = data;
//            m_phoneFileInfo.damaged = data.isNull();
//            autoUpdate();
//        });

//        ThreadService::getService()->startTask(ThreadService::E_Image_Render_Task, m_task, &p);
//    } else if (m_phoneFileInfo.type == VIDEO) {
//    }
//}
