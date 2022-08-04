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
#include "TrObject.h"

TrObject *TrObject::g_instance = nullptr;

TrObject::TrObject(QObject *parent)
    : QObject(parent)
{
}

void TrObject::initInstance(QObject *p)
{
    g_instance = new TrObject(p);
}

TrObject *TrObject::getInstance()
{
    return g_instance;
}

QString TrObject::getDlgTitle(const int &id)
{
    switch (id) {
    case dlg_app_title:
        return tr("Phone Assistant");
    case dlg_app_Description:
        return tr("Phone Assistant helps you manage the files in smart phones easily.");
    case dlg_ClosePopUpWidget:
        return tr("Please choose your action");
    case dlg_FileRepeatConfirmDlg:
        return tr("Another file with the same name already exists, replace it?");
    case dlg_Exporting:
        return tr("Exporting");
    case dlg_Importing:
        return tr("Importing");
    case dlg_Copying:
        return tr("Copying");
    }
    return m_strNoFind;
}

/**
 * @brief Dialog 按钮文案
 * @param id
 * @return
 */
QString TrObject::getDlgBtnText(const int &id)
{
    switch (id) {
    case Dlg_Btn_ok:
        return tr("OK");
    case Dlg_Btn_cancel:
        return tr("Cancel");
    case Dlg_Btn_Confirm:
        return tr("Confirm");
    case Dlg_Btn_Save:
        return tr("Save");
    case Dlg_Btn_Delete:
        return tr("Delete");
    case Dlg_Btn_Skip:
        return tr("Skip");
    case Dlg_Btn_Replace:
        return tr("Replace");
    case Dlg_Btn_Keep_both:
        return tr("Keep both");
    case Dlg_Btn_Exit:
        return tr("Exit");
    case Minimize_to_system_tray:
        return tr("Minimize to system tray");
    case Select_All:
        return tr("Select All");
    case Unselect_All:
        return tr("Unselect All");
    case New_Folder:
        return tr("New folder");
    case Btn_Export:
        return tr("Export");
    case Btn_Import:
        return tr("Import");
    }
    return m_strNoFind;
}

QString TrObject::getCheckBoxText(const int &id)
{
    switch (id) {
    case Do_not_ask_again:
        return tr("Do not ask again");
    case Apply_to_all:
        return tr("Apply to all");
    }
    return m_strNoFind;
}

QString TrObject::getListViewText(const int &id)
{
    switch (id) {
    case List_App:
        return tr("Apps");
    case List_Photo:
        return tr("Photos");
    case List_Video:
        return tr("Videos");
    case List_Song:
        return tr("Songs");
    case List_eBook:
        return tr("eBooks");
    case List_File:
        return tr("Files");
    }
    return m_strNoFind;
}

QString TrObject::getTreeHeaderText(const int &id)
{
    switch (id) {
    case Header_Name:
        return tr("Name");
    case Header_Size:
        return tr("Size");
    case Header_Version:
        return tr("Version");
    case Header_Action:
        return tr("Action");
    case Header_Time_modified:
        return tr("Time modified");
    case Header_Type:
        return tr("Type");
    case Header_Duration:
        return tr("Duration");
    case Header_Format:
        return tr("Format");
    case Tree_Directory:
        return tr("Directory");
    case Tree_File:
        return tr("File");
    case Tree_Unknown:
        return tr("Unknown");
    case Tree_New_folder:
        return tr("New folder");
    }
    return m_strNoFind;
}

QString TrObject::getMenuActionText(const int &id)
{
    switch (id) {
    case Menu_Copy:
        return tr("Copy");
    case Menu_Paste:
        return tr("Paste");
    case Menu_Refresh:
        return tr("Refresh");
    case Menu_Settings:
        return tr("Settings");
    }

    return m_strNoFind;
}

QString TrObject::getLabelText(const int &id)
{
    switch (id) {
    case Loading:
        return tr("Loading...");
    case Detecting:
        return tr("Detecting...");
    case ApkInstalling:
        return tr("Installing Phone Assistant on your phone, which takes 1-3 minutes, please wait patiently...");
    case No_device_connected:
        return tr("No device connected");
    case Connect_your_device_to_PC_with_USB_cables:
        return tr("Connect your device to PC with USB cables");
    case Connected_but_not_recognized_Click_below_buttons:
        return tr("Connected but not recognized? Click below buttons");
    case No_files:
        return tr("No files");
    case path_not_exist:
        return tr("The path does not exist");
    case ios_Occasion_1:
        return tr("Occasion 1. The USB cable or connector does not work");
    case ios_Occasion_1_1:
        return tr("Check the USB connection, especially whether the connector is dusty or dirty. Clean and reconnect, or try another USB cable.");
    case ios_Occasion_2:
        return tr("Occasion 2. iOS device locked and not authorized");
    case ios_Occasion_2_1:
        return tr("Keep your device unlocked, connect it to Phone Assistant again, and tap on \"Trust This Computer\" on the device.");
    case android_1:
        return tr("I. Change USB connection mode");
    case android_2:
        return tr("1. Swipe down the notification bar;");
    case android_3:
        return tr("2. Tap on \"Charging this device via USB\";");
    case android_4:
        return tr("3. Choose the File Transfer, MTP or PTP mode.");
    case android_5:
        return tr("II. Enable USB debugging");
    case android_6:
        return tr("1. Unlock your phone and open \"Settings\";");
    case android_7:
        return tr("2. Find \"About phone\" and tap on it;");
    case android_8:
        return tr("3. Tap on \"System version\" several times until it prompts \"Developer options enabled\";");
    case android_9:
        return tr("4. Back to \"Settings\" and tap on \"Developer options\";");
    case android_10:
        return tr("5. Find \"USB debugging\" and enable it.");
    }
    return m_strNoFind;
}

QString TrObject::getFileOperationText(const int &id)
{
    switch (id) {
    case Please_select_a_file:
        return tr("Please select a file");
    case Processing_data:
        return tr("Processing data, please wait...");
    case Delete_it:
        return tr("Are you sure you want to delete it?");
    case Delete_items:
        return tr("Are you sure you want to delete the items?");
    case Uninstall_App:
        return tr("Are you sure you want to uninstall this app?");
    case Uninstall_Apps:
        return tr("Are you sure you want to uninstall these apps?");
    case Deleting:
        return tr("Deleting:");
    case delete_failed:
        return tr("Reason: I/O error");
    case copy_failed:
        return tr("Copy failed:");
    case copy_failed_count:
        return tr("Copy failed: %1");
    case Deletion_failed:
        return tr("Deletion failed:");
    case Deletion_failed_count:
        return tr("Deletion failed: %1");
    case Total_item_selected:
        return tr("Total %1, %2 item(s) selected");
    case Total_item_selected_size:
        return tr("Total %1, %2 item(s) selected, %3");
    case items_size:
        return tr("%1 item(s), %2");
    case items_count:
        return tr("%1 item(s)");
    case file_copy:
        return tr("copy");
    }
    return m_strNoFind;
}

QString TrObject::getDeviceLabelText(const int &id)
{
    switch (id) {
    case Internal_Storage:
        return tr("Internal Storage");
    case device_Connected:
        return tr("Connected");
    case Unlock_your_phone_and_tap_Trust:
        return tr("Unlock your phone and tap Trust");
    case Connect_your_mobile_device:
        return tr("Connect your mobile device");
    case Connect_Now:
        return tr("Connect Now");
    case Enable_USB_debugging_on_your_phone:
        return tr("Enable USB debugging on your phone");
    case device_debug_1:
        return tr("I. Change USB connection mode");
    case device_debug_2:
        return tr("1. Swipe down the notification bar;\n"
                  "2. Tap on \"Charging this device via USB\";\n"
                  "3. Choose the File Transfer, MTP or PTP mode.");
    case device_debug_3:
        return tr("II. Enable USB debugging");
    case device_debug_4:
        return tr("1. Unlock your phone and open \"Settings\";\n"
                  "2. Find \"About phone\" and tap on it;\n"
                  "3. Tap on \"System version\" several times until it prompts \"Developer options enabled\";\n"
                  "4. Back to \"Settings\" and tap on \"Developer options\";\n"
                  "5. Find \"USB debugging\" and enable it.\n");
    case device_authorize:
        return tr("Tap \"Authorize\" on your device to connect. If no authorization prompt, please connect again.");
    case Used:
        return tr("Used");
    case AppInstallError:
        return tr("Failed to install Phone Assistant on your phone");
    case AppUninstallFailed:
        return tr("Failed to uninstall the app");
    case AppsUninstallFailed:
        return tr("Failed to uninstall %1 apps");
    }
    return m_strNoFind;
}

QString TrObject::getMountText(const int &id)
{
    switch (id) {
    case Failed_mount_the_directories:
        return tr("Failed to mount the directories");
    case ios_mount_create_failed:
        return tr("Failed to create the mounted directory:");
    case ios_mount_delete_failed:
        return tr("Failed to delete the mounted directory:");
    case System_Disk:
        return tr("System Disk");
    }
    return m_strNoFind;
}

QString TrObject::getMessageText(const int &id)
{
    switch (id) {
    case You_are_using_PTP_connection:
        return tr("You are using PTP connection, so cannot import anything");
    case Input_a_folder_name_please:
        return tr("Input a folder name please");
    case folder_name_illage:
        return tr("The folder name must not contain special characters (<>:'\"\\|/?)");
    case folder_name_dot_start:
        return tr("The folder name cannot start with a dot (.)");
    case folder_name_exist:
        return tr("The name \"%1\" is in use, please input another one");
    case folder_create_failed:
        return tr("Failed to create the folder: %1");
    case Export_stopped_one_file:
        return tr("Export stopped, 1 file exported");
    }
    return m_strNoFind;
}

QString TrObject::getFileSelectText(const int &id)
{
    switch (id) {
    case file_select_szie:
        return tr("%1 file(s) selected (%2)");
    case folder_select_contains_item_szie:
        return tr("1 folder selected (contains %1 item), %2");
    case folder_select_contains_items_szie:
        return tr("1 folder selected (contains %1 items), %2");
    case folders_selected_contains_items_size:
        return tr("%1 folders selected (contains %2 items), %3");
    }
    return m_strNoFind;
}

QString TrObject::getArrowPhoneInfoText(const int &id)
{
    switch (id) {
    case device_info:
        return tr("Device Info");
    case device_info_Brand:
        return tr("Brand:");
    case device_info_Model:
        return tr("Model:");
    case device_info_system_name:
        return tr("System name:");
    case device_info_system_version:
        return tr("System version:");
    case device_info_c_r:
        return tr("Country/Region:");
    case device_info_Storage:
        return tr("Storage:");
    case device_info_RAM:
        return tr("RAM:");
    }
    return m_strNoFind;
}

QString TrObject::getImportExportText(const int &id)
{
    switch (id) {
    case Importing:
        return tr("Importing:");
    case file_Import_success:
        return tr("%1 files imported");
    case file_Import_stopped:
        return tr("Import stopped, %1 files imported");
    case Import_failed:
        return tr("Import failed");
    case Import_failed_count:
        return tr("Import failed: %1");
    case Export_success:
        return tr("Export successful");
    case Export_success_count:
        return tr("%1 files exported");
    case file_Export_stopped:
        return tr("Export stopped, %1 files exported");
    //    case Export_failed:
    //        return tr("Export failed");
    case Export_failed_count:
        return tr("Export failed: %1");
    case app_export_count:
        return tr("%1 apps exported");
    }
    return m_strNoFind;
}

QString TrObject::getPhotoImportExportText(const int &id)
{
    switch (id) {
    case photo_export_success:
        return tr("Export successful");
    case photo_import_success:
        return tr("%1 photos imported");
    case photo_import_stop:
        return tr("Import stopped, %1 photos imported");
    case photo_export_stop_count:
        return tr("Export stopped, %1 photos exported");
    }
    return m_strNoFind;
}

QString TrObject::getVideoImportExportText(const int &id)
{
    switch (id) {
    case video_export_success:
        return tr("Export successful");
    case video_import_success:
        return tr("%1 videos imported");
    case video_import_stop:
        return tr("Import stopped, %1 videos imported");
    case video_export_stop_count:
        return tr("Export stopped, %1 videos exported");
    }
    return m_strNoFind;
}

QString TrObject::getMusicImportExportText(const int &id)
{
    switch (id) {
    case song_export_stop_count:
        return tr("Export stopped, %1 songs exported");
    case song_export_count:
        return tr("%1 songs exported");
    case song_import_count:
        return tr("%1 songs imported to %2 folder");
    case song_import_stop:
        return tr("Import stopped, %1 songs imported to %2 folder");
    }
    return m_strNoFind;
}

QString TrObject::getEBookImportExportText(const int &id)
{
    switch (id) {
    case eBooks_export_stop_count:
        return tr("Export stopped, %1 eBooks exported");
    case eBooks_export_count:
        return tr("%1 eBooks exported");
    case eBooks_import_count:
        return tr("%1 eBooks imported to %2 folder");
    case eBooks_import_stop:
        return tr("Import stopped, %1 eBooks imported to %2 folder");
    }
    return m_strNoFind;
}
