/*
 * refit/screen.c
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

#include "screen.h"
#include <Platform.h> // Only use angled for Platform, else, xcode project won't compile
#include "../Platform/BasicIO.h"
#include "menu.h"
#include "../gui/REFIT_MENU_SCREEN.h"

#ifndef DEBUG_ALL
#define DEBUG_SCR 1
#else
#define DEBUG_SCR DEBUG_ALL
#endif

#if DEBUG_SCR == 0
#define DBG(...)
#else
#define DBG(...) DebugLog(DEBUG_SCR, __VA_ARGS__)	
#endif

// Console defines and variables

UINTN ConWidth;
UINTN ConHeight;
CHAR16 *BlankLine = NULL;
INTN BanHeight = 0;

static VOID DrawScreenHeader(IN CONST CHAR16 *Title);
static VOID UpdateConsoleVars(VOID);

// UGA defines and variables


// general defines and variables

//static XBool haveError = FALSE;

//
// Screen initialization and switching
//

VOID InitScreen()
{
	//DbgHeader("InitScreen");
  // initialize libeg
  egInitScreen();
        
  // disable cursor
	gST->ConOut->EnableCursor(gST->ConOut, FALSE);
    
  UpdateConsoleVars();

  // show the banner (even when in graphics mode)
	//DrawScreenHeader(L"Initializing...");
}

VOID SetupScreen(VOID)
{
    // switch to text mode if requested
}

//
// Screen control for running tools
//
VOID BeginTextScreen(IN CONST CHAR16 *Title)
{
    DrawScreenHeader(Title);
    
    // reset error flag
    haveError = false;
}

void FinishTextScreen(IN XBool WaitAlways)
{
    if (haveError || WaitAlways) {
 //       PauseForKey(L"FinishTextScreen");
    }
    
    // reset error flag
    haveError = false;
}

VOID TerminateScreen(VOID)
{
    // clear text screen
	gST->ConOut->SetAttribute(gST->ConOut, ATTR_BANNER);
	gST->ConOut->ClearScreen(gST->ConOut);
    
    // enable cursor
	gST->ConOut->EnableCursor(gST->ConOut, TRUE);
}

static VOID DrawScreenHeader(IN CONST CHAR16 *Title)
{
  UINTN i;
	CHAR16* BannerLine = (__typeof__(BannerLine))AllocatePool((ConWidth + 1) * sizeof(CHAR16));
  BannerLine[ConWidth] = 0;

  // clear to black background
	//gST->ConOut->SetAttribute(gST->ConOut, ATTR_BASIC);
  //gST->ConOut->ClearScreen (gST->ConOut);

  // paint header background
  gST->ConOut->SetAttribute(gST->ConOut, ATTR_BANNER);
	
	for (i = 1; i < ConWidth-1; i++) {
    BannerLine[i] = BOXDRAW_HORIZONTAL;
	}
	
	BannerLine[0] = BOXDRAW_DOWN_RIGHT;
	BannerLine[ConWidth-1] = BOXDRAW_DOWN_LEFT;
  gST->ConOut->SetCursorPosition (gST->ConOut, 0, 0);
	printf("%ls", BannerLine);

	for (i = 1; i < ConWidth-1; i++)
    BannerLine[i] = ' ';
	BannerLine[0] = BOXDRAW_VERTICAL;
	BannerLine[ConWidth-1] = BOXDRAW_VERTICAL;
  gST->ConOut->SetCursorPosition (gST->ConOut, 0, 1);
	printf("%ls", BannerLine);

	for (i = 1; i < ConWidth-1; i++)
    BannerLine[i] = BOXDRAW_HORIZONTAL;
 	BannerLine[0] = BOXDRAW_UP_RIGHT;
	BannerLine[ConWidth-1] = BOXDRAW_UP_LEFT;
  gST->ConOut->SetCursorPosition (gST->ConOut, 0, 2);
	printf("%ls", BannerLine);

	FreePool(BannerLine);

  // print header text
  gST->ConOut->SetCursorPosition (gST->ConOut, 3, 1);
  printf("%ls", Title);

  // reposition cursor
  gST->ConOut->SetAttribute (gST->ConOut, ATTR_BASIC);
  gST->ConOut->SetCursorPosition (gST->ConOut, 0, 4);
}


////
//// Error handling
////
//
//BOOLEAN CheckFatalError(IN EFI_STATUS Status, IN CONST CHAR16 *where)
//{
////    CHAR16 ErrorName[64];
//
//    if (!EFI_ERROR(Status))
//        return FALSE;
//
////    StatusToString(ErrorName, Status);
//    gST->ConOut->SetAttribute (gST->ConOut, ATTR_ERROR);
//    printf("Fatal Error: %s %ls\n", efiStrError(Status), where);
//    gST->ConOut->SetAttribute (gST->ConOut, ATTR_BASIC);
//    haveError = TRUE;
//
//    //gBS->Exit(ImageHandle, ExitStatus, ExitDataSize, ExitData);
//
//    return TRUE;
//}
//
//BOOLEAN CheckError(IN EFI_STATUS Status, IN CONST CHAR16 *where)
//{
////    CHAR16 ErrorName[64];
//
//    if (!EFI_ERROR(Status))
//        return FALSE;
//
////    StatusToString(ErrorName, Status);
//    gST->ConOut->SetAttribute (gST->ConOut, ATTR_ERROR);
//    printf("Error: %s %ls\n", efiStrError(Status), where);
//    gST->ConOut->SetAttribute (gST->ConOut, ATTR_BASIC);
//    haveError = TRUE;
//
//    return TRUE;
//}


#define MAX_SIZE_ANIME 256


//
// Updates console variables, according to ConOut resolution 
// This should be called when initializing screen, or when resolution changes
//

static VOID UpdateConsoleVars()
{
    UINTN i;

    // get size of text console
	if  (gST->ConOut->QueryMode(gST->ConOut, gST->ConOut->Mode->Mode, &ConWidth, &ConHeight) != EFI_SUCCESS) {
        // use default values on error
        ConWidth = 80;
        ConHeight = 25;
    }

    // free old BlankLine when it exists
    if (BlankLine != NULL) {
        FreePool(BlankLine);
    }

    // make a buffer for a whole text line
    BlankLine = (__typeof__(BlankLine))AllocatePool((ConWidth + 1) * sizeof(CHAR16));
	
	for (i = 0; i < ConWidth; i++) {
        BlankLine[i] = ' ';
	}
	
    BlankLine[i] = 0;
}
