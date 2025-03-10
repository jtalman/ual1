// @(#)root/g3d:$Name:  $:$Id: TTRD2.cxx,v 1.4 2005/03/09 18:19:26 brun Exp $
// Author: Nenad Buncic   13/09/95

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include "TTRD2.h"
#include "TNode.h"

ClassImp(TTRD2)

//______________________________________________________________________________
// Begin_Html <P ALIGN=CENTER> <IMG SRC="gif/trd2.gif"> </P> End_Html
// TRD2 is a trapezoid with both x and y dimensions varying along z.
// It has 8 parameters:
//
//     - name       name of the shape
//     - title      shape's title
//     - material  (see TMaterial)
//     - dx1        half-length along x at the z surface positioned at -DZ
//     - dx2        half-length along x at the z surface positioned at +DZ
//     - dy1        half-length along y at the z surface positioned at -DZ
//     - dy2        half-length along y at the z surface positioned at +DZ
//     - dz         half-length along the z-axis


//______________________________________________________________________________
TTRD2::TTRD2()
{
  // TRD2 shape default constructor
}


//______________________________________________________________________________
TTRD2::TTRD2(const char *name, const char *title, const char *material, Float_t dx1, Float_t dx2, Float_t dy1,
       Float_t dy2, Float_t dz) : TBRIK(name, title,material,dx1,dy1,dz)
{
   // TRD2 shape normal constructor

   fDx2 = dx2;
   fDy2 = dy2;
}


//______________________________________________________________________________
TTRD2::~TTRD2()
{
   // TRD2 shape default destructor
}


//______________________________________________________________________________
void TTRD2::SetPoints(Double_t *points) const
{
   // Create TRD2 points

   Float_t dx1, dx2, dy1, dy2, dz;

   dx1 = TBRIK::fDx;
   dx2 = fDx2;
   dy1 = TBRIK::fDy;
   dy2 = fDy2;
   dz  = TBRIK::fDz;

   if (points) {
      points[ 0] = -dx1 ; points[ 1] = -dy1 ; points[ 2] = -dz;
      points[ 3] = -dx1 ; points[ 4] =  dy1 ; points[ 5] = -dz;
      points[ 6] =  dx1 ; points[ 7] =  dy1 ; points[ 8] = -dz;
      points[ 9] =  dx1 ; points[10] = -dy1 ; points[11] = -dz;
      points[12] = -dx2 ; points[13] = -dy2 ; points[14] =  dz;
      points[15] = -dx2 ; points[16] =  dy2 ; points[17] =  dz;
      points[18] =  dx2 ; points[19] =  dy2 ; points[20] =  dz;
      points[21] =  dx2 ; points[22] = -dy2 ; points[23] =  dz;
   }
}
