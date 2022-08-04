/*
* Copyright (C) 2019 ~ 2020 Deepin Technology Co., Ltd.
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
#ifndef PHOTOVIDEOWIDGET_H
#define PHOTOVIDEOWIDGET_H

#include <QWidget>
#include <DLabel>
#include <DCommandLinkButton>

#include "defines.h"
#include "widget/CustomWidget.h"

DWIDGET_USE_NAMESPACE

class PhotoListFrame;
class PhotoTreeFrame;
class PhoneFileThread;

class PMThread;

class PhotoVideoWidget : public CustomWidget
{
    Q_OBJECT
public:
    enum RunMode {
        Photo = 0, // 照片模式
        Video // 视频模式
    };

    explicit PhotoVideoWidget(const RunMode &mode, QWidget *parent = nullptr);
    virtual ~PhotoVideoWidget() override;

    // UIInterface interface
public:
    void updateWidgetInfo(const PhoneInfo &info) override;

public:
    // 初始化照片视频的内存
    //    static void initCache(const PhoneInfo &info);
    // 设置 顶部标题栏 按钮状态
    void setWidgetBtnState() override;
    // 提供的手机信息与界面一致才会清空，防止清错
    void clearWidgetInfo(const QString &devInfo) override;
    // 数据是否为空
    bool isEmpty();

protected:
    // 菜单刷新按钮
    void refreshWidgetData() override;
    // 标题栏按钮
    void slotTitleWidgetBtnClicked(const int &nId) override;
    bool event(QEvent *e) override;
    // 是否开启读取线程 QThread::isRunning
    virtual bool isReading() override;
    // 暂停读取
    virtual bool pauseRead() override;
    // 继续读取
    virtual bool continueRead() override;

private:
    // 初始化界面
    void initUI();
    // 初始化连接
    void initConn();
    // 获取网格
    PhotoListFrame *getListFrame();
    // 获取列表
    PhotoTreeFrame *getTreeFrame();
    // 刷新展示数据, 参数为子界面路径
    void refreshData(QString strPath = "");
    // 初始化
    void restoreAll();
    // 刷新UI界面(控件)
    void refreshUI();
    // 刷新title button
    void refreshTitleBtn();
    // 刷新状态栏
    void calculateSelectCountSize() override;
    // 刷新提示框
    void refreshPromptDlg();
    // 清空
    void clear();
    // 清空缓存
    void clearCache(const QString &strPhoneID = "");
    // 多线程读取（待用）
    void loadImage(const QStringList &listFilePath);
    // 根据文件路径 删除item
    void delItem(QString strPath);
    // 全选
    void selectAll(bool bSelected, bool bOnlyList = false);

private:
    void onCopyFile(QStringList &listPendingData, const QString &strResPath, const int &copyType);
    // 获取过滤字符串
    QString getCurFilters();
    // 验重
    void checkFileRepeat(QStringList &listSrcFile, QString strDesPath);
    // listSrcFile(全路径)中是否包含strFile(文件名)  返回listSrcAbsolutePath: 可能有多个目录下的同名文件
    bool fileListContains(const QStringList &listSrcFile, const QString &strFile, QStringList &listSrcAbsolutePath);
    // 从界面中验重
    bool contains(PhoneFileInfo &info);

private slots:
    // 删除(按钮)
    void onDeleteBtnClicked();
    // 导出(按钮)
    void onExportBtnClicked();
    // 导入(按钮)
    void onImportBtnClicked();
    // 全选(按钮)
    void onChooseBtnClicked();
    // 网格(按钮)
    void onGridBtnClicked();
    // 列表(按钮)
    void onListBtnClicked();
    // 列表网格切换
    void slotStackedCurrentChanged(int nIndex);
    // 处理文件相关报错
    void onFileError(int nErrorCode, QString strErrorMsg);
    void slotCopyFileError(QString strErrorMsg);

    // 处理文件相关报错
    void slotDeleteFileError(QString strErrorMsg);
    // 读取到图片
    void onFindPicture(PhoneFileInfo &info);
    // 导入导出中断
    void onImportExportProgressClose();
    // 导入中断
    //    void onImportProgressClose();
    // 导出中断
    //    void onExportProgressClose();
    // 进入文件夹
    void onEnterFolder(const QString &strPath);
    // 进度
    void slotCopyProgress(int nCur, int nTotal, QString strFileName);
    // 进度结果
    void slotCopyProgressResult(int, int nCount, int nSuccess, int nFailed, bool bIsStop);
    // 进度
    void slotDeleteProgress(QString strFileName);
    // 进度结果
    void slotDeleteProgressResult(int nFailed, bool bIsStop);
    // 列表全选
    void onTreeWgtSelectAll(bool bSelected);
    // 单选信号
    void onItemSelect(QString sPath, bool bSelected);
    // 系统文字变换
    void onFontChanged();
    // 读取线程结束
    void onReadFinished();

private:
    RunMode m_runMode; // 运行模式
    PhoneInfo m_phoneInfo; // 手机信息
    bool m_bIsAlbum; // 是否为相册界面
    QString m_strLastPath; // 最后进入的目录 方便返回
    QStringList m_nDeletePendingData; // 待处理数据

    DLabel *lbl_title; // title名称
    DCommandLinkButton *m_btnChoose = nullptr; // 全选按钮

    PhoneFileThread *m_tRead = nullptr;

    //    PhoneFileThread *m_tPhotoVideoImport = nullptr;
    //    PhoneFileThread *m_tPhotoVideoExport = nullptr;

    PMThread *m_tPhotoVideoCopy = nullptr;
    PMThread *m_pDeleteThread = nullptr;
};

#endif // PHOTOVIDEOWIDGET_H
