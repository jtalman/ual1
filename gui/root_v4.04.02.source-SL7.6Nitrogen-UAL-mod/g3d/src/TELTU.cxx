// @(#)root/g3d:$Name:  $:$Id: TELTU.cxx,v 1.2 2002/11/11 11:21:16 brun Exp $
// Author: Rene Brun   26/06/97

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include "TELTU.h"
#include "TNode.h"

ClassImp(TELTU)

//______________________________________________________________________________
// Begin_Html <P ALIGN=CENTER> <IMG SRC="gif/ELTU.gif"> </P> End_Html
// 'ELTU' is a  cylinder with  an elliptical  section.  It  has three
//        parameters:  the  ellipse  semi-axis   in  X,  the  ellipse
//        semi-axis in Y  and the half length in Z.   The equation of
//        the conical curve is:
//             X**2/fRx**2  +  Y**2/fRy**2  =  1
//        ELTU is not divisible.
//
//     - name       name of the shape
//     - title      shape's title
//     - material  (see TMaterial)
//     - rx         the  ellipse  semi-axis   in  X
//     - ry         the  ellipse  semi-axis   in  Y
//     - dz         half-length in z



//______________________________________________________________________________
TELTU::TELTU()
{
//*-*-*-*-*-*-*-*-*-*-*-*ELTU shape default constructor*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                    ==============================


}

//______________________________________________________________________________
TELTU::TELTU(const char *name, const char *title, const char *material, Float_t rx, Float_t ry,
             Float_t dz):TTUBE (name,title,material,0,rx,dz,rx?ry/rx:1.0)
{}

//______________________________________________________________________________
TELTU::~TELTU()
{
//*-*-*-*-*-*-*-*-*-*-*-*-*ELTU shape default destructor*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                      =============================

}

