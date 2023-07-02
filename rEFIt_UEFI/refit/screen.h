#include "../libeg/libeg.h"
#include "../libeg/libscreen.h"

void InitScreen();
void SetupScreen(void);
void BeginTextScreen(IN CONST CHAR16 *Title);
void FinishTextScreen(IN XBool WaitAlways);
void TerminateScreen(void);


//BOOLEAN CheckFatalError(IN EFI_STATUS Status, IN CONST CHAR16 *where);
//BOOLEAN CheckError(IN EFI_STATUS Status, IN CONST CHAR16 *where);
