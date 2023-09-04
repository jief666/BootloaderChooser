/*
 Slice 2012
 */

#include "../entry_scan/entry_scan.h"
#include "../entry_scan/loader.h"
#include "../refit/screen.h"
#include "../refit/menu.h"
#include "guid.h"
#include "../include/Pci.h"
#include "../include/Devices.h"
#include "../Platform/Settings.h"
#include "../Settings/Self.h"

#ifndef DEBUG_ALL
#define DEBUG_SET 1
#else
#define DEBUG_SET DEBUG_ALL
#endif

#if DEBUG_SET == 0
#define DBG(...)
#else
#define DBG(...) DebugLog (DEBUG_SET, __VA_ARGS__)
#endif

//#define DUMP_KERNEL_KEXT_PATCHES 1

//#define SHORT_LOCATE 1

//#define kXMLTagArray      "array"

//EFI_GUID gRandomUUID = {0x0A0B0C0D, 0x0000, 0x1010, {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07}};

#define NUM_OF_CONFIGS 3
#define GEN_PMCON_1                 0xA0

//BOOLEAN                        SavePreBootLog;
//UINT8                            DefaultAudioVolume;
//INTN LayoutBannerOffset = 64;
//INTN LayoutTextOffset = 0;
//INTN LayoutButtonOffset = 0;


TagDict*                          gConfigDict = NULL;

SETTINGS_DATA                   gSettings;
LANGUAGES                       gLanguage;

XString8Array                   ConfigsList;


// global configuration with default values
REFIT_CONFIG   GlobalConfig;

bool forceRewrite = false;



EFI_STATUS
LoadUserSettings (
                  IN const XStringW& ConfName,
                  TagDict** Dict)
{
  EFI_STATUS Status = EFI_NOT_FOUND;
  XBuffer<UINT8> ConfigPtr;
  XStringW   ConfigPlistPath;

  //  DbgHeader("LoadUserSettings");

  // load config
  if ( ConfName.isEmpty() || Dict == NULL ) {
    return EFI_UNSUPPORTED;
  }

  ConfigPlistPath = SWPrintf("%ls.plist", ConfName.wc_str());
  if ( !FileExists(&self.getCloverDir(), ConfigPlistPath) ) {
    return EFI_NOT_FOUND;
  }
  Status = egLoadFile(&self.getCloverDir(), ConfigPlistPath.wc_str(), &ConfigPtr);
  if (EFI_ERROR(Status)) {
    DBG("Cannot load %ls\\%ls (%s). Using default.\n", self.getCloverDirFullPath().wc_str(), ConfigPlistPath.wc_str(), efiStrError(Status));
    return Status;
  }
//    DBG("Using %ls.plist at RootDir at path: %ls\n", ConfName.wc_str(), ConfigPlistPath.wc_str());

  Status = ParseXML(ConfigPtr.CData(), Dict, ConfigPtr.size());
  if (EFI_ERROR(Status)) {
    //  Dict = NULL;
    DBG("config.plist parse error Status=%s\n", efiStrError(Status));
    return Status;
  }
  if ( ConfigPtr.size() > 0 ) {
    ConfigPtr[ConfigPtr.size()-1] = 0;
    if ( !strstr((char*)ConfigPtr.data(), "</plist>") ) {
      forceRewrite = true;
    }
  }
  // free configPtr ?
  return Status;
}


void savePreferencesFile()
{

  if ( forceRewrite ||
       gSettings.GUITimeOut != gSettings.GUITimeOutFromConfig ||
       gSettings.DefaultVolume != gSettings.DefaultVolumeFromConfig ||
       gSettings.SaveDebugLogToDisk != gSettings.SaveDebugLogToDiskFromConfig ) {

    XString8 buf;

    buf += S8Printf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    buf += S8Printf("<!DOCTYPE plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">\n");
    buf += S8Printf("<plist version=\"1.0\">\n");
    buf += S8Printf("<dict>\n");
    buf += S8Printf("    <key>Timeout</key>\n");
    buf += S8Printf("    <integer>%lld</integer>\n", gSettings.GUITimeOut);
    buf += S8Printf("    <key>DefaultVolume</key>\n");
    buf += S8Printf("    <string>%ls</string>\n", gSettings.DefaultVolume.wc_str());
    buf += S8Printf("    <key>Debug</key>\n");
    buf += S8Printf("    <string>%s</string>\n", gSettings.SaveDebugLogToDisk ? "true" : "false");
    buf += S8Printf("</dict>\n");
    buf += S8Printf("</plist>\n");

    egSaveFile(&self.getCloverDir(), L"BLC.plist", buf.data(), buf.length());
  }
}





EFI_STATUS
GetUserSettings (const TagDict* CfgDict)
{
  EFI_STATUS  Status = EFI_SUCCESS;


  if ( CfgDict == NULL ) return EFI_SUCCESS;

//  DbgHeader("GetUserSettings");

  {
    const TagStruct* Prop = CfgDict->propertyForKey("Timeout");
    if ( Prop ) {
      if ( Prop->isInt64() ) {
        gSettings.GUITimeOut = gSettings.GUITimeOutFromConfig = Prop->getInt64()->intValue();
      }else{
        MsgLog("MALFORMED plist. Timeout must be integer");
      }
    }
  }
  {
    const TagStruct* Prop = CfgDict->propertyForKey("DefaultVolume");
    if ( Prop ) {
      if ( Prop->isString() ) {
        gSettings.DefaultVolume = gSettings.DefaultVolumeFromConfig = Prop->getString()->stringValue();
      }else{
        MsgLog("MALFORMED plist. DefaultVolume must be string");
      }
    }
  }
  {
    const TagStruct* Prop = CfgDict->propertyForKey("Debug");
    if ( Prop  && !Prop->isTrueOrYes() && !Prop->isFalseOrNn() ) {
      MsgLog("MALFORMED plist. debug must be true or false");
    }
    gSettings.SaveDebugLogToDisk = gSettings.SaveDebugLogToDiskFromConfig = IsPropertyNotNullAndTrue(Prop);
  }
  return Status;
}



