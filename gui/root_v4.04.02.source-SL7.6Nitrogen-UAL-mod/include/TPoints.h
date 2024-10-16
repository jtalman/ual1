// @(#)root/graf:$Name:  $:$Id: TPoints.h,v 1.2 2000/06/13 11:22:28 brun Exp $
// Author: Rene Brun   23/02/95

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TPoints
#define ROOT_TPoints


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TPoints                                                              //
//                                                                      //
// 2-D graphics point (world coordinates, i.e. floating point).         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_Gtypes
#include "Gtypes.h"
#endif


class TPoints {

private:
   Double_t    fX;           //X world coordinate
   Double_t    fY;           //Y world coordinate

public:
   TPoints() : fX(0), fY(0) { }
   TPoints(Double_t xy) : fX(xy), fY(xy) { }
   TPoints(Double_t x, Double_t y) : fX(x), fY(y) { }
   virtual ~TPoints() { }
   Double_t   GetX() const { return fX; }
   Double_t   GetY() const { return fY; }
   void       SetX(Double_t x) { fX = x; }
   void       SetY(Double_t y) { fY = y; }

   ClassDef(TPoints,0)  //2-D graphics point
};

#endif
