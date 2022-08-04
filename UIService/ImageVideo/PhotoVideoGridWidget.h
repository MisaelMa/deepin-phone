///*
// * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co., Ltd.
// *
// * Author:     yelei <yelei@uniontech.com>
// * Maintainer: yelei <yelei@uniontech.com>
// * This program is free software: you can redistribute it and/or modify
// * it under the terms of the GNU General Public License as published by
// * the Free Software Foundation, either version 3 of the License, or
// * any later version.
// * This program is distributed in the hope that it will be useful,
// * but WITHOUT ANY WARRANTY; without even the implied warranty of
// * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// * GNU General Public License for more details.
// * You should have received a copy of the GNU General Public License
// * along with this program.  If not, see <http://www.gnu.org/licenses/>.
// */
//#ifndef PHOTOGRIDWIDGET_H
//#define PHOTOGRIDWIDGET_H

//#include "defines.h"
//#include "phonefilethread.h"

//#include <DLabel>
//#include <dflowlayout.h>
//#include <DFloatingButton>
//#include <QStackedLayout>
//#include <DCommandLinkButton>

//#include "widget/CustomWidget.h"

//DWIDGET_USE_NAMESPACE

//class PhoneFileThread;
//class ImageItemWidget;

//class PhotoVideoGridWidget : public CustomWidget
//{
//    Q_OBJECT
//public:
//    enum RunMode {
//        Photo = 0, //照片模式
//        Video //视频模式
//    };

//    explicit PhotoVideoGridWidget(const RunMode &mode, DWidget *parent = nullptr);
//    virtual ~PhotoVideoGridWidget() override;

//    //初始化照片视频的内存
//    static void initCache(const PhoneInfo &info);
//    //根据手机信息刷新界面
//    void updatePhoneInfo(const PhoneInfo &info);

//public:
//    void setWidgetBtnState() override;
//    //提供的手机信息与界面一致才会清空，防止清错
//    void clearWidgetInfo(const QString &devInfo) override;

//    // CustomWidget interface
//protected:
//    void refreshWidgetData() override;
//    void slotTitleWidgetBtnClicked(const int &) override;
//    // 是否开启读取线程 QThread::isRunning
//    virtual bool isReading() override { return false; }
//    // 暂停读取
//    virtual bool pauseRead() override { return false; }
//    // 继续读取
//    virtual bool continueRead() override { return false; }

//private slots:
//    void slotStackedCurrentChanged(int);

//    void onFileError(int nErrorCode, QString strErrorMsg);
//    void onFindPicture(PhoneFileInfo &info);
//    void onReloadImage(const QString &path, const QPixmap &data);
//    void reloadFileInfo(PhoneFileInfo &info);
//    void onEnterFolder(const QString &strPath); //进入文件夹
//    void onItemSelectChanged(); //选中状态改变
//    void onDeleteBtnClicked();
//    void onExportBtnClicked();
//    void onImportBtnClicked();
//    void onGridBtnClicked();
//    void onListBtnClicked();
//    void onProgress(int nRunMode, int nCur, int nTotal, QString strFileName);
//    void onProgressResult(int nRunMode, int nCount, int nSuccess, int nFailed, bool bIsStop); // 结果
//    void onListWgtFileSelectChanged(QString strFilePath, bool bSelected); //列表选择改变
//    void onListWgtSelectAll(bool bSelected); // 列表全选
//    void onImportProgressClose();
//    void onExportProgressClose();
//    void loadImage(const QStringList &listFilePath);

//private:
//    void initUI();
//    void initConn();
//    void clear();
//    void delItem(QString strPath, bool bHasFailed); // 根据文件路径 删除item
//    void refreshMode(QString strPath = ""); // 根据信息重新计算大小位置显示数据
//    void refreshUI();
//    void restoreAll(); //恢复初始状态
//    void selectAll(bool bSelected, bool bSendSig = false); //选中 是否发出改变状态信号
//    bool isSelectedAll();
//    void checkFileRepeat(QStringList &listSrcFile, QString strDesPath, bool bRecount = false); //验重
//    QList<ImageItemWidget *> getCheckedItems();
//    QString getCurFilters();
//    void setTitleBtnEnble(); //选中切换 按钮改变
//    void calcTotal(); // 计算文件总数
//    void refreshStatusBar(); //状态栏刷新
//    void clearCache(const QString &strPhoneID = ""); // 清空缓存

//    bool fileListContains(QStringList &listSrcFile, QString &strFile, QString &strSrcAbsolutePath);

//private:
//    RunMode m_runMode;
//    QScrollArea *m_gridWgt; // 网格
//    DCommandLinkButton *m_btnChoose = nullptr; // 全选按钮
//    DFlowLayout *m_mainLayout;
//    QSize m_sItem; // item大小
//    QSize m_sIcon; // item中icon大小
//    bool m_bIsAlbum; // 是否为相册界面
//    QString m_strLastPath; // 最后进入的目录 方便返回
//    DLabel *lbl_title;
//    PhoneInfo m_phoneInfo;
//    QList<ImageItemWidget *> m_listPhotoItem;
//    QStringList m_nDeletePendingData; // 待处理数据

//    PhoneFileThread *m_tRead;
//};

//#endif // PHOTOGRIDWIDGET_H
