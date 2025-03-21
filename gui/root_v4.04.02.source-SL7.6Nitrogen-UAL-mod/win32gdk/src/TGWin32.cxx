// @(#)root/win32gdk:$Name:  $:$Id: TGWin32.cxx,v 1.94 2005/04/26 16:36:48 brun Exp $
// Author: Rene Brun, Olivier Couet, Fons Rademakers, Bertrand Bellenot 27/11/01

/*************************************************************************
 * Copyright (C) 1995-2001, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGWin32                                                              //
//                                                                      //
// This class is the basic interface to the Win32 graphics system.      //
// It is  an implementation of the abstract TVirtualX class.            //
//                                                                      //
// This code was initially developed in the context of HIGZ and PAW     //
// by Olivier Couet (package X11INT).                                   //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TGWin32.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <process.h>
#include <wchar.h>
#include "gdk/gdkkeysyms.h"
#include "xatom.h"

#include "TROOT.h"
#include "TApplication.h"
#include "TColor.h"
#include "TPoint.h"
#include "TMath.h"
#include "TStorage.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TGFrame.h"
#include "TError.h"
#include "TException.h"
#include "TClassTable.h"
#include "KeySymbols.h"
#include "TWinNTSystem.h"
#include "TGWin32VirtualXProxy.h"
#include "TGWin32InterpreterProxy.h"
#include "TWin32SplashThread.h"
#include "TString.h"
#include "TObjString.h"
#include "TObjArray.h"

extern "C" {
void gdk_win32_draw_rectangle (GdkDrawable    *drawable,
				      GdkGC          *gc,
				      gint            filled,
				      gint            x,
				      gint            y,
				      gint            width,
				      gint            height);
void gdk_win32_draw_arc       (GdkDrawable    *drawable,
				      GdkGC          *gc,
				      gint            filled,
				      gint            x,
				      gint            y,
				      gint            width,
				      gint            height,
				      gint            angle1,
				      gint            angle2);
void gdk_win32_draw_polygon   (GdkDrawable    *drawable,
				      GdkGC          *gc,
				      gint            filled,
				      GdkPoint       *points,
				      gint            npoints);
void gdk_win32_draw_text      (GdkDrawable    *drawable,
				      GdkFont        *font,
				      GdkGC          *gc,
				      gint            x,
				      gint            y,
				      const gchar    *text,
				      gint            text_length);
void gdk_win32_draw_points    (GdkDrawable    *drawable,
				      GdkGC          *gc,
				      GdkPoint       *points,
				      gint            npoints);
void gdk_win32_draw_segments  (GdkDrawable    *drawable,
				      GdkGC          *gc,
				      GdkSegment     *segs,
				      gint            nsegs);
void gdk_win32_draw_lines     (GdkDrawable    *drawable,
				      GdkGC          *gc,
				      GdkPoint       *points,
				      gint            npoints);

};

/////////////////////////////////// globals //////////////////////////////////
int gdk_debug_level;

//void (*gDrawDIB)(ULong_t bmi, ULong_t bmbits, Int_t xpos, Int_t ypos) = 0;
//unsigned char *(*gGetBmBits)(Drawable_t wid, Int_t w, Int_t h) = 0;

GdkAtom gClipboardAtom = GDK_NONE;
static XWindow_t *gCws;         // gCws: pointer to the current window
static XWindow_t *gTws;         // gTws: temporary pointer

//
// gColors[0]           : background also used for b/w screen
// gColors[1]           : foreground also used for b/w screen
// gColors[2..kMAXCOL-1]: colors which can be set by SetColor
//
const Int_t kBIGGEST_RGB_VALUE = 65535;
const Int_t kMAXCOL = 1000;
static struct {
   Int_t defined;
   GdkColor color;
} gColors[kMAXCOL];

//
// Primitives Graphic Contexts global for all windows
//
const int kMAXGC = 7;
static GdkGC *gGClist[kMAXGC];
static GdkGC *gGCline;          // = gGClist[0];  // PolyLines
static GdkGC *gGCmark;          // = gGClist[1];  // PolyMarker
static GdkGC *gGCfill;          // = gGClist[2];  // Fill areas
static GdkGC *gGCtext;          // = gGClist[3];  // Text
static GdkGC *gGCinvt;          // = gGClist[4];  // Inverse text
static GdkGC *gGCdash;          // = gGClist[5];  // Dashed lines
static GdkGC *gGCpxmp;          // = gGClist[6];  // Pixmap management

static GdkGC *gGCecho;          // Input echo

static Int_t gFillHollow;       // Flag if fill style is hollow
static GdkPixmap *gFillPattern; // Fill pattern

//
// Text management
//
static char *gTextFont = "arial.ttf";      // Current font

//
// Markers
//
const Int_t kMAXMK = 100;
static struct {
   int type;
   int n;
   GdkPoint xy[kMAXMK];
} gMarker;                      // Point list to draw marker

//
// Keep style values for line GdkGC
//
static int  gLineStyle = GDK_LINE_SOLID;
static int  gCapStyle  = GDK_CAP_BUTT;
static int  gJoinStyle = GDK_JOIN_MITER;
static char gDashList[10];
static int  gDashLength = 0;
static int  gDashOffset = 0;
static int  gDashSize   = 0;

//
// Event masks
//
static ULong_t gMouseMask =
    GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_ENTER_NOTIFY_MASK
    | GDK_LEAVE_NOTIFY_MASK | GDK_POINTER_MOTION_MASK | GDK_KEY_PRESS_MASK
    | GDK_KEY_RELEASE_MASK;
static ULong_t gKeybdMask =
    GDK_BUTTON_PRESS_MASK | GDK_KEY_PRESS_MASK | GDK_ENTER_NOTIFY_MASK |
    GDK_LEAVE_NOTIFY_MASK;

//
// Data to create an invisible cursor
//
const char null_cursor_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
static GdkCursor *gNullCursor;
static HWND gConsoleWindow = 0;   // console window handle

//
// Data to create fill area interior style
//

static char p1_bits[] = {
   ~(0xaa), ~(0xaa), ~(0x55), ~(0x55), ~(0xaa), ~(0xaa), ~(0x55), ~(0x55),
       ~(0xaa), ~(0xaa), ~(0x55), ~(0x55),
   ~(0xaa), ~(0xaa), ~(0x55), ~(0x55), ~(0xaa), ~(0xaa), ~(0x55), ~(0x55),
       ~(0xaa), ~(0xaa), ~(0x55), ~(0x55),
   ~(0xaa), ~(0xaa), ~(0x55), ~(0x55), ~(0xaa), ~(0xaa), ~(0x55), ~(0x55)
};
static char p2_bits[] = {
   ~(0x44), ~(0x44), ~(0x11), ~(0x11), ~(0x44), ~(0x44), ~(0x11), ~(0x11),
       ~(0x44), ~(0x44), ~(0x11), ~(0x11),
   ~(0x44), ~(0x44), ~(0x11), ~(0x11), ~(0x44), ~(0x44), ~(0x11), ~(0x11),
       ~(0x44), ~(0x44), ~(0x11), ~(0x11),
   ~(0x44), ~(0x44), ~(0x11), ~(0x11), ~(0x44), ~(0x44), ~(0x11), ~(0x11)
};
static char p3_bits[] = {
   ~(0x00), ~(0x00), ~(0x44), ~(0x44), ~(0x00), ~(0x00), ~(0x11), ~(0x11),
       ~(0x00), ~(0x00), ~(0x44), ~(0x44),
   ~(0x00), ~(0x00), ~(0x11), ~(0x11), ~(0x00), ~(0x00), ~(0x44), ~(0x44),
       ~(0x00), ~(0x00), ~(0x11), ~(0x11),
   ~(0x00), ~(0x00), ~(0x44), ~(0x44), ~(0x00), ~(0x00), ~(0x11), ~(0x11)
};
static char p4_bits[] = {
   ~(0x80), ~(0x80), ~(0x40), ~(0x40), ~(0x20), ~(0x20), ~(0x10), ~(0x10),
       ~(0x08), ~(0x08), ~(0x04), ~(0x04),
   ~(0x02), ~(0x02), ~(0x01), ~(0x01), ~(0x80), ~(0x80), ~(0x40), ~(0x40),
       ~(0x20), ~(0x20), ~(0x10), ~(0x10),
   ~(0x08), ~(0x08), ~(0x04), ~(0x04), ~(0x02), ~(0x02), ~(0x01), ~(0x01)
};
static char p5_bits[] = {
   ~(0x20), ~(0x20), ~(0x40), ~(0x40), ~(0x80), ~(0x80), ~(0x01), ~(0x01),
       ~(0x02), ~(0x02), ~(0x04), ~(0x04),
   ~(0x08), ~(0x08), ~(0x10), ~(0x10), ~(0x20), ~(0x20), ~(0x40), ~(0x40),
       ~(0x80), ~(0x80), ~(0x01), ~(0x01),
   ~(0x02), ~(0x02), ~(0x04), ~(0x04), ~(0x08), ~(0x08), ~(0x10), ~(0x10)
};
static char p6_bits[] = {
   ~(0x44), ~(0x44), ~(0x44), ~(0x44), ~(0x44), ~(0x44), ~(0x44), ~(0x44),
       ~(0x44), ~(0x44), ~(0x44), ~(0x44),
   ~(0x44), ~(0x44), ~(0x44), ~(0x44), ~(0x44), ~(0x44), ~(0x44), ~(0x44),
       ~(0x44), ~(0x44), ~(0x44), ~(0x44),
   ~(0x44), ~(0x44), ~(0x44), ~(0x44), ~(0x44), ~(0x44), ~(0x44), ~(0x44)
};
static char p7_bits[] = {
   ~(0x00), ~(0x00), ~(0x00), ~(0x00), ~(0x00), ~(0x00), ~(0xff), ~(0xff),
       ~(0x00), ~(0x00), ~(0x00), ~(0x00),
   ~(0x00), ~(0x00), ~(0xff), ~(0xff), ~(0x00), ~(0x00), ~(0x00), ~(0x00),
       ~(0x00), ~(0x00), ~(0xff), ~(0xff),
   ~(0x00), ~(0x00), ~(0x00), ~(0x00), ~(0x00), ~(0x00), ~(0xff), ~(0xff)
};
static char p8_bits[] = {
   ~(0x11), ~(0x11), ~(0xb8), ~(0xb8), ~(0x7c), ~(0x7c), ~(0x3a), ~(0x3a),
       ~(0x11), ~(0x11), ~(0xa3), ~(0xa3),
   ~(0xc7), ~(0xc7), ~(0x8b), ~(0x8b), ~(0x11), ~(0x11), ~(0xb8), ~(0xb8),
       ~(0x7c), ~(0x7c), ~(0x3a), ~(0x3a),
   ~(0x11), ~(0x11), ~(0xa3), ~(0xa3), ~(0xc7), ~(0xc7), ~(0x8b), ~(0x8b)
};
static char p9_bits[] = {
   ~(0x10), ~(0x10), ~(0x10), ~(0x10), ~(0x28), ~(0x28), ~(0xc7), ~(0xc7),
       ~(0x01), ~(0x01), ~(0x01), ~(0x01),
   ~(0x82), ~(0x82), ~(0x7c), ~(0x7c), ~(0x10), ~(0x10), ~(0x10), ~(0x10),
       ~(0x28), ~(0x28), ~(0xc7), ~(0xc7),
   ~(0x01), ~(0x01), ~(0x01), ~(0x01), ~(0x82), ~(0x82), ~(0x7c), ~(0x7c)
};
static char p10_bits[] = {
   ~(0x10), ~(0x10), ~(0x10), ~(0x10), ~(0x10), ~(0x10), ~(0xff), ~(0xff),
       ~(0x01), ~(0x01), ~(0x01), ~(0x01),
   ~(0x01), ~(0x01), ~(0xff), ~(0xff), ~(0x10), ~(0x10), ~(0x10), ~(0x10),
       ~(0x10), ~(0x10), ~(0xff), ~(0xff),
   ~(0x01), ~(0x01), ~(0x01), ~(0x01), ~(0x01), ~(0x01), ~(0xff), ~(0xff)
};
static char p11_bits[] = {
   ~(0x08), ~(0x08), ~(0x49), ~(0x49), ~(0x2a), ~(0x2a), ~(0x1c), ~(0x1c),
       ~(0x2a), ~(0x2a), ~(0x49), ~(0x49),
   ~(0x08), ~(0x08), ~(0x00), ~(0x00), ~(0x80), ~(0x80), ~(0x94), ~(0x94),
       ~(0xa2), ~(0xa2), ~(0xc1), ~(0xc1),
   ~(0xa2), ~(0xa2), ~(0x94), ~(0x94), ~(0x80), ~(0x80), ~(0x00), ~(0x00)
};
static char p12_bits[] = {
   ~(0x1c), ~(0x1c), ~(0x22), ~(0x22), ~(0x41), ~(0x41), ~(0x41), ~(0x41),
       ~(0x41), ~(0x41), ~(0x22), ~(0x22),
   ~(0x1c), ~(0x1c), ~(0x00), ~(0x00), ~(0xc1), ~(0xc1), ~(0x22), ~(0x22),
       ~(0x14), ~(0x14), ~(0x14), ~(0x14),
   ~(0x14), ~(0x14), ~(0x22), ~(0x22), ~(0xc1), ~(0xc1), ~(0x00), ~(0x00)
};
static char p13_bits[] = {
   ~(0x01), ~(0x01), ~(0x82), ~(0x82), ~(0x44), ~(0x44), ~(0x28), ~(0x28),
       ~(0x10), ~(0x10), ~(0x28), ~(0x28),
   ~(0x44), ~(0x44), ~(0x82), ~(0x82), ~(0x01), ~(0x01), ~(0x82), ~(0x82),
       ~(0x44), ~(0x44), ~(0x28), ~(0x28),
   ~(0x10), ~(0x10), ~(0x28), ~(0x28), ~(0x44), ~(0x44), ~(0x82), ~(0x82)
};
static char p14_bits[] = {
   ~(0xff), ~(0xff), ~(0x11), ~(0x10), ~(0x11), ~(0x10), ~(0x11), ~(0x10),
       ~(0xf1), ~(0x1f), ~(0x11), ~(0x11),
   ~(0x11), ~(0x11), ~(0x11), ~(0x11), ~(0xff), ~(0x11), ~(0x01), ~(0x11),
       ~(0x01), ~(0x11), ~(0x01), ~(0x11),
   ~(0xff), ~(0xff), ~(0x01), ~(0x10), ~(0x01), ~(0x10), ~(0x01), ~(0x10)
};
static char p15_bits[] = {
   ~(0x22), ~(0x22), ~(0x55), ~(0x55), ~(0x22), ~(0x22), ~(0x00), ~(0x00),
       ~(0x88), ~(0x88), ~(0x55), ~(0x55),
   ~(0x88), ~(0x88), ~(0x00), ~(0x00), ~(0x22), ~(0x22), ~(0x55), ~(0x55),
       ~(0x22), ~(0x22), ~(0x00), ~(0x00),
   ~(0x88), ~(0x88), ~(0x55), ~(0x55), ~(0x88), ~(0x88), ~(0x00), ~(0x00)
};
static char p16_bits[] = {
   ~(0x0e), ~(0x0e), ~(0x11), ~(0x11), ~(0xe0), ~(0xe0), ~(0x00), ~(0x00),
       ~(0x0e), ~(0x0e), ~(0x11), ~(0x11),
   ~(0xe0), ~(0xe0), ~(0x00), ~(0x00), ~(0x0e), ~(0x0e), ~(0x11), ~(0x11),
       ~(0xe0), ~(0xe0), ~(0x00), ~(0x00),
   ~(0x0e), ~(0x0e), ~(0x11), ~(0x11), ~(0xe0), ~(0xe0), ~(0x00), ~(0x00)
};
static char p17_bits[] = {
   ~(0x44), ~(0x44), ~(0x22), ~(0x22), ~(0x11), ~(0x11), ~(0x00), ~(0x00),
       ~(0x44), ~(0x44), ~(0x22), ~(0x22),
   ~(0x11), ~(0x11), ~(0x00), ~(0x00), ~(0x44), ~(0x44), ~(0x22), ~(0x22),
       ~(0x11), ~(0x11), ~(0x00), ~(0x00),
   ~(0x44), ~(0x44), ~(0x22), ~(0x22), ~(0x11), ~(0x11), ~(0x00), ~(0x00)
};
static char p18_bits[] = {
   ~(0x11), ~(0x11), ~(0x22), ~(0x22), ~(0x44), ~(0x44), ~(0x00), ~(0x00),
       ~(0x11), ~(0x11), ~(0x22), ~(0x22),
   ~(0x44), ~(0x44), ~(0x00), ~(0x00), ~(0x11), ~(0x11), ~(0x22), ~(0x22),
       ~(0x44), ~(0x44), ~(0x00), ~(0x00),
   ~(0x11), ~(0x11), ~(0x22), ~(0x22), ~(0x44), ~(0x44), ~(0x00), ~(0x00)
};
static char p19_bits[] = {
   ~(0xe0), ~(0x03), ~(0x98), ~(0x0c), ~(0x84), ~(0x10), ~(0x42), ~(0x21),
       ~(0x42), ~(0x21), ~(0x21), ~(0x42),
   ~(0x19), ~(0x4c), ~(0x07), ~(0xf0), ~(0x19), ~(0x4c), ~(0x21), ~(0x42),
       ~(0x42), ~(0x21), ~(0x42), ~(0x21),
   ~(0x84), ~(0x10), ~(0x98), ~(0x0c), ~(0xe0), ~(0x03), ~(0x80), ~(0x00)
};
static char p20_bits[] = {
   ~(0x22), ~(0x22), ~(0x11), ~(0x11), ~(0x11), ~(0x11), ~(0x11), ~(0x11),
       ~(0x22), ~(0x22), ~(0x44), ~(0x44),
   ~(0x44), ~(0x44), ~(0x44), ~(0x44), ~(0x22), ~(0x22), ~(0x11), ~(0x11),
       ~(0x11), ~(0x11), ~(0x11), ~(0x11),
   ~(0x22), ~(0x22), ~(0x44), ~(0x44), ~(0x44), ~(0x44), ~(0x44), ~(0x44)
};
static char p21_bits[] = {
   ~(0xf1), ~(0xf1), ~(0x10), ~(0x10), ~(0x10), ~(0x10), ~(0x10), ~(0x10),
       ~(0x1f), ~(0x1f), ~(0x01), ~(0x01),
   ~(0x01), ~(0x01), ~(0x01), ~(0x01), ~(0xf1), ~(0xf1), ~(0x10), ~(0x10),
       ~(0x10), ~(0x10), ~(0x10), ~(0x10),
   ~(0x1f), ~(0x1f), ~(0x01), ~(0x01), ~(0x01), ~(0x01), ~(0x01), ~(0x01)
};
static char p22_bits[] = {
   ~(0x8f), ~(0x8f), ~(0x08), ~(0x08), ~(0x08), ~(0x08), ~(0x08), ~(0x08),
       ~(0xf8), ~(0xf8), ~(0x80), ~(0x80),
   ~(0x80), ~(0x80), ~(0x80), ~(0x80), ~(0x8f), ~(0x8f), ~(0x08), ~(0x08),
       ~(0x08), ~(0x08), ~(0x08), ~(0x08),
   ~(0xf8), ~(0xf8), ~(0x80), ~(0x80), ~(0x80), ~(0x80), ~(0x80), ~(0x80)
};
static char p23_bits[] = {
   ~(0xAA), ~(0xAA), ~(0x55), ~(0x55), ~(0x6a), ~(0x6a), ~(0x74), ~(0x74),
       ~(0x78), ~(0x78), ~(0x74), ~(0x74),
   ~(0x6a), ~(0x6a), ~(0x55), ~(0x55), ~(0xAA), ~(0xAA), ~(0x55), ~(0x55),
       ~(0x6a), ~(0x6a), ~(0x74), ~(0x74),
   ~(0x78), ~(0x78), ~(0x74), ~(0x74), ~(0x6a), ~(0x6a), ~(0x55), ~(0x55)
};
static char p24_bits[] = {
   ~(0x80), ~(0x00), ~(0xc0), ~(0x00), ~(0xea), ~(0xa8), ~(0xd5), ~(0x54),
       ~(0xea), ~(0xa8), ~(0xd5), ~(0x54),
   ~(0xeb), ~(0xe8), ~(0xd5), ~(0xd4), ~(0xe8), ~(0xe8), ~(0xd4), ~(0xd4),
       ~(0xa8), ~(0xe8), ~(0x54), ~(0xd5),
   ~(0xa8), ~(0xea), ~(0x54), ~(0xd5), ~(0xfc), ~(0xff), ~(0xfe), ~(0xff)
};
static char p25_bits[] = {
   ~(0x80), ~(0x00), ~(0xc0), ~(0x00), ~(0xe0), ~(0x00), ~(0xf0), ~(0x00),
       ~(0xff), ~(0xf0), ~(0xff), ~(0xf0),
   ~(0xfb), ~(0xf0), ~(0xf9), ~(0xf0), ~(0xf8), ~(0xf0), ~(0xf8), ~(0x70),
       ~(0xf8), ~(0x30), ~(0xff), ~(0xf0),
   ~(0xff), ~(0xf8), ~(0xff), ~(0xfc), ~(0xff), ~(0xfe), ~(0xff), ~(0xff)
};

static bool gdk_initialized = false;


//---- MWM Hints stuff

struct MWMHintsProperty_t {
   Handle_t fFlags;
   Handle_t fFunctions;
   Handle_t fDecorations;
   Int_t fInputMode;
};

//---- hints

const ULong_t kMWMHintsFunctions = BIT(0);
const ULong_t kMWMHintsDecorations = BIT(1);
const ULong_t kMWMHintsInputMode = BIT(2);

const Int_t kPropMotifWMHintsElements = 4;
const Int_t kPropMWMHintElements = kPropMotifWMHintsElements;


//---- Key symbol mapping

struct KeySymbolMap_t {
   KeySym fXKeySym;
   EKeySym fKeySym;
};

static char *keyCodeToString[] = {
   "",                          /* 0x000 */
   "",                          /* 0x001, VK_LBUTTON */
   "",                          /* 0x002, VK_RBUTTON */
   "",                          /* 0x003, VK_CANCEL */
   "",                          /* 0x004, VK_MBUTTON */
   "",                          /* 0x005 */
   "",                          /* 0x006 */
   "",                          /* 0x007 */
   "\015",                      /* 0x008, VK_BACK */
   "\t",                        /* 0x009, VK_TAB */
   "",                          /* 0x00A */
   "",                          /* 0x00B */
   "",                          /* 0x00C, VK_CLEAR */
   "\r",                        /* 0x00D, VK_RETURN */
   "",                          /* 0x00E */
   "",                          /* 0x00F */
   "",                          /* 0x010, VK_SHIFT */
   "",                          /* 0x011, VK_CONTROL */
   "",                          /* 0x012, VK_MENU */
   "",                          /* 0x013, VK_PAUSE */
   "",                          /* 0x014, VK_CAPITAL */
   "",                          /* 0x015, VK_KANA */
   "",                          /* 0x016 */
   "",                          /* 0x017 */
   "",                          /* 0x018 */
   "",                          /* 0x019, VK_KANJI */
   "",                          /* 0x01A */
   "",                          /* 0x01B, VK_ESCAPE */
   "",                          /* 0x01C, VK_CONVERT */
   "",                          /* 0x01D, VK_NONCONVERT */
   "",                          /* 0x01E */
   "",                          /* 0x01F */
   " ",                         /* 0x020, VK_SPACE */
   "",                          /* 0x021, VK_PRIOR */
   "",                          /* 0x022, VK_NEXT */
   "",                          /* 0x023, VK_END */
   "",                          /* 0x024, VK_HOME */
   "",                          /* 0x025, VK_LEFT */
   "",                          /* 0x026, VK_UP */
   "",                          /* 0x027, VK_RIGHT */
   "",                          /* 0x028, VK_DOWN */
   "",                          /* 0x029, VK_SELECT */
   "",                          /* 0x02A, VK_PRINT */
   "",                          /* 0x02B, VK_EXECUTE */
   "",                          /* 0x02C, VK_SNAPSHOT */
   "",                          /* 0x02D, VK_INSERT */
   "\037",                      /* 0x02E, VK_DELETE */
   "",                          /* 0x02F, VK_HELP */
};

//---- Mapping table of all non-trivial mappings (the ASCII keys map
//---- one to one so are not included)

static KeySymbolMap_t gKeyMap[] = {
   {GDK_Escape, kKey_Escape},
   {GDK_Tab, kKey_Tab},
#ifndef GDK_ISO_Left_Tab
   {0xFE20, kKey_Backtab},
#else
   {GDK_ISO_Left_Tab, kKey_Backtab},
#endif
   {GDK_BackSpace, kKey_Backspace},
   {GDK_Return, kKey_Return},
   {GDK_Insert, kKey_Insert},
   {GDK_Delete, kKey_Delete},
   {GDK_Clear, kKey_Delete},
   {GDK_Pause, kKey_Pause},
   {GDK_Print, kKey_Print},
   {0x1005FF60, kKey_SysReq},   // hardcoded Sun SysReq
   {0x1007ff00, kKey_SysReq},   // hardcoded X386 SysReq
   {GDK_Home, kKey_Home},       // cursor movement
   {GDK_End, kKey_End},
   {GDK_Left, kKey_Left},
   {GDK_Up, kKey_Up},
   {GDK_Right, kKey_Right},
   {GDK_Down, kKey_Down},
   {GDK_Prior, kKey_Prior},
   {GDK_Next, kKey_Next},
   {GDK_Shift_L, kKey_Shift},   // modifiers
   {GDK_Shift_R, kKey_Shift},
   {GDK_Shift_Lock, kKey_Shift},
   {GDK_Control_L, kKey_Control},
   {GDK_Control_R, kKey_Control},
   {GDK_Meta_L, kKey_Meta},
   {GDK_Meta_R, kKey_Meta},
   {GDK_Alt_L, kKey_Alt},
   {GDK_Alt_R, kKey_Alt},
   {GDK_Caps_Lock, kKey_CapsLock},
   {GDK_Num_Lock, kKey_NumLock},
   {GDK_Scroll_Lock, kKey_ScrollLock},
   {GDK_KP_Space, kKey_Space},  // numeric keypad
   {GDK_KP_Tab, kKey_Tab},
   {GDK_KP_Enter, kKey_Enter},
   {GDK_KP_Equal, kKey_Equal},
   {GDK_KP_Multiply, kKey_Asterisk},
   {GDK_KP_Add, kKey_Plus},
   {GDK_KP_Separator, kKey_Comma},
   {GDK_KP_Subtract, kKey_Minus},
   {GDK_KP_Decimal, kKey_Period},
   {GDK_KP_Divide, kKey_Slash},
   {0, (EKeySym) 0}
};


//////////// internal classes & structures (very private) ////////////////

struct XWindow_t {
   Int_t    open;                 // 1 if the window is open, 0 if not
   Int_t    double_buffer;        // 1 if the double buffer is on, 0 if not
   Int_t    ispixmap;             // 1 if pixmap, 0 if not
   GdkDrawable *drawing;          // drawing area, equal to window or buffer
   GdkDrawable *window;           // win32 window
   GdkDrawable *buffer;           // pixmap used for double buffer
   UInt_t   width;                // width of the window
   UInt_t   height;               // height of the window
   Int_t    clip;                 // 1 if the clipping is on
   Int_t    xclip;                // x coordinate of the clipping rectangle
   Int_t    yclip;                // y coordinate of the clipping rectangle
   UInt_t   wclip;                // width of the clipping rectangle
   UInt_t   hclip;                // height of the clipping rectangle
   ULong_t *new_colors;           // new image colors (after processing)
   Int_t    ncolors;              // number of different colors
};


/////////////////////static auxilary functions /////////////////////////////////
//______________________________________________________________________________
static Int_t _lookup_string(Event_t * event, char *buf, Int_t buflen)
{
   int i;
   int n = event->fUser[1];
   if (n > 0) {
      for (i = 0; i < n; i++) {
         buf[i] = event->fUser[2 + i];
      }
      buf[n] = 0;
   } else {
      buf[0] = 0;
   }
   if (event->fCode <= 0x20) {
      strncpy(buf, keyCodeToString[event->fCode], buflen - 1);
   }
   return n;
}

//______________________________________________________________________________
inline void SplitLong(Long_t ll, Long_t & i1, Long_t & i2)
{
   union {
      Long_t l;
      Int_t i[2];
   } conv;

   conv.l    = 0L;
   conv.i[0] = 0;
   conv.i[1] = 0;

   conv.l = ll;
   i1 = conv.i[0];
   i2 = conv.i[1];
}

//______________________________________________________________________________
inline void AsmLong(Long_t i1, Long_t i2, Long_t & ll)
{
   union {
      Long_t l;
      Int_t i[2];
   } conv;

   conv.i[0] = (Int_t) i1;
   conv.i[1] = (Int_t) i2;
   ll = conv.l;
}

//______________________________________________________________________________
static BOOL CALLBACK EnumChildProc(HWND hwndChild, LPARAM lParam)
{
   // Make sure the child window is visible.

   ::ShowWindow(hwndChild, SW_SHOWNORMAL);
   return TRUE;
}

//______________________________________________________________________________
static void _ChangeProperty(HWND w, char *np, char *dp, int n, Atom_t type)
{
   HGLOBAL hMem;
   char *p;

   hMem = ::GetProp(w, np);
   if (hMem != NULL) {
      ::GlobalFree(hMem);
   }
   hMem = ::GlobalAlloc(GHND, n + sizeof(Atom_t));
   p = (char *) ::GlobalLock(hMem);
   memcpy(p, &type, sizeof(Atom_t));
   memcpy(p + sizeof(Atom_t), dp, n);
   ::GlobalUnlock(hMem);
   ::SetProp(w, np, hMem);
   ::GlobalFree(hMem);
}

//______________________________________________________________________________
static void W32ChangeProperty(HWND w, Atom_t property, Atom_t type,
                       int format, int mode, const unsigned char *data,
                       int nelements)
{
   //

   char *atomName;
   char buffer[256];
   char *p, *s;
   int len;
   char propName[32];

   if (mode == GDK_PROP_MODE_REPLACE || mode == GDK_PROP_MODE_PREPEND) {
      len = (int) ::GlobalGetAtomName(property, buffer, sizeof(buffer));
      if ((atomName = (char *) malloc(len + 1)) == NULL) {
         return;
      } else {
         strcpy(atomName, buffer);
      }
      sprintf(propName, "#0x%0.4x", atomName);
      _ChangeProperty(w, propName, (char *) data, nelements, type);
      free(atomName);
   }
}

//______________________________________________________________________________
static int _GetWindowProperty(GdkWindow * id, Atom_t property, Long_t long_offset,
                       Long_t long_length, Bool_t delete_it, Atom_t req_type,
                       Atom_t * actual_type_return,
                       Int_t * actual_format_return, ULong_t * nitems_return,
                       ULong_t * bytes_after_return, UChar_t ** prop_return)
{
   //

   if (!id) return 0;

   char *atomName;
   char *data, *destPtr;
   char propName[32];
   HGLOBAL handle;
   HGLOBAL hMem;
   HWND w;

   w = (HWND) GDK_DRAWABLE_XID(id);

   if (::IsClipboardFormatAvailable(CF_TEXT) && ::OpenClipboard(NULL)) {
      handle = ::GetClipboardData(CF_TEXT);
      if (handle != NULL) {
         data = (char *) ::GlobalLock(handle);
         *nitems_return = strlen(data);
         *prop_return = (UChar_t *) malloc(*nitems_return + 1);
         destPtr = (char *) *prop_return;
         while (*data != '\0') {
            if (*data != '\r') {
               *destPtr = *data;
               destPtr++;
            }
            data++;
         }
         *destPtr = '\0';
         ::GlobalUnlock(handle);
         *actual_type_return = XA_STRING;
         *bytes_after_return = 0;
      }
      ::CloseClipboard();
      return 1;
   }
   if (delete_it) {
      ::RemoveProp(w, propName);
   }
   return 1;
}

//______________________________________________________________________________
static ULong_t GetPixelImage(Drawable_t id, Int_t x, Int_t y)
{
   //

   if (!id) return 0;

   GdkImage *image = (GdkImage *)id;
   ULong_t pixel;

   if (image->depth == 1) {
      pixel = (((char *) image->mem)[y * image->bpl + (x >> 3)] & (1 << (7 - (x & 0x7)))) != 0;
   } else {
      UChar_t *pixelp = (UChar_t *) image->mem + y * image->bpl + x * image->bpp;
      switch (image->bpp) {
         case 1:
            pixel = *pixelp;
            break;
         // Windows is always LSB, no need to check image->byte_order.
         case 2:
            pixel = pixelp[0] | (pixelp[1] << 8);
            break;
         case 3:
            pixel = pixelp[0] | (pixelp[1] << 8) | (pixelp[2] << 16);
            break;
         case 4:
            pixel = pixelp[0] | (pixelp[1] << 8) | (pixelp[2] << 16);
            break;
      }
   }
   return pixel;
}

//______________________________________________________________________________
static void CollectImageColors(ULong_t pixel, ULong_t * &orgcolors,
                                 Int_t & ncolors, Int_t & maxcolors)
{
   // Collect in orgcolors all different original image colors.

   if (maxcolors == 0) {
      ncolors = 0;
      maxcolors = 100;
      orgcolors = (ULong_t*) ::operator new(maxcolors*sizeof(ULong_t));
   }

   for (int i = 0; i < ncolors; i++) {
      if (pixel == orgcolors[i]) return;
   }
   if (ncolors >= maxcolors) {
      orgcolors = (ULong_t *) TStorage::ReAlloc(orgcolors,
                                                maxcolors * 2 *
                                                sizeof(ULong_t),
                                                maxcolors *
                                                sizeof(ULong_t));
      maxcolors *= 2;
   }
   orgcolors[ncolors++] = pixel;
}

//______________________________________________________________________________
static char *EventMask2String(UInt_t evmask)
{
   // debug function for printing event mask

   static char bfr[500];
   char *p = bfr;

   *p = '\0';
#define BITmask(x) \
  if (evmask & k##x##Mask) \
    p += sprintf (p, "%s" #x, (p > bfr ? " " : ""))
   BITmask(Exposure);
   BITmask(PointerMotion);
   BITmask(ButtonMotion);
   BITmask(ButtonPress);
   BITmask(ButtonRelease);
   BITmask(KeyPress);
   BITmask(KeyRelease);
   BITmask(EnterWindow);
   BITmask(LeaveWindow);
   BITmask(FocusChange);
   BITmask(StructureNotify);
#undef BITmask

   return bfr;
}

//______________________________________________________________________________
static void TGWin32SetConsoleWindowName()
{
   //

   char pszNewWindowTitle[1024]; // contains fabricated WindowTitle
   char pszOldWindowTitle[1024]; // contains original WindowTitle

   ::GetConsoleTitle(pszOldWindowTitle, 1024);
   // format a "unique" NewWindowTitle
   wsprintf(pszNewWindowTitle,"%d/%d", ::GetTickCount(), ::GetCurrentProcessId());
   // change current window title
   ::SetConsoleTitle(pszNewWindowTitle);
   // ensure window title has been updated
   ::Sleep(40);
   // look for NewWindowTitle
   gConsoleWindow = ::FindWindow(0, pszNewWindowTitle);
   // restore original window title
   ::ShowWindow(gConsoleWindow, SW_RESTORE);
   ::SetForegroundWindow(gConsoleWindow);
   ::SetConsoleTitle("ROOT session");
}


///////////////////////////////////////////////////////////////////////////////
class TGWin32MainThread {

public:
   void     *fHandle;                     // handle of server (aka command) thread
   DWORD    fId;                          // id of server (aka command) thread
   static LPCRITICAL_SECTION  fCritSec;      // general mutex
   static LPCRITICAL_SECTION  fMessageMutex; // message queue mutex

   TGWin32MainThread();
   ~TGWin32MainThread();
   static void LockMSG();
   static void UnlockMSG();
};

TGWin32MainThread *gMainThread = 0;
LPCRITICAL_SECTION TGWin32MainThread::fCritSec = 0;
LPCRITICAL_SECTION TGWin32MainThread::fMessageMutex = 0;


//______________________________________________________________________________
TGWin32MainThread::~TGWin32MainThread()
{
   // dtor

   if (fCritSec) {
      ::LeaveCriticalSection(fCritSec);
      ::DeleteCriticalSection(fCritSec);
   }
   fCritSec = 0;

   if (fMessageMutex) {
      ::LeaveCriticalSection(fMessageMutex);
      ::DeleteCriticalSection(fMessageMutex);
   }
   fMessageMutex = 0;

   if(fHandle) {
      ::PostThreadMessage(fId, WM_QUIT, 0, 0);
      ::CloseHandle(fHandle);
   }
   fHandle = 0;
}

//______________________________________________________________________________
void TGWin32MainThread::LockMSG()
{
   // lock message queue

   if (fMessageMutex) ::EnterCriticalSection(fMessageMutex);
}

//______________________________________________________________________________
void TGWin32MainThread::UnlockMSG()
{
   // unlock message queue

   if (fMessageMutex) ::LeaveCriticalSection(fMessageMutex);
}

//______________________________________________________________________________
static Bool_t MessageProcessingFunc(MSG *msg)
{
   // windows message processing procedure

   Bool_t ret = kFALSE;

   if (msg->message == TGWin32ProxyBase::fgPostMessageId) {
      if (msg->wParam) {
         TGWin32ProxyBase *proxy = (TGWin32ProxyBase*)msg->wParam;
         proxy->ExecuteCallBack(kTRUE);
      } else {
         ret = kTRUE;
      }
   } else if (msg->message == TGWin32ProxyBase::fgPingMessageId) {
      TGWin32ProxyBase::GlobalUnlock();
   } else {
      if ( (msg->message >= WM_NCMOUSEMOVE) &&
           (msg->message <= WM_NCMBUTTONDBLCLK) ) {
         TGWin32ProxyBase::GlobalLock();
      }
      TGWin32MainThread::LockMSG();
      TranslateMessage(msg);
      DispatchMessage(msg);
      TGWin32MainThread::UnlockMSG();
   }
   return ret;
}

///////////////////////////////////////////////////////////////////////////////
class TGWin32RefreshTimer : public TTimer {

public:
   TGWin32RefreshTimer() : TTimer(100, kTRUE) { if (gSystem) gSystem->AddTimer(this); }
   ~TGWin32RefreshTimer() {}
   Bool_t Notify()
   {
      Reset();
      MSG msg;

      while (::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE)) {
         ::PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE);
         MessageProcessingFunc(&msg);
      }
      return kFALSE;
   }
};

//______________________________________________________________________________
static DWORD WINAPI MessageProcessingLoop(void *p)
{
   // thread for processing windows messages (aka Main/Server thread)

   MSG msg;
   Int_t erret;
   Bool_t endLoop = kFALSE;

   // force to create message queue
   ::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

   // periodically we refresh windows
   TGWin32RefreshTimer *refersh = new TGWin32RefreshTimer();

   while (!endLoop) {
      erret = ::GetMessage(&msg, NULL, NULL, NULL);
      if (erret <= 0) endLoop = kTRUE;
      endLoop = MessageProcessingFunc(&msg);
   }

   TGWin32::Instance()->CloseDisplay();
   delete refersh;

   // exit thread
   if (erret == -1) {
      erret = ::GetLastError();
      Error("MsgLoop", "Error in GetMessage");
      ::ExitThread(-1);
   } else {
      ::ExitThread(0);
   }
   return 0;
}

//______________________________________________________________________________
TGWin32MainThread::TGWin32MainThread()
{
   // constructor

   fCritSec = new CRITICAL_SECTION;
   ::InitializeCriticalSection(fCritSec);
   fMessageMutex = new CRITICAL_SECTION;
   ::InitializeCriticalSection(fMessageMutex);
   fHandle = ::CreateThread( NULL, 0, &MessageProcessingLoop, 0, 0, &fId );
}


///////////////////////// TGWin32 implementation ///////////////////////////////
ClassImp(TGWin32)

//______________________________________________________________________________
TGWin32::TGWin32()
{
   // Default constructor.

   fScreenNumber = 0;
   fWindows = 0;
}

//______________________________________________________________________________
TGWin32::TGWin32(const char *name, const char *title) : TVirtualX(name,title)
{
   // Normal Constructor.

   fScreenNumber = 0;
   fHasTTFonts = kFALSE;
   fTextAlignH = 1;
   fTextAlignV = 1;
   fTextAlign = 7;
   fTextMagnitude = 1;
   fCharacterUpX = 1;
   fCharacterUpY = 1;
   fDrawMode = kCopy;
   fWindows = 0;
   fMaxNumberOfWindows = 10;
   fXEvent = 0;
   fFillColorModified = kFALSE;
   fFillStyleModified = kFALSE;
   fLineColorModified = kFALSE;
   fPenModified = kFALSE;
   fMarkerStyleModified = kFALSE;
   fMarkerColorModified = kFALSE;

   fWindows = (XWindow_t*) TStorage::Alloc(fMaxNumberOfWindows*sizeof(XWindow_t));
   for (int i = 0; i < fMaxNumberOfWindows; i++) fWindows[i].open = 0;

   if (NeedSplash()) {
      new TWin32SplashThread(FALSE);
   }

   // initialize GUI thread and proxy objects
   if (!gROOT->IsBatch() && !gMainThread) {
      gMainThread = new TGWin32MainThread();
      TGWin32ProxyBase::fgMainThreadId = gMainThread->fId;
      TGWin32VirtualXProxy::fgRealObject = this;
      gPtr2VirtualX = &TGWin32VirtualXProxy::ProxyObject;
      gPtr2Interpreter = &TGWin32InterpreterProxy::ProxyObject;
   }
   gDrawDIB  = &(TGWin32::DrawDIB);
   gGetBmBits  = &(TGWin32::GetBmBits);
   gDIB2Pixmap = &(TGWin32::DIB2Pixmap);
   TGWin32SetConsoleWindowName();
}

//______________________________________________________________________________
TGWin32::~TGWin32()
{
   // destructor.

   CloseDisplay();
}

//______________________________________________________________________________
Bool_t TGWin32::IsCmdThread() const
{
   // returns kTRUE if we are inside cmd/server thread

   return (::GetCurrentThreadId() == TGWin32ProxyBase::fgMainThreadId);
}

//______________________________________________________________________________
Bool_t TGWin32::NeedSplash()
{
   // return kFALSE if option "-l" was specified as main programm command arg

   if (gROOT->IsBatch() || !gApplication) return kFALSE;

   TString arg = gSystem->BaseName(gApplication->Argv(0));

   if ((arg != "root") && (arg != "rootn") &&
       (arg != "root.exe") && (arg != "rootn.exe")) return kFALSE;

   if (gROOT->IsBatch()) return kFALSE;

   for(int i=1; i<gApplication->Argc(); i++) {
      arg = gApplication->Argv(i);
      arg.Strip(TString::kBoth);

      if ((arg == "-l") || (arg == "-b")) {
         return kFALSE;
      }
   }
   return TRUE;
}

//______________________________________________________________________________
void TGWin32::CloseDisplay()
{
   // close display (terminate server/gMainThread thread)

   // disable any processing while exiting
   TGWin32ProxyBase::GlobalLock();

   // terminate server thread
   gPtr2VirtualX = 0;
   gPtr2Interpreter = 0;
   gVirtualX = TGWin32VirtualXProxy::RealObject();
   gInterpreter = TGWin32InterpreterProxy::RealObject();

   // The lock above does not work, so at least
   // minimize the risk
   TGWin32MainThread *delThread = gMainThread;
   if (gMainThread) {
      gMainThread = 0;
      delete delThread;
   }

   TGWin32ProxyBase::fgMainThreadId = 0;

   // terminate ROOT logo splash thread
   TWin32SplashThread *delSplash = gSplash;
   if (gSplash) {
      gSplash = 0;
      delete delSplash;
   }

   if (fWindows) TStorage::Dealloc(fWindows);
   fWindows = 0;

   if (fXEvent) gdk_event_free((GdkEvent*)fXEvent);

   TGWin32ProxyBase::GlobalUnlock();

   gROOT->SetBatch(kTRUE); // no GUI is possible
}

//______________________________________________________________________________
void  TGWin32::Lock()
{
   //

   if (gMainThread && gMainThread->fCritSec) ::EnterCriticalSection(gMainThread->fCritSec);
}

//______________________________________________________________________________
void TGWin32::Unlock()
{
   //

   if (gMainThread && gMainThread->fCritSec) ::LeaveCriticalSection(gMainThread->fCritSec);
}

//______________________________________________________________________________
Bool_t TGWin32::Init(void *display)
{
   // Initialize Win32 system. Returns kFALSE in case of failure.

   if (!gdk_initialized) {
      if (!gdk_init_check(NULL, NULL)) return kFALSE;
      gdk_initialized = true;
   }

   if (!gClipboardAtom) {
      gClipboardAtom = gdk_atom_intern("CLIPBOARD", kFALSE);
   }

   return kTRUE;
}

//______________________________________________________________________________
Int_t TGWin32::OpenDisplay(const char *dpyName)
{
   // Open the display. Return -1 if the opening fails, 0 when ok.

   GdkPixmap *pixmp1, *pixmp2;
   GdkColor fore, back;
   GdkColor color;
   GdkGCValues gcvals;
   int i;

   if (!Init((void*)dpyName)) {
      return -1;
   }

   if (gDebug <= 4) {
      gdk_debug_level = gDebug;
   } else {
      gdk_debug_level = 0;
   }

   fScreenNumber = 0;           //DefaultScreen(fDisplay);
   fVisual = gdk_visual_get_best();
   fColormap = gdk_colormap_get_system();
   fDepth = gdk_visual_get_best_depth();

   gColors[1].defined = 1;      // default foreground
   gdk_color_black((GdkColormap *)fColormap, &gColors[1].color);

   gColors[0].defined = 1;      // default background
   gdk_color_white((GdkColormap *)fColormap, &gColors[0].color);

   // Create primitives graphic contexts
   for (i = 0; i < kMAXGC; i++) {
      gGClist[i]  = gdk_gc_new(GDK_ROOT_PARENT());
      gdk_gc_set_foreground(gGClist[i], &gColors[1].color);
      gdk_gc_set_background(gGClist[i], &gColors[0].color);
   }

   gGCline = gGClist[0];        // PolyLines
   gGCmark = gGClist[1];        // PolyMarker
   gGCfill = gGClist[2];        // Fill areas
   gGCtext = gGClist[3];        // Text
   gGCinvt = gGClist[4];        // Inverse text
   gGCdash = gGClist[5];        // Dashed lines
   gGCpxmp = gGClist[6];        // Pixmap management

   gdk_gc_get_values(gGCtext, &gcvals);
   gdk_gc_set_foreground(gGCinvt, &gcvals.background);
   gdk_gc_set_background(gGCinvt, &gcvals.foreground);

   // Create input echo graphic context
   GdkGCValues echov;
   gdk_color_black(fColormap, &echov.foreground);	// = BlackPixel(fDisplay, fScreenNumber);
   gdk_color_white(fColormap, &echov.background);	// = WhitePixel(fDisplay, fScreenNumber);
   echov.function = GDK_INVERT;
   echov.subwindow_mode = GDK_CLIP_BY_CHILDREN;
   gGCecho =
       gdk_gc_new_with_values((GdkWindow *) GDK_ROOT_PARENT(), &echov,
                              (GdkGCValuesMask) (GDK_GC_FOREGROUND |
                                                 GDK_GC_BACKGROUND |
                                                 GDK_GC_FUNCTION |
                                                 GDK_GC_SUBWINDOW));
   // Create a null cursor
   pixmp1 = gdk_bitmap_create_from_data(GDK_ROOT_PARENT(),
                                       (const char *)null_cursor_bits, 16,16);

   pixmp2 = gdk_bitmap_create_from_data(GDK_ROOT_PARENT(),
                                       (const char *)null_cursor_bits, 16, 16);

   gNullCursor = gdk_cursor_new_from_pixmap((GdkDrawable *)pixmp1, (GdkDrawable *)pixmp2,
                                             &fore, &back, 0, 0);
   // Create cursors
   fCursors[kBottomLeft] = gdk_cursor_new(GDK_BOTTOM_LEFT_CORNER);
   fCursors[kBottomRight] = gdk_cursor_new(GDK_BOTTOM_RIGHT_CORNER);
   fCursors[kTopLeft] = gdk_cursor_new(GDK_TOP_LEFT_CORNER);
   fCursors[kTopRight] = gdk_cursor_new(GDK_TOP_RIGHT_CORNER);
   fCursors[kBottomSide] =  gdk_cursor_new(GDK_BOTTOM_SIDE);
   fCursors[kLeftSide] = gdk_cursor_new(GDK_LEFT_SIDE);
   fCursors[kTopSide] = gdk_cursor_new(GDK_TOP_SIDE);
   fCursors[kRightSide] = gdk_cursor_new(GDK_RIGHT_SIDE);
   fCursors[kMove] = gdk_cursor_new(GDK_FLEUR);
   fCursors[kCross] =gdk_cursor_new(GDK_CROSSHAIR);
   fCursors[kArrowHor] = gdk_cursor_new(GDK_SB_H_DOUBLE_ARROW);
   fCursors[kArrowVer] = gdk_cursor_new(GDK_SB_V_DOUBLE_ARROW);
   fCursors[kHand] = gdk_cursor_new(GDK_HAND2);
   fCursors[kRotate] = gdk_cursor_new(GDK_EXCHANGE);
   fCursors[kPointer] = gdk_cursor_new( GDK_LEFT_PTR);
   fCursors[kArrowRight] = gdk_cursor_new(GDK_ARROW);
   fCursors[kCaret] =  gdk_cursor_new(GDK_XTERM);
   fCursors[kWatch] = gdk_cursor_new(GDK_WATCH);

   // Setup color information
   fRedDiv = fGreenDiv = fBlueDiv = fRedShift = fGreenShift = fBlueShift = -1;

   if ( gdk_visual_get_best_type() == GDK_VISUAL_TRUE_COLOR) {
      int i;
      for (i = 0; i < int(sizeof(fVisual->blue_mask)*kBitsPerByte); i++) {
         if (fBlueShift == -1 && ((fVisual->blue_mask >> i) & 1)) {
            fBlueShift = i;
         }
         if ((fVisual->blue_mask >> i) == 1) {
            fBlueDiv = sizeof(UShort_t)*kBitsPerByte - i - 1 + fBlueShift;
            break;
         }
      }
      for (i = 0; i < int(sizeof(fVisual->green_mask)*kBitsPerByte); i++) {
         if (fGreenShift == -1 && ((fVisual->green_mask >> i) & 1)) {
            fGreenShift = i;
         }
         if ((fVisual->green_mask >> i) == 1) {
            fGreenDiv = sizeof(UShort_t)*kBitsPerByte - i - 1 + fGreenShift;
            break;
         }
      }
      for (i = 0; i < int(sizeof(fVisual->red_mask)*kBitsPerByte); i++) {
         if (fRedShift == -1 && ((fVisual->red_mask >> i) & 1)) {
            fRedShift = i;
         }
         if ((fVisual->red_mask >> i) == 1) {
            fRedDiv = sizeof(UShort_t)*kBitsPerByte - i - 1 + fRedShift;
            break;
         }
      }
   }

   SetName("Win32TTF");
   SetTitle("ROOT interface to Win32 with TrueType fonts");

   if (!TTF::IsInitialized()) TTF::Init();

   if (fDepth > 8) {
      TTF::SetSmoothing(kTRUE);
   } else {
      TTF::SetSmoothing(kFALSE);
   }

   fHasTTFonts = kTRUE;
   return 0;
}

//______________________________________________________________________________
Bool_t TGWin32::AllocColor(GdkColormap *cmap, GdkColor *color)
{
   // Allocate color in colormap. If we are on an <= 8 plane machine
   // we will use XAllocColor. If we are on a >= 15 (15, 16 or 24) plane
   // true color machine we will calculate the pixel value using:
   // for 15 and 16 bit true colors have 6 bits precision per color however
   // only the 5 most significant bits are used in the color index.
   // Except for 16 bits where green uses all 6 bits. I.e.:
   //   15 bits = rrrrrgggggbbbbb
   //   16 bits = rrrrrggggggbbbbb
   // for 24 bits each r, g and b are represented by 8 bits.
   //
   // Since all colors are set with a max of 65535 (16 bits) per r, g, b
   // we just right shift them by 10, 11 and 10 bits for 16 planes, and
   // (10, 10, 10 for 15 planes) and by 8 bits for 24 planes.
   // Returns kFALSE in case color allocation failed.

   if (fRedDiv == -1) {
      if ( gdk_color_alloc((GdkColormap *)cmap, (GdkColor *)color) ) return kTRUE;
   } else {
      color->pixel = (color->red   >> fRedDiv)   << fRedShift |
                     (color->green >> fGreenDiv) << fGreenShift |
                     (color->blue  >> fBlueDiv)  << fBlueShift;
      return kTRUE;
   }

   return kFALSE;
}

//______________________________________________________________________________
void TGWin32::QueryColors(GdkColormap *cmap, GdkColor *color, Int_t ncolors)
{
   // Returns the current RGB value for the pixel in the XColor structure.

   ULong_t r, g, b;

   if (fRedDiv == -1) {
      GdkColorContext *cc = gdk_color_context_new(gdk_visual_get_system(), cmap);
      gdk_color_context_query_colors(cc, color, ncolors);
   } else {
      for (Int_t i = 0; i < ncolors; i++) {
         r = (color[i].pixel & fVisual->red_mask) >> fRedShift;
         color[i].red = UShort_t(r*kBIGGEST_RGB_VALUE/(fVisual->red_mask >> fRedShift));

         g = (color[i].pixel & fVisual->green_mask) >> fGreenShift;
         color[i].green = UShort_t(g*kBIGGEST_RGB_VALUE/(fVisual->green_mask >> fGreenShift));

         b = (color[i].pixel & fVisual->blue_mask) >> fBlueShift;
         color[i].blue = UShort_t(b*kBIGGEST_RGB_VALUE/(fVisual->blue_mask >> fBlueShift));
      }
   }
}

//______________________________________________________________________________
void TGWin32::Align(void)
{
   // Compute alignment variables. The alignment is done on the horizontal string
   // then the rotation is applied on the alignment variables.
   // SetRotation and LayoutGlyphs should have been called before.

   EAlign align = (EAlign) fTextAlign;

   // vertical alignment
   if (align == kTLeft || align == kTCenter || align == kTRight) {
      fAlign.y = TTF::GetAscent();
   } else if (align == kMLeft || align == kMCenter || align == kMRight) {
      fAlign.y = TTF::GetAscent()/2;
   } else {
      fAlign.y = 0;
   }
   // horizontal alignment
   if (align == kTRight || align == kMRight || align == kBRight) {
      fAlign.x = TTF::GetWidth();
   } else if (align == kTCenter || align == kMCenter || align == kBCenter) {
      fAlign.x = TTF::GetWidth()/2;
   } else {
      fAlign.x = 0;
   }

   FT_Vector_Transform(&fAlign, TTF::GetRotMatrix());
   fAlign.x = fAlign.x >> 6;
   fAlign.y = fAlign.y >> 6;
}

//______________________________________________________________________________
void TGWin32::DrawImage(FT_Bitmap *source, ULong_t fore, ULong_t back,
                         GdkImage *xim, Int_t bx, Int_t by)
{
   // Draw FT_Bitmap bitmap to xim image at position bx,by using specified
   // foreground color.

   UChar_t d = 0, *s = source->buffer;

   if (TTF::GetSmoothing()) {

      static GdkColor col[5];
      GdkColor *bcol = 0, *bc;
      Int_t    x, y;

      // background kClear, i.e. transparent, we take as background color
      // the average of the rgb values of all pixels covered by this character
      if (back == (ULong_t) -1 && (UInt_t)source->width) {
         ULong_t r, g, b;
         Int_t   dots, dotcnt;
         const Int_t maxdots = 50000;

         dots = Int_t(source->width * source->rows);
         dots = dots > maxdots ? maxdots : dots;
         bcol = new GdkColor[dots];
         if (!bcol) return;

         bc = bcol;
         dotcnt = 0;
         for (y = 0; y < (int) source->rows; y++) {
            for (x = 0; x < (int) source->width; x++, bc++) {
               bc->pixel = GetPixelImage((Drawable_t)xim, bx + x, by + y);
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
         if (bc->red == r && bc->green == g && bc->blue == b) {
            bc->pixel = back;
         } else {
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
         if (back != (ULong_t) -1) {
            col[3].pixel = back;
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
               PutPixel((Drawable_t)xim, bx + x, by + y, p);
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
            if (TESTBIT(d,7-n)) {
               PutPixel((Drawable_t)xim, bx + x, by + y, fore);
            }
            if (++n == (int) kBitsPerByte) n = 0;
         }
         row += source->pitch;
      }
   }
}

//______________________________________________________________________________
void TGWin32::DrawText(Int_t x, Int_t y, Float_t angle, Float_t mgn,
                       const char *text, ETextMode mode)
{
   // Draw text using TrueType fonts. If TrueType fonts are not available the
   // text is drawn with TGWin32::DrawText.

   if (!TTF::IsInitialized()) TTF::Init();
   TTF::SetRotationMatrix(angle);
   TTF::PrepareString(text);
   TTF::LayoutGlyphs();
   Align();
   RenderString(x, y, mode);
}

//______________________________________________________________________________
GdkImage *TGWin32::GetBackground(Int_t x, Int_t y, UInt_t w, UInt_t h)
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

   return gdk_image_get((GdkDrawable*)cws->drawing, x, y, w, h);
}

//______________________________________________________________________________
Bool_t TGWin32::IsVisible(Int_t x, Int_t y, UInt_t w, UInt_t h)
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
void TGWin32::RenderString(Int_t x, Int_t y, ETextMode mode)
{
   // Perform the string rendering in the pad.
   // LayoutGlyphs should have been called before.

   TTGlyph* glyph = TTF::GetGlyphs();
   GdkGCValues gcvals;

   // compute the size and position of the XImage that will contain the text
   Int_t Xoff = 0; if (TTF::GetBox().xMin < 0) Xoff = -TTF::GetBox().xMin;
   Int_t Yoff = 0; if (TTF::GetBox().yMin < 0) Yoff = -TTF::GetBox().yMin;
   Int_t w    = TTF::GetBox().xMax + Xoff;
   Int_t h    = TTF::GetBox().yMax + Yoff;
   Int_t x1   = x-Xoff-fAlign.x;
   Int_t y1   = y+Yoff+fAlign.y-h;

   if (!IsVisible(x1, y1, w, h)) {
       return;
   }

   // create the XImage that will contain the text
   UInt_t depth = fDepth;
   GdkImage *xim  = gdk_image_new(GDK_IMAGE_SHARED, gdk_visual_get_best(), w, h);

   // use malloc since Xlib will use free() in XDestroyImage
//   xim->data = (char *) malloc(xim->bytes_per_line * h);
//   memset(xim->data, 0, xim->bytes_per_line * h);

   ULong_t   pixel;
   ULong_t   bg;

   gdk_gc_get_values((GdkGC*)GetGC(3), &gcvals);

   // get the background
   if (mode == kClear) {
      // if mode == kClear we need to get an image of the background
      GdkImage *bim = GetBackground(x1, y1, w, h);
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
            pixel = GetPixelImage((Drawable_t)bim, xp, yp);
            PutPixel((Drawable_t)xim, xo+xp, yo+yp, pixel);
         }
      }

      gdk_image_unref((GdkImage *)bim);

      bg = (ULong_t) -1;
   } else {
      // if mode == kOpaque its simple, we just draw the background

      GdkImage *bim = GetBackground(x1, y1, w, h);
      if (!bim) {
         pixel = gcvals.background.pixel;
      } else {
         pixel = GetPixelImage((Drawable_t)bim, 0, 0);
      }
      Int_t xo = 0, yo = 0;
      if (x1 < 0) xo = -x1;
      if (y1 < 0) yo = -y1;

      for (int yp = 0; yp < h; yp++) {
         for (int xp = 0; xp < (int) w; xp++) {
            PutPixel((Drawable_t)xim, xo+xp, yo+yp, pixel);
         }
      }
      if (bim) {
         gdk_image_unref((GdkImage *)bim);
         bg = (ULong_t) -1;
      } else {
         bg = pixel;
      }
   }

   // paint the glyphs in the XImage
   glyph = TTF::GetGlyphs();
   for (int n = 0; n < TTF::GetNumGlyphs(); n++, glyph++) {
      if (FT_Glyph_To_Bitmap(&glyph->fImage,
                             TTF::GetSmoothing() ? ft_render_mode_normal
                                              : ft_render_mode_mono,
                             0, 1 )) continue;
      FT_BitmapGlyph bitmap = (FT_BitmapGlyph)glyph->fImage;
      FT_Bitmap*     source = &bitmap->bitmap;
      Int_t          bx, by;

      bx = bitmap->left+Xoff;
      by = h - bitmap->top-Yoff;
      DrawImage(source, gcvals.foreground.pixel, bg, xim, bx, by);
   }

   // put the Ximage on the screen
   XWindow_t *cws = GetCurrentWindow();
   gdk_draw_image(cws->drawing, GetGC(6), xim, 0, 0, x1, y1, w, h);

   gdk_image_unref(xim);
}

//______________________________________________________________________________
void TGWin32::SetTextFont(Font_t fontnumber)
{
   // Set specified font.

   fTextFont = fontnumber;
   TTF::SetTextFont(fontnumber);
}

//______________________________________________________________________________
Int_t TGWin32::SetTextFont(char *fontname, ETextSetMode mode)
{
   // Set text font to specified name.
   // mode       : loading flag
   // mode=0     : search if the font exist (kCheck)
   // mode=1     : search the font and load it if it exists (kLoad)
   // font       : font name
   //
   // Set text font to specified name. This function returns 0 if
   // the specified font is found, 1 if not.

   return TTF::SetTextFont(fontname);
}

//______________________________________________________________________________
void TGWin32::SetTextSize(Float_t textsize)
{
   // Set current text size.

   fTextSize = textsize;
   TTF::SetTextSize(textsize);
}

//______________________________________________________________________________
void TGWin32::ClearWindow()
{
   // Clear current window.

   if (!fWindows) return;

   if (!gCws->ispixmap && !gCws->double_buffer) {
      gdk_window_set_background(gCws->drawing, (GdkColor *) & gColors[0].color);
      gdk_window_clear(gCws->drawing);
      GdiFlush();
   } else {
      SetColor(gGCpxmp, 0);
      gdk_win32_draw_rectangle(gCws->drawing, gGCpxmp, 0,
                         0, 0, gCws->width, gCws->height);
      SetColor(gGCpxmp, 1);
   }
}

//______________________________________________________________________________
void TGWin32::ClosePixmap()
{
   // Delete current pixmap.

   CloseWindow1();
}

//______________________________________________________________________________
void TGWin32::CloseWindow()
{
   // Delete current window.

   CloseWindow1();
}

//______________________________________________________________________________
void TGWin32::CloseWindow1()
{
   // Delete current window.

   int wid;

   if (gCws->ispixmap) {
      gdk_pixmap_unref(gCws->window);
   } else {
      gdk_window_destroy(gCws->window, kTRUE);
   }

   if (gCws->buffer) {
      gdk_pixmap_unref(gCws->buffer);
   }
   if (gCws->new_colors) {
      gdk_colormap_free_colors((GdkColormap *) fColormap,
                               (GdkColor *)gCws->new_colors, gCws->ncolors);

      delete [] gCws->new_colors;
      gCws->new_colors = 0;
   }

   GdiFlush();
   gCws->open = 0;

   if (!fWindows) return;

   // make first window in list the current window
   for (wid = 0; wid < fMaxNumberOfWindows; wid++) {
      if (fWindows[wid].open) {
         gCws = &fWindows[wid];
         return;
      }
   }
   gCws = 0;
}

//______________________________________________________________________________
void TGWin32::CopyPixmap(int wid, int xpos, int ypos)
{
   // Copy the pixmap wid at the position xpos, ypos in the current window.

   if (!fWindows) return;

   gTws = &fWindows[wid];
   gdk_window_copy_area(gCws->drawing, gGCpxmp, xpos, ypos, gTws->drawing,
                        0, 0, gTws->width, gTws->height);
   GdiFlush();
}

//______________________________________________________________________________
void TGWin32::DrawBox(int x1, int y1, int x2, int y2, EBoxMode mode)
{
   // Draw a box.
   // mode=0 hollow  (kHollow)
   // mode=1 solid   (kSolid)

   if (!fWindows) return;

   Int_t x = TMath::Min(x1, x2);
   Int_t y = TMath::Min(y1, y2);
   Int_t w = TMath::Abs(x2 - x1);
   Int_t h = TMath::Abs(y2 - y1);

   switch (mode) {

   case kHollow:
      if (fLineColorModified) UpdateLineColor();
      if (fPenModified) UpdateLineStyle();
      gdk_win32_draw_rectangle(gCws->drawing, gGCline, 0, x, y, w, h);
      break;

   case kFilled:
      if (fFillStyleModified) UpdateFillStyle();
      if (fFillColorModified) UpdateFillColor();
      gdk_win32_draw_rectangle(gCws->drawing, gGCfill, 1, x, y, w, h);
      break;

   default:
      break;
   }
}

//______________________________________________________________________________
void TGWin32::DrawCellArray(Int_t x1, Int_t y1, Int_t x2, Int_t y2,
                            Int_t nx, Int_t ny, Int_t *ic)
{
   // Draw a cell array.
   // x1,y1        : left down corner
   // x2,y2        : right up corner
   // nx,ny        : array size
   // ic           : array
   //
   // Draw a cell array. The drawing is done with the pixel presicion
   // if (X2-X1)/NX (or Y) is not a exact pixel number the position of
   // the top rigth corner may be wrong.

   int i, j, icol, ix, iy, w, h, current_icol;

   if (!fWindows) return;

   current_icol = -1;
   w = TMath::Max((x2 - x1) / (nx), 1);
   h = TMath::Max((y1 - y2) / (ny), 1);
   ix = x1;

   if (fFillStyleModified) UpdateFillStyle();
   if (fFillColorModified) UpdateFillColor();

   for (i = 0; i < nx; i++) {
      iy = y1 - h;
      for (j = 0; j < ny; j++) {
         icol = ic[i + (nx * j)];
         if (icol != current_icol) {
            gdk_gc_set_foreground(gGCfill, (GdkColor *) & gColors[icol].color);
            current_icol = icol;
         }

         gdk_win32_draw_rectangle(gCws->drawing, gGCfill, kTRUE, ix,  iy, w, h);
         iy = iy - h;
      }
      ix = ix + w;
   }
}

//______________________________________________________________________________
void TGWin32::DrawFillArea(int n, TPoint *xyt)
{
   // Fill area described by polygon.
   // n         : number of points
   // xy(2,n)   : list of points

   int i;
   static int lastn = 0;
   static GdkPoint *xy = 0;

   if (!fWindows) return;

   if (fFillStyleModified) UpdateFillStyle();
   if (fFillColorModified) UpdateFillColor();

   if (lastn!=n) {
      delete [] (GdkPoint *)xy;
      xy = new GdkPoint[n];
      lastn = n;
   }
   for (i = 0; i < n; i++) {
      xy[i].x = xyt[i].fX;
      xy[i].y = xyt[i].fY;
   }

   if (gFillHollow) {
      gdk_win32_draw_lines(gCws->drawing, gGCfill, xy, n);
   } else {
      gdk_win32_draw_polygon(gCws->drawing, gGCfill, 1, xy, n);
   }
}

//______________________________________________________________________________
void TGWin32::DrawLine(int x1, int y1, int x2, int y2)
{
   // Draw a line.
   // x1,y1        : begin of line
   // x2,y2        : end of line

   if (!fWindows) return;

   if (fLineColorModified) UpdateLineColor();
   if (fPenModified) UpdateLineStyle();

   if (gLineStyle == GDK_LINE_SOLID) {
      gdk_draw_line(gCws->drawing, gGCline, x1, y1, x2, y2);
   } else {
      int i;
      gint8 dashes[32];
      for (i = 0; i < gDashSize; i++) {
         dashes[i] = (gint8) gDashList[i];
      }
      for (i = gDashSize; i < 32; i++) {
         dashes[i] = (gint8) 0;
      }
      gdk_gc_set_dashes(gGCdash, gDashOffset, dashes, gDashSize);
      gdk_draw_line(gCws->drawing, gGCdash, x1, y1, x2, y2);
   }
}

//______________________________________________________________________________
void TGWin32::DrawPolyLine(int n, TPoint * xyt)
{
   // Draw a line through all points.
   // n         : number of points
   // xy        : list of points

   int i;

   if (!fWindows) return;

   Point_t *xy = new Point_t[n];

   for (i = 0; i < n; i++) {
      xy[i].fX = xyt[i].fX;
      xy[i].fY = xyt[i].fY;
   }

   if (fLineColorModified) UpdateLineColor();
   if (fPenModified) UpdateLineStyle();

   if (n > 1) {
      if (gLineStyle == GDK_LINE_SOLID) {
         gdk_win32_draw_lines(gCws->drawing, gGCline, (GdkPoint *)xy, n);
      } else {
         int i;
         gint8 dashes[32];

         for (i = 0; i < gDashSize; i++) {
            dashes[i] = (gint8) gDashList[i];
         }
         for (i = gDashSize; i < 32; i++) {
            dashes[i] = (gint8) 0;
         }

         gdk_gc_set_dashes(gGCdash, gDashOffset, dashes, gDashSize);
         gdk_win32_draw_lines(gCws->drawing, (GdkGC*)gGCdash, (GdkPoint *)xy, n);

         // calculate length of line to update dash offset
         for (i = 1; i < n; i++) {
            int dx = xy[i].fX - xy[i - 1].fX;
            int dy = xy[i].fY - xy[i - 1].fY;

            if (dx < 0) dx = -dx;
            if (dy < 0) dy = -dy;
            gDashOffset += dx > dy ? dx : dy;
         }
         gDashOffset %= gDashLength;
      }
   } else {
      gdk_win32_draw_points( gCws->drawing, gLineStyle == GDK_LINE_SOLID ?
                              gGCline : gGCdash, (GdkPoint *)xy,1);
   }
   delete [] xy;
}

//______________________________________________________________________________
void TGWin32::DrawPolyMarker(int n, TPoint *xyt)
{
   // Draw n markers with the current attributes at position x, y.
   // n    : number of markers to draw
   // xy   : x,y coordinates of markers

   int i;
   static int lastn = 0;
   static GdkPoint *xy = 0;

   if (!fWindows) return;

   if (fMarkerStyleModified) UpdateMarkerStyle();
   if (fMarkerColorModified) UpdateMarkerColor();

   if (lastn!=n) {
      delete [] (GdkPoint *)xy;
      xy = new GdkPoint[n];
      lastn = n;
   }

   for (i = 0; i < n; i++) {
      xy[i].x = xyt[i].fX;
      xy[i].y = xyt[i].fY;
   }

   if (gMarker.n <= 0) {
       gdk_win32_draw_points(gCws->drawing, gGCmark, xy, n);
   } else {
      int r = gMarker.n / 2;
      int m;

      for (m = 0; m < n; m++) {
         int hollow = 0;
         switch (gMarker.type) {
            int i;

         case 0:               // hollow circle
            gdk_win32_draw_arc(gCws->drawing, gGCmark, kFALSE, xy[m].x-r, xy[m].y-r,
                              gMarker.n, gMarker.n, 0, 23040);
            break;

         case 1:               // filled circle
            gdk_win32_draw_arc(gCws->drawing, gGCmark, kTRUE, xy[m].x-r, xy[m].y-r,
                              gMarker.n, gMarker.n, 0, 23040);
            break;

         case 2:               // hollow polygon
            hollow = 1;
         case 3:               // filled polygon
            for (i = 0; i < gMarker.n; i++) {
               gMarker.xy[i].x += xy[m].x;
               gMarker.xy[i].y += xy[m].y;
            }
            if (hollow) {
               gdk_win32_draw_lines(gCws->drawing, gGCmark, (GdkPoint *)gMarker.xy, gMarker.n);
            } else {
               gdk_win32_draw_polygon(gCws->drawing, gGCmark, 1, (GdkPoint *)gMarker.xy, gMarker.n);
            }
            for (i = 0; i < gMarker.n; i++) {
               gMarker.xy[i].x -= xy[m].x;
               gMarker.xy[i].y -= xy[m].y;
            }
            break;

         case 4:               // segmented line
            for (i = 0; i < gMarker.n; i += 2) {
               gdk_draw_line(gCws->drawing, gGCmark,
                             xy[m].x + gMarker.xy[i].x,
                             xy[m].y + gMarker.xy[i].y,
                             xy[m].x + gMarker.xy[i + 1].x,
                             xy[m].y + gMarker.xy[i + 1].y);
            }
            break;
         }
      }
   }
}

//______________________________________________________________________________
void TGWin32::GetCharacterUp(Float_t & chupx, Float_t & chupy)
{
   // Return character up vector.

   chupx = fCharacterUpX;
   chupy = fCharacterUpY;
}

//______________________________________________________________________________
XWindow_t *TGWin32::GetCurrentWindow() const
{
   // Return current window pointer. Protected method used by TGWin32TTF.

   return gCws;
}

//______________________________________________________________________________
GdkGC *TGWin32::GetGC(Int_t which) const
{
   // Return desired Graphics Context ("which" maps directly on gGCList[]).
   // Protected method used by TGWin32TTF.

   if (which >= kMAXGC || which < 0) {
      Error("GetGC", "trying to get illegal GdkGC (which = %d)", which);
      return 0;
   }

   return gGClist[which];
}

//______________________________________________________________________________
Int_t TGWin32::GetDoubleBuffer(int wid)
{
   // Query the double buffer value for the window wid.

   if (!fWindows) return 0;

   gTws = &fWindows[wid];

   if (!gTws->open) {
      return -1;
   } else {
      return gTws->double_buffer;
   }
}

//______________________________________________________________________________
void TGWin32::GetGeometry(int wid, int &x, int &y, unsigned int &w,
                          unsigned int &h)
{
   // Return position and size of window wid.
   // wid        : window identifier
   // x,y        : window position (output)
   // w,h        : window size (output)
   // if wid < 0 the size of the display is returned

   if (!fWindows) return;

   if (wid < 0) {
      x = 0;
      y = 0;

      w = gdk_screen_width();
      h = gdk_screen_height();
   } else {
      int depth;
      int width, height;

      gTws = &fWindows[wid];
      gdk_window_get_geometry((GdkDrawable *) gTws->window, &x, &y,
                              &width, &height, &depth);

      gdk_window_get_deskrelative_origin((GdkDrawable *) gTws->window, &x, &y);

      if (width > 0 && height > 0) {
         gTws->width = width;
         gTws->height = height;
      }
      w = gTws->width;
      h = gTws->height;
   }
}

//______________________________________________________________________________
const char *TGWin32::DisplayName(const char *dpyName)
{
   // Return hostname on which the display is opened.

   return "localhost";          //return gdk_get_display();
}

//______________________________________________________________________________
void TGWin32::GetPlanes(int &nplanes)
{
   // Get maximum number of planes.

   nplanes = gdk_visual_get_best_depth();
}

//______________________________________________________________________________
void TGWin32::GetRGB(int index, float &r, float &g, float &b)
{
   // Get rgb values for color "index".

   r = gColors[index].color.red;
   g = gColors[index].color.green;
   b = gColors[index].color.blue;
}

//______________________________________________________________________________
void TGWin32::GetTextExtent(unsigned int &w, unsigned int &h, char *mess)
{
   // Return the size of a character string.
   // iw          : text width
   // ih          : text height
   // mess        : message

   TTF::SetTextFont(gTextFont);
   TTF::SetTextSize(fTextSize);
   TTF::GetTextExtent(w, h, mess);
}

//______________________________________________________________________________
Window_t TGWin32::GetWindowID(int wid)
{
   // Return the X11 window identifier.
   // wid      : Workstation identifier (input)

   if (!fWindows) return 0;
   return (Window_t) fWindows[wid].window;
}

//______________________________________________________________________________
void TGWin32::MoveWindow(int wid, int x, int y)
{
   // Move the window wid.
   // wid  : GdkWindow identifier.
   // x    : x new window position
   // y    : y new window position

   if (!fWindows) return;

   gTws = &fWindows[wid];
   if (!gTws->open) return;

   gdk_window_move((GdkDrawable *) gTws->window, x, y);
}

//______________________________________________________________________________
Int_t TGWin32::OpenPixmap(unsigned int w, unsigned int h)
{
   // Open a new pixmap.
   // w,h : Width and height of the pixmap.

   GdkWindow root;
   int wval, hval;
   int xx, yy, i, wid;
   int ww, hh, border, depth;
   wval = w;
   hval = h;

   // Select next free window number
 again:
   for (wid = 0; wid < fMaxNumberOfWindows; wid++) {
      if (!fWindows[wid].open) {
         fWindows[wid].open = 1;
         gCws = &fWindows[wid];
         break;
      }
   }
   if (wid == fMaxNumberOfWindows) {
      int newsize = fMaxNumberOfWindows + 10;
      fWindows = (XWindow_t *) TStorage::ReAlloc(fWindows,
                                                 newsize * sizeof(XWindow_t),
                                                 fMaxNumberOfWindows *
                                                 sizeof(XWindow_t));

      for (i = fMaxNumberOfWindows; i < newsize; i++) fWindows[i].open = 0;
      fMaxNumberOfWindows = newsize;
      goto again;
   }

   depth =gdk_visual_get_best_depth();
   gCws->window = (GdkPixmap *) gdk_pixmap_new(GDK_ROOT_PARENT(),wval,hval,depth);
   gdk_drawable_get_size((GdkDrawable *) gCws->window, &ww, &hh);

   for (i = 0; i < kMAXGC; i++) {
      gdk_gc_set_clip_mask((GdkGC *) gGClist[i], (GdkDrawable *)None);
   }

   SetColor(gGCpxmp, 0);
   gdk_win32_draw_rectangle(gCws->window,(GdkGC *)gGCpxmp, kTRUE,
                           0, 0, ww, hh);
   SetColor(gGCpxmp, 1);

   // Initialise the window structure
   gCws->drawing = gCws->window;
   gCws->buffer = 0;
   gCws->double_buffer = 0;
   gCws->ispixmap = 1;
   gCws->clip = 0;
   gCws->width = wval;
   gCws->height = hval;
   gCws->new_colors = 0;

   return wid;
}

//______________________________________________________________________________
Int_t TGWin32::InitWindow(ULong_t win)
{
   // Open window and return window number.
   // Return -1 if window initialization fails.

   GdkWindowAttr attributes;
   unsigned long attr_mask = 0;
   int wid;
   int xval, yval;
   int wval, hval, border, depth;
   GdkWindow root;

   GdkWindow *wind = (GdkWindow *) win;

   gdk_window_get_geometry(wind, &xval, &yval, &wval, &hval, &depth);

   // Select next free window number

 again:
   for (wid = 0; wid < fMaxNumberOfWindows; wid++) {
      if (!fWindows[wid].open) {
         fWindows[wid].open = 1;
         fWindows[wid].double_buffer = 0;
         gCws = &fWindows[wid];
         break;
      }
   }

   if (wid == fMaxNumberOfWindows) {
      int newsize = fMaxNumberOfWindows + 10;
      fWindows =
          (XWindow_t *) TStorage::ReAlloc(fWindows,
                                          newsize * sizeof(XWindow_t),
                                          fMaxNumberOfWindows *
                                          sizeof(XWindow_t));

      for (int i = fMaxNumberOfWindows; i < newsize; i++) {
         fWindows[i].open = 0;
      }

      fMaxNumberOfWindows = newsize;
      goto again;
   }
   // Create window
   attributes.wclass = GDK_INPUT_OUTPUT;
   attributes.event_mask = 0L;  //GDK_ALL_EVENTS_MASK;
   attributes.event_mask |= GDK_EXPOSURE_MASK | GDK_STRUCTURE_MASK |
       GDK_PROPERTY_CHANGE_MASK;
//                            GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK;
   if (xval >= 0) {
      attributes.x = xval;
   } else {
      attributes.x = -1.0 * xval;
   }

   if (yval >= 0) {
      attributes.y = yval;
   } else {
      attributes.y = -1.0 * yval;
   }
   attributes.width = wval;
   attributes.height = hval;
   attributes.colormap = gdk_colormap_get_system();
   attributes.visual = gdk_window_get_visual(wind);
   attributes.override_redirect = TRUE;

   if ((attributes.y > 0) && (attributes.x > 0)) {
      attr_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_COLORMAP |
          GDK_WA_WMCLASS | GDK_WA_NOREDIR;
   } else {
      attr_mask = GDK_WA_COLORMAP | GDK_WA_WMCLASS | GDK_WA_NOREDIR;
   }

   if (attributes.visual != NULL) {
      attr_mask |= GDK_WA_VISUAL;
   }
   attributes.window_type = GDK_WINDOW_CHILD;
   gCws->window = gdk_window_new(wind, &attributes, attr_mask);
   HWND window = (HWND)GDK_DRAWABLE_XID((GdkWindow *)gCws->window);
   ::ShowWindow(window, SW_SHOWNORMAL);
   ::ShowWindow(window, SW_RESTORE);
   ::BringWindowToTop(window);

   // Initialise the window structure

   gCws->drawing = gCws->window;
   gCws->buffer = 0;
   gCws->double_buffer = 0;
   gCws->ispixmap = 0;
   gCws->clip = 0;
   gCws->width = wval;
   gCws->height = hval;
   gCws->new_colors = 0;

   return wid;
}

//______________________________________________________________________________
void TGWin32::QueryPointer(int &ix, int &iy)
{
   // Query pointer position.
   // ix       : X coordinate of pointer
   // iy       : Y coordinate of pointer
   // (both coordinates are relative to the origin of the root window)

   GdkModifierType mask;
   GdkWindow *retw = gdk_window_get_pointer((GdkWindow *) gCws->window,
                                             &ix, &iy, &mask);
}

//______________________________________________________________________________
void TGWin32::RemovePixmap(GdkDrawable *pix)
{
   // Remove the pixmap pix.

   gdk_pixmap_unref((GdkPixmap *)pix);
}

//______________________________________________________________________________
Int_t TGWin32::RequestLocator(Int_t mode, Int_t ctyp, Int_t & x, Int_t & y)
{
   // Request Locator position.
   // x,y       : cursor position at moment of button press (output)
   // ctyp      : cursor type (input)
   //   ctyp=1 tracking cross
   //   ctyp=2 cross-hair
   //   ctyp=3 rubber circle
   //   ctyp=4 rubber band
   //   ctyp=5 rubber rectangle
   //
   // mode      : input mode
   //   mode=0 request
   //   mode=1 sample
   //
   // Request locator:
   // return button number  1 = left is pressed
   //                       2 = middle is pressed
   //                       3 = right is pressed
   //        in sample mode:
   //                      11 = left is released
   //                      12 = middle is released
   //                      13 = right is released
   //                      -1 = nothing is pressed or released
   //                      -2 = leave the window
   //                    else = keycode (keyboard is pressed)

   static int xloc = 0;
   static int yloc = 0;
   static int xlocp = 0;
   static int ylocp = 0;
   static GdkCursor *cursor = NULL;
   Int_t  xtmp, ytmp;

   GdkEvent *event;
   GdkEvent *next_event;
   int button_press;
   int radius;

   // Change the cursor shape
   if (cursor == NULL) {
      if (ctyp > 1) {
         gdk_window_set_cursor((GdkWindow *)gCws->window, (GdkCursor *)gNullCursor);
         gdk_gc_set_foreground((GdkGC *) gGCecho, &gColors[0].color);
      } else {
         cursor = gdk_cursor_new((GdkCursorType)GDK_CROSSHAIR);
         gdk_window_set_cursor((GdkWindow *)gCws->window, (GdkCursor *)cursor);
      }
   }

   // Event loop
   button_press = 0;

   // Set max response time to 2 minutes to avoid timeout 
   // in TGWin32ProxyBase::ForwardCallBack during RequestLocator
   TGWin32VirtualXProxy::fMaxResponseTime = 120000;
   while (button_press == 0) {
      event = gdk_event_get();

      switch (ctyp) {

      case 1:
         break;

      case 2:
         gdk_draw_line(gCws->window, gGCecho, xloc, 0, xloc, gCws->height);
         gdk_draw_line(gCws->window, gGCecho, 0, yloc, gCws->width, yloc);
         break;

      case 3:
         radius = (int) TMath::Sqrt((double)((xloc - xlocp) * (xloc - xlocp) +
                                             (yloc - ylocp) * (yloc - ylocp)));

         gdk_win32_draw_arc(gCws->window, gGCecho, kFALSE,
                            xlocp - radius, ylocp - radius,
                            2 * radius, 2 * radius, 0, 23040);
         break;

      case 4:
         gdk_draw_line(gCws->window, gGCecho, xlocp, ylocp, xloc, yloc);
         break;

      case 5:
         gdk_win32_draw_rectangle( gCws->window, gGCecho, kFALSE,
                                 TMath::Min(xlocp, xloc), TMath::Min(ylocp, yloc),
                                 TMath::Abs(xloc - xlocp), TMath::Abs(yloc - ylocp));
         break;

      default:
         break;
      }

      xloc = event->button.x;
      yloc = event->button.y;

      switch (event->type) {

      case GDK_LEAVE_NOTIFY:
         if (mode == 0) {
            while (1) {
               event = gdk_event_get();

               if (event->type == GDK_ENTER_NOTIFY) {
                  gdk_event_free(event);
                  break;
               }
               gdk_event_free(event);
            }
         } else {
            button_press = -2;
         }
         break;

      case GDK_BUTTON_PRESS:
         button_press = event->button.button;
         xlocp = event->button.x;
         ylocp = event->button.y;
         gdk_cursor_unref(cursor);
         cursor = 0;
         break;

      case GDK_BUTTON_RELEASE:
         if (mode == 1) {
            button_press = 10 + event->button.button;
            xlocp = event->button.x;
            ylocp = event->button.y;
         }
         break;

      case GDK_KEY_PRESS:
         if (mode == 1) {
            button_press = event->key.keyval;
            xlocp = event->button.x;
            ylocp = event->button.y;
         }
         break;

      case GDK_KEY_RELEASE:
         if (mode == 1) {
            button_press = -event->key.keyval;
            xlocp = event->button.x;
            ylocp = event->button.y;
         }
         break;

      default:
         break;
      }

      xtmp = event->button.x;
      ytmp = event->button.y;

      gdk_event_free(event);

      if (mode == 1) {
         if (button_press == 0) {
            button_press = -1;
         }
         break;
      }
   }
   TGWin32VirtualXProxy::fMaxResponseTime = 1000;

   x = xtmp;
   y = ytmp;

   return button_press;
}

//______________________________________________________________________________
Int_t TGWin32::RequestString(int x, int y, char *text)
{
   // Request a string.
   // x,y         : position where text is displayed
   // text        : text displayed (input), edited text (output)
   //
   // Request string:
   // text is displayed and can be edited with Emacs-like keybinding
   // return termination code (0 for ESC, 1 for RETURN)

   static GdkCursor *cursor = NULL;
   static int percent = 0;      // bell volume
   static GdkWindow *CurWnd;
   HWND focuswindow;
   int focusrevert;
   GdkEvent *event;
   KeySym keysym;
   int key = -1;
   int len_text = strlen(text);
   int nt;                      // defined length of text
   int pt;                      // cursor position in text
   MSG msg;

   CurWnd = (GdkWindow *)gCws->window;
   // change the cursor shape
   if (cursor == NULL) {
      cursor = gdk_cursor_new((GdkCursorType)GDK_QUESTION_ARROW);
   }
   if (cursor != 0) {
      gdk_window_set_cursor(CurWnd, cursor);
   }
   for (nt = len_text; nt > 0 && text[nt - 1] == ' '; nt--);

   pt = nt;
   focuswindow = ::SetFocus((HWND)GDK_DRAWABLE_XID(CurWnd));

   // Set max response time to 2 minutes to avoid timeout 
   // in TGWin32ProxyBase::ForwardCallBack during RequestString
   TGWin32VirtualXProxy::fMaxResponseTime = 120000;
   TTF::SetTextFont(gTextFont);
   TTF::SetTextSize(fTextSize);
   do {
      char tmp[2];
      char keybuf[8];
      char nbytes;
      UInt_t dx, ddx, h;
      int i;

      if (EventsPending()) {
         event = gdk_event_get();
      } else {
         gSystem->ProcessEvents();
         ::SleepEx(10, kTRUE);
         continue;
      }

      DrawText(x, y, 0.0, 1.0, text, kOpaque);
      TTF::GetTextExtent(dx, h, text);
      DrawText(x+dx, y, 0.0, 1.0, " ", kOpaque);

      if (pt == 0) {
         dx = 0;
      } else {
         char *stmp = new char[pt+1];
         strncpy(stmp, text, pt);
         stmp[pt] = '\0';
         TTF::GetTextExtent(ddx, h, stmp);
         dx = ddx;
         delete stmp;
      }

      if (pt < len_text) {
         tmp[0] = text[pt];
         tmp[1] = '\0';
         DrawText(x+dx, y, 0.0, 1.0, tmp, kOpaque);
      } else {
         DrawText(x+dx, y, 0.0, 1.0, " ", kOpaque);
      }

      if (event != NULL) {
         switch (event->type) {
         case GDK_BUTTON_PRESS:
         case GDK_ENTER_NOTIFY:
            focuswindow = ::SetFocus((HWND)GDK_DRAWABLE_XID(CurWnd));
            break;

         case GDK_LEAVE_NOTIFY:
            ::SetFocus(focuswindow);
            break;
         case GDK_KEY_PRESS:
            nbytes = event->key.length;
            for (i = 0; i < nbytes; i++) {
               keybuf[i] = event->key.string[i];
            }
            keysym = event->key.keyval;
            switch (keysym) {   // map cursor keys
            case GDK_BackSpace:
               keybuf[0] = 0x08;	// backspace
               nbytes = 1;
               break;
            case GDK_Return:
               keybuf[0] = 0x0d;	// return
               nbytes = 1;
               break;
            case GDK_Delete:
               keybuf[0] = 0x7f;	// del
               nbytes = 1;
               break;
            case GDK_Escape:
               keybuf[0] = 0x1b;	// esc
               nbytes = 1;
               break;
            case GDK_Home:
               keybuf[0] = 0x01;	// home
               nbytes = 1;
               break;
            case GDK_Left:
               keybuf[0] = 0x02;	// backward
               nbytes = 1;
               break;
            case GDK_Right:
               keybuf[0] = 0x06;	// forward
               nbytes = 1;
               break;
            case GDK_End:
               keybuf[0] = 0x05;	// end
               nbytes = 1;
               break;
            }
            if (nbytes == 1) {
               if (isascii(keybuf[0]) && isprint(keybuf[0])) {
                  // insert character
                  if (nt < len_text) {
                     nt++;
                  }
                  for (i = nt - 1; i > pt; i--) {
                     text[i] = text[i - 1];
                  }
                  if (pt < len_text) {
                     text[pt] = keybuf[0];
                     pt++;
                  }
               } else {
                  switch (keybuf[0]) {
                     // Emacs-like editing keys

                  case 0x08:   //'\010':    // backspace
                  case 0x7f:   //'\177':    // delete
                     // delete backward
                     if (pt > 0) {
                        for (i = pt; i < nt; i++) {
                           text[i - 1] = text[i];
                        }
                        text[nt - 1] = ' ';
                        nt--;
                        pt--;
                     }
                     break;
                  case 0x01:   //'\001':    // ^A
                     // beginning of line
                     pt = 0;
                     break;
                  case 0x02:   //'\002':    // ^B
                     // move backward
                     if (pt > 0) {
                        pt--;
                     }
                     break;
                  case 0x04:   //'\004':    // ^D
                     // delete forward
                     if (pt > 0) {
                        for (i = pt; i < nt; i++) {
                           text[i - 1] = text[i];
                        }
                        text[nt - 1] = ' ';
                        pt--;
                     }
                     break;
                  case 0x05:   //'\005':    // ^E
                     // end of line
                     pt = nt;
                     break;

                  case 0x06:   //'\006':    // ^F
                     // move forward
                     if (pt < nt) {
                        pt++;
                     }
                     break;
                  case 0x0b:   //'\013':    // ^K
                     // delete to end of line
                     for (i = pt; i < nt; i++)
                        text[i] = ' ';
                     nt = pt;
                     break;
                  case 0x14:   //'\024':    // ^T
                     // transpose
                     if (pt > 0) {
                        char c = text[pt];
                        text[pt] = text[pt - 1];
                        text[pt - 1] = c;
                     }
                     break;
                  case 0x0A:   //'\012':    // newline
                  case 0x0D:   //'\015':    // return
                     key = 1;
                     break;
                  case 0x1B:   //'\033':    // escape
                     key = 0;
                     break;

                  default:
                     gdk_beep();
                     break;
                  }
               }
            }
         default:
            SetInputFocus((Window_t)gCws->window);
            break;
         }
         gdk_event_free(event);
      }
   } while (key < 0);
   TGWin32VirtualXProxy::fMaxResponseTime = 1000;
   ::SetFocus(focuswindow);
   SetInputFocus((Window_t)CurWnd);

   gdk_window_set_cursor(CurWnd, (GdkCursor *)fCursors[kPointer]);
   if (cursor != 0) {
      gdk_cursor_unref(cursor);
      cursor = 0;
   }

   return key;
}

//______________________________________________________________________________
void TGWin32::RescaleWindow(int wid, unsigned int w, unsigned int h)
{
   // Rescale the window wid.
   // wid  : GdkWindow identifier
   // w    : Width
   // h    : Heigth

    int i;

   if (!fWindows) return;

   gTws = &fWindows[wid];
   if (!gTws->open)
      return;

   // don't do anything when size did not change
   if (gTws->width == w && gTws->height == h)
      return;

   gdk_window_resize((GdkWindow *) gTws->window, w, h);

   if (gTws->buffer) {
      // don't free and recreate pixmap when new pixmap is smaller
      if (gTws->width < w || gTws->height < h) {
         gdk_pixmap_unref(gTws->buffer);
         gTws->buffer = gdk_pixmap_new(GDK_ROOT_PARENT(),	// NULL,
                                       w, h, gdk_visual_get_best_depth());
      }
      for (i = 0; i < kMAXGC; i++) {
         gdk_gc_set_clip_mask(gGClist[i], None);
      }
      SetColor(gGCpxmp, 0);
      gdk_win32_draw_rectangle(gTws->buffer, gGCpxmp, 1, 0, 0, w, h);
      SetColor(gGCpxmp, 1);

      if (gTws->double_buffer) gTws->drawing = gTws->buffer;
   }
   gTws->width = w;
   gTws->height = h;
}

//______________________________________________________________________________
int TGWin32::ResizePixmap(int wid, unsigned int w, unsigned int h)
{
   // Resize a pixmap.
   // wid : pixmap to be resized
   // w,h : Width and height of the pixmap

   GdkWindow root;
   int wval, hval;
   int xx, yy, i;
   int ww, hh, border, depth;
   wval = w;
   hval = h;

   if (!fWindows) return 0;

   gTws = &fWindows[wid];

   // don't do anything when size did not change
   //  if (gTws->width == wval && gTws->height == hval) return 0;

   // due to round-off errors in TPad::Resize() we might get +/- 1 pixel
   // change, in those cases don't resize pixmap
   if (gTws->width >= wval - 1 && gTws->width <= wval + 1 &&
       gTws->height >= hval - 1 && gTws->height <= hval + 1)
      return 0;

   // don't free and recreate pixmap when new pixmap is smaller
   if (gTws->width < wval || gTws->height < hval) {
      gdk_pixmap_unref((GdkPixmap *)gTws->window);
      depth = gdk_visual_get_best_depth();
      gTws->window = gdk_pixmap_new(GDK_ROOT_PARENT(), wval, hval, depth);
   }

   gdk_drawable_get_size(gTws->window, &ww, &hh);

   for (i = 0; i < kMAXGC; i++) {
      gdk_gc_set_clip_mask((GdkGC *) gGClist[i], (GdkDrawable *)None);
   }

   SetColor(gGCpxmp, 0);
   gdk_win32_draw_rectangle(gTws->window,(GdkGC *)gGCpxmp, kTRUE, 0, 0, ww, hh);
   SetColor(gGCpxmp, 1);

   // Initialise the window structure
   gTws->drawing = gTws->window;
   gTws->width = wval;
   gTws->height = hval;
   return 1;
}

//______________________________________________________________________________
void TGWin32::ResizeWindow(int wid)
{
   // Resize the current window if necessary.

   int i;
   int xval = 0, yval = 0;
   GdkWindow *win, *root = NULL;
   int wval = 0, hval = 0, depth = 0;

   if (!fWindows) return;

   gTws = &fWindows[wid];

   win = (GdkWindow *) gTws->window;
   gdk_window_get_geometry(win, &xval, &yval,
                           &wval, &hval, &depth);

   // don't do anything when size did not change
   if (gTws->width == wval && gTws->height == hval) {
      return;
   }

   gdk_window_resize((GdkWindow *) gTws->window, wval, hval);

   if (gTws->buffer) {
      if (gTws->width < wval || gTws->height < hval) {
         gdk_pixmap_unref((GdkPixmap *)gTws->buffer);
         depth = gdk_visual_get_best_depth();
         gTws->buffer = (GdkPixmap *) gdk_pixmap_new(GDK_ROOT_PARENT(),
                                                     wval, hval, depth);
      }

      for (i = 0; i < kMAXGC; i++) {
         gdk_gc_set_clip_mask((GdkGC *) gGClist[i], (GdkDrawable *)None);
      }

      SetColor(gGCpxmp, 0);
      gdk_win32_draw_rectangle(gTws->buffer,(GdkGC *)gGCpxmp, kTRUE, 0, 0, wval, hval);

      SetColor(gGCpxmp, 1);

      if (gTws->double_buffer) gTws->drawing = gTws->buffer;
   }

   gTws->width = wval;
   gTws->height = hval;
}

//______________________________________________________________________________
void TGWin32::SelectWindow(int wid)
{
   // Select window to which subsequent output is directed.

   int i;
   GdkRectangle rect;

   if (!fWindows || wid < 0 || wid >= fMaxNumberOfWindows || !fWindows[wid].open) {
      return;
   }

   gCws = &fWindows[wid];

   if (gCws->clip && !gCws->ispixmap && !gCws->double_buffer) {
      rect.x = gCws->xclip;
      rect.y = gCws->yclip;
      rect.width = gCws->wclip;
      rect.height = gCws->hclip;

      for (i = 0; i < kMAXGC; i++) {
         gdk_gc_set_clip_rectangle((GdkGC *) gGClist[i], &rect);
      }
   } else {
      for (i = 0; i < kMAXGC; i++) {
         gdk_gc_set_clip_mask((GdkGC *) gGClist[i], (GdkDrawable *)None);
      }
   }
}

//______________________________________________________________________________
void TGWin32::SetCharacterUp(Float_t chupx, Float_t chupy)
{
   // Set character up vector.

   if (chupx == fCharacterUpX && chupy == fCharacterUpY) return;

   if (chupx == 0 && chupy == 0) {
      fTextAngle = 0;
   } else if (chupx == 0 && chupy == 1) {
      fTextAngle = 0;
   } else if (chupx == -1 && chupy == 0) {
      fTextAngle = 90;
   } else if (chupx == 0 && chupy == -1) {
      fTextAngle = 180;
   } else if (chupx == 1 && chupy == 0) {
      fTextAngle = 270;
   } else {
      fTextAngle =
          ((TMath::
            ACos(chupx / TMath::Sqrt(chupx * chupx + chupy * chupy)) *
            180.) / 3.14159) - 90;
      if (chupy < 0) fTextAngle = 180 - fTextAngle;
      if (TMath::Abs(fTextAngle) <= 0.01) fTextAngle = 0;
   }
   fCharacterUpX = chupx;
   fCharacterUpY = chupy;
}

//______________________________________________________________________________
void TGWin32::SetClipOFF(int wid)
{
   // Turn off the clipping for the window wid.

   if (!fWindows) return;

   gTws = &fWindows[wid];
   gTws->clip = 0;

   for (int i = 0; i < kMAXGC; i++) {
      gdk_gc_set_clip_mask((GdkGC *) gGClist[i], (GdkDrawable *)None);
   }
}

//______________________________________________________________________________
void TGWin32::SetClipRegion(int wid, int x, int y, unsigned int w,
                            unsigned int h)
{
   // Set clipping region for the window wid.
   // wid        : GdkWindow indentifier
   // x,y        : origin of clipping rectangle
   // w,h        : size of clipping rectangle;

   if (!fWindows) return;

   gTws = &fWindows[wid];
   gTws->xclip = x;
   gTws->yclip = y;
   gTws->wclip = w;
   gTws->hclip = h;
   gTws->clip = 1;
   GdkRectangle rect;

   if (gTws->clip && !gTws->ispixmap && !gTws->double_buffer) {
      rect.x = gTws->xclip;
      rect.y = gTws->yclip;
      rect.width = gTws->wclip;
      rect.height = gTws->hclip;

      for (int i = 0; i < kMAXGC; i++) {
         gdk_gc_set_clip_rectangle((GdkGC *)gGClist[i], &rect);
      }
   }
}

//______________________________________________________________________________
ULong_t TGWin32::GetPixel(Color_t ci)
{
   // Return pixel value associated to specified ROOT color number.

   if (ci >= 0 && ci < kMAXCOL && !gColors[ci].defined) {
      TColor *color = gROOT->GetColor(ci);
      if (color) {
         SetRGB(ci, color->GetRed(), color->GetGreen(), color->GetBlue());
      } else {
         Warning("GetPixel", "color with index %d not defined", ci);
      }
   }
   return gColors[ci].color.pixel;
}

//______________________________________________________________________________
void TGWin32::SetColor(GdkGC *gc, int ci)
{
   // Set the foreground color in GdkGC.

   GdkGCValues gcvals;
   GdkColor color;

   if (ci<=0) ci = 10; //white

   if (ci >= 0 && ci < kMAXCOL && !gColors[ci].defined) {
      TColor *tcol = gROOT->GetColor(ci);

      if (tcol) {
         SetRGB(ci, tcol->GetRed(), tcol->GetGreen(), tcol->GetBlue());
      }
   }

   if (fColormap && (ci < 0 || ci >= kMAXCOL || !gColors[ci].defined)) {
      ci = 0;
   } else if (!fColormap && ci < 0) {
      ci = 0;
   } else if (!fColormap && ci > 1) {
      ci = 0;
   }

   if (fDrawMode == kXor) {
      gdk_gc_get_values(gc, &gcvals);

      color.pixel = gColors[ci].color.pixel ^ gcvals.background.pixel;
      color.red = GetRValue(color.pixel);
      color.green = GetGValue(color.pixel);
      color.blue = GetBValue(color.pixel);

      gdk_gc_set_foreground(gc, &color);

   } else {
      color.pixel = gColors[ci].color.pixel;
      color.red = gColors[ci].color.red;
      color.green = gColors[ci].color.green;
      color.blue = gColors[ci].color.blue;
      gdk_gc_set_foreground(gc, &color);

      // make sure that foreground and background are different
      gdk_gc_get_values(gc, &gcvals);

      if (gcvals.foreground.pixel == gcvals.background.pixel) {
         color.pixel = gColors[!ci].color.pixel;
         color.red = gColors[!ci].color.red;
         color.green = gColors[!ci].color.green;
         color.blue = gColors[!ci].color.blue;
         gdk_gc_set_background(gc, &color);
      }
   }
}

//______________________________________________________________________________
void TGWin32::SetCursor(int wid, ECursor cursor)
{
   // Set the cursor.

   if (!fWindows) return;

   gTws = &fWindows[wid];
   gdk_window_set_cursor((GdkWindow *)gTws->window, (GdkCursor *)fCursors[cursor]);
}

//______________________________________________________________________________
void TGWin32::SetCursor(Window_t id, Cursor_t curid)
{
   // Set the specified cursor.

   if (!id) return;

   static GdkWindow *lid = 0;
   static GdkCursor *lcur = 0;

   if ((lid == (GdkWindow *)id) && (lcur==(GdkCursor *)curid)) return;
   lid = (GdkWindow *)id;
   lcur = (GdkCursor *)curid;

   gdk_window_set_cursor((GdkWindow *) id, (GdkCursor *)curid);
}

//______________________________________________________________________________
void TGWin32::SetDoubleBuffer(int wid, int mode)
{
   // Set the double buffer on/off on window wid.
   // wid  : GdkWindow identifier.
   //        999 means all the opened windows.
   // mode : 1 double buffer is on
   //        0 double buffer is off

   if (!fWindows) return;

   if (wid == 999) {
      for (int i = 0; i < fMaxNumberOfWindows; i++) {
         gTws = &fWindows[i];
         if (gTws->open) {
            switch (mode) {
            case 1:
               SetDoubleBufferON();
               break;
            default:
               SetDoubleBufferOFF();
               break;
            }
         }
      }
   } else {
      gTws = &fWindows[wid];
      if (!gTws->open) return;

      switch (mode) {
      case 1:
         SetDoubleBufferON();
         return;
      default:
         SetDoubleBufferOFF();
         return;
      }
   }
}

//______________________________________________________________________________
void TGWin32::SetDoubleBufferOFF()
{
   // Turn double buffer mode off.

   if (!gTws->double_buffer) return;
   gTws->double_buffer = 0;
   gTws->drawing = gTws->window;
}

//______________________________________________________________________________
void TGWin32::SetDoubleBufferON()
{
   // Turn double buffer mode on.

   Int_t depth;

   if (!fWindows || gTws->double_buffer || gTws->ispixmap) return;

   if (!gTws->buffer) {
      gTws->buffer = gdk_pixmap_new(GDK_ROOT_PARENT(),	//NULL,
                                    gTws->width, gTws->height,
                                    gdk_visual_get_best_depth());
      SetColor(gGCpxmp, 0);
      gdk_win32_draw_rectangle(gTws->buffer, gGCpxmp, 1, 0, 0, gTws->width,
                         gTws->height);
      SetColor(gGCpxmp, 1);
   }
   for (int i = 0; i < kMAXGC; i++) {
      gdk_gc_set_clip_mask(gGClist[i], None);
   }
   gTws->double_buffer = 1;
   gTws->drawing = gTws->buffer;
}

//______________________________________________________________________________
void TGWin32::SetDrawMode(EDrawMode mode)
{
   // Set the drawing mode.
   // mode : drawing mode
   //   mode=1 copy
   //   mode=2 xor
   //   mode=3 invert
   //   mode=4 set the suitable mode for cursor echo according to
   //          the vendor

   int i;

   switch (mode) {
   case kCopy:
      for (i = 0; i < kMAXGC; i++) {
         gdk_gc_set_function(gGClist[i], GDK_COPY);
      }
      break;

   case kXor:
      for (i = 0; i < kMAXGC; i++) {
         gdk_gc_set_function(gGClist[i], GDK_XOR);
      }
      break;

   case kInvert:
      for (i = 0; i < kMAXGC; i++) {
         gdk_gc_set_function(gGClist[i], GDK_INVERT);
      }
      break;
   }
   fDrawMode = mode;
}

//______________________________________________________________________________
void TGWin32::SetFillColor(Color_t cindex)
{
   // Set color index for fill areas.

   Int_t indx = Int_t(cindex);

   if (!gStyle->GetFillColor() && cindex > 1) {
      indx = 0;
   }

   fFillColor = indx;
   fFillColorModified = kTRUE;
}

//______________________________________________________________________________
void TGWin32::UpdateFillColor()
{
   //

   if (fFillColor >= 0) {
      SetColor(gGCfill, fFillColor);
   }

   // invalidate fill pattern
   if (gFillPattern != NULL) {
      gdk_pixmap_unref(gFillPattern);
      gFillPattern = NULL;
   }
   fFillColorModified = kFALSE;
}

//______________________________________________________________________________
void TGWin32::SetFillStyle(Style_t fstyle)
{
   // Set fill area style.
   // fstyle   : compound fill area interior style
   //    fstyle = 1000*interiorstyle + styleindex

   if (fFillStyle==fstyle) return;

   fFillStyle = fstyle;
   fFillStyleModified = kTRUE;
}

//______________________________________________________________________________
void TGWin32::UpdateFillStyle()
{
   // Set fill area style index.

   char* pchar;
   static int current_fasi = 0;

   Int_t style = fFillStyle / 1000;
   Int_t fasi = fFillStyle % 1000;

   switch (style) {

   case 1:                     // solid
      gFillHollow = 0;
      gdk_gc_set_fill(gGCfill, GDK_SOLID);
      break;

   case 2:                     // pattern
      gFillHollow = 1;
      break;

   case 3:                     // hatch
      gFillHollow = 0;
      gdk_gc_set_fill(gGCfill, GDK_STIPPLED);

      if (fasi != current_fasi) {
         if (gFillPattern != NULL) {
            gdk_pixmap_unref(gFillPattern);
            gFillPattern = NULL;
         }

         switch (fasi) {
         case 1:
            pchar = (char *) p1_bits;
            break;
         case 2:
            pchar = (char *) p2_bits;
            break;
         case 3:
            pchar = (char *) p3_bits;
            break;
         case 4:
            pchar = (char *) p4_bits;
            break;
         case 5:
            pchar = (char *) p5_bits;
            break;
         case 6:
            pchar = (char *) p6_bits;
            break;
         case 7:
            pchar = (char *) p7_bits;
            break;
         case 8:
            pchar = (char *) p8_bits;
            break;
         case 9:
            pchar = (char *) p9_bits;
            break;
         case 10:
            pchar = (char *) p10_bits;
            break;
         case 11:
            pchar = (char *) p11_bits;
            break;
         case 12:
            pchar = (char *) p12_bits;
            break;
         case 13:
            pchar = (char *) p13_bits;
            break;
         case 14:
            pchar = (char *) p14_bits;
            break;
         case 15:
            pchar = (char *) p15_bits;
            break;
         case 16:
            pchar = (char *) p16_bits;
            break;
         case 17:
            pchar = (char *) p17_bits;
            break;
         case 18:
            pchar = (char *) p18_bits;
            break;
         case 19:
            pchar = (char *) p19_bits;
            break;
         case 20:
            pchar = (char *) p20_bits;
            break;
         case 21:
            pchar = (char *) p21_bits;
            break;
         case 22:
            pchar = (char *) p22_bits;
            break;
         case 23:
            pchar = (char *) p23_bits;
            break;
         case 24:
            pchar = (char *) p24_bits;
            break;
         case 25:
            pchar = (char *) p25_bits;
            break;
         default:
            pchar = (char *) p2_bits;
            break;
         }

         gFillPattern = gdk_bitmap_create_from_data(GDK_ROOT_PARENT(),
                                                    (const char *)pchar, 16, 16);
         gdk_gc_set_stipple(gGCfill, gFillPattern);
         current_fasi = fasi;
      }
      break;

   default:
      gFillHollow = 1;
   }

   fFillStyleModified = kFALSE;
}

//______________________________________________________________________________
void TGWin32::SetInput(int inp)
{
   // Set input on or off.

   EnableWindow((HWND) GDK_DRAWABLE_XID(gCws->window), inp);
}

//______________________________________________________________________________
void TGWin32::SetLineColor(Color_t cindex)
{
   // Set color index for lines.

   if ((cindex < 0) || (cindex==fLineColor)) return;

   fLineColor =  cindex;
   fLineColorModified = kTRUE;
}

//______________________________________________________________________________
void TGWin32::UpdateLineColor()
{
   //

   SetColor(gGCline, Int_t(fLineColor));
   SetColor(gGCdash, Int_t(fLineColor));
   fLineColorModified = kFALSE;
}

//______________________________________________________________________________
void TGWin32::SetLineType(int n, int *dash)
{
   // Set line type.
   // n         : length of dash list
   // dash(n)   : dash segment lengths
   //
   // if n <= 0 use solid lines
   // if n >  0 use dashed lines described by DASH(N)
   //    e.g. N=4,DASH=(6,3,1,3) gives a dashed-dotted line with dash length 6
   //    and a gap of 7 between dashes

   if (n <= 0) {
      gLineStyle = GDK_LINE_SOLID;
      gdk_gc_set_line_attributes(gGCline, fLineWidth,
                                 (GdkLineStyle)gLineStyle,
                                 (GdkCapStyle) gCapStyle,
                                 (GdkJoinStyle) gJoinStyle);
   } else {
      int i, j;
      gDashSize = TMath::Min((int)sizeof(gDashList),n);
      gDashLength = 0;
      for (i = 0; i < gDashSize; i++) {
         gDashList[i] = dash[i];
         gDashLength += gDashList[i];
      }
      gDashOffset = 0;
      gLineStyle = GDK_LINE_ON_OFF_DASH;
      gdk_gc_set_line_attributes(gGCdash, fLineWidth,
                                 (GdkLineStyle) gLineStyle,
                                 (GdkCapStyle) gCapStyle,
                                 (GdkJoinStyle) gJoinStyle);
   }
   fPenModified = kFALSE;
}

//______________________________________________________________________________
void TGWin32::SetLineStyle(Style_t lstyle)
{
   // Set line style.

   if (fLineStyle == lstyle) return;

   fLineStyle = lstyle;
   fPenModified = kTRUE;
}

//______________________________________________________________________________
void TGWin32::UpdateLineStyle()
{
   // Update line style

   static Int_t dashed[2] = { 3, 3 };
   static Int_t dotted[2] = { 1, 2 };
   static Int_t dasheddotted[4] = { 3, 4, 1, 4 };

   if (fLineStyle <= 1) {
      SetLineType(0, 0);
   } else if (fLineStyle == 2) {
      SetLineType(2, dashed);
   } else if (fLineStyle == 3) {
      SetLineType(2, dotted);
   } else if (fLineStyle == 4) {
      SetLineType(4, dasheddotted);
   } else {
      TString st = (TString)gStyle->GetLineStyleString(fLineStyle);
      TObjArray *tokens = st.Tokenize(" ");
      Int_t nt;
      nt = tokens->GetEntries();
      Int_t *linestyle = new Int_t[nt];
      for (Int_t j = 0; j<nt; j++) {
         Int_t it;
         sscanf(((TObjString*)tokens->At(j))->GetName(), "%d", &it);
         linestyle[j] = (Int_t)(it/4);
      }
      SetLineType(nt,linestyle);
      delete [] linestyle;
      delete tokens;
   }
   fPenModified = kFALSE;
}

//______________________________________________________________________________
void TGWin32::SetLineWidth(Width_t width)
{
   // Set line width.
   // width   : line width in pixels

   if ((fLineWidth==width) || (width<0)) return;

   if (width == 1) {
      fLineWidth = 0;
   } else {
      fLineWidth = width;
   }

   fPenModified = kTRUE;
}

//______________________________________________________________________________
void TGWin32::SetMarkerColor(Color_t cindex)
{
   // Set color index for markers.

   if ((cindex<0) || (cindex==fMarkerColor)) return;
   fMarkerColor = cindex;
   fMarkerColorModified = kTRUE;
}

//______________________________________________________________________________
void TGWin32::UpdateMarkerColor()
{
   //

   SetColor(gGCmark, Int_t(fMarkerColor));
   fMarkerColorModified = kFALSE;
}

//______________________________________________________________________________
void TGWin32::SetMarkerSize(Float_t msize)
{
   // Set marker size index.
   // msize  : marker scale factor

   if ((msize==fMarkerSize) || (msize<0)) return;

   fMarkerSize = msize;
   SetMarkerStyle(-fMarkerStyle);
}

//______________________________________________________________________________
void TGWin32::SetMarkerType(int type, int n, GdkPoint * xy)
{
   // Set marker type.
   // type      : marker type
   // n         : length of marker description
   // xy        : list of points describing marker shape
   //
   // if n == 0 marker is a single point
   // if TYPE == 0 marker is hollow circle of diameter N
   // if TYPE == 1 marker is filled circle of diameter N
   // if TYPE == 2 marker is a hollow polygon describe by line XY
   // if TYPE == 3 marker is a filled polygon describe by line XY
   // if TYPE == 4 marker is described by segmented line XY
   //   e.g. TYPE=4,N=4,XY=(-3,0,3,0,0,-3,0,3) sets a plus shape of 7x7 pixels

   gMarker.type = type;
   gMarker.n = n < kMAXMK ? n : kMAXMK;
   if (gMarker.type >= 2) {
      for (int i = 0; i < gMarker.n; i++) {
         gMarker.xy[i] = xy[i];
      }
   }
}

//______________________________________________________________________________
void TGWin32::SetMarkerStyle(Style_t markerstyle)
{
   // Set marker style.

   if ((fMarkerStyle==markerstyle) || (markerstyle >= 31)) return;
   fMarkerStyle = TMath::Abs(markerstyle);
   fMarkerStyleModified = kTRUE;
}

//______________________________________________________________________________
void TGWin32::UpdateMarkerStyle()
{
   //

   static GdkPoint shape[15];

   Int_t im = Int_t(4 * fMarkerSize + 0.5);

   if (fMarkerStyle == 2) {
      // + shaped marker
      shape[0].x = -im;
      shape[0].y = 0;
      shape[1].x = im;
      shape[1].y = 0;
      shape[2].x = 0;
      shape[2].y = -im;
      shape[3].x = 0;
      shape[3].y = im;
      SetMarkerType(4, 4, shape);
   } else if (fMarkerStyle == 3) {
      // * shaped marker
      shape[0].x = -im;
      shape[0].y = 0;
      shape[1].x = im;
      shape[1].y = 0;
      shape[2].x = 0;
      shape[2].y = -im;
      shape[3].x = 0;
      shape[3].y = im;
      im = Int_t(0.707 * Float_t(im) + 0.5);
      shape[4].x = -im;
      shape[4].y = -im;
      shape[5].x = im;
      shape[5].y = im;
      shape[6].x = -im;
      shape[6].y = im;
      shape[7].x = im;
      shape[7].y = -im;
      SetMarkerType(4, 8, shape);
   } else if (fMarkerStyle == 4 || fMarkerStyle == 24) {
      // O shaped marker
      SetMarkerType(0, im * 2, shape);
   } else if (fMarkerStyle == 5) {
      // X shaped marker
      im = Int_t(0.707 * Float_t(im) + 0.5);
      shape[0].x = -im;
      shape[0].y = -im;
      shape[1].x = im;
      shape[1].y = im;
      shape[2].x = -im;
      shape[2].y = im;
      shape[3].x = im;
      shape[3].y = -im;
      SetMarkerType(4, 4, shape);
   } else if (fMarkerStyle == 6) {
      // + shaped marker (with 1 pixel)
      shape[0].x = -1;
      shape[0].y = 0;
      shape[1].x = 1;
      shape[1].y = 0;
      shape[2].x = 0;
      shape[2].y = -1;
      shape[3].x = 0;
      shape[3].y = 1;
      SetMarkerType(4, 4, shape);
   } else if (fMarkerStyle == 7) {
      // . shaped marker (with 9 pixel)
      shape[0].x = -1;
      shape[0].y = 1;
      shape[1].x = 1;
      shape[1].y = 1;
      shape[2].x = -1;
      shape[2].y = 0;
      shape[3].x = 1;
      shape[3].y = 0;
      shape[4].x = -1;
      shape[4].y = -1;
      shape[5].x = 1;
      shape[5].y = -1;
      SetMarkerType(4, 6, shape);
   } else if (fMarkerStyle == 8 || fMarkerStyle == 20) {
      // O shaped marker (filled)
      SetMarkerType(1, im * 2, shape);
   } else if (fMarkerStyle == 21) {  // here start the old HIGZ symbols
      // HIGZ full square
      shape[0].x = -im;
      shape[0].y = -im;
      shape[1].x = im;
      shape[1].y = -im;
      shape[2].x = im;
      shape[2].y = im;
      shape[3].x = -im;
      shape[3].y = im;
      shape[4].x = -im;
      shape[4].y = -im;
      SetMarkerType(3, 5, shape);
   } else if (fMarkerStyle == 22) {
      // HIGZ full triangle up
      shape[0].x = -im;
      shape[0].y = im;
      shape[1].x = im;
      shape[1].y = im;
      shape[2].x = 0;
      shape[2].y = -im;
      shape[3].x = -im;
      shape[3].y = im;
      SetMarkerType(3, 4, shape);
   } else if (fMarkerStyle == 23) {
      // HIGZ full triangle down
      shape[0].x = 0;
      shape[0].y = im;
      shape[1].x = im;
      shape[1].y = -im;
      shape[2].x = -im;
      shape[2].y = -im;
      shape[3].x = 0;
      shape[3].y = im;
      SetMarkerType(3, 4, shape);
   } else if (fMarkerStyle == 25) {
      // HIGZ open square
      shape[0].x = -im;
      shape[0].y = -im;
      shape[1].x = im;
      shape[1].y = -im;
      shape[2].x = im;
      shape[2].y = im;
      shape[3].x = -im;
      shape[3].y = im;
      shape[4].x = -im;
      shape[4].y = -im;
      SetMarkerType(2, 5, shape);
   } else if (fMarkerStyle == 26) {
      // HIGZ open triangle up
      shape[0].x = -im;
      shape[0].y = im;
      shape[1].x = im;
      shape[1].y = im;
      shape[2].x = 0;
      shape[2].y = -im;
      shape[3].x = -im;
      shape[3].y = im;
      SetMarkerType(2, 4, shape);
   } else if (fMarkerStyle == 27) {
      // HIGZ open losange
      Int_t imx = Int_t(2.66 * fMarkerSize + 0.5);
      shape[0].x = -imx;
      shape[0].y = 0;
      shape[1].x = 0;
      shape[1].y = -im;
      shape[2].x = imx;
      shape[2].y = 0;
      shape[3].x = 0;
      shape[3].y = im;
      shape[4].x = -imx;
      shape[4].y = 0;
      SetMarkerType(2, 5, shape);
   } else if (fMarkerStyle == 28) {
      // HIGZ open cross
      Int_t imx = Int_t(1.33 * fMarkerSize + 0.5);
      shape[0].x = -im;
      shape[0].y = -imx;
      shape[1].x = -imx;
      shape[1].y = -imx;
      shape[2].x = -imx;
      shape[2].y = -im;
      shape[3].x = imx;
      shape[3].y = -im;
      shape[4].x = imx;
      shape[4].y = -imx;
      shape[5].x = im;
      shape[5].y = -imx;
      shape[6].x = im;
      shape[6].y = imx;
      shape[7].x = imx;
      shape[7].y = imx;
      shape[8].x = imx;
      shape[8].y = im;
      shape[9].x = -imx;
      shape[9].y = im;
      shape[10].x = -imx;
      shape[10].y = imx;
      shape[11].x = -im;
      shape[11].y = imx;
      shape[12].x = -im;
      shape[12].y = -imx;
      SetMarkerType(2, 13, shape);
   } else if (fMarkerStyle == 29) {
      // HIGZ full star pentagone
      Int_t im1 = Int_t(0.66 * fMarkerSize + 0.5);
      Int_t im2 = Int_t(2.00 * fMarkerSize + 0.5);
      Int_t im3 = Int_t(2.66 * fMarkerSize + 0.5);
      Int_t im4 = Int_t(1.33 * fMarkerSize + 0.5);
      shape[0].x = -im;
      shape[0].y = im4;
      shape[1].x = -im2;
      shape[1].y = -im1;
      shape[2].x = -im3;
      shape[2].y = -im;
      shape[3].x = 0;
      shape[3].y = -im2;
      shape[4].x = im3;
      shape[4].y = -im;
      shape[5].x = im2;
      shape[5].y = -im1;
      shape[6].x = im;
      shape[6].y = im4;
      shape[7].x = im4;
      shape[7].y = im4;
      shape[8].x = 0;
      shape[8].y = im;
      shape[9].x = -im4;
      shape[9].y = im4;
      shape[10].x = -im;
      shape[10].y = im4;
      SetMarkerType(3, 11, shape);
   } else if (fMarkerStyle == 30) {
      // HIGZ open star pentagone
      Int_t im1 = Int_t(0.66 * fMarkerSize + 0.5);
      Int_t im2 = Int_t(2.00 * fMarkerSize + 0.5);
      Int_t im3 = Int_t(2.66 * fMarkerSize + 0.5);
      Int_t im4 = Int_t(1.33 * fMarkerSize + 0.5);
      shape[0].x = -im;
      shape[0].y = im4;
      shape[1].x = -im2;
      shape[1].y = -im1;
      shape[2].x = -im3;
      shape[2].y = -im;
      shape[3].x = 0;
      shape[3].y = -im2;
      shape[4].x = im3;
      shape[4].y = -im;
      shape[5].x = im2;
      shape[5].y = -im1;
      shape[6].x = im;
      shape[6].y = im4;
      shape[7].x = im4;
      shape[7].y = im4;
      shape[8].x = 0;
      shape[8].y = im;
      shape[9].x = -im4;
      shape[9].y = im4;
      shape[10].x = -im;
      shape[10].y = im4;
      SetMarkerType(2, 11, shape);
   } else if (fMarkerStyle == 31) {
      // HIGZ +&&x (kind of star)
      SetMarkerType(1, im * 2, shape);
   } else {
      // single dot
      SetMarkerType(0, 0, shape);
   }
   fMarkerStyleModified = kFALSE;
}

//______________________________________________________________________________
void TGWin32::SetOpacity(Int_t percent)
{
   // Set opacity of a window. This image manipulation routine works
   // by adding to a percent amount of neutral to each pixels RGB.
   // Since it requires quite some additional color map entries is it
   // only supported on displays with more than > 8 color planes (> 256
   // colors)

   Int_t depth = gdk_visual_get_best_depth();

   if (depth <= 8) return;
   if (percent == 0) return;

   // if 100 percent then just make white
   ULong_t *orgcolors = 0, *tmpc = 0;
   Int_t maxcolors = 0, ncolors, ntmpc = 0;

   // save previous allocated colors, delete at end when not used anymore
   if (gCws->new_colors) {
      tmpc = gCws->new_colors;
      ntmpc = gCws->ncolors;
   }
   // get pixmap from server as image
   GdkImage *image = gdk_image_get((GdkDrawable*)gCws->drawing, 0, 0,
                                   gCws->width, gCws->height);

   // collect different image colors
   int x, y;
   for (y = 0; y < (int) gCws->height; y++) {
      for (x = 0; x < (int) gCws->width; x++) {
         ULong_t pixel = GetPixelImage((Drawable_t)image, x, y);
         CollectImageColors(pixel, orgcolors, ncolors, maxcolors);
      }
   }
   if (ncolors == 0) {
      gdk_image_unref(image);
      ::operator delete(orgcolors);
      return;
   }
   // create opaque counter parts
   MakeOpaqueColors(percent, orgcolors, ncolors);

   // put opaque colors in image
   for (y = 0; y < (int) gCws->height; y++) {
      for (x = 0; x < (int) gCws->width; x++) {
         ULong_t pixel = GetPixelImage((Drawable_t)image, x, y);
         Int_t idx = FindColor(pixel, orgcolors, ncolors);
         PutPixel((Drawable_t)image, x, y, gCws->new_colors[idx]);
      }
   }

   // put image back in pixmap on server
   gdk_draw_image(gCws->drawing, gGCpxmp, (GdkImage *)image,
                  0, 0, 0, 0, gCws->width, gCws->height);
   GdiFlush();

   // clean up
   if (tmpc) {
      gdk_colors_free((GdkColormap *)fColormap, tmpc, ntmpc, 0);
      delete[]tmpc;
   }
   gdk_image_unref(image);
   ::operator delete(orgcolors);
}

//______________________________________________________________________________
void TGWin32::MakeOpaqueColors(Int_t percent, ULong_t *orgcolors, Int_t ncolors)
{
   // Get RGB values for orgcolors, add percent neutral to the RGB and
   // allocate new_colors.

   Int_t ret;
   if (ncolors <= 0) return;
   GdkColor *xcol = new GdkColor[ncolors];

   int i;
   for (i = 0; i < ncolors; i++) {
      xcol[i].pixel = orgcolors[i];
      xcol[i].red = xcol[i].green = xcol[i].blue = 0;
   }

   GdkColorContext *cc;
   cc = gdk_color_context_new(gdk_visual_get_system(), (GdkColormap *)fColormap);
   gdk_color_context_query_colors(cc, xcol, ncolors);

   UShort_t add = percent * kBIGGEST_RGB_VALUE / 100;

   Int_t val;
   for (i = 0; i < ncolors; i++) {
      val = xcol[i].red + add;
      if (val > kBIGGEST_RGB_VALUE) {
         val = kBIGGEST_RGB_VALUE;
      }
      xcol[i].red = (UShort_t) val;
      val = xcol[i].green + add;
      if (val > kBIGGEST_RGB_VALUE) {
         val = kBIGGEST_RGB_VALUE;
      }
      xcol[i].green = (UShort_t) val;
      val = xcol[i].blue + add;
      if (val > kBIGGEST_RGB_VALUE) {
         val = kBIGGEST_RGB_VALUE;
      }
      xcol[i].blue = (UShort_t) val;

      ret = gdk_color_alloc((GdkColormap *)fColormap, &xcol[i]);

      if (!ret) {
         Warning("MakeOpaqueColors",
                 "failed to allocate color %hd, %hd, %hd", xcol[i].red,
                 xcol[i].green, xcol[i].blue);
      // assumes that in case of failure xcol[i].pixel is not changed
      }
   }

   gCws->new_colors = new ULong_t[ncolors];
   gCws->ncolors = ncolors;

   for (i = 0; i < ncolors; i++) {
      gCws->new_colors[i] = xcol[i].pixel;
   }

   delete []xcol;
}

//______________________________________________________________________________
Int_t TGWin32::FindColor(ULong_t pixel, ULong_t * orgcolors, Int_t ncolors)
{
   // Returns index in orgcolors (and new_colors) for pixel.

   for (int i = 0; i < ncolors; i++) {
      if (pixel == orgcolors[i]) return i;
   }
   Error("FindColor", "did not find color, should never happen!");

   return 0;
}

//______________________________________________________________________________
void TGWin32::SetRGB(int cindex, float r, float g, float b)
{
   // Set color intensities for given color index.
   // cindex     : color index
   // r,g,b      : red, green, blue intensities between 0.0 and 1.0

   GdkColor xcol;

   if (fColormap && cindex >= 0 && cindex < kMAXCOL) {
      xcol.red = (unsigned short) (r * kBIGGEST_RGB_VALUE);
      xcol.green = (unsigned short) (g * kBIGGEST_RGB_VALUE);
      xcol.blue = (unsigned short) (b * kBIGGEST_RGB_VALUE);
      xcol.pixel = RGB(xcol.red, xcol.green, xcol.blue);

      if (gColors[cindex].defined == 1) {
         gColors[cindex].defined = 0;
      }

      Int_t ret = gdk_colormap_alloc_color(fColormap, &xcol, 1, 1);
      if (ret != 0) {
         gColors[cindex].defined = 1;
         gColors[cindex].color.pixel = xcol.pixel;
         gColors[cindex].color.red = r;
         gColors[cindex].color.green = g;
         gColors[cindex].color.blue = b;
      }
   }
}

//______________________________________________________________________________
void TGWin32::SetTextAlign(Short_t talign)
{
   // Set text alignment.
   // txalh   : horizontal text alignment
   // txalv   : vertical text alignment

   static Short_t current = 0;
   if (talign==current) return;
   current = talign;

   Int_t txalh = talign / 10;
   Int_t txalv = talign % 10;
   fTextAlignH = txalh;
   fTextAlignV = txalv;

   switch (txalh) {

   case 0:
   case 1:
      switch (txalv) {          //left
      case 1:
         fTextAlign = 7;        //bottom
         break;
      case 2:
         fTextAlign = 4;        //center
         break;
      case 3:
         fTextAlign = 1;        //top
         break;
      }
      break;
   case 2:
      switch (txalv) {          //center
      case 1:
         fTextAlign = 8;        //bottom
         break;
      case 2:
         fTextAlign = 5;        //center
         break;
      case 3:
         fTextAlign = 2;        //top
         break;
      }
      break;
   case 3:
      switch (txalv) {          //right
      case 1:
         fTextAlign = 9;        //bottom
         break;
      case 2:
         fTextAlign = 6;        //center
         break;
      case 3:
         fTextAlign = 3;        //top
         break;
      }
      break;
   }
}

//______________________________________________________________________________
void TGWin32::SetTextColor(Color_t cindex)
{
   // Set color index for text.

   static Int_t current = 0;
   GdkGCValues values;
   if ((cindex < 0) || (Int_t(cindex)==current)) return;

   SetColor(gGCtext, Int_t(cindex));
   gdk_gc_get_values(gGCtext, &values);
   gdk_gc_set_foreground(gGCinvt, &values.background);
   gdk_gc_set_background(gGCinvt, &values.foreground);
   gdk_gc_set_background(gGCtext, (GdkColor *) & gColors[0].color);
   current = Int_t(cindex);
}

//______________________________________________________________________________
void TGWin32::Sync(int mode)
{
}

//______________________________________________________________________________
void TGWin32::UpdateWindow(int mode)
{
   // Update display.
   // mode : (1) update
   //        (0) sync
   //
   // Synchronise client and server once (not permanent).
   // Copy the pixmap gCws->drawing on the window gCws->window
   // if the double buffer is on.

   if (gCws->double_buffer) {
      gdk_window_copy_area(gCws->window, gGCpxmp, 0, 0,
                           gCws->drawing, 0, 0, gCws->width, gCws->height);
   }
   Update(mode);
}

//______________________________________________________________________________
void TGWin32::Warp(int ix, int iy, Window_t id)
{
   // Set pointer position.
   // ix       : New X coordinate of pointer
   // iy       : New Y coordinate of pointer
   // Coordinates are relative to the origin of the window id
   // or to the origin of the current window if id == 0.

   if (!id) return;

   POINT cpt, tmp;
   HWND dw;
   if (!id)
      dw = (HWND) GDK_DRAWABLE_XID((GdkWindow *)gCws->window);
   else
      dw = (HWND) GDK_DRAWABLE_XID((GdkWindow *)id);
   GetCursorPos(&cpt);
   tmp.x = ix > 0 ? ix : cpt.x;
   tmp.y = iy > 0 ? iy : cpt.y;
   ClientToScreen(dw, &tmp);
   SetCursorPos(tmp.x, tmp.y);
}

//______________________________________________________________________________
void TGWin32::WritePixmap(int wid, unsigned int w, unsigned int h,
                          char *pxname)
{
   // Write the pixmap wid in the bitmap file pxname.
   // wid         : Pixmap address
   // w,h         : Width and height of the pixmap.
   // lenname     : pixmap name length
   // pxname      : pixmap name

   int wval, hval;
   wval = w;
   hval = h;

   if (!fWindows) return;
   gTws = &fWindows[wid];
//   XWriteBitmapFile(fDisplay,pxname,(Pixmap)gTws->drawing,wval,hval,-1,-1);
}


//
// Functions for GIFencode()
//

static FILE *gGifFile;           // output unit used WriteGIF and PutByte
static GdkImage *gGifImage = 0;  // image used in WriteGIF and GetPixel

extern "C" {
   int GIFquantize(UInt_t width, UInt_t height, Int_t * ncol, Byte_t * red,
                   Byte_t * green, Byte_t * blue, Byte_t * outputBuf,
                   Byte_t * outputCmap);
   long GIFencode(int Width, int Height, Int_t Ncol, Byte_t R[],
                  Byte_t G[], Byte_t B[], Byte_t ScLine[],
                  void (*get_scline) (int, int, Byte_t *),
                  void (*pb) (Byte_t));
   int GIFdecode(Byte_t * GIFarr, Byte_t * PIXarr, int *Width, int *Height,
                 int *Ncols, Byte_t * R, Byte_t * G, Byte_t * B);
   int GIFinfo(Byte_t * GIFarr, int *Width, int *Height, int *Ncols);
}


//______________________________________________________________________________
static void GetPixel(int y, int width, Byte_t * scline)
{
   // Get pixels in line y and put in array scline.

   for (int i = 0; i < width; i++) {
       scline[i] = Byte_t(GetPixelImage((Drawable_t)gGifImage, i, y));
   }
}

//______________________________________________________________________________
static void PutByte(Byte_t b)
{
   // Put byte b in output stream.

   if (ferror(gGifFile) == 0) fputc(b, gGifFile);
}

//______________________________________________________________________________
void TGWin32::ImgPickPalette(GdkImage * image, Int_t & ncol, Int_t * &R,
                             Int_t * &G, Int_t * &B)
{
   // Returns in R G B the ncol colors of the palette used by the image.
   // The image pixels are changed to index values in these R G B arrays.
   // This produces a colormap with only the used colors (so even on displays
   // with more than 8 planes we will be able to create GIF's when the image
   // contains no more than 256 different colors). If it does contain more
   // colors we will have to use GIFquantize to reduce the number of colors.
   // The R G B arrays must be deleted by the caller.

   ULong_t *orgcolors = 0;
   Int_t maxcolors = 0, ncolors;

   // collect different image colors
   int x, y;
   for (x = 0; x < (int) gCws->width; x++) {
      for (y = 0; y < (int) gCws->height; y++) {
         ULong_t pixel = GetPixelImage((Drawable_t)image, x, y);
         CollectImageColors(pixel, orgcolors, ncolors, maxcolors);
      }
   }

   // get RGB values belonging to pixels
   GdkColor *xcol = new GdkColor[ncolors];

   int i;
   for (i = 0; i < ncolors; i++) {
      xcol[i].pixel = orgcolors[i];
//      xcol[i].red   = xcol[i].green = xcol[i].blue = 0;
      xcol[i].red = GetRValue(xcol[i].pixel);
      xcol[i].green = GetGValue(xcol[i].pixel);
      xcol[i].blue = GetBValue(xcol[i].pixel);
   }

   GdkColorContext *cc;
   cc =  gdk_color_context_new(gdk_visual_get_system(), (GdkColormap *)fColormap);
   gdk_color_context_query_colors(cc, xcol, ncolors);

   // create RGB arrays and store RGB's for each color and set number of colors
   // (space must be delete by caller)
   R = new Int_t[ncolors];
   G = new Int_t[ncolors];
   B = new Int_t[ncolors];

   for (i = 0; i < ncolors; i++) {
      R[i] = xcol[i].red;
      G[i] = xcol[i].green;
      B[i] = xcol[i].blue;
   }
   ncol = ncolors;

   // update image with indices (pixels) into the new RGB colormap
   for (x = 0; x < (int) gCws->width; x++) {
      for (y = 0; y < (int) gCws->height; y++) {
         ULong_t pixel = GetPixelImage((Drawable_t)image, x, y);
         Int_t idx = FindColor(pixel, orgcolors, ncolors);
         PutPixel((Drawable_t)image, x, y, idx);
      }
   }

   // cleanup
   delete[]xcol;
   ::operator delete(orgcolors);
}

//______________________________________________________________________________
Int_t TGWin32::WriteGIF(char *name)
{
   // Writes the current window into GIF file.

   Byte_t scline[2000], r[256], b[256], g[256];
   Int_t *R, *G, *B;
   Int_t ncol, maxcol, i;

   if (gGifImage) {
      gdk_image_unref((GdkImage *)gGifImage);
   }

   gGifImage = gdk_image_get((GdkDrawable*)gCws->drawing, 0, 0,
                             gCws->width, gCws->height);

   ImgPickPalette(gGifImage, ncol, R, G, B);

   if (ncol > 256) {
      //GIFquantize(...);
      Error("WriteGIF",
            "can not create GIF of image containing more than 256 colors");
   }

   maxcol = 0;
   for (i = 0; i < ncol; i++) {
      if (maxcol < R[i]) maxcol = R[i];
      if (maxcol < G[i]) maxcol = G[i];
      if (maxcol < B[i]) maxcol = B[i];
      r[i] = 0;
      g[i] = 0;
      b[i] = 0;
   }
   if (maxcol != 0) {
      for (i = 0; i < ncol; i++) {
         r[i] = R[i] * 255 / maxcol;
         g[i] = G[i] * 255 / maxcol;
         b[i] = B[i] * 255 / maxcol;
      }
   }

   gGifFile = fopen(name, "wb");

   if (gGifFile) {
      GIFencode(gCws->width, gCws->height,
          ncol, r, g, b, scline, ::GetPixel, PutByte);
      fclose(gGifFile);
      i = 1;
    } else {
      Error("WriteGIF","cannot write file: %s",name);
      i = 0;
   }
   delete[]R;
   delete[]G;
   delete[]B;

   return i;
}

//______________________________________________________________________________
void TGWin32::PutImage(int offset, int itran, int x0, int y0, int nx,
                       int ny, int xmin, int ymin, int xmax, int ymax,
                       unsigned char *image, Drawable_t wid)
{
   // Draw image.

   const int MAX_SEGMENT = 20;
   int i, n, x, y, xcur, x1, x2, y1, y2;
   unsigned char *jimg, *jbase, icol;
   int nlines[256];
   GdkSegment lines[256][MAX_SEGMENT];
   GdkDrawable *id;

   if (wid) {
      id = (GdkDrawable*)wid;
   } else {
      id = gCws->drawing;
   }

   for (i = 0; i < 256; i++) nlines[i] = 0;

   x1 = x0 + xmin;
   y1 = y0 + ny - ymax - 1;
   x2 = x0 + xmax;
   y2 = y0 + ny - ymin - 1;
   jbase = image + (ymin - 1) * nx + xmin;

   for (y = y2; y >= y1; y--) {
      xcur = x1;
      jbase += nx;
      for (jimg = jbase, icol = *jimg++, x = x1 + 1; x <= x2; jimg++, x++) {
         if (icol != *jimg) {
            if (icol != itran) {
               n = nlines[icol]++;
               lines[icol][n].x1 = xcur;
               lines[icol][n].y1 = y;
               lines[icol][n].x2 = x - 1;
               lines[icol][n].y2 = y;
               if (nlines[icol] == MAX_SEGMENT) {
                  SetColor(gGCline, (int) icol + offset);
                  gdk_win32_draw_segments(id, (GdkGC *) gGCline,
                                       (GdkSegment *) &lines[icol][0], MAX_SEGMENT);
                  nlines[icol] = 0;
               }
            }
            icol = *jimg;
            xcur = x;
         }
      }
      if (icol != itran) {
         n = nlines[icol]++;
         lines[icol][n].x1 = xcur;
         lines[icol][n].y1 = y;
         lines[icol][n].x2 = x - 1;
         lines[icol][n].y2 = y;
         if (nlines[icol] == MAX_SEGMENT) {
            SetColor(gGCline, (int) icol + offset);
            gdk_win32_draw_segments(id, (GdkGC *) gGCline,
                              (GdkSegment *)&lines[icol][0], MAX_SEGMENT);
            nlines[icol] = 0;
         }
      }
   }

   for (i = 0; i < 256; i++) {
      if (nlines[i] != 0) {
         SetColor(gGCline, i + offset);
         gdk_win32_draw_segments(id, (GdkGC *) gGCline,
                           (GdkSegment *)&lines[icol][0], nlines[i]);
      }
   }
}

//______________________________________________________________________________
Pixmap_t TGWin32::ReadGIF(int x0, int y0, const char *file, Window_t id)
{
   // If id is NULL - loads the specified gif file at position [x0,y0] in the
   // current window. Otherwise creates pixmap from gif file

   FILE *fd;
   Seek_t filesize;
   unsigned char *GIFarr, *PIXarr, R[256], G[256], B[256], *j1, *j2, icol;
   int i, j, k, width, height, ncolor, irep, offset;
   float rr, gg, bb;
   Pixmap_t pic = 0;

   fd = fopen(file, "r+b");
   if (!fd) {
      Error("ReadGIF", "unable to open GIF file");
      return pic;
   }

   fseek(fd, 0L, 2);
   filesize = Seek_t(ftell(fd));
   fseek(fd, 0L, 0);

   if (!(GIFarr = (unsigned char *) calloc(filesize + 256, 1))) {
      Error("ReadGIF", "unable to allocate array for gif");
      return pic;
   }

   if (fread(GIFarr, filesize, 1, fd) != 1) {
      Error("ReadGIF", "GIF file read failed");
      return pic;
   }

   irep = GIFinfo(GIFarr, &width, &height, &ncolor);
   if (irep != 0) {
      return pic;
   }

   if (!(PIXarr = (unsigned char *) calloc((width * height), 1))) {
      Error("ReadGIF", "unable to allocate array for image");
      return pic;
   }

   irep = GIFdecode(GIFarr, PIXarr, &width, &height, &ncolor, R, G, B);
   if (irep != 0) {
      return pic;
   }
   // S E T   P A L E T T E

   offset = 8;

   for (i = 0; i < ncolor; i++) {
      rr = R[i] / 255.;
      gg = G[i] / 255.;
      bb = B[i] / 255.;
      j = i + offset;
      SetRGB(j, rr, gg, bb);
   }

   // O U T P U T   I M A G E

   for (i = 1; i <= height / 2; i++) {
      j1 = PIXarr + (i - 1) * width;
      j2 = PIXarr + (height - i) * width;
      for (k = 0; k < width; k++) {
         icol = *j1;
         *j1++ = *j2;
         *j2++ = icol;
      }
   }

   if (id) pic = CreatePixmap(id, width, height);
   PutImage(offset, -1, x0, y0, width, height, 0, 0, width-1, height-1, PIXarr, pic);

   if (pic) return pic;
   else if (gCws->drawing) return  (Pixmap_t)gCws->drawing;
   else return 0;
}

//////////////////////////// GWin32Gui //////////////////////////////////////////
//______________________________________________________________________________
void TGWin32::MapWindow(Window_t id)
{
   // Map window on screen.

   if (!id) return;

   gdk_window_show((GdkWindow *)id);
   if ((GDK_DRAWABLE_TYPE((GdkWindow *)id) != GDK_WINDOW_TEMP) &&
       (GetParent(id) == GetDefaultRootWindow())) {
      HWND window = (HWND)GDK_DRAWABLE_XID((GdkWindow *)id);
      ::SetForegroundWindow(window);
   }
}

//______________________________________________________________________________
void TGWin32::MapSubwindows(Window_t id)
{
   //

   if (!id) return;

   HWND wp;
   EnumChildWindows((HWND)GDK_DRAWABLE_XID((GdkWindow *)id),
                    EnumChildProc, (LPARAM) NULL);
}

//______________________________________________________________________________
void TGWin32::MapRaised(Window_t id)
{
   // Map window on screen and put on top of all windows.

   if (!id) return;

   HWND hwnd = ::GetForegroundWindow();
   HWND window = (HWND)GDK_DRAWABLE_XID((GdkWindow *)id);
   gdk_window_show((GdkWindow *)id);
   if (GDK_DRAWABLE_TYPE((GdkWindow *)id) != GDK_WINDOW_TEMP)
      ::SetForegroundWindow(window);

   if (hwnd == gConsoleWindow) {
      RECT r1, r2, r3;
      ::GetWindowRect(gConsoleWindow, &r1);
      ::GetWindowRect(window, &r2);
      if (!::IntersectRect(&r3, &r2, &r1)) {
          ::SetForegroundWindow(gConsoleWindow);
      }
   }
}

//______________________________________________________________________________
void TGWin32::UnmapWindow(Window_t id)
{
   // Unmap window from screen.

   if (!id) return;

   gdk_window_hide((GdkWindow *) id);
}

//______________________________________________________________________________
void TGWin32::DestroyWindow(Window_t id)
{
   // Destroy window.

   if (!id) return;

   gdk_window_destroy((GdkDrawable *) id, kTRUE);
}

//______________________________________________________________________________
void TGWin32::DestroySubwindows(Window_t id)
{
   // Destroy all internal subwindows

   if (!id) return;

   gdk_window_destroy((GdkDrawable *) id, kFALSE);
}

//______________________________________________________________________________
void TGWin32::RaiseWindow(Window_t id)
{
   // Put window on top of window stack.

   if (!id) return;

   HWND window = (HWND)GDK_DRAWABLE_XID((GdkWindow *)id);
   if (GDK_DRAWABLE_TYPE((GdkWindow *)id) == GDK_WINDOW_TEMP) {
       ::SetWindowPos(window, HWND_TOPMOST,  0, 0, 0, 0,
                      SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
   }
   else {
      ::BringWindowToTop(window);
      ::SetForegroundWindow(window);
   }
}

//______________________________________________________________________________
void TGWin32::LowerWindow(Window_t id)
{
   // Lower window so it lays below all its siblings.

   if (!id) return;

   HWND window = (HWND)GDK_DRAWABLE_XID((GdkWindow *)id);
   ::SetWindowPos(window, HWND_BOTTOM, 0, 0, 0, 0,
                  SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
}

//______________________________________________________________________________
void TGWin32::MoveWindow(Window_t id, Int_t x, Int_t y)
{
   // Move a window.

   if (!id) return;

   gdk_window_move((GdkDrawable *) id, x, y);
}

//______________________________________________________________________________
void TGWin32::MoveResizeWindow(Window_t id, Int_t x, Int_t y, UInt_t w,
                               UInt_t h)
{
   // Move and resize a window.

   if (!id) return;

   gdk_window_move_resize((GdkWindow *) id, x, y, w, h);
}

//______________________________________________________________________________
void TGWin32::ResizeWindow(Window_t id, UInt_t w, UInt_t h)
{
   // Resize the window.

   if (!id) return;

   gdk_window_resize((GdkWindow *) id, w, h);
}

//______________________________________________________________________________
void TGWin32::IconifyWindow(Window_t id)
{
   // Iconify the window.

   if (!id) return;

   gdk_window_lower((GdkWindow *) id);
   ::CloseWindow((HWND)GDK_DRAWABLE_XID((GdkWindow *)id));
}

//______________________________________________________________________________
void TGWin32::ReparentWindow(Window_t id, Window_t pid, Int_t x, Int_t y)
{
   // Reparent window, make pid the new parent and position the window at
   // position (x,y) in new parent.

   if (!id) return;

   gdk_window_reparent((GdkWindow *)id, (GdkWindow *)pid, x, y);
}

//______________________________________________________________________________
void TGWin32::SetWindowBackground(Window_t id, ULong_t color)
{
   // Set the window background color.

   if (!id) return;

   GdkColor back;
   back.pixel = color;
   back.red = GetRValue(color);
   back.green = GetGValue(color);
   back.blue = GetBValue(color);

   gdk_window_set_background((GdkWindow *) id, &back);
}

//______________________________________________________________________________
void TGWin32::SetWindowBackgroundPixmap(Window_t id, Pixmap_t pxm)
{
   // Set pixmap as window background.

   if (!id) return;

   gdk_window_set_back_pixmap((GdkWindow *) id, (GdkPixmap *) pxm, 0);
}

//______________________________________________________________________________
Window_t TGWin32::CreateWindow(Window_t parent, Int_t x, Int_t y,
                               UInt_t w, UInt_t h, UInt_t border,
                               Int_t depth, UInt_t clss,
                               void *visual, SetWindowAttributes_t * attr,
                               UInt_t wtype)
{
   // Return handle to newly created gdk window.

   GdkWMDecoration deco;
   GdkWindowAttr xattr;
   GdkWindow *newWin;
   GdkColor background_color;
   ULong_t xmask = 0;
   UInt_t xevmask;

   if (attr) {
      MapSetWindowAttributes(attr, xmask, xattr);
      xattr.window_type = GDK_WINDOW_CHILD;
      if (wtype & kTransientFrame) {
         xattr.window_type = GDK_WINDOW_DIALOG;
      }
      if (wtype & kMainFrame) {
         xattr.window_type = GDK_WINDOW_TOPLEVEL;
      }
      if (wtype & kTempFrame) {
         xattr.window_type = GDK_WINDOW_TEMP;
      }
      newWin = gdk_window_new((GdkWindow *) parent, &xattr, xmask);
   } else {
      xattr.width = w;
      xattr.height = h;
      xattr.wclass = GDK_INPUT_OUTPUT;
      xattr.event_mask = 0L;    //GDK_ALL_EVENTS_MASK;
      xattr.event_mask |= GDK_EXPOSURE_MASK | GDK_STRUCTURE_MASK |
          GDK_PROPERTY_CHANGE_MASK;
//                          GDK_ENTER_NOTIFY_MASK | GDK_LEAVE_NOTIFY_MASK;
      if (x >= 0) {
         xattr.x = x;
      } else {
         xattr.x = -1.0 * x;
      }
      if (y >= 0) {
         xattr.y = y;
      } else {
         xattr.y = -1.0 * y;
      }
      xattr.colormap = gdk_colormap_get_system();
      xattr.cursor = NULL;
      xattr.override_redirect = TRUE;
      if ((xattr.y > 0) && (xattr.x > 0)) {
         xmask = GDK_WA_X | GDK_WA_Y | GDK_WA_COLORMAP |
             GDK_WA_WMCLASS | GDK_WA_NOREDIR;
      } else {
         xmask = GDK_WA_COLORMAP | GDK_WA_WMCLASS | GDK_WA_NOREDIR;
      }
      if (visual != NULL) {
         xattr.visual = (GdkVisual *) visual;
         xmask |= GDK_WA_VISUAL;
      } else {
         xattr.visual = gdk_visual_get_system();
         xmask |= GDK_WA_VISUAL;
      }
      xattr.window_type = GDK_WINDOW_CHILD;
      if (wtype & kTransientFrame) {
         xattr.window_type = GDK_WINDOW_DIALOG;
      }
      if (wtype & kMainFrame) {
         xattr.window_type = GDK_WINDOW_TOPLEVEL;
      }
      if (wtype & kTempFrame) {
         xattr.window_type = GDK_WINDOW_TEMP;
      }
      newWin = gdk_window_new((GdkWindow *) parent, &xattr, xmask);
      gdk_window_set_events(newWin, (GdkEventMask) 0L);
   }
   if (border > 0) {
      gdk_window_set_decorations(newWin,
                                 (GdkWMDecoration) GDK_DECOR_BORDER);
   }
   if (attr) {
      if ((attr->fMask & kWABackPixmap)) {
         if (attr->fBackgroundPixmap == kNone) {
            gdk_window_set_back_pixmap(newWin, (GdkPixmap *) GDK_NONE, 0);
         } else if (attr->fBackgroundPixmap == kParentRelative) {
            gdk_window_set_back_pixmap(newWin, (GdkPixmap *) GDK_NONE, 1);
         } else {
            gdk_window_set_back_pixmap(newWin,
                                       (GdkPixmap *) attr->
                                       fBackgroundPixmap, 0);
         }
      }
      if ((attr->fMask & kWABackPixel)) {
         background_color.pixel = attr->fBackgroundPixel;
         background_color.red = GetRValue(attr->fBackgroundPixel);
         background_color.green = GetGValue(attr->fBackgroundPixel);
         background_color.blue = GetBValue(attr->fBackgroundPixel);
         gdk_window_set_background(newWin, &background_color);
      }
   }
   return (Window_t) newWin;
}

//______________________________________________________________________________
void TGWin32::MapEventMask(UInt_t & emask, UInt_t & xemask, Bool_t tox)
{
   // Map event mask to or from gdk.

   if (tox) {
      Long_t lxemask = 0L;
      if ((emask & kKeyPressMask)) {
         lxemask |= GDK_KEY_PRESS_MASK;
      }
      if ((emask & kKeyReleaseMask)) {
         lxemask |= GDK_KEY_RELEASE_MASK;
      }
      if ((emask & kButtonPressMask)) {
         lxemask |= GDK_BUTTON_PRESS_MASK;
      }
      if ((emask & kButtonReleaseMask)) {
         lxemask |= GDK_BUTTON_RELEASE_MASK;
      }
      if ((emask & kPointerMotionMask)) {
         lxemask |= GDK_POINTER_MOTION_MASK;
      }
      if ((emask & kButtonMotionMask)) {
         lxemask |= GDK_BUTTON_MOTION_MASK;
      }
      if ((emask & kExposureMask)) {
         lxemask |= GDK_EXPOSURE_MASK;
      }
      if ((emask & kStructureNotifyMask)) {
         lxemask |= GDK_STRUCTURE_MASK;
      }
      if ((emask & kEnterWindowMask)) {
         lxemask |= GDK_ENTER_NOTIFY_MASK;
      }
      if ((emask & kLeaveWindowMask)) {
         lxemask |= GDK_LEAVE_NOTIFY_MASK;
      }
      if ((emask & kFocusChangeMask)) {
         lxemask |= GDK_FOCUS_CHANGE_MASK;
      }
      xemask = (UInt_t) lxemask;
   } else {
      emask = 0;
      if ((xemask & GDK_KEY_PRESS_MASK)) {
         emask |= kKeyPressMask;
      }
      if ((xemask & GDK_KEY_RELEASE_MASK)) {
         emask |= kKeyReleaseMask;
      }
      if ((xemask & GDK_BUTTON_PRESS_MASK)) {
         emask |= kButtonPressMask;
      }
      if ((xemask & GDK_BUTTON_RELEASE_MASK)) {
         emask |= kButtonReleaseMask;
      }
      if ((xemask & GDK_POINTER_MOTION_MASK)) {
         emask |= kPointerMotionMask;
      }
      if ((xemask & GDK_BUTTON_MOTION_MASK)) {
         emask |= kButtonMotionMask;
      }
      if ((xemask & GDK_EXPOSURE_MASK)) {
         emask |= kExposureMask;
      }
      if ((xemask & GDK_STRUCTURE_MASK)) {
         emask |= kStructureNotifyMask;
      }
      if ((xemask & GDK_ENTER_NOTIFY_MASK)) {
         emask |= kEnterWindowMask;
      }
      if ((xemask & GDK_LEAVE_NOTIFY_MASK)) {
         emask |= kLeaveWindowMask;
      }
      if ((xemask & GDK_FOCUS_CHANGE_MASK)) {
         emask |= kFocusChangeMask;
      }
   }
}

//______________________________________________________________________________
void TGWin32::MapSetWindowAttributes(SetWindowAttributes_t * attr,
                                     ULong_t & xmask,
                                     GdkWindowAttr & xattr)
{
   // Map a SetWindowAttributes_t to a GdkWindowAttr structure.

   Mask_t mask = attr->fMask;
   xmask = 0;

   if ((mask & kWAOverrideRedirect)) {
      xmask |= GDK_WA_NOREDIR;
      xattr.override_redirect = attr->fOverrideRedirect;
   }
   if ((mask & kWAEventMask)) {
      UInt_t xmsk, msk = (UInt_t) attr->fEventMask;
      MapEventMask(msk, xmsk, kTRUE);
      xattr.event_mask = xmsk;
   }
   if ((mask & kWAColormap)) {
      xmask |= GDK_WA_COLORMAP;
      xattr.colormap = (GdkColormap *) attr->fColormap;
   }
   if ((mask & kWACursor)) {
      xmask |= GDK_WA_CURSOR;
      if (attr->fCursor != kNone) {
         xattr.cursor = (GdkCursor *) attr->fCursor;
      }
   }
   xattr.wclass = GDK_INPUT_OUTPUT;
}

//______________________________________________________________________________
void TGWin32::MapGCValues(GCValues_t & gval,
                          ULong_t & xmask, GdkGCValues & xgval, Bool_t tox)
{
   // Map a GCValues_t to a XCGValues structure if tox is true. Map
   // the other way in case tox is false.

   if (tox) {
      // map GCValues_t to XGCValues
      Mask_t mask = gval.fMask;
      xmask = 0;

      if ((mask & kGCFunction)) {
         xmask |= GDK_GC_FUNCTION;
         switch (gval.fFunction) {
         case kGXclear:
            xgval.function = GDK_CLEAR;
            break;
         case kGXand:
            xgval.function = GDK_AND;
            break;
         case kGXandReverse:
            xgval.function = GDK_AND_REVERSE;
            break;
         case kGXcopy:
            xgval.function = GDK_COPY;
            break;
         case kGXandInverted:
            xgval.function = GDK_AND_INVERT;
            break;
         case kGXnoop:
            xgval.function = GDK_NOOP;
            break;
         case kGXxor:
            xgval.function = GDK_XOR;
            break;
         case kGXor:
            xgval.function = GDK_OR;
            break;
         case kGXequiv:
            xgval.function = GDK_EQUIV;
            break;
         case kGXinvert:
            xgval.function = GDK_INVERT;
            break;
         case kGXorReverse:
            xgval.function = GDK_OR_REVERSE;
            break;
         case kGXcopyInverted:
            xgval.function = GDK_COPY_INVERT;
            break;
         case kGXorInverted:
            xgval.function = GDK_OR_INVERT;
            break;
         case kGXnand:
            xgval.function = GDK_NAND;
            break;
         case kGXset:
            xgval.function = GDK_SET;
            break;
         }
      }
      if (mask & kGCSubwindowMode) {
         xmask |= GDK_GC_SUBWINDOW;
         if (gval.fSubwindowMode == kIncludeInferiors) {
            xgval.subwindow_mode = GDK_INCLUDE_INFERIORS;
         } else {
            xgval.subwindow_mode = GDK_CLIP_BY_CHILDREN;
         }
      }
      if ((mask & kGCForeground)) {
         xmask |= GDK_GC_FOREGROUND;
         xgval.foreground.pixel = gval.fForeground;
         xgval.foreground.red = GetRValue(gval.fForeground);
         xgval.foreground.green = GetGValue(gval.fForeground);
         xgval.foreground.blue = GetBValue(gval.fForeground);
      }
      if ((mask & kGCBackground)) {
         xmask |= GDK_GC_BACKGROUND;
         xgval.background.pixel = gval.fBackground;
         xgval.background.red = GetRValue(gval.fBackground);
         xgval.background.green = GetGValue(gval.fBackground);
         xgval.background.blue = GetBValue(gval.fBackground);
      }
      if (mask & kGCLineWidth) {
         xmask |= GDK_GC_LINE_WIDTH;
         xgval.line_width = gval.fLineWidth;
      }
      if (mask & kGCLineStyle) {
         xmask |= GDK_GC_LINE_STYLE;
         xgval.line_style = (GdkLineStyle) gval.fLineStyle; // ident mapping
      }
      if (mask & kGCCapStyle) {
         xmask |= GDK_GC_CAP_STYLE;
         xgval.cap_style = (GdkCapStyle) gval.fCapStyle; // ident mapping
      }
      if (mask & kGCJoinStyle) {
         xmask |= GDK_GC_JOIN_STYLE;
         xgval.join_style = (GdkJoinStyle) gval.fJoinStyle; // ident mapping
      }
      if ((mask & kGCFillStyle)) {
         xmask |= GDK_GC_FILL;
         xgval.fill = (GdkFill) gval.fFillStyle;   // ident mapping
      }
      if ((mask & kGCTile)) {
         xmask |= GDK_GC_TILE;
         xgval.tile = (GdkPixmap *) gval.fTile;
      }
      if ((mask & kGCStipple)) {
         xmask |= GDK_GC_STIPPLE;
         xgval.stipple = (GdkPixmap *) gval.fStipple;
      }
      if ((mask & kGCTileStipXOrigin)) {
         xmask |= GDK_GC_TS_X_ORIGIN;
         xgval.ts_x_origin = gval.fTsXOrigin;
      }
      if ((mask & kGCTileStipYOrigin)) {
         xmask |= GDK_GC_TS_Y_ORIGIN;
         xgval.ts_y_origin = gval.fTsYOrigin;
      }
      if ((mask & kGCFont)) {
         xmask |= GDK_GC_FONT;
         xgval.font = (GdkFont *) gval.fFont;
      }
      if ((mask & kGCGraphicsExposures)) {
         xmask |= GDK_GC_EXPOSURES;
         xgval.graphics_exposures = gval.fGraphicsExposures;
      }
      if ((mask & kGCClipXOrigin)) {
         xmask |= GDK_GC_CLIP_X_ORIGIN;
         xgval.clip_x_origin = gval.fClipXOrigin;
      }
      if ((mask & kGCClipYOrigin)) {
         xmask |= GDK_GC_CLIP_Y_ORIGIN;
         xgval.clip_y_origin = gval.fClipYOrigin;
      }
      if ((mask & kGCClipMask)) {
         xmask |= GDK_GC_CLIP_MASK;
         xgval.clip_mask = (GdkPixmap *) gval.fClipMask;
      }
   } else {
      // map XValues to GCValues_t
      Mask_t mask = 0;

      if ((xmask & GDK_GC_FUNCTION)) {
         mask |= kGCFunction;
         gval.fFunction = (EGraphicsFunction) xgval.function;	// ident mapping
         switch (xgval.function) {
         case GDK_CLEAR:
            gval.fFunction = kGXclear;
            break;
         case GDK_AND:
            gval.fFunction = kGXand;
            break;
         case GDK_AND_REVERSE:
            gval.fFunction = kGXandReverse;
            break;
         case GDK_COPY:
            gval.fFunction = kGXcopy;
            break;
         case GDK_AND_INVERT:
            gval.fFunction = kGXandInverted;
            break;
         case GDK_NOOP:
            gval.fFunction = kGXnoop;
            break;
         case GDK_XOR:
            gval.fFunction = kGXxor;
            break;
         case GDK_OR:
            gval.fFunction = kGXor;
            break;
         case GDK_EQUIV:
            gval.fFunction = kGXequiv;
            break;
         case GDK_INVERT:
            gval.fFunction = kGXinvert;
            break;
         case GDK_OR_REVERSE:
            gval.fFunction = kGXorReverse;
            break;
         case GDK_COPY_INVERT:
            gval.fFunction = kGXcopyInverted;
            break;
         case GDK_OR_INVERT:
            gval.fFunction = kGXorInverted;
            break;
         case GDK_NAND:
            gval.fFunction = kGXnand;
            break;
         case GDK_SET:
            gval.fFunction = kGXset;
            break;
         }
      }
      if (xmask & GDK_GC_SUBWINDOW) {
         mask |= kGCSubwindowMode;
         if (xgval.subwindow_mode == GDK_INCLUDE_INFERIORS)
            gval.fSubwindowMode = kIncludeInferiors;
         else
            gval.fSubwindowMode = kClipByChildren;
      }
      if ((xmask & GDK_GC_FOREGROUND)) {
         mask |= kGCForeground;
         gval.fForeground = xgval.foreground.pixel;
      }
      if ((xmask & GDK_GC_BACKGROUND)) {
         mask |= kGCBackground;
         gval.fBackground = xgval.background.pixel;
      }
      if ((xmask & GDK_GC_LINE_WIDTH)) {
         mask |= kGCLineWidth;
         gval.fLineWidth = xgval.line_width;
      }
      if ((xmask & GDK_GC_LINE_STYLE)) {
         mask |= kGCLineStyle;
         gval.fLineStyle = xgval.line_style;	// ident mapping
      }
      if ((xmask & GDK_GC_CAP_STYLE)) {
         mask |= kGCCapStyle;
         gval.fCapStyle = xgval.cap_style;	// ident mapping
      }
      if ((xmask & GDK_GC_JOIN_STYLE)) {
         mask |= kGCJoinStyle;
         gval.fJoinStyle = xgval.join_style;	// ident mapping
      }
      if ((xmask & GDK_GC_FILL)) {
         mask |= kGCFillStyle;
         gval.fFillStyle = xgval.fill;	// ident mapping
      }
      if ((xmask & GDK_GC_TILE)) {
         mask |= kGCTile;
         gval.fTile = (Pixmap_t) xgval.tile;
      }
      if ((xmask & GDK_GC_STIPPLE)) {
         mask |= kGCStipple;
         gval.fStipple = (Pixmap_t) xgval.stipple;
      }
      if ((xmask & GDK_GC_TS_X_ORIGIN)) {
         mask |= kGCTileStipXOrigin;
         gval.fTsXOrigin = xgval.ts_x_origin;
      }
      if ((xmask & GDK_GC_TS_Y_ORIGIN)) {
         mask |= kGCTileStipYOrigin;
         gval.fTsYOrigin = xgval.ts_y_origin;
      }
      if ((xmask & GDK_GC_FONT)) {
         mask |= kGCFont;
         gval.fFont = (FontH_t) xgval.font;
      }
      if ((xmask & GDK_GC_EXPOSURES)) {
         mask |= kGCGraphicsExposures;
         gval.fGraphicsExposures = (Bool_t) xgval.graphics_exposures;
      }
      if ((xmask & GDK_GC_CLIP_X_ORIGIN)) {
         mask |= kGCClipXOrigin;
         gval.fClipXOrigin = xgval.clip_x_origin;
      }
      if ((xmask & GDK_GC_CLIP_Y_ORIGIN)) {
         mask |= kGCClipYOrigin;
         gval.fClipYOrigin = xgval.clip_y_origin;
      }
      if ((xmask & GDK_GC_CLIP_MASK)) {
         mask |= kGCClipMask;
         gval.fClipMask = (Pixmap_t) xgval.clip_mask;
      }
      gval.fMask = mask;
   }
}

//______________________________________________________________________________
void TGWin32::GetWindowAttributes(Window_t id, WindowAttributes_t & attr)
{
   // Get window attributes and return filled in attributes structure.

   if (!id) return;

   GdkWindowAttr xattr;

   gdk_window_get_geometry((GdkWindow *) id,
                           &attr.fX,
                           &attr.fY,
                           &attr.fWidth, &attr.fHeight, &attr.fDepth);

   attr.fRoot = (Window_t) GDK_ROOT_PARENT();
   attr.fColormap = (Colormap_t) gdk_window_get_colormap((GdkWindow *) id);
   attr.fBorderWidth = 0;
   attr.fVisual = gdk_window_get_visual((GdkWindow *) id);
   attr.fClass = kInputOutput;
   attr.fBackingStore = kNotUseful;
   attr.fSaveUnder = kFALSE;
   attr.fMapInstalled = kTRUE;
   attr.fOverrideRedirect = kFALSE;   // boolean value for override-redirect

   if (!gdk_window_is_visible((GdkWindow *) id)) {
      attr.fMapState = kIsUnmapped;
   } else if (!gdk_window_is_viewable((GdkWindow *) id)) {
      attr.fMapState = kIsUnviewable;
   } else {
      attr.fMapState = kIsViewable;
   }

   UInt_t tmp_mask = (UInt_t)gdk_window_get_events((GdkWindow *) id);
   UInt_t evmask;
   MapEventMask(evmask, tmp_mask, kFALSE);

   attr.fYourEventMask = evmask;
}

//______________________________________________________________________________
Display_t TGWin32::GetDisplay() const
{
   //

   return 0;
}

//______________________________________________________________________________
Int_t TGWin32::GetDepth() const
{
   // Get maximum number of planes.

   return gdk_visual_get_best_depth();
}

//______________________________________________________________________________
Atom_t TGWin32::InternAtom(const char *atom_name, Bool_t only_if_exist)
{
   // Return atom handle for atom_name. If it does not exist
   // create it if only_if_exist is false. Atoms are used to communicate
   // between different programs (i.e. window manager) via the X server.

   GdkAtom a = gdk_atom_intern((const gchar *) atom_name, only_if_exist);

   if (a == None) return kNone;
   return (Atom_t) a;
}

//______________________________________________________________________________
Window_t TGWin32::GetDefaultRootWindow() const
{
   // Return handle to the default root window created when calling
   // XOpenDisplay().

   return (Window_t) GDK_ROOT_PARENT();
}

//______________________________________________________________________________
Window_t TGWin32::GetParent(Window_t id) const
{
   // Return the parent of the window.

   if (!id) return (Window_t)0;

   return (Window_t)gdk_window_get_parent((GdkWindow *) id);
}

//______________________________________________________________________________
FontStruct_t TGWin32::LoadQueryFont(const char *font_name)
{
   // Load font and query font. If font is not found 0 is returned,
   // otherwise an opaque pointer to the FontStruct_t.
   // Free the loaded font using DeleteFont().

   return (FontStruct_t) gdk_font_load(font_name);
}

//______________________________________________________________________________
FontH_t TGWin32::GetFontHandle(FontStruct_t fs)
{
   // Return handle to font described by font structure.

   if (fs) {
      return (FontH_t)gdk_font_ref((GdkFont *) fs);
   }
   return 0;
}

//______________________________________________________________________________
void TGWin32::DeleteFont(FontStruct_t fs)
{
   // Explicitely delete font structure obtained with LoadQueryFont().

   gdk_font_unref((GdkFont *) fs);
}

//______________________________________________________________________________
GContext_t TGWin32::CreateGC(Drawable_t id, GCValues_t *gval)
{
   // Create a graphics context using the values set in gval (but only for
   // those entries that are in the mask).

   if (!id) return (GContext_t)0;

   GdkGCValues xgval;
   ULong_t xmask = 0;

   if (gval) MapGCValues(*gval, xmask, xgval, kTRUE);

   xgval.subwindow_mode = GDK_CLIP_BY_CHILDREN;	// GDK_INCLUDE_INFERIORS;

   GdkGC *gc = gdk_gc_new_with_values((GdkDrawable *) id,
                                      &xgval, (GdkGCValuesMask)xmask);
   return (GContext_t) gc;
}

//______________________________________________________________________________
void TGWin32::ChangeGC(GContext_t gc, GCValues_t * gval)
{
   // Change entries in an existing graphics context, gc, by values from gval.

   GdkGCValues xgval;
   ULong_t xmask = 0;
   Mask_t mask = 0;

   if (gval) {
      mask = gval->fMask;
      MapGCValues(*gval, xmask, xgval, kTRUE);
   }
   if (mask & kGCForeground) {
      gdk_gc_set_foreground((GdkGC *) gc, &xgval.foreground);
   }
   if (mask & kGCBackground) {
      gdk_gc_set_background((GdkGC *) gc, &xgval.background);
   }
   if (mask & kGCFont) {
      gdk_gc_set_font((GdkGC *) gc, xgval.font);
   }
   if (mask & kGCFunction) {
      gdk_gc_set_function((GdkGC *) gc, xgval.function);
   }
   if (mask & kGCFillStyle) {
      gdk_gc_set_fill((GdkGC *) gc, xgval.fill);
   }
   if (mask & kGCTile) {
      gdk_gc_set_tile((GdkGC *) gc, xgval.tile);
   }
   if (mask & kGCStipple) {
      gdk_gc_set_stipple((GdkGC *) gc, xgval.stipple);
   }
   if ((mask & kGCTileStipXOrigin) || (mask & kGCTileStipYOrigin)) {
      gdk_gc_set_ts_origin((GdkGC *) gc, xgval.ts_x_origin,
                           xgval.ts_y_origin);
   }
   if ((mask & kGCClipXOrigin) || (mask & kGCClipYOrigin)) {
      gdk_gc_set_clip_origin((GdkGC *) gc, xgval.clip_x_origin,
                             xgval.clip_y_origin);
   }
   if (mask & kGCClipMask) {
      gdk_gc_set_clip_mask((GdkGC *) gc, xgval.clip_mask);
   }
   if (mask & kGCGraphicsExposures) {
      gdk_gc_set_exposures((GdkGC *) gc, xgval.graphics_exposures);
   }
   if (mask & kGCLineWidth) {
      gdk_gc_set_values((GdkGC *) gc, &xgval, GDK_GC_LINE_WIDTH);
   }
   if (mask & kGCLineStyle) {
      gdk_gc_set_values((GdkGC *) gc, &xgval, GDK_GC_LINE_STYLE);
   }
   if (mask & kGCCapStyle) {
      gdk_gc_set_values((GdkGC *) gc, &xgval, GDK_GC_CAP_STYLE);
   }
   if (mask & kGCJoinStyle) {
      gdk_gc_set_values((GdkGC *) gc, &xgval, GDK_GC_JOIN_STYLE);
   }
   if (mask & kGCSubwindowMode) {
      gdk_gc_set_subwindow((GdkGC *) gc, xgval.subwindow_mode);
   }
}

//______________________________________________________________________________
void TGWin32::CopyGC(GContext_t org, GContext_t dest, Mask_t mask)
{
   // Copies graphics context from org to dest. Only the values specified
   // in mask are copied. Both org and dest must exist.

   GCValues_t gval;
   GdkGCValues xgval;
   ULong_t xmask;

   if (!mask) {
      // in this case copy all fields
      mask = (Mask_t) - 1;
   }

   gval.fMask = mask;           // only set fMask used to convert to xmask
   MapGCValues(gval, xmask, xgval, kTRUE);

   gdk_gc_copy((GdkGC *) dest, (GdkGC *) org);
}

//______________________________________________________________________________
void TGWin32::DeleteGC(GContext_t gc)
{
   // Explicitely delete a graphics context.

   gdk_gc_unref((GdkGC *) gc);
}

//______________________________________________________________________________
Cursor_t TGWin32::CreateCursor(ECursor cursor)
{
   // Create cursor handle (just return cursor from cursor pool fCursors).

   return (Cursor_t) fCursors[cursor];
}

//______________________________________________________________________________
Pixmap_t TGWin32::CreatePixmap(Drawable_t id, UInt_t w, UInt_t h)
{
   // Creates a pixmap of the width and height you specified
   // and returns a pixmap ID that identifies it.

   GdkWindow *wid = (GdkWindow *)id;
   if (!id) wid =  GDK_ROOT_PARENT();

   return (Pixmap_t) gdk_pixmap_new(wid, w, h, gdk_visual_get_best_depth());
}

//______________________________________________________________________________
Pixmap_t TGWin32::CreatePixmap(Drawable_t id, const char *bitmap,
                               UInt_t width, UInt_t height,
                               ULong_t forecolor, ULong_t backcolor,
                               Int_t depth)
{
   // Create a pixmap from bitmap data. Ones will get foreground color and
   // zeroes background color.

   GdkColor fore, back;
   fore.pixel = forecolor;
   fore.red = GetRValue(forecolor);
   fore.green = GetGValue(forecolor);
   fore.blue = GetBValue(forecolor);

   back.pixel = backcolor;
   back.red = GetRValue(backcolor);
   back.green = GetGValue(backcolor);
   back.blue = GetBValue(backcolor);

   GdkWindow *wid = (GdkWindow *)id;
   if (!id) wid =  GDK_ROOT_PARENT();

   return (Pixmap_t) gdk_pixmap_create_from_data(wid, (char *) bitmap, width,
                                                 height, depth, &fore, &back);
}

//______________________________________________________________________________
Pixmap_t TGWin32::CreateBitmap(Drawable_t id, const char *bitmap,
                               UInt_t width, UInt_t height)
{
   // Create a bitmap (i.e. pixmap with depth 1) from the bitmap data.

   int siz = width*height;
   char *ibitmap = new char[siz];
   for (int i=0; i<siz; i++) {
      ibitmap[i] = ~bitmap[i];
   }
   GdkWindow *wid = (GdkWindow *)id;
   if (!id) wid =  GDK_ROOT_PARENT();

   Pixmap_t ret = (Pixmap_t) gdk_bitmap_create_from_data(wid,
                                                 (char *) ibitmap, width, height);
   delete [] ibitmap;
   return ret;
}

//______________________________________________________________________________
void TGWin32::DeletePixmap(Pixmap_t pmap)
{
   // Explicitely delete pixmap resource.

   gdk_pixmap_unref((GdkPixmap *) pmap);
}

//______________________________________________________________________________
Bool_t TGWin32::CreatePictureFromFile(Drawable_t id, const char *filename,
                                      Pixmap_t & pict,
                                      Pixmap_t & pict_mask,
                                      PictureAttributes_t & attr)
{
   // Create a picture pixmap from data on file. The picture attributes
   // are used for input and output. Returns kTRUE in case of success,
   // kFALSE otherwise. If mask does not exist it is set to kNone.

   GdkBitmap *gdk_pixmap_mask;
   if (strstr(filename, ".xpm") || strstr(filename, ".XPM")) {
      GdkWindow *wid = (GdkWindow *)id;
      if (!id) wid =  GDK_ROOT_PARENT();

      pict = (Pixmap_t) gdk_pixmap_create_from_xpm(wid, &gdk_pixmap_mask, 0,
                                                filename);
      pict_mask = (Pixmap_t) gdk_pixmap_mask;
   } else if (strstr(filename, ".gif") || strstr(filename, ".GIF")) {
      pict = ReadGIF(0, 0, filename, id);
      pict_mask = kNone;
   }

   gdk_drawable_get_size((GdkPixmap *) pict, (int *) &attr.fWidth,
                         (int *) &attr.fHeight);
   if (pict) {
      return kTRUE;
   }
   if (pict_mask) {
      pict_mask = kNone;
   }
   return kFALSE;
}

//______________________________________________________________________________
Bool_t TGWin32::CreatePictureFromData(Drawable_t id, char **data,
                                      Pixmap_t & pict,
                                      Pixmap_t & pict_mask,
                                      PictureAttributes_t & attr)
{
   // Create a pixture pixmap from data. The picture attributes
   // are used for input and output. Returns kTRUE in case of success,
   // kFALSE otherwise. If mask does not exist it is set to kNone.

   GdkBitmap *gdk_pixmap_mask;
   GdkWindow *wid = (GdkWindow *)id;
   if (!id) wid =  GDK_ROOT_PARENT();

   pict = (Pixmap_t) gdk_pixmap_create_from_xpm_d(wid, &gdk_pixmap_mask, 0,
                                                  data);
   pict_mask = (Pixmap_t) gdk_pixmap_mask;

   if (pict) {
      return kTRUE;
   }
   if (pict_mask) {
      pict_mask = kNone;
   }
   return kFALSE;
}

//______________________________________________________________________________
Bool_t TGWin32::ReadPictureDataFromFile(const char *filename, char ***ret_data)
{
   // Read picture data from file and store in ret_data. Returns kTRUE in
   // case of success, kFALSE otherwise.

   Bool_t ret = kFALSE;
   GdkPixmap *pxm = gdk_pixmap_create_from_xpm(NULL, NULL, NULL, filename);
   ret_data = 0;

   if (pxm==NULL) return kFALSE;

   HBITMAP hbm = (HBITMAP)GDK_DRAWABLE_XID(pxm);
   BITMAP bitmap;

   ret = ::GetObject(hbm, sizeof(HBITMAP), (LPVOID)&bitmap);
   ret_data = (char ***)&bitmap.bmBits;
   gdk_pixmap_unref(pxm);
   return ret;
}

//______________________________________________________________________________
void TGWin32::DeletePictureData(void *data)
{
   // Delete picture data created by the function ReadPictureDataFromFile.

   free(data);
}

//______________________________________________________________________________
void TGWin32::SetDashes(GContext_t gc, Int_t offset, const char *dash_list,
                        Int_t n)
{
   // Specify a dash pattertn. Offset defines the phase of the pattern.
   // Each element in the dash_list array specifies the length (in pixels)
   // of a segment of the pattern. N defines the length of the list.

   int i;
   gint8 dashes[32];
   for (i = 0; i < n; i++) {
      dashes[i] = (gint8) dash_list[i];
   }
   for (i = n; i < 32; i++) {
      dashes[i] = (gint8) 0;
   }

   gdk_gc_set_dashes((GdkGC *) gc, offset, dashes, n);
}

//______________________________________________________________________________
void TGWin32::MapColorStruct(ColorStruct_t * color, GdkColor & xcolor)
{
   // Map a ColorStruct_t to a XColor structure.

   xcolor.pixel = color->fPixel;
   xcolor.red = color->fRed;
   xcolor.green = color->fGreen;
   xcolor.blue = color->fBlue;
}

//______________________________________________________________________________
Bool_t TGWin32::ParseColor(Colormap_t cmap, const char *cname,
                           ColorStruct_t & color)
{
   // Parse string cname containing color name, like "green" or "#00FF00".
   // It returns a filled in ColorStruct_t. Returns kFALSE in case parsing
   // failed, kTRUE in case of success. On success, the ColorStruct_t
   // fRed, fGreen and fBlue fields are all filled in and the mask is set
   // for all three colors, but fPixel is not set.

   GdkColor xc;

   if (gdk_color_parse((char *)cname, &xc)) {
      color.fPixel = xc.pixel = RGB(xc.red, xc.green, xc.blue);
      color.fRed = xc.red;
      color.fGreen = xc.green;
      color.fBlue = xc.blue;
      return kTRUE;
   }
   return kFALSE;
}

//______________________________________________________________________________
Bool_t TGWin32::AllocColor(Colormap_t cmap, ColorStruct_t & color)
{
   // Find and allocate a color cell according to the color values specified
   // in the ColorStruct_t. If no cell could be allocated it returns kFALSE,
   // otherwise kTRUE.

   int status;
   GdkColor xc;

   xc.red = color.fRed;
   xc.green = color.fGreen;
   xc.blue = color.fBlue;

   status = gdk_colormap_alloc_color((GdkColormap *) cmap, &xc, FALSE, TRUE);
   color.fPixel = xc.pixel;

   return kTRUE;                // status != 0 ? kTRUE : kFALSE;
}

//______________________________________________________________________________
void TGWin32::QueryColor(Colormap_t cmap, ColorStruct_t & color)
{
   // Fill in the primary color components for a specific pixel value.
   // On input fPixel should be set on return the fRed, fGreen and
   // fBlue components will be set.

   GdkColor xc;
   xc.pixel = color.fPixel;

   GdkColorContext *cc = gdk_color_context_new(gdk_visual_get_system(), fColormap);
   gdk_color_context_query_color(cc, &xc);

   color.fPixel = xc.pixel;
   color.fRed = xc.red;
   color.fGreen = xc.green;
   color.fBlue = xc.blue;
}

//______________________________________________________________________________
void TGWin32::FreeColor(Colormap_t cmap, ULong_t pixel)
{
   // Free color cell with specified pixel value.

   // FIXME: to be implemented.
}

//______________________________________________________________________________
Bool_t TGWin32::CheckEvent(Window_t id, EGEventType type, Event_t & ev)
{
   // Check if there is for window "id" an event of type "type". If there
   // is fill in the event structure and return true. If no such event
   // return false.

   if (!id) return kFALSE;

   Event_t tev;
   GdkEvent xev;

   TGWin32MainThread::LockMSG();
   tev.fType = type;
   MapEvent(tev, xev, kTRUE);
   Bool_t r = gdk_check_typed_window_event((GdkWindow *) id, xev.type, &xev);

   if (r) MapEvent(ev, xev, kFALSE);
   TGWin32MainThread::UnlockMSG();

   return r ? kTRUE : kFALSE;
}

//______________________________________________________________________________
void TGWin32::SendEvent(Window_t id, Event_t * ev)
{
   // Send event ev to window id.

   if (!ev || !id) return;

   TGWin32MainThread::LockMSG();
   GdkEvent xev;
   MapEvent(*ev, xev, kTRUE);
   gdk_event_put(&xev);
   TGWin32MainThread::UnlockMSG();
}

//______________________________________________________________________________
Int_t TGWin32::EventsPending()
{
    // Returns number of pending events.

   Int_t ret;

   TGWin32MainThread::LockMSG();
   ret = (Int_t)gdk_event_queue_find_first();
   TGWin32MainThread::UnlockMSG();

   return ret;
}

//______________________________________________________________________________
void TGWin32::NextEvent(Event_t & event)
{
   // Copies first pending event from event queue to Event_t structure
   // and removes event from queue. Not all of the event fields are valid
   // for each event type, except fType and fWindow.

   TGWin32MainThread::LockMSG();
   GdkEvent *xev = gdk_event_unqueue();

   // fill in Event_t
   event.fType = kOtherEvent;   // bb add
   if (xev == NULL) {
      TGWin32MainThread::UnlockMSG();
      return;
   }
   MapEvent(event, *xev, kFALSE);
   gdk_event_free (xev);
   TGWin32MainThread::UnlockMSG();
}

//______________________________________________________________________________
void TGWin32::MapModifierState(UInt_t & state, UInt_t & xstate, Bool_t tox)
{
   // Map modifier key state to or from X.

   if (tox) {
      xstate = state;
      if (state & kAnyModifier) {
         xstate = GDK_MODIFIER_MASK;
      }
   } else {
      state = xstate;
   }
}

void _set_event_time(GdkEvent * event, UInt_t time)
{
   //

   if (event) {
      switch (event->type) {
      case GDK_MOTION_NOTIFY:
         event->motion.time = time;
      case GDK_BUTTON_PRESS:
      case GDK_2BUTTON_PRESS:
      case GDK_3BUTTON_PRESS:
      case GDK_BUTTON_RELEASE:
      case GDK_SCROLL:
         event->button.time = time;
      case GDK_KEY_PRESS:
      case GDK_KEY_RELEASE:
         event->key.time = time;
      case GDK_ENTER_NOTIFY:
      case GDK_LEAVE_NOTIFY:
         event->crossing.time = time;
      case GDK_PROPERTY_NOTIFY:
         event->property.time = time;
      case GDK_SELECTION_CLEAR:
      case GDK_SELECTION_REQUEST:
      case GDK_SELECTION_NOTIFY:
         event->selection.time = time;
      case GDK_PROXIMITY_IN:
      case GDK_PROXIMITY_OUT:
         event->proximity.time = time;
      case GDK_DRAG_ENTER:
      case GDK_DRAG_LEAVE:
      case GDK_DRAG_MOTION:
      case GDK_DRAG_STATUS:
      case GDK_DROP_START:
      case GDK_DROP_FINISHED:
         event->dnd.time = time;
      default:                 /* use current time */
         break;
      }
   }
}

//______________________________________________________________________________
void TGWin32::MapEvent(Event_t & ev, GdkEvent & xev, Bool_t tox)
{
   // Map Event_t structure to gdk_event structure. If tox is false
   // map the other way.

   if (tox) {
      // map from Event_t to gdk_event
      xev.type = GDK_NOTHING;
      if (ev.fType == kGKeyPress)
         xev.type = GDK_KEY_PRESS;
      if (ev.fType == kKeyRelease)
         xev.type = GDK_KEY_RELEASE;
      if (ev.fType == kButtonPress)
         xev.type = GDK_BUTTON_PRESS;
      if (ev.fType == kButtonRelease)
         xev.type = GDK_BUTTON_RELEASE;
      if (ev.fType == kMotionNotify)
         xev.type = GDK_MOTION_NOTIFY;
      if (ev.fType == kEnterNotify)
         xev.type = GDK_ENTER_NOTIFY;
      if (ev.fType == kLeaveNotify)
         xev.type = GDK_LEAVE_NOTIFY;
      if (ev.fType == kExpose)
         xev.type = GDK_EXPOSE;
      if (ev.fType == kConfigureNotify)
         xev.type = GDK_CONFIGURE;
      if (ev.fType == kMapNotify)
         xev.type = GDK_MAP;
      if (ev.fType == kUnmapNotify)
         xev.type = GDK_UNMAP;
      if (ev.fType == kDestroyNotify)
         xev.type = GDK_DESTROY;
      if (ev.fType == kClientMessage)
         xev.type = GDK_CLIENT_EVENT;
      if (ev.fType == kSelectionClear)
         xev.type = GDK_SELECTION_CLEAR;
      if (ev.fType == kSelectionRequest)
         xev.type = GDK_SELECTION_REQUEST;
      if (ev.fType == kSelectionNotify)
         xev.type = GDK_SELECTION_NOTIFY;

      xev.any.type = xev.type;
      xev.any.send_event = ev.fSendEvent;
      if (ev.fType == kDestroyNotify) {
         xev.any.window = (GdkWindow *) ev.fWindow;
      }
      if (ev.fType == kFocusIn) {
         xev.type = GDK_FOCUS_CHANGE;
         xev.focus_change.type = xev.type;
         xev.focus_change.window = (GdkWindow *) ev.fWindow;
         xev.focus_change.in = TRUE;
      }
      if (ev.fType == kFocusOut) {
         xev.type = GDK_FOCUS_CHANGE;
         xev.focus_change.type = xev.type;
         xev.focus_change.window = (GdkWindow *) ev.fWindow;
         xev.focus_change.in = FALSE;
      }
      if (ev.fType == kGKeyPress || ev.fType == kKeyRelease) {
         xev.key.window = (GdkWindow *) ev.fWindow;
         xev.key.type = xev.type;
         MapModifierState(ev.fState, xev.key.state, kTRUE); // key mask
         xev.key.keyval = ev.fCode; // key code
      }
      if (ev.fType == kButtonPress || ev.fType == kButtonRelease) {
         xev.button.window = (GdkWindow *) ev.fWindow;
         xev.button.type = xev.type;
         xev.button.x = ev.fX;
         xev.button.y = ev.fY;
         xev.button.x_root = ev.fXRoot;
         xev.button.y_root = ev.fYRoot;
         MapModifierState(ev.fState, xev.button.state, kTRUE); // button mask
         if (ev.fType == kButtonRelease)
            xev.button.state |= GDK_RELEASE_MASK;
         xev.button.button = ev.fCode; // button code
      }
      if (ev.fType == kSelectionNotify) {
         xev.selection.window = (GdkWindow *) ev.fUser[0];
         xev.selection.requestor = (guint32) ev.fUser[0];
         xev.selection.selection = (GdkAtom) ev.fUser[1];
         xev.selection.target = (GdkAtom) ev.fUser[2];
         xev.selection.property = (GdkAtom) ev.fUser[3];
         xev.selection.type = xev.type;
      }
      if (ev.fType == kClientMessage) {
         if ((ev.fFormat == 32) && (ev.fHandle == gWM_DELETE_WINDOW)) {
            xev.type = GDK_DELETE;
            xev.any.type = xev.type;
            xev.any.window = (GdkWindow *) ev.fWindow;
         } else {
            xev.client.window = (GdkWindow *) ev.fWindow;
            xev.client.type = xev.type;
            xev.client.message_type = (GdkAtom) ev.fHandle;
            xev.client.data_format = ev.fFormat;
            xev.client.data.l[0] = ev.fUser[0];
            if (sizeof(ev.fUser[0]) > 4) {
               SplitLong(ev.fUser[1], xev.client.data.l[1],
                         xev.client.data.l[3]);
               SplitLong(ev.fUser[2], xev.client.data.l[2],
                         xev.client.data.l[4]);
            } else {
               xev.client.data.l[1] = ev.fUser[1];
               xev.client.data.l[2] = ev.fUser[2];
               xev.client.data.l[3] = ev.fUser[3];
               xev.client.data.l[4] = ev.fUser[4];
            }
         }
      }
      if (ev.fType == kMotionNotify) {
         xev.motion.window = (GdkWindow *) ev.fWindow;
         xev.motion.type = xev.type;
         xev.motion.x = ev.fX;
         xev.motion.y = ev.fY;
         xev.motion.x_root = ev.fXRoot;
         xev.motion.y_root = ev.fYRoot;
      }
      if ((ev.fType == kEnterNotify) || (ev.fType == kLeaveNotify)) {
         xev.crossing.window = (GdkWindow *) ev.fWindow;
         xev.crossing.type = xev.type;
         xev.crossing.x = ev.fX;
         xev.crossing.y = ev.fY;
         xev.crossing.x_root = ev.fXRoot;
         xev.crossing.y_root = ev.fYRoot;
         xev.crossing.mode = (GdkCrossingMode) ev.fCode; // NotifyNormal, NotifyGrab, NotifyUngrab
         MapModifierState(ev.fState, xev.crossing.state, kTRUE);  // key or button mask
      }
      if (ev.fType == kExpose) {
         xev.expose.window = (GdkWindow *) ev.fWindow;
         xev.expose.type = xev.type;
         xev.expose.area.x = ev.fX;
         xev.expose.area.y = ev.fY;
         xev.expose.area.width = ev.fWidth;  // width and
         xev.expose.area.height = ev.fHeight;   // height of exposed area
         xev.expose.count = ev.fCount; // number of expose events still to come
      }
      if (ev.fType == kConfigureNotify) {
         xev.configure.window = (GdkWindow *) ev.fWindow;
         xev.configure.type = xev.type;
         xev.configure.x = ev.fX;
         xev.configure.y = ev.fY;
         xev.configure.width = ev.fWidth;
         xev.configure.height = ev.fHeight;
      }
      if (ev.fType == kSelectionClear) {
         xev.selection.window = (GdkWindow *) ev.fWindow;
         xev.selection.type = xev.type;
         xev.selection.selection = ev.fUser[0];
      }
      if (ev.fType == kSelectionRequest) {
         xev.selection.window = (GdkWindow *) ev.fUser[0];
         xev.selection.type = xev.type;
         xev.selection.selection = ev.fUser[1];
         xev.selection.target = ev.fUser[2];
         xev.selection.property = ev.fUser[3];
      }
      if ((ev.fType == kMapNotify) || (ev.fType == kUnmapNotify)) {
         xev.any.window = (GdkWindow *) ev.fWindow;
      }
      _set_event_time(&xev, ev.fTime);
   } else {
      // map from gdk_event to Event_t
      ev.fType = kOtherEvent;
      if (xev.type == GDK_KEY_PRESS)
         ev.fType = kGKeyPress;
      if (xev.type == GDK_KEY_RELEASE)
         ev.fType = kKeyRelease;
      if (xev.type == GDK_BUTTON_PRESS)
         ev.fType = kButtonPress;
      if (xev.type == GDK_BUTTON_RELEASE)
         ev.fType = kButtonRelease;
      if (xev.type == GDK_MOTION_NOTIFY)
         ev.fType = kMotionNotify;
      if (xev.type == GDK_ENTER_NOTIFY)
         ev.fType = kEnterNotify;
      if (xev.type == GDK_LEAVE_NOTIFY)
         ev.fType = kLeaveNotify;
      if (xev.type == GDK_EXPOSE)
         ev.fType = kExpose;
      if (xev.type == GDK_CONFIGURE)
         ev.fType = kConfigureNotify;
      if (xev.type == GDK_MAP)
         ev.fType = kMapNotify;
      if (xev.type == GDK_UNMAP)
         ev.fType = kUnmapNotify;
      if (xev.type == GDK_DESTROY)
         ev.fType = kDestroyNotify;
      if (xev.type == GDK_SELECTION_CLEAR)
         ev.fType = kSelectionClear;
      if (xev.type == GDK_SELECTION_REQUEST)
         ev.fType = kSelectionRequest;
      if (xev.type == GDK_SELECTION_NOTIFY)
         ev.fType = kSelectionNotify;

      ev.fSendEvent = kFALSE; //xev.any.send_event ? kTRUE : kFALSE;
      ev.fTime = gdk_event_get_time((GdkEvent *)&xev);

      if ((xev.type == GDK_MAP) || (xev.type == GDK_UNMAP)) {
         ev.fWindow = (Window_t) xev.any.window;
      }
      if (xev.type == GDK_DELETE) {
         ev.fWindow = (Window_t) xev.any.window;
         ev.fType = kClientMessage;
         ev.fFormat = 32;
         ev.fHandle = gWM_DELETE_WINDOW;
         ev.fUser[0] = (Long_t) gWM_DELETE_WINDOW;
         if (sizeof(ev.fUser[0]) > 4) {
            AsmLong(xev.client.data.l[1], xev.client.data.l[3],
                    ev.fUser[1]);
            AsmLong(xev.client.data.l[2], xev.client.data.l[4],
                    ev.fUser[2]);
         } else {
            ev.fUser[1] = 0; //xev.client.data.l[1];
            ev.fUser[2] = 0; //xev.client.data.l[2];
            ev.fUser[3] = 0; //xev.client.data.l[3];
            ev.fUser[4] = 0; //xev.client.data.l[4];
         }
      }
      if (xev.type == GDK_DESTROY) {
         ev.fType = kDestroyNotify;
         ev.fHandle = (Window_t) xev.any.window;   // window to be destroyed
         ev.fWindow = (Window_t) xev.any.window;
      }
      if (xev.type == GDK_FOCUS_CHANGE) {
         ev.fWindow = (Window_t) xev.focus_change.window;
         ev.fCode = kNotifyNormal;
         ev.fState = 0;
         if (xev.focus_change.in == TRUE) {
            ev.fType = kFocusIn;
         } else {
            ev.fType = kFocusOut;
         }
      }
      if (ev.fType == kGKeyPress || ev.fType == kKeyRelease) {
         ev.fWindow = (Window_t) xev.key.window;
         MapModifierState(ev.fState, xev.key.state, kFALSE);   // key mask
         ev.fCode = xev.key.keyval;  // key code
         ev.fUser[1] = xev.key.length;
         if (xev.key.length > 0) ev.fUser[2] = xev.key.string[0];
         if (xev.key.length > 1) ev.fUser[3] = xev.key.string[1];
         if (xev.key.length > 2) ev.fUser[4] = xev.key.string[2];
         HWND tmpwin = (HWND) GetWindow((HWND) GDK_DRAWABLE_XID((GdkWindow *)xev.key.window), GW_CHILD);
         if (tmpwin) {
            ev.fUser[0] = (ULong_t) gdk_xid_table_lookup((HANDLE)tmpwin);
         } else {
            ev.fUser[0] = (ULong_t) xev.key.window;
         }
      }
      if (ev.fType == kButtonPress || ev.fType == kButtonRelease) {
         ev.fWindow = (Window_t) xev.button.window;
         ev.fX = xev.button.x;
         ev.fY = xev.button.y;
         ev.fXRoot = xev.button.x_root;
         ev.fYRoot = xev.button.y_root;
         MapModifierState(ev.fState, xev.button.state, kFALSE);   // button mask
         if (ev.fType == kButtonRelease) {
            ev.fState |= kButtonReleaseMask;
         }
         ev.fCode = xev.button.button; // button code
         POINT tpoint;
         tpoint.x = xev.button.x;
         tpoint.y = xev.button.y;
         HWND tmpwin = ChildWindowFromPoint((HWND) GDK_DRAWABLE_XID((GdkWindow *)xev.button.window), tpoint);
         if (tmpwin) {
             ev.fUser[0] = (ULong_t) gdk_xid_table_lookup((HANDLE)tmpwin);
         } else {
            ev.fUser[0] = (ULong_t) 0;
         }
      }
      if (ev.fType == kMotionNotify) {
         ev.fWindow = (Window_t) xev.motion.window;
         ev.fX = xev.motion.x;
         ev.fY = xev.motion.y;
         ev.fXRoot = xev.motion.x_root;
         ev.fYRoot = xev.motion.y_root;
         MapModifierState(ev.fState, xev.motion.state, kFALSE);   // key or button mask

         POINT tpoint;
         tpoint.x = xev.button.x;
         tpoint.y = xev.button.y;
         HWND tmpwin = ChildWindowFromPoint((HWND) GDK_DRAWABLE_XID((GdkWindow *)xev.motion.window), tpoint);
         if (tmpwin) {
             ev.fUser[0] = (ULong_t)gdk_xid_table_lookup((HANDLE)tmpwin);
         } else {
            ev.fUser[0] = (ULong_t) xev.motion.window;
         }
      }
      if (ev.fType == kEnterNotify || ev.fType == kLeaveNotify) {
         ev.fWindow = (Window_t) xev.crossing.window;
         ev.fX = xev.crossing.x;
         ev.fY = xev.crossing.y;
         ev.fXRoot = xev.crossing.x_root;
         ev.fYRoot = xev.crossing.y_root;
         ev.fCode = xev.crossing.mode; // NotifyNormal, NotifyGrab, NotifyUngrab
         MapModifierState(ev.fState, xev.crossing.state, kFALSE); // key or button mask
      }
      if (ev.fType == kExpose) {
         ev.fWindow = (Window_t) xev.expose.window;
         ev.fX = xev.expose.area.x;
         ev.fY = xev.expose.area.y;
         ev.fWidth = xev.expose.area.width;  // width and
         ev.fHeight = xev.expose.area.height;   // height of exposed area
         ev.fCount = xev.expose.count; // number of expose events still to come
      }
      if (ev.fType == kConfigureNotify) {
         ev.fWindow = (Window_t) xev.configure.window;
         ev.fX = xev.configure.x;
         ev.fY = xev.configure.y;
         ev.fWidth = xev.configure.width;
         ev.fHeight = xev.configure.height;
      }
      if (xev.type == GDK_CLIENT_EVENT) {
         ev.fWindow = (Window_t) xev.client.window;
         ev.fType = kClientMessage;
         ev.fHandle = xev.client.message_type;
         ev.fFormat = xev.client.data_format;
         ev.fUser[0] = xev.client.data.l[0];
         if (sizeof(ev.fUser[0]) > 4) {
            AsmLong(xev.client.data.l[1], xev.client.data.l[3],
                    ev.fUser[1]);
            AsmLong(xev.client.data.l[2], xev.client.data.l[4],
                    ev.fUser[2]);
         } else {
            ev.fUser[1] = xev.client.data.l[1];
            ev.fUser[2] = xev.client.data.l[2];
            ev.fUser[3] = xev.client.data.l[3];
            ev.fUser[4] = xev.client.data.l[4];
         }
      }
      if (ev.fType == kSelectionClear) {
         ev.fWindow = (Window_t) xev.selection.window;
         ev.fUser[0] = xev.selection.selection;
      }
      if (ev.fType == kSelectionRequest) {
         ev.fWindow = (Window_t) xev.selection.window;
         ev.fUser[0] = (ULong_t) xev.selection.window;
         ev.fUser[1] = xev.selection.selection;
         ev.fUser[2] = xev.selection.target;
         ev.fUser[3] = xev.selection.property;
      }
      if (ev.fType == kSelectionNotify) {
         ev.fWindow = (Window_t) xev.selection.window;
         ev.fUser[0] = (ULong_t) xev.selection.window;
         ev.fUser[1] = xev.selection.selection;
         ev.fUser[2] = xev.selection.target;
         ev.fUser[3] = xev.selection.property;
      }
      if (xev.type == GDK_SCROLL) {
         ev.fType = kButtonRelease;
         if (xev.scroll.direction == GDK_SCROLL_UP) {
            ev.fCode = kButton4;
         } else if (xev.scroll.direction == GDK_SCROLL_DOWN) {
            ev.fCode = kButton5;
         }
         ev.fWindow = (Window_t) xev.scroll.window;
         ev.fX = xev.scroll.x;
         ev.fY = xev.scroll.y;
         ev.fXRoot = xev.scroll.x_root;
         ev.fYRoot = xev.scroll.y_root;
         POINT tpoint;
         tpoint.x = xev.scroll.x;
         tpoint.y = xev.scroll.y;
         HWND tmpwin = ChildWindowFromPoint((HWND) GDK_DRAWABLE_XID((GdkWindow *)xev.scroll.window), tpoint);
         if (tmpwin) {
             ev.fUser[0] = (ULong_t)gdk_xid_table_lookup((HANDLE)tmpwin);
         } else {
            ev.fUser[0] = (ULong_t) 0;
         }
      }
   }
}

//______________________________________________________________________________
void TGWin32::Bell(Int_t percent)
{
   //

   gdk_beep();
}

//______________________________________________________________________________
void TGWin32::CopyArea(Drawable_t src, Drawable_t dest, GContext_t gc,
                       Int_t src_x, Int_t src_y, UInt_t width,
                       UInt_t height, Int_t dest_x, Int_t dest_y)
{
   // Copy a drawable (i.e. pixmap) to another drawable (pixmap, window).
   // The graphics context gc will be used and the source will be copied
   // from src_x,src_y,src_x+width,src_y+height to dest_x,dest_y.

   if (!src || !dest) return;

   gdk_window_copy_area((GdkDrawable *) dest, (GdkGC *) gc, dest_x, dest_y,
                        (GdkDrawable *) src, src_x, src_y, width, height);
}

//______________________________________________________________________________
void TGWin32::ChangeWindowAttributes(Window_t id, SetWindowAttributes_t * attr)
{
   // Change window attributes.

   if (!id) return;

   GdkWMDecoration deco;
   GdkColor color;
   GdkEventMask xevent_mask;
   UInt_t xevmask;
   Mask_t evmask;
   HWND w, flag;

   if (attr) {
      color.pixel = attr->fBackgroundPixel;
      color.red = GetRValue(attr->fBackgroundPixel);
      color.green = GetGValue(attr->fBackgroundPixel);
      color.blue = GetBValue(attr->fBackgroundPixel);
   }
   if (attr && (attr->fMask & kWAEventMask)) {
      evmask = (Mask_t) attr->fEventMask;
      MapEventMask(evmask, xevmask);
      gdk_window_set_events((GdkWindow *) id, (GdkEventMask) xevmask);
   }
   if (attr && (attr->fMask & kWABackPixel)) {
      gdk_window_set_background((GdkWindow *) id, &color);
   }
//   if (attr && (attr->fMask & kWAOverrideRedirect))
//      gdk_window_set_override_redirect ((GdkWindow *) id, attr->fOverrideRedirect);
   if (attr && (attr->fMask & kWABackPixmap)) {
      gdk_window_set_back_pixmap((GdkWindow *) id,
                                 (GdkPixmap *) attr->fBackgroundPixmap, 0);
   }
   if (attr && (attr->fMask & kWACursor)) {
      gdk_window_set_cursor((GdkWindow *) id, (GdkCursor *) attr->fCursor);
   }
   if (attr && (attr->fMask & kWAColormap)) {
      gdk_window_set_colormap((GdkWindow *) id,(GdkColormap *) attr->fColormap);
   }
   if (attr && (attr->fMask & kWABorderWidth)) {
      if (attr->fBorderWidth > 0) {
         gdk_window_set_decorations((GdkWindow *) id,
                                    (GdkWMDecoration) GDK_DECOR_BORDER);
      }
   }
}

//______________________________________________________________________________
void TGWin32::ChangeProperty(Window_t id, Atom_t property, Atom_t type,
                             UChar_t * data, Int_t len)
{
   // This function alters the property for the specified window and
   // causes the X server to generate a PropertyNotify event on that
   // window.

   if (!id) return;

   gdk_property_change((GdkWindow *) id, (GdkAtom) property,
                       (GdkAtom) type, 8, GDK_PROP_MODE_REPLACE, data,len);
}

//______________________________________________________________________________
void TGWin32::DrawLine(Drawable_t id, GContext_t gc, Int_t x1, Int_t y1,
                       Int_t x2, Int_t y2)
{
   // Draw a line.

   if (!id) return;

   gdk_draw_line((GdkDrawable *) id, (GdkGC *) gc, x1, y1, x2, y2);
}

//______________________________________________________________________________
void TGWin32::ClearArea(Window_t id, Int_t x, Int_t y, UInt_t w, UInt_t h)
{
   // Clear a window area to the bakcground color.

   if (!id) return;

   gdk_window_clear_area((GdkWindow *) id, x, y, w, h);
}

//______________________________________________________________________________
void TGWin32::WMDeleteNotify(Window_t id)
{
   // Tell WM to send message when window is closed via WM.

   if (!id) return;

   Atom prop;
   prop = (Atom_t) gdk_atom_intern("WM_DELETE_WINDOW", FALSE);

   W32ChangeProperty((HWND) GDK_DRAWABLE_XID((GdkWindow *) id),
                     prop, XA_ATOM, 32, GDK_PROP_MODE_REPLACE,
                     (unsigned char *) &gWM_DELETE_WINDOW, 1);
}

//______________________________________________________________________________
void TGWin32::SetKeyAutoRepeat(Bool_t on)
{
   // Turn key auto repeat on or off.

   if (on) {
      gdk_key_repeat_restore();
    } else {
      gdk_key_repeat_disable();
   }
}

//______________________________________________________________________________
void TGWin32::GrabKey(Window_t id, Int_t keycode, UInt_t modifier, Bool_t grab)
{
   // Establish passive grab on a certain key. That is, when a certain key
   // keycode is hit while certain modifier's (Shift, Control, Meta, Alt)
   // are active then the keyboard will be grabed for window id.
   // When grab is false, ungrab the keyboard for this key and modifier.

   UInt_t xmod;

   MapModifierState(modifier, xmod);

   if (grab) {
      gdk_key_grab(keycode, (GdkEventMask)xmod, (GdkWindow *)id);
   } else {
      gdk_key_ungrab(keycode, (GdkEventMask)xmod, (GdkWindow *)id);
   }
}

//______________________________________________________________________________
void TGWin32::GrabButton(Window_t id, EMouseButton button, UInt_t modifier,
                         UInt_t evmask, Window_t confine, Cursor_t cursor,
                         Bool_t grab)
{
   // Establish passive grab on a certain mouse button. That is, when a
   // certain mouse button is hit while certain modifier's (Shift, Control,
   // Meta, Alt) are active then the mouse will be grabed for window id.
   // When grab is false, ungrab the mouse button for this button and modifier.

   UInt_t xevmask;
   UInt_t xmod;

   if (!id) return;

   MapModifierState(modifier, xmod);

   if (grab) {
      MapEventMask(evmask, xevmask);
      gdk_button_grab(button, xmod, ( GdkWindow *)id, 1,  (GdkEventMask)xevmask,
                      (GdkWindow*)confine,  (GdkCursor*)cursor);
   } else {
      gdk_button_ungrab(button, xmod, ( GdkWindow *)id);
   }
}

//______________________________________________________________________________
void TGWin32::GrabPointer(Window_t id, UInt_t evmask, Window_t confine,
                          Cursor_t cursor, Bool_t grab, Bool_t owner_events)
{
   // Establish an active pointer grab. While an active pointer grab is in
   // effect, further pointer events are only reported to the grabbing
   // client window.

   UInt_t xevmask;
   MapEventMask(evmask, xevmask);

   if (grab) {
      if(!::IsWindowVisible((HWND)GDK_DRAWABLE_XID(id))) return;
      gdk_pointer_grab((GdkWindow *) id, owner_events, (GdkEventMask) xevmask,
                       (GdkWindow *) confine, (GdkCursor *) cursor,
                       GDK_CURRENT_TIME);
   } else {
      gdk_pointer_ungrab(GDK_CURRENT_TIME);
   }
}

//______________________________________________________________________________
void TGWin32::SetWindowName(Window_t id, char *name)
{
   // Set window name.

   if (!id) return;

   gdk_window_set_title((GdkWindow *) id, name);
}

//______________________________________________________________________________
void TGWin32::SetIconName(Window_t id, char *name)
{
   // Set window icon name.

   if (!id) return;

   gdk_window_set_icon_name((GdkWindow *) id, name);
}

//______________________________________________________________________________
void TGWin32::SetIconPixmap(Window_t id, Pixmap_t pic)
{
   // Set pixmap the WM can use when the window is iconized.

   if (!id) return;

   gdk_window_set_icon((GdkWindow *)id, NULL, (GdkPixmap *)pic, (GdkPixmap *)pic);
}

#define safestrlen(s) ((s) ? strlen(s) : 0)

//______________________________________________________________________________
void TGWin32::SetClassHints(Window_t id, char *className, char *resourceName)
{
   // Set the windows class and resource name.

   if (!id) return;

   char *class_string;
   char *s;
   int len_nm, len_cl;
   GdkAtom type, prop;

   prop = gdk_atom_intern("WM_CLASS", kFALSE);

   len_nm = safestrlen(resourceName);
   len_cl = safestrlen(className);

   if ((class_string = s =
        (char *) malloc((unsigned) (len_nm + len_cl + 2)))) {
      if (len_nm) {
         strcpy(s, resourceName);
         s += len_nm + 1;
      } else
         *s++ = '\0';
      if (len_cl) {
         strcpy(s, className);
      } else {
         *s = '\0';
      }

      W32ChangeProperty((HWND) GDK_DRAWABLE_XID((GdkWindow *) id),
                            (Atom) XA_WM_CLASS, (Atom) XA_WM_CLASS, 8,
                            GDK_PROP_MODE_REPLACE,
                            (unsigned char *) class_string,
                            len_nm + len_cl + 2);
      free(class_string);
   }
}

//______________________________________________________________________________
void TGWin32::SetMWMHints(Window_t id, UInt_t value, UInt_t funcs,
                          UInt_t input)
{
   // Set decoration style for MWM-compatible wm (mwm, ncdwm, fvwm?).

   if (!id) return;

   gdk_window_set_decorations((GdkDrawable *) id, (GdkWMDecoration) value);
   gdk_window_set_functions((GdkDrawable *) id, (GdkWMFunction) funcs);
}

//______________________________________________________________________________
void TGWin32::SetWMPosition(Window_t id, Int_t x, Int_t y)
{
   //

   if (!id) return;

   gdk_window_move((GdkDrawable *) id, x, y);
}

//______________________________________________________________________________
void TGWin32::SetWMSize(Window_t id, UInt_t w, UInt_t h)
{
   //

   if (!id) return;

   gdk_window_resize((GdkWindow *) id, w, h);
}

//______________________________________________________________________________
void TGWin32::SetWMSizeHints(Window_t id, UInt_t wmin, UInt_t hmin,
                             UInt_t wmax, UInt_t hmax,
                             UInt_t winc, UInt_t hinc)
{
   // Give the window manager minimum and maximum size hints. Also
   // specify via winc and hinc the resize increments.

   if (!id) return;

   GdkGeometry hints;
   GdkWindowHints flags;

   flags = (GdkWindowHints) (GDK_HINT_MIN_SIZE | GDK_HINT_MAX_SIZE |
                             GDK_HINT_RESIZE_INC);
   hints.min_width = (Int_t) wmin;
   hints.max_width = (Int_t) wmax;
   hints.min_height = (Int_t) hmin;
   hints.max_height = (Int_t) hmax;
   hints.width_inc = (Int_t) winc;
   hints.height_inc = (Int_t) hinc;

   gdk_window_set_geometry_hints((GdkWindow *) id, (GdkGeometry *) &hints,
                                 (GdkWindowHints) flags);
}

//______________________________________________________________________________
void TGWin32::SetWMState(Window_t id, EInitialState state)
{
   // Set the initial state of the window. Either kNormalState or kIconicState.

   if (!id) return;

#if 0
   XWMHints hints;
   Int_t xstate = NormalState;

   if (state == kNormalState)
      xstate = NormalState;
   if (state == kIconicState)
      xstate = IconicState;

   hints.flags = StateHint;
   hints.initial_state = xstate;

   XSetWMHints((GdkWindow *) id, &hints);
#endif
}

//______________________________________________________________________________
void TGWin32::SetWMTransientHint(Window_t id, Window_t main_id)
{
   // Tell window manager that window is a transient window of gdk_parent_root.

   if (!id) return;

   gdk_window_set_transient_for((GdkWindow *) id, (GdkWindow *) main_id);
}

//______________________________________________________________________________
void TGWin32::DrawString(Drawable_t id, GContext_t gc, Int_t x, Int_t y,
                         const char *s, Int_t len)
{
   // Draw a string using a specific graphics context in position (x,y).

   if (!id) return;

   GdkGCValues values;
   gdk_gc_get_values((GdkGC *) gc, &values);
   gdk_win32_draw_text((GdkDrawable *) id, (GdkFont *) values.font,
                 (GdkGC *) gc, x, y, (const gchar *)s, len);
}

//______________________________________________________________________________
Int_t TGWin32::TextWidth(FontStruct_t font, const char *s, Int_t len)
{
   // Return lenght of string in pixels. Size depends on font.

   return gdk_text_width((GdkFont *)font, s, len);
}

//______________________________________________________________________________
void TGWin32::GetFontProperties(FontStruct_t font, Int_t & max_ascent,
                                Int_t & max_descent)
{
   // Return some font properties.

   GdkFont *f = (GdkFont *) font;
   max_ascent = f->ascent;
   max_descent = f->descent;
}

//______________________________________________________________________________
void TGWin32::GetGCValues(GContext_t gc, GCValues_t & gval)
{
   // Get current values from graphics context gc. Which values of the
   // context to get is encoded in the GCValues::fMask member.

   GdkGCValues xgval;
   ULong_t xmask;

   MapGCValues(gval, xmask, xgval, kTRUE);
   gdk_gc_get_values((GdkGC *) gc, &xgval);
   MapGCValues(gval, xmask, xgval, kFALSE);
}

//______________________________________________________________________________
FontStruct_t TGWin32::GetFontStruct(FontH_t fh)
{
   // Retrieve associated font structure once we have the font handle.
   // Free returned FontStruct_t using FreeFontStruct().

   return (FontStruct_t) gdk_font_ref((GdkFont *) fh);
}

//______________________________________________________________________________
void TGWin32::FreeFontStruct(FontStruct_t fs)
{
   // Free font structure returned by GetFontStruct().

   gdk_font_unref((GdkFont *) fs);
}

//______________________________________________________________________________
void TGWin32::ClearWindow(Window_t id)
{
   // Clear window.

   if (!id) return;

   gdk_window_clear((GdkDrawable *) id);
}

//______________________________________________________________________________
Int_t TGWin32::KeysymToKeycode(UInt_t keysym)
{
   // Convert a keysym to the appropriate keycode. For example keysym is
   // a letter and keycode is the matching keyboard key (which is dependend
   // on the current keyboard mapping).

   UInt_t xkeysym;
   MapKeySym(keysym, xkeysym);
   return xkeysym;
}

//______________________________________________________________________________
void TGWin32::FillRectangle(Drawable_t id, GContext_t gc, Int_t x, Int_t y,
                            UInt_t w, UInt_t h)
{
   // Draw a filled rectangle. Filling is done according to the gc.

   if (!id) return;

   gdk_win32_draw_rectangle((GdkDrawable *) id, (GdkGC *) gc, kTRUE, x, y, w, h);
}

//______________________________________________________________________________
void TGWin32::DrawRectangle(Drawable_t id, GContext_t gc, Int_t x, Int_t y,
                            UInt_t w, UInt_t h)
{
   // Draw a rectangle outline.

   if (!id) return;

   gdk_win32_draw_rectangle((GdkDrawable *) id, (GdkGC *) gc, kFALSE, x, y, w, h);
}

//______________________________________________________________________________
void TGWin32::DrawSegments(Drawable_t id, GContext_t gc, Segment_t * seg,
                           Int_t nseg)
{
   // Draws multiple line segments. Each line is specified by a pair of points.

   if (!id) return;

   gdk_win32_draw_segments((GdkDrawable *) id, (GdkGC *) gc, (GdkSegment *)seg, nseg);
}

//______________________________________________________________________________
void TGWin32::SelectInput(Window_t id, UInt_t evmask)
{
   // Defines which input events the window is interested in. By default
   // events are propageted up the window stack. This mask can also be
   // set at window creation time via the SetWindowAttributes_t::fEventMask
   // attribute.

   if (!id) return;

   UInt_t xevmask;
   MapEventMask(evmask, xevmask, kTRUE);
   gdk_window_set_events((GdkWindow *) id, (GdkEventMask)xevmask);
}

//______________________________________________________________________________
Window_t TGWin32::GetInputFocus()
{
   // Returns the window id of the window having the input focus.

   HWND hwnd = ::GetFocus();
   return (Window_t) gdk_xid_table_lookup(hwnd);
}

//______________________________________________________________________________
void TGWin32::SetInputFocus(Window_t id)
{
   // Set keyboard input focus to window id.

   if (!id) return;

   HWND hwnd = (HWND)GDK_DRAWABLE_XID((GdkWindow *)id);
   ::SetFocus(hwnd);
}

//______________________________________________________________________________
Window_t TGWin32::GetPrimarySelectionOwner()
{
   // Returns the window id of the current owner of the primary selection.
   // That is the window in which, for example some text is selected.

   return (Window_t)gdk_selection_owner_get(gClipboardAtom);
}

//______________________________________________________________________________
void TGWin32::SetPrimarySelectionOwner(Window_t id)
{
   // Makes the window id the current owner of the primary selection.
   // That is the window in which, for example some text is selected.

   if (!id) return;

   gdk_selection_owner_set((GdkWindow *) id, gClipboardAtom, GDK_CURRENT_TIME, 0);
}

//______________________________________________________________________________
void TGWin32::ConvertPrimarySelection(Window_t id, Atom_t clipboard, Time_t when)
{
   // XConvertSelection() causes a SelectionRequest event to be sent to the
   // current primary selection owner. This event specifies the selection
   // property (primary selection), the format into which to convert that
   // data before storing it (target = XA_STRING), the property in which
   // the owner will place the information (sel_property), the window that
   // wants the information (id), and the time of the conversion request
   // (when).
   // The selection owner responds by sending a SelectionNotify event, which
   // confirms the selected atom and type.

   if (!id) return;

   gdk_selection_convert((GdkWindow *) id, clipboard,
                         gdk_atom_intern("GDK_TARGET_STRING", 0), when);
}

//______________________________________________________________________________
void TGWin32::LookupString(Event_t * event, char *buf, Int_t buflen,
                           UInt_t & keysym)
{
   // Convert the keycode from the event structure to a key symbol (according
   // to the modifiers specified in the event structure and the current
   // keyboard mapping). In buf a null terminated ASCII string is returned
   // representing the string that is currently mapped to the key code.

   KeySym xkeysym;
   _lookup_string(event, buf, buflen);
   UInt_t ks, xks = (UInt_t) event->fCode;
   MapKeySym(ks, xks, kFALSE);
   keysym = (Int_t) ks;
}

//______________________________________________________________________________
void TGWin32::MapKeySym(UInt_t & keysym, UInt_t & xkeysym, Bool_t tox)
{
   // Map to and from X key symbols. Keysym are the values returned by
   // XLookUpString.

   if (tox) {
      xkeysym = GDK_VoidSymbol;
      if (keysym < 127) {
         xkeysym = keysym;
      } else if (keysym >= kKey_F1 && keysym <= kKey_F35) {
         xkeysym = GDK_F1 + (keysym - (UInt_t) kKey_F1);	// function keys
      } else {
         for (int i = 0; gKeyMap[i].fKeySym; i++) {	// any other keys
            if (keysym == (UInt_t) gKeyMap[i].fKeySym) {
               xkeysym = (UInt_t) gKeyMap[i].fXKeySym;
               break;
            }
         }
      }
   } else {
      keysym = kKey_Unknown;
      // commentary in X11/keysymdef says that X codes match ASCII
      if (xkeysym < 127) {
         keysym = xkeysym;
      } else if (xkeysym >= GDK_F1 && xkeysym <= GDK_F35) {
         keysym = kKey_F1 + (xkeysym - GDK_F1);	// function keys
      } else if (xkeysym >= GDK_KP_0 && xkeysym <= GDK_KP_9) {
         keysym = kKey_0 + (xkeysym - GDK_KP_0);	// numeric keypad keys
      } else {
         for (int i = 0; gKeyMap[i].fXKeySym; i++) {	// any other keys
            if (xkeysym == gKeyMap[i].fXKeySym) {
               keysym = (UInt_t) gKeyMap[i].fKeySym;
               break;
            }
         }
      }
   }
}

//______________________________________________________________________________
void TGWin32::GetPasteBuffer(Window_t id, Atom_t atom, TString & text,
                             Int_t & nchar, Bool_t del)
{
   // Get contents of paste buffer atom into string. If del is true delete
   // the paste buffer afterwards.

   if (!id) return;

   char *data;
   int nread, actual_format;

   nread = gdk_selection_property_get((GdkWindow *) id,
                                      (unsigned char **) &data,
                                      (GdkAtom *) & atom, &actual_format);

   if ((nread == 0) || (data == NULL)) {
      nchar = 0;
      return;
   }

   text.Insert(0, (const char *) data);
   nchar = 1;                   //strlen(data);
   g_free(data);

   if (del) {
      gdk_property_delete((GdkWindow *) id,
                          gdk_atom_intern("GDK_SELECTION", FALSE));
   }
}

//______________________________________________________________________________
void TGWin32::TranslateCoordinates(Window_t src, Window_t dest,
                                   Int_t src_x, Int_t src_y,
                                   Int_t &dest_x, Int_t &dest_y,
                                   Window_t &child)
{
   // TranslateCoordinates translates coordinates from the frame of
   // reference of one window to another. If the point is contained
   // in a mapped child of the destination, the id of that child is
   // returned as well.

   if (!src || !dest) return;

   HWND sw, dw, ch = NULL;
   POINT point;
   sw = (HWND)GDK_DRAWABLE_XID((GdkWindow *)src);
   dw = (HWND)GDK_DRAWABLE_XID((GdkWindow *)dest);
   point.x = src_x;
   point.y = src_y;
   ::MapWindowPoints(sw,        // handle of window to be mapped from
                   dw,          // handle to window to be mapped to
                   &point,      // pointer to array with points to map
                   1);          // number of structures in array
   ch = ::ChildWindowFromPointEx(dw, point, CWP_SKIPDISABLED | CWP_SKIPINVISIBLE);
   child = (Window_t)gdk_xid_table_lookup(ch);

   if (child == src) {
      child = (Window_t) 0;
   }
   dest_x = point.x;
   dest_y = point.y;
}

//______________________________________________________________________________
void TGWin32::GetWindowSize(Drawable_t id, Int_t & x, Int_t & y,
                            UInt_t & w, UInt_t & h)
{
   // Return geometry of window (should be called GetGeometry but signature
   // already used).

   if (!id) return;

   Int_t ddum;
   if (GDK_DRAWABLE_TYPE(id) == GDK_DRAWABLE_PIXMAP) {
      x = y = 0;
      gdk_drawable_get_size((GdkDrawable *)id, (int*)&w, (int*)&h);
   }
   else {
      gdk_window_get_geometry((GdkDrawable *) id, &x, &y, (int*)&w, 
                              (int*)&h, &ddum);
   }
}

//______________________________________________________________________________
void TGWin32::FillPolygon(Window_t id, GContext_t gc, Point_t * points,
                          Int_t npnt)
{
   // FillPolygon fills the region closed by the specified path.
   // The path is closed automatically if the last point in the list does
   // not coincide with the first point. All point coordinates are
   // treated as relative to the origin. For every pair of points
   // inside the polygon, the line segment connecting them does not
   // intersect the path.

   if (!id) return;

   gdk_win32_draw_polygon((GdkWindow *) id, (GdkGC *) gc, 1, (GdkPoint *) points, npnt);
}

//______________________________________________________________________________
void TGWin32::QueryPointer(Window_t id, Window_t &rootw,
                           Window_t &childw, Int_t &root_x,
                           Int_t &root_y, Int_t &win_x, Int_t &win_y,
                           UInt_t &mask)
{
   // Returns the root window the pointer is logically on and the pointer
   // coordinates relative to the root window's origin.
   // The pointer coordinates returned to win_x and win_y are relative to
   // the origin of the specified window. In this case, QueryPointer returns
   // the child that contains the pointer, if any, or else kNone to
   // childw. QueryPointer returns the current logical state of the
   // keyboard buttons and the modifier keys in mask.

   if (!id) return;

   POINT mousePt, sPt, currPt;
   HWND chw, window;
   UInt_t umask = 0;
   BYTE kbd[256];

   window = (HWND)GDK_DRAWABLE_XID((GdkWindow *)id);
   rootw = (Window_t)GDK_ROOT_PARENT();
   ::GetCursorPos(&currPt);
   chw = ::WindowFromPoint(currPt);
   childw = (Window_t)gdk_xid_table_lookup(chw);
   root_x = currPt.x;
   root_y = currPt.y;

   ::ClientToScreen(window, &currPt);
   win_x = currPt.x;
   win_y = currPt.y;

   ::GetKeyboardState (kbd);

   if (kbd[VK_SHIFT] & 0x80) {
      umask |= GDK_SHIFT_MASK;
   }
   if (kbd[VK_CAPITAL] & 0x80) {
      umask |= GDK_LOCK_MASK;
   }
   if (kbd[VK_CONTROL] & 0x80) {
      umask |= GDK_CONTROL_MASK;
   }
   if (kbd[VK_MENU] & 0x80) {
      umask |= GDK_MOD1_MASK;
   }
   if (kbd[VK_LBUTTON] & 0x80) {
      umask |= GDK_BUTTON1_MASK;
   }
   if (kbd[VK_MBUTTON] & 0x80) {
      umask |= GDK_BUTTON2_MASK;
   }
   if (kbd[VK_RBUTTON] & 0x80) {
      umask |= GDK_BUTTON3_MASK;
   }

   MapModifierState(mask, umask, kFALSE);
}

//______________________________________________________________________________
void TGWin32::SetForeground(GContext_t gc, ULong_t foreground)
{
   // Set foreground color in graphics context (shortcut for ChangeGC with
   // only foreground mask set).

   GdkColor fore;
   fore.pixel = foreground;
   fore.red = GetRValue(foreground);
   fore.green = GetGValue(foreground);
   fore.blue = GetBValue(foreground);
   gdk_gc_set_foreground((GdkGC *) gc, &fore);
}

//______________________________________________________________________________
void TGWin32::SetClipRectangles(GContext_t gc, Int_t x, Int_t y,
                                Rectangle_t * recs, Int_t n)
{
   // Set clipping rectangles in graphics context. X, Y specify the origin
   // of the rectangles. Recs specifies an array of rectangles that define
   // the clipping mask and n is the number of rectangles.

   Int_t i;
   GdkRectangle *grects = new GdkRectangle[n];

   for (i = 0; i < n; i++) {
      grects[i].x = x+recs[i].fX;
      grects[i].y = y+recs[i].fY;
      grects[i].width = recs[i].fWidth;
      grects[i].height = recs[i].fHeight;
   }

   for (i = 0; i < n; i++) {
      gdk_gc_set_clip_rectangle((GdkGC *)gc, (GdkRectangle*)recs);
   }
   delete [] grects;
}

//______________________________________________________________________________
void TGWin32::Update(Int_t mode)
{
   // Flush (mode = 0, default) or synchronize (mode = 1) X output buffer.
   // Flush flushes output buffer. Sync flushes buffer and waits till all
   // requests have been processed by X server.

   GdiFlush();
}

//______________________________________________________________________________
Region_t TGWin32::CreateRegion()
{
   // Create a new empty region.

   return (Region_t) gdk_region_new();
}

//______________________________________________________________________________
void TGWin32::DestroyRegion(Region_t reg)
{
   // Destroy region.

   gdk_region_destroy((GdkRegion *) reg);
}

//______________________________________________________________________________
void TGWin32::UnionRectWithRegion(Rectangle_t * rect, Region_t src, Region_t dest)
{
   // Union of rectangle with a region.

   GdkRectangle r;
   r.x = rect->fX;
   r.y = rect->fY;
   r.width = rect->fWidth;
   r.height = rect->fHeight;
   dest = (Region_t) gdk_region_union_with_rect((GdkRegion *) src, &r);
}

//______________________________________________________________________________
Region_t TGWin32::PolygonRegion(Point_t * points, Int_t np, Bool_t winding)
{
   // Create region for the polygon defined by the points array.
   // If winding is true use WindingRule else EvenOddRule as fill rule.

   return (Region_t) gdk_region_polygon((GdkPoint*)points, np,
                                 winding ? GDK_WINDING_RULE : GDK_EVEN_ODD_RULE);
}

//______________________________________________________________________________
void TGWin32::UnionRegion(Region_t rega, Region_t regb, Region_t result)
{
   // Compute the union of rega and regb and return result region.
   // The output region may be the same result region.

   result = (Region_t) gdk_regions_union((GdkRegion *) rega, (GdkRegion *) regb);
}

//______________________________________________________________________________
void TGWin32::IntersectRegion(Region_t rega, Region_t regb,
                              Region_t result)
{
   // Compute the intersection of rega and regb and return result region.
   // The output region may be the same as the result region.

   result = (Region_t) gdk_regions_intersect((GdkRegion *) rega,(GdkRegion *) regb);
}

//______________________________________________________________________________
void TGWin32::SubtractRegion(Region_t rega, Region_t regb, Region_t result)
{
   // Subtract rega from regb.

   result = (Region_t)gdk_regions_subtract((GdkRegion *) rega,(GdkRegion *) regb);
}

//______________________________________________________________________________
void TGWin32::XorRegion(Region_t rega, Region_t regb, Region_t result)
{
   // Calculate the difference between the union and intersection of
   // two regions.

   result = (Region_t) gdk_regions_xor((GdkRegion *) rega, (GdkRegion *) regb);
}

//______________________________________________________________________________
Bool_t TGWin32::EmptyRegion(Region_t reg)
{
   // Return true if the region is empty.

   return (Bool_t) gdk_region_empty((GdkRegion *) reg);
}

//______________________________________________________________________________
Bool_t TGWin32::PointInRegion(Int_t x, Int_t y, Region_t reg)
{
   // Returns true if the point x,y is in the region.

   return (Bool_t) gdk_region_point_in((GdkRegion *) reg, x, y);
}

//______________________________________________________________________________
Bool_t TGWin32::EqualRegion(Region_t rega, Region_t regb)
{
   // Returns true if two regions are equal.

   return (Bool_t) gdk_region_equal((GdkRegion *) rega, (GdkRegion *) regb);
}

//______________________________________________________________________________
void TGWin32::GetRegionBox(Region_t reg, Rectangle_t * rect)
{
   // Return smallest enclosing rectangle.

   GdkRectangle r;
   gdk_region_get_clipbox((GdkRegion *) reg, &r);
   rect->fX = r.x;
   rect->fY = r.y;
   rect->fWidth = r.width;
   rect->fHeight = r.height;
}

//______________________________________________________________________________
char **TGWin32::ListFonts(const char *fontname, Int_t /*max*/, Int_t &count)
{
   // Return list of font names matching "fontname".

   char  foundry[32], family[100], weight[32], slant[32], font_name[256];
   char  **fontlist;
   Int_t n1, fontcount = 0;

   sscanf(fontname, "-%30[^-]-%100[^-]-%30[^-]-%30[^-]-%n",
          foundry, family, weight, slant, &n1);
   // replace "medium" by "normal"
   if(!stricmp(weight,"medium")) {
      sprintf(weight,"normal");
   }
   // since all sizes are allowed with TTF, just forget it...
   sprintf(font_name, "-%s-%s-%s-%s-*", foundry, family, weight, slant);
   fontlist = gdk_font_list_new(font_name, &fontcount);
   count = fontcount;

   if (fontcount > 0) return fontlist;
   return 0;
}

//______________________________________________________________________________
void TGWin32::FreeFontNames(char **fontlist)
{
   //

   gdk_font_list_free(fontlist);
}

//______________________________________________________________________________
Drawable_t TGWin32::CreateImage(UInt_t width, UInt_t height)
{
   //

   return (Drawable_t) gdk_image_new(GDK_IMAGE_SHARED, gdk_visual_get_best(),
                                     width, height);
}

//______________________________________________________________________________
void TGWin32::GetImageSize(Drawable_t id, UInt_t &width, UInt_t &height)
{
   //

   width  = ((GdkImage*)id)->width;
   height = ((GdkImage*)id)->height;
}

//______________________________________________________________________________
void TGWin32::PutPixel(Drawable_t id, Int_t x, Int_t y, ULong_t pixel)
{
   //

   if (!id) return;

   GdkImage *image = (GdkImage *)id;
   if (image->depth == 1) {
      if (pixel & 1) {
         ((UChar_t *) image->mem)[y * image->bpl + (x >> 3)] |= (1 << (7 - (x & 0x7)));
      } else {
         ((UChar_t *) image->mem)[y * image->bpl + (x >> 3)] &= ~(1 << (7 - (x & 0x7)));
      }
   } else {
      UChar_t *pixelp = (UChar_t *) image->mem + y * image->bpl + x * image->bpp;
      // Windows is always LSB, no need to check image->byte_order.
      switch (image->bpp) {
         case 4:
            pixelp[3] = 0;
         case 3:
            pixelp[2] = ((pixel >> 16) & 0xFF);
         case 2:
            pixelp[1] = ((pixel >> 8) & 0xFF);
         case 1:
            pixelp[0] = (pixel & 0xFF);
      }
   }
}

//______________________________________________________________________________
void TGWin32::PutImage(Drawable_t id, GContext_t gc, Drawable_t img, Int_t dx,
                       Int_t dy, Int_t x, Int_t y, UInt_t w, UInt_t h)
{
   //

   if (!id) return;

   gdk_draw_image((GdkDrawable *) id, (GdkGC *)gc, (GdkImage *)img,
                  x, y, dx, dy, w, h);
   ::GdiFlush();
}

//______________________________________________________________________________
void TGWin32::DeleteImage(Drawable_t img)
{
   //

   gdk_image_unref((GdkImage *)img);
}

//______________________________________________________________________________
void TGWin32::DrawDIB(ULong_t bmi, ULong_t bmbits, Int_t xpos, Int_t ypos)
{
   // Draws DIB bitmap (added for libAfterImage on Win32)
   // bmi        - pointer on bitmap info structure
   // bmbits     - pointer on bitmap bits array
   // xpos, ypos - position of bitmap

   HDC hdc;
   int saved_hdc;
   BITMAPINFO *lpbmi = (BITMAPINFO *)bmi;
   HWND hwnd = (HWND) GDK_DRAWABLE_XID(gCws->drawing);
   if (GDK_DRAWABLE_TYPE(gCws->drawing) == GDK_DRAWABLE_PIXMAP) {
      hdc = ::CreateCompatibleDC(NULL);
      saved_hdc = ::SaveDC(hdc);
      ::SelectObject(hdc, hwnd);
   }
   else {
      hdc = ::GetDC(hwnd);
      saved_hdc = ::SaveDC(hdc);
   }
   ::StretchDIBits( hdc, xpos, ypos, lpbmi->bmiHeader.biWidth, lpbmi->bmiHeader.biHeight, 
                  0, 0, lpbmi->bmiHeader.biWidth, lpbmi->bmiHeader.biHeight,  
                  (void *)bmbits, lpbmi, DIB_RGB_COLORS, SRCCOPY );
   ::RestoreDC(hdc, saved_hdc);
   if (GDK_DRAWABLE_TYPE(gCws->drawing) == GDK_DRAWABLE_PIXMAP) {
      ::DeleteDC(hdc);
   } else {
      ::ReleaseDC(hwnd, hdc);
   }
}

//______________________________________________________________________________
unsigned char *TGWin32::GetBmBits(Drawable_t wid, Int_t width, Int_t height)
{
   // Gets DIB bits (added for libAfterImage on Win32)
   // width, height - position of bitmap
   // returns a pointer on bitmap bits array
   
   HDC hdc, memdc;
   BITMAPINFO bmi;
   HGDIOBJ oldbitmap1, oldbitmap2;
   BITMAP bm;
   HBITMAP ximage;
   VOID  *bmbits;

   if (GDK_DRAWABLE_TYPE(wid) == GDK_DRAWABLE_PIXMAP) {
      hdc = ::CreateCompatibleDC(NULL);
      oldbitmap1 = ::SelectObject(hdc, GDK_DRAWABLE_XID(wid));
      ::GetObject(GDK_DRAWABLE_XID(wid), sizeof(BITMAP), &bm);
   } else {
      hdc = ::GetDC((HWND)GDK_DRAWABLE_XID(wid));
   }

   memdc = ::CreateCompatibleDC(hdc);

   bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
   bmi.bmiHeader.biWidth = width;
   bmi.bmiHeader.biHeight = -height;
   bmi.bmiHeader.biPlanes = 1;
   bmi.bmiHeader.biBitCount = 32;
   bmi.bmiHeader.biCompression = BI_RGB;
   bmi.bmiHeader.biSizeImage = 0;
   bmi.bmiHeader.biXPelsPerMeter = bmi.bmiHeader.biYPelsPerMeter = 0;
   bmi.bmiHeader.biClrUsed = 0;
   bmi.bmiHeader.biClrImportant = 0;

   ximage = ::CreateDIBSection(hdc, (BITMAPINFO *) & bmi, DIB_RGB_COLORS, &bmbits, NULL, 0);

   oldbitmap2 = ::SelectObject(memdc, ximage);

   ::BitBlt(memdc, 0, 0, width, height, hdc, 0, 0, SRCCOPY);
   ::SelectObject(memdc, oldbitmap2);
   ::DeleteDC(memdc);
   if (GDK_DRAWABLE_TYPE(wid) == GDK_DRAWABLE_PIXMAP) {
      ::SelectObject(hdc, oldbitmap1);
      ::DeleteDC(hdc);
   } else {
      ::ReleaseDC((HWND)GDK_DRAWABLE_XID(wid), hdc);
   }
   return (unsigned char *)bmbits;
}

//______________________________________________________________________________
Pixmap_t TGWin32::DIB2Pixmap(ULong_t bmi, ULong_t bmbits)
{
   // Converts a DIB (Device Independant Bitmap) into 
   // a GdkPixmap (added for libAfterImage on Win32)
   // bmi        - pointer on bitmap info structure
   // bmbits     - pointer on bitmap bits array
   // returns Pixmap_t (pointer on a GdkPixmap)
   GdkPixmap *pixmap = 0;
   void *pbmbits;
   BITMAPINFO *lpbmi = (BITMAPINFO *)bmi;
   SIZE size;

   HDC hdc = ::GetDC( NULL );

   HBITMAP bitmap = ::CreateDIBitmap(hdc, &lpbmi->bmiHeader, CBM_INIT,
                                (void *)bmbits, lpbmi, DIB_RGB_COLORS);
   ::ReleaseDC(NULL, hdc);

   // For an obscure reason, we have to set the size of the 
   // bitmap this way before to call gdk_pixmap_foreign_new
   // otherwise, it fails...
   ::SetBitmapDimensionEx(bitmap, lpbmi->bmiHeader.biWidth, 
                          lpbmi->bmiHeader.biHeight, &size);

   pixmap = gdk_pixmap_foreign_new((guint32)bitmap);

   return (Pixmap_t) pixmap;
}



