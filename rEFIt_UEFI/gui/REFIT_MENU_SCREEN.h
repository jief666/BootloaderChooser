/*
 *
 * Copyright (c) 2020 Jief
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

#ifndef __REFIT_MENU_SCREEN_H__
#define __REFIT_MENU_SCREEN_H__


#include "../Platform/Settings.h"
#include "../refit/lib.h"


#include "../cpp_foundation/XObjArray.h"
#include "../cpp_foundation/XString.h"
#include "../cpp_foundation/XStringArray.h"
#include "menu_items/menu_items.h"


#ifdef _MSC_VER
#define __attribute__(x)
#endif

//some unreal values
#define FILM_CENTRE   40000
//#define FILM_LEFT     50000
//#define FILM_TOP      50000
//#define FILM_RIGHT    60000
//#define FILM_BOTTOM   60000
//#define FILM_PERCENT 100000
#define INITVALUE       40000



//typedef struct {
//  INTN    CurrentSelection, LastSelection;
//  INTN    MaxScroll, MaxIndex;
//  INTN    FirstVisible, LastVisible, MaxVisible, MaxFirstVisible;
//  BOOLEAN IsScrolling, PaintAll, PaintSelection;
//} SCROLL_STATE;


class REFIT_MENU_ENTRY_ITEM_ABSTRACT;
class REFIT_MENU_ENTRY;
class REFIT_ABSTRACT_MENU_ENTRY;

typedef VOID (REFIT_MENU_SCREEN::*MENU_STYLE_FUNC)(IN UINTN Function, IN CONST CHAR16 *ParamText);

const REFIT_ABSTRACT_MENU_ENTRY nullREFIT_ABSTRACT_MENU_ENTRY;

class EntryArray : public XObjArray<REFIT_ABSTRACT_MENU_ENTRY>
{
public:
  bool includeHidden = false;

  size_t sizeIncludingHidden() const { return XObjArray<REFIT_ABSTRACT_MENU_ENTRY>::size(); }

  size_t size() const
  {
    if ( includeHidden ) return sizeIncludingHidden();
    size_t size = 0;
    for ( size_t i=0 ; i < XObjArray<REFIT_ABSTRACT_MENU_ENTRY>::size() ; i++ ) {
      const REFIT_ABSTRACT_MENU_ENTRY& entry = XObjArray<REFIT_ABSTRACT_MENU_ENTRY>::ElementAt(i);
      if ( !entry.Hidden ) {
        size++;
      }
    }
    return size;
  }
  size_t length() const { return size(); }

  template<typename IntegralType, enable_if(is_integral(IntegralType))>
  REFIT_ABSTRACT_MENU_ENTRY& operator[](IntegralType nIndex)
  {
    if ( includeHidden ) return XObjArray<REFIT_ABSTRACT_MENU_ENTRY>::operator [] (nIndex);
    if (nIndex < 0) {
      panic("EntryArray::operator[] : i < 0. System halted\n");
    }
    size_t size = 0;
    for ( size_t i=0 ; i < XObjArray<REFIT_ABSTRACT_MENU_ENTRY>::size() ; i++ ) {
      if ( !XObjArray<REFIT_ABSTRACT_MENU_ENTRY>::ElementAt(i).Hidden ) {
        if ( size == (unsigned_type(IntegralType))nIndex ) return XObjArray<REFIT_ABSTRACT_MENU_ENTRY>::ElementAt(i);
        size++;
      }
    }
    panic("EntryArray::operator[] nIndex > size()");
  }

  template<typename IntegralType, enable_if(is_integral(IntegralType))>
  const REFIT_ABSTRACT_MENU_ENTRY& operator[](IntegralType nIndex) const { return (*(const_cast<EntryArray*>(this)))[nIndex]; }

//  template<typename IntegralType, enable_if(is_integral(IntegralType))>
//  const REFIT_ABSTRACT_MENU_ENTRY& operator[](IntegralType nIndex) const
//  {
//    if ( includeHidden ) return XObjArray<REFIT_ABSTRACT_MENU_ENTRY>::operator [] (nIndex);
//    if (nIndex < 0) {
//      panic("EntryArray::operator[] : i < 0. System halted\n");
//    }
//    size_t size = 0;
//    for ( size_t i=0 ; i < XObjArray<REFIT_ABSTRACT_MENU_ENTRY>::size() ; i++ ) {
//      if ( !XObjArray<REFIT_ABSTRACT_MENU_ENTRY>::ElementAt(i).Hidden ) {
//        if ( size == (unsigned_type(IntegralType))nIndex ) return XObjArray<REFIT_ABSTRACT_MENU_ENTRY>::ElementAt(i);
//        size++;
//      }
//    }
//    panic("EntryArray::operator[] nIndex > size()");
//  }


//  template<typename StringType, enable_if(is_char_ptr(StringType) || is___LString(StringType))>
  REFIT_ABSTRACT_MENU_ENTRY* operator[](const char* sIndex)
  {
    if ( sIndex == NULL || *sIndex == 0 ) {
      panic("EntryArray::operator[const char*] : sIndex == NULL || *sIndex == 0. System halted\n");
    }
    for ( size_t i=0 ; i < XObjArray<REFIT_ABSTRACT_MENU_ENTRY>::size() ; i++ ) {
      if ( !XObjArray<REFIT_ABSTRACT_MENU_ENTRY>::ElementAt(i).Hidden ) {
        if ( XObjArray<REFIT_ABSTRACT_MENU_ENTRY>::ElementAt(i).Title.isEqual(sIndex) ) {
          return &XObjArray<REFIT_ABSTRACT_MENU_ENTRY>::ElementAt(i);
        }
      }
    }
    return NULL;
  }

  const REFIT_ABSTRACT_MENU_ENTRY* operator[](const char* sIndex) const { return (*(const_cast<EntryArray*>(this)))[sIndex]; }

};

class REFIT_MENU_SCREEN
{
public:
  REFIT_ABSTRACT_MENU_ENTRY* menuEntry;
  UINTN             ID;
  XStringW          Title;
  XStringWArray     InfoLines;

  EntryArray        Entries;
  
  INTN              TimeoutSeconds;
  bool              Daylight;
  XStringW          TimeoutText;
  XStringW          ThemeName;  //?
  BOOLEAN           isBootScreen;

  ACTION          mAction;
  UINTN           mItemID;
  SCROLL_STATE    ScrollState;
  BOOLEAN         ScrollEnabled;
  INTN            TextStyle;
  BOOLEAN         IsDragging;



  REFIT_MENU_SCREEN(REFIT_ABSTRACT_MENU_ENTRY* _menuEntry)
      : menuEntry(_menuEntry), ID(0), Title(), InfoLines(), Entries(),
        TimeoutSeconds(0), Daylight(true), TimeoutText(), ThemeName(),
        isBootScreen(false),
        mAction(ActionNone), mItemID(0), ScrollState{0,0,0,0,0,0,0,0,false,false,false}, ScrollEnabled(0), TextStyle(0), IsDragging(0)
  {
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
  };

  REFIT_MENU_SCREEN(REFIT_ABSTRACT_MENU_ENTRY* _menuEntry, UINTN ID, XStringW TTitle, XStringW TTimeoutText)
      : menuEntry(_menuEntry), ID(ID), Title(TTitle), InfoLines(), Entries(),
        TimeoutSeconds(0), Daylight(true), TimeoutText(TTimeoutText), ThemeName(),
        isBootScreen(false),
        mAction(ActionNone), mItemID(0), ScrollState{0,0,0,0,0,0,0,0,false,false,false}, ScrollEnabled(0), TextStyle(0), IsDragging(0)
  {};

  //TODO exclude CHAR16
  REFIT_MENU_SCREEN(REFIT_ABSTRACT_MENU_ENTRY* _menuEntry, UINTN ID, CONST CHAR16* TitleC, CONST CHAR16* TimeoutTextC)
      : menuEntry(_menuEntry), ID(ID), Title(), InfoLines(), Entries(),
        TimeoutSeconds(0), Daylight(true), TimeoutText(), ThemeName(),
        isBootScreen(false),
        mAction(ActionNone), mItemID(0), ScrollState{0,0,0,0,0,0,0,0,false,false,false}, ScrollEnabled(0), TextStyle(0), IsDragging(0)
  {
    Title.takeValueFrom(TitleC);
    TimeoutText.takeValueFrom(TimeoutTextC);
  };

  REFIT_MENU_SCREEN(REFIT_ABSTRACT_MENU_ENTRY* _menuEntry, UINTN ID, XStringW  TTitle, XStringW  TTimeoutText, REFIT_ABSTRACT_MENU_ENTRY* entry1, REFIT_ABSTRACT_MENU_ENTRY* entry2)
      : menuEntry(_menuEntry), ID(ID), Title(TTitle), InfoLines(), Entries(),
        TimeoutSeconds(0), Daylight(true), TimeoutText(TTimeoutText), ThemeName(),
        isBootScreen(false),
        mAction(ActionNone), mItemID(0), ScrollState{0,0,0,0,0,0,0,0,false,false,false}, ScrollEnabled(0), TextStyle(0), IsDragging(0)
  {
    Entries.AddReference(entry1, false);
    Entries.AddReference(entry2, false);
  };

  REFIT_MENU_SCREEN(const REFIT_MENU_SCREEN&) = delete;
  REFIT_MENU_SCREEN& operator=(const REFIT_MENU_SCREEN&) = delete;


  //Scroll functions
  VOID InitScroll(IN INTN ItemCount, IN UINTN MaxCount,
                  IN UINTN VisibleSpace, IN INTN Selected);
  VOID UpdateScroll(IN UINTN Movement);

  //mouse functions and event
  EFI_STATUS WaitForInputEventPoll(UINTN TimeoutDefault);

  //menu functions
  VOID AddMenuItem_(REFIT_MENU_ENTRY_ITEM_ABSTRACT* InputBootArgs, CONST CHAR8 *Title, BOOLEAN Cursor);
  VOID AddMenuInfo_f(CONST char *format, ...) __attribute__((format(printf, 2, 3)));
  VOID AddMenuInfoLine_f(CONST char *format, ...) __attribute__((format(printf, 2, 3)));
  VOID AddMenuEntry(IN REFIT_ABSTRACT_MENU_ENTRY *Entry, bool freeIt);
  REFIT_MENU_SWITCH* AddMenuItemSwitch(CONST CHAR8 *Title, BOOLEAN Cursor);
  VOID AddMenuCheck(CONST CHAR8 *Text, UINTN Bit);
  REFIT_INPUT_DIALOG* AddMenuItemInput(CONST CHAR8 *Title, BOOLEAN Cursor);
  VOID FreeMenu();
  INTN FindMenuShortcutEntry(IN CHAR16 Shortcut);
  UINTN RunGenericMenu(IN OUT INTN *DefaultEntryIndex, OUT REFIT_ABSTRACT_MENU_ENTRY **ChosenEntry);
  UINTN RunMenu(OUT REFIT_ABSTRACT_MENU_ENTRY **ChosenEntry);
  UINTN RunMainMenu(IN INTN DefaultSelection, OUT REFIT_ABSTRACT_MENU_ENTRY **ChosenEntry);
  UINTN InputDialog();

  void ApplyInputs();


  //Style functions

  virtual VOID TextMenuStyle(IN UINTN Function, IN CONST CHAR16 *ParamText);

  virtual ~REFIT_MENU_SCREEN() {};
};

#endif
/*

 EOF */
