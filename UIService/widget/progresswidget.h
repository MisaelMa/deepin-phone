/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co., Ltd.
 *
 * Author:     zhangliangliang <zhangliangliang@uniontech.com>
 * Maintainer: zhangliangliang <zhangliangliang@uniontech.com>
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
#ifndef EXPORTPROGRESSDLG_H
#define EXPORTPROGRESSDLG_H

#include <DDialog>
#include <DProgressBar>
#include <DFloatingWidget>
#include <DLabel>
#include <DCheckBox>

DWIDGET_USE_NAMESPACE

//class ExportProgressDlg : public DDialog
//{
//    Q_OBJECT

//public:
//    explicit ExportProgressDlg(DWidget *parent = nullptr);

//    void setProgressBarRange(int minValue, int maxValue);
//    void updateProgressBarValue(int curValue);

//signals:
//    void sigCloseBtnClicked();

//protected:
//    void hideEvent(QHideEvent *event) Q_DECL_OVERRIDE;
//    virtual void resizeEvent(QResizeEvent *event) override;

//private:
//    DProgressBar *m_pExportProgressBar;
//};

class ImportProgressWgt : public DFloatingWidget
{
    Q_OBJECT
public:
    explicit ImportProgressWgt(DWidget *parent = nullptr);

    void updateProgressText(QString text);
    void setCloseBtnHidden(bool bHidden);

signals:
    void sigCloseBtnClicked();

private:
    DLabel *m_txtInfo;
    DWidget *m_pIconBtn;
};

class FileRepeatConfirmDlg : public DDialog
{
    Q_OBJECT

public:
    explicit FileRepeatConfirmDlg(QString fileName, QString srcPath, QString dstPath, bool hideCoexistBtn = false,
                                  DWidget *parent = nullptr, bool hideCheckBox = true);

    bool getAskCheckBoxValue() { return m_checkBox->isChecked(); }
    void hideCheckBox();

protected:
    // 屏蔽关闭事件
    void closeEvent(QCloseEvent *event) override;
    // 屏蔽esc关闭事件
    void keyPressEvent(QKeyEvent *event) override;

private:
    DLabel *m_labInfo;
    DLabel *m_labSrcPath;
    DLabel *m_labDstPath;
    DCheckBox *m_checkBox;
};

class ImportExportProgressDlg : public DDialog
{
    Q_OBJECT

public:
    explicit ImportExportProgressDlg(QString title, DWidget *parent = nullptr);

    void setProgressBarRange(int minValue, int maxValue);
    void updateProgressBarValue(int curValue);
    void setProgressBarFinish();

signals:
    void sigCloseBtnClicked();

protected:
    void hideEvent(QHideEvent *event) Q_DECL_OVERRIDE;
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    DProgressBar *m_pProgressBar;
};

#endif // EXPORTPROGRESSDLG_H
