/* @(#)root/base:$Name:  $:$Id: TPoint.h,v 1.4 2002/05/09 20:21:59 brun Exp $ */

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TPoint
#define ROOT_TPoint


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TPoint                                                               //
//                                                                      //
// TPoint implements a 2D screen (device) point (see also TPoints).     //
//                                                                      //
// Don't add in dictionary since that will add a virtual table pointer  //
// and that will destroy the data layout of an array of TPoint's which  //
// should match the layout of an array of XPoint's (so no extra copying //
// needs to be done in the X11 drawing routines).                       //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_Gtypes
#include "Gtypes.h"
#endif


class TPoint {

public:    // for easy access
#if !defined(WIN32) && !defined(G__WIN32)
   SCoord_t    fX;         //X device coordinate
   SCoord_t    fY;         //Y device coordinate
#else
   long        fX;         //X device coordinate
   long        fY;         //Y device coordinate
#endif

public:
   TPoint() : fX(0), fY(0) { }
   TPoint(SCoord_t xy) : fX(xy), fY(xy) { }
   TPoint(SCoord_t x, SCoord_t y) : fX(x), fY(y) { }
   ~TPoint() { }
   SCoord_t    GetX() const { return (SCoord_t)fX; }
   SCoord_t    GetY() const { return (SCoord_t)fY; }
   void        SetX(SCoord_t x) { fX = x; }
   void        SetY(SCoord_t y) { fY = y; }
};

#endif
