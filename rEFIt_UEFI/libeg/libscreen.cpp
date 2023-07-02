/*
 * libeg/screen.c
 * Screen handling functions
 *
 * Copyright (c) 2006 Christoph Pfisterer
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
#include "../Platform/Settings.h"


//#include <efiUgaDraw.h>
#include <Protocol/GraphicsOutput.h>
//#include <Protocol/efiConsoleControl.h>

// Console defines and variables

VOID egDumpSetConsoleVideoModes(VOID)
{
  UINTN i;
  UINTN Width, Height;
  UINTN BestMode = 0, BestWidth = 0, BestHeight = 0;
  EFI_STATUS Status;
  STATIC int Once=0;
  
  if (gST->ConOut != NULL && gST->ConOut->Mode != NULL) {
    if (!Once) {
      MsgLog("Console modes reported: %d, available modes:\n",gST->ConOut->Mode->MaxMode);
    }
    
    for (i=1; i <= (UINTN)gST->ConOut->Mode->MaxMode; i++) {
      Status = gST->ConOut->QueryMode(gST->ConOut, i-1, &Width, &Height);
      if (Status == EFI_SUCCESS) {
        //MsgLog("  Mode %d: %dx%d%ls\n", i, Width, Height, (i-1==(UINTN)gST->ConOut->Mode->Mode)?L" (current mode)":L"");
        if (!Once) {
        MsgLog(" - [%02llu]: %llux%llu%ls\n", i, Width, Height, (i-1==(UINTN)gST->ConOut->Mode->Mode)?L" (current mode)":L"");
        }
        // Select highest mode (-1) or lowest mode (-2) as/if requested
        if ((GlobalConfig.ConsoleMode == -1 && (BestMode == 0 || Height > BestHeight || (Height == BestHeight && Width > BestWidth))) ||
            (GlobalConfig.ConsoleMode == -2 && (BestMode == 0 || Height < BestHeight || (Height == BestHeight && Width < BestWidth)))) {
          BestMode = i;
          BestWidth = Width;
          BestHeight = Height;
        }
      }
    }
    Once++;
  } else {
    MsgLog("Console modes are not available.\n");
    return;
  }
  
  if (GlobalConfig.ConsoleMode > 0) {
    // Specific mode chosen, try to set it
    BestMode = GlobalConfig.ConsoleMode;
  }
  
  if (BestMode >= 1 && BestMode <= (UINTN)gST->ConOut->Mode->MaxMode) {
    // Mode is valid
    if (BestMode-1 != (UINTN)gST->ConOut->Mode->Mode) {
      Status = gST->ConOut->SetMode(gST->ConOut, BestMode-1);
      MsgLog("  Setting mode (%llu): %s\n", BestMode, efiStrError(Status));
    } else {
      MsgLog("  Selected mode (%llu) is already set\n", BestMode);
    }
  } else if (BestMode != 0) {
    MsgLog("  Selected mode (%llu) is not valid\n", BestMode);
  }
}

#include <Protocol/efiConsoleControl.h>
static EFI_CONSOLE_CONTROL_PROTOCOL *ConsoleControl = NULL;
static EFI_GUID ConsoleControlProtocolGuid = EFI_CONSOLE_CONTROL_PROTOCOL_GUID;


VOID egInitScreen()
{
  EFI_STATUS Status;
    // get protocols
  if ( !ConsoleControl ) {
    Status = EfiLibLocateProtocol(ConsoleControlProtocolGuid, (void **) &ConsoleControl);
    if (EFI_ERROR(Status)) ConsoleControl = NULL;
  }
  if ( ConsoleControl ) {
    EFI_CONSOLE_CONTROL_SCREEN_MODE mode;
    BOOLEAN uga;
    BOOLEAN b;
    Status = ConsoleControl->GetMode(ConsoleControl, &mode, &uga, &b);
    if ( !EFI_ERROR(Status) && mode != EfiConsoleControlScreenText ) {
      Status = ConsoleControl->SetMode(ConsoleControl, EfiConsoleControlScreenText);
    }
  }else{
    MsgLog("ConstrolControl==NULL, cannot switch to text mode\n");
  }
  GlobalConfig.ConsoleMode = -1;
  egDumpSetConsoleVideoModes();
}


/* EOF */
