// @(#)root/asimage:$Name:  $:$Id: TASImage.h,v 1.9 2005/05/03 13:11:32 brun Exp $
// Author: Fons Rademakers, Reiner Rohlfs 28/11/2001

/*************************************************************************
 * Copyright (C) 1995-2001, Rene Brun, Fons Rademakers and Reiner Rohlfs *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TASImage
#define ROOT_TASImage


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TASImage                                                             //
//                                                                      //
// Interface to image processing library using libAfterImage.           //
// It allows for the reading and writing of images in different         //
// formats, several image manipulations (scaling, tiling, merging,      //
// etc.) and displaying in pads.                                        //
// The size of the image on the screen does not depend on the original  //
// size of the image but on the size of the pad. Therefore it is very   //
// easy to resize the image on the screen by resizing the pad.          //
//                                                                      //
// Besides reading an image from a file an image can be defined by a    //
// two dimensional array of values. A palette defines the color of      //
// each value.                                                          //
//                                                                      //
// The image can be zoomed by defining a rectangle with the mouse.      //
// The color palette can be modified with a GUI, just select            //
// StartPaletteEditor() from the context menu.                          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TImage
#include "TImage.h"
#endif

struct ASImage;
struct ASVisual;
class TH2D;

class TASImage : public TImage {

private:
   enum { kNoZoom = 0, kZoom = 1, kZoomOps = -1 };

   void DrawVLine(UInt_t x, UInt_t y1, UInt_t y2, UInt_t col, UInt_t thick);
   void DrawHLine(UInt_t y, UInt_t x1, UInt_t x2, UInt_t col, UInt_t thick);
   void DrawLineInternal(UInt_t x1, UInt_t y1, UInt_t x2, UInt_t y2, UInt_t col, UInt_t thick);
   void DrawWideLine(UInt_t x1, UInt_t y1, UInt_t x2, UInt_t y2,  UInt_t col, UInt_t thick);
   void DrawDashHLine(UInt_t y, UInt_t x1, UInt_t x2, UInt_t nDash, const char *pDash, UInt_t col, UInt_t thick);
   void DrawDashVLine(UInt_t x, UInt_t y1, UInt_t y2, UInt_t nDash, const char *pDash, UInt_t col, UInt_t thick);
   void DrawDashZLine(UInt_t x1, UInt_t y1, UInt_t x2, UInt_t y2, UInt_t nDash, const char *pDash, UInt_t col);
   Bool_t GetPolygonSpans(UInt_t npt, TPoint *ppt, UInt_t *nspans, TPoint **firstPoint, UInt_t **firstWidth);
   void GetFillAreaSpans(UInt_t npt, TPoint *ppt, UInt_t *nspans, TPoint **firstPoint, UInt_t **firstWidth);
   void FillRectangleInternal(UInt_t col, Int_t x, Int_t y, UInt_t width, UInt_t height);
   void DrawTextTTF(Int_t x, Int_t y, const char *text, Int_t size, UInt_t color, const char *font_name, Float_t angle);
   void DrawGlyph(void *bitmap, UInt_t color, Int_t x, Int_t y);

protected:
   ASImage  *fImage;        //! pointer to image structure of original image
   TASImage *fScaledImage;  //! temporary scaled and zoomed image produced from original image
   Double_t  fMaxValue;     // max value in image
   Double_t  fMinValue;     // min value in image
   UInt_t    fZoomOffX;     // X - offset for zooming in image pixels
   UInt_t    fZoomOffY;     // Y - offset for zooming im image pixels
   UInt_t    fZoomWidth;    // width of zoomed image in image pixels
   UInt_t    fZoomHeight;   // hight of zoomed image in image pixels
   Int_t     fZoomUpdate;   // kZoom - new zooming required, kZoomOps - other ops in action, kNoZoom - no zooming or ops
   Bool_t    fEditable;     // kTRUE image can be resized, moved by resizing/moving gPad 
   Bool_t    fPaintMode;    // kTRUE - fast mode, kFALSE - low memory mode

   static ASVisual *fgVisual;  // pointer to visual structure
   static Bool_t    fgInit;    // global flag to init afterimage only once

   EImageFileTypes GetFileType(const char *ext);
   void MapFileTypes(EImageFileTypes &type, UInt_t &astype,
                     Bool_t toas = kTRUE);
   void MapQuality(EImageQuality &quality, UInt_t &asquality,
                   Bool_t toas = kTRUE);

   static Bool_t InitVisual();

public:
   TASImage();
   TASImage(UInt_t w, UInt_t h);
   TASImage(const char *file, EImageFileTypes type = kUnknown);
   TASImage(const char *name, const Double_t *imageData, UInt_t width, UInt_t height, TImagePalette *palette = 0);
   TASImage(const char *name, const TArrayD &imageData, UInt_t width, TImagePalette *palette = 0);
   TASImage(const char *name, const TVectorD &imageData, UInt_t width, TImagePalette *palette = 0);
   TASImage(const TASImage &img);
   TASImage &operator=(const TASImage &img);
   virtual ~TASImage();

   TObject *Clone(const char *newname) const;

   void  SetEditable(Bool_t on = kTRUE) { fEditable = on; }             //*TOGGLE*
   Bool_t IsEditable() const { return fEditable; }

   // Pad conversions
   void  FromPad(TVirtualPad *pad, Int_t x = 0, Int_t y = 0,
                 UInt_t w = 0, UInt_t h = 0);
   void  Draw(Option_t *option = "");
   void  Paint(Option_t *option = "");
   Int_t DistancetoPrimitive(Int_t px, Int_t py);
   void  ExecuteEvent(Int_t event, Int_t px, Int_t py);
   char *GetObjectInfo(Int_t px, Int_t py) const;

   // Transformations
   void  SetPalette(const TImagePalette *palette);
   void  Zoom(UInt_t offX, UInt_t offY, UInt_t width, UInt_t height);   //*MENU*
   void  UnZoom();                                                      //*MENU*
   void  Flip(Int_t flip = 180);                                        //*MENU*
   void  Mirror(Bool_t vert = kTRUE);                                   //*MENU*
   void  Scale(UInt_t width, UInt_t height);                            //*MENU*
   void  Tile(UInt_t width, UInt_t height);                             //*MENU*
   void  Crop(Int_t x = 0, Int_t y = 0, UInt_t width = 0, UInt_t height = 0); //*MENU*
   void  Pad(const char *color = "#00FFFFFF", UInt_t left = 0, 
             UInt_t right = 0, UInt_t top = 0, UInt_t bottom = 0);      //*MENU*
   void  Blur(Double_t hr = 3, Double_t vr = 3);                        //*MENU*
   void  Vectorize(UInt_t max_colors = 256, UInt_t dither = 4, Int_t opaque_threshold = 1);  //*MENU*
   void  StartPaletteEditor();                                          //*MENU*
   void  HSV(UInt_t hue = 0, UInt_t radius = 360, Int_t H = 0, Int_t S = 0, Int_t V = 0, 
             Int_t x = 0, Int_t y = 0, UInt_t width = 0, UInt_t height = 0);
   void  Merge(const TImage *im, const char *op = "alphablend", Int_t x = 0, Int_t y = 0);
   void  Append(const TImage *im, const char * option = "+", const char *color = "#00000000");
   void  Gradient(UInt_t angle = 0, const char *colors = "#FFFFFF #000000", const char *offsets = 0,
                  Int_t x = 0, Int_t y = 0, UInt_t width = 0, UInt_t height = 0);
   void  Bevel(Int_t x = 0, Int_t y = 0, UInt_t width = 0, UInt_t height = 0, const char *hi = "#ffdddddd",
               const char *lo = "#ff555555", UShort_t thick = 1, Bool_t pressed = kFALSE);
   void  DrawText(Int_t  x = 0, Int_t y = 0, const char *text = "", Int_t size = 12,
                  const char *color = 0, const char *font = "fixed", EText3DType type = TImage::kPlain,
                  const char *fore_file = 0, Float_t angle = 0);

   // vector graphics
   void  BeginPaint(Bool_t fast = kTRUE);
   void  EndPaint();
   void  DrawLine(UInt_t x1, UInt_t y1, UInt_t x2, UInt_t y2, const char *col = "#000000", UInt_t thick = 1);
   void  DrawDashLine(UInt_t x1, UInt_t y1, UInt_t x2, UInt_t y2, UInt_t nDash, const char *pDash, const char *col = "#000000", UInt_t thick = 1);
   void  DrawBox(Int_t x1, Int_t y1, Int_t x2, Int_t y2, const char *col = "#000000", UInt_t thick = 1, Int_t mode = 0);
   void  DrawRectangle(UInt_t x, UInt_t y, UInt_t w, UInt_t h, const char *col = "#000000", UInt_t thick = 1);
   void  FillRectangle(const char *col = 0, Int_t x = 0, Int_t y = 0, UInt_t width = 0, UInt_t height = 0);
   void  DrawPolyLine(UInt_t nn, TPoint *xy, const char *col = "#000000", UInt_t thick = 1, TImage::ECoordMode mode = kCoordModeOrigin);
   void  PutPixel(Int_t x, Int_t y, const char *col = "#000000");
   void  PolyPoint(UInt_t npt, TPoint *ppt, const char *col = "#000000", TImage::ECoordMode mode = kCoordModeOrigin);
   void  DrawSegments(UInt_t nseg, Segment_t *seg, const char *col = "#000000", UInt_t thick = 1);
   void  FillPolygon(UInt_t npt, TPoint *ppt, const char *col = "#000000", const char *stipple = 0, UInt_t w = 16, UInt_t h = 16);
   void  FillPolygon(UInt_t npt, TPoint *ppt, TImage *tile);
   void  CropPolygon(UInt_t npt, TPoint *ppt);
   void  DrawFillArea(UInt_t npt, TPoint *ppt, const char *col = "#000000", const char *stipple = 0, UInt_t w = 16, UInt_t h = 16);
   void  DrawFillArea(UInt_t npt, TPoint *ppt, TImage *tile);
   void  FillSpans(UInt_t npt, TPoint *ppt, UInt_t *widths, const char *col = "#000000", const char *stipple = 0, UInt_t w = 16, UInt_t h = 16);
   void  FillSpans(UInt_t npt, TPoint *ppt, UInt_t *widths, TImage *tile);
   void  CropSpans(UInt_t npt, TPoint *ppt, UInt_t *widths);
   void  CopyArea(TImage *dst, Int_t xsrc, Int_t ysrc, UInt_t w, UInt_t h, Int_t xdst = 0, Int_t ydst = 0, Int_t gfunc = 3, EColorChan chan = kAllChan);
   void  DrawCellArray(Int_t x1, Int_t y1, Int_t x2, Int_t y2, Int_t nx, Int_t ny, UInt_t *ic);

   // Input / output
   void  ReadImage(const char *file, EImageFileTypes type = TImage::kUnknown);
   void  WriteImage(const char *file, EImageFileTypes type = TImage::kUnknown); //*MENU*
   void  SetImage(const Double_t *imageData, UInt_t width, UInt_t height, TImagePalette *palette = 0);
   void  SetImage(const TArrayD &imageData, UInt_t width, TImagePalette *palette = 0);
   void  SetImage(const TVectorD &imageData, UInt_t width, TImagePalette *palette = 0);
   void  SetImage(Pixmap_t pxm, Pixmap_t mask = 0);

   // Utilities
   UInt_t GetWidth() const;
   UInt_t GetHeight() const;
   UInt_t GetScaledWidth() const;
   UInt_t GetScaledHeight() const;
   Bool_t IsValid() const { return fImage ? kTRUE : kFALSE; }
   ASImage         *GetImage() const { return fImage; }
   TImage          *GetScaledImage() const { return fScaledImage; }
   Pixmap_t         GetPixmap();
   Pixmap_t         GetMask();
   TArrayL         *GetPixels(Int_t x = 0, Int_t y = 0, UInt_t w = 0, UInt_t h = 0);
   TArrayD         *GetArray(UInt_t w = 0, UInt_t h = 0, TImagePalette *pal = gWebImagePalette);
   UInt_t          *GetArgbArray();
   UInt_t          *GetScanline(UInt_t y);

   static const ASVisual *GetVisual();
   static UInt_t AlphaBlend(UInt_t bot, UInt_t top);

   ClassDef(TASImage,1)  // Image display class
};

#endif
