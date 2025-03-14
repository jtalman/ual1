// @(#)root/hist:$Name:  $:$Id: TGraph2DErrors.h,v 1.00
// Author: Olivier Couet 26/11/2003

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TGraph2DErrors
#define ROOT_TGraph2DErrors


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGraph2DErrors                                                       //
//                                                                      //
// a 2D Graph with error bars                                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TGraph2D
#include "TGraph2D.h"
#endif

class TGraph2DErrors : public TGraph2D {

protected:
    Double_t *fEX; //[fNpoints] array of X errors
    Double_t *fEY; //[fNpoints] array of Y errors
    Double_t *fEZ; //[fNpoints] array of Z errors

public:
        TGraph2DErrors();
        TGraph2DErrors(Int_t n);
        TGraph2DErrors(Int_t n, Double_t *x, Double_t *y, Double_t *z,
                                Double_t *ex=0, Double_t *ey=0, Double_t *ez=0, Option_t *option="");
        virtual ~TGraph2DErrors();
        Double_t        GetErrorX(Int_t bin) const;
        Double_t        GetErrorY(Int_t bin) const;
        Double_t        GetErrorZ(Int_t bin) const;
        Double_t       *GetEX() const {return fEX;}
        Double_t       *GetEY() const {return fEY;}
        Double_t       *GetEZ() const {return fEZ;}
        virtual void    SetPoint(Int_t i, Double_t x, Double_t y, Double_t z);
        virtual void    SetPointError(Int_t i, Double_t ex, Double_t ey, Double_t ez);

        ClassDef(TGraph2DErrors,1)  //A 2D graph with error bars
};

#endif


