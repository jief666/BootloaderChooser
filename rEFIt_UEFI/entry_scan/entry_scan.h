/*
 * refit/scan/entry_scan.h
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

#include <Platform.h> // Only use angled for Platform, else, xcode project won't compile
#include "../gui/menu_items/menu_items.h"

extern REFIT_MENU_ITEM_RETURN MenuEntryReturn;
extern REFIT_MENU_ITEM_OPTIONS MenuEntryOptions;
extern REFIT_MENU_ITEM_ABOUT MenuEntryAbout;
extern REFIT_MENU_ITEM_RESET MenuEntryReset;
extern REFIT_MENU_ITEM_SHUTDOWN MenuEntryShutdown;
//extern REFIT_MENU_ENTRY MenuEntryHelp;
//extern REFIT_MENU_ENTRY MenuEntryExit;

//extern XObjArray<REFIT_VOLUME> Volumes;


//
//// Ask user for file path from directory menu
//BOOLEAN AskUserForFilePathFromDir(IN CHAR16 *Title OPTIONAL, IN REFIT_VOLUME *Volume,
//                                  IN CHAR16 *ParentPath, IN EFI_FILE *Dir,
//                                  OUT EFI_DEVICE_PATH_PROTOCOL **Result);
//// Ask user for file path from volumes menu
//BOOLEAN AskUserForFilePathFromVolumes(IN CHAR16 *Title OPTIONAL, OUT EFI_DEVICE_PATH_PROTOCOL **Result);
//// Ask user for file path
//BOOLEAN AskUserForFilePath(IN CHAR16 *Title OPTIONAL, IN EFI_DEVICE_PATH_PROTOCOL *Root OPTIONAL, OUT EFI_DEVICE_PATH_PROTOCOL **Result);

// loader
VOID ScanLoaders(VOID);


//// locked graphics
//CONST CHAR8 *CustomBootModeToStr(IN UINT8 Mode);
//EFI_STATUS LockBootScreen(VOID);
//EFI_STATUS UnlockBootScreen(VOID);

