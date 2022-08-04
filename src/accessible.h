#ifndef ACCESSIBLE_H
#define ACCESSIBLE_H

#include "accessibledefine.h"

#include "widget/MainWindow.h"
#include "widget/MainWidget.h"
#include "widget/closepopupwidget.h"
//#include "widget/ImageItemWidget.h"
#include "widget/MainRightWidget.h"
#include "widget/progresswidget.h"
#include "widget/RightTitleWidget.h"
#include "widget/SettingDialog.h"
#include "widget/SpinnerWidget.h"
#include "widget/TitleWidget.h"

#include "phoneui/devicesearchwidget.h"
#include "phoneui/mainitemwidget.h"
#include "phoneui/mainitemwidget.h"
#include "phoneui/nodevicewidget.h"
#include "phoneui/PhoneInfoWidget.h"

#include "app/PhoneAppWidget.h"

#include "musicEbook/EBookWidget.h"
#include "musicEbook/MusicWidget.h"

#include "ImageVideo/PhotoTreeFrame.h"
#include "ImageVideo/PhotoVideoWidget.h"

#include "file/FileManageWidget.h"

#include <DSwitchButton>
#include <DBackgroundGroup>
#include <DFloatingButton>
#include <DLineEdit>
#include <DLabel>
#include <DListView>
#include <DCommandLinkButton>
#include <DSearchEdit>
#include <DTitlebar>
#include <DComboBox>
#include <DCheckBox>
#include <DTreeView>
#include <DIconButton>
#include <DToolButton>
#include <DProgressBar>
#include <DTextEdit>
#include <DDialog>
#include <DFileDialog>

DWIDGET_USE_NAMESPACE
//using namespace DCC_NAMESPACE;

SET_FORM_ACCESSIBLE(MainWindow, "MainWindow")
SET_FORM_ACCESSIBLE(MainWidget, "MainWidget")
SET_FORM_ACCESSIBLE(ClosePopUpWidget, "ClosePopUpWidget")
SET_FORM_ACCESSIBLE(DeviceSearchWidget, "DeviceSearchWidget")
//SET_FORM_ACCESSIBLE(ImageItemWidget, "ImageItemWidget")
SET_FORM_ACCESSIBLE(MainRightWidget, "MainRightWidget")
//SET_FORM_ACCESSIBLE(ExportProgressDlg, "ExportProgressDlg")
SET_FORM_ACCESSIBLE(ImportProgressWgt, "ImportProgressWgt")
SET_FORM_ACCESSIBLE(FileRepeatConfirmDlg, "FileRepeatConfirmDlg")
SET_FORM_ACCESSIBLE(SettingDialog, "SettingDialog")
SET_FORM_ACCESSIBLE(SpinnerWidget, "SpinnerWidget")
SET_FORM_ACCESSIBLE(TitleWidget, "TitleWidget")
SET_FORM_ACCESSIBLE(EBookWidget, "EBookWidget")
SET_FORM_ACCESSIBLE(FileManageWidget, "FileManageWidget")
SET_FORM_ACCESSIBLE(BaseThemeWgt, "BaseThemeWgt")
SET_FORM_ACCESSIBLE(PhoneInfoWidget, "PhoneInfoWidget")
SET_FORM_ACCESSIBLE(UnlockWgt, "UnlockWgt")
SET_FORM_ACCESSIBLE(DebugModeWidget, "DebugModeWidget")
SET_FORM_ACCESSIBLE(UsbAuthorizeWidget, "UsbAuthorizeWidget")
SET_FORM_ACCESSIBLE(ApkInstallErrorWidget, "ApkInstallErrorWidget")
SET_FORM_ACCESSIBLE(DisconnWgt, "DisconnWgt")
SET_FORM_ACCESSIBLE(BatteryWgt, "BatteryWgt")
SET_FORM_ACCESSIBLE(ArrowPhoneInfo, "ArrowPhoneInfo")
SET_FORM_ACCESSIBLE(MusicWidget, "MusicWidget")
SET_FORM_ACCESSIBLE(NoDeviceWidget, "NoDeviceWidget")
SET_FORM_ACCESSIBLE(PhoneAppWidget, "PhoneAppWidget")
SET_FORM_ACCESSIBLE(PhotoTreeFrame, "PhotoTreeFrame")
SET_FORM_ACCESSIBLE(PhotoVideoWidget, "PhotoVideoWidget")

SET_FORM_ACCESSIBLE(RightTitleWidget, "RightTitleWidget")

SET_BUTTON_ACCESSIBLE(VIconBtn, "VIconBtn")

// Qt控件
SET_FORM_ACCESSIBLE(QFrame, m_w->objectName().isEmpty() ? "frame" : m_w->objectName())
SET_FORM_ACCESSIBLE(QWidget, m_w->objectName().isEmpty() ? "widget" : m_w->objectName())
SET_BUTTON_ACCESSIBLE(QPushButton, m_w->text().isEmpty() ? "qpushbutton" : m_w->text())
SET_SLIDER_ACCESSIBLE(QSlider, "qslider")
SET_FORM_ACCESSIBLE(QMenu, "qmenu")
//SET_LABEL_ACCESSIBLE(QLabel, m_w->text().isEmpty() ? "qlabel" : m_w->text())

// Dtk控件
SET_FORM_ACCESSIBLE(DFrame, m_w->objectName().isEmpty() ? "frame" : m_w->objectName())
SET_FORM_ACCESSIBLE(DWidget, m_w->objectName().isEmpty() ? "widget" : m_w->objectName())
SET_FORM_ACCESSIBLE(DBackgroundGroup, m_w->objectName().isEmpty() ? "dbackgroundgroup" : m_w->objectName())
SET_BUTTON_ACCESSIBLE(DSwitchButton, m_w->text().isEmpty() ? "switchbutton" : m_w->text())
SET_BUTTON_ACCESSIBLE(DFloatingButton, m_w->toolTip().isEmpty() ? "DFloatingButton" : m_w->toolTip())
SET_FORM_ACCESSIBLE(DSearchEdit, m_w->objectName().isEmpty() ? "DSearchEdit" : m_w->objectName())
SET_BUTTON_ACCESSIBLE(DPushButton, m_w->objectName().isEmpty() ? "DPushButton" : m_w->objectName())
SET_BUTTON_ACCESSIBLE(DIconButton, m_w->objectName().isEmpty() ? "DIconButton" : m_w->objectName())
SET_BUTTON_ACCESSIBLE(DCheckBox, m_w->objectName().isEmpty() ? "DCheckBox" : m_w->objectName())
SET_BUTTON_ACCESSIBLE(DCommandLinkButton, "DCommandLinkButton")
SET_FORM_ACCESSIBLE(DTitlebar, m_w->objectName().isEmpty() ? "DTitlebar" : m_w->objectName())
//SET_LABEL_ACCESSIBLE(DLabel, m_w->text().isEmpty() ? "DLabel" : m_w->text())
SET_BUTTON_ACCESSIBLE(DToolButton, m_w->objectName().isEmpty() ? "DToolButton" : m_w->objectName())
SET_FORM_ACCESSIBLE(DDialog, m_w->objectName().isEmpty() ? "DDialog" : m_w->objectName())
SET_FORM_ACCESSIBLE(DFileDialog, m_w->objectName().isEmpty() ? "DFileDialog" : m_w->objectName())

QAccessibleInterface *accessibleFactory(const QString &classname, QObject *object)
{
    QAccessibleInterface *interface = nullptr;
    // 应用主窗口
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), MainWindow);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), MainWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), ClosePopUpWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), DeviceSearchWidget);
    //    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), ImageItemWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), MainRightWidget);
    //    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), ExportProgressDlg);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), ImportProgressWgt);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), FileRepeatConfirmDlg);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), RightTitleWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), SettingDialog);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), SpinnerWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), TitleWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), EBookWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), FileManageWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), BaseThemeWgt);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), PhoneInfoWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), UnlockWgt);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), DebugModeWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), UsbAuthorizeWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), ApkInstallErrorWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), DisconnWgt);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), BatteryWgt);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), ArrowPhoneInfo);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), MusicWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), NoDeviceWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), PhoneAppWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), PhotoTreeFrame);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), PhotoVideoWidget);

    //  自定义控件
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), VIconBtn);

    //  Qt 控件
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), QFrame);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), QWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), QPushButton);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), QSlider);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), QMenu);
    //    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), QLabel);

    //  dtk 控件
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), DFrame);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), DWidget);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), DBackgroundGroup);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), DSwitchButton);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), DFloatingButton);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), DSearchEdit);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), DPushButton);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), DIconButton);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), DCheckBox);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), DCommandLinkButton);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), DTitlebar);
    //    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), DLabel);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), DDialog);
    USE_ACCESSIBLE(QString(classname).replace("dccV20::", ""), DFileDialog);

    return interface;
}

#endif // ACCESSIBLE_H
