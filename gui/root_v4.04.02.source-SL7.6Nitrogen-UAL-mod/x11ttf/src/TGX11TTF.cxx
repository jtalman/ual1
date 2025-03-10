// @(#)root/x11ttf:$Name:  $:$Id: TGX11TTF.cxx,v 1.12 2003/06/04 11:03:59 rdm Exp $
// Author: Olivier Couet     01/10/02
// Author: Fons Rademakers   21/11/98

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGX11TTF                                                             //
//                                                                      //
// Interface to low level X11 (Xlib). This class gives access to basic  //
// X11 graphics via the parent class TGX11. However, all text and font  //
// handling is done via the Freetype TrueType library. When the         //
// shared library containing this class is loaded the global gVirtualX  //
// is redirected to point to this class.                                //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <stdlib.h>

#include "TGX11TTF.h"


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TTFInit                                                              //
//                                                                      //
// Small utility class that takes care of switching the current         //
// gVirtualX to the new TGX11TTF class as soon as the shared library    //
// containing this class is loaded.                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class TTFX11Init {
public:
   TTFX11Init();
};

TTFX11Init::TTFX11Init()
{
   if (gVirtualX && gVirtualX->IsA() == TGX11::Class()) {
      TGX11 *oldg = (TGX11 *) gVirtualX;
      gVirtualX = new TGX11TTF(*oldg);
      delete oldg;
   }
}

static TTFX11Init gTTFX11Init;

ClassImp(TGX11TTF)

//______________________________________________________________________________
TGX11TTF::TGX11TTF(const TGX11 &org) : TGX11(org)
{
   // Create copy of TGX11 but now use TrueType fonts.

   SetName("X11TTF");
   SetTitle("ROOT interface to X11 with TrueType fonts");

   if (!TTF::fgInit) TTF::Init();

   if (fDepth > 8) {
      TTF::SetSmoothing(kTRUE);
   } else {
      TTF::SetSmoothing(kFALSE);
   }

   fHasTTFonts = kTRUE;
}

//______________________________________________________________________________
TGX11TTF::~TGX11TTF()
{
}

//______________________________________________________________________________
void TGX11TTF::Align(void)
{
   // Compute alignment variables. The alignment is done on the horizontal string
   // then the rotation is applied on the alignment variables.
   // SetRotation and LayoutGlyphs should have been called before.

   EAlign align = (EAlign) fTextAlign;

   // vertical alignment
   if (align == kTLeft || align == kTCenter || align == kTRight) {
      fAlign.y = TTF::fgAscent;
   } else if (align == kMLeft || align == kMCenter || align == kMRight) {
      fAlign.y = TTF::fgAscent/2;
   } else {
      fAlign.y = 0;
   }

   // horizontal alignment
   if (align == kTRight || align == kMRight || align == kBRight) {
      fAlign.x = TTF::fgWidth;
   } else if (align == kTCenter || align == kMCenter || align == kBCenter) {
      fAlign.x = TTF::fgWidth/2;
   } else {
      fAlign.x = 0;
   }

   FT_Vector_Transform(&fAlign, TTF::fgRotMatrix);
   fAlign.x = fAlign.x >> 6;
   fAlign.y = fAlign.y >> 6;
}

//______________________________________________________________________________
void TGX11TTF::DrawImage(FT_Bitmap *source, ULong_t fore, ULong_t back,
                         XImage *xim, Int_t bx, Int_t by)
{
   // Draw FT_Bitmap bitmap to xim image at position bx,by using specified
   // foreground color.

   UChar_t d = 0, *s = source->buffer;

   if (TTF::fgSmoothing) {

      static XColor col[5];
      XColor  *bcol = 0, *bc;
      Int_t    x, y;

      // background kClear, i.e. transparent, we take as background color
      // the average of the rgb values of all pixels covered by this character
      if (back == (ULong_t) -1 && (UInt_t)source->width) {
         ULong_t r, g, b;
         Int_t   dots, dotcnt;
         const Int_t maxdots = 50000;

         dots = Int_t(source->width * source->rows);
         dots = dots > maxdots ? maxdots : dots;
         bcol = new XColor[dots];
         if (!bcol) return;
         bc = bcol;
         dotcnt = 0;
         for (y = 0; y < (int) source->rows; y++) {
            for (x = 0; x < (int) source->width; x++, bc++) {
///               bc->pixel = XGetPixel(xim, bx + x, by - c->TTF::fgAscent + y);
               bc->pixel = XGetPixel(xim, bx + x, by + y);
               bc->flags = DoRed | DoGreen | DoBlue;
               if (++dotcnt >= maxdots) break;
            }
         }
         QueryColors(fColormap, bcol, dots);
         r = g = b = 0;
         bc = bcol;
         dotcnt = 0;
         for (y = 0; y < (int) source->rows; y++) {
            for (x = 0; x < (int) source->width; x++, bc++) {
               r += bc->red;
               g += bc->green;
               b += bc->blue;
               if (++dotcnt >= maxdots) break;
            }
         }
         if (dots != 0) {
            r /= dots;
            g /= dots;
            b /= dots;
         }
         bc = &col[0];
         if (bc->red == r && bc->green == g && bc->blue == b)
            bc->pixel = back;
         else {
            bc->pixel = ~back;
            bc->red   = (UShort_t) r;
            bc->green = (UShort_t) g;
            bc->blue  = (UShort_t) b;
         }
      }
      delete [] bcol;

      // if fore or background have changed from previous character
      // recalculate the 3 smooting colors (interpolation between fore-
      // and background colors)
      if (fore != col[4].pixel || back != col[0].pixel) {
         col[4].pixel = fore;
         col[4].flags = DoRed|DoGreen|DoBlue;
         if (back != (ULong_t) -1) {
            col[3].pixel = back;
            col[3].flags = DoRed | DoGreen | DoBlue;
            QueryColors(fColormap, &col[3], 2);
            col[0] = col[3];
         } else {
            QueryColors(fColormap, &col[4], 1);
         }

         // interpolate between fore and backgound colors
         for (x = 3; x > 0; x--) {
            col[x].red   = (col[4].red  *x + col[0].red  *(4-x)) /4;
            col[x].green = (col[4].green*x + col[0].green*(4-x)) /4;
            col[x].blue  = (col[4].blue *x + col[0].blue *(4-x)) /4;
            if (!AllocColor(fColormap, &col[x])) {
               Warning("DrawImage", "cannot allocate smoothing color");
               col[x].pixel = col[x+1].pixel;
            }
         }
      }

      // put smoothed character, character pixmap values are an index
      // into the 5 colors used for aliasing (4 = foreground, 0 = background)
      for (y = 0; y < (int) source->rows; y++) {
         for (x = 0; x < (int) source->width; x++) {
            d = *s++ & 0xff;
            d = ((d + 10) * 5) / 256;
            if (d > 4) d = 4;
            if (d && x < (int) source->width) {
               ULong_t p = col[d].pixel;
               XPutPixel(xim, bx + x, by + y, p);
            }
         }
      }
   } else {
      // no smoothing, just put character using foreground color
      UChar_t* row=s;
      for (int y = 0; y < (int) source->rows; y++) {
         int n = 0;
         s = row;
         for (int x = 0; x < (int) source->width; x++) {
            if (n == 0) d = *s++;
            if (TESTBIT(d,7-n))
               XPutPixel(xim, bx + x, by + y, fore);
            if (++n == (int) kBitsPerByte) n = 0;
         }
         row += source->pitch;
      }
   }
}

//______________________________________________________________________________
void TGX11TTF::DrawText(Int_t x, Int_t y, Float_t angle, Float_t mgn,
                        const char *text, ETextMode mode)
{
   // Draw text using TrueType fonts. If TrueType fonts are not available the
   // text is drawn with TGX11::DrawText.

   if (!fHasTTFonts) {
      TGX11::DrawText(x, y, angle, mgn, text, mode);
   } else {
      if (!TTF::fgInit) TTF::Init();
      TTF::SetRotationMatrix(angle);
      TTF::PrepareString(text);
      TTF::LayoutGlyphs();
      Align();
      RenderString(x, y, mode);
   }
}

//______________________________________________________________________________
XImage *TGX11TTF::GetBackground(Int_t x, Int_t y, UInt_t w, UInt_t h)
{
   // Get the background of the current window in an XImage.

   XWindow_t *cws = GetCurrentWindow();

   if (x < 0) {
      w += x;
      x  = 0;
   }
   if (y < 0) {
      h += y;
      y  = 0;
   }

   if (x+w > cws->width)  w = cws->width - x;
   if (y+h > cws->height) h = cws->height - y;

   return XGetImage(fDisplay, cws->drawing, x, y, w, h, AllPlanes, ZPixmap);
}

//______________________________________________________________________________
Bool_t TGX11TTF::IsVisible(Int_t x, Int_t y, UInt_t w, UInt_t h)
{
   // Test if there is really something to render

   XWindow_t *cws = GetCurrentWindow();

   // If w or h is 0, very likely the string is only blank characters
   if ((int)w == 0 || (int)h == 0)  return kFALSE;

   // If string falls outside window, there is probably no need to draw it.
   if (x + (int)w <= 0 || x >= (int)cws->width)  return kFALSE;
   if (y + (int)h <= 0 || y >= (int)cws->height) return kFALSE;

   return kTRUE;
}

//______________________________________________________________________________
void TGX11TTF::RenderString(Int_t x, Int_t y, ETextMode mode)
{
   // Perform the string rendering in the pad.
   // LayoutGlyphs should have been called before.

   TTGlyph* glyph = TTF::fgGlyphs;

   // compute the size and position of the XImage that will contain the text
   Int_t Xoff = 0; if (TTF::GetBox().xMin < 0) Xoff = -TTF::GetBox().xMin;
   Int_t Yoff = 0; if (TTF::GetBox().yMin < 0) Yoff = -TTF::GetBox().yMin;
   Int_t w    = TTF::GetBox().xMax + Xoff;
   Int_t h    = TTF::GetBox().yMax + Yoff;
   Int_t x1   = x-Xoff-fAlign.x;
   Int_t y1   = y+Yoff+fAlign.y-h;

   if (!IsVisible(x1, y1, w, h)) return;

   // create the XImage that will contain the text
   UInt_t depth = fDepth;
   XImage *xim  = 0;
   xim = XCreateImage(fDisplay, fVisual,
                      depth, ZPixmap, 0, 0, w, h,
                      depth == 24 ? 32 : (depth==15?16:depth), 0);

   // use malloc since Xlib will use free() in XDestroyImage
   xim->data = (char *) malloc(xim->bytes_per_line * h);
   memset(xim->data, 0, xim->bytes_per_line * h);

   ULong_t   bg;
   XGCValues values;
   XGetGCValues(fDisplay, *GetGC(3), GCForeground | GCBackground, &values);

   // get the background
   if (mode == kClear) {
      // if mode == kClear we need to get an image of the background
      XImage *bim = GetBackground(x1, y1, w, h);
      if (!bim) {
         Error("DrawText", "error getting background image");
         return;
      }

      // and copy it into the text image
      Int_t xo = 0, yo = 0;
      if (x1 < 0) xo = -x1;
      if (y1 < 0) yo = -y1;

      for (int yp = 0; yp < (int) bim->height; yp++) {
         for (int xp = 0; xp < (int) bim->width; xp++) {
            ULong_t pixel = XGetPixel(bim, xp, yp);
            XPutPixel(xim, xo+xp, yo+yp, pixel);
         }
      }
      XDestroyImage(bim);
      bg = (ULong_t) -1;
   } else {
      // if mode == kOpaque its simple, we just draw the background
      XAddPixel(xim, values.background);
      bg = values.background;
   }

   // paint the glyphs in the XImage
   glyph = TTF::fgGlyphs;
   for (int n = 0; n < TTF::fgNumGlyphs; n++, glyph++) {
      if (FT_Glyph_To_Bitmap(&glyph->fImage,
                             TTF::fgSmoothing ? ft_render_mode_normal
                                              : ft_render_mode_mono,
                             0, 1 )) continue;
      FT_BitmapGlyph bitmap = (FT_BitmapGlyph)glyph->fImage;
      FT_Bitmap*     source = &bitmap->bitmap;
      Int_t          bx, by;

      bx = bitmap->left+Xoff;
      by = h - bitmap->top-Yoff;
      DrawImage(source, values.foreground, bg, xim, bx, by);
   }

   // put the Ximage on the screen
   XWindow_t *cws = GetCurrentWindow();
   GC *gc = GetGC(6);      // gGCpxmp
   XPutImage(fDisplay, cws->drawing, *gc, xim, 0, 0, x1, y1, w, h);
   XDestroyImage(xim);
}

//______________________________________________________________________________
void TGX11TTF::SetTextFont(Font_t fontnumber)
{
   // Set specified font.

   fTextFont = fontnumber;
   if (!fHasTTFonts) {
      TGX11::SetTextFont(fontnumber);
   } else {
      TTF::SetTextFont(fontnumber);
   }
}

//______________________________________________________________________________
Int_t TGX11TTF::SetTextFont(char *fontname, ETextSetMode mode)
{
   // Set text font to specified name.
   // mode       : loading flag
   // mode=0     : search if the font exist (kCheck)
   // mode=1     : search the font and load it if it exists (kLoad)
   // font       : font name
   //
   // Set text font to specified name. This function returns 0 if
   // the specified font is found, 1 if not.

   if (!fHasTTFonts) {
      return TGX11::SetTextFont(fontname, mode);
   } else {
      return TTF::SetTextFont(fontname);
   }
}

//______________________________________________________________________________
void TGX11TTF::SetTextSize(Float_t textsize)
{
   // Set current text size.

   fTextSize = textsize;
   if (!fHasTTFonts) {
      TGX11::SetTextSize(textsize);
   } else {
      TTF::SetTextSize(textsize);
   }
}
