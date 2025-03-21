// @(#)root/graf:$Name:  $:$Id: TTF.cxx,v 1.6 2003/07/23 19:24:09 brun Exp $
// Author: Olivier Couet     01/10/02

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TTF                                                                  //
//                                                                      //
// Interface to the freetype 2 library.                                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

// config.h is needed for TTFFONTDIR
#ifdef HAVE_CONFIG
#include "config.h"
#endif

#include "TTF.h"
#include "TSystem.h"
#include "TEnv.h"
#include "TMath.h"
#include "TError.h"


// to scale fonts to the same size as the old TT version
const Float_t kScale = 0.93376068;

TTF gCleanupTTF; // Allows to call "Cleanup" at the end of the session

Bool_t      TTF::fgInit       = kFALSE;
Bool_t      TTF::fgSmoothing  = kTRUE;
Bool_t      TTF::fgKerning    = kTRUE;
Bool_t      TTF::fgHinting    = kFALSE;
Int_t       TTF::fgTBlankW    = 0;
Int_t       TTF::fgWidth      = 0;
Int_t       TTF::fgAscent     = 0;
Int_t       TTF::fgCurFontIdx = -1;
Int_t       TTF::fgFontCount  = 0;
Int_t       TTF::fgNumGlyphs  = 0;
char       *TTF::fgFontName[kTTMaxFonts];
FT_Matrix  *TTF::fgRotMatrix;
FT_Library  TTF::fgLibrary;
FT_BBox     TTF::fgCBox;
FT_Face     TTF::fgFace[kTTMaxFonts];
FT_CharMap  TTF::fgCharMap[kTTMaxFonts];
TTGlyph     TTF::fgGlyphs[kMaxGlyphs];


ClassImp(TTF)

//______________________________________________________________________________
TTF::~TTF()
{
   // Cleanup TTF environment.

   Cleanup();
}

//______________________________________________________________________________
void TTF::Init()
{
   // Initialise the TrueType fonts interface.

   fgInit = kTRUE;

   // initialize FTF library
   if (FT_Init_FreeType(&fgLibrary)) {
      Error("TTF::Init", "error initializing FreeType");
      return;
   }

   // load default font (arialbd)
   SetTextFont(62);
}

//______________________________________________________________________________
void TTF::Cleanup()
{
   // Cleanup. Is called by the gCleanupTTF destructor.

   if (!fgInit) return;

   for (int i = 0; i < fgFontCount; i++) {
      delete [] fgFontName[i];
      FT_Done_Face(fgFace[i]);
   }
   if (fgRotMatrix) delete fgRotMatrix;
   FT_Done_FreeType(fgLibrary);
}

//______________________________________________________________________________
Short_t TTF::CharToUnicode(UInt_t code)
{
   // Map char to unicode. Returns 0 in case no mapping exists.

   if (!fgCharMap[fgCurFontIdx]) {
      UShort_t i, platform, encoding;
      FT_CharMap  charmap;

      if (!fgFace[fgCurFontIdx]) return 0;
      Int_t n = fgFace[fgCurFontIdx]->num_charmaps;
      for (i = 0; i < n; i++) {
         if (!fgFace[fgCurFontIdx]) continue;
         charmap  = fgFace[fgCurFontIdx]->charmaps[i];
         platform = charmap->platform_id;
         encoding = charmap->encoding_id;
         if ((platform == 3 && encoding == 1) ||
             (platform == 0 && encoding == 0) ||
             (platform == 1 && encoding == 0 &&
              !strcmp(fgFontName[fgCurFontIdx], "symbol.ttf"))) {
            fgCharMap[fgCurFontIdx] = charmap;
            if (FT_Set_Charmap(fgFace[fgCurFontIdx], fgCharMap[fgCurFontIdx]))
                Error("TTF::CharToUnicode", "error in FT_Set_CharMap");
            return FT_Get_Char_Index(fgFace[fgCurFontIdx], (FT_ULong)code);
         }
      }
   }
   return FT_Get_Char_Index(fgFace[fgCurFontIdx], (FT_ULong)code);
}

//______________________________________________________________________________
void TTF::GetTextExtent(UInt_t &w, UInt_t &h, char *text)
{
   // Get width (w) and height (h) when text is horizontal.

   if (!fgInit) Init();

   SetRotationMatrix(0);
   PrepareString(text);
   LayoutGlyphs();
   Int_t Xoff = 0; if (fgCBox.xMin < 0) Xoff = -fgCBox.xMin;
   Int_t Yoff = 0; if (fgCBox.yMin < 0) Yoff = -fgCBox.yMin;
   w = fgCBox.xMax + Xoff + fgTBlankW;
   h = fgCBox.yMax + Yoff;
}

//______________________________________________________________________________
void TTF::LayoutGlyphs()
{
   // Compute the glyps positions, fgAscent and fgWidth (needed for alignment).
   // Perform the Glyphs transformation.
   // Compute the string control box.
   // If required take the "kerning" into account.
   // SetRotation and PrepareString should have been called before.

   TTGlyph*  glyph = fgGlyphs;
   FT_Vector origin;
   FT_UInt   load_flags;
   FT_UInt   prev_index = 0;

   fgAscent = 0;
   fgWidth  = 0;

   load_flags = FT_LOAD_DEFAULT;
   if (!fgHinting) load_flags |= FT_LOAD_NO_HINTING;

   fgCBox.xMin = fgCBox.yMin =  32000;
   fgCBox.xMax = fgCBox.yMax = -32000;

   for (int n = 0; n < fgNumGlyphs; n++, glyph++) {

      // compute glyph origin
      if (fgKerning) {
         if (prev_index) {
            FT_Vector  kern;
            FT_Get_Kerning(fgFace[fgCurFontIdx], prev_index, glyph->fIndex,
                           fgHinting ? ft_kerning_default : ft_kerning_unfitted,
                           &kern);
            fgWidth += kern.x;
         }
         prev_index = glyph->fIndex;
      }

      origin.x = fgWidth;
      origin.y = 0;

      // clear existing image if there is one
      if (glyph->fImage) FT_Done_Glyph(glyph->fImage);

      // load the glyph image (in its native format)
      if (FT_Load_Glyph(fgFace[fgCurFontIdx], glyph->fIndex, load_flags))
         continue;

      // extract the glyph image
      if (FT_Get_Glyph (fgFace[fgCurFontIdx]->glyph, &glyph->fImage))
         continue;

      glyph->fPos = origin;
      fgWidth    += fgFace[fgCurFontIdx]->glyph->advance.x;
      fgAscent    = TMath::Max((Int_t)(fgFace[fgCurFontIdx]->glyph->metrics.horiBearingY), fgAscent);

      // transform the glyphs
      FT_Vector_Transform(&glyph->fPos, fgRotMatrix);
      if (FT_Glyph_Transform(glyph->fImage, fgRotMatrix, &glyph->fPos))
         continue;

      // compute the string control box
      FT_BBox  bbox;
      FT_Glyph_Get_CBox(glyph->fImage, ft_glyph_bbox_pixels, &bbox);
      if (bbox.xMin < fgCBox.xMin) fgCBox.xMin = bbox.xMin;
      if (bbox.yMin < fgCBox.yMin) fgCBox.yMin = bbox.yMin;
      if (bbox.xMax > fgCBox.xMax) fgCBox.xMax = bbox.xMax;
      if (bbox.yMax > fgCBox.yMax) fgCBox.yMax = bbox.yMax;
   }
}

//______________________________________________________________________________
void TTF::PrepareString(const char *string)
{
   // Put the characters in "string" in the "glyphs" array.

   const unsigned char *p = (const unsigned char*) string;
   TTGlyph *glyph = fgGlyphs;
   UInt_t index;       // Unicode value
   Int_t NbTBlank = 0; // number of trailing blanks

   fgTBlankW   = 0;
   fgNumGlyphs = 0;
   while (*p) {
      index = CharToUnicode((FT_ULong)*p);
      if (index != 0) {
         glyph->fIndex = index;
         glyph++;
         fgNumGlyphs++;
      }
      if (index == 3) {
         NbTBlank++;
      } else {
         NbTBlank = 0;
      }
      if (fgNumGlyphs >= kMaxGlyphs) break;
      p++;
   }

   // compute the trailing blanks width. It is use to compute the text
   // width in GetTextExtent
   if (NbTBlank) {
      FT_UInt load_flags = FT_LOAD_DEFAULT;
      if (!fgHinting) load_flags |= FT_LOAD_NO_HINTING;
      if (FT_Load_Glyph(fgFace[fgCurFontIdx], 3, load_flags)) return;
      fgTBlankW = (Int_t)((fgFace[fgCurFontIdx]->glyph->advance.x)>>6)*NbTBlank;
   }
}

//______________________________________________________________________________
void TTF::SetHinting(Bool_t state)
{
   // Set hinting flag.

   fgHinting = state;
}

//______________________________________________________________________________
void TTF::SetKerning(Bool_t state)
{
   // Set kerning flag.

   fgKerning = state;
}

//______________________________________________________________________________
void TTF::SetRotationMatrix(Float_t angle)
{
   // Set the rotation matrix used to rotate the font outlines.

   Float_t rangle = Float_t(angle * TMath::Pi() / 180.); // Angle in radian
#if defined(FREETYPE_PATCH) && \
    (FREETYPE_MAJOR == 2) && (FREETYPE_MINOR == 1) && (FREETYPE_PATCH == 2)
   Float_t sin    = TMath::Sin(rangle);
   Float_t cos    = TMath::Cos(rangle);
#else
   Float_t sin    = TMath::Sin(-rangle);
   Float_t cos    = TMath::Cos(-rangle);
#endif

   if (!fgRotMatrix) fgRotMatrix = new FT_Matrix;

   fgRotMatrix->xx = (FT_Fixed) (cos * (1<<16));
   fgRotMatrix->xy = (FT_Fixed) (sin * (1<<16));
   fgRotMatrix->yx = -fgRotMatrix->xy;
   fgRotMatrix->yy =  fgRotMatrix->xx;
}

//______________________________________________________________________________
void TTF::SetSmoothing(Bool_t state)
{
   // Set smoothing (anti-aliasing) flag.

   fgSmoothing = state;
}

//______________________________________________________________________________
Int_t TTF::SetTextFont(const char *fontname)
{
   // Set text font to specified name.
   // font       : font name
   //
   // Set text font to specified name. This function returns 0 if
   // the specified font is found, 1 if not.

   if (!fgInit) Init();

   if (!fontname || !fontname[0]) {
      Warning("TTF::SetTextFont",
              "no font name specified, using default font %s", fgFontName[0]);
      fgCurFontIdx = 0;
      return 0;
   }
   const char *basename = gSystem->BaseName(fontname);

   // check if font is in cache
   int i;
   for (i = 0; i < fgFontCount; i++) {
      if (!strcmp(fgFontName[i], basename)) {
         fgCurFontIdx = i;
         return 0;
      }
   }

   // enough space in cache to load font?
   if (fgFontCount >= kTTMaxFonts) {
      Error("TTF::SetTextFont", "too many fonts opened (increase kTTMaxFont = %d)",
            kTTMaxFonts);
      Warning("TTF::SetTextFont", "using default font %s", fgFontName[0]);
      fgCurFontIdx = 0;    // use font 0 (default font, set in ctor)
      return 0;
   }

   // try to load font (font must be in Root.TTFontPath resource)
   const char *ttpath = gEnv->GetValue("Root.TTFontPath",
# ifdef TTFFONTDIR
                                       TTFFONTDIR);
# else
                                       "$(ROOTSYS)/fonts");
# endif

   char *ttfont = gSystem->Which(ttpath, fontname, kReadPermission);

   if (!ttfont) {
      Error("TTF::SetTextFont", "font file %s not found in path", fontname);
      if (fgFontCount) {
         Warning("TTF::SetTextFont", "using default font %s", fgFontName[0]);
         fgCurFontIdx = 0;    // use font 0 (default font, set in ctor)
         return 0;
      } else {
         return 1;
      }
   }

   FT_Face  tface = 0;

   if (FT_New_Face(fgLibrary, ttfont, 0, &tface)) {
      Error("TTF::SetTextFont", "error loading font %s", ttfont);
      delete [] ttfont;
      if (tface) FT_Done_Face(tface);
      if (fgFontCount) {
         Warning("TTF::SetTextFont", "using default font %s", fgFontName[0]);
         fgCurFontIdx = 0;
         return 0;
      } else {
         return 1;
      }
   }

   delete [] ttfont;

   fgFontName[fgFontCount] = StrDup(basename);
   fgCurFontIdx            = fgFontCount;
   fgFace[fgCurFontIdx]    = tface;
   fgCharMap[fgCurFontIdx] = 0;
   fgFontCount++;

   return 0;
}

//______________________________________________________________________________
void TTF::SetTextFont(Font_t fontnumber)
{
   // Set specified font.
   // List of the currently supported fonts (screen and PostScript)
   // =============================================================
   //   Font ID       X11                        TTF
   //        1 : times-medium-i-normal       timesi.ttf
   //        2 : times-bold-r-normal         timesbd.ttf
   //        3 : times-bold-i-normal         timesi.ttf
   //        4 : helvetica-medium-r-normal   arial.ttf
   //        5 : helvetica-medium-o-normal   ariali.ttf
   //        6 : helvetica-bold-r-normal     arialbd.ttf
   //        7 : helvetica-bold-o-normal     arialbi.ttf
   //        8 : courier-medium-r-normal     cour.ttf
   //        9 : courier-medium-o-normal     couri.ttf
   //       10 : courier-bold-r-normal       courbd.ttf
   //       11 : courier-bold-o-normal       courbi.ttf
   //       12 : symbol-medium-r-normal      symbol.ttf
   //       13 : times-medium-r-normal       times.ttf
   //       14 :                             wingding.ttf

   const char *fontname;

   if (!fgInit) Init();

   switch (fontnumber/10) {

      case 1:
          fontname = "timesi.ttf";
          break;
      case 2:
          fontname = "timesbd.ttf";
          break;
      case 3:
          fontname = "timesbi.ttf";
          break;
      case 4:
          fontname = "arial.ttf";
          break;
      case 5:
          fontname = "ariali.ttf";
          break;
      case 6:
          fontname = "arialbd.ttf";
          break;
      case 7:
          fontname = "arialbi.ttf";
          break;
      case 8:
          fontname = "cour.ttf";
          break;
      case 9:
          fontname = "couri.ttf";
          break;
      case 10:
          fontname = "courbd.ttf";
          break;
      case 11:
          fontname = "courbi.ttf";
          break;
      case 12:
          fontname = "symbol.ttf";
          break;
      case 13:
          fontname = "times.ttf";
          break;
      case 14:
          fontname = "wingding.ttf";
          break;
      default:
          fontname = "arialbd.ttf";
          break;
   }

   SetTextFont(fontname);
}

//______________________________________________________________________________
void TTF::SetTextSize(Float_t textsize)
{
   // Set current text size.

   if (!fgInit) Init();
   if (textsize < 0) return;

   if (fgCurFontIdx < 0 || fgFontCount <= fgCurFontIdx) {
      Error("TTF::SetTextSize", "current font index out of bounds");
      fgCurFontIdx = 0;
      return;
   }

   Int_t tsize = (Int_t)(textsize*kScale+0.5) << 6;
   if (FT_Set_Char_Size(fgFace[fgCurFontIdx], tsize, tsize, 72, 72))
      Error("TTF::SetTextSize", "error in FT_Set_Char_Size");
}

//______________________________________________________________________________
void TTF::Version(Int_t &major, Int_t &minor, Int_t &patch)
{
   FT_Library_Version(fgLibrary, &major, &minor, &patch);
}

//______________________________________________________________________________
Bool_t TTF::GetHinting()
{
    return fgHinting;
}

//______________________________________________________________________________
Bool_t TTF::GetKerning()
{
    return fgKerning;
}

//______________________________________________________________________________
Bool_t TTF::GetSmoothing()
{
    return fgSmoothing;
}

//______________________________________________________________________________
Bool_t TTF::IsInitialized()
{
    return fgInit;
}

//______________________________________________________________________________
Int_t  TTF::GetWidth()
{
    return fgWidth;
}

//______________________________________________________________________________
Int_t  TTF::GetAscent()
{
    return fgAscent;
}

//______________________________________________________________________________
Int_t  TTF::GetNumGlyphs()
{
    return fgNumGlyphs;
}

//______________________________________________________________________________
FT_Matrix *TTF::GetRotMatrix()
{
    return fgRotMatrix;
}

//______________________________________________________________________________
const FT_BBox &TTF::GetBox()
{
    return fgCBox;
}

//______________________________________________________________________________
TTGlyph *TTF::GetGlyphs()
{
    return fgGlyphs;
}
