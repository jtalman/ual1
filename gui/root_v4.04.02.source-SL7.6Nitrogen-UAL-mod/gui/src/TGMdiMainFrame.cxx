// @(#)root/gui:$Name:  $:$Id: TGMdiMainFrame.cxx,v 1.16 2005/01/12 18:39:29 brun Exp $
// Author: Bertrand Bellenot   20/08/2004

/*************************************************************************
 * Copyright (C) 1995-2004, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

/**************************************************************************

    This file is part of TGMdi an extension to the xclass toolkit.
    Copyright (C) 1998-2002 by Harald Radke, Hector Peraza.

    This application is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This application is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

**************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGMdiMainFrame.                                                      //
//                                                                      //
// This file contains the TGMdiMainFrame class.                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "KeySymbols.h"
#include "TGFrame.h"
#include "TGMdiMainFrame.h"
#include "TGMdiDecorFrame.h"
#include "TGMdiFrame.h"
#include "TGMdiMenu.h"
#include "TGGC.h"
#include "TGResourcePool.h"
#include "Riostream.h"

ClassImp(TGMdiMainFrame)
ClassImp(TGMdiContainer)
ClassImp(TGMdiGeometry)
ClassImp(TGMdiFrameList)

//______________________________________________________________________________
TGMdiMainFrame::TGMdiMainFrame(const TGWindow *p, TGMdiMenuBar *menuBar,
                               Int_t w, Int_t h, UInt_t options,
                               Pixel_t back) :
   TGCanvas(p, w, h, options | kDoubleBorder | kSunkenFrame | kMdiMainFrame, back)
{
   // Create a MDI main frame.

   fContainer = new TGMdiContainer(this, 10, 10, kOwnBackground,
                         fClient->GetShadow(GetDefaultFrameBackground()));
   TGCanvas::SetContainer(fContainer);

   fNumberOfFrames = 0;
   fMenuBar = menuBar;
   fChildren = 0;
   fCurrent = 0;
   fArrangementMode = 0;

   const TGResourcePool *res = GetResourcePool();
   fBackCurrent = res->GetSelectedBgndColor();
   fForeCurrent = res->GetSelectedFgndColor();
   fForeNotCurrent = res->GetFrameBgndColor();
   fBackNotCurrent = res->GetFrameShadowColor();
   fFontCurrent = (TGFont *)res->GetMenuFont();
   fFontNotCurrent = fFontCurrent;

   fBoxGC = new TGGC(*gClient->GetResourcePool()->GetFrameGC());
   fBoxGC->SetForeground(fForeNotCurrent);
   fBoxGC->SetBackground(fBackNotCurrent);
   fBoxGC->SetFunction(kGXxor);
   fBoxGC->SetLineWidth(TGMdiDecorFrame::kMdiBorderWidth-3);
   fBoxGC->SetSubwindowMode(kIncludeInferiors);
   fBoxGC->SetStipple(fClient->GetResourcePool()->GetCheckeredBitmap());
   fBoxGC->SetFillStyle(kFillOpaqueStippled);

   fCurrentX = fCurrentY = 0;
   fResizeMode = kMdiDefaultResizeMode;

   fWinListMenu = new TGPopupMenu(fClient->GetDefaultRoot());

   const TGMainFrame *main = (TGMainFrame *) GetMainFrame();
   if (main){
      Int_t keycode = gVirtualX->KeysymToKeycode(kKey_Tab);
      main->BindKey(this, keycode, kKeyControlMask);
      main->BindKey(this, keycode, kKeyControlMask | kKeyShiftMask);
      keycode = gVirtualX->KeysymToKeycode(kKey_F4);
      main->BindKey(this, keycode, kKeyControlMask);
   }

   MapSubwindows();
   Layout();
   MapWindow();
   SetWindowName();
}

//______________________________________________________________________________
TGMdiMainFrame::~TGMdiMainFrame()
{
   TGMdiFrameList *tmp, *travel = fChildren;

   while (travel) {
      tmp = travel->GetNext();
      delete travel;
      travel = tmp;
   }

   if (fFontCurrent) fClient->FreeFont((TGFont *)fFontCurrent);
   if (fFontNotCurrent != fFontCurrent) fClient->FreeFont((TGFont *)fFontNotCurrent);

   delete fBoxGC;

   if (!MustCleanup()) {

      const TGMainFrame *main = (TGMainFrame *) GetMainFrame();

      if (main) {
         Int_t keycode = gVirtualX->KeysymToKeycode(kKey_Tab);
         main->RemoveBind(this, keycode, kKeyControlMask);
         main->RemoveBind(this, keycode, kKeyControlMask | kKeyShiftMask);
         keycode = gVirtualX->KeysymToKeycode(kKey_F4);
         main->RemoveBind(this, keycode, kKeyControlMask);
      }
   }
}

//______________________________________________________________________________
void TGMdiMainFrame::SetResizeMode(Int_t mode)
{
   TGMdiFrameList *travel;

   fResizeMode = mode;
   for (travel = fChildren; travel; travel = travel->GetNext()) {
      travel->GetDecorFrame()->SetResizeMode(mode);
   }
}

//______________________________________________________________________________
Bool_t TGMdiMainFrame::HandleKey(Event_t *event)
{
   char   input[10];
   UInt_t keysym;

   if (event->fType == kGKeyPress) {
      gVirtualX->LookupString(event, input, sizeof(input), keysym);
      if ((EKeySym)keysym == kKey_Tab) {
         if (event->fState & kKeyControlMask) {
            if (event->fState & kKeyShiftMask) {
               CirculateUp();
            } else {
               CirculateDown();
            }
            return kTRUE;
         }
      } else if ((EKeySym)keysym == kKey_F4) {
         if (event->fState & kKeyControlMask) {
            Close(GetCurrent());
            return kTRUE;
         }
      }
   }
   return kFALSE;
}

//______________________________________________________________________________
void TGMdiMainFrame::AddMdiFrame(TGMdiFrame *frame)
{
   TGMdiFrameList *travel;

   frame->UnmapWindow();

   travel = new TGMdiFrameList;
   travel->SetCyclePrev(travel);
   travel->SetCycleNext(travel);
   travel->SetPrev(0);
   if (fChildren) fChildren->SetPrev(travel);
   travel->SetNext(fChildren);
   fChildren = travel;

   travel->SetDecorFrame(new TGMdiDecorFrame(this, frame, frame->GetWidth(),
                                       frame->GetHeight(), fBoxGC));

   travel->SetFrameId(frame->GetId());
   travel->GetDecorFrame()->SetResizeMode(fResizeMode);

   if (fCurrentX + travel->GetDecorFrame()->GetWidth() > fWidth) fCurrentX = 0;
   if (fCurrentY + travel->GetDecorFrame()->GetHeight() > fHeight) fCurrentY = 0;
   travel->GetDecorFrame()->Move(fCurrentX, fCurrentY);

   fCurrentX += travel->GetDecorFrame()->GetTitleBar()->GetHeight() + fBorderWidth * 2;
   fCurrentY += travel->GetDecorFrame()->GetTitleBar()->GetHeight() + fBorderWidth * 2;

   fNumberOfFrames++;

   UpdateWinListMenu();
   SetCurrent(travel);
   Layout();

   SendMessage(fParent, MK_MSG(kC_MDI, kMDI_CREATE), travel->GetDecorFrame()->GetId(), 0);
   FrameCreated(travel->GetDecorFrame()->GetId());
}

//______________________________________________________________________________
Bool_t TGMdiMainFrame::RemoveMdiFrame(TGMdiFrame *frame)
{
   TGMdiFrameList *travel = fChildren;

   if (frame && frame->IsEditable()) frame->SetEditable(kFALSE);

   while (travel && (travel->GetFrameId() != frame->GetId()))
      travel = travel->GetNext();
   if (!travel) return kFALSE;

   if (travel == fCurrent) fCurrent = 0;

   // unlink the element from the fCycle list
   travel->GetCyclePrev()->SetCycleNext(travel->GetCycleNext());
   travel->GetCycleNext()->SetCyclePrev(travel->GetCyclePrev());

   // and from the main list
   if (travel->GetNext()) {
      travel->GetNext()->SetPrev(travel->GetPrev());
   }
   if (travel->GetPrev()) {
      travel->GetPrev()->SetNext(travel->GetNext());
   } else {
      fChildren = travel->GetNext();
   }

   if (!fCurrent) {
      if (fChildren) SetCurrent(travel->GetCyclePrev());
   }

   travel->GetDecorFrame()->RemoveFrame(frame);

   UInt_t old_id = frame->GetId();

   delete travel->fDecor;

   fNumberOfFrames--;

   UpdateWinListMenu();
   Layout();

   SendMessage(fParent, MK_MSG(kC_MDI, kMDI_CLOSE), old_id, 0);
   FrameClosed(old_id);

   return kTRUE;
}

//______________________________________________________________________________
Bool_t TGMdiMainFrame::SetCurrent(UInt_t id)
{
   if (fCurrent && (fCurrent->GetDecorFrame()->GetId() == id)) {
      fCurrent->GetDecorFrame()->RaiseWindow();
      Emit("SetCurrent(TGMdiFrame*)", (long)fCurrent->GetDecorFrame()->GetMdiFrame());
      return kTRUE;
   }

   TGMdiFrameList *travel = fChildren;
   while (travel && (travel->GetDecorFrame()->GetId() != id)) travel = travel->GetNext();
   if (!travel) return kFALSE;

   return SetCurrent(travel);
}

//______________________________________________________________________________
Bool_t TGMdiMainFrame::SetCurrent(TGMdiFrame *f)
{
   if (fCurrent && (fCurrent->GetDecorFrame()->GetMdiFrame() == f)) {
      fCurrent->GetDecorFrame()->RaiseWindow();
      Emit("SetCurrent(TGMdiFrame*)", (long)fCurrent->GetDecorFrame()->GetMdiFrame());
      return kTRUE;
   }

   TGMdiFrameList *travel = fChildren;
   while (travel && (travel->GetDecorFrame()->GetMdiFrame() != f)) travel = travel->GetNext();
   if (!travel) return kFALSE;

   return SetCurrent(travel);
}

//______________________________________________________________________________
Bool_t TGMdiMainFrame::SetCurrent(TGMdiFrameList *newcurrent)
{
   if (fCurrent && (fCurrent == newcurrent)) {
      fCurrent->GetDecorFrame()->RaiseWindow();
      Emit("SetCurrent(TGMdiFrame*)", (long)fCurrent->GetDecorFrame()->GetMdiFrame());
      return kTRUE;
   }

   if (fCurrent) {
      if (!fCurrent->GetDecorFrame()->IsMaximized())
         fCurrent->GetDecorFrame()->GetTitleBar()->SetTitleBarColors(fForeNotCurrent,
                                                        fBackNotCurrent,
                                                        fFontNotCurrent);
   }

   if (newcurrent) {
      if (fCurrent) {
         // unlink the element from the old position
         newcurrent->GetCyclePrev()->SetCycleNext(newcurrent->GetCycleNext());
         newcurrent->GetCycleNext()->SetCyclePrev(newcurrent->GetCyclePrev());
         // and link it to the top of the window fCycle stack
         newcurrent->SetCyclePrev(fCurrent);
         newcurrent->SetCycleNext(fCurrent->GetCycleNext());
         fCurrent->SetCycleNext(newcurrent);
         newcurrent->GetCycleNext()->SetCyclePrev(newcurrent);
      } else {
         // no current? well, put it at the head of the list...
         if (fChildren && newcurrent != fChildren) {
            // unlink the element from the old position
            newcurrent->GetCyclePrev()->SetCycleNext(newcurrent->GetCycleNext());
            newcurrent->GetCycleNext()->SetCyclePrev(newcurrent->GetCyclePrev());
            // and link it to the beginning of the window list
            newcurrent->SetCyclePrev(fChildren);
            newcurrent->SetCycleNext(fChildren->GetCycleNext());
            fChildren->SetCycleNext(newcurrent);
            newcurrent->GetCycleNext()->SetCyclePrev(newcurrent);
         }
      }
   }

   fCurrent = newcurrent;

   if (fCurrent) {
      if (!fCurrent->GetDecorFrame()->IsMaximized())
         fCurrent->GetDecorFrame()->GetTitleBar()->SetTitleBarColors(fForeCurrent,
                                                        fBackCurrent,
                                                        fFontCurrent);
   }

   fCurrent->GetDecorFrame()->RaiseWindow();
   Emit("SetCurrent(TGMdiFrame*)", (long)fCurrent->GetDecorFrame()->GetMdiFrame());

   fWinListMenu->RCheckEntry(fCurrent->GetDecorFrame()->GetId(), 0, kMaxInt);

   return kTRUE;
}

//______________________________________________________________________________
void TGMdiMainFrame::CirculateUp()
{
   // Bring the lowest window to the top.

   if (fCurrent) {
      fCurrent->GetDecorFrame()->GetTitleBar()->SetTitleBarColors(fForeNotCurrent,
                                                     fBackNotCurrent,
                                                     fFontNotCurrent);

      fCurrent = fCurrent->GetCycleNext();

      fCurrent->GetDecorFrame()->RaiseWindow();
      fCurrent->GetDecorFrame()->GetTitleBar()->SetTitleBarColors(fForeCurrent,
                                                     fBackCurrent,
                                                     fFontCurrent);

   } else if (fChildren) {
      SetCurrent(fChildren);
   }
}

//______________________________________________________________________________
void TGMdiMainFrame::CirculateDown()
{
   // Send the highest window to the bottom.

   if (fCurrent) {
      fCurrent->GetDecorFrame()->LowerWindow();
      fCurrent->GetDecorFrame()->GetTitleBar()->SetTitleBarColors(fForeNotCurrent,
                                                     fBackNotCurrent,
                                                     fFontNotCurrent);

      fCurrent = fCurrent->GetCyclePrev();   // do not call SetCurrent in order
                                         // to not to alter the stacking order
      fCurrent->GetDecorFrame()->RaiseWindow();
      fCurrent->GetDecorFrame()->GetTitleBar()->SetTitleBarColors(fForeCurrent,
                                                     fBackCurrent,
                                                     fFontCurrent);

   } else if (fChildren) {
      SetCurrent(fChildren);
   }
}

//______________________________________________________________________________
TGMdiDecorFrame *TGMdiMainFrame::GetDecorFrame(TGMdiFrame *frame) const
{
   TGMdiFrameList *travel = fChildren;
   while (travel && (travel->GetDecorFrame()->GetMdiFrame() != frame))
      travel = travel->GetNext();
   if (!travel) return 0;
   return travel->GetDecorFrame();
}

//______________________________________________________________________________
TGMdiDecorFrame *TGMdiMainFrame::GetDecorFrame(UInt_t id) const
{
   TGMdiFrameList *travel = fChildren;
   while (travel && (travel->GetDecorFrame()->GetId() != id)) travel = travel->GetNext();
   if (!travel) return 0;
   return travel->GetDecorFrame();
}

//______________________________________________________________________________
TGMdiFrame *TGMdiMainFrame::GetMdiFrame(UInt_t id) const
{
   TGMdiDecorFrame *frame = GetDecorFrame(id);
   if (!frame) return 0;
   return frame->GetMdiFrame();
}

//______________________________________________________________________________
TGRectangle TGMdiMainFrame::GetBBox() const
{
   if (fCurrent && fCurrent->GetDecorFrame()->IsMaximized()) {
      return TGRectangle(0, 0, fWidth - 2 * fBorderWidth, fHeight - 2 * fBorderWidth);
   } else {
      TGRectangle rect;
      TGMdiFrameList *travel;

      for (travel = fChildren; travel; travel = travel->GetNext()) {
         TGRectangle wrect(travel->GetDecorFrame()->GetX(), travel->GetDecorFrame()->GetY(),
                          travel->GetDecorFrame()->GetWidth(), travel->GetDecorFrame()->GetHeight());
         rect.Merge(wrect);
      }
      return rect;
   }
}

//______________________________________________________________________________
TGRectangle TGMdiMainFrame::GetMinimizedBBox() const
{
   TGRectangle rect;
   TGMdiFrameList *travel;
   Int_t first = kTRUE;

   for (travel = fChildren; travel; travel = travel->GetNext()) {
      if (travel->GetDecorFrame()->IsMinimized()) {
         TGRectangle wrect(travel->GetDecorFrame()->GetX(), travel->GetDecorFrame()->GetY(),
                           travel->GetDecorFrame()->GetWidth(), travel->GetDecorFrame()->GetHeight());
         if (first) rect = wrect; else rect.Merge(wrect);
         first = kFALSE;
      }
   }
   return rect;
}

//______________________________________________________________________________
void TGMdiMainFrame::UpdateWinListMenu()
{
   char buf[256], scut;
   TGMdiFrameList *travel;
   const TGPicture *pic;

   TGMenuEntry *e;
   TIter fNext(fWinListMenu->GetListOfEntries());
   while ((e = (TGMenuEntry*)fNext())) {
      fWinListMenu->DeleteEntry(e);
   }
   scut = '0';

   if (!fChildren) {
      fWinListMenu->AddEntry(new TGHotString("(None)"), 1000);
      fWinListMenu->DisableEntry(1000);
      return;
   }

   for (travel = fChildren; travel; travel = travel->GetNext()) {
      scut++;
      if (scut == ('9' + 1)) scut = 'A';
      sprintf(buf, "&%c. %s", scut, travel->GetDecorFrame()->GetWindowName());
      if (travel->GetDecorFrame()->GetMdiButtons() & kMdiMenu)
         pic = travel->GetDecorFrame()->GetWindowIcon();
      else
         pic = 0;
      fWinListMenu->AddEntry(new TGHotString(buf), travel->GetDecorFrame()->GetId(), 0, pic);
   }

   if (fCurrent)
      fWinListMenu->RCheckEntry(fCurrent->GetDecorFrame()->GetId(), 0, kMaxInt);
}

//______________________________________________________________________________
void TGMdiMainFrame::Layout()
{
   TGCanvas::Layout();
   if (fCurrent && fCurrent->GetDecorFrame()->IsMaximized())
      fCurrent->GetDecorFrame()->MoveResize(0, 0, fWidth - 2 *fBorderWidth, fHeight -
                                   2 * fBorderWidth);
}

//______________________________________________________________________________
void TGMdiMainFrame::ArrangeFrames(Int_t mode)
{
   Int_t factor_x = 0;
   Int_t factor_y = 0;
   Int_t num_mapped = 0;
   Int_t x = 0;
   Int_t y = 0;
   Int_t w = fWidth - 2 * fBorderWidth;  //GetContainer()->GetWidth();
   Int_t h = fHeight - 2 * fBorderWidth;  //GetContainer()->GetHeight();
   
   fArrangementMode = mode;

   TGMdiFrameList *tmp, *travel;

   for (travel = fChildren; travel; travel = travel->GetNext()) {
      if (travel->GetDecorFrame()->IsMaximized())
         Restore(travel->GetDecorFrame()->GetMdiFrame());
      if (!travel->GetDecorFrame()->IsMinimized())
         ++num_mapped;
   }

   // must also restore view to 0,0
   GetViewPort()->SetHPos(0);
   GetViewPort()->SetVPos(0);

   ArrangeMinimized();

   travel = fChildren;

   if (num_mapped == 0) return;

   TGRectangle irect = GetMinimizedBBox();
   h -= irect.fH;

   switch (mode) {
      case kMdiTileHorizontal:
         factor_y = h / num_mapped;
         for (travel = fChildren; travel; travel = travel->GetNext()) {
             if (!travel->GetDecorFrame()->IsMinimized()) {
                travel->GetDecorFrame()->MoveResize(x, y, w, factor_y);
                y = y + factor_y;
             }
         }
         break;

      case kMdiTileVertical:
         factor_x = w / num_mapped;
         for (travel = fChildren; travel; travel = travel->GetNext()) {
            if (!travel->GetDecorFrame()->IsMinimized()) {
               travel->GetDecorFrame()->MoveResize(x, y, factor_x, h);
               x = x + factor_x;
            }
         }
         break;

      case kMdiCascade:
         y = travel->GetDecorFrame()->GetTitleBar()->GetX() +
             travel->GetDecorFrame()->GetTitleBar()->GetHeight();
         x = y;
         factor_y = (h * 2) / 3;
         factor_x = (w * 2) / 3;

         travel = fCurrent;
         if (!travel) travel = fChildren;
         tmp = travel;
         if (travel) {
            do {
               travel = travel->GetCycleNext();
               if (!travel->GetDecorFrame()->IsMinimized()) {
                  travel->GetDecorFrame()->MoveResize(x - y, x - y, factor_x, factor_y);
                  x += y;
               }
            } while (travel != tmp);
         }
         break;
   }

   FramesArranged(mode);

   Layout();
}

//______________________________________________________________________________
void TGMdiMainFrame::ArrangeMinimized()
{
   // This is an attempt to an "smart" minimized window re-arrangement.

   TGMdiFrameList *travel, *closest;
   Int_t x, y, w, h;

   Bool_t arranged = kTRUE;

   for (travel = fChildren; travel && arranged; travel = travel->GetNext())
      if (travel->GetDecorFrame()->IsMinimized()) arranged = kFALSE;

   // return if there is nothing to do

   if (arranged || !fChildren) return;

   h = fChildren->GetDecorFrame()->GetTitleBar()->GetDefaultHeight() +
       fChildren->GetDecorFrame()->GetBorderWidth();
   w = kMinimizedWidth * h + fChildren->GetDecorFrame()->GetBorderWidth();

   x = 0;
   y = GetViewPort()->GetHeight() - h;

   // we'll use the _minimizedUserPlacement variable as a "not arranged" flag

   for (travel = fChildren; travel; travel = travel->GetNext())
      travel->GetDecorFrame()->SetMinUserPlacement();

   do {
      closest = 0;
      Int_t cdist = 0;
      for (travel = fChildren; travel; travel = travel->GetNext()) {
         if (travel->GetDecorFrame()->IsMinimized()) {
            if (travel->GetDecorFrame()->GetMinUserPlacement()) {
               Int_t dx = travel->GetDecorFrame()->GetX() - x;
               Int_t dy = y - travel->GetDecorFrame()->GetY();
               Int_t dist = dx * dx + dy * dy;
               if (!closest || (dist < cdist)) {
                  closest = travel;
                  cdist = dist;
               }
            }
         }
      }

      if (closest) {
         closest->GetDecorFrame()->SetMinimizedX(x);
         closest->GetDecorFrame()->SetMinimizedY(y);
         closest->GetDecorFrame()->MoveResize(x, y, w, h);
         closest->GetDecorFrame()->SetMinUserPlacement(kFALSE);

         x += w;
         if (x + w > (Int_t)GetViewPort()->GetWidth()) {
            x = 0;
            y -= h;
         }
      }

   } while (closest);

   // reset the fMinimizedUserPlacement settings for all windows

   for (travel = fChildren; travel; travel = travel->GetNext())
      travel->GetDecorFrame()->SetMinUserPlacement(kFALSE);
}

//______________________________________________________________________________
Bool_t TGMdiMainFrame::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{

   switch (GET_MSG(msg)) {
      case kC_MDI:
         SetCurrent(parm1);
         switch (GET_SUBMSG(msg)) {

            case kMDI_MINIMIZE:
               Minimize(GetCurrent());
               break;

            case kMDI_MAXIMIZE:
               Maximize(GetCurrent());
               break;

            case kMDI_RESTORE:
               Restore(GetCurrent());
               break;

            case kMDI_CLOSE:
               Close(GetCurrent());
               break;

            case kMDI_MOVE:
               FreeMove(GetCurrent());
               break;

            case kMDI_SIZE:
               FreeSize(GetCurrent());
               break;

            case kMDI_HELP:
               ContextHelp(GetCurrent());
               break;
         }
         break;

      default:
         return TGCanvas::ProcessMessage(msg, parm1, parm2);
   }

   return kTRUE;
}

//______________________________________________________________________________
void TGMdiMainFrame::Maximize(TGMdiFrame *mdiframe)
{
   TGMdiDecorFrame *frame = GetDecorFrame(mdiframe);

   if (!frame) return;

   if (frame->IsMaximized()) return;

   if (frame->IsMinimized()) Restore(mdiframe);

   frame->SetDecorBorderWidth(0);
   frame->SetPreResizeX(frame->GetX());
   frame->SetPreResizeY(frame->GetY());
   frame->SetPreResizeWidth(frame->GetWidth());
   frame->SetPreResizeHeight(frame->GetHeight());
   frame->GetUpperHR()->UnmapWindow();
   frame->GetLowerHR()->UnmapWindow();
   frame->GetLeftVR()->UnmapWindow();
   frame->GetRightVR()->UnmapWindow();
   frame->GetUpperLeftCR()->UnmapWindow();
   frame->GetUpperRightCR()->UnmapWindow();
   frame->GetLowerLeftCR()->UnmapWindow();
   frame->GetLowerRightCR()->UnmapWindow();

   frame->MoveResize(fBorderWidth, fBorderWidth, fWidth - 2 *fBorderWidth,
       fHeight - 2 * fBorderWidth);
   frame->Maximize();
   frame->GetTitleBar()->LayoutButtons(frame->GetMdiButtons(), frame->IsMinimized(),
                                   frame->IsMaximized());
   frame->GetTitleBar()->RemoveFrames(frame->GetTitleBar()->GetWinIcon(),
                                  frame->GetTitleBar()->GetButtons());
   frame->HideFrame(frame->GetTitleBar());

   if (fMenuBar) {
      frame->GetTitleBar()->GetWinIcon()->SetBackgroundColor(GetDefaultFrameBackground());
      frame->GetTitleBar()->GetButtons()->SetBackgroundColor(GetDefaultFrameBackground());
      fMenuBar->AddFrames(frame->GetTitleBar()->GetWinIcon(),
                          frame->GetTitleBar()->GetButtons());
      fMenuBar->Layout();
   }

   SendMessage(fParent, MK_MSG(kC_MDI, kMDI_MAXIMIZE), frame->GetId(), 0);
   FrameMaximized(frame->GetId());

   Layout();
}

//______________________________________________________________________________
void TGMdiMainFrame::Restore(TGMdiFrame *mdiframe)
{
   TGMdiDecorFrame *frame = GetDecorFrame(mdiframe);

   if (!frame) return;

   if (frame->IsMinimized() == kFALSE && frame->IsMaximized() == kFALSE) return;

   if (frame->IsMinimized()) {
      frame->SetMinimizedX(frame->GetX());
      frame->SetMinimizedY(frame->GetY());
      frame->Minimize(kFALSE);
      frame->GetTitleBar()->SetTitleBarColors(fForeCurrent,
                                          fBackCurrent,
                                          fFontCurrent);
   } else if (frame->IsMaximized()) {
      frame->SetDecorBorderWidth(TGMdiDecorFrame::kMdiBorderWidth);
      frame->MapSubwindows();

      if (fMenuBar) {
         fMenuBar->RemoveFrames(frame->GetTitleBar()->GetWinIcon(),
                                frame->GetTitleBar()->GetButtons());
         fMenuBar->Layout();
      }

      frame->GetTitleBar()->AddFrames(frame->GetTitleBar()->GetWinIcon(),
                                      frame->GetTitleBar()->GetButtons());
      frame->GetTitleBar()->SetTitleBarColors(fForeCurrent, fBackCurrent,
                                              fFontCurrent);
      frame->ShowFrame(frame->GetTitleBar());
   }
   frame->Minimize(kFALSE);
   frame->Maximize(kFALSE);
   frame->GetTitleBar()->LayoutButtons(frame->GetMdiButtons(), kFALSE, kFALSE);
   frame->MoveResize(frame->GetPreResizeX(), frame->GetPreResizeY(),
                     frame->GetPreResizeWidth(), frame->GetPreResizeHeight());

   SendMessage(fParent, MK_MSG(kC_MDI, kMDI_RESTORE), frame->GetId(), 0);
   FrameRestored(frame->GetId());

   Layout();
}

//______________________________________________________________________________
void TGMdiMainFrame::Minimize(TGMdiFrame *mdiframe)
{
   Int_t x, y, w, h;
   TGMdiDecorFrame *frame = GetDecorFrame(mdiframe);

   if (!frame) return;

   if (frame->IsMinimized()) return;

   if (frame->IsMaximized()) Restore(mdiframe);

   frame->SetPreResizeX(frame->GetX());
   frame->SetPreResizeY(frame->GetY());
   frame->SetPreResizeWidth(frame->GetWidth());
   frame->SetPreResizeHeight(frame->GetHeight());

   h = frame->GetTitleBar()->GetDefaultHeight() + frame->GetBorderWidth();
   w = kMinimizedWidth * h + frame->GetBorderWidth();

   if (!frame->GetMinUserPlacement()) {

      x = 0;
      y = GetViewPort()->GetHeight() - h;

      while (1) {
         TGMdiFrameList *travel;
         Bool_t taken = kFALSE;

         // find an empty spot...
         for (travel = fChildren; travel; travel = travel->GetNext()) {
            if (travel->GetDecorFrame()->IsMinimized()) {
               TGPosition p(travel->GetDecorFrame()->GetX(),
                            travel->GetDecorFrame()->GetY());
               TGDimension s(travel->GetDecorFrame()->GetWidth(),
                             travel->GetDecorFrame()->GetHeight());
               if ((x <= p.fX + (Int_t) s.fWidth - 1) && (x + w - 1 >= p.fX) &&
                   (y <= p.fY + (Int_t) s.fHeight - 1) && (y + h - 1 >= p.fY)) {
                  taken = kTRUE;
                  break;
               }
            }
         }
         if (!taken) break;

         x += w;
         if (x + w > (Int_t)GetViewPort()->GetWidth()) {
            x = 0;
            y -= h;
         }
      }

      frame->SetMinimizedX(x);
      frame->SetMinimizedY(y);
   }

   frame->Minimize();

   frame->MoveResize(frame->GetMinimizedX(), frame->GetMinimizedY(), w, h);
   frame->LowerWindow();
   frame->GetTitleBar()->LayoutButtons(frame->GetMdiButtons(),
                                       frame->IsMinimized(),
                                       frame->IsMaximized());
   frame->Layout();

   SendMessage(fParent, MK_MSG(kC_MDI, kMDI_MINIMIZE), frame->GetId(), 0);
   FrameMinimized(frame->GetId());

   Layout();
}

//______________________________________________________________________________
Int_t TGMdiMainFrame::Close(TGMdiFrame *mdiframe)
{
   if (!mdiframe) return kFALSE;

   TGMdiDecorFrame *frame = GetDecorFrame(mdiframe);
   Restore(mdiframe);
   mdiframe->Emit("CloseWindow()");
   if (mdiframe->TestBit(kNotDeleted) && !mdiframe->TestBit(TGMdiFrame::kDontCallClose))
      return frame->CloseWindow();
   return kTRUE;
}

//______________________________________________________________________________
void TGMdiMainFrame::FreeMove(TGMdiFrame *mdiframe)
{
   TGMdiDecorFrame *frame = GetDecorFrame(mdiframe);
   if (!frame) return;

   Int_t x = frame->GetTitleBar()->GetWidth() / 2;
   Int_t y = frame->GetTitleBar()->GetHeight() - 1;// / 2;

   gVirtualX->Warp(x, y, frame->GetTitleBar()->GetId());

   frame->GetTitleBar()->SetLeftButPressed();
   frame->GetTitleBar()->SetX0(x);
   frame->GetTitleBar()->SetY0(y);
   Cursor_t cursor = gVirtualX->CreateCursor(kMove);
   gVirtualX->SetCursor(frame->GetTitleBar()->GetId(), cursor);

   gVirtualX->GrabPointer(frame->GetTitleBar()->GetId(),
                          kButtonReleaseMask | kPointerMotionMask,
                          kNone, cursor, kTRUE, kFALSE);
}

//______________________________________________________________________________
void TGMdiMainFrame::FreeSize(TGMdiFrame *mdiframe)
{
   TGMdiDecorFrame *frame = GetDecorFrame(mdiframe);
   if (!frame) return;

   Int_t x = frame->GetLowerRightCR()->GetWidth() - 5;
   Int_t y = frame->GetLowerRightCR()->GetHeight() - 5;

   Int_t xroot, yroot;
   Window_t win;

   gVirtualX->TranslateCoordinates(frame->GetLowerRightCR()->GetId(),
              fClient->GetDefaultRoot()->GetId(), x, y, xroot, yroot, win);

   gVirtualX->Warp(x, y, frame->GetLowerRightCR()->GetId());

   Event_t event;

   event.fType = kButtonPress;
   event.fWindow = frame->GetLowerRightCR()->GetId();
   event.fCode = kButton1;
   event.fX = x;
   event.fY = y;
   event.fXRoot = xroot;
   event.fYRoot = yroot;

   Cursor_t cursor = gVirtualX->CreateCursor(kBottomRight);
   gVirtualX->SetCursor(frame->GetLowerRightCR()->GetId(), cursor);

   gVirtualX->GrabPointer(frame->GetLowerRightCR()->GetId(),
               kButtonReleaseMask | kPointerMotionMask,
               kNone, cursor, kTRUE, kFALSE);

   frame->GetLowerRightCR()->HandleButton(&event);
}

//______________________________________________________________________________
Int_t TGMdiMainFrame::ContextHelp(TGMdiFrame *mdiframe)
{
   if (mdiframe)
      return mdiframe->Help();
   else
      return kFALSE;
}

//______________________________________________________________________________
TGMdiFrame *TGMdiMainFrame::GetCurrent() const
{
   if (fCurrent)
      return fCurrent->GetDecorFrame()->GetMdiFrame();
   else
      return 0;
}

//______________________________________________________________________________
TGMdiGeometry TGMdiMainFrame::GetWindowGeometry(TGMdiFrame *f) const
{
   TGMdiGeometry geom;

   geom.fValueMask = 0;

   const TGMdiDecorFrame *frame = GetDecorFrame(f);
   if (frame) {
      Int_t th = frame->GetTitleBar()->GetDefaultHeight();
      Int_t bw = frame->GetBorderWidth();

      if (frame->IsMinimized() || frame->IsMaximized()) {
         geom.fDecoration = TGRectangle(frame->GetPreResizeX(),
                                        frame->GetPreResizeY(),
                                        (unsigned) frame->GetPreResizeWidth(),
                                        (unsigned) frame->GetPreResizeHeight());
      } else {
         geom.fDecoration = TGRectangle(frame->GetX(),
                                        frame->GetY(),
                                        (unsigned) frame->GetWidth(),
                                        (unsigned) frame->GetHeight());
      }
      geom.fValueMask |= kMdiDecorGeometry;

      geom.fClient = TGRectangle(geom.fDecoration.fX + bw,
                                 geom.fDecoration.fY + bw + th,
                                 (unsigned) (geom.fDecoration.fW - 2 * bw),
                                 (unsigned) (geom.fDecoration.fH - 2 * bw - th));
      geom.fValueMask |= kMdiClientGeometry;

      if (frame->GetMinUserPlacement()) {
         Int_t mh = th + 2 * bw;
         Int_t mw = kMinimizedWidth * mh;

         geom.fIcon = TGRectangle(frame->GetMinimizedX(),
                                  frame->GetMinimizedY(),
                                  (unsigned) mw,
                                  (unsigned) mh);
         geom.fValueMask |= kMdiIconGeometry;
      }

   }

   return geom;
}

//______________________________________________________________________________
void TGMdiMainFrame::ConfigureWindow(TGMdiFrame *f, TGMdiGeometry &geom)
{
   TGMdiDecorFrame *frame = GetDecorFrame(f);
   if (frame) {
      if (geom.fValueMask & kMdiDecorGeometry) {
         if (frame->IsMinimized() || frame->IsMaximized()) {
            frame->SetPreResizeX(geom.fDecoration.fX);
            frame->SetPreResizeY(geom.fDecoration.fY);
            frame->SetPreResizeWidth(geom.fDecoration.fW);
            frame->SetPreResizeHeight(geom.fDecoration.fH);
         } else {
            frame->MoveResize(geom.fDecoration.fX, geom.fDecoration.fY,
                              geom.fDecoration.fW, geom.fDecoration.fH);
         }
      } else if (geom.fValueMask & kMdiClientGeometry) {

      }
      if (geom.fValueMask & kMdiIconGeometry) {
         frame->SetMinimizedX(geom.fIcon.fX);
         frame->SetMinimizedY(geom.fIcon.fY);
         frame->SetMinUserPlacement();
         if (frame->IsMinimized())
            frame->Move(frame->GetMinimizedX(), frame->GetMinimizedY());
      }
      Layout();
   }
}

//_____________________________________________________________________________
void TGMdiMainFrame::CloseAll()
{
   // close all mdi frames

   TGMdiFrameList *tmp, *travel = fChildren;
   TGMdiFrame *mdi;

   while (travel) {
      tmp = travel->GetNext();
      mdi = travel->GetDecorFrame()->GetMdiFrame();
      RemoveMdiFrame(mdi);
      travel = tmp;
   }
}

//______________________________________________________________________________
Bool_t TGMdiMainFrame::IsMaximized(TGMdiFrame *f)
{
   TGMdiDecorFrame *frame = GetDecorFrame(f);
   if (frame) return frame->IsMaximized();
   return kFALSE;
}

//______________________________________________________________________________
Bool_t TGMdiMainFrame::IsMinimized(TGMdiFrame *f)
{
   TGMdiDecorFrame *frame = GetDecorFrame(f);
   if (frame) return frame->IsMinimized();
   return kFALSE;
}

//______________________________________________________________________________
TGMdiContainer::TGMdiContainer(const TGMdiMainFrame *p, Int_t w, Int_t h,
                               UInt_t options, ULong_t back) :
  TGFrame(p->GetViewPort(), w, h, options, back)
{
   fMain = p;
   AddInput(kStructureNotifyMask);
}

//______________________________________________________________________________
TGDimension TGMdiContainer::GetDefaultSize() const
{
   TGRectangle rect = fMain->GetBBox();

   Int_t xpos = -fMain->GetViewPort()->GetHPos();
   Int_t ypos = -fMain->GetViewPort()->GetVPos();

   return TGDimension(TMath::Max(Int_t(xpos + fWidth), rect.RightBottom().fX + 1),
                      TMath::Max(Int_t(ypos + fHeight), rect.RightBottom().fY + 1));
}

//______________________________________________________________________________
Bool_t TGMdiContainer::HandleConfigureNotify(Event_t *event)
{
   if (event->fWindow != fId) {
      TGRectangle rect = fMain->GetBBox();

      Int_t vw = fMain->GetViewPort()->GetWidth();
      Int_t vh = fMain->GetViewPort()->GetHeight();

      Int_t w = TMath::Max(vw, rect.RightBottom().fX + 1);
      Int_t h = TMath::Max(vh, rect.RightBottom().fY + 1);

      if ((w != (Int_t)fWidth) || (h != (Int_t)fHeight)) {
         ((TGMainFrame *)fMain)->Layout();
         return kTRUE;
      }
   }
   return kFALSE;
}

//______________________________________________________________________________
void TGMdiMainFrame::SavePrimitive(ofstream &out, Option_t *option)
{
   // Save a MDI main frame as a C++ statement(s) on output stream out

   if (fBackground != GetDefaultFrameBackground()) SaveUserColor(out, option);

   out << endl << "   // MDI main frame" << endl;
   out << "   TGMdiMainFrame *";
   out << GetName() << " = new TGMdiMainFrame(" << fParent->GetName()
       << "," << GetMenu()->GetName() << "," << GetWidth() << "," << GetHeight();

   if (fBackground == GetDefaultFrameBackground()) {
      if (!GetOptions()) {
         out << ");" << endl;
      } else {
         out << "," << GetOptionString() <<");" << endl;
      }
   } else {
      out << "," << GetOptionString() << ",ucolor);" << endl;
   }

   TGMdiFrameList *travel=fChildren;
   travel->SetCycleNext(travel);
   for (travel = fChildren; travel; travel = travel->GetNext()) {
      TGMdiFrame *mf = travel->GetDecorFrame()->GetMdiFrame();
      if (mf) mf->SavePrimitive(out, option);
   } 
   if (fArrangementMode) {
      out << "   " << GetName() << "->ArrangeFrames(";
      switch (fArrangementMode) {
      
         case kMdiTileHorizontal:
            out << "kMdiTileHorizontal);" << endl;
         break;

         case kMdiTileVertical:
            out << "kMdiTileVertical);" << endl;
         break;

         case kMdiCascade:
            out << "kMdiCascade);" << endl;
         break;
      }
   }
   if (fResizeMode != kMdiOpaque)
      out << "   " << GetName() << "->SetResizeMode(kMdiNonOpaque);" << endl;

   if (fCurrent)
      out << "   " << GetName() << "->SetCurrent(" << GetCurrent()->GetName() 
          << ");" << endl;
}


