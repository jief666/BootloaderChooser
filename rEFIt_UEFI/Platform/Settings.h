#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#define CLOVER_SIGN             SIGNATURE_32('C','l','v','r')
#define HEIGHT_2K 1100

#include "../gui/menu_items/menu_items.h"
#include "../Platform/plist/plist.h"


class SETTINGS_DATA {
public:
  XStringW                DefaultVolume = XStringW();;
  XStringW                DefaultVolumeFromConfig = XStringW();
  BOOLEAN                 SaveDebugLogToDisk = 0;;
  BOOLEAN                 SaveDebugLogToDiskFromConfig = 0;
  INTN                    GUITimeOut = 0;;
  INTN                    GUITimeOutFromConfig = 0;

  SETTINGS_DATA() {};
  SETTINGS_DATA(const SETTINGS_DATA& other) = delete; // Can be defined if needed
  const SETTINGS_DATA& operator = ( const SETTINGS_DATA & ) = delete; // Can be defined if needed

  XBuffer<UINT8> serialize() const;

  ~SETTINGS_DATA() {}

};

//#pragma GCC diagnostic ignored "-Wpadded"

typedef enum {
  english = 0,  //en
  russian,    //ru
  french,     //fr
  german,     //de
  dutch,      //nl
  italian,    //it
  spanish,    //es
  portuguese, //pt
  brasil,     //br
  polish,     //pl
  ukrainian,  //ua
  croatian,   //hr
  czech,      //cs
  indonesian, //id
  korean,     //ko
  chinese,    //cn
  romanian    //ro
  //something else? add, please
} LANGUAGES;


extern SETTINGS_DATA                  gSettings;
extern LANGUAGES                      gLanguage;

extern TagDict*                       gConfigDict;


class REFIT_CONFIG
{
public:
//  INTN        Timeout;
  BOOLEAN     Quiet;
  BOOLEAN     DebugLogDISABLED;
  BOOLEAN     ScratchDebugLogAtStartDISABLED;
  XStringW    Theme;
  XStringW    ScreenResolution;
  INTN        ConsoleMode;
  INT32       Timezone;
  INTN        Codepage;
  INTN        CodepageSize;
  BOOLEAN     SavePreBootLogDISABLED;


  REFIT_CONFIG() : /*Timeout(-1), */Quiet(TRUE),
                   DebugLogDISABLED(FALSE), ScratchDebugLogAtStartDISABLED(FALSE), Theme(), ScreenResolution(), ConsoleMode(0), Timezone(0xFF),
                   Codepage(0xC0), CodepageSize(0xC0), SavePreBootLogDISABLED(false) {};
  REFIT_CONFIG(const REFIT_CONFIG& other) = delete; // Can be defined if needed
  const REFIT_CONFIG& operator = ( const REFIT_CONFIG & ) = delete; // Can be defined if needed
  ~REFIT_CONFIG() {  }

} ;


extern REFIT_CONFIG GlobalConfig;



EFI_STATUS
GetUserSettings (
      const TagDict*   CfgDict
  );

EFI_STATUS
LoadUserSettings (
    const XStringW& ConfName,
    TagDict** dict
  );

void savePreferencesFile();

#endif
