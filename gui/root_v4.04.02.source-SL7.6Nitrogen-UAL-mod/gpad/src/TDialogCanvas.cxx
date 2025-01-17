// @(#)root/gpad:$Name:  $:$Id: TDialogCanvas.cxx,v 1.9 2003/03/05 20:11:10 brun Exp $
// Author: Rene Brun   03/07/96

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include "TROOT.h"
#include "TDialogCanvas.h"
#include "TGroupButton.h"
#include "TText.h"
#include "TStyle.h"

ClassImp(TDialogCanvas)

//______________________________________________________________________________
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*
//*-*   A DialogCanvas is a canvas specialized to set attributes.
//*-*   It contains, in general, TGroupButton objects.
//*-*   When the APPLY button is executed, the actions corresponding
//*-*   to the active buttons are executed via the Interpreter.
//*-*
//*-*  See examples in TAttLineCanvas, TAttFillCanvas, TAttTextCanvas, TAttMarkerCanvas

//______________________________________________________________________________
TDialogCanvas::TDialogCanvas() : TCanvas()
{
//*-*-*-*-*-*-*-*-*-*-*-*DialogCanvas default constructor*-*-*-*-*-*-*-*-*-*-*
//*-*                    ================================

}

//_____________________________________________________________________________
TDialogCanvas::TDialogCanvas(const char *name, const char *title, Int_t ww, Int_t wh)
             : TCanvas(name,title,-ww,wh)
{
//*-*-*-*-*-*-*-*-*-*-*-*DialogCanvas constructor*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                    ========================

   SetFillColor(36);
   fRefObject = 0;
   fRefPad    = 0;
}

//_____________________________________________________________________________
TDialogCanvas::TDialogCanvas(const char *name, const char *title, Int_t wtopx, Int_t wtopy, UInt_t ww, UInt_t wh)
             : TCanvas(name,title,-wtopx,wtopy,ww,wh)
{
//*-*-*-*-*-*-*-*-*-*-*-*DialogCanvas constructor*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                    ========================

   SetFillColor(36);
   fRefObject = 0;
   fRefPad    = 0;
}

//______________________________________________________________________________
TDialogCanvas::~TDialogCanvas()
{
//*-*-*-*-*-*-*-*-*-*-*DialogCanvas default destructor*-*-*-*-*-*-*-*-*-*-*-*
//*-*                  ===============================
}

//______________________________________________________________________________
void TDialogCanvas::Apply(const char *action)
{
//*-*-*-*-*-*-*-*-*Called when the APPLY button is executed*-*-*-*-*-*-*-*-*-*-*
//*-*              ========================================

   if (!fRefPad) return;
   SetCursor(kWatch);

   TIter next(fPrimitives);
   TObject *refobj = fRefObject;
   TObject *obj;
   TGroupButton *button;
   if (!strcmp(action,"gStyle")) fRefObject = gStyle;

   while ((obj = next())) {
      if (obj->InheritsFrom(TGroupButton::Class())) {
         button = (TGroupButton*)obj;
         if (button->GetBorderMode() < 0) button->ExecuteAction();
      }
   }
   fRefObject = refobj;
   if (!gROOT->GetSelectedPad()) return;
   gROOT->GetSelectedPad()->Modified();
   gROOT->GetSelectedPad()->Update();
}

//______________________________________________________________________________
void TDialogCanvas::BuildStandardButtons()
{
//*-*-*-*-*-*-*-*-*Create APPLY, gStyle and CLOSE buttons*-*-*-*-*-*-*-*-*-*-*
//*-*              ======================================

   TGroupButton *apply = new TGroupButton("APPLY","Apply","",.05,.01,.3,.09);
   apply->SetTextSize(0.55);
   apply->SetBorderSize(3);
   apply->SetFillColor(44);
   apply->Draw();

   apply = new TGroupButton("APPLY","gStyle","",.375,.01,.625,.09);
   apply->SetTextSize(0.55);
   apply->SetBorderSize(3);
   apply->SetFillColor(44);
   apply->Draw();

   apply = new TGroupButton("APPLY","Close","",.70,.01,.95,.09);
   apply->SetTextSize(0.55);
   apply->SetBorderSize(3);
   apply->SetFillColor(44);
   apply->Draw();
}

//______________________________________________________________________________
void TDialogCanvas::Range(Double_t x1, Double_t y1, Double_t x2, Double_t y2)
{
//*-*-*-*-*-*-*-*-*-*-*Set world coordinate system for the pad*-*-*-*-*-*-*
//*-*                  =======================================

   TPad::Range(x1,y1,x2,y2);
}

//______________________________________________________________________________
void TDialogCanvas::RecursiveRemove(TObject *obj)
{
//*-*-*-*-*-*-*-*Recursively remove object from a pad and its subpads*-*-*-*-*
//*-*            ====================================================

   TPad::RecursiveRemove(obj);
   if (fRefObject == obj) fRefObject = 0;
   if (fRefPad    == obj) fRefPad    = 0;
}
