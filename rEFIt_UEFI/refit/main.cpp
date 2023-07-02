/*
 * refit/main.c
 * Main code for the boot menu
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
#include "../cpp_foundation/XString.h"
#include "../cpp_util/globals_ctor.h"
#include "../cpp_util/globals_dtor.h"

#include "../entry_scan/entry_scan.h"
#include "../gui/menu_items/menu_globals.h"
#include "menu.h"
#include "../Platform/Settings.h"
#include "../Platform/Events.h"
#include "screen.h"
#include "../entry_scan/common.h"
#include "../Platform/guid.h"
#include "../Platform/CloverVersion.h"
#include "../gui/REFIT_MENU_SCREEN.h"
#include "../Settings/Self.h"

#ifndef DEBUG_ALL
#define DEBUG_MAIN 1
#else
#define DEBUG_MAIN DEBUG_ALL
#endif

#if DEBUG_MAIN == 0
#define DBG(...)
#else
#define DBG(...) DebugLog(DEBUG_MAIN, __VA_ARGS__)
#endif




//static EFI_STATUS StartEFILoadedImage(IN EFI_HANDLE ChildImageHandle,
//                                    IN CONST XString8Array& LoadOptions, IN CONST CHAR16 *LoadOptionsPrefix,
//                                    IN CONST XStringW& ImageTitle,
//                                    OUT UINTN *ErrorInStep)
//{
//  EFI_STATUS                  Status, ReturnStatus;
//  EFI_LOADED_IMAGE_PROTOCOL   *ChildLoadedImage;
//  CHAR16                      ErrorInfo[256];
////  CHAR16                  *FullLoadOptions = NULL;
//  XStringW loadOptionsW; // This has to be declared here, so it's not be freed before calling StartImage
//
////  DBG("Starting %ls\n", ImageTitle);
//  if (ErrorInStep != NULL) {
//    *ErrorInStep = 0;
//  }
//  ReturnStatus = Status = EFI_NOT_FOUND;  // in case no image handle was specified
//  if (ChildImageHandle == NULL) {
//    if (ErrorInStep != NULL) *ErrorInStep = 1;
//    goto bailout;
//  }
//
//  // set load options
//  if (!LoadOptions.isEmpty()) {
//    ReturnStatus = Status = gBS->HandleProtocol(ChildImageHandle, &gEfiLoadedImageProtocolGuid, (VOID **) &ChildLoadedImage);
//    if (CheckError(Status, L"while getting a LoadedImageProtocol handle")) {
//      if (ErrorInStep != NULL)
//        *ErrorInStep = 2;
//      goto bailout_unload;
//    }
//
//    if (LoadOptionsPrefix != NULL) {
//      // NOTE: That last space is also added by the EFI shell and seems to be significant
//      //  when passing options to Apple's boot.efi...
//      loadOptionsW = SWPrintf("%ls %s ", LoadOptionsPrefix, LoadOptions.ConcatAll(" "_XS8).c_str());
//    }else{
//      loadOptionsW = SWPrintf("%s ", LoadOptions.ConcatAll(" "_XS8).c_str()); // Jief : should we add a space ? Wasn't the case before big refactoring. Yes, a space required.
//    }
//    // NOTE: We also include the terminating null in the length for safety.
//    ChildLoadedImage->LoadOptionsSize = (UINT32)loadOptionsW.sizeInBytes() + sizeof(wchar_t);
//    ChildLoadedImage->LoadOptions = loadOptionsW.wc_str(); //will it be deleted after the procedure exit? Yes, if we don't copy loadOptionsW, so it'll be freed at the end of method
//    //((UINT32)StrLen(LoadOptions) + 1) * sizeof(CHAR16);
//    DBG("start image '%ls'\n", ImageTitle.s());
//    DBG("Using load options '%ls'\n", (CHAR16*)ChildLoadedImage->LoadOptions);
//
//  }
//  //DBG("Image loaded at: %p\n", ChildLoadedImage->ImageBase);
//  //PauseForKey(L"continue");
//
//  // close open file handles
//  UninitRefitLib();
//
//  // turn control over to the image
//  //
//  // Before calling the image, enable the Watchdog Timer for
//  // the 5 Minute period - Slice - NO! For slow driver and slow disk we need more
//  //
//  gBS->SetWatchdogTimer (600, 0x0000, 0x00, NULL);
//
//  ReturnStatus = Status = gBS->StartImage(ChildImageHandle, NULL, NULL);
//  //
//  // Clear the Watchdog Timer after the image returns
//  //
//  gBS->SetWatchdogTimer (0x0000, 0x0000, 0x0000, NULL);
//
//  //PauseForKey(L"Returned from StartImage\n");
//
//  // control returns here when the child image calls Exit()
//  if (ImageTitle.notEmpty()) {
//	  snwprintf(ErrorInfo, 512, "returned from %ls", ImageTitle.s());
//  }
//
//  if (CheckError(Status, ErrorInfo)) {
//    if (ErrorInStep != NULL)
//      *ErrorInStep = 3;
//  }
//  if (!EFI_ERROR(ReturnStatus)) { //why unload driver?!
//    goto bailout;
//  }
//
//bailout_unload:
//  // unload the image, we don't care if it works or not...
//  Status = gBS->UnloadImage(ChildImageHandle);
//bailout:
//  return ReturnStatus;
//}


EFI_GUID gOcBootstrapProtocolGuid = { 0xBA1EB455, 0xB182, 0x4F14, { 0x85, 0x21, 0xE4, 0x22, 0xC3, 0x25, 0xDE, 0xF6 }};

///
/// Forward declaration of OC_BOOTSTRAP_PROTOCOL structure.
///
typedef struct OC_BOOTSTRAP_PROTOCOL_ OC_BOOTSTRAP_PROTOCOL;

/**
  Restart OpenCore at specified file system.

  @param[in] This           This protocol.
  @param[in] FileSystem     File system to bootstrap in.
  @param[in] LoadPath       EFI device path to loaded image.

  @retval EFI_ALREADY_STARTED if already started.
  @retval Does not return on success.
**/
typedef
EFI_STATUS
(EFIAPI *OC_BOOTSTRAP_RERUN) (
  IN OC_BOOTSTRAP_PROTOCOL            *This,
  const EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *FileSystem,
  const EFI_DEVICE_PATH_PROTOCOL         *LoadPath  OPTIONAL
  );

///
/// The structure exposed by the OC_BOOTSTRAP_PROTOCOL.
///
struct OC_BOOTSTRAP_PROTOCOL_ {
  UINTN               Revision;
  UINTN               NestedCount;
  void   *VaultKey;
  OC_BOOTSTRAP_RERUN  ReRun;
  void  *GetLoadHandle;
};




//
// Null ConOut OutputString() implementation - for blocking
// text output from boot.efi when booting in graphics mode
//
EFI_STATUS EFIAPI
NullConOutOutputString(IN EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *This, IN CONST CHAR16 *String) {
  (void)This;
  (void)String;
  return EFI_SUCCESS;
}

bool dirNameEqual(const XStringW& path, const XStringW& dirname)
{
  if ( !path.startWith(dirname) ) return false;
  size_t pos = path.rindexOf('\\', SIZE_T_MAX-1);
  if ( pos > dirname.length() ) return false;
  return true;
}

void uninstallOcBootstrapProtocol()
{
  EFI_STATUS Status;

// Unload gOcBootstrapProtocolGuid. OpenCore should do it....
  UINTN HandleCount = 0;
  EFI_HANDLE *HandleBuffer = NULL;
  Status = gBS->LocateHandleBuffer (ByProtocol, &gOcBootstrapProtocolGuid, NULL, &HandleCount, &HandleBuffer);
  if (!EFI_ERROR(Status))
  {
    for (UINTN HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++)
    {
      EFI_HANDLE Interface = NULL;
      Status = gBS->HandleProtocol (HandleBuffer[HandleIndex], &gOcBootstrapProtocolGuid, &Interface);
      if (!EFI_ERROR(Status))
      {
        Status = gBS->UninstallProtocolInterface (HandleBuffer[HandleIndex], &gOcBootstrapProtocolGuid, Interface);
        if (EFI_ERROR(Status))
        {
          MsgLog("Cannot uninstall OcBootstrap Protocol protocol. Status=%s\n", efiStrError (Status));
          MsgLog("Reboot most likely needed\n");
        }
      }
      else
      {
        if (Status != EFI_UNSUPPORTED)
        {
          MsgLog("HandleProtocol for OcBootstrap Protocol failed. Status=%s\n", efiStrError (Status));
          MsgLog("Reboot most likely needed\n");
        }
      }
    }
  }
  else
  {
    DBG("LocateHandleBuffer  status=%s\n", efiStrError (Status));
// Assume no protocol AptioMemoryFix is installed.
  }
}

/*
 *  Previous that use Bootstrap.ReRun
 */
//EFI_STATUS LOADER_ENTRY::StartLoader()
//{
//  EFI_STATUS              Status;
//  XBuffer<UINT8>          ImageBuffer;
//
////  DBG("StartLoader() start\n");
//  DbgHeader("StartLoader");
//
//  XStringW devicePathString = DevicePathToXStringW(DevicePath);
//
//// Load image into memory (will be started later)
//  Status = egLoadFile(&self.getSelfVolumeRootDir(), LoaderPath.wc_str(), &ImageBuffer);
//  if ( EFI_ERROR(Status) ) {
//    MsgLog("BS: Failed to locate valid OpenCore image!\n");
//    return EFI_NOT_FOUND;
//  }
//
//  INTN offsetCloverRev = FindMem(ImageBuffer.vdata(), ImageBuffer.size(), "Clover revision: ", strlen("Clover revision: "));
//  if ( offsetCloverRev != -1 ) {
//    // It is Clover
//    offsetCloverRev += strlen("Clover revision: ");
//    XString8 revision = LString8(ImageBuffer.CData(offsetCloverRev));
//    DBG("Found clover revision '%s'\n", revision.c_str());
//    INTN offsetPathIndependant = FindMem(ImageBuffer.vdata(), ImageBuffer.size(), "path_independant", strlen("path_independant"));
//    if ( offsetPathIndependant == -1 ) {
//      //It's an old Clover. Can only start in /EFI/CLOVER.
//      if ( !dirNameEqual(LoaderPath, L"\\EFI\\CLOVER\\"_XSW) ) {
//        DBG("This Clover cannot start outside \\EFI\\CLOVER\\\n");
//        return EFI_UNSUPPORTED;
//      }
//    }
//    // Load image
//    EFI_HANDLE ImageHandle = NULL;
//    Status = gBS->LoadImage (FALSE, self.getSelfImageHandle(), DevicePath, ImageBuffer.vdata(), ImageBuffer.size(), &ImageHandle);
//    if ( EFI_ERROR (Status) ) {
//      MsgLog("Failed to load Clover image - %s\n", efiStrError(Status));
//      return Status;
//    }
//    DBG("StartImage %ls, status=%s\n", devicePathString.wc_str(), efiStrError(Status));
//    DBG("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
//    if (GlobalConfig.SavePreBootLog) Status = SaveBooterLog(&self.getCloverDir(), PREBOOT_LOG);
////      UninitRefitLib(); // close open file handles
//    // Start image
//    Status = gBS->StartImage (ImageHandle, NULL, NULL);
//    if ( EFI_ERROR(Status) ) {
//      MsgLog("Failed to start Clover image - %s\n", efiStrError(Status));
//      gBS->UnloadImage (ImageHandle);
//      return Status;
//    }
////      ReinitRefitLib();
//    gBS->UnloadImage (ImageHandle);
//    return EFI_SUCCESS;
//  }else{
//    // It is OpenCore
//    INTN offsetpenCore = FindMem(ImageBuffer.vdata(), ImageBuffer.size(), "OpenCore Bootloader (c) Acidanthera Research", strlen("OpenCore Bootloader (c) Acidanthera Research"));
//    if ( offsetpenCore != -1 ) {
//      // it's OpenCore
//
//      // Load image
//      EFI_HANDLE ImageHandle = NULL;
//      Status = gBS->LoadImage (FALSE, self.getSelfImageHandle(), NULL, ImageBuffer.vdata(), ImageBuffer.size(), &ImageHandle);
//      if ( EFI_ERROR (Status) ) {
//        MsgLog("BS: Failed to load OpenCore image - %s\n", efiStrError(Status));
//        return Status;
//      }
//      // Start image
//      Status = gBS->StartImage (ImageHandle, NULL, NULL);
//      if ( EFI_ERROR(Status) ) {
//        MsgLog("BS: Failed to start OpenCore image - %s\n", efiStrError(Status));
//        gBS->UnloadImage (ImageHandle);
//      }
//      OC_BOOTSTRAP_PROTOCOL* OcBootstrap = NULL;
//      Status = gBS->LocateProtocol(&gOcBootstrapProtocolGuid, NULL, (void**)&OcBootstrap);
//      if ( EFI_ERROR (Status) ) {
//        MsgLog("BS: Failed to locate bootstrap protocol - %s\n", efiStrError(Status));
//        gBS->UnloadImage (ImageHandle);
//        return Status;
//      }
//
//      if ( OcBootstrap->Revision <= 6 ) {
//        // this OC cannot boot outside /EFI/OC
//        if ( !dirNameEqual(LoaderPath, L"\\EFI\\OC\\"_XSW) ) {
//          MsgLog("This OpenCore version cannot start outside \\EFI\\OC\\\n");
//
//          gBS->UnloadImage (ImageHandle);
//
//          // Unload gOcBootstrapProtocolGuid. OpenCore should do it....
//          uninstallOcBootstrapProtocol();
//
//          // ToDO message
//          return EFI_UNSUPPORTED;
//        }
//      }
//      DBG("OcBootstrap->ReRun %ls, status=%s\n", devicePathString.wc_str(), efiStrError(Status));
//      DBG("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
//      if (GlobalConfig.SavePreBootLog) Status = SaveBooterLog(&self.getCloverDir(), PREBOOT_LOG);
////      UninitRefitLib(); // close open file handles
//
//      Status = OcBootstrap->ReRun(OcBootstrap, &self.getSelfSimpleVolume(), &self.getSelfDevicePath());
//      if ( EFI_ERROR(Status) ) {
//        MsgLog("OcBootstrap->ReRun failed - %s\n", efiStrError(Status));
//        gBS->UnloadImage (ImageHandle);
//        return Status;
//      }
////      ReinitRefitLib();
//      uninstallOcBootstrapProtocol();
//      gBS->UnloadImage (ImageHandle);
//      return EFI_SUCCESS;
//    }
//    else
//    {
//      // It's something else than Clover and OpenCore. Let's try anyway ?... Not yet.
//      MsgLog("Can only start Clover and OC\n");
//      return EFI_UNSUPPORTED;
//    }
//  }
//}

static bool rebootNeededBeforeNextStartImage = false;
/*
 * StartLoader() Version for OC that contains path_independant
 */
EFI_STATUS LOADER_ENTRY::StartLoader()
{
  EFI_STATUS              Status;
  XBuffer<UINT8>          ImageBuffer;

//  DBG("StartLoader() start\n");
  DbgHeader("StartLoader");

  XStringW devicePathString = FileDevicePathToXStringW(DevicePath);
  
  DBG("Start image %ls\n", devicePathString.wc_str());

  gSettings.DefaultVolume = devicePathString;
//  gSettings.GUITimeOut = 0;
  savePreferencesFile();

// Load image into memory (will be started later)
  Status = egLoadFile(this->Volume->RootDir, LoaderPath.wc_str(), &ImageBuffer);
  if ( EFI_ERROR(Status) ) {
    MsgLog("Failed to load the file '%ls'. %s.\n", LoaderPath.wc_str(), efiStrError(Status));
    return EFI_NOT_FOUND;
  }

  INTN offsetPathIndependant = FindMem(ImageBuffer.vdata(), ImageBuffer.size(), "path_independant", strlen("path_independant"));
  if ( offsetPathIndependant == -1 ) {
    // This a previous version of OC or Clover
    INTN offsetCloverRev = FindMem(ImageBuffer.vdata(), ImageBuffer.size(), "Clover revision: ", strlen("Clover revision: "));
    if ( offsetCloverRev != -1 ) {
      // It is an old Clover
      offsetCloverRev += strlen("Clover revision: ");
      XString8 revision = LString8(ImageBuffer.CData(offsetCloverRev));
      DBG("Found clover revision '%s'\n", revision.c_str());

      if ( !dirNameEqual(LoaderPath, L"\\EFI\\CLOVER\\"_XSW) ) {
        MsgLog("This Clover cannot start outside \\EFI\\CLOVER\\\n");
        return EFI_UNSUPPORTED;
      }
    }else{
      INTN offsetpenCore = FindMem(ImageBuffer.vdata(), ImageBuffer.size(), "OpenCore Bootloader (c) Acidanthera Research", strlen("OpenCore Bootloader (c) Acidanthera Research"));
      if ( offsetpenCore != -1 ) {
        // It's an old OpenCore
        if ( !dirNameEqual(LoaderPath, L"\\EFI\\OC\\"_XSW) ) {
          MsgLog("This OpenCore version cannot start outside \\EFI\\OC\\\n");
          return EFI_UNSUPPORTED;
        }
      }else{
        // An other image. Linux, Windows ? Let's try.
      }
    }
  }
  if ( rebootNeededBeforeNextStartImage ) {
    DBG("You need to reboot to be able to start this\n");
    PauseForKey("You need to reboot to be able to start this\n"_XS8);
    // Attempt warm reboot
    gRT->ResetSystem(EfiResetWarm, EFI_SUCCESS, 0, NULL);
    // Warm reboot may not be supported attempt cold reboot
    gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
    CpuDeadLoop();
  }
  // Load image
  EFI_HANDLE ImageHandle = NULL;
  Status = gBS->LoadImage (FALSE, self.getSelfImageHandle(), DevicePath, ImageBuffer.vdata(), ImageBuffer.size(), &ImageHandle);
  if ( EFI_ERROR (Status) ) {
    MsgLog("Failed to load image - %s\n", efiStrError(Status));
    return Status;
  }
  DBG("Boot image %ls, status=%s\n", devicePathString.wc_str(), efiStrError(Status));
  DBG("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
//  if (GlobalConfig.SavePreBootLog) Status = SaveBooterLog(&self.getCloverDir(), PREBOOT_LOG);
//      UninitRefitLib(); // close open file handles
  // Start image
  Status = gBS->StartImage (ImageHandle, NULL, NULL);
  if ( EFI_ERROR(Status) ) {
    MsgLog("Failed to start image - %s\n", efiStrError(Status));
    gBS->UnloadImage (ImageHandle);
    return Status;
  }
  DBG("----------------------------------------------------------------------------------\n");

//      ReinitRefitLib();
  gBS->UnloadImage (ImageHandle);
  if ( EFI_ERROR(Status) ) {
    MsgLog("Failed to unload image - %s\n", efiStrError(Status));
    gBS->UnloadImage (ImageHandle);
    return Status;
  }
  rebootNeededBeforeNextStartImage = true;
  return EFI_SUCCESS;
}

//
// main entry point
//
EFI_STATUS
EFIAPI
RefitMain (IN EFI_HANDLE           ImageHandle,
           IN EFI_SYSTEM_TABLE     *SystemTable)
{
  EFI_STATUS Status;
  BOOLEAN           MainLoopRunning = TRUE;
  BOOLEAN           ReinitDesktop = TRUE;
//  BOOLEAN           AfterTool = FALSE;
  REFIT_ABSTRACT_MENU_ENTRY  *ChosenEntry = NULL;
  REFIT_ABSTRACT_MENU_ENTRY  *OptionEntry = NULL;
  UINTN             MenuExit;
  XStringW          ConfName;
  EFI_TIME          Now;

  REFIT_MENU_SCREEN BootScreen(NULL);
  BootScreen.isBootScreen = true; //other screens will be constructed as false

  // bootstrap
  gST       = SystemTable;
  gImageHandle  = ImageHandle;
  gBS       = SystemTable->BootServices;
  gRT       = SystemTable->RuntimeServices;
//  /*Status = */EfiGetSystemConfigurationTable (&gEfiDxeServicesTableGuid, (VOID **) &gDS);
  
  TSCFrequency = GetMemLogTscTicksPerSecond(); //ticks for 1second

#ifdef DEBUG_ON_SERIAL_PORT
  SerialPortInitialize();
#endif

  {
    EFI_LOADED_IMAGE* LoadedImage;
    Status = gBS->HandleProtocol(gImageHandle, &gEfiLoadedImageProtocolGuid, (VOID **) &LoadedImage);

//    if ( !EFI_ERROR(Status) ) {
//      XString8 msg = S8Printf("Clover : Image base = 0x%llX\n", (uintptr_t)LoadedImage->ImageBase); // do not change, it's used by grep to feed the debugger
//      SerialPortWrite((UINT8*)msg.c_str(), msg.length());
//    }
    if ( !EFI_ERROR(Status) ) DBG("BootloaderChooserX64 : Image base = 0x%llX\n", (uintptr_t)LoadedImage->ImageBase); // do not change, it's used by grep to feed the debugger

#ifdef DEBUG_ON_SERIAL_PORT
    //gBS->Stall(2500000); // to give time to gdb to connect. Only needed if you want to put a breakpoint soon after this line.
#endif
  }

  construct_globals_objects(gImageHandle); // do this after SelfLoadedImage is initialized


#ifdef JIEF_DEBUG
//  all_tests();
//  PauseForKey(L"press\n");
#endif

  gRT->GetTime(&Now, NULL);

  InitBooterLog();

  if (Now.TimeZone < -1440 || Now.TimeZone > 1440) {
    MsgLog("Now is %02d.%02d.%d,  %02d:%02d:%02d (GMT)\n", Now.Day, Now.Month, Now.Year, Now.Hour, Now.Minute, Now.Second);
  } else {
    MsgLog("Now is %02d.%02d.%d,  %02d:%02d:%02d (GMT+%d)\n", Now.Day, Now.Month, Now.Year, Now.Hour, Now.Minute, Now.Second, GlobalConfig.Timezone);
  }
	MsgLog("Starting %s on %ls EFI\n", gRevisionStr, gST->FirmwareVendor);
//	if ( gBuildInfo ) DBG("Build with: [%s]\n", gBuildInfo);

	BdsLibConnectAllEfi(); //

  Status = InitRefitLib(gImageHandle);
  if (EFI_ERROR(Status))
    return Status;


  DBG("SelfDirPath = %ls\n", self.getCloverDirFullPath().wc_str());

  // disable EFI watchdog timer
  gBS->SetWatchdogTimer(0x0000, 0x0000, 0x0000, NULL);
//  ZeroMem((VOID*)&gSettings, sizeof(SETTINGS_DATA));

  Status = InitializeUnicodeCollationProtocol();
  if (EFI_ERROR(Status)) {
    DBG("UnicodeCollation Status=%s\n", efiStrError(Status));
  }
  
  Status = LoadUserSettings(L"BLC"_XSW, &gConfigDict);
  DBG("%ls\\BLC.plist%ls loaded: %s\n", self.getCloverDirFullPath().wc_str(), EFI_ERROR(Status) ? L" not" : L"", efiStrError(Status));
  GetUserSettings(gConfigDict);

  DbgHeader("InitScreen");
  InitScreen();
  SetupScreen();
	
//  //Now we have to reinit handles
//  Status = ReinitRefitLib();
//  if (EFI_ERROR(Status)){
//    DebugLog(2, " %s", efiStrError(Status));
//    PauseForKey(L"Error reinit refit\n");
//    return Status;
//  }
	
  MainMenu.Entries.setEmpty();
  OptionMenu.Entries.setEmpty();
  ScanVolumes();
  ScanLoaders();

  if ( gSettings.DefaultVolume.isEmpty() ) {
    MainMenu.TimeoutSeconds = -1;
  }else{
    MainMenu.TimeoutSeconds = gSettings.GUITimeOut;
  }
//MainMenu.TimeoutSeconds = 100;
//    if (gSettings.DisableCloverHotkeys) MenuEntryOptions.ShortcutLetter = 0x00;

  MainMenu.AddMenuEntry(&MenuEntryOptions, false);
  MainMenu.AddMenuEntry(&MenuEntryAbout, false);
  MainMenu.AddMenuEntry(&MenuEntryReset, false);
  MainMenu.AddMenuEntry(&MenuEntryShutdown, false);

  size_t DefaultIndex = SIZE_T_MAX;

  do
  {
    if ( gSettings.DefaultVolume.notEmpty() )
    {
      for ( size_t i = 0 ; i < MainMenu.Entries.size() ; i++ ) {
        const LOADER_ENTRY* lePtr = MainMenu.Entries[i].getLOADER_ENTRY();
        if ( lePtr ) {
          const LOADER_ENTRY& le = *lePtr;
          if ( le.DevicePathString.isEqualIC(gSettings.DefaultVolume) ) {
            le.SubScreen->Entries["Default"]->getREFIT_INPUT_DIALOG()->Item.BValue = true;
            DefaultIndex = i;
          }
        }
      }
    }
    // wait for user ACK when there were errors
    FinishTextScreen(false);

    MainLoopRunning = TRUE;
//    AfterTool = FALSE;

//OptionsMenu(&OptionEntry);
//savePreferencesFile();

    while (MainLoopRunning)
    {
      if (MainMenu.TimeoutSeconds == 0 && DefaultIndex != SIZE_T_MAX && !ReadAllKeyStrokes()) {
        // go strait to DefaultVolume loading
        ChosenEntry = &MainMenu.Entries[DefaultIndex];
        MenuExit = MENU_EXIT_TIMEOUT;
      } else {
        MenuExit = MainMenu.RunMainMenu(DefaultIndex, &ChosenEntry);
      }
      MainMenu.TimeoutSeconds = -1;

      if (MenuExit == MENU_EXIT_OPTIONS){
        OptionsMenu(&OptionEntry);
        savePreferencesFile();
        continue;
      }

      if (MenuExit == MENU_EXIT_HELP){
        HelpRefit();
        continue;
      }

      // EjectVolume
      if (MenuExit == MENU_EXIT_EJECT){
        Status = EFI_SUCCESS;
        if (ChosenEntry->getLOADER_ENTRY() ) {
          Status = EjectVolume(ChosenEntry->getLOADER_ENTRY()->Volume);
        }
        if (!EFI_ERROR(Status)) {
          break; //main loop is broken so Reinit all
        }
        continue;
      }

      // Hide toggle
      if (MenuExit == MENU_EXIT_HIDE_TOGGLE) {
          MainMenu.Entries.includeHidden = !MainMenu.Entries.includeHidden;
          continue;
      }

      // We don't allow exiting the main menu with the Escape key.
      if (MenuExit == MENU_EXIT_ESCAPE){
        break;   //refresh main menu
        //           continue;
      }

      if ( ChosenEntry->getREFIT_MENU_ITEM_RESET() ) {    // Restart
        // Attempt warm reboot
        gRT->ResetSystem(EfiResetWarm, EFI_SUCCESS, 0, NULL);
        // Warm reboot may not be supported attempt cold reboot
        gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
        // Terminate the screen and just exit
        TerminateScreen();
        MainLoopRunning = FALSE;
        ReinitDesktop = FALSE;
//        AfterTool = TRUE;
      }

      if ( ChosenEntry->getREFIT_MENU_ITEM_SHUTDOWN() ) { // It is not Shut Down, it is Exit from Clover
        TerminateScreen();
        //         gRT->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
        MainLoopRunning = FALSE;   // just in case we get this far
        ReinitDesktop = FALSE;
//        AfterTool = TRUE;
      }
      if ( ChosenEntry->getREFIT_MENU_ITEM_OPTIONS() ) {    // Options like KernelFlags, DSDTname etc.
        OptionsMenu(&OptionEntry);
        savePreferencesFile();
      }
      if ( ChosenEntry->getREFIT_MENU_ITEM_ABOUT() ) {    // About rEFIt
        AboutRefit();
      }

  /* -- not passed here
//  case TAG_HELP:
      HelpRefit();
      break;
  */
      if ( ChosenEntry->getLOADER_ENTRY() ) {   // Boot OS via .EFI loader
        TerminateScreen(); // it clears the screen
        MsgLog("\n");
        MsgLog("\n");
        Status = ChosenEntry->StartLoader();
        InitScreen();
        if ( EFI_ERROR(Status) ) PauseForKey(S8Printf("Cannot load. Status=%s. Press any key 2", efiStrError(Status)));
        MainLoopRunning = FALSE;
//        ReinitDesktop = FALSE;
//        AfterTool = TRUE;
      }

    } //MainLoopRunning
//    UninitRefitLib();
//    if (!AfterTool) {
//      ReinitRefitLib();
//    }
//    if (ReinitDesktop) {
//      ReinitRefitLib();
//    }
    //    PauseForKey(L"After ReinitSelfLib");
  } while (ReinitDesktop);

  // If we end up here, things have gone wrong. Try to reboot, and if that
  // fails, go into an endless loop.
  //Slice - NO!!! Return to EFI GUI
  //   gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
  //   EndlessIdleLoop();

  return EFI_SUCCESS;
}


