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
#ifndef TROBJECT_H
#define TROBJECT_H

#include <QObject>

/**
 * @brief 文案翻译类
 */

enum E_Dlg_Title {
    dlg_app_title,
    dlg_app_Description,
    dlg_ClosePopUpWidget,
    dlg_FileRepeatConfirmDlg,
    dlg_Exporting,
    dlg_Importing,
    dlg_Copying
};

enum E_Dlg_Btn {
    Dlg_Btn_ok,
    Dlg_Btn_cancel,
    Dlg_Btn_Confirm,
    Dlg_Btn_Save,
    Dlg_Btn_Delete,
    Dlg_Btn_Skip,
    Dlg_Btn_Replace,
    Dlg_Btn_Keep_both,
    Dlg_Btn_Exit,
    Minimize_to_system_tray,
    Select_All,
    Unselect_All,
    New_Folder,
    Btn_Export,
    Btn_Import
};

enum E_Check_Box {
    Do_not_ask_again,
    Apply_to_all
};

enum E_List_Text {
    List_App,
    List_Photo,
    List_Video,
    List_Song,
    List_eBook,
    List_File
};

enum E_Tree_Type {
    Header_Name,
    Header_Size,
    Header_Version,
    Header_Action,
    Header_Time_modified,
    Header_Type,
    Header_Duration,
    Header_Format,
    Tree_Directory,
    Tree_File,
    Tree_Unknown,
    Tree_New_folder
};

enum E_Menu_Action {
    Menu_Copy,
    Menu_Paste,
    Menu_Refresh,
    Menu_Settings
};

enum E_Label_Text {
    Loading,
    Detecting,
    ApkInstalling,
    No_device_connected,
    Connect_your_device_to_PC_with_USB_cables,
    Connected_but_not_recognized_Click_below_buttons,
    No_files,
    path_not_exist,
    ios_Occasion_1,
    ios_Occasion_1_1,
    ios_Occasion_2,
    ios_Occasion_2_1,
    android_1,
    android_2,
    android_3,
    android_4,
    android_5,
    android_6,
    android_7,
    android_8,
    android_9,
    android_10
};

enum E_Device_Label_Text {
    Internal_Storage,
    device_Connected,
    Unlock_your_phone_and_tap_Trust,
    Connect_your_mobile_device,
    Connect_Now,
    Enable_USB_debugging_on_your_phone,
    device_debug_1,
    device_debug_2,
    device_debug_3,
    device_debug_4,
    device_authorize,
    Used,
    AppInstallError,
    AppUninstallFailed,
    AppsUninstallFailed
};

enum E_ArrowPhoneInfo_Type {
    device_info,
    device_info_Brand,
    device_info_Model,
    device_info_system_name,
    device_info_system_version,
    device_info_c_r,
    device_info_Storage,
    device_info_RAM
};

enum E_File_Operation {
    Please_select_a_file,
    Processing_data,
    Delete_it,
    Delete_items,
    Uninstall_App,
    Uninstall_Apps,
    Deleting,
    delete_failed,
    copy_failed,
    copy_failed_count,
    Deletion_failed,
    Deletion_failed_count,
    Total_item_selected,
    Total_item_selected_size,
    items_size,
    items_count,
    file_copy
};

enum E_ImportExport_Operation {
    Importing,
    file_Import_success,
    file_Import_stopped,
    Import_failed,
    Import_failed_count,
    Export_success,
    file_Export_stopped,
    Export_success_count,
    Export_failed_count,
    app_export_count,
};

/**
 * @brief 电子书的文案
 */
enum E_eBook_ImportExport_Operation {
    eBooks_export_stop_count,
    eBooks_export_count,
    eBooks_import_count,
    eBooks_import_stop,
};

/**
 * @brief 音乐的文案
 */
enum E_Music_ImportExport_Operation {
    song_export_stop_count,
    song_export_count,
    song_import_count,
    song_import_stop,
};

/**
 * @brief 图片的文案
 */
enum E_Photo_ImportExport_Operation {
    photo_export_stop_count,
    photo_export_success,
    photo_import_success,
    photo_import_stop,
};

/**
 * @brief 图片的文案
 */
enum E_Video_ImportExport_Operation {
    video_export_stop_count,
    video_export_success,
    video_import_success,
    video_import_stop,
};

enum E_Message_type {
    You_are_using_PTP_connection,
    Input_a_folder_name_please,
    folder_name_illage,
    folder_name_exist,
    folder_create_failed,
    Export_stopped_one_file,
    folder_name_dot_start
};

enum E_Mount_Type {
    Failed_mount_the_directories,
    ios_mount_create_failed,
    ios_mount_delete_failed,
    System_Disk
};

enum E_File_Select_Type {
    file_select_szie,
    folder_select_contains_item_szie,
    folder_select_contains_items_szie,
    folders_selected_contains_items_size
};

class TrObject : public QObject
{
    Q_OBJECT
public:
    explicit TrObject(QObject *parent = nullptr);

public:
    static void initInstance(QObject *p);
    static TrObject *getInstance();

public:
    QString getDlgTitle(const int &);
    QString getDlgBtnText(const int &);
    QString getCheckBoxText(const int &);
    QString getListViewText(const int &);
    QString getTreeHeaderText(const int &);
    QString getMenuActionText(const int &);
    QString getLabelText(const int &);
    QString getFileOperationText(const int &);
    QString getDeviceLabelText(const int &);
    QString getMountText(const int &);
    QString getMessageText(const int &);
    QString getFileSelectText(const int &);
    QString getArrowPhoneInfoText(const int &);
    QString getImportExportText(const int &);
    QString getPhotoImportExportText(const int &);
    QString getVideoImportExportText(const int &);
    QString getMusicImportExportText(const int &);
    QString getEBookImportExportText(const int &);

private:
    static TrObject *g_instance;
    QString m_strNoFind = "No Define";
};

#endif // TROBJECT_H
