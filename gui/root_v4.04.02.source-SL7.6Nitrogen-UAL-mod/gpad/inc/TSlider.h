// @(#)root/gpad:$Name:  $:$Id: TSlider.h,v 1.4 2001/01/23 21:09:08 brun Exp $
// Author: Rene Brun   23/11/96

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TSlider
#define ROOT_TSlider


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TSlider                                                              //
//                                                                      //
//  A TSlider object is a specialized TPad including a TSliderBox object//
//  The TSliderBox can be moved in the pad.                             //
//  Slider drawing options include the possibility to change the slider //
//  starting and ending positions or only one of them.                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TPad
#include "TPad.h"
#endif

class TSlider : public TPad {

protected:
   Double_t      fMinimum;      //Slider minimum value in [0,1]
   Double_t      fMaximum;      //Slider maximum value in [0,1]
   TObject      *fObject;       //!Pointer to associated object
   TString       fMethod;       //command to be executed when slider is changed

private:
   TSlider(const TSlider &org);             // no copy, use TObject::Clone()
   TSlider &operator=(const TSlider &rhs);  // idem

public:
   TSlider();
   TSlider(const char *name, const char *title, Double_t x1, Double_t y1,Double_t x2 ,Double_t y2, Color_t color=16, Short_t bordersize=2, Short_t bordermode =-1);
   virtual ~TSlider();
   TObject      *GetObject()  const {return fObject;}
   Double_t      GetMinimum() const {return fMinimum;}
   Double_t      GetMaximum() const {return fMaximum;}
   virtual const char *GetMethod() const { return fMethod.Data(); }
   virtual void  Paint(Option_t *option="");
   virtual void  SavePrimitive(ofstream &out, Option_t *option);
   virtual void  SetMethod(const char *method) { fMethod=method; } // *MENU*
   void          SetObject(TObject *obj=0) {fObject=obj;}
   virtual void  SetMinimum(Double_t min=0) {fMinimum=min;}
   virtual void  SetMaximum(Double_t max=1) {fMaximum=max;}
   virtual void  SetRange(Double_t xmin=0, Double_t xmax=1);

   ClassDef(TSlider,1)  //A user interface slider.
};

#endif

