// @(#)root/base:$Name:  $:$Id: TColor.h,v 1.11 2004/12/10 15:54:15 brun Exp $
// Author: Rene Brun   12/12/94

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TColor
#define ROOT_TColor

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TColor                                                               //
//                                                                      //
// Color defined by RGB or HLS.                                         //
// At initialization time, a table of colors is generated. This linked  //
// list can be accessed from the ROOT object                            //
// (see TROOT::GetListOfColors()). When a color is defined in the range //
// of [1,50], two "companion" colors are also defined:                  //
//    - the dark version (color_index + 100)                            //
//    - the bright version (color_index + 150)                          //
// The dark and bright color are used to give 3-D effects when drawing  //
// various boxes (see TWbox, TPave, TPaveText, TPaveLabel,etc).         //
//                                                                      //
// This is the list of currently supported basic colors (here dark and  //
// bright colors are not shown).                                        //
//Begin_Html
/*
<img src="gif/colors.gif">
*/
//End_Html
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TNamed
#include "TNamed.h"
#endif


class TColor : public TNamed {

private:
   Int_t        fNumber;        //Color number identifier
   Float_t      fRed;           //Fraction of Red
   Float_t      fGreen;         //Fraction of Green
   Float_t      fBlue;          //Fraction of Blue
   Float_t      fHue;           //Hue
   Float_t      fLight;         //Light
   Float_t      fSaturation;    //Saturation
   Float_t      fAlpha;         //Alpha (transparency)

   void           Allocate();
   static Float_t HLStoRGB1(Float_t rn1, Float_t rn2, Float_t huei);

public:
   TColor();
   TColor(Int_t color, Float_t r, Float_t g, Float_t b, const char *name="", Float_t a = 1);
   TColor(const TColor &color);
   virtual ~TColor();
   const char   *AsHexString() const;
   void          Copy(TObject &color) const;
   virtual void  GetRGB(Float_t &r, Float_t &g, Float_t &b) const { r = fRed; g = fGreen; b = fBlue; }
   virtual void  GetHLS(Float_t &h, Float_t &l, Float_t &s) const { h = fHue; l = fLight; s = fSaturation; }
   Int_t         GetNumber() const { return fNumber; }
   ULong_t       GetPixel() const;
   Float_t       GetRed() const { return fRed; }
   Float_t       GetGreen() const { return fGreen; }
   Float_t       GetBlue() const { return fBlue; }
   Float_t       GetHue() const { return fHue; }
   Float_t       GetLight() const { return fLight; }
   Float_t       GetSaturation() const { return fSaturation; }
   Float_t       GetAlpha() const { return fAlpha; }
   virtual void  ls(Option_t *option="") const;
   virtual void  Print(Option_t *option="") const;
   virtual void  SetRGB(Float_t r, Float_t g, Float_t b);

   static void    HLS2RGB(Float_t h, Float_t l, Float_t s, Float_t &r, Float_t &g, Float_t &b);
   static void    HLS2RGB(Int_t h, Int_t l, Int_t s, Int_t &r, Int_t &g, Int_t &b);
   static void    HLStoRGB(Float_t h, Float_t l, Float_t s, Float_t &r, Float_t &g, Float_t &b)
                     { TColor::HLS2RGB(h, l, s, r, g, b); } // backward compatible
   static void    RGB2HLS(Float_t r, Float_t g, Float_t b, Float_t &h, Float_t &l, Float_t &s);
   static void    RGB2HLS(Int_t r, Int_t g, Int_t b, Int_t &h, Int_t &l, Int_t &s);
   static void    RGBtoHLS(Float_t r, Float_t g, Float_t b, Float_t &h, Float_t &l, Float_t &s)
                     { TColor::RGB2HLS(r, g, b, h, l, s); } // backward compatible
   static Int_t   GetColor(const char *hexcolor);
   static Int_t   GetColor(Float_t r, Float_t g, Float_t b);
   static Int_t   GetColor(Int_t r, Int_t g, Int_t b);
   static Int_t   GetColor(ULong_t pixel);
   static ULong_t Number2Pixel(Int_t ci);
   static ULong_t RGB2Pixel(Int_t r, Int_t g, Int_t b);
   static ULong_t RGB2Pixel(Float_t r, Float_t g, Float_t b);
   static void    Pixel2RGB(ULong_t pixel, Int_t &r, Int_t &g, Int_t &b);
   static void    Pixel2RGB(ULong_t pixel, Float_t &r, Float_t &g, Float_t &b);
   static const char *PixelAsHexString(ULong_t pixel);
   static void    SaveColor(ofstream &out, Int_t ci);

   ClassDef(TColor,2)  //Color defined by RGB or HLS
};

#endif

