// @(#)root/gui:$Name:  $:$Id: TGDoubleSlider.cxx,v 1.12 2005/01/12 18:39:29 brun Exp $
// Author: Reiner Rohlfs   30/09/98

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
// TGDoubleSlider, TGDoubleVSlider and TGDoubleHSlider                  //
//                                                                      //
// DoubleSlider widgets allow easy selection of a min and a max value   //
// out of a range.                                                      //
// DoubleSliders can be either horizontal or vertical oriented and      //
// there is a choice of three different types of tick marks.            //
//                                                                      //
// To change the min value press the mouse near to the left / bottom    //
// edge of the slider.                                                  //
// To change the max value press the mouse near to the right / top      //
// edge of the slider.                                                  //
// To change both values simultaneously press the mouse near to the     //
// center of the slider.                                                //
//                                                                      //
// TGDoubleSlider is an abstract base class. Use the concrete           //
// TGDoubleVSlider and TGDoubleHSlider.                                 //
//                                                                      //
// Dragging the slider will generate the event:                         //
// kC_VSLIDER, kSL_POS, slider id, 0  (for vertical slider)             //
// kC_HSLIDER, kSL_POS, slider id, 0  (for horizontal slider)           //
//                                                                      //
// Pressing the mouse will generate the event:                          //
// kC_VSLIDER, kSL_PRESS, slider id, 0  (for vertical slider)           //
// kC_HSLIDER, kSL_PRESS, slider id, 0  (for horizontal slider)         //
//                                                                      //
// Releasing the mouse will generate the event:                         //
// kC_VSLIDER, kSL_RELEASE, slider id, 0  (for vertical slider)         //
// kC_HSLIDER, kSL_RELEASE, slider id, 0  (for horizontal slider)       //
//                                                                      //
// Use the functions GetMinPosition(), GetMaxPosition() and             //
// GetPosition() to retrieve the position of the slider.                //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TGDoubleSlider.h"
#include "Riostream.h"
#include "TSystem.h"


ClassImp(TGDoubleSlider)
ClassImp(TGDoubleVSlider)
ClassImp(TGDoubleHSlider)

//______________________________________________________________________________
TGDoubleSlider::TGDoubleSlider(const TGWindow *p, UInt_t w, UInt_t h, UInt_t type, Int_t id,
                               UInt_t options, ULong_t back,
                               Bool_t reversed, Bool_t mark_ends)
   : TGFrame(p, w, h, options, back)
{
   // Slider constructor.

   fWidgetId    = id;
   fWidgetFlags = kWidgetWantFocus;
   fMsgWindow   = p;

   fScaleType = type;
   fScale = 10;

   fReversedScale = reversed;
   fMarkEnds = mark_ends;

   gVirtualX->GrabButton(fId, kAnyButton, kAnyModifier,
                    kButtonPressMask | kButtonReleaseMask |
                    kPointerMotionMask, kNone, kNone);
   SetWindowName();
}

//______________________________________________________________________________
void TGDoubleSlider::FixBounds(Float_t &min, Float_t &max)
{
   // Avoid boundaries to be equal.

   if (min > max) min = max;

   Float_t eps = 1e-6;
   if (max - min < eps) {
      if (max == 0)
         max += eps;
      else
         max += max*eps;
      if (min == 0)
         min -= eps;
      else
         min -= min*eps;
   }
}

//______________________________________________________________________________
TString TGDoubleSlider::GetSString() const
{
   // Returns the slider type as a string - used in SavePrimitive()

   TString stype;

   if (fScaleType) {
       if (fScaleType & kDoubleScaleNo)  {
           if (stype.Length() == 0) stype  = "kDoubleScaleNo";
           else                     stype += " | kDoubleScaleNo";
       }
       if (fScaleType & kDoubleScaleDownRight) {
           if (stype.Length() == 0) stype  = "kDoubleScaleDownRight";
           else                     stype += " | kDoubleScaleDownRight";
       }
       if (fScaleType & kDoubleScaleBoth) {
           if (stype.Length() == 0) stype  = "kDoubleScaleBoth";
           else                     stype += " | kDoubleScaleBoth";
       }
   }
   return stype;
}

//______________________________________________________________________________
TGDoubleVSlider::TGDoubleVSlider(const TGWindow *p, UInt_t h, UInt_t type, Int_t id,
                                 UInt_t options, ULong_t back,
                                 Bool_t reversed, Bool_t mark_ends)
    : TGDoubleSlider(p, kDoubleSliderWidth, h, type, id, options, back,
                     reversed, mark_ends)
{
   // Create a vertical slider widget.

   // set initial values
   fSmin = h/8*3; fSmax = h/8*5; fVmin = 0; fVmax = h;
   SetWindowName();
}

//______________________________________________________________________________
TGDoubleVSlider::~TGDoubleVSlider()
{
   // Delete vertical slider widget.
}

//______________________________________________________________________________
void TGDoubleVSlider::DoRedraw()
{
   // Redraw vertical slider widget.

   FixBounds(fVmin, fVmax);

   // cleanup the drawable
   gVirtualX->ClearWindow(fId);

   if (fSmin < fVmin) fSmin = fVmin;
   if (fSmax < fVmin) fSmax = fVmin;
   if (fSmin > fVmax) fSmin = fVmax;
   if (fSmax > fVmax) fSmax = fVmax;
   if (fSmin > fSmax) fSmin = fSmax = (fSmin + fSmax) / 2;

   int relMin = (int)((fHeight-16) * (fSmin - fVmin) / (fVmax - fVmin)) + 1;
   int relMax = (int)((fHeight-16) * (fSmax - fVmin) / (fVmax - fVmin) + 15);

   gVirtualX->DrawLine(fId, GetHilightGC()(), fWidth/2-6, relMin, fWidth/2+5, relMin);
   gVirtualX->DrawLine(fId, GetHilightGC()(), fWidth/2-6, relMin, fWidth/2-6, relMax);
   gVirtualX->DrawLine(fId, GetBlackGC()(),   fWidth/2+5, relMax, fWidth/2-6, relMax);
   gVirtualX->DrawLine(fId, GetBlackGC()(),   fWidth/2+5, relMax, fWidth/2+5, relMin);

   if (relMin-1 > 8) {
      gVirtualX->DrawLine(fId, GetShadowGC()(),  fWidth/2-1, 8, fWidth/2-1, relMin-1);
      gVirtualX->DrawLine(fId, GetHilightGC()(), fWidth/2+1, 8, fWidth/2+1, relMin-1);
      gVirtualX->DrawLine(fId, GetBlackGC()(),   fWidth/2,   8, fWidth/2,   relMin-1);
   }
   if (relMax+1 < (int)fHeight-8) {
      gVirtualX->DrawLine(fId, GetShadowGC()(),  fWidth/2-1, relMax+1, fWidth/2-1, fHeight-8);
      gVirtualX->DrawLine(fId, GetHilightGC()(), fWidth/2+1, relMax+1, fWidth/2+1, fHeight-8);
      gVirtualX->DrawLine(fId, GetBlackGC()(),   fWidth/2,   relMax+1, fWidth/2,   fHeight-8);
   }

   // check scale
   if (fScale == 1) fScale++;
   if (fScale * 2 > (int)fHeight) fScale = 0;
   if (fScale > 0 && !(fScaleType & kDoubleScaleNo)) {
      int lines = ((int)fHeight-16) / fScale;
      int remain = ((int)fHeight-16) % fScale;
      if (lines < 1) lines = 1;
      for (int i = 0; i <= lines; i++) {
         int y = i * fScale + (i * remain) / lines;
         gVirtualX->DrawLine(fId, GetBlackGC()(), fWidth/2+8, y+7, fWidth/2+10, y+7);
         if ((fScaleType && kDoubleScaleBoth))
            gVirtualX->DrawLine(fId, GetBlackGC()(), fWidth/2-9, y+7, fWidth/2-11, y+7);
      }
   }

   if (fMarkEnds) {
      // Draw scaling zones.
      int y1 = (relMax - relMin) / 4 + relMin;
      int y2 = (relMax - relMin) / 4 * 3 + relMin;
      gVirtualX->DrawLine(fId, GetBlackGC()(), fWidth/2-6, y1, fWidth/2+5, y1);
      gVirtualX->DrawLine(fId, GetBlackGC()(), fWidth/2-6, y2, fWidth/2+5, y2);
   }
}

//______________________________________________________________________________
Bool_t TGDoubleVSlider::HandleButton(Event_t *event)
{
   // Handle mouse button event in vertical slider.

   if (event->fType == kButtonPress && event->fCode == kButton1) {
      fPressPoint = event->fY;
      fPressSmin  = fSmin;
      fPressSmax  = fSmax;

      int relMin = (int)((fHeight-16) * (fSmin - fVmin) / (fVmax - fVmin)) + 1;
      int relMax = (int)((fHeight-16) * (fSmax - fVmin) / (fVmax - fVmin) + 15);
      if (fPressPoint < (relMax - relMin) / 4 + relMin)
         // move only min value
         fMove = 1;
      else if (fPressPoint > (relMax - relMin) / 4 * 3 + relMin)
         // move only max value
         fMove = 2;
      else
         // move min and max value
         fMove = 3;

      SendMessage(fMsgWindow, MK_MSG(kC_VSLIDER, kSL_PRESS), fWidgetId, 0);
      fClient->ProcessLine(fCommand, MK_MSG(kC_VSLIDER, kSL_PRESS), fWidgetId, 0);
      Pressed();

      // last argument kFALSE forces all specified events to this window
      gVirtualX->GrabPointer(fId, kButtonPressMask | kButtonReleaseMask |
                             kPointerMotionMask, kNone, kNone,
                             kTRUE, kFALSE);
   } else if (event->fType == kButtonRelease && event->fCode == kButton1) {
      SendMessage(fMsgWindow, MK_MSG(kC_VSLIDER, kSL_RELEASE), fWidgetId, 0);
      fClient->ProcessLine(fCommand, MK_MSG(kC_VSLIDER, kSL_RELEASE), fWidgetId, 0);
      Released();
      fMove = 0;

      gVirtualX->GrabPointer(0, 0, 0, 0, kFALSE);  // ungrab pointer
   } else
      fMove = 0;

   return kTRUE;
}

//______________________________________________________________________________
Bool_t TGDoubleVSlider::HandleMotion(Event_t *event)
{
   // Handle mouse motion event in vertical slider.

   static Long_t was = gSystem->Now();
   Long_t now = (long)gSystem->Now();

   if ((now-was) < 50) return kTRUE;
   was = now;

   int       diff;
   Float_t   oldMin, oldMax;

   diff    = event->fY - fPressPoint;
   oldMin  = fSmin;
   oldMax  = fSmax;

   if (fMove == 1) {
      // change of min value
      fSmin = fPressSmin + diff * (fVmax - fVmin) / (fHeight-16);
      if (fSmin < fVmin) fSmin = fVmin;
      if (fSmin > fSmax) fSmin = fSmax;
   } else if (fMove == 2) {
      // change of max value
      fSmax = fPressSmax + diff * (fVmax - fVmin) / (fHeight-16);
      if (fSmax > fVmax) fSmax = fVmax;
      if (fSmax < fSmin) fSmax = fSmin;
   } else if (fMove == 3) {
      // change of min and of max value
      Float_t logicalDiff;
      logicalDiff = diff * (fVmax - fVmin) / (fHeight-16);
      if (fPressSmax + logicalDiff > fVmax)
         logicalDiff = fVmax - fPressSmax;
      if (fPressSmin + logicalDiff < fVmin)
         logicalDiff = fVmin - fPressSmin;
      fSmax = fPressSmax + logicalDiff;
      fSmin = fPressSmin + logicalDiff;
   }

   // check if position has changed
   if (fMove != 0 && (fSmax != oldMax || fSmin != oldMin)) {
      fClient->NeedRedraw(this);
      SendMessage(fMsgWindow, MK_MSG(kC_VSLIDER, kSL_POS), fWidgetId, 0);
      fClient->ProcessLine(fCommand, MK_MSG(kC_VSLIDER, kSL_POS), fWidgetId, 0);
      PositionChanged();
   }
   return kTRUE;
}

//______________________________________________________________________________
TGDoubleHSlider::TGDoubleHSlider(const TGWindow *p, UInt_t w, UInt_t type, Int_t id,
                                 UInt_t options, ULong_t back,
                                 Bool_t reversed, Bool_t mark_ends)
    : TGDoubleSlider(p, w, kDoubleSliderHeight, type, id, options, back,
                     reversed, mark_ends)
{
   // Create horizontal slider widget.

   // set initial values
   fSmin = w/8*3; fSmax = w/8*5; fVmin = 0; fVmax = w;
   SetWindowName();
}

//______________________________________________________________________________
TGDoubleHSlider::~TGDoubleHSlider()
{
   // Delete a horizontal slider widget.

}

//______________________________________________________________________________
void TGDoubleHSlider::DoRedraw()
{
   // Redraw horizontal slider widget.

   FixBounds(fVmin, fVmax);

   // cleanup drawable
   gVirtualX->ClearWindow(fId);

   if (fSmin < fVmin) fSmin = fVmin;
   if (fSmax > fVmax) fSmax = fVmax;
   if (fSmin > fSmax) fSmin = fSmax = (fSmin + fSmax) / 2;

   int relMin = (int)((fWidth-16) * (fSmin - fVmin) / (fVmax - fVmin)) + 1;
   int relMax = (int)((fWidth-16) * (fSmax - fVmin) / (fVmax - fVmin) + 15);

   gVirtualX->DrawLine(fId, GetHilightGC()(), relMin, fHeight/2-6, relMin, fHeight/2+5);
   gVirtualX->DrawLine(fId, GetHilightGC()(), relMax, fHeight/2-6, relMin, fHeight/2-6);
   gVirtualX->DrawLine(fId, GetBlackGC()(),   relMax, fHeight/2+5, relMax, fHeight/2-6);
   gVirtualX->DrawLine(fId, GetBlackGC()(),   relMin, fHeight/2+5, relMax, fHeight/2+5);

   if (relMin-1 > 8) {
      gVirtualX->DrawLine(fId, GetShadowGC()(),  8, fHeight/2-1, relMin-1, fHeight/2-1);
      gVirtualX->DrawLine(fId, GetHilightGC()(), 8, fHeight/2+1, relMin-1, fHeight/2+1);
      gVirtualX->DrawLine(fId, GetBlackGC()(),   8, fHeight/2,   relMin-1, fHeight/2);
   }
   if (relMax+1 < (int)fWidth-8) {
      gVirtualX->DrawLine(fId, GetShadowGC()(),  relMax+1, fHeight/2-1, fWidth-8, fHeight/2-1);
      gVirtualX->DrawLine(fId, GetHilightGC()(), relMax+1, fHeight/2+1, fWidth-8, fHeight/2+1);
      gVirtualX->DrawLine(fId, GetBlackGC()(),   relMax+1, fHeight/2,   fWidth-8, fHeight/2);
   }

   if (fScale == 1) fScale++;
   if (fScale * 2 > (int)fWidth) fScale = 0;
   if (fScale > 0 && !(fScaleType & kDoubleScaleNo)) {
      int lines = ((int)fWidth-16) / fScale;
      int remain = ((int)fWidth-16) % fScale;
      if (lines < 1) lines = 1;
      for (int i = 0; i <= lines; i++) {
         int x = i * fScale + (i * remain) / lines;
         gVirtualX->DrawLine(fId, GetBlackGC()(), x+7, fHeight/2+8, x+7, fHeight/2+10);
         if ((fScaleType && kDoubleScaleBoth))
            gVirtualX->DrawLine(fId, GetBlackGC()(), x+7, fHeight/2-9, x+7, fHeight/2-11);
      }
   }

   if (fMarkEnds) {
      // Draw scaling zones.
      int x1 = (relMax - relMin) / 4 + relMin;
      int x2 = (relMax - relMin) / 4 * 3 + relMin;
      gVirtualX->DrawLine(fId, GetBlackGC()(), x1, fHeight/2-6, x1, fHeight/2+5);
      gVirtualX->DrawLine(fId, GetBlackGC()(), x2, fHeight/2-6, x2, fHeight/2+5);
   }
}

//______________________________________________________________________________
Bool_t TGDoubleHSlider::HandleButton(Event_t *event)
{
   // Handle mouse button event in horizontal slider widget.

   if (event->fType == kButtonPress && event->fCode == kButton1) {
      fPressPoint = event->fX;
      fPressSmin  = fSmin;
      fPressSmax  = fSmax;

      int relMin = (int)((fWidth-16) * (fSmin - fVmin) / (fVmax - fVmin)) + 1;
      int relMax = (int)((fWidth-16) * (fSmax - fVmin) / (fVmax - fVmin) + 15);
      if (fPressPoint < (relMax - relMin) / 4 + relMin)
         // move only min value
         fMove = 1;
      else if (fPressPoint > (relMax - relMin) / 4 * 3 + relMin)
         // move only max value
         fMove = 2;
      else
         // move min and max value
         fMove = 3;

      SendMessage(fMsgWindow, MK_MSG(kC_HSLIDER, kSL_PRESS), fWidgetId, 0);
      fClient->ProcessLine(fCommand, MK_MSG(kC_HSLIDER, kSL_PRESS), fWidgetId, 0);
      Pressed();

      // last argument kFALSE forces all specified events to this window
      gVirtualX->GrabPointer(fId, kButtonPressMask | kButtonReleaseMask |
                             kPointerMotionMask, kNone, kNone,
                             kTRUE, kFALSE);
   } else if (event->fType == kButtonRelease && event->fCode == kButton1) {
      SendMessage(fMsgWindow, MK_MSG(kC_HSLIDER, kSL_RELEASE), fWidgetId, 0);
      fClient->ProcessLine(fCommand, MK_MSG(kC_HSLIDER, kSL_RELEASE), fWidgetId, 0);
      Released();
      fMove = 0;

      gVirtualX->GrabPointer(0, 0, 0, 0, kFALSE);  // ungrab pointer
   } else
      fMove = 0;

   return kTRUE;
}

//______________________________________________________________________________
Bool_t TGDoubleHSlider::HandleMotion(Event_t *event)
{
   // Handle mouse motion event in horizontal slide widget.

   static Long_t was = gSystem->Now();
   Long_t now = (long)gSystem->Now();

   if ((now-was) < 50) return kTRUE;
   was = now;

   int     diff;
   Float_t oldMin, oldMax;

   diff    = event->fX - fPressPoint;
   oldMin  = fSmin;
   oldMax  = fSmax;

   if (fMove == 1) {
      // change of min value
      fSmin = fPressSmin + diff * (fVmax - fVmin) / (fWidth-16);
      if (fSmin < fVmin) fSmin = fVmin;
      if (fSmin > fSmax) fSmin = fSmax;
   } else if (fMove == 2) {
      // change of max value
      fSmax = fPressSmax + diff * (fVmax - fVmin) / (fWidth-16);
      if (fSmax > fVmax) fSmax = fVmax;
      if (fSmax < fSmin) fSmax = fSmin;
   } else if (fMove == 3) {
      // change of min and of max value
      Float_t logicalDiff;
      logicalDiff = diff * (fVmax - fVmin) / (fWidth-16);
      if (fPressSmax + logicalDiff > fVmax)
         logicalDiff = fVmax - fPressSmax;
      if (fPressSmin + logicalDiff < fVmin)
         logicalDiff = fVmin - fPressSmin;
      fSmax = fPressSmax + logicalDiff;
      fSmin = fPressSmin + logicalDiff;
   }

   // check if position has changed
   if (fMove != 0 && (fSmax != oldMax || fSmin != oldMin)) {
      fClient->NeedRedraw(this);
      SendMessage(fMsgWindow, MK_MSG(kC_HSLIDER, kSL_POS), fWidgetId, 0);
      fClient->ProcessLine(fCommand, MK_MSG(kC_HSLIDER, kSL_POS), fWidgetId, 0);
      PositionChanged();
   }
   return kTRUE;
}

//______________________________________________________________________________
void TGDoubleHSlider::SavePrimitive(ofstream &out, Option_t *option)
{
    // Save an horizontal slider as a C++ statement(s) on output stream out.

   SaveUserColor(out, option);

   out <<"   TGDoubleHSlider *";
   out << GetName() << " = new TGDoubleHSlider(" << fParent->GetName()
       << "," << GetWidth() << ",";
   out << GetSString() << "," << WidgetId() << ",";
   out << GetOptionString() << ",ucolor";
   if (fMarkEnds) {
      switch (fReversedScale) {
         case kTRUE:
            out << ",kTRUE,kTRUE);" << endl;
            break;
         case kFALSE:
            out << ",kFALSE,kTRUE);" << endl;
            break;
      }
   } else if (fReversedScale) {
      out << ",kTRUE);" << endl;
   } else {
      out << ");" << endl;
   }

   if (fVmin != 0 || fVmax != (Int_t)fWidth)
      out << "   " << GetName() << "->SetRange(" << fVmin << "," << fVmax
          << ");" << endl;

   if (fSmin != fWidth/8*3 || fSmax != fWidth/8*5)
      out << "   " << GetName() << "->SetPosition(" << GetMinPosition()
          << "," << GetMaxPosition() << ");" << endl;

   if (fScale != 10)
      out << "   " << GetName() << "->SetScale(" << fScale << ");" << endl;

}

//______________________________________________________________________________
void TGDoubleVSlider::SavePrimitive(ofstream &out, Option_t *option)
{
    // Save an horizontal slider as a C++ statement(s) on output stream out.
   
   SaveUserColor(out, option);

   out<<"   TGDoubleVSlider *";
   out << GetName() << " = new TGDoubleVSlider("<< fParent->GetName()
       << "," << GetHeight() << ",";
   out << GetSString() << "," << WidgetId() << ",";
   out << GetOptionString() << ",ucolor";
   if (fMarkEnds) {
      switch (fReversedScale) {
         case kTRUE:
            out << ",kTRUE,kTRUE);" << endl;
            break;
         case kFALSE:
            out << ",kFALSE,kTRUE);" << endl;
            break;
      }
   } else if (fReversedScale) {
      out << ",kTRUE);" << endl;
   } else {
      out << ");" << endl;
   }

   if (fVmin != 0 || fVmax != (Int_t)fHeight)
      out << "   " << GetName() <<"->SetRange(" << fVmin << "," << fVmax
          << ");" << endl;


   if (fSmin != fHeight/8*3 || fSmax != fHeight/8*5)
      out << "   " << GetName() << "->SetPosition(" << GetMinPosition()
          << "," << GetMaxPosition() << ");" << endl;


   if (fScale != 10)
      out << "   " << GetName() << "->SetScale(" << fScale << ");" << endl;

}
