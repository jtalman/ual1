// @(#)root/hist:$Name:  $:$Id: TH1K.h,v 1.3 2002/01/02 21:44:50 brun Exp $
// Author: Victor Perevoztchikov <perev@bnl.gov>  21/02/2001

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TH1K
#define ROOT_TH1K


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TH1K                                                                 //
//                                                                      //
// 1-Dim histogram nearest K Neighbour class.                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TH1.h"
//________________________________________________________________________

class TH1K : public TH1, public TArrayF {

private:
    void Sort(); 
protected:
    Int_t fReady;	//!
    Int_t fNIn;
    Int_t fKOrd;	//!
    Int_t fKCur;	//!
public:
    TH1K();
    TH1K(const char *name,const char *title,Int_t nbinsx,Axis_t xlow,Axis_t xup,Int_t k=0);
    virtual ~TH1K();

    virtual Int_t   Fill(Axis_t x);
    virtual Int_t   Fill(Axis_t x,Stat_t w){return TH1::Fill(x,w);}
    virtual Int_t   Fill(const char *name,Stat_t w){return TH1::Fill(name,w);}
    virtual Stat_t  GetBinContent(Int_t bin) const;
    virtual Stat_t  GetBinContent(Int_t bin,Int_t) const {return GetBinContent(bin);}
    virtual Stat_t  GetBinContent(Int_t bin,Int_t,Int_t) const {return GetBinContent(bin);}

    virtual Stat_t  GetBinError(Int_t bin) const;
    virtual Stat_t  GetBinError(Int_t bin,Int_t) const {return GetBinError(bin);}
    virtual Stat_t  GetBinError(Int_t bin,Int_t,Int_t) const {return GetBinError(bin);}
    
    
    virtual void    Reset(Option_t *option="");
    virtual void    SavePrimitive(ofstream &out, Option_t *option);

    void    SetKOrd(Int_t k){fKOrd=k;}
   
    ClassDef(TH1K,1)  //1-Dim Nearest Kth neighbour method
};

#endif
