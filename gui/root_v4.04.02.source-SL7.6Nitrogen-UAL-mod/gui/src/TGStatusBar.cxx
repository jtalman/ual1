// @(#)root/gui:$Name:  $:$Id: TGStatusBar.cxx,v 1.12 2004/10/21 14:25:30 rdm Exp $
// Author: Fons Rademakers   23/01/98

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/
/**************************************************************************

    This source is based on Xclass95, a Win95-looking GUI toolkit.
    Copyright (C) 1996, 1997 David Barth, Ricky Ralston, Hector Peraza.

    Xclass95 is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

**************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGStatusBar                                                          //
//                                                                      //
// Provides a StatusBar widget.                                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TGStatusBar.h"
#include "TGResourcePool.h"
#include "Riostream.h"


const TGFont *TGStatusBar::fgDefaultFont = 0;
TGGC         *TGStatusBar::fgDefaultGC = 0;
TGLayoutHints *TGStatusBar::fgHints = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 0, 0, 0);


class TGStatusBarPart : public TGHorizontalFrame {
friend class TGStatusBar;
private:
   TGString  *fStatusInfo;    // status text to be displayed in this part
   Int_t      fYt;            // y position of text in frame
   virtual void DoRedraw();

public:
   TGStatusBarPart(const TGWindow *p, Int_t h, Int_t y, ULong_t back = GetDefaultFrameBackground());
   ~TGStatusBarPart() { delete fStatusInfo; DestroyWindow(); }
   void SetText(TGString *text);
   const TGString *GetText() const { return fStatusInfo; }
};

//______________________________________________________________________________
TGStatusBarPart::TGStatusBarPart(const TGWindow *p, Int_t h, Int_t y, ULong_t back)
   : TGHorizontalFrame(p, 5, 5, kChildFrame | kHorizontalFrame, back)
{
   // Create statusbar part frame. This frame will contain the text for this
   // statusbar part.

   fStatusInfo = 0;
   fYt = y + 1;
   fHeight = h;
   MapWindow();
}

//______________________________________________________________________________
void TGStatusBarPart::SetText(TGString *text)
{
   // Set text in this part of the statusbar.

   if (fStatusInfo) delete fStatusInfo;
   fStatusInfo = text;
   fClient->NeedRedraw(this);
}

//______________________________________________________________________________
void TGStatusBarPart::DoRedraw()
{
   // Draw string in statusbar part frame.

   TGHorizontalFrame::DoRedraw();

   if (fStatusInfo)
      fStatusInfo->Draw(fId, TGStatusBar::GetDefaultGC()(), 3, fYt);
}


ClassImp(TGStatusBar)

//______________________________________________________________________________
TGStatusBar::TGStatusBar(const TGWindow *p, UInt_t w, UInt_t h,
                         UInt_t options, ULong_t back) :
   TGHorizontalFrame(p, w, h, options, back)
{
   // Create a status bar widget. By default it consist of one part.
   // Multiple parts can be created using SetParts().

   fBorderWidth   = 2;
   fStatusPart    = new TGStatusBarPart* [1];
   fParts         = new Int_t [1];
   fXt            = new Int_t [1];
   fParts[0]      = 100;
   fNpart         = 1;
   f3DCorner      = kTRUE;

   int max_ascent, max_descent;
   gVirtualX->GetFontProperties(GetDefaultFontStruct(), max_ascent, max_descent);
   int ht = max_ascent + max_descent;

   fYt = max_ascent;

   fStatusPart[0] = new TGStatusBarPart(this, ht, fYt);
   AddFrame(fStatusPart[0], fgHints);
   Resize(w, ht + 5);
}

//______________________________________________________________________________
TGStatusBar::~TGStatusBar()
{
   // Delete status bar widget.

   if (!MustCleanup()) {
      for (int i = 0; i < fNpart; i++) {
         delete fStatusPart[i];
      }
   }

   delete [] fStatusPart;
   delete [] fParts;
   delete [] fXt;
}

//______________________________________________________________________________
void TGStatusBar::SetText(TGString *text, Int_t partidx)
{
   // Set text in partition partidx in status bar. The TGString is
   // adopted by the status bar.

   if (partidx < 0 || partidx >= fNpart) {
      Error("SetText", "partidx out of range (0,%d)", fNpart-1);
      return;
   }

   fStatusPart[partidx]->SetText(text);
}

//______________________________________________________________________________
void TGStatusBar::SetText(const char *text, Int_t partidx)
{
   // Set text in partion partidx in status bar.

   SetText(new TGString(text), partidx);
}

//______________________________________________________________________________
void TGStatusBar::DrawBorder()
{
   // Draw the status bar border (including cute 3d corner).

   // Current width is known at this stage so calculate fXt's.
   int i;
   for (i = 0; i < fNpart; i++) {
      if (i == 0)
         fXt[i] = 0;
      else
         fXt[i] = fXt[i-1] + (fWidth * fParts[i-1] / 100);
   }

   //TGFrame::DrawBorder();
   for (i = 0; i < fNpart; i++) {
      int xmax, xmin = fXt[i];
      if (i == fNpart-1)
         xmax = fWidth;
      else
         xmax = fXt[i+1] - 2;

      if (i == fNpart-1) {
         if (f3DCorner)
            fStatusPart[i]->MoveResize(fXt[i]+2, 1, xmax - fXt[i] - 15, fHeight - 2);
         else
            fStatusPart[i]->MoveResize(fXt[i]+2, 1, xmax - fXt[i], fHeight - 2);
      } else
         fStatusPart[i]->MoveResize(fXt[i]+2, 1, xmax - fXt[i] - 4, fHeight - 2);

      gVirtualX->DrawLine(fId, GetShadowGC()(), xmin, 0, xmax-2, 0);
      gVirtualX->DrawLine(fId, GetShadowGC()(), xmin, 0, xmin, fHeight-2);
      gVirtualX->DrawLine(fId, GetHilightGC()(), xmin, fHeight-1, xmax-1, fHeight-1);
      if (i == fNpart-1)
         gVirtualX->DrawLine(fId, GetHilightGC()(), xmax-1, fHeight-1, xmax-1, 0);
      else
         gVirtualX->DrawLine(fId, GetHilightGC()(), xmax-1, fHeight-1, xmax-1, 1);
   }

   // 3d corner...
   if (f3DCorner) {
      gVirtualX->DrawLine(fId, GetShadowGC()(),  fWidth-3,  fHeight-2, fWidth-2, fHeight-3);
      gVirtualX->DrawLine(fId, GetShadowGC()(),  fWidth-4,  fHeight-2, fWidth-2, fHeight-4);
      gVirtualX->DrawLine(fId, GetHilightGC()(), fWidth-5,  fHeight-2, fWidth-2, fHeight-5);

      gVirtualX->DrawLine(fId, GetShadowGC()(),  fWidth-7,  fHeight-2, fWidth-2, fHeight-7);
      gVirtualX->DrawLine(fId, GetShadowGC()(),  fWidth-8,  fHeight-2, fWidth-2, fHeight-8);
      gVirtualX->DrawLine(fId, GetHilightGC()(), fWidth-9,  fHeight-2, fWidth-2, fHeight-9);

      gVirtualX->DrawLine(fId, GetShadowGC()(),  fWidth-11, fHeight-2, fWidth-2, fHeight-11);
      gVirtualX->DrawLine(fId, GetShadowGC()(),  fWidth-12, fHeight-2, fWidth-2, fHeight-12);
      gVirtualX->DrawLine(fId, GetHilightGC()(), fWidth-13, fHeight-2, fWidth-2, fHeight-13);

      gVirtualX->DrawLine(fId, GetBckgndGC()(),  fWidth-13, fHeight-1, fWidth-1, fHeight-1);
      gVirtualX->DrawLine(fId, GetBckgndGC()(),  fWidth-1,  fHeight-1, fWidth-1, fHeight-13);
   }
}

//______________________________________________________________________________
void TGStatusBar::DoRedraw()
{
   // Redraw status bar.

   // calls DrawBorder()
   TGFrame::DoRedraw();

   for (int i = 0; i < fNpart; i++)
      fStatusPart[i]->DoRedraw();
}

//______________________________________________________________________________
void TGStatusBar::SetParts(Int_t *parts, Int_t npart)
{
   // Divide the status bar in nparts. Size of each part is given in parts
   // array (percentual).

   if (npart < 1) {
      Warning("SetParts", "must be at least one part");
      npart = 1;
   }
   if (npart > 15) {
      Error("SetParts", "to many parts (limit is 15)");
      return;
   }

   int i;
   for (i = 0; i < fNpart; i++)
      delete fStatusPart[i];

   delete [] fStatusPart;
   delete [] fParts;
   delete [] fXt;
   fList->Delete();

   fStatusPart = new TGStatusBarPart* [npart];
   fParts      = new Int_t [npart];
   fXt         = new Int_t [npart];

   int tot = 0;
   for (i = 0; i < npart; i++) {
      fStatusPart[i] = new TGStatusBarPart(this, fHeight, fYt);
      AddFrame(fStatusPart[i], fgHints);
      fParts[i] = parts[i];
      tot += parts[i];
      if (tot > 100)
         Error("SetParts", "sum of part > 100");
   }
   if (tot < 100)
      fParts[npart-1] += 100 - tot;
   fNpart = npart;
}

//______________________________________________________________________________
void TGStatusBar::SetParts(Int_t npart)
{
   // Divide the status bar in npart equal sized parts.

   if (npart < 1) {
      Warning("SetParts", "must be at least one part");
      npart = 1;
   }
   if (npart > 40) {
      Error("SetParts", "to many parts (limit is 40)");
      return;
   }

   int i;
   for (i = 0; i < fNpart; i++)
      delete fStatusPart[i];

   delete [] fStatusPart;
   delete [] fParts;
   delete [] fXt;
   fList->Delete();

   fStatusPart = new TGStatusBarPart* [npart];
   fParts      = new Int_t [npart];
   fXt         = new Int_t [npart];

   int sz  = 100/npart;
   int tot = 0;
   for (i = 0; i < npart; i++) {
      fStatusPart[i] = new TGStatusBarPart(this, fHeight, fYt);
      AddFrame(fStatusPart[i], fgHints);
      fParts[i] = sz;
      tot += sz;
   }
   if (tot < 100)
      fParts[npart-1] += 100 - tot;
   fNpart = npart;
}

//______________________________________________________________________________
FontStruct_t TGStatusBar::GetDefaultFontStruct()
{
   if (!fgDefaultFont)
      fgDefaultFont = gClient->GetResourcePool()->GetStatusFont();
   return fgDefaultFont->GetFontStruct();
}

//______________________________________________________________________________
const TGGC &TGStatusBar::GetDefaultGC()
{
   if (!fgDefaultGC) {
      fgDefaultGC = new TGGC(*gClient->GetResourcePool()->GetFrameGC());
      fgDefaultGC->SetFont(fgDefaultFont->GetFontHandle());
   }
   return *fgDefaultGC;
}

//______________________________________________________________________________
TGCompositeFrame *TGStatusBar::GetBarPart(Int_t npart) const
{
   // Returns bar part. That allows to put in the bar part
   // something more interesting than text ;-)

   return  ((npart<fNpart) && (npart>=0)) ? (TGCompositeFrame*)fStatusPart[npart] : 0;
}

//______________________________________________________________________________
TGDimension TGStatusBar::GetDefaultSize() const
{
   //

   UInt_t h = fHeight;

   for (int i = 0; i < fNpart; i++) {
      h = TMath::Max(h,fStatusPart[i]->GetDefaultHeight()+1);
   }
   return TGDimension(fWidth, h);
}

//______________________________________________________________________________
void TGStatusBar::SavePrimitive(ofstream &out, Option_t *option)
{
    // Save a status bar widget as a C++ statement(s) on output stream out.

   if (fBackground != GetDefaultFrameBackground()) SaveUserColor(out, option);

   out << endl;
   out << "   // status bar" << endl;

   out << "   TGStatusBar *";
   out << GetName() <<" = new TGStatusBar("<< fParent->GetName()
       << "," << GetWidth() << "," << GetHeight();

   if (fBackground == GetDefaultFrameBackground()) {
      if (GetOptions() == (kSunkenFrame | kHorizontalFrame)) {
           out <<");" << endl;
      } else {
        out << "," << GetOptionString() <<");" << endl;
      }
   } else {
     out << "," << GetOptionString() << ",ucolor);" << endl;
   }

   int i; char quote = '"';

   if (fNpart > 1)  {
      out << "   Int_t parts" << GetName()+5 << "[] = {" << fParts[0];

      for (i=1; i<fNpart; i++) {
           out  << "," << fParts[i];
      }
      out << "};" << endl;

      out << "   " << GetName() << "->SetParts(parts" << GetName()+5
          << "," << fNpart << ");" <<endl;
   }
   for (i=0; i<fNpart; i++) {
      if (fStatusPart[i]->GetText()) {
          out << "   " << GetName() << "->SetText(" << quote
              << fStatusPart[i]->GetText()->GetString()
              << quote << "," << i << ");" << endl;
      } else {
         if (!fStatusPart[i]->GetList()) continue;
         out << "   TGCompositeFrame *" << fStatusPart[i]->GetName()
             << " = " << GetName() << "->GetBarPart(" << i << ");" << endl;

         TGFrameElement *el;
         TIter next(fStatusPart[i]->GetList());

         while ((el = (TGFrameElement *) next())) {
            el->fFrame->SavePrimitive(out, option);
            out << "   " << fStatusPart[i]->GetName() << "->AddFrame("
                << el->fFrame->GetName();
            el->fLayout->SavePrimitive(out, option);
            out << ");" << endl;
         }
      }
   }
}
