/*
 * refit/scan/loader.c
 *
 * Copyright (c) 2006-2010 Christoph Pfisterer
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *  * Neither the name of Christoph Pfisterer nor the names of the
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "loader.h"
#include "../cpp_foundation/XString.h"
#include "entry_scan.h"
#include "../Platform/Settings.h"
#include "../refit/screen.h"
#include "../refit/menu.h"
#include "common.h"
#include "../Platform/guid.h"
#include "../refit/lib.h"
#include "../gui/REFIT_MENU_SCREEN.h"
#include "../Settings/Self.h"

#ifndef DEBUG_ALL
#define DEBUG_SCAN_LOADER 1
#else
#define DEBUG_SCAN_LOADER DEBUG_ALL
#endif

#if DEBUG_SCAN_LOADER == 0
#define DBG(...)
#else
#define DBG(...) DebugLog(DEBUG_SCAN_LOADER, __VA_ARGS__)
#endif

const XStringW MACOSX_LOADER_PATH = L"\\System\\Library\\CoreServices\\boot.efi"_XSW;

const XStringW LINUX_ISSUE_PATH = L"\\etc\\issue"_XSW;
#define LINUX_BOOT_PATH L"\\boot"
#define LINUX_BOOT_ALT_PATH L"\\boot"
const XString8 LINUX_LOADER_PATH = "vmlinuz"_XS8;
const XStringW LINUX_FULL_LOADER_PATH = SWPrintf("%ls\\%s", LINUX_BOOT_PATH, LINUX_LOADER_PATH.c_str());
#define LINUX_LOADER_SEARCH_PATH L"vmlinuz*"
const XString8Array LINUX_DEFAULT_OPTIONS = Split<XString8Array>("ro add_efi_memmap quiet splash vt.handoff=7", " ");

#if defined(MDE_CPU_X64)
#define BOOT_LOADER_PATH L"\\EFI\\BOOT\\BOOTX64.efi"_XSW
#else
#define BOOT_LOADER_PATH L"\\EFI\\BOOT\\BOOTIA32.efi"_XSW
#endif


//extern LOADER_ENTRY *SubMenuKextInjectMgmt(LOADER_ENTRY *Entry);

// Linux loader path data
typedef struct LINUX_PATH_DATA
{
   CONST XStringW Path;
   CONST XStringW Title;
   CONST XStringW Icon;
   CONST XString8  Issue;
} LINUX_PATH_DATA;

typedef struct LINUX_ICON_DATA
{
   CONST CHAR16 *DirectoryName;
   CONST CHAR16 *IconName;
} LINUX_ICON_MAPPING;




STATIC LOADER_ENTRY *CreateLoaderEntry(IN CONST XStringW& LoaderPath,
                                       IN CONST XStringW& FullTitle,
                                       IN REFIT_VOLUME *Volume,
                                       IN CHAR16 Hotkey)
{
  EFI_DEVICE_PATH       *LoaderDevicePath;
  XStringW               LoaderDevicePathString;
  XStringW               FilePathAsString;
  LOADER_ENTRY          *Entry;

  // Check parameters are valid
  if ((LoaderPath.isEmpty()) || (Volume == NULL)) {
    return NULL;
  }

  // Get the loader device path
  LoaderDevicePath = FileDevicePath(Volume->DeviceHandle, LoaderPath);
  if (LoaderDevicePath == NULL) {
    return NULL;
  }
  LoaderDevicePathString = FileDevicePathToXStringW(LoaderDevicePath);
  if (LoaderDevicePathString.isEmpty()) {
    return NULL;
  }

  // Ignore this loader if it's self path
  XStringW selfDevicePathAsXStringW = FileDevicePathToXStringW(&self.getSelfDevicePath());
  if ( selfDevicePathAsXStringW == LoaderDevicePathString ) {
    DBG("     skipped because path `%ls` is self path!\n", LoaderDevicePathString.wc_str());
    return NULL;
  }

  Entry = new LOADER_ENTRY();

  Entry->Row = 0;
  Entry->Volume = Volume;

  Entry->LoaderPath       = LoaderPath;
  Entry->DisplayedVolName = Volume->VolName;
  Entry->DevicePath       = LoaderDevicePath;
  Entry->DevicePathString = LoaderDevicePathString;

  //actions
  Entry->AtClick = ActionSelect;
  Entry->AtDoubleClick = ActionEnter;
  Entry->AtRightClick = ActionDetails;
  Entry->BuildVersion.setEmpty();
  Entry->Title = FullTitle;
  Entry->ShortcutLetter = Hotkey;

  DBG("    Loader entry created for '%ls'\n", Entry->DevicePathString.wc_str());
  return Entry;
}

void LOADER_ENTRY::AddDefaultMenu()
{
  XStringW     FileName;

  FileName = LoaderPath.basename();

  // create the submenu
  SubScreen = new REFIT_MENU_SCREEN(this);
  SubScreen->Title.SWPrintf("Options for %ls on %ls", Title.wc_str(), DisplayedVolName.wc_str());

  SubScreen->AddMenuInfoLine_f("%ls", FileDevicePathToXStringW(DevicePath).wc_str());
//  SubScreen->AddMenuInfoLine_f("Options: %s", LoadOptions.ConcatAll(" "_XS8).c_str());
//  REFIT_MENU_SWITCH* itemMakeDefault = SubScreen->AddMenuItemSwitch(0, "Make default : ", false);
  REFIT_INPUT_DIALOG* itemMakeDefault = SubScreen->AddMenuItemInput("Default", false);
  itemMakeDefault->Item.ItemType = BoolValue;
  itemMakeDefault->Item.BValue = gSettings.DefaultVolume.isEqualIC(Title);


  SubScreen->AddMenuEntry(&MenuEntryReturn, false);
  // DBG("    Added '%ls': OSType='%d', OSVersion='%s'\n",Title,LoaderType,OSVersion);
}

LOADER_ENTRY* AddLoaderEntry(IN CONST XStringW& LoaderPath,
                       IN CONST XStringW& FullTitle,
                       IN REFIT_VOLUME *Volume)
{
  LOADER_ENTRY *Entry;

  if ((LoaderPath.isEmpty()) || (Volume == NULL) || (Volume->RootDir == NULL) || !FileExists(Volume->RootDir, LoaderPath)) {
    return NULL;
  }

//  DBG("        AddLoaderEntry for Volume Name=%ls\n", Volume->VolName.wc_str());

  Entry = CreateLoaderEntry(LoaderPath, FullTitle, Volume, 0);
  if ( Entry ) {
    // can be NULL if Entry si skipped
    Entry->AddDefaultMenu();
    MainMenu.AddMenuEntry(Entry, true);
  }
  return Entry;
}


VOID ScanLoaders(VOID)
{
  //DBG("Scanning loaders...\n");
  DbgHeader("ScanLoaders");
   
  for (UINTN VolumeIndex = 0; VolumeIndex < Volumes.size(); VolumeIndex++)
  {
    REFIT_VOLUME* Volume = &Volumes[VolumeIndex];
    if (Volume->RootDir == NULL) { // || Volume->VolName == NULL)
      //DBG(", no file system\n", VolumeIndex);
      continue;
    }

    DBG("- [%02llu]: '%ls'\n", VolumeIndex, Volume->VolName.wc_str());
    if (Volume->VolName.isEmpty()) {
      Volume->VolName = L"Unknown"_XSW;
    }


//    if (Volume->Hidden) {
//      DBG(", hidden\n");
//      continue;
//    }


    REFIT_DIR_ITER  rootFolderIter;
    DirIterOpen(Volume->RootDir, L"\\", &rootFolderIter);
    if ( EFI_ERROR(rootFolderIter.LastStatus) != EFI_SUCCESS ) {
      DBG("Error when opening root folder\n");
    }
    // Browsing root folder
    EFI_FILE_INFO  *rootFolderEntry = NULL;
    while (DirIterNext(&rootFolderIter, 1, L"*", &rootFolderEntry))
    {
      if (rootFolderEntry->FileName[0] == '.') {
        continue;
      }
      if ( !LStringW(rootFolderEntry->FileName).isEqualIC("EFI") ) {
        continue;
      }
//      DBG("subdirname = %ls\n", rootFolderEntry->FileName);

      // Browsing subDir
      REFIT_DIR_ITER  subFolderIter;
      DirIterOpen(Volume->RootDir, SWPrintf("\\%ls", rootFolderEntry->FileName).wc_str(), &subFolderIter);
      if ( EFI_ERROR(subFolderIter.LastStatus) != EFI_SUCCESS ) {
        DBG("Error when opening sub folder '%ls'\n", rootFolderEntry->FileName);
      }
      EFI_FILE_INFO  *subDirEntry = NULL;
      while (DirIterNext(&subFolderIter, 1, L"*", &subDirEntry))
      {
        if (subDirEntry->FileName[0] == '.') {
          continue;
        }
        if ( LStringW(subDirEntry->FileName).isEqualIC("BOOT" ) ) {
          continue;
        }
//        DBG("subsubdirname = %ls\n", subDirEntry->FileName);

        // Browsing  subsubDir
        REFIT_DIR_ITER  subsubDirIter;
        DirIterOpen(Volume->RootDir, SWPrintf("\\%ls\\%ls", rootFolderEntry->FileName, subDirEntry->FileName).wc_str(), &subsubDirIter);
        if ( EFI_ERROR(subsubDirIter.LastStatus) != EFI_SUCCESS ) {
          DBG("Error when opening \\%ls\\%ls folder\n", rootFolderEntry->FileName, subDirEntry->FileName);
        }
        EFI_FILE_INFO  *subsubDirIterEntry = NULL;
        while (DirIterNext(&subsubDirIter, 2, L"*.efi", &subsubDirIterEntry)) {
          if (subsubDirIterEntry->FileName[0] == '.') {
            continue;
          }
          LStringW filename = LStringW(subsubDirIterEntry->FileName);
          size_t filenameLength = filename.length();
//          DBG("filename = %ls\n", filename.wc_str());

          XStringW path = SWPrintf("\\%ls\\%ls\\%ls", rootFolderEntry->FileName, subDirEntry->FileName, subsubDirIterEntry->FileName);

          AddLoaderEntry(path, SWPrintf("[%ls] %ls", Volume->getVolLabelOrOSXVolumeNameOrVolName().wc_str(), path.wc_str()), Volume);

          if ( filename.rindexOf(".efi") == filenameLength-strlen(".efi") ) {
          }
        }
      }
    }
  }
}

