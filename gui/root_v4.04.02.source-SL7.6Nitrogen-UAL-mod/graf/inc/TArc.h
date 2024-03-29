// @(#)root/graf:$Name:  $:$Id: TArc.h,v 1.4 2002/10/31 07:27:34 brun Exp $
// Author: Rene Brun   16/10/95

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TArc
#define ROOT_TArc


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TArc                                                                 //
//                                                                      //
// Arc of a circle.                                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TEllipse
#include "TEllipse.h"
#endif


class TArc : public TEllipse {

public:
        TArc();
        TArc(Double_t x1, Double_t y1,Double_t radius
           , Double_t phimin=0,Double_t phimax=360);
        TArc(const TArc &arc);
        virtual ~TArc();
                void Copy(TObject &arc) const;
        virtual void DrawArc(Double_t x1, Double_t y1, Double_t radius
                            ,Double_t  phimin=0, Double_t  phimax=360, Option_t *option="");
        virtual void   SavePrimitive(ofstream &out, Option_t *option);

        ClassDef(TArc,1)  //Arc of a circle
};

#endif
