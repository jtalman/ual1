// @(#)root/histpainter:$Name:  $:$Id: TPaletteAxis.h,v 1.3 2003/01/13 16:57:35 brun Exp $
// Author: Rene Brun   15/11/2002

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TPaletteAxis
#define ROOT_TPaletteAxis


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TPaletteAxis                                                         //
//                                                                      //
// class used to display a color palette axis for 2-d plots             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TPave
#include "TPave.h"
#endif
#ifndef ROOT_TGaxis
#include "TGaxis.h"
#endif
#ifndef ROOT_TH1
#include "TH1.h"
#endif

class TPaletteAxis : public TPave {

protected:
   TGaxis       fAxis;          //palette axis
   TH1         *fH;             //pointer to parent histogram
   TString      fName;          //Pave name

public:
        // TPaletteAxis status bits
        enum { kHasView   = BIT(11)};

   TPaletteAxis();
   TPaletteAxis(Double_t x1, Double_t y1,Double_t x2 ,Double_t y2, TH1 *h);
   TPaletteAxis(const TPaletteAxis &palette);
   virtual ~TPaletteAxis();
           void  Copy(TObject &palette) const;
   virtual Int_t DistancetoPrimitive(Int_t px, Int_t py);
   virtual void  ExecuteEvent(Int_t event, Int_t px, Int_t py);
         TGaxis *GetAxis() {return &fAxis;}
     Option_t   *GetName() const {return fName.Data();}
   virtual char *GetObjectInfo(Int_t px, Int_t py) const;
   virtual void  Paint(Option_t *option="");
   virtual void  SavePrimitive(ofstream &out, Option_t *option);
   virtual void  SetName(const char *name="") {fName = name;} // *MENU*
   virtual void  SetLabelColor(Int_t labelcolor) {fAxis.SetLabelColor(labelcolor);} // *MENU*
   virtual void  SetLabelFont(Int_t labelfont) {fAxis.SetLabelFont(labelfont);} // *MENU*
   virtual void  SetLabelOffset(Float_t labeloffset) {fAxis.SetLabelOffset(labeloffset);} // *MENU*
   virtual void  SetLabelSize(Float_t labelsize) {fAxis.SetLabelSize(labelsize);} // *MENU*
   virtual void  SetTitleOffset(Float_t titleoffset=1) {fAxis.SetTitleOffset(titleoffset);} // *MENU*
   virtual void  SetTitleSize(Float_t titlesize) {fAxis.SetTitleSize(titlesize);} // *MENU*
   virtual void  UnZoom();  // *MENU*

        ClassDef(TPaletteAxis,2)  //class used to display a color palette axis for 2-d plots
};

#endif

