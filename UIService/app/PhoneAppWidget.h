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
#ifndef PHONEAPPWIDGET_H
#define PHONEAPPWIDGET_H

#include "defines.h"

#include "widget/CustomWidget.h"

DWIDGET_USE_NAMESPACE

class PhoneAppWidget : public CustomWidget
{
    Q_OBJECT

public:
    explicit PhoneAppWidget(DWidget *parent = nullptr);
    ~PhoneAppWidget() override;

    // UIInterface interface
public:
    void updateWidgetInfo(const PhoneInfo &info) override;
    /**
     * @brief 左侧切换选项， 右侧也切换， 需要根据当前显示设置 按钮状态
     * @author  wzx
     */
    void setWidgetBtnState() override;

    /**
     * @brief 清除数据
     * @param devInfo
     */
    void clearWidgetInfo(const QString &devInfo) override;

protected:
    /**
     * @brief 右键刷新
     */
    void refreshWidgetData() override;
    // 是否开启读取线程 QThread::isRunning
    virtual bool isReading() override { return false; }
    // 暂停读取
    virtual bool pauseRead() override { return false; }
    // 继续读取
    virtual bool continueRead() override { return false; }

private slots:
    /**
     * @brief 卸载 单个应用
     * @param index
     */
    void slotUninstallClicked(const QModelIndex &index); //add by Airy  单选删除

    /**
     * @brief 导出单个应用
     * @param index
     */
    void slotExportApp(const QModelIndex &index); //add by Airy  单选删除

    /**
     * @brief 标题栏 按钮点击
     */
    void slotTitleWidgetBtnClicked(const int &) override;

    /**
     * @brief tree 列表选中状态变化处理
     */
    void slotUpdateSelectData(const bool &);

    /**
     * @brief 表头全选处理
     */
    void slotUpdateSelectAll(const bool &);

    /**
     * @brief 数据加载完毕
     */
    void slotLoadDataEnd();

private:
    void initUi();
    void initConnection();

    /**
     * @brief 多选卸载App
     */
    void uninstallApp();

    /**
     * @brief 多选导出App
     */
    void exportApp();

    /**
     * @brief 更新选中内容信息
     */
    void calculateSelectCountSize() override;

    /**
     * @brief 卸载结束之后的操作
     */
    void afterUninstallApp();

    /**
     * @brief 设置当前机器的设备id 和 设备类型
     * @param sDeviceId
     * @param nType
     */
    void setPhoneTypeAndId(const QString &sDeviceId, const DEVICE_TYPE &nType); //add by Airy for:get phone type

private:
    QString m_devId = ""; //手机设备id
};

#endif // PHONEAPPWIDGET_H
