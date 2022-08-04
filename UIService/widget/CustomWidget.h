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
#ifndef CUSTOMWIDGET_H
#define CUSTOMWIDGET_H

#include <DWidget>

#include <QVBoxLayout>
#include <QStackedLayout>

#include "base/UIInterface.h"
#include "defines.h"
#include "base/UIModelManager.h"
#include "base/BaseListView.h"
#include "base/BaseTreeView.h"

class SpinnerWidget;

DWIDGET_USE_NAMESPACE

/**
 * @brief 应用各个 Ui界面 第一 继承， app、music、video、ebook、filemanager 基类
 */
class FileDisplayTask;
class CustomWidgetPrivate;
class CustomWidget : public DWidget
    , public UIInterface
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(CustomWidget)
public:
    explicit CustomWidget(DWidget *p = nullptr);
    virtual ~CustomWidget() override;

protected:
    void resizeEvent(QResizeEvent *event) override;

protected:
    void spinnerStart();
    void spinnerStop();
    void initMainLayout();
    void setShowText(const QString &strText);
    bool SpinnerIsVisible();
    bool execMsgBox(const QString &strText, bool bShowCancel = true); //返回 true同意
    /**
     * @brief 导出的文件夹路径 选择对话框
     * @return
     */
    QString getExportPath();
    /**
     * @brief 消息提醒
     */
    void sendOkMessage(const QString &);
    void sendWarnMessage(const QString &);

    bool checkOperating();

    void RemovePathPrefix(QString &path, int devType, QString devId);
    void setUIModel();

protected slots:
    virtual void slotTitleWidgetBtnClicked(const int &) = 0;
    virtual void slotCustomContextMenuRequested(const QPoint &);

private:
    void initSpinnerWidget();
    void initConnection();

protected:
    enum ViewMode { VIEW_ICON_MODE,
                    VIEW_LIST_MODE,
                    VIEW_EMPTY
    };

    int m_currFileViewMode = VIEW_ICON_MODE; //  当前显示 网格、列表

    BaseTreeView *m_pTreeViewFrame = nullptr;
    BaseListView *m_pListViewFrame = nullptr;
    UIModelManager *m_pUIModelManager = nullptr;

    SpinnerWidget *m_pSpinnerWidget = nullptr; //  每一个UI 对应自己的 spinner
    QVBoxLayout *m_pMmainLayout = nullptr;
    QStackedLayout *m_pStackViewLayout = nullptr;

    E_Widget_Type m_pWidgetType = E_No_Define;

    FileDisplayTask *m_loadTask = nullptr;
    CustomWidgetPrivate *d_ptr;
};

#endif // CUSTOMWIDGET_H
