// @(#)root/g3d:$Name:  $:$Id: TELTU.h,v 1.1.1.1 2000/05/16 17:00:43 rdm Exp $
// Author: Rene Brun   26/06/97

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TELTU
#define ROOT_TELTU


////////////////////////////////////////////////////////////////////////////
//                                                                        //
// TELTU                                                                  //
//                                                                        //
// 'ELTU' is a  cylinder with  an elliptical  section.  It  has three     //
//        parameters:  the  ellipse  semi-axis   in  X,  the  ellipse     //
//        semi-axis in Y  and the half length in Z.   The equation of     //
//        the conical curve is:                                           //
//             X**2/fRx**2  +  Y**2/fRy**2  =  1                          //
//        ELTU is not divisible.                                          //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TTUBE
#include "TTUBE.h"
#endif

class TELTU : public TTUBE {

    public:
        TELTU();
        TELTU(const char *name, const char *title, const char *material, Float_t rx, Float_t ry,Float_t dz);
        virtual ~TELTU();

        ClassDef(TELTU,1)  //ELTU shape
};

#endif
