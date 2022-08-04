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
#include "PhoneAppWidget.h"

#include <QDebug>
#include <QHeaderView>
#include <QFileInfo>
#include <QDir>

#include <DFrame>
#include <DDialog>

#include "widget/progresswidget.h"

#include "widget/SpinnerWidget.h"
#include "widget/TitleWidget.h"
#include "widget/RightTitleWidget.h"
#include "TrObject.h"
#include "MountService.h"
#include "utils.h"

#include "AppTreeView.h"
#include "PhoneTreeAppModel.h"
//#include "ItemModel/SortFilterProxyModel.h"

#define CHECKBOX_WIDTH 50
#define NAME_WIDTH 380
#define APP_MESSAGE_COLUMN 5
#define IOS_UNINSTALL_OVERTIME 5000
#define ANDRIOD_UNINSTALL_FAILED -1
/**
* @brief  卸载应用超时
*/
#define UNINSTALL_OVERTIME 10000

PhoneAppWidget::PhoneAppWidget(DWidget *parent)
    : CustomWidget(parent)
{
    setObjectName("PhoneAppWidget");
    m_pWidgetType = E_Widget_App;

    initUi();
    initConnection();

    //#ifdef QT_DEBUG
    //    auto timer = new QTimer(this);
    //    connect(timer, &QTimer::timeout, this, [=]() {
    //        refreshWidgetData();
    //    });
    //    timer->start(10 * 1000);
    //#endif
}

PhoneAppWidget::~PhoneAppWidget()
{
}

void PhoneAppWidget::initUi()
{
    m_pTreeViewFrame = new AppTreeView(this);

    connect(static_cast<AppTreeView *>(m_pTreeViewFrame), &AppTreeView::sigDeleteData, this, &PhoneAppWidget::slotUninstallClicked);
    connect(static_cast<AppTreeView *>(m_pTreeViewFrame), &AppTreeView::sigExportData, this, &PhoneAppWidget::slotExportApp);
    connect(m_pTreeViewFrame, &BaseTreeView::sigLoadDataEnd, this, &PhoneAppWidget::slotLoadDataEnd);
    connect(m_pTreeViewFrame, &BaseTreeView::sigSelectAll, this, &PhoneAppWidget::slotUpdateSelectAll);
    connect(m_pTreeViewFrame, &BaseTreeView::sigUpdateSelectData, this, &PhoneAppWidget::slotUpdateSelectData);

    m_pTreeViewFrame->setColumnWidth(0, CHECKBOX_WIDTH);
    m_pTreeViewFrame->setColumnWidth(1, NAME_WIDTH);
    m_pTreeViewFrame->setColumnHidden(4, true); //add by Airy

    initMainLayout();

    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Delete_Btn, false);
    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Export_Btn, false);
}

void PhoneAppWidget::initConnection()
{
}

//  app 数据 只需要 类型、设备id
void PhoneAppWidget::setPhoneTypeAndId(const QString &sDeviceId, const DEVICE_TYPE &nType)
{
    auto model = m_pTreeViewFrame->getSourceModel();
    if (model) {
        bool m_bIsReload = false;

        //       m_devId = model->strPhoneDevId();
        //隐藏IOS的应用大小列
        if (nType == Mount_Ios) {
            TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Export_Btn, false);

            m_pTreeViewFrame->setColumnHidden(2, true);
            static_cast<AppTreeView *>(m_pTreeViewFrame)->setIsCanExport(false);

            if ((sDeviceId != m_devId)) {
                m_bIsReload = true;
            }
        } else {
            TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Export_Btn, true);

            m_pTreeViewFrame->setColumnHidden(2, false);
            static_cast<AppTreeView *>(m_pTreeViewFrame)->setIsCanExport(true);

            //  如果id 不一样， 刷新应用信息
            if ((sDeviceId != m_devId)) {
                m_bIsReload = true;
            }
        }
        m_devId = sDeviceId;
        if (m_bIsReload) {
            model->setStrPhoneDevId(sDeviceId);
            model->setNDeviceType(nType);

            refreshWidgetData();
        }
    }
}

/**
 * @brief PhoneAppWidget::setWidgetBtnState 左侧切换选项， 右侧也切换， 需要根据当前显示设置 按钮状态
 * @author  wzx
 */
void PhoneAppWidget::setWidgetBtnState()
{
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Left_Btn, false);
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Right_Btn, false);
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_New_Folder_Btn, false);
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Import_Btn, false);
    TitleWidget::getInstance()->setWidgetBtnVisible(TitleWidget::E_Delete_Btn, true);
    RightTitleWidget::getInstance()->setWidgetBtnVisible(RightTitleWidget::E_Icon_View_Btn, false);
    RightTitleWidget::getInstance()->setWidgetBtnVisible(RightTitleWidget::E_List_View_Btn, false);

    bool rl = m_pTreeViewFrame->selectionModel()->hasSelection();
    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Delete_Btn, rl);
    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Export_Btn, rl);
}

//拔掉手机，则将设备id置空，此时重新刷新数据
void PhoneAppWidget::clearWidgetInfo(const QString &devInfo)
{
    Q_UNUSED(devInfo);
    if (devInfo == m_devId) {
        m_devId = "";
    }
}

/**
 * @brief 应用刷新
 */
void PhoneAppWidget::refreshWidgetData()
{
    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Delete_Btn, false);
    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Export_Btn, false);

    auto model = m_pTreeViewFrame->getSourceModel();

    if (model && m_pTreeViewFrame) {
        m_pTreeViewFrame->setHeaderViewEnable(false);
        m_pTreeViewFrame->setHeaderViewState(false);

        spinnerStart();

        m_pTreeViewFrame->selectionModel()->blockSignals(true);

        model->reloadModel();

        m_pTreeViewFrame->selectionModel()->blockSignals(false);
    }
}

/**
 * @brief  执行导出，删除操作
 * @param[in] E_Export_Btn：导出按钮 E_Delete_Btn 删除按钮
 */
void PhoneAppWidget::slotTitleWidgetBtnClicked(const int &nId)
{
    if (this->isVisible() == false)
        return;

    if (nId == TitleWidget::E_Export_Btn) {
        exportApp();
    } else if (nId == TitleWidget::E_Delete_Btn) {
        uninstallApp();
    }
}

/**
 * @brief tree 列表选中状态变化处理
 *  @param[in] bSelection 头部按钮状态
 */
void PhoneAppWidget::slotUpdateSelectData(const bool &bSelection)
{
    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Delete_Btn, bSelection);
    TitleWidget::getInstance()->setWidgetBtnEnable(TitleWidget::E_Export_Btn, bSelection);

    calculateSelectCountSize();
}

/**
 * @brief 表头全选处理
 * @param[in] rl 是否全选
 */
void PhoneAppWidget::slotUpdateSelectAll(const bool &rl)
{
    slotUpdateSelectData(rl);
}

/**
 * @brief 更新选中内容信息
 */
void PhoneAppWidget::calculateSelectCountSize()
{
    QString strText = "";

    auto model = m_pTreeViewFrame->getSourceModel();

    if (m_pTreeViewFrame->selectionModel()->hasSelection()) {
        auto indexList = m_pTreeViewFrame->selectionModel()->selectedRows(2); //  取第一列的选中项
        DEVICE_TYPE type = model->nDeviceType();

        if (type == Mount_Android) {
            uint64_t uCheckedSize = 0;

            foreach (auto index, indexList) {
                uCheckedSize += index.data(ROLE_ITEM_FILE_SIZE).toUInt(); // value<PhoneAppInfo>();
                //                uCheckedSize += stAppInfo.nAppSize;
            }
            strText = TrObject::getInstance()->getFileOperationText(Total_item_selected_size).arg(model->rowCount()).arg(indexList.size()).arg(Utils::dataSizeToString(uCheckedSize));
        } else {
            strText = TrObject::getInstance()->getFileOperationText(Total_item_selected).arg(model->rowCount()).arg(indexList.size());
        }
    } else { //  不选中的时候, 只显示个数
        strText = TrObject::getInstance()->getFileOperationText(items_count).arg(model->rowCount());
    }

    spinnerStop();
    setShowText(strText);
}
/**
 * @brief 卸载 单个APK
 * @param index
 */
void PhoneAppWidget::slotUninstallClicked(const QModelIndex &index)
{
    auto tip = new DDialog(QString(TrObject::getInstance()->getFileOperationText(Uninstall_App)), "", this);
    tip->setIcon(QIcon::fromTheme("dialog-warning"));
    tip->addButton(TrObject::getInstance()->getDlgBtnText(Dlg_Btn_cancel), false, DDialog::ButtonNormal);
    tip->addButton(TrObject::getInstance()->getDlgBtnText(Dlg_Btn_Confirm), true, DDialog::ButtonRecommend);

    int Ok = tip->exec();
    if (Ok == DDialog::Accepted) {
        //删除过程中禁用全选
        m_pTreeViewFrame->setHeaderViewEnable(true);

        QString strPacketName = index.data(APP_PACKAGE_NAME).toString();
        QString strAppName = index.data(APP_APK_NAME).toString();

#ifdef QT_DEBUG
        qDebug() << __FUNCTION__ << "app packagename = " << strPacketName << "   showName = " << strAppName;
#endif
        if (strPacketName != "" && strAppName != "") {
            auto model = m_pTreeViewFrame->getSourceModel();

            DEVICE_TYPE type = model->nDeviceType();
            QString m_devId = model->strPhoneDevId();

            switch (type) {
            case Mount_Android: {
                QString sRes = MountService::getService()->uninstallPhoneAPP(Mount_Android, m_devId, strPacketName, UNINSTALL_OVERTIME);
                qDebug() << __LINE__ << __FUNCTION__ << sRes;
                if (sRes.indexOf("Success") == ANDRIOD_UNINSTALL_FAILED) {
                    sendWarnMessage(TrObject::getInstance()->getDeviceLabelText(AppUninstallFailed)); //暂时不输出导出失败信息
                } else {
                    model->deleteRowByPath(strAppName, strPacketName);
                }
            } break;
            case Mount_Ios: {
                QTime startTime = QTime::currentTime();
                MountService::getService()->uninstallPhoneAPP(Mount_Ios, m_devId, strPacketName, UNINSTALL_OVERTIME);
                QTime stopTime = QTime::currentTime();
                int elapsed = startTime.msecsTo(stopTime);
                qDebug() << "QTime.currentTime =" << elapsed << "ms";
                if (elapsed < IOS_UNINSTALL_OVERTIME) {
                    model->deleteRowByPath(strAppName, strPacketName);
                } else {
                    sendWarnMessage(TrObject::getInstance()->getDeviceLabelText(AppUninstallFailed));
                }

            } break;
            default:
                break;
            }
        }

        afterUninstallApp();
    }
    delete tip;
}

/**
 * @brief  多选情况下卸载APK
 */
void PhoneAppWidget::uninstallApp()
{
    auto model = m_pTreeViewFrame->getSourceModel(); //获取App数据
    DEVICE_TYPE type = model->nDeviceType(); //获取数据类型，ios,andriod
    QString m_devId = model->strPhoneDevId(); //获取数据的设备ID

    //对话框title显示内容，单复数有变化
    QString sMsg = TrObject::getInstance()->getFileOperationText(Uninstall_App);
    int nDelRows = m_pTreeViewFrame->selectionModel()->selectedRows().size();
    if (nDelRows > 1) {
        sMsg = TrObject::getInstance()->getFileOperationText(Uninstall_Apps);
    }

    /**
     * @brief 增加为空判断，数据数据
     */
    auto selectIndexs = m_pTreeViewFrame->selectionModel()->selectedRows(APP_MESSAGE_COLUMN);
    if (selectIndexs.isEmpty()) {
        sendWarnMessage(TrObject::getInstance()->getFileOperationText(Please_select_a_file));
        return;
    }

    //弹出删除选择对话框
    DDialog *tip = new DDialog(sMsg, "", this);
    tip->setIcon(QIcon::fromTheme("dialog-warning"));
    tip->addButton(TrObject::getInstance()->getDlgBtnText(Dlg_Btn_cancel), false, DDialog::ButtonNormal);
    tip->addButton(TrObject::getInstance()->getDlgBtnText(Dlg_Btn_Confirm), true, DDialog::ButtonRecommend);

    int Ok = tip->exec();
    if (Ok == DDialog::Accepted) {
        m_pTreeViewFrame->selectionModel()->blockSignals(true);
        bool bRet = false;

        //将选择要删除的几个项放到一个list中，统一处理
        QStringList selectAppList;
        foreach (auto item, selectIndexs) {
            selectAppList << item.data(APP_PACKAGE_NAME).toString();
        }

        int uninstallErrorCount = 0; //删除失败计数标志位
        QString uninstallErrorStr;

        //遍历需要删除的选项，根据是否成功卸载，进行删除选项
        foreach (auto apkName, selectAppList) {
            if (apkName != "") {
                switch (int(type)) {
                case Mount_Android: {
                    QString sRes = MountService::getService()->uninstallPhoneAPP(Mount_Android, m_devId, apkName, UNINSTALL_OVERTIME);
                    qDebug() << __LINE__ << __FUNCTION__ << sRes;

                    if (sRes.indexOf("Success") == ANDRIOD_UNINSTALL_FAILED) {
                        uninstallErrorCount++;
                        bRet = false;
                    } else {
                        bRet = true;
                    }
                    break;
                }
                case Mount_Ios:
                    // ios不论卸载是否成功，返回值都是一样的，IOS的库卸载app时，不论是否卸载成功，返回值都一样。
                    //并且测试在x86上测试卸载失败需要5s以上，而成功卸载一个2g左右的APP只需要1.5左右。
                    //故使用执行时长作为判断卸载失败与否的判断依据。

                    QTime startTime = QTime::currentTime();
                    QString sRes = MountService::getService()->uninstallPhoneAPP(Mount_Ios, m_devId, apkName, UNINSTALL_OVERTIME);
                    qDebug() << __LINE__ << __FUNCTION__ << sRes;
                    QTime stopTime = QTime::currentTime();
                    int elapsed = startTime.msecsTo(stopTime);
                    qDebug() << "QTime.currentTime =" << elapsed << "ms";
                    if (elapsed > IOS_UNINSTALL_OVERTIME) {
                        uninstallErrorCount++;
                        bRet = false;
                    } else {
                        bRet = true;
                    }
                    break;
                }
            }

            if (bRet) {
                auto model = m_pTreeViewFrame->getSourceModel();
                for (auto i = model->rowCount() - 1; i >= 0; i--) {
                    QString strApkName = model->index(i, APP_MESSAGE_COLUMN).data(APP_PACKAGE_NAME).toString();
                    if (apkName.compare(strApkName) == 0) {
                        model->removeRow(i);
                        break;
                    }
                }
            }
            bRet = false;
        }
        if (uninstallErrorCount != 0)
            sendWarnMessage(TrObject::getInstance()->getDeviceLabelText(AppsUninstallFailed).arg(uninstallErrorCount));

        m_pTreeViewFrame->reset();
        m_pTreeViewFrame->selectionModel()->blockSignals(false);
        afterUninstallApp();
    }
    delete tip;
}

/**
 * @brief 导出 单个APk
 * @param index
 */
void PhoneAppWidget::slotExportApp(const QModelIndex &index)
{
    QString pathFile = getExportPath();
    if (pathFile == "")
        return;

    auto model = m_pTreeViewFrame->getSourceModel();
    auto mount = MountService::getService();
    QString strPacketName = index.data(APP_PACKAGE_NAME).toString();
    QString sApkName = index.data(APP_APK_NAME).toString();

    if (strPacketName == "" || sApkName == "")
        return;

    QString m_devId = model->strPhoneDevId();

    QString strPacketPath = MountService::getService()->packagePath(m_devId, strPacketName);
    QString strPhonePath = strPacketPath.remove("package:/");

    if (strPhonePath == "")
        return;

    QString old_APKName = "\"" + pathFile + "/" + sApkName + ".apktmp" + "\""; //导出过程中，安装包名称
    QString new_APKName = "\"" + pathFile + "/" + sApkName + ".apk" + "\""; //导出完成后安装包名称

    QString strExpotr = mount->exportPackage(m_devId, strPhonePath, old_APKName, -1); //导出APP操作
    qDebug() << __LINE__ << __FUNCTION__ << strExpotr;
    //导出是否成功判断
    if (strExpotr.contains("1 file pulled") & strExpotr.contains("100%"))
        mount->changeInstallationPackageName(m_devId, old_APKName, new_APKName);

    QString strText = TrObject::getInstance()->getImportExportText(Export_success);

    sendOkMessage(strText);
}

/**
 * @brief 导出多个安装包
 */
void PhoneAppWidget::exportApp()
{
    auto mount = MountService::getService();

    auto model = m_pTreeViewFrame->getSourceModel();
    QString devId = model->strPhoneDevId();
    auto indexList = m_pTreeViewFrame->selectionModel()->selectedRows(APP_MESSAGE_COLUMN); //根据第五列选中判断
    /**
     * @brief 增加为空判断，数据数据
     */
    if (indexList.isEmpty()) {
        sendWarnMessage(TrObject::getInstance()->getFileOperationText(Please_select_a_file));
        return;
    }

    //选择保存文件夹路径
    QString pathFile = getExportPath();
    if (pathFile == "")
        return;

    foreach (auto index, indexList) {
        QString strPacketName = index.data(APP_PACKAGE_NAME).toString();
        QString strApkName = index.data(APP_APK_NAME).toString();
        if (strPacketName == "" || strApkName == "")
            continue;
        QString strPacketPath = mount->packagePath(devId, strPacketName);
        QString strPhonePath = strPacketPath.remove("package:/"); //获取apk的安装包路径
        if (strPhonePath == "")
            continue;
        QString old_APKName = "\"" + pathFile + "/" + strApkName + ".apktmp" + "\""; //导出中间状态安装包名称
        QString new_APKName = "\"" + pathFile + "/" + strApkName + ".apk" + "\""; //导出成功后安装包名称

        QString strExpotr = mount->exportPackage(devId, strPhonePath, old_APKName, -1); //导出APP操作
        //        qDebug() << __LINE__ << __FUNCTION__ << strExpotr;
        //是否导出成功判断
        if (strExpotr.contains("1 file pulled") & strExpotr.contains("100%"))
            mount->changeInstallationPackageName(devId, old_APKName, new_APKName);
    }
    //导出成功提示语
    QString strText = "";
    if (indexList.size() == m_pTreeViewFrame->model()->rowCount()) {
        strText = TrObject::getInstance()->getImportExportText(Export_success);
    } else {
        strText = TrObject::getInstance()->getImportExportText(app_export_count).arg(indexList.size());
    }
    sendOkMessage(strText);
}

/**
 * @brief  删除之后， 判断全选、勾选状态
 */
void PhoneAppWidget::afterUninstallApp()
{
    auto model = m_pTreeViewFrame->getSourceModel();
    auto rowCount = model->rowCount();
    if (rowCount > 0) {
        auto selectRows = m_pTreeViewFrame->selectionModel()->selectedRows();
        if (selectRows.count() == rowCount) {
            m_pTreeViewFrame->setHeaderViewState(true);
        } else {
            m_pTreeViewFrame->setHeaderViewState(false);
        }
    } else {
        m_pTreeViewFrame->setHeaderViewEnable(false);
        m_pTreeViewFrame->setHeaderViewState(false);
    }

    bool rl = m_pTreeViewFrame->selectionModel()->hasSelection();
    slotUpdateSelectData(rl);
}
/**
 * @brief 数据加载完毕
 */
void PhoneAppWidget::slotLoadDataEnd()
{
    spinnerStop();

    auto model = m_pTreeViewFrame->getSourceModel();

    m_pTreeViewFrame->setHeaderViewEnable(model->rowCount()); //  header checkbox 是否可点

    calculateSelectCountSize();
}

void PhoneAppWidget::updateWidgetInfo(const PhoneInfo &info)
{
    setPhoneTypeAndId(info.strPhoneID, info.type);
}
