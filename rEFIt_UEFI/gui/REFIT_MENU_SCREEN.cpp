/*
 * refit/menu.c
 * Menu functions
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

#include "./REFIT_MENU_SCREEN.h"
#include <Platform.h>
//#include "../include/scroll_images.h"

//#include "colors.h"

#include "../libeg/FloatLib.h"
#include "REFIT_MENU_SCREEN.h"
//#include "screen.h"
#include "../cpp_foundation/XString.h"
#include "../refit/menu.h"  // for DrawTextXY. Must disappear soon.
#include "../refit/screen.h"
#include "../Platform/Events.h"
#include "../Settings/Self.h"

#ifndef DEBUG_ALL
#define DEBUG_MENU 1
#else
#define DEBUG_MENU DEBUG_ALL
#endif

#if DEBUG_MENU == 0
#define DBG(...)
#else
#define DBG(...) DebugLog(DEBUG_MENU, __VA_ARGS__)
#endif

#include "../Platform/Settings.h"


#define SCROLL_LINE_UP        (0)
#define SCROLL_LINE_DOWN      (1)
#define SCROLL_PAGE_UP        (2)
#define SCROLL_PAGE_DOWN      (3)
#define SCROLL_FIRST          (4)
#define SCROLL_LAST           (5)
#define SCROLL_NONE           (6)
#define SCROLL_SCROLL_DOWN    (7)
#define SCROLL_SCROLL_UP      (8)
//#define SCROLL_SCROLLBAR_MOVE (9)

//
#define TEXT_CORNER_REVISION  (1)
#define TEXT_CORNER_HELP      (2)
#define TEXT_CORNER_OPTIMUS   (3)
//
#define TITLE_MAX_LEN (SVALUE_MAX_SIZE / sizeof(CHAR16) + 128)
//
// other menu definitions

#define MENU_FUNCTION_INIT            (0)
#define MENU_FUNCTION_CLEANUP         (1)
#define MENU_FUNCTION_PAINT_ALL       (2)
#define MENU_FUNCTION_PAINT_SELECTION (3)
#define MENU_FUNCTION_PAINT_TIMEOUT   (4)


//
const CHAR16 ArrowUp[2]   = { ARROW_UP, 0 }; //defined in  Simple Text Out protocol
const CHAR16 ArrowDown[2] = { ARROW_DOWN, 0 };
//
//BOOLEAN MainAnime = FALSE;
//
////TODO Scroll variables must be a part of REFIT_SCREEN
////BOOLEAN ScrollEnabled = FALSE;
//
INTN ScrollbarYMovement;
//
//
////#define TextHeight (FONT_CELL_HEIGHT + TEXT_YMARGIN * 2)
//

//
////TODO spacing must be a part of layout in XTheme
#define TITLEICON_SPACING (16)
////#define ROW0__TILESIZE (144)
////#define ROW1_TILESIZE (64)
#define TILE1_XSPACING (8)
////#define TILE_YSPACING (24)
#define ROW0_SCROLLSIZE (100)
//
//
static INTN OldX = 0, OldY = 0;
static INTN OldTextWidth = 0, OldTextHeight = 0;
static UINTN OldRow = 0;
static INTN OldTimeoutTextWidth = 0;
static INTN MenuWidth;

BOOLEAN mGuiReady = FALSE;



VOID REFIT_MENU_SCREEN::AddMenuInfo_f(CONST char *format, ...)
{

//DBG("%s, %s : Line=%s\n", __FILE__, __LINE__, XString(Line).c);
  REFIT_INFO_DIALOG *InputBootArgs;

  InputBootArgs = new REFIT_INFO_DIALOG;
  VA_LIST va;
	VA_START(va, format);
  InputBootArgs->Title.vSWPrintf(format, va);
  VA_END(va);
  InputBootArgs->AtClick = ActionLight;
  AddMenuEntry(InputBootArgs, true);
}

//
// Graphics helper functions
//

/*
  SelectionImages:
    [0] SelectionBig
    [2] SelectionSmall
    [4] SelectionIndicator
  Buttons:
    [0] radio_button
    [1] radio_button_selected
    [2] checkbox
    [3] checkbox_checked
*/


//
// Scrolling functions
//
#define CONSTRAIN_MIN(Variable, MinValue) if (Variable < MinValue) Variable = MinValue
#define CONSTRAIN_MAX(Variable, MaxValue) if (Variable > MaxValue) Variable = MaxValue

VOID REFIT_MENU_SCREEN::InitScroll(IN INTN ItemCount, IN UINTN MaxCount,
                       IN UINTN VisibleSpace, IN INTN Selected)
{
  //ItemCount - a number to scroll (Row0)
  //MaxCount - total number (Row0 + Row1)
  //VisibleSpace - a number to fit

  ScrollState.LastSelection = ScrollState.CurrentSelection = Selected;
  //MaxIndex, MaxScroll, MaxVisible are indexes, 0..N-1
  ScrollState.MaxIndex = (INTN)MaxCount - 1;
  ScrollState.MaxScroll = ItemCount - 1;

  if (VisibleSpace == 0) {
    ScrollState.MaxVisible = ScrollState.MaxScroll;
  } else {
    ScrollState.MaxVisible = (INTN)VisibleSpace - 1;
  }

  if (ScrollState.MaxVisible >= ItemCount) {
      ScrollState.MaxVisible = ItemCount - 1;
  }

  ScrollState.MaxFirstVisible = ScrollState.MaxScroll - ScrollState.MaxVisible;
  CONSTRAIN_MIN(ScrollState.MaxFirstVisible, 0);
  ScrollState.FirstVisible = MIN(Selected, ScrollState.MaxFirstVisible);


  ScrollState.IsScrolling = (ScrollState.MaxFirstVisible > 0);
  ScrollState.PaintAll = true;
  ScrollState.PaintSelection = false;

  ScrollState.LastVisible = ScrollState.FirstVisible + ScrollState.MaxVisible;

}

VOID REFIT_MENU_SCREEN::UpdateScroll(IN UINTN Movement)
{
  ScrollState.LastSelection = ScrollState.CurrentSelection;

  switch (Movement) {

    case SCROLL_LINE_UP: //of left = decrement
      if (ScrollState.CurrentSelection > 0) {
        ScrollState.CurrentSelection --;
        if (ScrollState.CurrentSelection < ScrollState.FirstVisible) {
          ScrollState.PaintAll = true;
          ScrollState.FirstVisible = ScrollState.CurrentSelection;
        }
        if (ScrollState.CurrentSelection == ScrollState.MaxScroll) {
          ScrollState.PaintAll = true;
        }
        if ((ScrollState.CurrentSelection < ScrollState.MaxScroll) &&
             (ScrollState.CurrentSelection > ScrollState.LastVisible)) {
          ScrollState.PaintAll = true;
          ScrollState.LastVisible = ScrollState.CurrentSelection;
          ScrollState.FirstVisible = ScrollState.LastVisible - ScrollState.MaxVisible;
        }
      }
      break;

    case SCROLL_LINE_DOWN: //or right -- increment
      if (ScrollState.CurrentSelection < ScrollState.MaxIndex) {
        ScrollState.CurrentSelection++;
        if ((ScrollState.CurrentSelection > ScrollState.LastVisible) &&
            (ScrollState.CurrentSelection <= ScrollState.MaxScroll)){
          ScrollState.PaintAll = true;
          ScrollState.FirstVisible++;
          CONSTRAIN_MAX(ScrollState.FirstVisible, ScrollState.MaxFirstVisible);
        }
        if (ScrollState.CurrentSelection == ScrollState.MaxScroll + 1) {
          ScrollState.PaintAll = true;
        }
      }
      break;

    case SCROLL_SCROLL_DOWN:
      if (ScrollState.FirstVisible < ScrollState.MaxFirstVisible) {
        if (ScrollState.CurrentSelection == ScrollState.FirstVisible)
          ScrollState.CurrentSelection++;
        ScrollState.FirstVisible++;
        ScrollState.LastVisible++;
        ScrollState.PaintAll = true;
      }
      break;

    case SCROLL_SCROLL_UP:
      if (ScrollState.FirstVisible > 0) {
        if (ScrollState.CurrentSelection == ScrollState.LastVisible)
          ScrollState.CurrentSelection--;
        ScrollState.FirstVisible--;
        ScrollState.LastVisible--;
        ScrollState.PaintAll = true;
      }
      break;

    case SCROLL_PAGE_UP:
      if (ScrollState.CurrentSelection > 0) {
        if (ScrollState.CurrentSelection == ScrollState.MaxIndex) {   // currently at last entry, special treatment
          if (ScrollState.IsScrolling)
            ScrollState.CurrentSelection -= ScrollState.MaxVisible - 1;  // move to second line without scrolling
          else
            ScrollState.CurrentSelection = 0;                // move to first entry
        } else {
          if (ScrollState.FirstVisible > 0)
            ScrollState.PaintAll = true;
          ScrollState.CurrentSelection -= ScrollState.MaxVisible;          // move one page and scroll synchronously
          ScrollState.FirstVisible -= ScrollState.MaxVisible;
        }
        CONSTRAIN_MIN(ScrollState.CurrentSelection, 0);
        CONSTRAIN_MIN(ScrollState.FirstVisible, 0);
        if (ScrollState.CurrentSelection < ScrollState.FirstVisible) {
          ScrollState.PaintAll = true;
          ScrollState.FirstVisible = ScrollState.CurrentSelection;
        }
      }
      break;

    case SCROLL_PAGE_DOWN:
//    DBG("cur=%lld, maxInd=%lld, maxVis=%lld, First=%lld, maxFirst=%lld, lastVis=%lld, maxscr=%lld\n",
//        ScrollState.CurrentSelection, ScrollState.MaxIndex, ScrollState.MaxVisible, ScrollState.FirstVisible,
//        ScrollState.MaxFirstVisible, ScrollState.CurrentSelection, ScrollState.LastVisible);

      if (ScrollState.CurrentSelection < ScrollState.MaxIndex) {
        if (ScrollState.CurrentSelection == 0) {   // currently at first entry, special treatment
          if (ScrollState.IsScrolling)
            ScrollState.CurrentSelection = ScrollState.MaxVisible - 1;  // move to second-to-last line without scrolling
          else
            ScrollState.CurrentSelection = ScrollState.MaxIndex;         // move to last entry
        } else {
//          if (ScrollState.FirstVisible < ScrollState.MaxFirstVisible)
//            ScrollState.PaintAll = true;
          ScrollState.CurrentSelection += ScrollState.MaxVisible;          // move one page and scroll synchronously
          ScrollState.FirstVisible += ScrollState.MaxVisible;
        }
        CONSTRAIN_MAX(ScrollState.CurrentSelection, ScrollState.MaxIndex); // if (v>m) v=m;
        CONSTRAIN_MAX(ScrollState.FirstVisible, ScrollState.MaxFirstVisible);
        if ((ScrollState.CurrentSelection > ScrollState.LastVisible) &&
            (ScrollState.CurrentSelection <= ScrollState.MaxScroll)){
 //         ScrollState.PaintAll = true;
          ScrollState.FirstVisible+= ScrollState.MaxVisible;
          CONSTRAIN_MAX(ScrollState.FirstVisible, ScrollState.MaxFirstVisible);
        }
        ScrollState.PaintAll = true;
      }
//    DBG("after cur=%lld, maxInd=%lld, maxVis=%lld, First=%lld, maxFirst=%lld, lastVis=%lld, maxscr=%lld\n",
//        ScrollState.CurrentSelection, ScrollState.MaxIndex, ScrollState.MaxVisible, ScrollState.FirstVisible,
//        ScrollState.MaxFirstVisible, ScrollState.CurrentSelection, ScrollState.LastVisible);

      break;

    case SCROLL_FIRST:
      if (ScrollState.CurrentSelection > 0) {
        ScrollState.CurrentSelection = 0;
        if (ScrollState.FirstVisible > 0) {
          ScrollState.PaintAll = true;
          ScrollState.FirstVisible = 0;
        }
      }
      break;

    case SCROLL_LAST:
      if (ScrollState.CurrentSelection < ScrollState.MaxIndex) {
        ScrollState.CurrentSelection = ScrollState.MaxIndex;
        if (ScrollState.FirstVisible < ScrollState.MaxFirstVisible) {
          ScrollState.PaintAll = true;
          ScrollState.FirstVisible = ScrollState.MaxFirstVisible;
        }
      }
      break;

    case SCROLL_NONE:
      // The caller has already updated CurrentSelection, but we may
      // have to scroll to make it visible.
      if (ScrollState.CurrentSelection < ScrollState.FirstVisible) {
        ScrollState.PaintAll = true;
        ScrollState.FirstVisible = ScrollState.CurrentSelection; // - (ScrollState.MaxVisible >> 1);
        CONSTRAIN_MIN(ScrollState.FirstVisible, 0);
      } else if ((ScrollState.CurrentSelection > ScrollState.LastVisible) &&
                 (ScrollState.CurrentSelection <= ScrollState.MaxScroll)) {
        ScrollState.PaintAll = true;
        ScrollState.FirstVisible = ScrollState.CurrentSelection - ScrollState.MaxVisible;
        CONSTRAIN_MAX(ScrollState.FirstVisible, ScrollState.MaxFirstVisible);
      }
      break;
    default:
      panic("REFIT_MENU_SCREEN::UpdateScroll");
  }

  if (!ScrollState.PaintAll && ScrollState.CurrentSelection != ScrollState.LastSelection)
    ScrollState.PaintSelection = true;
  ScrollState.LastVisible = ScrollState.FirstVisible + ScrollState.MaxVisible;

}

VOID REFIT_MENU_SCREEN::AddMenuInfoLine_f(CONST char *format, ...)
{
  XStringW* s = new XStringW();
  VA_LIST va;
	VA_START(va, format);
  s->vSWPrintf(format, va);
  VA_END(va);
  InfoLines.AddReference(s, true);
}

VOID REFIT_MENU_SCREEN::AddMenuEntry(IN REFIT_ABSTRACT_MENU_ENTRY *Entry, bool freeIt)
{
	if ( !Entry ) return;
	Entries.AddReference(Entry, freeIt);
}

// This is supposed to be a destructor ?
VOID REFIT_MENU_SCREEN::FreeMenu()
{
  REFIT_ABSTRACT_MENU_ENTRY *Tentry = NULL;
//TODO - here we must Free for a list of Entries, Screens, InputBootArgs
  if (Entries.size() > 0) {
    for (UINTN i = 0; i < Entries.size(); i++) {
      Tentry = &Entries[i];
      if (Tentry->SubScreen) {
        // don't free image because of reusing them
        Tentry->SubScreen->FreeMenu();
        Tentry->SubScreen = NULL;
      }
    }
    Entries.setEmpty();
  }
  InfoLines.setEmpty();
}

INTN REFIT_MENU_SCREEN::FindMenuShortcutEntry(IN CHAR16 Shortcut)
{
  if (Shortcut >= 'a' && Shortcut <= 'z')
    Shortcut -= ('a' - 'A');
  if (Shortcut) {
    for (UINTN i = 0; i < Entries.size(); i++) {
      if (Entries[i].ShortcutDigit == Shortcut ||
          Entries[i].ShortcutLetter == Shortcut) {
        return i;
      }
    }
  }
  return -1;
}

//
// generic input menu function
// usr-sse2
//
UINTN REFIT_MENU_SCREEN::InputDialog()
{
	if ( !Entries[ScrollState.CurrentSelection].getREFIT_MENU_ITEM_IEM_ABSTRACT() ) {
		DebugLog(2, "BUG : InputDialog called with !Entries[ScrollState.CurrentSelection].REFIT_MENU_ENTRY_ITEM_ABSTRACT()\n");
		return 0; // is it the best thing to do ? CpuDeadLog ?
	}

  EFI_STATUS    Status;
  EFI_INPUT_KEY key;
  UINTN         ind = 0;
  UINTN         MenuExit = 0;
  //UINTN         LogSize;
  UINTN         Pos = (Entries[ScrollState.CurrentSelection]).Row;
  REFIT_MENU_ENTRY_ITEM_ABSTRACT& selectedEntry = *Entries[ScrollState.CurrentSelection].getREFIT_MENU_ITEM_IEM_ABSTRACT();
  INPUT_ITEM&   Item = selectedEntry.Item;
  XStringW      Backup = Item.SValue;
  UINTN         BackupPos, BackupShift;
//  XStringW      Buffer;
  //SCROLL_STATE  StateLine;

  /*
    I would like to see a LineSize that depends on the Title width and the menu width so
    the edit dialog does not extend beyond the menu width.
    There are 3 cases:
    1) Text menu where MenuWidth is min of ConWidth - 6 and max of 50 and all StrLen(Title)
    2) Graphics menu where MenuWidth is measured in pixels and font is fixed width.
       The following works well in my case but depends on font width and minimum screen size.
         LineSize = 76 - StrLen(Screen->Entries[State->CurrentSelection].Title);
    3) Graphics menu where font is proportional. In this case LineSize would depend on the
       current width of the displayed string which would need to be recalculated after
       every change.
    Anyway, the above will not be implemented for now, and LineSize will remain at 38
    because it works.
  */
  UINTN         LineSize = 38;
  // make sure that LineSize is not too big
  UINTN MaxPossibleLineSize = (MenuWidth - selectedEntry.Title.length());
  if (LineSize > MaxPossibleLineSize) {
    LineSize = MaxPossibleLineSize;
  }

  BackupShift = Item.LineShift;
  BackupPos = Pos;

  do {

    if (Item.ItemType == BoolValue) {
      Item.BValue = !Item.BValue;
      MenuExit = MENU_EXIT_ENTER;
    } else if (Item.ItemType == RadioSwitch) {
      MenuExit = MENU_EXIT_ENTER;
    } else if (Item.ItemType == CheckBit) {
      Item.IValue ^= Pos;
      MenuExit = MENU_EXIT_ENTER;
    } else {

      Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &key);

      if (Status == EFI_NOT_READY) {
        gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &ind);
        continue;
      }

      switch (key.ScanCode) {
        case SCAN_RIGHT:
          if (Pos + Item.LineShift < Item.SValue.length()) {
            if (Pos < LineSize)
              Pos++;
            else
              Item.LineShift++;
          }
          break;
        case SCAN_LEFT:
          if (Pos > 0)
            Pos--;
          else if (Item.LineShift > 0)
            Item.LineShift--;
          break;
        case SCAN_HOME:
          Pos = 0;
          Item.LineShift=0;
          break;
        case SCAN_END:
          if (Item.SValue.length() < LineSize)
            Pos = Item.SValue.length();
          else {
            Pos = LineSize;
            Item.LineShift = Item.SValue.length() - LineSize;
          }
          break;
        case SCAN_ESC:
          MenuExit = MENU_EXIT_ESCAPE;
          continue;
          break;
//        case SCAN_F2:
//          GlobalConfig.SavePreBootLog = true;
//          break;
          //not used here
/*        case SCAN_F6:
          Status = egSaveFile(SelfRootDir, VBIOS_BIN, (UINT8*)(UINTN)0xc0000, 0x20000);
          if (EFI_ERROR(Status)) {
            Status = egSaveFile(NULL, VBIOS_BIN, (UINT8*)(UINTN)0xc0000, 0x20000);
          }
          break; */

        case SCAN_DELETE:
          // forward delete
          if (Pos + Item.LineShift < Item.SValue.length()) {
            Item.SValue.deleteCharsAtPos(Pos + Item.LineShift);
//            for (i = Pos + Item.LineShift; i < Item.SValue.length(); i++) {
//               Buffer[i] = Buffer[i+1];
//            }
            /*
            // Commented this out because it looks weird - Forward Delete should not
            // affect anything left of the cursor even if it's just to shift more of the
            // string into view.
            if (Item.LineShift > 0 && Item.LineShift + LineSize > StrLen(Buffer)) {
              Item.LineShift--;
              Pos++;
            }
            */
          }
          break;
      }

      switch (key.UnicodeChar) {
        case CHAR_BACKSPACE:
          if (Item.SValue[0] != CHAR_NULL && Pos != 0) {
            Item.SValue.deleteCharsAtPos(Pos + Item.LineShift - 1);
//            for (i = Pos + Item.LineShift; i <= Item.SValue.length(); i++) {
//               Buffer[i-1] = Buffer[i];
//            }
            Item.LineShift > 0 ? Item.LineShift-- : Pos--;
          }

          break;

        case CHAR_LINEFEED:
        case CHAR_CARRIAGE_RETURN:
          MenuExit = MENU_EXIT_ENTER;
          Pos = 0;
          Item.LineShift = 0;
          break;
        default:
          if ((key.UnicodeChar >= 0x20) &&
              (key.UnicodeChar < 0x80)){
            if ( Item.SValue.length() < SVALUE_MAX_SIZE) {
              Item.SValue.insertAtPos(key.UnicodeChar, Pos + Item.LineShift);
//              for (i = StrLen(Buffer)+1; i > Pos + Item.LineShift; i--) {
//                 Buffer[i] = Buffer[i-1];
//              }
//              Buffer[i] = key.UnicodeChar;
              Pos < LineSize ? Pos++ : Item.LineShift++;
            }
          }
          break;
      }
    }
    // Redraw the field
    (Entries[ScrollState.CurrentSelection]).Row = Pos;
    TextMenuStyle(MENU_FUNCTION_PAINT_SELECTION, NULL);
  } while (!MenuExit);

  switch (MenuExit) {
    case MENU_EXIT_ENTER:
      Item.Valid = true;
      ApplyInputs();
      break;

    case MENU_EXIT_ESCAPE:
      if ( !Item.SValue.isEqual(Backup) ) {
        Item.SValue = Backup;
        if (Item.ItemType != BoolValue) {
          Item.LineShift = BackupShift;
          (Entries[ScrollState.CurrentSelection]).Row = BackupPos;
        }
        TextMenuStyle( MENU_FUNCTION_PAINT_SELECTION, NULL);
      }
      break;
  }
  Item.Valid = false;
  if (Item.SValue.notEmpty()) {
    MsgLog("EDITED: %ls\n", Item.SValue.wc_str());
  }
  return 0;
}


// TimeoutDefault for a wait in seconds
// return EFI_TIMEOUT if no inputs
//the function must be in menu_screen class
//so UpdatePointer(); => mPointer.Update(&gItemID, &Screen->mAction);
EFI_STATUS REFIT_MENU_SCREEN::WaitForInputEventPoll(UINTN TimeoutDefault)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN TimeoutRemain = TimeoutDefault * 100;

  while (TimeoutRemain != 0) {
    Status = WaitFor2EventWithTsc(gST->ConIn->WaitForKey, NULL, 10);
    if (Status != EFI_TIMEOUT) {
      break;
    }
    TimeoutRemain--;
  }
  return Status;
}


UINTN REFIT_MENU_SCREEN::RunGenericMenu(IN OUT INTN *DefaultEntryIndex, OUT REFIT_ABSTRACT_MENU_ENTRY **ChosenEntry)
{
  EFI_STATUS    Status;
  EFI_INPUT_KEY key;
  //    UINTN         Index;
  INTN          ShortcutEntry;
  BOOLEAN       HaveTimeout = FALSE;
  INTN          TimeoutCountdown = 0;
  UINTN         MenuExit;

  if (ChosenEntry == NULL) {
    TextStyle = 0;
  } else {
    TextStyle = 2;
  }

  //no default - no timeout!
  if ((*DefaultEntryIndex != -1) && (TimeoutSeconds > 0)) {
    //      DBG("have timeout\n");
    HaveTimeout = true;
    TimeoutCountdown =  TimeoutSeconds;
  }
  MenuExit = 0;

  TextMenuStyle(MENU_FUNCTION_INIT, NULL);
  //  DBG("scroll inited\n");
  // override the starting selection with the default index, if any
  if (*DefaultEntryIndex >= 0 && *DefaultEntryIndex <=  ScrollState.MaxIndex) {
     ScrollState.CurrentSelection = *DefaultEntryIndex;
     UpdateScroll(SCROLL_NONE);
  }
  IsDragging = false;
  //  DBG("RunGenericMenu CurrentSelection=%d MenuExit=%d\n",
  //      State.CurrentSelection, MenuExit);

  // exhaust key buffer and be sure no key is pressed to prevent option selection
  // when coming with a key press from timeout=0, for example
  while (ReadAllKeyStrokes()) gBS->Stall(500 * 1000);
  while (!MenuExit) {
    // update the screen
    if (ScrollState.PaintAll) {
      TextMenuStyle(MENU_FUNCTION_PAINT_ALL, NULL);
      ScrollState.PaintAll = false;
    } else if (ScrollState.PaintSelection) {
      TextMenuStyle(MENU_FUNCTION_PAINT_SELECTION, NULL);
      ScrollState.PaintSelection = false;
    }

    if (HaveTimeout) {
      XStringW TOMessage = SWPrintf("%ls in %lld seconds", TimeoutText.wc_str(), TimeoutCountdown);
      TextMenuStyle(MENU_FUNCTION_PAINT_TIMEOUT, TOMessage.wc_str());
    }

    key.UnicodeChar = 0;
    key.ScanCode = 0;
    if (!mGuiReady) {
      mGuiReady = true;
      DBG("UI ready\n");
    }
    
    EFI_TIME          Now;
    gRT->GetTime(&Now, NULL);
    if (GlobalConfig.Timezone != 0xFF) {
      INT32 NowHour = Now.Hour + GlobalConfig.Timezone;
      if (NowHour <  0 ) NowHour += 24;
      if (NowHour >= 24 ) NowHour -= 24;
      Daylight = (NowHour > 8) && (NowHour < 20);  //this is the screen member
    } else {
      Daylight = true;
    }

    Status = WaitForInputEventPoll(1); //wait for 1 seconds.
    if (Status == EFI_TIMEOUT) {
      if (HaveTimeout) {
        if (TimeoutCountdown <= 0) {
          // timeout expired
          MenuExit = MENU_EXIT_TIMEOUT;
          break;
        } else {
          //     gBS->Stall(100000);
          TimeoutCountdown--;
        }
      }
      continue;
    }

    switch (mAction) {
      case ActionSelect:
        ScrollState.LastSelection = ScrollState.CurrentSelection;
        ScrollState.CurrentSelection = mItemID;
        ScrollState.PaintAll = true;
        break;
      case ActionEnter:
        ScrollState.LastSelection = ScrollState.CurrentSelection;
        ScrollState.CurrentSelection = mItemID;
        if ( Entries[mItemID].getREFIT_INPUT_DIALOG() ||  Entries[mItemID].getREFIT_MENU_CHECKBIT() ) {
          MenuExit = InputDialog();
        } else if (Entries[mItemID].getREFIT_MENU_SWITCH()) {
          MenuExit = InputDialog();
          ScrollState.PaintAll = true;
        } else if (!Entries[mItemID].getREFIT_INFO_DIALOG()) {
          MenuExit = MENU_EXIT_ENTER;
        }
        break;
      case ActionHelp:
        MenuExit = MENU_EXIT_HELP;
        break;
      case ActionOptions:
        ScrollState.LastSelection = ScrollState.CurrentSelection;
        ScrollState.CurrentSelection = mItemID;
        MenuExit = MENU_EXIT_OPTIONS;
        break;
      case ActionDetails:
        ScrollState.LastSelection = ScrollState.CurrentSelection;
        // Index = State.CurrentSelection;
        ScrollState.CurrentSelection = mItemID;
        if ((Entries[mItemID].getREFIT_INPUT_DIALOG()) ||
            (Entries[mItemID].getREFIT_MENU_CHECKBIT())) {
          MenuExit = InputDialog();
        } else if (Entries[mItemID].getREFIT_MENU_SWITCH()) {
          MenuExit = InputDialog();
          ScrollState.PaintAll = true;
        } else if (!Entries[mItemID].getREFIT_INFO_DIALOG()) {
          MenuExit = MENU_EXIT_DETAILS;
        }
        break;
      case ActionDeselect:
        ScrollState.LastSelection = ScrollState.CurrentSelection;
        ScrollState.PaintAll = true;
        break;
      case ActionFinish:
        MenuExit = MENU_EXIT_ESCAPE;
        break;
      case ActionScrollDown:
        UpdateScroll(SCROLL_SCROLL_DOWN);
        break;
      case ActionScrollUp:
        UpdateScroll(SCROLL_SCROLL_UP);
        break;
      case ActionNone:
        break;
      default:
        panic("REFIT_MENU_SCREEN::RunGenericMenu");
        break;
    }

    // read key press (and wait for it if applicable)
    Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &key);
    if ((Status == EFI_NOT_READY) && (mAction == ActionNone)) {
      continue;
    }
    if (mAction == ActionNone) {
      ReadAllKeyStrokes(); //clean to avoid doubles
    }
    if (HaveTimeout) {
      // the user pressed a key, cancel the timeout
      TextMenuStyle(MENU_FUNCTION_PAINT_TIMEOUT, L"");
      HaveTimeout = FALSE;
    }

    mAction = ActionNone; //do action once
    // react to key press
    switch (key.ScanCode) {
      case SCAN_UP:
      case SCAN_LEFT:
        UpdateScroll(SCROLL_LINE_UP);
        break;
      case SCAN_DOWN:
      case SCAN_RIGHT:
        UpdateScroll(SCROLL_LINE_DOWN);
        break;
      case SCAN_HOME:
        UpdateScroll(SCROLL_FIRST);
        break;
      case SCAN_END:
        UpdateScroll(SCROLL_LAST);
        break;
      case SCAN_PAGE_UP:
        UpdateScroll(SCROLL_PAGE_UP);
        break;
      case SCAN_PAGE_DOWN:
        UpdateScroll(SCROLL_PAGE_DOWN);
        break;
      case SCAN_ESC:
        MenuExit = MENU_EXIT_ESCAPE;
        break;
      case SCAN_INSERT:
        MenuExit = MENU_EXIT_OPTIONS;
        break;

      case SCAN_F1:
        MenuExit = MENU_EXIT_HELP;
        break;
//      case SCAN_F2:
//        GlobalConfig.SavePreBootLog = true;
//        //let it be twice
//        Status = SaveBooterLog(&self.getCloverDir(), PREBOOT_LOG);
//        if (EFI_ERROR(Status)) {
//          Status = SaveBooterLog(NULL, PREBOOT_LOG);
//        }
//        break;
      case SCAN_F3:
         MenuExit = MENU_EXIT_HIDE_TOGGLE;
         break;
        break;
/* just a sample code
      case SCAN_F7:
        Status = egMkDir(SelfRootDir,  L"EFI\\CLOVER\\new_folder");
        DBG("create folder %s\n", efiStrError(Status));
        if (!EFI_ERROR(Status)) {
          Status = egSaveFile(SelfRootDir,  L"EFI\\CLOVER\\new_folder\\new_file.txt", (UINT8*)SomeText, sizeof(*SomeText)+1);
          DBG("create file %s\n", efiStrError(Status));
        }
        break;
*/
      case SCAN_F12:
        MenuExit = MENU_EXIT_EJECT;
        ScrollState.PaintAll = true;
        break;

    }
    switch (key.UnicodeChar) {
      case CHAR_LINEFEED:
      case CHAR_CARRIAGE_RETURN:
        if ((Entries[ScrollState.CurrentSelection].getREFIT_INPUT_DIALOG()) ||
            (Entries[ScrollState.CurrentSelection].getREFIT_MENU_CHECKBIT())) {
          MenuExit = InputDialog();
        } else if (Entries[ScrollState.CurrentSelection].getREFIT_MENU_SWITCH()){
          MenuExit = InputDialog();
          ScrollState.PaintAll = true;
        } else if (Entries[ScrollState.CurrentSelection].getREFIT_MENU_ENTRY_CLOVER()){
          MenuExit = MENU_EXIT_DETAILS;
        } else if (!Entries[ScrollState.CurrentSelection].getREFIT_INFO_DIALOG()) {
          MenuExit = MENU_EXIT_ENTER;
        }
        break;
      case ' ': //CHAR_SPACE
        if ((Entries[ScrollState.CurrentSelection].getREFIT_INPUT_DIALOG()) ||
            (Entries[ScrollState.CurrentSelection].getREFIT_MENU_CHECKBIT())) {
          MenuExit = InputDialog();
        } else if (Entries[ScrollState.CurrentSelection].getREFIT_MENU_SWITCH()){
          MenuExit = InputDialog();
          ScrollState.PaintAll = true;
        } else if (!Entries[ScrollState.CurrentSelection].getREFIT_INFO_DIALOG()) {
          MenuExit = MENU_EXIT_DETAILS;
        }
        break;

      default:
        ShortcutEntry = FindMenuShortcutEntry(key.UnicodeChar);
        if (ShortcutEntry >= 0) {
          ScrollState.CurrentSelection = ShortcutEntry;
          MenuExit = MENU_EXIT_ENTER;
        }
        break;
    }
  }

  TextMenuStyle(MENU_FUNCTION_CLEANUP, NULL);

	if (ChosenEntry) {
    *ChosenEntry = &Entries[ScrollState.CurrentSelection];
	}

  *DefaultEntryIndex = ScrollState.CurrentSelection;

  return MenuExit;
}

/**
 * Text Mode menu.
 */
VOID REFIT_MENU_SCREEN::TextMenuStyle(IN UINTN Function, IN CONST CHAR16 *ParamText)
{
  INTN i = 0;
  static UINTN TextMenuWidth = 0,ItemWidth = 0, MenuHeight = 0;
  static UINTN MenuPosY = 0;
  //static CHAR16 **DisplayStrings;
  XStringW ResultString;

  switch (Function) {

    case MENU_FUNCTION_INIT:
      // vertical layout
      MenuPosY = 4;

	  if (InfoLines.size() > 0) {
        MenuPosY += InfoLines.size() + 1;
	  }

      MenuHeight = ConHeight - MenuPosY;

	  if (TimeoutSeconds > 0) {
        MenuHeight -= 2;
	  }

      InitScroll(Entries.size(), Entries.size(), MenuHeight, 0);

      // determine width of the menu
      TextMenuWidth = 50;  // minimum
      for (i = 0; i <= ScrollState.MaxIndex; i++) {
        ItemWidth = Entries[i].Title.length();

		if (TextMenuWidth < ItemWidth) {
          TextMenuWidth = ItemWidth;
        }
      }

	  if (TextMenuWidth > ConWidth - 6) {
        TextMenuWidth = ConWidth - 6;
	  }
	  MenuWidth = TextMenuWidth;
    
    break;

    case MENU_FUNCTION_CLEANUP:
      // release temporary memory

			// reset default output colours
	  gST->ConOut->SetAttribute(gST->ConOut, ATTR_BANNER);

      break;

    case MENU_FUNCTION_PAINT_ALL:
        // paint the whole screen (initially and after scrolling)
      gST->ConOut->SetAttribute (gST->ConOut, ATTR_CHOICE_BASIC);
      for (i = 0; i < (INTN)ConHeight - 4; i++) {
        gST->ConOut->SetCursorPosition(gST->ConOut, 0, 4 + i);
        gST->ConOut->OutputString(gST->ConOut, BlankLine);
      }

      BeginTextScreen(Title.wc_str());

      if (InfoLines.size() > 0) {
        gST->ConOut->SetAttribute (gST->ConOut, ATTR_BASIC);

        for (i = 0; i < (INTN)InfoLines.size(); i++) {
          gST->ConOut->SetCursorPosition (gST->ConOut, 3, 4 + i);
          gST->ConOut->OutputString (gST->ConOut, InfoLines[i].wc_str());
        }
      }

      for (i = ScrollState.FirstVisible; i <= ScrollState.LastVisible && i <= ScrollState.MaxIndex; i++) {
        gST->ConOut->SetCursorPosition (gST->ConOut, 2, MenuPosY + (i - ScrollState.FirstVisible));

        if (i == ScrollState.CurrentSelection) {
          gST->ConOut->SetAttribute (gST->ConOut, ATTR_CHOICE_CURRENT);
        } else {
          gST->ConOut->SetAttribute (gST->ConOut, ATTR_CHOICE_BASIC);
        }

        ResultString.takeValueFrom(Entries[i].Title);

        if (Entries[i].getREFIT_INPUT_DIALOG()) {
          REFIT_INPUT_DIALOG& entry = (REFIT_INPUT_DIALOG&) Entries[i];
          if (entry.getREFIT_INPUT_DIALOG()) {
            if ((entry).Item.ItemType == BoolValue) {
              ResultString += (entry).Item.BValue ? L": [+]" : L": [ ]";
            } else {
              ResultString += (entry).Item.SValue;
            }
          } else if (entry.getREFIT_MENU_CHECKBIT()) {
            // check boxes
            ResultString += ((entry).Item.IValue & (entry.Row)) ? L": [+]" : L": [ ]";
          } else if (entry.getREFIT_MENU_SWITCH()) {
            // radio buttons
          }
        }

        for ( size_t j = ResultString.length() ; j < TextMenuWidth; j++ ) {
          ResultString += L' ';
        }

        gST->ConOut->OutputString(gST->ConOut, ResultString.wc_str());
      }

      // scrolling indicators
      gST->ConOut->SetAttribute (gST->ConOut, ATTR_SCROLLARROW);
      gST->ConOut->SetCursorPosition (gST->ConOut, 0, MenuPosY);

      if (ScrollState.FirstVisible > 0) {
          gST->ConOut->OutputString (gST->ConOut, ArrowUp);
      } else {
          gST->ConOut->OutputString (gST->ConOut, L" ");
      }

      gST->ConOut->SetCursorPosition (gST->ConOut, 0, MenuPosY + ScrollState.MaxVisible);

      if (ScrollState.LastVisible < ScrollState.MaxIndex) {
          gST->ConOut->OutputString (gST->ConOut, ArrowDown);
      } else {
          gST->ConOut->OutputString (gST->ConOut, L" ");
      }

      break;

    case MENU_FUNCTION_PAINT_SELECTION:
			// last selection
      // redraw selection cursor
      gST->ConOut->SetCursorPosition (gST->ConOut, 2, MenuPosY + (ScrollState.LastSelection - ScrollState.FirstVisible));
      gST->ConOut->SetAttribute (gST->ConOut, ATTR_CHOICE_BASIC);
      //gST->ConOut->OutputString (gST->ConOut, DisplayStrings[ScrollState.LastSelection]);
			ResultString = Entries[ScrollState.LastSelection].Title;
      if (Entries[ScrollState.LastSelection].getREFIT_INPUT_DIALOG()) {
        REFIT_INPUT_DIALOG& entry = (REFIT_INPUT_DIALOG&) Entries[ScrollState.LastSelection];
        if (entry.getREFIT_INPUT_DIALOG()) {
          if (entry.Item.ItemType == BoolValue) {
            ResultString += entry.Item.BValue ? L": [+]" : L": [ ]";
          } else {
            ResultString += entry.Item.SValue;
          }
        } else if (entry.getREFIT_MENU_CHECKBIT()) {
          // check boxes
          ResultString += (entry.Item.IValue & (entry.Row)) ? L": [+]" : L": [ ]";
        } else if (entry.getREFIT_MENU_SWITCH()) {
          // radio buttons
        }
      }

      for (size_t j = ResultString.length(); j < TextMenuWidth; j++) {
        ResultString += L' ';
      }

      gST->ConOut->OutputString (gST->ConOut, ResultString.wc_str());

        // current selection
      gST->ConOut->SetCursorPosition (gST->ConOut, 2, MenuPosY + (ScrollState.CurrentSelection - ScrollState.FirstVisible));
      gST->ConOut->SetAttribute (gST->ConOut, ATTR_CHOICE_CURRENT);
      ResultString = Entries[ScrollState.CurrentSelection].Title;
      if (Entries[ScrollState.CurrentSelection].getREFIT_INPUT_DIALOG()) {
        REFIT_INPUT_DIALOG& entry = (REFIT_INPUT_DIALOG&) Entries[ScrollState.CurrentSelection];
        if (entry.getREFIT_INPUT_DIALOG()) {
          if (entry.Item.ItemType == BoolValue) {
            ResultString += entry.Item.BValue ? L": [+]" : L": [ ]";
          } else {
            ResultString += entry.Item.SValue;
          }
        } else if (entry.getREFIT_MENU_CHECKBIT()) {
          // check boxes
          ResultString += (entry.Item.IValue & (entry.Row)) ? L": [+]" : L": [ ]";
        } else if (entry.getREFIT_MENU_SWITCH()) {
          // radio buttons
        }
      }

      for (size_t j = ResultString.length() ; j < TextMenuWidth; j++) {
        ResultString += L' ';
      }

      gST->ConOut->OutputString (gST->ConOut, ResultString.wc_str());
        //gST->ConOut->OutputString (gST->ConOut, DisplayStrings[ScrollState.CurrentSelection]);

      break;

    case MENU_FUNCTION_PAINT_TIMEOUT:
      if (ParamText[0] == 0) {
        // clear message
        gST->ConOut->SetAttribute (gST->ConOut, ATTR_BASIC);
        gST->ConOut->SetCursorPosition (gST->ConOut, 0, ConHeight - 1);
        gST->ConOut->OutputString (gST->ConOut, BlankLine + 1);
      } else {
        // paint or update message
        gST->ConOut->SetAttribute (gST->ConOut, ATTR_ERROR);
        gST->ConOut->SetCursorPosition (gST->ConOut, 3, ConHeight - 1);
        XStringW TimeoutMessage = SWPrintf("%ls  ", ParamText);
        gST->ConOut->OutputString (gST->ConOut, TimeoutMessage.wc_str());
      }

      break;
  }
}

/**
 * Draw text with specific coordinates.
 */








UINTN REFIT_MENU_SCREEN::RunMenu(OUT REFIT_ABSTRACT_MENU_ENTRY **ChosenEntry)
{
  INTN Index = -1;

  return RunGenericMenu(&Index, ChosenEntry);
}


VOID REFIT_MENU_SCREEN::AddMenuCheck(CONST CHAR8 *Text, UINTN Bit)
{
  REFIT_MENU_CHECKBIT *InputBootArgs;

  InputBootArgs = new REFIT_MENU_CHECKBIT;
  InputBootArgs->Title.takeValueFrom(Text);
//  InputBootArgs->Tag = TAG_CHECKBIT_OLD;
  InputBootArgs->Row = Bit;
//  InputBootArgs->Item = &InputItems[ItemNum];
  InputBootArgs->AtClick = ActionEnter;
  InputBootArgs->AtRightClick = ActionDetails;
  AddMenuEntry(InputBootArgs, true);
}


VOID REFIT_MENU_SCREEN::AddMenuItem_(REFIT_MENU_ENTRY_ITEM_ABSTRACT* InputBootArgs, CONST CHAR8 *Line, BOOLEAN Cursor)
{
  InputBootArgs->Title.takeValueFrom(Line);
  InputBootArgs->Row          = 0;
  InputBootArgs->AtClick        = Cursor?ActionSelect:ActionEnter;
  InputBootArgs->AtRightClick   = Cursor?ActionNone:ActionDetails;
  InputBootArgs->AtDoubleClick  = Cursor?ActionEnter:ActionNone;

  AddMenuEntry(InputBootArgs, true);
}

REFIT_INPUT_DIALOG* REFIT_MENU_SCREEN::AddMenuItemInput(CONST CHAR8 *Line, BOOLEAN Cursor)
{
  REFIT_INPUT_DIALOG *InputBootArgs = new REFIT_INPUT_DIALOG;
  AddMenuItem_(InputBootArgs, Line, Cursor);
  return InputBootArgs;
}

REFIT_MENU_SWITCH* REFIT_MENU_SCREEN::AddMenuItemSwitch(CONST CHAR8 *Line, BOOLEAN Cursor)
{
  REFIT_MENU_SWITCH *InputBootArgs = new REFIT_MENU_SWITCH;
  AddMenuItem_(InputBootArgs, Line, Cursor);
  return InputBootArgs;
}


UINTN REFIT_MENU_SCREEN::RunMainMenu(IN INTN DefaultSelection, OUT REFIT_ABSTRACT_MENU_ENTRY **ChosenEntry)
{

  REFIT_ABSTRACT_MENU_ENTRY    *TempChosenEntry  = 0;
  REFIT_ABSTRACT_MENU_ENTRY    *MainChosenEntry  = 0;
  REFIT_ABSTRACT_MENU_ENTRY    *NextChosenEntry  = NULL;
  UINTN               MenuExit = 0, SubMenuExit = 0;
  INTN                DefaultEntryIndex = DefaultSelection;
  INTN                SubMenuIndex;

  // initialize static variables when menu runs so that values from previos sessions won't be used
  OldX = 0;
  OldY = 0;
  OldTextWidth = 0;
  OldTextHeight = 0;
  OldRow = 0;
  OldTimeoutTextWidth = 0;

  while (!MenuExit) {
    MenuExit = RunGenericMenu(&DefaultEntryIndex, &MainChosenEntry);
    TimeoutSeconds = 0;

    if (MenuExit == MENU_EXIT_DETAILS && MainChosenEntry->SubScreen != NULL) {

      SubMenuIndex = -1;
      SubMenuExit = 0;
      while (!SubMenuExit) {
        //
        //running details menu
        //
        SubMenuExit = MainChosenEntry->SubScreen->RunGenericMenu(&SubMenuIndex, &TempChosenEntry);

        if (SubMenuExit == MENU_EXIT_ESCAPE || TempChosenEntry->getREFIT_MENU_ITEM_RETURN() ) {
          SubMenuExit = MENU_EXIT_ENTER;
          MenuExit = 0;
          break;
        }

        if (MainChosenEntry->getREFIT_MENU_ENTRY_CLOVER()) {
          MainChosenEntry->getREFIT_MENU_ENTRY_CLOVER()->LoadOptions = (((REFIT_MENU_ENTRY_CLOVER*)TempChosenEntry)->LoadOptions);
        }
        
        if (SubMenuExit == MENU_EXIT_DETAILS) {
          SubMenuExit = 0;
          continue;
        }
 //       DBG(" exit menu with LoadOptions: %ls\n", ((LOADER_ENTRY*)MainChosenEntry)->LoadOptions);

        if (SubMenuExit == MENU_EXIT_ENTER && MainChosenEntry->getLOADER_ENTRY() && TempChosenEntry->getLOADER_ENTRY()) {
          // Only for non-legacy entries, as LEGACY_ENTRY doesn't have Flags/Options
          MainChosenEntry->getLOADER_ENTRY()->Flags = TempChosenEntry->getLOADER_ENTRY()->Flags;
          DBG(" get MainChosenEntry FlagsBits = 0x%X\n", ((LOADER_ENTRY*)MainChosenEntry)->Flags);
          // copy also loadoptions from subentry to mainentry
          MainChosenEntry->getLOADER_ENTRY()->LoadOptions = TempChosenEntry->getLOADER_ENTRY()->LoadOptions;
        }

        //---- Details submenu (kexts disabling etc)
        if (SubMenuExit == MENU_EXIT_ENTER /*|| MenuExit == MENU_EXIT_DETAILS*/) {
          if (TempChosenEntry->SubScreen != NULL) {
            UINTN NextMenuExit = 0;
            INTN NextEntryIndex = -1;
            while (!NextMenuExit) {
              //
              // running submenu
              //
              NextMenuExit = TempChosenEntry->SubScreen->RunGenericMenu(&NextEntryIndex, &NextChosenEntry);
              if (NextMenuExit == MENU_EXIT_ESCAPE || NextChosenEntry->getREFIT_MENU_ITEM_RETURN() ) {
                SubMenuExit = 0;
                NextMenuExit = MENU_EXIT_ENTER;
                break;
              }
              DBG(" get NextChosenEntry FlagsBits = 0x%X\n", ((LOADER_ENTRY*)NextChosenEntry)->Flags);
              //---- Details submenu (kexts disabling etc) second level
              if (NextMenuExit == MENU_EXIT_ENTER /*|| MenuExit == MENU_EXIT_DETAILS*/) {
                if (NextChosenEntry->SubScreen != NULL) {
                  UINTN DeepMenuExit = 0;
                  INTN DeepEntryIndex = -1;
                  REFIT_ABSTRACT_MENU_ENTRY    *DeepChosenEntry  = NULL;
                  while (!DeepMenuExit) {
                    //
                    // run deep submenu
                    //
                    DeepMenuExit = NextChosenEntry->SubScreen->RunGenericMenu(&DeepEntryIndex, &DeepChosenEntry);
                    if (DeepMenuExit == MENU_EXIT_ESCAPE || DeepChosenEntry->getREFIT_MENU_ITEM_RETURN() ) {
                      DeepMenuExit = MENU_EXIT_ENTER;
                      NextMenuExit = 0;
                      break;
                    }
                    DBG(" get DeepChosenEntry FlagsBits = 0x%X\n", ((LOADER_ENTRY*)DeepChosenEntry)->Flags);
                  } //while(!DeepMenuExit)
                }
              }

            } //while(!NextMenuExit)
          }
        }
        //---------
      }
    }
  }

  if (ChosenEntry) {
    *ChosenEntry = MainChosenEntry;
  }
  return MenuExit;
}


long long int strtoll(const wchar_t *c)
{
    long long int value = 0;
    int sign = 1;

    while (*c == ' ') c++;

    if( *c == '+' || *c == '-' )
    {
        if( *c == '-' ) sign = -1;
        c++;
    }
    while (*c == ' ') c++;

    while (IS_DIGIT(*c))
    {
        value *= 10;
        value += (int) (*c-'0');
        c++;
    }
    return (value * sign);
}

void REFIT_MENU_SCREEN::ApplyInputs()
{
//  DBG("ApplyInputs\n");

  for ( size_t i = 0 ; i < MainMenu.Entries.size() ; i++ ) {
    const LOADER_ENTRY* lePtr = MainMenu.Entries[i].getLOADER_ENTRY();
    if ( lePtr ) {
      const LOADER_ENTRY& le = *lePtr;
      if ( le.DevicePathString.isEqualIC(gSettings.DefaultVolume) ) {
        le.SubScreen->Entries["Default"]->getREFIT_INPUT_DIALOG()->Item.BValue = false;
      }
    }
  }

  if ( menuEntry  &&  menuEntry->getLOADER_ENTRY() ) {
    REFIT_ABSTRACT_MENU_ENTRY* abstractItemMakeDefault  = Entries["Default"];
    REFIT_INPUT_DIALOG* itemMakeDefault = abstractItemMakeDefault->getREFIT_INPUT_DIALOG();
    if ( itemMakeDefault->Item.Valid ) {
      gSettings.DefaultVolume = FileDevicePathToXStringW(menuEntry->getLOADER_ENTRY()->DevicePath);
      savePreferencesFile();
  DBG("ApplyInputs\n");
    }
  }

  REFIT_ABSTRACT_MENU_ENTRY* abstractItemTimeOut  = Entries["Timeout: "];
  if ( abstractItemTimeOut ) {
    REFIT_INPUT_DIALOG* itemTimeOut = abstractItemTimeOut->getREFIT_INPUT_DIALOG();
    gSettings.GUITimeOut = strtoll(itemTimeOut->Item.SValue.wc_str());
  }

  REFIT_ABSTRACT_MENU_ENTRY* abstractItemSaveLog  = Entries["Save debug log"];
  if ( abstractItemSaveLog ) {
    REFIT_INPUT_DIALOG* itemSaveLog = abstractItemSaveLog->getREFIT_INPUT_DIALOG();
    gSettings.SaveDebugLogToDisk = itemSaveLog->Item.BValue;
  }

}
