// @(#)root/gpad:$Name:  $:$Id: TInspectCanvas.h,v 1.2 2000/12/13 15:13:49 brun Exp $
// Author: Rene Brun   08/01/2000

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TInspectCanvas
#define ROOT_TInspectCanvas


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TInspectCanvas                                                       //
//                                                                      //
// This class is a specialized canvas to inspect Root objects.          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TCanvas
#include "TCanvas.h"
#endif
#ifndef ROOT_TAttText
#include "TAttText.h"
#endif

class TButton;

class TInspectCanvas : public TCanvas, public TAttText {

protected:

   TButton     *fBackward;      //Pointer to the Backward button
   TButton     *fForward;       //Pointer to the Forward button
   TList       *fObjects;       //List of objects inspected
   TObject     *fCurObject;     //Pointer to object being inspected

public:
   TInspectCanvas();
   TInspectCanvas(UInt_t ww, UInt_t wh);
   virtual        ~TInspectCanvas();
   TButton       *GetBackward() const  {return fBackward;}
   TButton       *GetForward() const    {return fForward;}
   TObject       *GetCurObject() const  {return fCurObject;}
   TList         *GetObjects() const    {return fObjects;}
   static  void   GoBackward();
   static  void   GoForward();
   static  void   Inspector(TObject *obj);
   virtual void   InspectObject(TObject *obj);
   virtual void   RecursiveRemove(TObject *obj);

   //dummies
   virtual void   Divide(Int_t nx=1, Int_t ny=1, Float_t xmargin=0.01, Float_t ymargin=0.01, Int_t color=0);
   virtual void   SetGrid(Int_t valuex = 1, Int_t valuey = 1);
   virtual void   SetGridx(Int_t value = 1);
   virtual void   SetGridy(Int_t value = 1);
   virtual void   SetLogx(Int_t value = 1);
   virtual void   SetLogy(Int_t value = 1);
   virtual void   SetLogz(Int_t value = 1);
   virtual void   SetTickx(Int_t value = 1);
   virtual void   SetTicky(Int_t value = 1);
   virtual void   x3d(Option_t *option="");

   ClassDef(TInspectCanvas,1)  //The canvas Inspector
};

inline void TInspectCanvas::Divide(Int_t, Int_t, Float_t, Float_t, Int_t) { }
inline void TInspectCanvas::SetGrid(Int_t, Int_t) { }
inline void TInspectCanvas::SetGridx(Int_t) { }
inline void TInspectCanvas::SetGridy(Int_t) { }
inline void TInspectCanvas::SetLogx(Int_t) { }
inline void TInspectCanvas::SetLogy(Int_t) { }
inline void TInspectCanvas::SetLogz(Int_t) { }
inline void TInspectCanvas::SetTickx(Int_t) { }
inline void TInspectCanvas::SetTicky(Int_t) { }
inline void TInspectCanvas::x3d(Option_t *) { }

#endif

