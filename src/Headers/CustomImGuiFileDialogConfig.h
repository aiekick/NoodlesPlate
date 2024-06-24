// NoodlesPlate Copyright (C) 2017-2024 Stephane Cuillerdier aka Aiekick
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <ImGuiPack.h>

#define ICON_NDP_FILE_LIST u8"\uf0c9"
#define ICON_NDP_FILE_LIST_THUMBNAILS u8"\uf00b"
#define ICON_NDP_FILE_GRID_THUMBNAILS u8"\uf00a"
#define ICON_NDP_ADD u8"\uffb7"
#define ICON_NDP_REMOVE u8"\uffb6"
#define ICON_NDP_OK u8"\ufff6"
#define ICON_NDP_CANCEL u8"\ufff5"
#define ICON_NDP_RESET u8"\uffb8"
#define ICON_NDP_DRIVES u8"\uff3d"
#define ICON_NDP2_PENCIL u8"\uff01"
#define ICON_NDP_SEARCH u8"\ufff9"
#define ICON_NDP2_FOLDER u8"\uff3c"
#define ICON_NDP_LINK u8"\uffa5"
#define ICON_NDP2_FILE u8"\uff4e"
#define ICON_NDP_CHEVRON_DOWN u8"\uffb0"
#define ICON_NDP_CHEVRON_UP u8"\uffb1"
#define ICON_NDP_BOOKMARK u8"\uffe6"

// uncomment and modify defines under for customize ImGuiFileDialog

// #define MAX_FILE_DIALOG_NAME_BUFFER 1024
// #define MAX_PATH_BUFFER_SIZE 1024

// options of use std::filesystem isntead of dirent.h but need c++17
// #define USE_STD_FILESYSTEM

#define USE_THUMBNAILS
// the thumbnail generation use the stb_image and stb_resize lib who need to define the implementation
// btw if you already use them in your app, you can have compiler error due to "implemntation found in double"
// so uncomment these line for prevent the creation of implementation of these libs again
#define DONT_DEFINE_AGAIN__STB_IMAGE_IMPLEMENTATION
#define DONT_DEFINE_AGAIN__STB_IMAGE_RESIZE_IMPLEMENTATION
// #define IMGUI_RADIO_BUTTON RadioButton
// #define DisplayMode_ThumbailsList_ImageHeight 32.0f
#define DisplayMode_FilesList_ButtonString ICON_NDP_FILE_LIST
// #define DisplayMode_FilesList_ButtonHelp "File List"
#define DisplayMode_ThumbailsList_ButtonString ICON_NDP_FILE_LIST_THUMBNAILS
// #define DisplayMode_ThumbailsList_ButtonHelp "Thumbnails List"
#define DisplayMode_ThumbailsGrid_ButtonString ICON_NDP_FILE_GRID_THUMBNAILS
// #define DisplayMode_ThumbailsGrid_ButtonHelp "Thumbnails Grid"

// #define USE_EXPLORATION_BY_KEYS
//  this mapping by default is for GLFW but you can use another
//  Up key for explore to the top
#define IGFD_KEY_UP ImGuiKey_UpArrow  // GLFW_KEY_UP
// Down key for explore to the bottom
#define IGFD_KEY_DOWN ImGuiKey_DownArrow  // GLFW_KEY_DOWN
// Enter key for open directory
#define IGFD_KEY_ENTER ImGuiKey_Enter  // GLFW_KEY_ENTER
// BackSpace for comming back to the last directory
#define IGFD_KEY_BACKSPACE ImGuiKey_Backspace  // GLFW_KEY_BACKSPACE

// by ex you can quit the dialog by pressing the key excape
#define USE_DIALOG_EXIT_WITH_KEY
#define IGFD_EXIT_KEY ImGuiKey_Escape

// widget
// begin combo widget
#define IMGUI_BEGIN_COMBO ImGui::BeginContrastedCombo
// filter combobox width
// #define FILTER_COMBO_WIDTH 120.0f
// button widget use for compose path
#define IMGUI_PATH_BUTTON ImGui::ContrastedButton_For_Dialogs
// standar button
#define IMGUI_BUTTON ImGui::ContrastedButton_For_Dialogs

// locales string
#define createDirButtonString ICON_NDP_ADD
#define okButtonString ICON_NDP_OK " OK"
#define cancelButtonString ICON_NDP_CANCEL " Cancel"
#define resetButtonString ICON_NDP_RESET
#define drivesButtonString ICON_NDP_DRIVES
#define editPathButtonString ICON_NDP2_PENCIL
#define searchString ICON_NDP_SEARCH
#define dirEntryString ICON_NDP2_FOLDER
#define linkEntryString ICON_NDP_LINK
#define fileEntryString ICON_NDP2_FILE
// #define fileNameString "File Name : "
// #define buttonResetSearchString "Reset search"
// #define buttonDriveString "Drives"
// #define buttonEditPathString "Edit path\nYou can also right click on path buttons"
// #define buttonResetPathString "Reset to current directory"
// #define buttonCreateDirString "Create Directory"
// #define OverWriteDialogTitleString "The file Already Exist !"
// #define OverWriteDialogMessageString "Would you like to OverWrite it ?"
// #define OverWriteDialogConfirmButtonString "Confirm"
// #define OverWriteDialogCancelButtonString "Cancel"

// DateTimeFormat
// see strftime functionin <ctime> for customize
// "%Y/%m/%d %H:%M" give 2021:01:22 11:47
// "%Y/%m/%d %i:%M%p" give 2021:01:22 11:45PM
// #define DateTimeFormat "%Y/%m/%d %i:%M%p"

// theses icons will appear in table headers
#define USE_CUSTOM_SORTING_ICON
#define tableHeaderAscendingIcon ICON_NDP_CHEVRON_UP
#define tableHeaderDescendingIcon ICON_NDP_CHEVRON_DOWN
#define tableHeaderFileNameString " File name"
#define tableHeaderFileTypeString " Type"
#define tableHeaderFileSizeString " Size"
#define tableHeaderFileDateTimeString " Date"

#define USE_PLACES_FEATURE
// #define placesPaneWith 150.0f
// #define IMGUI_TOGGLE_BUTTON ToggleButton
#define placesButtonString ICON_NDP_BOOKMARK
// #define placesButtonHelpString "Places"
#define addPlaceButtonString ICON_NDP_ADD
#define removePlaceButtonString ICON_NDP_REMOVE

// a group for bookmarks will be added by default, but you can also create it yourself and many more
#define USE_PLACES_BOOKMARKS
// #define PLACE_BOOKMARKS_NAME "Bookmarks"
// #define PLACE_BOOKMARKS_DISPLAY_ORDER 0  // to the first

// a group for system devices (returned by IFileSystem), but you can also add yours
// #define USE_PLACES_DEVICES
// #define PLACE_DEVICES_NAME "Devices"
// #define PLACE_DEVICES_DISPLAY_ORDER 10  // to the end
