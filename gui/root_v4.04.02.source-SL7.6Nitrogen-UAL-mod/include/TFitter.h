// @(#)root/minuit:$Name:  $:$Id: TFitter.h,v 1.10 2004/10/22 21:18:23 brun Exp $
// Author: Rene Brun   31/08/99

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/
#ifndef ROOT_TFitter
#define ROOT_TFitter


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TFitter                                                              //
//                                                                      //
// The ROOT standard fitter based on TMinuit                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef ROOT_TVirtualFitter
#include "TVirtualFitter.h"
#endif

class TMinuit;

class TFitter : public TVirtualFitter {

private:
   Int_t      fNlog;       //Number of elements in fSunLog
   Double_t  *fCovar;      //Covariance matrix
   Double_t  *fSumLog;     //Sum of logs (array of fNlog elements)
   TMinuit   *fMinuit;     //pointer to the TMinuit object
   
   public:
   TFitter(Int_t maxpar = 25);
   virtual ~TFitter();
   virtual Double_t   Chisquare(Int_t npar, Double_t *params) const ;
   virtual void       Clear(Option_t *option="");
   virtual Int_t      ExecuteCommand(const char *command, Double_t *args, Int_t nargs);
   virtual void       FixParameter(Int_t ipar);
   virtual Double_t  *GetCovarianceMatrix() const;
   virtual Double_t   GetCovarianceMatrixElement(Int_t i, Int_t j) const;
   virtual Int_t      GetErrors(Int_t ipar,Double_t &eplus, Double_t &eminus, Double_t &eparab, Double_t &globcc) const;
   virtual Int_t      GetNumberTotalParameters() const;
   virtual Int_t      GetNumberFreeParameters() const;
   virtual Double_t   GetParError(Int_t ipar) const;
   virtual Double_t   GetParameter(Int_t ipar) const;
   virtual Int_t      GetParameter(Int_t ipar,char *name,Double_t &value,Double_t &verr,Double_t &vlow, Double_t &vhigh) const;
   virtual Int_t      GetStats(Double_t &amin, Double_t &edm, Double_t &errdef, Int_t &nvpar, Int_t &nparx) const;
   virtual Double_t   GetSumLog(Int_t i);
   virtual Bool_t     IsFixed(Int_t ipar) const;
   virtual void       PrintResults(Int_t level, Double_t amin) const;
   virtual void       ReleaseParameter(Int_t ipar);
   virtual void       SetFCN(void *fcn);
   virtual void       SetFCN(void (*fcn)(Int_t &, Double_t *, Double_t &f, Double_t *, Int_t));
   virtual void       SetFitMethod(const char *name);
   virtual Int_t      SetParameter(Int_t ipar,const char *parname,Double_t value,Double_t verr,Double_t vlow, Double_t vhigh);

    ClassDef(TFitter,0)  //The ROOT standard fitter based on TMinuit
};

#endif
