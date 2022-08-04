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

#include "phonelistview.h"

#include <DApplication>
#include <QMouseEvent>
#include <QHeaderView>

#include "PhoneViewItemDelegate.h"
#include "TrObject.h"

#define TREE_WIDTH 220
#define ITEM_HEIGHT 40
#define ITEM_WIDTH 108
#define ICON_DATA (Qt::UserRole + 99)
#define ITEM_DATE_ROLE (Qt::UserRole + 66)
#define ITEM_PHONE_ROLE (Qt::UserRole + 67)
#define ITEM_SHOW_ROLE (Qt::UserRole + 68)

DWIDGET_USE_NAMESPACE

PhoneListView::PhoneListView(DWidget *parent)
    : DTreeView(parent)
{
    initUI();

    connect(this, &PhoneListView::expanded, this, &PhoneListView::onItemExpanded);
    connect(this, &PhoneListView::clicked, this, &PhoneListView::onItemClicked);
    connect(DApplicationHelper::instance(), &DApplicationHelper::themeTypeChanged, this,
            &PhoneListView::onChangedTheme);
    refreshTheme();
}

void PhoneListView::appendPhoneInfo(const PhoneInfo &info)
{
    if (isExist(info))
        return;
    QStandardItem *item = nullptr;
    item = new QStandardItem(info.strDevName);
    item->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));

    item->setData(info.type, ITEM_DATE_ROLE);
    item->setData(QVariant::fromValue(info), ITEM_PHONE_ROLE);
    //    item->setData(-1, ITEM_COUNT_ROLE);
    m_pModel->appendRow(item);

    refreshTheme();

    if (this->currentIndex().row() == -1) {
        this->setCurrentIndex(item->index());
        this->setExpanded(item->index(), true);
    }
}

void PhoneListView::removePhoneInfo(const QString &devId)
{
    for (auto i = 0; i < m_pModel->rowCount(); i++) {
        QStandardItem *item = m_pModel->item(i);
        if (item) {
            PhoneInfo curInfo = item->data(ITEM_PHONE_ROLE).value<PhoneInfo>();
            if (devId == curInfo.strPhoneID) {
                m_pModel->removeRows(i, 1);
                break;
            }
        }
    }
    if (!this->currentIndex().isValid())
        this->setCurrentIndex(m_pModel->index(0, 0));
}

void PhoneListView::resetPhoneInfo(const PhoneInfo &info)
{
    for (auto i = 0; i < m_pModel->rowCount(); i++) {
        QStandardItem *item = m_pModel->item(i);
        if (item) {
            PhoneInfo curInfo = item->data(ITEM_PHONE_ROLE).value<PhoneInfo>();
            if (info == curInfo) {
                item->setData(QVariant::fromValue(info), ITEM_PHONE_ROLE);

                QFontMetrics fm(item->font());
                //                QString str = fm.elidedText(info.strDevName, Qt::ElideRight, this->width() - 80);
                QString str = info.strDevName;
                item->setText(str);

                if (this->currentIndex() == item->index()) {
                    emit mainItemChanged(info);
                }

                if (info.status == STATUS_CONNECTED && item->rowCount() == 0) {
                    item->appendRows(getChildrenItem(info));
                    refreshTheme();
                    //                    this->setExpanded(item->index(), true);
                } else if (info.status != STATUS_CONNECTED) {
                    item->removeRows(0, item->rowCount());
                }
                break;
            }
        }
    }
}

bool PhoneListView::getCurPhoneInfo(PhoneInfo &info)
{
    QModelIndex idx = this->currentIndex();
    if (!idx.isValid())
        return false;
    if (idx.parent().isValid())
        return false;
    info = idx.data(ITEM_PHONE_ROLE).value<PhoneInfo>();
    return true;
}

/*
 * 根据设备ID，获取设备信息,找到返回 true,未找到返回 false //add by zhangliangliang
*/
bool PhoneListView::getPhoneInfo(QString phoneId, PhoneInfo &info)
{
    bool isFound = false;
    for (auto i = 0; i < m_pModel->rowCount(); i++) {
        QStandardItem *item = m_pModel->item(i);
        if (item != nullptr) {
            PhoneInfo curInfo = item->data(ITEM_PHONE_ROLE).value<PhoneInfo>();
            if (phoneId == curInfo.strPhoneID) {
                info = curInfo;
                isFound = true;
                break;
            }
        }
    }

    return isFound;
}

/*
 * 根据设备ID，更新保存的电量信息 //add by zhangliangliang
*/
void PhoneListView::updatePhoneBattery(QString phoneId, int value)
{
    for (auto i = 0; i < m_pModel->rowCount(); i++) {
        QStandardItem *item = m_pModel->item(i);
        if (item != nullptr) {
            PhoneInfo curInfo = item->data(ITEM_PHONE_ROLE).value<PhoneInfo>();
            if (phoneId == curInfo.strPhoneID) {
                curInfo.battery = value;
                item->setData(QVariant::fromValue(curInfo), ITEM_PHONE_ROLE);
                break;
            }
        }
    }
}

bool PhoneListView::changeItem(const E_Widget_Type &type)
{
    QModelIndex idx = this->currentIndex();
    if (!idx.isValid())
        return false;
    if (idx.parent().isValid())
        return false;

    QStandardItem *item = m_pModel->itemFromIndex(idx);
    if (item->hasChildren()) {
        this->setExpanded(idx, true);
    }

    for (int i = 0; i < item->rowCount(); ++i) {
        if (item->child(i)->data(ITEM_SHOW_ROLE).toInt() == type) {
            this->setCurrentIndex(item->child(i)->index());
            return true;
        }
    }
    return false;
}

//void PhoneListView::setCount(QString strPhoneID, E_Widget_Type type, int nCount)
//{
//    for (auto i = 0; i < m_pModel->rowCount(); i++) {
//        QStandardItem *item = m_pModel->item(i);
//        if (item) {
//            PhoneInfo curInfo = item->data(ITEM_PHONE_ROLE).value<PhoneInfo>();
//            if (strPhoneID == curInfo.strPhoneID) {
//                for (int i = 0; i < item->rowCount(); ++i) {
//                    QStandardItem *itemChild = item->child(i);
//                    if (type == itemChild->data(ITEM_SHOW_ROLE).toInt()) {
//                        itemChild->setData(nCount, ITEM_COUNT_ROLE);
//                        break;
//                    }
//                }
//                break;
//            }
//        }
//    }
//}

void PhoneListView::initUI()
{
    this->setLineWidth(0);
    this->setFrameStyle(QFrame::NoFrame);
    this->setVerticalScrollMode(QAbstractItemView::ScrollMode::ScrollPerItem);
    this->header()->hide();
    this->setFixedWidth(TREE_WIDTH);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setViewportMargins(0, 0, 0, 0);
    setRootIsDecorated(false); //隐藏前面的下拉箭头
    //    setItemsExpandable(false);

    m_pModel = new QStandardItemModel(this);
    this->setModel(m_pModel);
    auto m_pItemDelegate = new PhoneViewItemDelegate(this);
    //    connect(m_pItemDelegate, &PhoneViewItemDelegate::itemDisconn, this, &PhoneListView::onItemDisconn);
    this->setItemDelegate(m_pItemDelegate);
    bItemChange = false;
}

void PhoneListView::refreshTheme()
{
    DGuiApplicationHelper::ColorType ct = DApplicationHelper::instance()->themeType();
    onChangedTheme(ct);
}

void PhoneListView::setDefaultSelect()
{
    emit clicked(currentIndex());
}

void PhoneListView::setCustomFont(QStandardItem *item)
{
    Q_UNUSED(item)
    //    QFont font = item->font();
    //    font.setPointSize(11);
    //    item->setFont(font);
    //    item->setTextAlignment(Qt::AlignCenter);
    this->setIconSize(QSize(16, 16));
}

QList<QStandardItem *> PhoneListView::getChildrenItem(const PhoneInfo &info)
{
    QList<QStandardItem *> list;
    QStandardItem *item_app = new QStandardItem();
    item_app->setText(TrObject::getInstance()->getListViewText(List_App));
    item_app->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    item_app->setData(E_Widget_App, ITEM_SHOW_ROLE);
    //    item_app->setData(-1, ITEM_COUNT_ROLE);
    list.append(item_app);
    QStandardItem *item_photo = new QStandardItem();
    item_photo->setText(TrObject::getInstance()->getListViewText(List_Photo));
    item_photo->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    item_photo->setData(E_Widget_Photo, ITEM_SHOW_ROLE);
    //    item_photo->setData(-1, ITEM_COUNT_ROLE);
    list.append(item_photo);
    QStandardItem *item_video = new QStandardItem();
    item_video->setText(TrObject::getInstance()->getListViewText(List_Video));
    item_video->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    item_video->setData(E_Widget_Video, ITEM_SHOW_ROLE);
    //    item_video->setData(-1, ITEM_COUNT_ROLE);
    list.append(item_video);
    if (info.type == Mount_Android) {
        QStandardItem *item_music = new QStandardItem();
        item_music->setText(TrObject::getInstance()->getListViewText(List_Song));
        item_music->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
        item_music->setData(E_Widget_Music, ITEM_SHOW_ROLE);
        //        item_music->setData(-1, ITEM_COUNT_ROLE);
        list.append(item_music);
        QStandardItem *item_eBook = new QStandardItem();
        item_eBook->setText(TrObject::getInstance()->getListViewText(List_eBook));
        item_eBook->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
        item_eBook->setData(E_Widget_Book, ITEM_SHOW_ROLE);
        //        item_eBook->setData(-1, ITEM_COUNT_ROLE);
        list.append(item_eBook);
    }
    QStandardItem *item_file = new QStandardItem();
    item_file->setText(TrObject::getInstance()->getListViewText(List_File));
    item_file->setSizeHint(QSize(ITEM_WIDTH, ITEM_HEIGHT));
    item_file->setData(E_Widget_File, ITEM_SHOW_ROLE);
    //    item_file->setData(-1, ITEM_COUNT_ROLE);
    list.append(item_file);

    return list;
}

bool PhoneListView::isExist(const PhoneInfo &info)
{
    for (auto i = 0; i < m_pModel->rowCount(); i++) {
        QStandardItem *item = m_pModel->item(i);
        if (item) {
            PhoneInfo curInfo = item->data(ITEM_PHONE_ROLE).value<PhoneInfo>();
            if (info == curInfo) {
                return true;
            }
        }
    }
    return false;
}

void PhoneListView::onChangedTheme(DGuiApplicationHelper::ColorType themeType)
{
    //    icon = "://images/";
    icon = "://";
    icon += ((themeType == DGuiApplicationHelper::LightType) ? "light/" : "dark/");

    QString _itemIcon;
    QModelIndex idx = this->currentIndex();
    QStandardItem *currentItem = m_pModel->itemFromIndex(idx);

    for (auto i = 0; i < m_pModel->rowCount(); i++) {
        QStandardItem *item = m_pModel->item(i);

        if (item) {
            setCustomFont(item);
            if (item->data(ITEM_DATE_ROLE).toInt() == Mount_Android) {
                _itemIcon = icon + "android_small.svg";
            } else if (item->data(ITEM_DATE_ROLE).toInt() == Mount_Ios) {
                _itemIcon = icon + "apple_small.svg";
            }

            if (currentItem != nullptr && item == currentItem) {
                _itemIcon.replace(".svg", "_checked.svg");
            }
            item->setIcon(QIcon(_itemIcon));
            item->setData(_itemIcon, ICON_DATA);

            if (item->hasChildren()) {
                for (int i = 0; i < item->rowCount(); i++) {
                    QStandardItem *childitem = item->child(i);
                    setCustomFont(childitem);
                    if (childitem->data(ITEM_SHOW_ROLE).toInt() == E_Widget_App) {
                        _itemIcon = icon + "app_small.svg";
                    } else if (childitem->data(ITEM_SHOW_ROLE).toInt() == E_Widget_Photo) {
                        _itemIcon = icon + "photo_small.svg";
                    } else if (childitem->data(ITEM_SHOW_ROLE).toInt() == E_Widget_Video) {
                        _itemIcon = icon + "video_small.svg";
                    } else if (childitem->data(ITEM_SHOW_ROLE).toInt() == E_Widget_Music) {
                        _itemIcon = icon + "music_small.svg";
                    } else if (childitem->data(ITEM_SHOW_ROLE).toInt() == E_Widget_Book) {
                        _itemIcon = icon + "eBook_small.svg";
                    } else if (childitem->data(ITEM_SHOW_ROLE).toInt() == E_Widget_File) {
                        _itemIcon = icon + "file_small.svg";
                    }
                    if (currentItem != nullptr && childitem == currentItem) {
                        _itemIcon.replace(".svg", "_checked.svg");
                    }
                    childitem->setIcon(QIcon(_itemIcon));
                    childitem->setData(_itemIcon, ICON_DATA);
                }
            }
        }
    }
}

void PhoneListView::onItemExpanded(QModelIndex idx)
{
    this->setCurrentIndex(idx);
    for (int i = 0; i < m_pModel->rowCount(); ++i) {
        QModelIndex itemIdx = m_pModel->index(i, 0);
        if (itemIdx != idx) {
            this->setExpanded(itemIdx, false);
        }
    }
}

void PhoneListView::onItemClicked(QModelIndex idx)
{
    if (!bItemChange) {
        if (m_pModel->itemFromIndex(idx)->hasChildren()) {
            if (this->isExpanded(idx))
                this->setExpanded(idx, false);
            else
                this->setExpanded(idx, true);
        }
    }
    //如果点同一个item 会自动伸缩
    bItemChange = false;
}

void PhoneListView::slotDeviceReconnect()
{
    PhoneInfo info;
    getCurPhoneInfo(info);
    info.status = STATUS_CONNECTED;
    resetPhoneInfo(info);
}

void PhoneListView::slotShowChanged(const E_Widget_Type &type)
{
    changeItem(type);
}

void PhoneListView::mouseMoveEvent(QMouseEvent *event)
{
    if ((QApplication::keyboardModifiers() == Qt::ControlModifier)) {
        return;
    }
    DTreeView::mouseMoveEvent(event);
}

void PhoneListView::mousePressEvent(QMouseEvent *event)
{
    if ((QApplication::keyboardModifiers() == Qt::ControlModifier)) {
        return;
    }
    if (event->button() == Qt::RightButton) {
        return;
    }
    DTreeView::mousePressEvent(event);
}
