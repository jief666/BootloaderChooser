/*
 * libeg/text.c
 * Text drawing functions
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
//Slice 2011 - 2016 numerous improvements, 2020 full rewritten for c++

extern "C" {
#include <Protocol/GraphicsOutput.h>
}

#include "../Platform/Settings.h"

//#include "egemb_font.h"
//#define FONT_CELL_WIDTH (7)
//#define FONT_CELL_HEIGHT (12)

#ifndef DEBUG_ALL
#define DEBUG_TEXT 0
#else
#define DEBUG_TEXT DEBUG_ALL
#endif

#if DEBUG_TEXT == 0
#define DBG(...)
#else
#define DBG(...) DebugLog(DEBUG_TEXT, __VA_ARGS__)
#endif

//NSVGfontChain *fontsDB = NULL;
//
////
//// Text rendering
////
////it is not good for vector theme
////it will be better to sum each letter width for the chosen font
//// so one more parameter is TextStyle
//VOID XTheme::MeasureText(IN const XStringW& Text, OUT INTN *Width, OUT INTN *Height)
//{
//  INTN ScaledWidth = CharWidth;
//  INTN ScaledHeight = FontHeight;
//  if (Scale != 0.f) {
//    ScaledWidth = (INTN)(CharWidth * Scale);
//    ScaledHeight = (INTN)(FontHeight * Scale);
//  }
//  if (Width != NULL)
//    *Width = StrLen(Text.wc_str()) * ((FontWidth > ScaledWidth) ? FontWidth : ScaledWidth);
//  if (Height != NULL)
//    *Height = ScaledHeight;
//}
//void XTheme::LoadFontImage(IN BOOLEAN UseEmbedded, IN INTN Rows, IN INTN Cols)
//{
//panic("LoadFontImage");
//  EFI_STATUS Status = EFI_NOT_FOUND;
//  XImage      NewImage; //tempopary image from file
//
//  INTN        ImageWidth, ImageHeight;
//  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *PixelPtr;
//  EFI_GRAPHICS_OUTPUT_BLT_PIXEL *FontPtr;
//  EFI_GRAPHICS_OUTPUT_BLT_PIXEL FirstPixel;
//  BOOLEAN     isKorean = (gLanguage == korean);
//  XStringW    fontFilePath;
//  const XStringW& commonFontDir = L"EFI\\CLOVER\\font"_XSW;
//
////  if (IsEmbeddedTheme() && !isKorean) { //or initial screen before theme init
////    Status = NewImage.FromPNG(ACCESS_EMB_DATA(emb_font_data), ACCESS_EMB_SIZE(emb_font_data)); //always success
////    MsgLog("Using embedded font\n");
////  } else if (isKorean){
////    Status = NewImage.LoadXImage(ThemeDir, L"FontKorean.png"_XSW);
////    MsgLog("Loading korean font from ThemeDir: %s\n", efiStrError(Status));
////    if (!EFI_ERROR(Status)) {
////      CharWidth = 22; //standard for korean
////    } else {
////      MsgLog("...using english\n");
////      gLanguage = english;
////    }
////  }
//
//  if (EFI_ERROR(Status)) {
//    //not loaded, use common
//    Rows = 16; //standard for english
//    Cols = 16;
////    Status = NewImage.LoadXImage(ThemeDir, FontFileName);
//  }
//
//  if (EFI_ERROR(Status)) {
//    //then take from common font folder
////    fontFilePath = SWPrintf(L"%s\\%s", commonFontDir, isKorean ? L"FontKorean.png" : ThemeX.FontFileName.data());
//    fontFilePath = commonFontDir + FontFileName;
//    Status = NewImage.LoadXImage(SelfRootDir, fontFilePath);
//    //else use embedded even if it is not embedded
//    if (EFI_ERROR(Status)) {
//      Status = NewImage.FromPNG(ACCESS_EMB_DATA(emb_font_data), ACCESS_EMB_SIZE(emb_font_data));
//    }
//    if (EFI_ERROR(Status)) {
//      MsgLog("No font found!\n");
//      return;
//    }
//  }
//
//  ImageWidth = NewImage.GetWidth();
//  //  DBG("ImageWidth=%lld\n", ImageWidth);
//  ImageHeight = NewImage.GetHeight();
//  //  DBG("ImageHeight=%lld\n", ImageHeight);
//  PixelPtr = NewImage.GetPixelPtr(0,0);
//
//  FontImage.setSizeInPixels(ImageWidth * Rows, ImageHeight / Rows);
//  FontPtr = FontImage.GetPixelPtr(0,0);
//
//  FontWidth = ImageWidth / Cols;
//  FontHeight = ImageHeight / Rows;
//  TextHeight = FontHeight + (int)(TEXT_YMARGIN * 2 * Scale);
////  if (!isKorean) {
////    CharWidth = FontWidth; //there is default value anyway
////  }
//
//  FirstPixel = *PixelPtr;
//  for (INTN y = 0; y < Rows; y++) {
//    for (INTN j = 0; j < FontHeight; j++) {
//      INTN Ypos = ((j * Rows) + y) * ImageWidth;
////      for (INTN x = 0; x < ImageWidth; x++) {
////        if (
////            //First pixel is accounting as "blue key"
////            (PixelPtr->Blue == FirstPixel.Blue) &&
////            (PixelPtr->Green == FirstPixel.Green) &&
////            (PixelPtr->Red == FirstPixel.Red)
////            ) {
////          PixelPtr->Reserved = 0; //if a pixel has same color as first pixel then it will be transparent
////        //} else if (ThemeX.DarkEmbedded) {
////        } else if (Theme X.embedded && !ThemeX.Daylight) {
////          *PixelPtr = SemiWhitePixel;  //special case to change a text to semi white, not blue pixels
////        }
////        FontPtr[Ypos + x] = *PixelPtr++; //not (x, YPos) !!!
////      }
//    }
//  }
//}
//
//VOID XTheme::PrepareFont()
//{
//
//  TextHeight = FontHeight + (int)(TEXT_YMARGIN * 2 * Scale);
//  if (TypeSVG) {
//    return;
//  }
//
//  // load the font
//  if (FontImage.isEmpty()) {
//    DBG("load font image type %d\n", Font);
//    LoadFontImage(TRUE, 16, 16); //anyway success
//  }
//
//  if (!FontImage.isEmpty()) {
//    if (Font == FONT_GRAY) {
//      //invert the font. embedded is dark
//      EFI_GRAPHICS_OUTPUT_BLT_PIXEL *p = FontImage.GetPixelPtr(0,0);
//      for (INTN Height = 0; Height < FontImage.GetHeight(); Height++){
//        for (INTN Width = 0; Width < FontImage.GetWidth(); Width++, p++){
//          p->Blue  ^= 0xFF;
//          p->Green ^= 0xFF;
//          p->Red   ^= 0xFF;
//          //p->a = 0xFF;    //huh! dont invert opacity
//        }
//      }
// //     FontImage.Draw(0, 300, 0.6f); //for debug purpose
//    }
//    DBG("Font %d prepared WxH=%lldx%lld CharWidth=%lld\n", Font, FontWidth, FontHeight, CharWidth);
//
//  } else {
//    DBG("Failed to load font\n");
//  }
//}

//search pixel similar to first
inline bool SamePix(const EFI_GRAPHICS_OUTPUT_BLT_PIXEL& Ptr, const EFI_GRAPHICS_OUTPUT_BLT_PIXEL& FirstPixel)
{
  //compare with first pixel of the array top-left point [0][0]
  return ((Ptr.Red >= FirstPixel.Red - (FirstPixel.Red >> 2)) &&
          (Ptr.Red <= FirstPixel.Red + (FirstPixel.Red >> 2)) &&
          (Ptr.Green >= FirstPixel.Green - (FirstPixel.Green >> 2)) &&
          (Ptr.Green <= FirstPixel.Green + (FirstPixel.Green >> 2)) &&
          (Ptr.Blue >= FirstPixel.Blue - (FirstPixel.Blue >> 2)) &&
          (Ptr.Blue <= FirstPixel.Blue + (FirstPixel.Blue >> 2)) &&
          (Ptr.Reserved == FirstPixel.Reserved)); //hack for transparent fonts
}
//
////used for proportional fonts in raster themes
////search empty column from begin Step=1 or from end Step=-1 in the input buffer
//// empty means similar to FirstPixel
//INTN XTheme::GetEmpty(const XImage& Buffer, const EFI_GRAPHICS_OUTPUT_BLT_PIXEL& FirstPixel, INTN MaxWidth, INTN Start, INTN Step)
//{
//  INTN m, i;
////  INTN Shift = (Step > 0)?0:1;
//  m = MaxWidth;
//  if (Step == 1) {
//    for (INTN j = 0; j < FontHeight; j++) {
//      for (i = 0; i < MaxWidth; i++) {
//        if (!SamePix(Buffer.GetPixel(Start + i,j), FirstPixel)) { //found not empty pixel
//          break;
//        }
//      }
//      m = MIN(m, i); //for each line to find minimum
//      if (m == 0) break;
//    }
//  } else { // go back
//    for (INTN j = 0; j < FontHeight; j++) {
//      for (i = 1; i <= MaxWidth; i++) {
//        if (!SamePix(Buffer.GetPixel(Start - i,j), FirstPixel)) { //found not empty pixel
//          break;
//        }
//      }
//      m = MIN(m, i); //for each line to find minimum
//      if (m == 0) break;
//    }
//  }
//  return m;
//}


/* EOF */
