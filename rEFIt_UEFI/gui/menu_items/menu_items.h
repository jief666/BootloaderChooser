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

#ifndef __menu_items_H__
#define __menu_items_H__


#include "../../refit/lib.h"
//#include "../../refit/menu.h"
//#include <UefiLoader.h>

#include "../../cpp_foundation/XObjArray.h"
#include "../../cpp_foundation/XStringArray.h"
#include "../../cpp_foundation/XString.h"
#include "../../libeg/libscreen.h"


/* types */

//typedef enum {
//  ActionNone = 0,
//  ActionHelp,
//  ActionSelect,
//  ActionEnter,
//  ActionDeselect,
//  ActionDestroy,
//  ActionOptions,
//  ActionDetails,
//  ActionFinish,
//  ActionScrollDown,
//  ActionScrollUp,
////  ActionMoveScrollbar,
//  ActionPageDown,
//  ActionPageUp,
//  ActionLight
//} ACTION;

//typedef enum {
//  none,
//  BoolValue,
//  Decimal,
//  Hex,
//  ASString,
//  UNIString,
//  RadioSwitch,
//  CheckBit,
//} ITEM_TYPE;


//class INPUT_ITEM {
//public:
//  ITEM_TYPE ItemType; //string, value, boolean
//  BOOLEAN Valid;
//  BOOLEAN BValue;
//  UINT8   Pad8;
//  UINT32  IValue;
//  //  UINT64  UValue;
//  //  CHAR8*  AValue;
//  XStringW SValue;
//  UINTN   LineShift;
//
//  INPUT_ITEM() : ItemType(none), Valid(0), BValue(0), Pad8(0), IValue(0), SValue(0), LineShift(0) {};
//  INPUT_ITEM(const INPUT_ITEM& other) = default; // default is fine if there is only native type and objects that have copy ctor
//  INPUT_ITEM& operator = ( const INPUT_ITEM & ) = default; // default is fine if there is only native type and objects that have copy ctor
//  ~INPUT_ITEM() { }
//};


class REFIT_MENU_SCREEN;
class REFIT_MENU_SWITCH;
class REFIT_MENU_CHECKBIT;
class REFIT_MENU_ENTRY_CLOVER;
class REFIT_MENU_ITEM_RETURN;
class REFIT_INPUT_DIALOG;
class REFIT_INFO_DIALOG;
class REFIT_MENU_ENTRY_LOADER_TOOL;
class REFIT_MENU_ITEM_SHUTDOWN;
class REFIT_MENU_ITEM_RESET;
class REFIT_MENU_ITEM_ABOUT;
class REFIT_MENU_ITEM_OPTIONS;
class REFIT_MENU_ITEM_ABSTRACT_ENTRY_LOADER;
class LOADER_ENTRY;
class REFIT_MENU_ENTRY_OTHER;
class REFIT_SIMPLE_MENU_ENTRY_TAG;
class REFIT_MENU_ENTRY_ITEM_ABSTRACT;
class REFIT_MENU_ITEM_BOOTNUM;

/**********************************************************  REFIT_ABSTRACT_MENU_ENTRY  *************************************************************/

class REFIT_ABSTRACT_MENU_ENTRY
{
  public:
  XStringW           Title;
  bool               Hidden;
  UINTN              Row;
  CHAR16             ShortcutDigit;
  CHAR16             ShortcutLetter;
  ACTION             AtClick;
  ACTION             AtDoubleClick;
  ACTION             AtRightClick;
  ACTION             AtMouseOver;
  REFIT_MENU_SCREEN *SubScreen;

  virtual REFIT_SIMPLE_MENU_ENTRY_TAG* getREFIT_SIMPLE_MENU_ENTRY_TAG() { return nullptr; };
  virtual const REFIT_MENU_SWITCH* getREFIT_MENU_SWITCH() const { return nullptr; };
  virtual REFIT_MENU_SWITCH* getREFIT_MENU_SWITCH() { return nullptr; };
  virtual REFIT_MENU_CHECKBIT* getREFIT_MENU_CHECKBIT() { return nullptr; };
  virtual REFIT_MENU_ENTRY_CLOVER* getREFIT_MENU_ENTRY_CLOVER() { return nullptr; };
  virtual REFIT_MENU_ITEM_RETURN* getREFIT_MENU_ITEM_RETURN() { return nullptr; };
  virtual const REFIT_INPUT_DIALOG* getREFIT_INPUT_DIALOG() const { return nullptr; };
  virtual REFIT_INPUT_DIALOG* getREFIT_INPUT_DIALOG() { return nullptr; };
  virtual REFIT_INFO_DIALOG* getREFIT_INFO_DIALOG() { return nullptr; };
  virtual REFIT_MENU_ENTRY_LOADER_TOOL* getREFIT_MENU_ENTRY_LOADER_TOOL() { return nullptr; };
  virtual REFIT_MENU_ITEM_SHUTDOWN* getREFIT_MENU_ITEM_SHUTDOWN() { return nullptr; };
  virtual REFIT_MENU_ITEM_RESET* getREFIT_MENU_ITEM_RESET() { return nullptr; };
  virtual REFIT_MENU_ITEM_ABOUT* getREFIT_MENU_ITEM_ABOUT() { return nullptr; };
  virtual REFIT_MENU_ITEM_OPTIONS* getREFIT_MENU_ITEM_OPTIONS() { return nullptr; };
  virtual REFIT_MENU_ITEM_ABSTRACT_ENTRY_LOADER* getREFIT_MENU_ITEM_ABSTRACT_ENTRY_LOADER() { return nullptr; };
  virtual LOADER_ENTRY* getLOADER_ENTRY() { return nullptr; };
  virtual REFIT_MENU_ENTRY_OTHER* getREFIT_MENU_ENTRY_OTHER() { return nullptr; };
  virtual REFIT_MENU_ENTRY_ITEM_ABSTRACT* getREFIT_MENU_ITEM_IEM_ABSTRACT() { return nullptr; };
  virtual REFIT_MENU_ITEM_BOOTNUM* getREFIT_MENU_ITEM_BOOTNUM() { return nullptr; };
  virtual EFI_STATUS StartLoader() { return EFI_UNSUPPORTED; };

  REFIT_ABSTRACT_MENU_ENTRY()
      : Title(), Hidden(0), Row(0), ShortcutDigit(0), ShortcutLetter(0), AtClick(ActionNone), AtDoubleClick(ActionNone), AtRightClick(ActionNone), AtMouseOver(ActionNone), SubScreen(NULL)
      {};
  REFIT_ABSTRACT_MENU_ENTRY(const XStringW& Title_)
      : Title(Title_), Hidden(0), Row(0), ShortcutDigit(0), ShortcutLetter(0), AtClick(ActionNone), AtDoubleClick(ActionNone), AtRightClick(ActionNone), AtMouseOver(ActionNone), SubScreen(NULL)
      {};
  REFIT_ABSTRACT_MENU_ENTRY(const XStringW& Title_, ACTION AtClick_)
      : Title(Title_), Hidden(0), Row(0), ShortcutDigit(0), ShortcutLetter(0), AtClick(AtClick_), AtDoubleClick(ActionNone), AtRightClick(ActionNone), AtMouseOver(ActionNone), SubScreen(NULL)
      {};
  REFIT_ABSTRACT_MENU_ENTRY(const XStringW& Title_, UINTN Row_, CHAR16 ShortcutDigit_, CHAR16 ShortcutLetter_, ACTION AtClick_)
      : Title(Title_), Hidden(0), Row(Row_), ShortcutDigit(ShortcutDigit_), ShortcutLetter(ShortcutLetter_), AtClick(AtClick_), AtDoubleClick(ActionNone), AtRightClick(ActionNone), AtMouseOver(ActionNone), SubScreen(NULL)
      {};
//  REFIT_ABSTRACT_MENU_ENTRY(const XStringW& Title_, UINTN Row_,
//                            CHAR16 ShortcutDigit_, CHAR16 ShortcutLetter_, const XIcon& Icon_,
//                            EG_RECT Place_, ACTION AtClick_, ACTION AtDoubleClick_, ACTION AtRightClick_, ACTION AtMouseOver_,
//                            REFIT_MENU_SCREEN *SubScreen_)
//              : Title(Title_), Row(Row_), ShortcutDigit(ShortcutDigit_), ShortcutLetter(ShortcutLetter_),
//                Image(Icon_), Place(Place_),
//                AtClick(AtClick_), AtDoubleClick(AtDoubleClick_), AtRightClick(AtRightClick_), AtMouseOver(AtMouseOver_),
//                SubScreen(SubScreen_)
//              {};

  REFIT_ABSTRACT_MENU_ENTRY(const REFIT_ABSTRACT_MENU_ENTRY&) = delete;
  REFIT_ABSTRACT_MENU_ENTRY& operator=(const REFIT_ABSTRACT_MENU_ENTRY&) = delete;

  virtual ~REFIT_ABSTRACT_MENU_ENTRY() {}; // virtual destructor : this is vital
};



/**********************************************************  REFIT_ABSTRACT_MENU_ENTRY  *************************************************************/

	class REFIT_SIMPLE_MENU_ENTRY_TAG : public REFIT_ABSTRACT_MENU_ENTRY
	{
	public:
	  UINTN              Tag;

	  REFIT_SIMPLE_MENU_ENTRY_TAG(const XStringW& Title_, UINTN Tag_, ACTION AtClick_)
				 : REFIT_ABSTRACT_MENU_ENTRY(Title_, AtClick_), Tag(Tag_)
				 {};

	  virtual REFIT_SIMPLE_MENU_ENTRY_TAG* getREFIT_SIMPLE_MENU_ENTRY_TAG() { return this; };
	};



/**********************************************************  Simple entries. Inherit from REFIT_ABSTRACT_MENU_ENTRY  *************************************************************/

	class REFIT_MENU_ITEM_RETURN : public REFIT_ABSTRACT_MENU_ENTRY
	{
	public:
	  REFIT_MENU_ITEM_RETURN() : REFIT_ABSTRACT_MENU_ENTRY() {};
	  REFIT_MENU_ITEM_RETURN(const XStringW& Title_, UINTN Row_, CHAR16 ShortcutDigit_, CHAR16 ShortcutLetter_, ACTION AtClick_)
				 : REFIT_ABSTRACT_MENU_ENTRY(Title_, Row_, ShortcutDigit_, ShortcutLetter_, AtClick_)
				 {};
	  virtual REFIT_MENU_ITEM_RETURN* getREFIT_MENU_ITEM_RETURN() { return this; };
	};

	class REFIT_MENU_ITEM_SHUTDOWN : public REFIT_ABSTRACT_MENU_ENTRY
	{
	public:
	  REFIT_MENU_ITEM_SHUTDOWN() : REFIT_ABSTRACT_MENU_ENTRY() {};
	  REFIT_MENU_ITEM_SHUTDOWN(const XStringW& Title_, UINTN Row_, CHAR16 ShortcutDigit_, CHAR16 ShortcutLetter_, ACTION AtClick_)
				 : REFIT_ABSTRACT_MENU_ENTRY(Title_, Row_, ShortcutDigit_, ShortcutLetter_, AtClick_)
				 {};
	  virtual REFIT_MENU_ITEM_SHUTDOWN* getREFIT_MENU_ITEM_SHUTDOWN() { return this; };
	};

	class REFIT_MENU_ITEM_RESET : public REFIT_ABSTRACT_MENU_ENTRY {
	public:
	  REFIT_MENU_ITEM_RESET() : REFIT_ABSTRACT_MENU_ENTRY() {};
	  REFIT_MENU_ITEM_RESET(const XStringW& Title_, UINTN Row_, CHAR16 ShortcutDigit_, CHAR16 ShortcutLetter_, ACTION AtClick_)
				 : REFIT_ABSTRACT_MENU_ENTRY(Title_, Row_, ShortcutDigit_, ShortcutLetter_, AtClick_)
				 {};
	  virtual REFIT_MENU_ITEM_RESET* getREFIT_MENU_ITEM_RESET() { return this; };
	};

	class REFIT_MENU_ITEM_ABOUT : public REFIT_ABSTRACT_MENU_ENTRY
	{
	public:
	  REFIT_MENU_ITEM_ABOUT() : REFIT_ABSTRACT_MENU_ENTRY() {};
	  REFIT_MENU_ITEM_ABOUT(const XStringW& Title_, UINTN Row_, CHAR16 ShortcutDigit_, CHAR16 ShortcutLetter_, ACTION AtClick_)
				 : REFIT_ABSTRACT_MENU_ENTRY(Title_, Row_, ShortcutDigit_, ShortcutLetter_, AtClick_)
				 {};
	  virtual REFIT_MENU_ITEM_ABOUT* getREFIT_MENU_ITEM_ABOUT() { return this; };
	};

	class REFIT_MENU_ITEM_OPTIONS : public REFIT_ABSTRACT_MENU_ENTRY {
	public:
	  REFIT_MENU_ITEM_OPTIONS() : REFIT_ABSTRACT_MENU_ENTRY() {};
	  REFIT_MENU_ITEM_OPTIONS(const XStringW& Title_, UINTN Row_, CHAR16 ShortcutDigit_, CHAR16 ShortcutLetter_, ACTION AtClick_)
				 : REFIT_ABSTRACT_MENU_ENTRY(Title_, Row_, ShortcutDigit_, ShortcutLetter_, AtClick_)
				 {};
	  virtual REFIT_MENU_ITEM_OPTIONS* getREFIT_MENU_ITEM_OPTIONS() { return this; };
	};


	class REFIT_INFO_DIALOG : public REFIT_ABSTRACT_MENU_ENTRY {
	public:
	  virtual REFIT_INFO_DIALOG* getREFIT_INFO_DIALOG() { return this; };
	};



/**********************************************************    *************************************************************/

	class REFIT_MENU_ENTRY_ITEM_ABSTRACT : public REFIT_ABSTRACT_MENU_ENTRY {
	public:
	  INPUT_ITEM Item;
	  REFIT_MENU_ENTRY_ITEM_ABSTRACT() : Item() {}
    REFIT_MENU_ENTRY_ITEM_ABSTRACT(const REFIT_MENU_ENTRY_ITEM_ABSTRACT&) = delete;
    REFIT_MENU_ENTRY_ITEM_ABSTRACT& operator=(const REFIT_MENU_ENTRY_ITEM_ABSTRACT&) = delete;
	  virtual REFIT_MENU_ENTRY_ITEM_ABSTRACT* getREFIT_MENU_ITEM_IEM_ABSTRACT() { return this; };
	};

		/* Classes that needs a Item member */
		class REFIT_INPUT_DIALOG : public REFIT_MENU_ENTRY_ITEM_ABSTRACT {
		public:
      virtual const REFIT_INPUT_DIALOG* getREFIT_INPUT_DIALOG() const { return this; };
      virtual REFIT_INPUT_DIALOG* getREFIT_INPUT_DIALOG() { return this; };
		};

		class REFIT_MENU_SWITCH : public REFIT_MENU_ENTRY_ITEM_ABSTRACT {
		public:
      virtual const REFIT_MENU_SWITCH* getREFIT_MENU_SWITCH() const { return this; };
      virtual REFIT_MENU_SWITCH* getREFIT_MENU_SWITCH() { return this; };
		};

		class REFIT_MENU_CHECKBIT : public REFIT_MENU_ENTRY_ITEM_ABSTRACT {
		public:
		  virtual REFIT_MENU_CHECKBIT* getREFIT_MENU_CHECKBIT() { return this; };
		};

/**********************************************************  Loader entries  *************************************************************/
	/*
	 * Super class of LOADER_ENTRY & LEGACY_ENTRY
	 */
	class REFIT_MENU_ITEM_ABSTRACT_ENTRY_LOADER : public REFIT_ABSTRACT_MENU_ENTRY
	{
	public:
	  XStringW     DevicePathString;
	  XString8Array LoadOptions; //moved here for compatibility with legacy
	  XStringW     LoaderPath;

    REFIT_MENU_ITEM_ABSTRACT_ENTRY_LOADER()
        : REFIT_ABSTRACT_MENU_ENTRY(), DevicePathString(0), LoadOptions(), LoaderPath()
        {}
    REFIT_MENU_ITEM_ABSTRACT_ENTRY_LOADER(const REFIT_MENU_ITEM_ABSTRACT_ENTRY_LOADER&) = delete;
    REFIT_MENU_ITEM_ABSTRACT_ENTRY_LOADER& operator=(const REFIT_MENU_ITEM_ABSTRACT_ENTRY_LOADER&) = delete;

	  virtual REFIT_MENU_ITEM_ABSTRACT_ENTRY_LOADER* getREFIT_MENU_ITEM_ABSTRACT_ENTRY_LOADER() { return this; };
	};



		//---------------------------------------  REFIT_MENU_ENTRY_CLOVER  ---------------------------------------//

		class REFIT_MENU_ENTRY_CLOVER : public REFIT_MENU_ITEM_ABSTRACT_ENTRY_LOADER
		{
		  public:
			REFIT_VOLUME     *Volume;
			XStringW         VolName;
			EFI_DEVICE_PATH  *DevicePath;
			UINT16            Flags;

			REFIT_MENU_ENTRY_CLOVER() : REFIT_MENU_ITEM_ABSTRACT_ENTRY_LOADER(), Volume(NULL), VolName(), DevicePath(NULL), Flags(0)  {};
      REFIT_MENU_ENTRY_CLOVER(const REFIT_MENU_ENTRY_CLOVER&) = delete;
      REFIT_MENU_ENTRY_CLOVER& operator=(const REFIT_MENU_ENTRY_CLOVER&) = delete;

			REFIT_MENU_ENTRY_CLOVER* getPartiallyDuplicatedEntry() const;
			virtual REFIT_MENU_ENTRY_CLOVER* getREFIT_MENU_ENTRY_CLOVER() { return this; };
		};


		//---------------------------------------  REFIT_MENU_ITEM_BOOTNUM  ---------------------------------------//

		class REFIT_MENU_ITEM_BOOTNUM : public REFIT_MENU_ITEM_ABSTRACT_ENTRY_LOADER
		{
		  public:
			REFIT_VOLUME     *Volume;
			UINTN             BootNum;

			REFIT_MENU_ITEM_BOOTNUM() : REFIT_MENU_ITEM_ABSTRACT_ENTRY_LOADER(), Volume(NULL), BootNum(0) {};
      REFIT_MENU_ITEM_BOOTNUM(const REFIT_MENU_ITEM_BOOTNUM&) = delete;
      REFIT_MENU_ITEM_BOOTNUM& operator=(const REFIT_MENU_ITEM_BOOTNUM&) = delete;
			virtual REFIT_MENU_ITEM_BOOTNUM* getREFIT_MENU_ITEM_BOOTNUM() { return this; };
		} ;


			//---------------------------------------  LOADER_ENTRY  ---------------------------------------//

			class LOADER_ENTRY : public REFIT_MENU_ITEM_BOOTNUM
			{
			  public:

				XStringW          DisplayedVolName;
				EFI_DEVICE_PATH  *DevicePath;
				UINT16            Flags;
				XString8          BuildVersion;
        EFI_GRAPHICS_OUTPUT_BLT_PIXEL BootBgColor;

				UINT8             CustomBoot;
				XStringW          Settings;
        
        

				LOADER_ENTRY()
						: REFIT_MENU_ITEM_BOOTNUM(), DisplayedVolName(), DevicePath(0), Flags(0), BuildVersion(),
              BootBgColor({0,0,0,0}),
              CustomBoot(0), Settings()
						{};
        LOADER_ENTRY(const LOADER_ENTRY&) = delete;
        LOADER_ENTRY& operator=(const LOADER_ENTRY&) = delete;
        
        
        EFI_STATUS StartLoader();
        void AddDefaultMenu();
				LOADER_ENTRY* getPartiallyDuplicatedEntry() const;
				virtual LOADER_ENTRY* getLOADER_ENTRY() { return this; };

};


#endif
/*
 
 EOF */
