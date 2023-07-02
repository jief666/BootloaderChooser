#ifndef __MENU_H__
#define __MENU_H__

#include "../cpp_foundation/XString.h"
#include "../gui/menu_items/menu_items.h"

extern INTN LayoutAnimMoveForMenuX;
extern INTN LayoutMainMenuHeight;

extern REFIT_MENU_SCREEN MainMenu;

extern VOID HelpRefit(VOID);
extern VOID AboutRefit(VOID);


VOID OptionsMenu(OUT REFIT_ABSTRACT_MENU_ENTRY **ChosenEntry);
VOID FreeScrollBar(VOID);

//VOID
//FillInputs ();



#endif

