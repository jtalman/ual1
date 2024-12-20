// @(#)root/gui:$Name:  $:$Id: TGMenu.cxx,v 1.51 2005/04/06 06:09:26 brun Exp $
// Author: Fons Rademakers   09/01/98

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
// TGMenuBar, TGPopupMenu, TGMenuTitle and TGMenuEntry                  //
//                                                                      //
// This header contains all different menu classes.                     //
//                                                                      //
// Selecting a menu item will generate the event:                       //
// kC_COMMAND, kCM_MENU, menu id, user data.                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TGMenu.h"
#include "TGResourcePool.h"
#include "TTimer.h"
#include "TMath.h"
#include "TSystem.h"
#include "TList.h"
#include "Riostream.h"
#include "KeySymbols.h"

const TGGC   *TGPopupMenu::fgDefaultGC = 0;
const TGGC   *TGPopupMenu::fgDefaultSelectedGC = 0;
const TGGC   *TGPopupMenu::fgDefaultSelectedBackgroundGC = 0;
const TGFont *TGPopupMenu::fgDefaultFont = 0;
const TGFont *TGPopupMenu::fgHilightFont = 0;

const TGGC   *TGMenuTitle::fgDefaultGC = 0;
const TGGC   *TGMenuTitle::fgDefaultSelectedGC = 0;
const TGFont *TGMenuTitle::fgDefaultFont = 0;


ClassImp(TGMenuBar)
ClassImp(TGMenuTitle)
ClassImpQ(TGPopupMenu)


//______________________________________________________________________________
class TPopupDelayTimer : public TTimer {
private:
   TGPopupMenu   *fPopup;
public:
   TPopupDelayTimer(TGPopupMenu *p, Long_t ms) : TTimer(ms, kTRUE) { fPopup = p; }
   Bool_t Notify();
};

//______________________________________________________________________________
Bool_t TPopupDelayTimer::Notify()
{
   fPopup->HandleTimer(0);
   Reset();
   return kFALSE;
}


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGMenuBar member functions.                                          //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
TGMenuBar::TGMenuBar(const TGWindow *p, UInt_t w, UInt_t h, UInt_t options)
   : TGHorizontalFrame(p, w, h, options | kHorizontalFrame)
{
   // Create a menu bar object.

   fCurrent       = 0;
   fTitles        = new TList;
   fStick         = kTRUE;
   fDefaultCursor = fClient->GetResourcePool()->GetGrabCursor();
   fTrash         = new TList();

   gVirtualX->GrabButton(fId, kButton1, kAnyModifier,
                       kButtonPressMask | kButtonReleaseMask | kEnterWindowMask,
                       kNone, kNone);

   fKeyNavigate = kFALSE;
}

//______________________________________________________________________________
TGMenuBar::~TGMenuBar()
{
   // Delete menu bar object. Removes also the hot keys from the main frame,
   // so hitting them will not cause the menus to popup.

   TGFrameElement *el;
   TGMenuTitle    *t;
   Int_t           keycode;

   if (!MustCleanup()) {
      fTrash->Delete();
   }
   delete fTrash;

   const TGMainFrame *main = (TGMainFrame *)GetMainFrame();

   if (!MustCleanup()) {
      TIter next(fList);
      while ((el = (TGFrameElement *) next())) {
         t = (TGMenuTitle *) el->fFrame;
         if ((keycode = t->GetHotKeyCode()) != 0 && main) {
            main->RemoveBind(this, keycode, kKeyMod1Mask);
         }
      }
   }

   // delete TGMenuTitles
   if (fTitles && !MustCleanup()) fTitles->Delete();
   delete fTitles;
}


//______________________________________________________________________________
void TGMenuBar::BindKeys(Bool_t on)
{
   // If on kTRUE bind arrow, popup menu hot keys, otherwise
   // remove key bindings.

   gVirtualX->GrabKey(fId, gVirtualX->KeysymToKeycode(kKey_Left), 0, on);
   gVirtualX->GrabKey(fId, gVirtualX->KeysymToKeycode(kKey_Right), 0, on);
   gVirtualX->GrabKey(fId, gVirtualX->KeysymToKeycode(kKey_Up), 0, on);
   gVirtualX->GrabKey(fId, gVirtualX->KeysymToKeycode(kKey_Down), 0, on);
   gVirtualX->GrabKey(fId, gVirtualX->KeysymToKeycode(kKey_Enter), 0, on);
   gVirtualX->GrabKey(fId, gVirtualX->KeysymToKeycode(kKey_Return), 0, on);
   gVirtualX->GrabKey(fId, gVirtualX->KeysymToKeycode(kKey_Escape), 0, on);

   if (fCurrent && fCurrent->GetMenu()) {
      TGMenuEntry *e;
      TIter next(fCurrent->GetMenu()->GetListOfEntries());
      while ((e = (TGMenuEntry*)next())) {
         Int_t hot = 0;
         if (e->GetLabel()) {
            hot = e->GetLabel()->GetHotChar();
         }
         if (!hot) continue;
         gVirtualX->GrabKey(fId, gVirtualX->KeysymToKeycode(hot), 0, on);
      }
   }
}

//______________________________________________________________________________
void TGMenuBar::BindHotKey(Int_t keycode, Bool_t on)
{
   // If on kTRUE bind hot keys, otherwise remove key binding.

   const TGMainFrame *main = (TGMainFrame *) GetMainFrame();

   if (!main) return;

   if (on) {
      // case unsensitive bindings
      main->BindKey(this, keycode, kKeyMod1Mask);
      main->BindKey(this, keycode, kKeyMod1Mask | kKeyShiftMask);
      main->BindKey(this, keycode, kKeyMod1Mask | kKeyLockMask);
      main->BindKey(this, keycode, kKeyMod1Mask | kKeyShiftMask | kKeyLockMask);
   } else {
      main->RemoveBind(this, keycode, kKeyMod1Mask);
      main->RemoveBind(this, keycode, kKeyMod1Mask | kKeyShiftMask);
      main->RemoveBind(this, keycode, kKeyMod1Mask | kKeyLockMask);
      main->RemoveBind(this, keycode, kKeyMod1Mask | kKeyShiftMask | kKeyLockMask);
   }
}

//______________________________________________________________________________
void TGMenuBar::AddPopup(TGHotString *s, TGPopupMenu *menu, TGLayoutHints *l,
                         TGPopupMenu *before)
{
   // Add popup menu to menu bar. The hot string will be adopted by the
   // menubar (actually the menu title) and deleted when possible.
   // If before is not 0 the menu will be added before it.

   TGMenuTitle *t;
   Int_t keycode;

   AddFrameBefore(t = new TGMenuTitle(this, s, menu), l, before);
   fTitles->Add(t);  // keep track of menu titles for later cleanup in dtor

   if ((keycode = t->GetHotKeyCode()) != 0) {
      BindHotKey(keycode, kTRUE);
   }
}

//______________________________________________________________________________
void TGMenuBar::AddPopup(const char *s, TGPopupMenu *menu, TGLayoutHints *l,
                         TGPopupMenu *before)
{
   // Add popup menu to menu bar. If before is not 0 the menu will be
   // added before it.

   AddPopup(new TGHotString(s), menu, l, before);
}

//______________________________________________________________________________
TGPopupMenu *TGMenuBar::AddPopup(const TString &s, Int_t padleft, Int_t padright,
                                 Int_t padtop, Int_t padbottom)
{
   // Add popup menu to menu bar.
   //
   // Comment:
   //    This method is valid  only for horizontal menu bars.
   //    The most common case is menu bar containing equidistant titles padding left side.
   //       TGMenuBar *bar;
   //       bar->AddPopup("title1", 10);
   //       bar->AddPopup("title2", 10);
   //       ...
   //
   //    To add equidistant titles  padding right side padleft must be 0.
   //       TGMenuBar *bar;
   //       bar->AddPopup("title1", 0, 10);
   //       bar->AddPopup("title2", 0, 10);
   //       ...
   //
   //    This method guarantee automatic cleanup when menu bar is destroyed.
   //    Do not delete returned popup-menu

   ULong_t hints = kLHintsTop;

   if (padleft)  {
      hints |= kLHintsLeft;
   } else {
      hints |= kLHintsRight;
   }

   TGLayoutHints *l = new TGLayoutHints(hints, padleft, padright,
                                               padtop, padbottom);
   fTrash->Add(l);

   TGPopupMenu *menu = new TGPopupMenu(fClient->GetDefaultRoot());
   AddPopup(new TGHotString(s), menu, l, 0);
   fTrash->Add(menu);
   return menu;
}

//______________________________________________________________________________
void TGMenuBar::AddFrameBefore(TGFrame *f, TGLayoutHints *l,
                               TGPopupMenu *before)
{
   // Private version of AddFrame for menubar, to make sure that we
   // indeed only add TGMenuTitle objects to it. If before is not 0
   // the menu will be added before it.

   if (f->IsA() != TGMenuTitle::Class()) {
      Error("AddFrameBefore", "may only add TGMenuTitle objects to a menu bar");
      return;
   }

   if (!before) {
      AddFrame(f, l);
      return;
   }

   TGFrameElement *nw;

   nw = new TGFrameElement;
   nw->fFrame  = f;
   nw->fLayout = l ? l : fgDefaultHints;
   nw->fState  = 1;

   TGFrameElement *el;
   TIter next(fList);
   while ((el = (TGFrameElement *) next())) {
      TGMenuTitle *t = (TGMenuTitle *) el->fFrame;
      if (t->GetMenu() == before) {
         fList->AddBefore(el, nw);
         return;
      }
   }
   fList->Add(nw);
}

//______________________________________________________________________________
TGPopupMenu *TGMenuBar::GetPopup(const char *s)
{
   // Return popup menu with the specified name. Returns 0 if menu is
   // not found. Returnes menu can be used as "before" in AddPopup().
   // Don't use hot key (&) in name.

   if (!GetList()) return 0;

   TGFrameElement *el;
   TIter next(GetList());
   TString str = s;

   while ((el = (TGFrameElement *) next())) {
      TGMenuTitle *t = (TGMenuTitle *) el->fFrame;
      if (str == t->GetName())
         return t->GetMenu();
   }
   return 0;
}

//______________________________________________________________________________
TGPopupMenu *TGMenuBar::RemovePopup(const char *s)
{
   // Remove popup menu from menu bar. Returned menu has to be deleted by
   // the user, or can be re-used in another AddPopup(). Returns 0 if
   // menu is not found. Don't use hot key (&) in name.

   if (!GetList()) return 0;

   TGFrameElement *el;
   TIter next(GetList());
   TString str = s;

   while ((el = (TGFrameElement *) next())) {
      TGMenuTitle *t = (TGMenuTitle *) el->fFrame;
      if (str == t->GetName()) {
         Int_t keycode;
         if ((keycode = t->GetHotKeyCode())) {
            BindHotKey(keycode, kFALSE);  // remove bind
         }
         TGPopupMenu *m = t->GetMenu();
         fTitles->Remove(t);
         t->DestroyWindow();
         RemoveFrame(t);
         delete t;
         return m;
      }
   }
   return 0;
}

//______________________________________________________________________________
Bool_t TGMenuBar::HandleMotion(Event_t *event)
{
   // Handle a mouse motion event in a menu bar.

   if (fKeyNavigate) return kTRUE;

   Int_t        dummy;
   Window_t     wtarget;
   TGMenuTitle *target = 0;

   fStick = kFALSE; // use some threshold!

   gVirtualX->TranslateCoordinates(fId, fId, event->fX, event->fY,
                                   dummy, dummy, wtarget);
   if (wtarget) target = (TGMenuTitle*) fClient->GetWindowById(wtarget);

   if (fCurrent && target && (target != fCurrent)) {
      // deactivate all others
      TGFrameElement *el;
      TIter next(fList);
      while ((el = (TGFrameElement *) next()))
         ((TGMenuTitle*)el->fFrame)->SetState(kFALSE);

      fStick   = kTRUE;
      fCurrent = target;
      target->SetState(kTRUE);
   }

   return kTRUE;
}

//______________________________________________________________________________
Bool_t TGMenuBar::HandleButton(Event_t *event)
{
   // Handle a mouse button event in a menubar.

   Int_t        dummy;
   Window_t     wtarget;
   TGMenuTitle *target;

   // We don't need to check the button number as GrabButton will
   // only allow button1 events

   if (event->fType == kButtonPress) {

      gVirtualX->TranslateCoordinates(fId, fId, event->fX, event->fY,
                                      dummy, dummy, wtarget);
      target = (TGMenuTitle*) fClient->GetWindowById(wtarget);

      if (target != 0) {
         fStick = kTRUE;

         if (target != fCurrent) {
            // deactivate all others
            TGFrameElement *el;
            TIter next(fList);
            while ((el = (TGFrameElement *) next()))
               ((TGMenuTitle*)el->fFrame)->SetState(kFALSE);

            fStick   = kTRUE;
            fCurrent = target;
            target->SetState(kTRUE);

            gVirtualX->GrabPointer(fId, kButtonPressMask | kButtonReleaseMask |
                                   kPointerMotionMask, kNone, fDefaultCursor);
         }
      }
      fKeyNavigate = kFALSE;
   }

   if (event->fType == kButtonRelease) {
      if (fStick) {
         fStick = kFALSE;
         return kTRUE;
      }

      TGFrameElement *el;
      TIter next(fList);
      while ((el = (TGFrameElement *) next()))
         ((TGMenuTitle*)el->fFrame)->SetState(kFALSE);

      gVirtualX->GrabPointer(0, 0, 0, 0, kFALSE);  // ungrab pointer

      if (fCurrent != 0) {
         target   = fCurrent; // tricky, because WaitFor
         fCurrent = 0;
         target->DoSendMessage();
      }
   }

   return kTRUE;
}

//______________________________________________________________________________
Bool_t TGMenuBar::HandleKey(Event_t *event)
{
   // Handle keyboard events in a menu bar.

   TGMenuTitle *target = 0;
   TGFrameElement *el;
   void *dummy;
   TIter next(fList);

   if (event->fType == kGKeyPress) {
      UInt_t keysym;
      char tmp[2];

      gVirtualX->LookupString(event, tmp, sizeof(tmp), keysym);

      if (event->fState & kKeyMod1Mask) {
         while ((el = (TGFrameElement *) next())) {
            target = (TGMenuTitle *) el->fFrame;
            if ((Int_t)event->fCode == target->GetHotKeyCode()) {
               RequestFocus();
               break;
            }
         }
         if (el == 0) target = 0;
      } else {
         fKeyNavigate = kTRUE;

         if (fCurrent) {
            TGFrameElement *cur  = 0;
            TGPopupMenu    *menu = 0;
            next.Reset();
            el = 0;
            while ((el = (TGFrameElement *) next())) {
               if (el->fFrame == fCurrent) {
                  cur = el;
                  menu = ((TGMenuTitle*)el->fFrame)->GetMenu();
                  break;
               }
            }

            if (!menu || !menu->fPoppedUp) return kFALSE;

            TGMenuEntry *ce = 0;

            TIter next2(menu->GetListOfEntries());

            while ((ce = (TGMenuEntry*)next2())) {
               UInt_t hot = 0;
               if (ce->GetLabel()) hot = ce->GetLabel()->GetHotChar();
               if (!hot || (hot != keysym)) continue;

               menu->Activate(ce);
               gVirtualX->GrabPointer(0, 0, 0, 0, kFALSE);
               fCurrent->SetState(kFALSE);
               menu->fStick = kFALSE;
               Event_t ev;
               ev.fType = kButtonRelease;
               ev.fWindow = menu->GetId();
               fCurrent = 0;
               return menu->HandleButton(&ev);
            }

            ce = menu->GetCurrent();
            TGPopupMenu *submenu = 0;

            while (ce && (ce->GetType() == kMenuPopup)) {
               submenu = ce->GetPopup();
               if (!submenu->fPoppedUp) break;
               ce =  submenu->GetCurrent();
               menu = submenu;
            }
            switch ((EKeySym)keysym) {
               case kKey_Left:
                  if (submenu) {
                     submenu->EndMenu(dummy);
                     break;
                  }
                  el = (TGFrameElement*)fList->Before(cur);
                  if (!el) el = (TGFrameElement*)fList->Last();
                  break;
               case kKey_Right:
                  if (submenu) {
                     if (!submenu->GetCurrent()) {
                        ce = (TGMenuEntry*)submenu->GetListOfEntries()->First();
                     } else {
                        submenu->EndMenu(dummy);
                     }
                     break;
                  }
                  el = (TGFrameElement*)fList->After(cur);
                  if (!el) el = (TGFrameElement*)fList->First();
                  break;
               case kKey_Up:
                  if (ce) ce = (TGMenuEntry*)menu->GetListOfEntries()->Before(ce);
                  while (ce && ((ce->GetType() == kMenuSeparator) ||
                         (ce->GetType() == kMenuLabel) ||
                         !(ce->GetStatus() & kMenuEnableMask))) {
                     ce = (TGMenuEntry*)menu->GetListOfEntries()->Before(ce);
                  }
                  if (!ce) ce = (TGMenuEntry*)menu->GetListOfEntries()->Last();
                  break;
               case kKey_Down:
                  if (ce) ce = (TGMenuEntry*)menu->GetListOfEntries()->After(ce);
                  while (ce && ((ce->GetType() == kMenuSeparator) ||
                         (ce->GetType() == kMenuLabel) ||
                         !(ce->GetStatus() & kMenuEnableMask))) {
                     ce = (TGMenuEntry*)menu->GetListOfEntries()->After(ce);
                  }
                  if (!ce) ce = (TGMenuEntry*)menu->GetListOfEntries()->First();
                  break;
               case kKey_Enter:
               case kKey_Return:
                  gVirtualX->GrabPointer(0, 0, 0, 0, kFALSE);
                  fCurrent->SetState(kFALSE);
                  menu->fStick = kFALSE;
                  Event_t ev;
                  ev.fType = kButtonRelease;
                  ev.fWindow = menu->GetId();
                  fCurrent = 0;
                  return menu->HandleButton(&ev);
               case kKey_Escape:
                  gVirtualX->GrabPointer(0, 0, 0, 0, kFALSE);
                  fCurrent->SetState(kFALSE);
                  fStick = kFALSE;
                  fCurrent = 0;
                  return menu->EndMenu(dummy);
               default:
                  break;
            }
            if (ce) menu->Activate(ce);

            el = el ? el : cur;
            if (el) target = (TGMenuTitle*)el->fFrame;
         } else {
            return kFALSE;
         }
      }

      if (target != 0) {
         fStick = kTRUE;

         if (target != fCurrent) {
            // deactivate all others
            next.Reset();
            while ((el = (TGFrameElement *) next()))
               ((TGMenuTitle*)el->fFrame)->SetState(kFALSE);

            fCurrent = target;
            target->SetState(kTRUE);
            fStick   = kTRUE;

            gVirtualX->GrabPointer(fId, kButtonPressMask | kButtonReleaseMask |
                                   kPointerMotionMask, kNone, fDefaultCursor);
            return kTRUE;
         }
      } else {
         return kFALSE;
      }
   }

   if (event->fType == kKeyRelease) {
      if (fStick) {
         fStick = kFALSE;
         return kTRUE;
      }
      gVirtualX->GrabPointer(0, 0, 0, 0, kFALSE);  // ungrab pointer

      next.Reset();
      while ((el = (TGFrameElement *) next()))
         ((TGMenuTitle*)el->fFrame)->SetState(kFALSE);

      if (fCurrent != 0) {
         target   = fCurrent; // tricky, because WaitFor
         fCurrent = 0;
         target->DoSendMessage();
      }
   }

   return kTRUE;
}


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGPopupMenu member functions.                                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
TGPopupMenu::TGPopupMenu(const TGWindow *p, UInt_t w, UInt_t h, UInt_t options)
    : TGFrame(p, w, h, options | kOwnBackground)
{
   // Create a popup menu.

   fNormGC        = GetDefaultGC()();
   fSelGC         = GetDefaultSelectedGC()();
   fSelbackGC     = GetDefaultSelectedBackgroundGC()();
   fFontStruct    = GetDefaultFontStruct();
   fHifontStruct  = GetHilightFontStruct();
   fDefaultCursor = fClient->GetResourcePool()->GetGrabCursor();

   // We need to change the default context to actually use the
   // Menu Fonts.  [Are we actually changing the global settings?]
   GCValues_t    gcval;
   gcval.fMask = kGCFont;
   gcval.fFont = gVirtualX->GetFontHandle(fFontStruct);
   gVirtualX->ChangeGC(fNormGC, &gcval);
   gVirtualX->ChangeGC(fSelGC, &gcval);

   fDelay     = 0;
   fEntryList = new TList;

   // in case any of these magic values is changes, check also Reposition()
   fBorderWidth = 3;
   fMenuHeight  = 6;
   fMenuWidth   = 8;
   fXl          = 16;
   fMsgWindow   = p;
   fStick       = kTRUE;
   fCurrent     = 0;
   fHasGrab     = kFALSE;
   fPoppedUp    = kFALSE;
   fMenuBar     = 0;

   SetWindowAttributes_t wattr;
   wattr.fMask             = kWAOverrideRedirect | kWASaveUnder;
   wattr.fOverrideRedirect = kTRUE;
   wattr.fSaveUnder        = kTRUE;

   gVirtualX->ChangeWindowAttributes(fId, &wattr);

   AddInput(kPointerMotionMask | kEnterWindowMask | kLeaveWindowMask);
}

//______________________________________________________________________________
TGPopupMenu::~TGPopupMenu()
{
   // Delete a popup menu.

   if (fEntryList) fEntryList->Delete();
   delete fEntryList;
   delete fDelay;
}

//______________________________________________________________________________
void TGPopupMenu::AddEntry(TGHotString *s, Int_t id, void *ud,
                           const TGPicture *p, TGMenuEntry *before)
{
   // Add a menu entry. The hotstring is adopted by the menu (actually by
   // the TGMenuEntry) and deleted when possible. A possible picture is
   // borrowed from the picture pool and therefore not adopted.
   // If before is not 0, the entry will be added before it.

   TGMenuEntry *nw = new TGMenuEntry;

   nw->fLabel    = s;
   nw->fPic      = p;
   nw->fType     = kMenuEntry;
   nw->fEntryId  = id;
   nw->fUserData = ud;
   nw->fPopup    = 0;
   nw->fStatus   = kMenuEnableMask;
   nw->fEx       = 2;
   nw->fEy       = fMenuHeight-2;

   if (before)
      fEntryList->AddBefore(before, nw);
   else
      fEntryList->Add(nw);

   UInt_t tw, pw = 0;
   tw = gVirtualX->TextWidth(fHifontStruct, s->GetString(), s->GetLength());
   if (p) {
      pw = p->GetWidth();
      if (pw+12 > fXl) { fMenuWidth += pw+12-fXl; fXl = pw+12; }
   }

   Int_t max_ascent, max_descent;
   nw->fEw = tw + pw /*+8*/+18+12;
   fMenuWidth = TMath::Max(fMenuWidth, nw->fEw);
   gVirtualX->GetFontProperties(fHifontStruct, max_ascent, max_descent);
   nw->fEh = max_ascent + max_descent + 3;
   fMenuHeight += nw->fEh;

   if (before)
      Reposition();
   else
      Resize(fMenuWidth, fMenuHeight);
}

//______________________________________________________________________________
void TGPopupMenu::AddEntry(const char *s, Int_t id, void *ud,
                           const TGPicture *p, TGMenuEntry *before)
{
   // Add a menu entry. The string s in not adopted.
   // If before is not 0, the entry will be added before it.

   AddEntry(new TGHotString(s), id, ud, p, before);
}

//______________________________________________________________________________
void TGPopupMenu::AddSeparator(TGMenuEntry *before)
{
   // Add a menu separator to the menu.
   // If before is not 0, the entry will be added before it.

   TGMenuEntry *nw = new TGMenuEntry;

   nw->fLabel    = 0;
   nw->fPic      = 0;
   nw->fType     = kMenuSeparator;
   nw->fEntryId  = -1;
   nw->fUserData = 0;
   nw->fPopup    = 0;
   nw->fStatus   = kMenuEnableMask;
   nw->fEx       = 2;
   nw->fEy       = fMenuHeight-2;

   if (before)
      fEntryList->AddBefore(before, nw);
   else
      fEntryList->Add(nw);

   nw->fEw = 0;
   nw->fEh = 4;
   fMenuHeight += nw->fEh;

   if (before)
      Reposition();
   else
      Resize(fMenuWidth, fMenuHeight);
}

//______________________________________________________________________________
void TGPopupMenu::AddLabel(TGHotString *s, const TGPicture *p,
                           TGMenuEntry *before)
{
   // Add a menu label to the menu. The hotstring is adopted by the menu
   // (actually by the TGMenuEntry) and deleted when possible. A possible
   // picture is borrowed from the picture pool and therefore not adopted.
   // If before is not 0, the entry will be added before it.

   TGMenuEntry *nw = new TGMenuEntry;

   nw->fLabel    = s;
   nw->fPic      = p;
   nw->fType     = kMenuLabel;
   nw->fEntryId  = -1;
   nw->fUserData = 0;
   nw->fPopup    = 0;
   nw->fStatus   = kMenuEnableMask | kMenuDefaultMask;
   nw->fEx       = 2;
   nw->fEy       = fMenuHeight-2;

   if (before)
      fEntryList->AddBefore(before, nw);
   else
      fEntryList->Add(nw);

   UInt_t tw, pw = 0;
   tw = gVirtualX->TextWidth(fHifontStruct, s->GetString(), s->GetLength());
   if (p) {
      pw = p->GetWidth();
      if (pw+12 > fXl) { fMenuWidth += pw+12-fXl; fXl = pw+12; }
   }

   Int_t max_ascent, max_descent;
   nw->fEw = tw + pw /*+8*/+18+12;
   fMenuWidth = TMath::Max(fMenuWidth, nw->fEw);
   gVirtualX->GetFontProperties(fHifontStruct, max_ascent, max_descent);
   nw->fEh = max_ascent + max_descent + 3;
   fMenuHeight += nw->fEh;

   if (before)
      Reposition();
   else
      Resize(fMenuWidth, fMenuHeight);
}

//______________________________________________________________________________
void TGPopupMenu::AddLabel(const char *s, const TGPicture *p,
                           TGMenuEntry *before)
{
   // Add a menu label to the menu. The string s in not adopted.
   // If before is not 0, the entry will be added before it.

   AddLabel(new TGHotString(s), p, before);
}

//______________________________________________________________________________
void TGPopupMenu::AddPopup(TGHotString *s, TGPopupMenu *popup,
                           TGMenuEntry *before)
{
   // Add a (cascading) popup menu to a popup menu. The hotstring is adopted
   // by the menu (actually by the TGMenuEntry) and deleted when possible.
   // If before is not 0, the entry will be added before it.

   TGMenuEntry *nw = new TGMenuEntry;

   nw->fLabel    = s;
   nw->fPic      = 0;
   nw->fType     = kMenuPopup;
   nw->fEntryId  = -2;
   nw->fUserData = 0;
   nw->fPopup    = popup;
   nw->fStatus   = kMenuEnableMask;
   nw->fEx       = 2;
   nw->fEy       = fMenuHeight-2;

   if (before)
      fEntryList->AddBefore(before, nw);
   else
      fEntryList->Add(nw);

   UInt_t tw = gVirtualX->TextWidth(fHifontStruct, s->GetString(),
                                    s->GetLength());

   Int_t max_ascent, max_descent;
   nw->fEw = tw +8+18+12;
   fMenuWidth = TMath::Max(fMenuWidth, nw->fEw);
   gVirtualX->GetFontProperties(fHifontStruct, max_ascent, max_descent);
   nw->fEh = max_ascent + max_descent + 3;
   fMenuHeight += nw->fEh;

   if (before)
      Reposition();
   else
      Resize(fMenuWidth, fMenuHeight);
}

//______________________________________________________________________________
void TGPopupMenu::AddPopup(const char *s, TGPopupMenu *popup,
                           TGMenuEntry *before)
{
   // Add a (cascading) popup menu to a popup menu. The string s is not
   // adopted. If before is not 0, the entry will be added before it.

   AddPopup(new TGHotString(s), popup, before);
}

//______________________________________________________________________________
void TGPopupMenu::Reposition()
{
   // Reposition entries in popup menu. Called after menu item has been
   // hidden or removed or inserted at a specified location.

   // in case any of these magic values is changes, check also the ctor.
   fMenuHeight = 6;
   fMenuWidth  = 8;
   fXl         = 16;

   TGMenuEntry *ptr;
   TIter next(fEntryList);

   while ((ptr = (TGMenuEntry *) next())) {

      if (ptr->fStatus & kMenuHideMask) continue;

      if (ptr->fPic) {
         UInt_t pw = ptr->fPic->GetWidth();
         if (pw+12 > fXl) { fMenuWidth += pw+12-fXl; fXl = pw+12; }
      }
      ptr->fEx     = 2;
      ptr->fEy     = fMenuHeight-2;
      fMenuWidth   = TMath::Max(fMenuWidth, ptr->fEw);
      fMenuHeight += ptr->fEh;
   }
   Resize(fMenuWidth, fMenuHeight);
}

//______________________________________________________________________________
void TGPopupMenu::PlaceMenu(Int_t x, Int_t y, Bool_t stick_mode, Bool_t grab_pointer)
{
   // Popup a popup menu. If stick mode is true keep the menu up. If
   // grab_pointer is true the pointer will be grabbed, which means that
   // all pointer events will go to the popup menu, independent of in
   // which window the pointer is.

   Int_t  rx, ry;
   UInt_t rw, rh;

   fStick = stick_mode;
   fCurrent = 0;

   // Parent is root window for a popup menu
   gVirtualX->GetWindowSize(fParent->GetId(), rx, ry, rw, rh);

   if (x < 0) x = 0;
   if (x + fMenuWidth > rw) x = rw - fMenuWidth;
   if (y < 0) y = 0;
   if (y + fMenuHeight > rh) y = rh - fMenuHeight;

   Move(x, y);
   MapRaised();

   if (grab_pointer) {
      gVirtualX->GrabPointer(fId, kButtonPressMask | kButtonReleaseMask |
                             kPointerMotionMask, kNone, fDefaultCursor);
      fHasGrab = kTRUE;
   } else {
      fHasGrab = kFALSE;
   }

   fPoppedUp = kTRUE;
   PoppedUp();
   if (fMenuBar) fMenuBar->BindKeys(kTRUE);

   fClient->RegisterPopup(this);
}

//______________________________________________________________________________
Int_t TGPopupMenu::EndMenu(void *&userData)
{
   // Close menu and return ID of selected menu item.
   // In case of cascading menus, recursively close all menus.

   Int_t id;

   if (fDelay) fDelay->Remove();

   // destroy any cascaded childs and get any ID

   if (fCurrent != 0) {

      // deactivate the entry
      fCurrent->fStatus &= ~kMenuActiveMask;

      if (fCurrent->fType == kMenuPopup) {
         id = fCurrent->fPopup->EndMenu(userData);
      } else {
         // return the ID if the entry is enabled, otherwise -1
         if (fCurrent->fStatus & kMenuEnableMask) {
            id       = fCurrent->fEntryId;
            userData = fCurrent->fUserData;
         } else {
            id       = -1;
            userData = 0;
         }
      }

   } else {
      // if no entry selected...
      id       = -1;
      userData = 0;
   }

   // then unmap itself
   UnmapWindow();

   gClient->UnregisterPopup(this);
   if (fMenuBar) fMenuBar->BindKeys(kFALSE);

   if (fPoppedUp) {
      fPoppedUp = kFALSE;
      PoppedDown();
   }

   return id;
}

//______________________________________________________________________________
Bool_t TGPopupMenu::HandleButton(Event_t *event)
{
   // Handle button event in the popup menu.

   int   id;
   void *ud;

   if (event->fType == kButtonRelease) {
      if (fStick) {
         fStick = kFALSE;
         return kTRUE;
      }
      //if (fCurrent != 0)
      //   if (fCurrent->fType == kMenuPopup) return kTRUE;
      id = EndMenu(ud);
      if (fHasGrab) gVirtualX->GrabPointer(0, 0, 0, 0, kFALSE);  // ungrab
      if (fCurrent != 0) {
         fCurrent->fStatus &= ~kMenuActiveMask;
         if (fCurrent->fStatus & kMenuEnableMask) {
            SendMessage(fMsgWindow, MK_MSG(kC_COMMAND, kCM_MENU), id,
                        (Long_t)ud);
            Activated(id);
         }
      }
   }
   return kTRUE;
}

//______________________________________________________________________________
Bool_t TGPopupMenu::HandleCrossing(Event_t *event)
{
   // Handle pointer crossing event in popup menu.

   if (event->fType == kEnterNotify) {

      TGMenuEntry *ptr;
      TIter next(fEntryList);

      while ((ptr = (TGMenuEntry *) next())) {
         if (ptr->fStatus & kMenuHideMask) continue;

         if ((event->fX >= ptr->fEx) && (event->fX <= ptr->fEx+(Int_t)fMenuWidth-10) &&
             (event->fY >= ptr->fEy) && (event->fY <= ptr->fEy+(Int_t)ptr->fEh))
            break;
      }
      Activate(ptr);
   } else {
      Activate((TGMenuEntry*)0);
   }

   return kTRUE;
}

//______________________________________________________________________________
Bool_t TGPopupMenu::HandleMotion(Event_t *event)
{
   // Handle pointer motion event in popup menu.

   TGFrame::HandleMotion(event);

   TGMenuEntry *ptr;
   TIter next(fEntryList);

   fStick = kFALSE;   // be careful with this, use some threshold
   while ((ptr = (TGMenuEntry *) next())) {
      if (ptr->fStatus & kMenuHideMask) continue;

      if ((event->fX >= ptr->fEx) && (event->fX <= ptr->fEx+(Int_t)fMenuWidth-4) &&  //fMenuWidth-10??
          (event->fY >= ptr->fEy) && (event->fY <= ptr->fEy+(Int_t)ptr->fEh))
         break;
   }
   Activate(ptr);

   return kTRUE;
}

//______________________________________________________________________________
void TGPopupMenu::Activate(TGMenuEntry *entry)
{
   // Activate a menu entry in a popup menu.

   if (entry == fCurrent) return;

   //-- Deactivate the current entry

   if (fCurrent != 0) {
      void *ud;
      if (entry == 0 && fCurrent->fType == kMenuPopup) return;
      if (fCurrent->fType == kMenuPopup) fCurrent->fPopup->EndMenu(ud);
      fCurrent->fStatus &= ~kMenuActiveMask;
      DrawEntry(fCurrent);
   }

   if (fDelay) fDelay->Remove();

   //-- Activate the new one

   if (entry) {
      entry->fStatus |= kMenuActiveMask;
      DrawEntry(entry);
      if (entry->fType == kMenuPopup) {
         if (!fDelay) fDelay = new TPopupDelayTimer(this, 350);
         fDelay->Reset();
         gSystem->AddTimer(fDelay);
         // after delay expires it will popup the cascading popup menu
         // iff it is still the current entry
      } else if (entry->fType == kMenuEntry) {
         // test...
         SendMessage(fMsgWindow, MK_MSG(kC_COMMAND, kCM_MENUSELECT),
                     entry->fEntryId, (Long_t)entry->fUserData);
         Highlighted(entry->fEntryId);
      }
   }
   fCurrent = entry;
}

//______________________________________________________________________________
Bool_t TGPopupMenu::HandleTimer(TTimer *)
{
   // If TPopupDelayTimer times out popup cascading popup menu (if it is
   // still the current entry).

   if (fCurrent != 0) {
      if (fCurrent->fType == kMenuPopup) {
         Int_t    ax, ay;
         Window_t wdummy;

         gVirtualX->TranslateCoordinates(fId,
                                       (fCurrent->fPopup->GetParent())->GetId(),
                                       fCurrent->fEx+fMenuWidth, fCurrent->fEy,
                                       ax, ay, wdummy);

         fCurrent->fPopup->PlaceMenu(ax-5, ay-1, kFALSE, kFALSE);
      }
   }
   fDelay->Remove();

   return kTRUE;
}

//______________________________________________________________________________
void TGPopupMenu::DoRedraw()
{
   // Draw popup menu.

   TGFrame::DoRedraw();

   TGMenuEntry *ptr;
   TIter next(fEntryList);

   while ((ptr = (TGMenuEntry *) next()))
      DrawEntry(ptr);
}

//______________________________________________________________________________
void TGPopupMenu::DrawEntry(TGMenuEntry *entry)
{
   // Draw popup menu entry.

   FontStruct_t  font;
   GCValues_t    gcval;

   if (entry->fStatus & kMenuHideMask)
      return;

   if (entry->fStatus & kMenuDefaultMask) {
      font = fHifontStruct;
      gcval.fMask = kGCFont;
      gcval.fFont = gVirtualX->GetFontHandle(font);
      gVirtualX->ChangeGC(fNormGC, &gcval);
      gVirtualX->ChangeGC(fSelGC, &gcval);
   } else {
      font = fFontStruct;
   }

   int max_ascent, max_descent;
   gVirtualX->GetFontProperties(font, max_ascent, max_descent);
   int tx = entry->fEx + fXl;
   int ty = entry->fEy + max_ascent;

   switch (entry->fType) {
      case kMenuPopup:
      case kMenuLabel:
      case kMenuEntry:
         if ((entry->fStatus & kMenuActiveMask) && entry->fType != kMenuLabel) {
            gVirtualX->FillRectangle(fId, fSelbackGC, entry->fEx+1, entry->fEy-1,
                                     fMenuWidth-6, max_ascent + max_descent + 3);
            if (entry->fType == kMenuPopup)
               DrawTrianglePattern(fSelGC, fMenuWidth-10, entry->fEy+3, fMenuWidth-6, entry->fEy+11);
            if (entry->fStatus & kMenuCheckedMask)
               DrawCheckMark(fSelGC, 6, entry->fEy+3, 14, entry->fEy+11);
            if (entry->fStatus & kMenuRadioMask)
               DrawRCheckMark(fSelGC, 6, entry->fEy+3, 14, entry->fEy+11);
            if (entry->fPic != 0)
               entry->fPic->Draw(fId, fSelGC, 8, entry->fEy+1);
            entry->fLabel->Draw(fId,
                           (entry->fStatus & kMenuEnableMask) ? fSelGC : GetShadowGC()(),
                           tx, ty);
         } else {
            gVirtualX->FillRectangle(fId, GetBckgndGC()(), entry->fEx+1, entry->fEy-1,
                                     fMenuWidth-6, max_ascent + max_descent + 3);
            if (entry->fType == kMenuPopup)
               DrawTrianglePattern(fNormGC, fMenuWidth-10, entry->fEy+3, fMenuWidth-6, entry->fEy+11);
            if (entry->fStatus & kMenuCheckedMask)
               DrawCheckMark(fNormGC, 6, entry->fEy+3, 14, entry->fEy+11);
            if (entry->fStatus & kMenuRadioMask)
               DrawRCheckMark(fNormGC, 6, entry->fEy+3, 14, entry->fEy+11);
            if (entry->fPic != 0)
               entry->fPic->Draw(fId, fNormGC, 8, entry->fEy+1);
            if (entry->fStatus & kMenuEnableMask) {
               entry->fLabel->Draw(fId, fNormGC, tx, ty);
            } else {
               entry->fLabel->Draw(fId, GetHilightGC()(), tx+1, ty+1);
               entry->fLabel->Draw(fId, GetShadowGC()(), tx, ty);
            }
         }
         break;

      case kMenuSeparator:
         gVirtualX->DrawLine(fId, GetShadowGC()(),  2, entry->fEy, fMenuWidth-3, entry->fEy);
         gVirtualX->DrawLine(fId, GetHilightGC()(), 2, entry->fEy+1, fMenuWidth-3, entry->fEy+1);
         break;
   }

   // restore font
   if (entry->fStatus & kMenuDefaultMask) {
      gcval.fFont = gVirtualX->GetFontHandle(fFontStruct);
      gVirtualX->ChangeGC(fNormGC, &gcval);
      gVirtualX->ChangeGC(fSelGC, &gcval);
   }
}

//______________________________________________________________________________
void TGPopupMenu::DrawBorder()
{
   // Draw border round popup menu.

   gVirtualX->DrawLine(fId, GetBckgndGC()(), 0, 0, fMenuWidth-2, 0);
   gVirtualX->DrawLine(fId, GetBckgndGC()(), 0, 0, 0, fMenuHeight-2);
   gVirtualX->DrawLine(fId, GetHilightGC()(), 1, 1, fMenuWidth-3, 1);
   gVirtualX->DrawLine(fId, GetHilightGC()(), 1, 1, 1, fMenuHeight-3);

   gVirtualX->DrawLine(fId, GetShadowGC()(),  1, fMenuHeight-2, fMenuWidth-2, fMenuHeight-2);
   gVirtualX->DrawLine(fId, GetShadowGC()(),  fMenuWidth-2, fMenuHeight-2, fMenuWidth-2, 1);
   gVirtualX->DrawLine(fId, GetBlackGC()(), 0, fMenuHeight-1, fMenuWidth-1, fMenuHeight-1);
   gVirtualX->DrawLine(fId, GetBlackGC()(), fMenuWidth-1, fMenuHeight-1, fMenuWidth-1, 0);
}

//______________________________________________________________________________
void TGPopupMenu::DrawTrianglePattern(GContext_t gc, Int_t l, Int_t t,
                                      Int_t r, Int_t b)
{
   // Draw triangle pattern. Used for menu entries that are of type
   // kMenuPopup (i.e. cascading menus).

   Point_t  points[3];

   int m = (t + b) >> 1;

   points[0].fX = l;
   points[0].fY = t;
   points[1].fX = l;
   points[1].fY = b;
   points[2].fX = r;
   points[2].fY = m;

   gVirtualX->FillPolygon(fId, gc, points, 3);
}

//______________________________________________________________________________
void TGPopupMenu::DrawCheckMark(GContext_t gc, Int_t l, Int_t t, Int_t, Int_t b)
{
   // Draw check mark. Used for checked button type menu entries.

   Segment_t seg[6];

   t = (t + b - 8) >> 1; ++t;

   seg[0].fX1 = 1+l; seg[0].fY1 = 3+t; seg[0].fX2 = 3+l; seg[0].fY2 = 5+t;
   seg[1].fX1 = 1+l; seg[1].fY1 = 4+t; seg[1].fX2 = 3+l; seg[1].fY2 = 6+t;
   seg[2].fX1 = 1+l; seg[2].fY1 = 5+t; seg[2].fX2 = 3+l; seg[2].fY2 = 7+t;
   seg[3].fX1 = 3+l; seg[3].fY1 = 5+t; seg[3].fX2 = 7+l; seg[3].fY2 = 1+t;
   seg[4].fX1 = 3+l; seg[4].fY1 = 6+t; seg[4].fX2 = 7+l; seg[4].fY2 = 2+t;
   seg[5].fX1 = 3+l; seg[5].fY1 = 7+t; seg[5].fX2 = 7+l; seg[5].fY2 = 3+t;

   gVirtualX->DrawSegments(fId, gc, seg, 6);
}

//______________________________________________________________________________
void TGPopupMenu::DrawRCheckMark(GContext_t gc, Int_t l, Int_t t, Int_t r, Int_t b)
{
   // Draw radio check mark. Used for radio button type menu entries.

   Segment_t seg[5];

   t = (t + b - 5) >> 1; ++t;
   l = (l + r - 5) >> 1; ++l;

   seg[0].fX1 = 1+l; seg[0].fY1 = 0+t; seg[0].fX2 = 3+l; seg[0].fY2 = 0+t;
   seg[1].fX1 = 0+l; seg[1].fY1 = 1+t; seg[1].fX2 = 4+l; seg[1].fY2 = 1+t;
   seg[2].fX1 = 0+l; seg[2].fY1 = 2+t; seg[2].fX2 = 4+l; seg[2].fY2 = 2+t;
   seg[3].fX1 = 0+l; seg[3].fY1 = 3+t; seg[3].fX2 = 4+l; seg[3].fY2 = 3+t;
   seg[4].fX1 = 1+l; seg[4].fY1 = 4+t; seg[4].fX2 = 3+l; seg[4].fY2 = 4+t;

   gVirtualX->DrawSegments(fId, gc, seg, 5);
}

//______________________________________________________________________________
void TGPopupMenu::DefaultEntry(Int_t id)
{
   // Set default entry (default entries are drawn with bold text).

   TGMenuEntry *ptr;
   TIter next(fEntryList);

   while ((ptr = (TGMenuEntry *) next())) {
      if (ptr->fEntryId == id)
         ptr->fStatus |= kMenuDefaultMask;
      else
         ptr->fStatus &= ~kMenuDefaultMask;
   }
}

//______________________________________________________________________________
void TGPopupMenu::EnableEntry(Int_t id)
{
   // Enable entry. By default entries are enabled.

   TGMenuEntry *ptr;
   TIter next(fEntryList);

   while ((ptr = (TGMenuEntry *) next()))
      if (ptr->fEntryId == id) {
         ptr->fStatus |= kMenuEnableMask;
         if (ptr->fStatus & kMenuHideMask) {
            ptr->fStatus &= ~kMenuHideMask;
            Reposition();
         }
         break;
      }
}

//______________________________________________________________________________
void TGPopupMenu::DisableEntry(Int_t id)
{
   // Disable entry (disabled entries appear in a sunken relieve).

   TGMenuEntry *ptr;
   TIter next(fEntryList);

   while ((ptr = (TGMenuEntry *) next()))
      if (ptr->fEntryId == id) { ptr->fStatus &= ~kMenuEnableMask; break; }
}

//______________________________________________________________________________
Bool_t TGPopupMenu::IsEntryEnabled(Int_t id)
{
   // Return true if menu entry is enabled.

   TGMenuEntry *ptr;
   TIter next(fEntryList);

   while ((ptr = (TGMenuEntry *) next()))
      if (ptr->fEntryId == id)
         return (ptr->fStatus & kMenuEnableMask) ? kTRUE : kFALSE;
   return kFALSE;
}

//______________________________________________________________________________
void TGPopupMenu::HideEntry(Int_t id)
{
   // Hide entry (hidden entries are not shown in the menu).
   // To enable a hidden entry call EnableEntry().

   TGMenuEntry *ptr;
   TIter next(fEntryList);

   while ((ptr = (TGMenuEntry *) next()))
      if (ptr->fEntryId == id) {
         ptr->fStatus |=  kMenuHideMask;
         ptr->fStatus &= ~kMenuEnableMask;
         Reposition();
         break;
      }
}

//______________________________________________________________________________
Bool_t TGPopupMenu::IsEntryHidden(Int_t id)
{
   // Return true if menu entry is hidden.

   TGMenuEntry *ptr;
   TIter next(fEntryList);

   while ((ptr = (TGMenuEntry *) next()))
      if (ptr->fEntryId == id)
         return (ptr->fStatus & kMenuHideMask) ? kTRUE : kFALSE;
   return kFALSE;
}

//______________________________________________________________________________
void TGPopupMenu::CheckEntry(Int_t id)
{
   // Check a menu entry (i.e. add a check mark in front of it).

   TGMenuEntry *ptr;
   TIter next(fEntryList);

   while ((ptr = (TGMenuEntry *) next()))
      if (ptr->fEntryId == id) { ptr->fStatus |= kMenuCheckedMask; break; }
}

//______________________________________________________________________________
void TGPopupMenu::UnCheckEntry(Int_t id)
{
   // Uncheck menu entry (i.e. remove check mark).

   TGMenuEntry *ptr;
   TIter next(fEntryList);

   while ((ptr = (TGMenuEntry *) next()))
      if (ptr->fEntryId == id) { ptr->fStatus &= ~kMenuCheckedMask; break; }
}

//______________________________________________________________________________
Bool_t TGPopupMenu::IsEntryChecked(Int_t id)
{
   // Return true if menu item is checked.

   TGMenuEntry *ptr;
   TIter next(fEntryList);

   while ((ptr = (TGMenuEntry *) next()))
      if (ptr->fEntryId == id)
         return (ptr->fStatus & kMenuCheckedMask) ? kTRUE : kFALSE;
   return kFALSE;
}

//______________________________________________________________________________
void TGPopupMenu::RCheckEntry(Int_t id, Int_t IDfirst, Int_t IDlast)
{
   // Radio-select entry (note that they cannot be unselected,
   // the selection must be moved to another entry instead).

   TGMenuEntry *ptr;
   TIter next(fEntryList);

   while ((ptr = (TGMenuEntry *) next()))
      if (ptr->fEntryId == id)
         ptr->fStatus |= kMenuRadioMask | kMenuRadioEntryMask;
      else
         if (ptr->fEntryId >= IDfirst && ptr->fEntryId <= IDlast) {
            ptr->fStatus &= ~kMenuRadioMask;
            ptr->fStatus |=  kMenuRadioEntryMask;
         }
}

//______________________________________________________________________________
Bool_t TGPopupMenu::IsEntryRChecked(Int_t id)
{
   // Return true if menu item has radio check mark.

   TGMenuEntry *ptr;
   TIter next(fEntryList);

   while ((ptr = (TGMenuEntry *) next()))
      if (ptr->fEntryId == id)
         return (ptr->fStatus & kMenuRadioMask) ? kTRUE : kFALSE;
   return kFALSE;
}

//______________________________________________________________________________
TGMenuEntry *TGPopupMenu::GetEntry(Int_t id)
{
   // Find entry with specified id. Use the returned entry in DeleteEntry()
   // or as the "before" item in the AddXXXX() methods. Returns 0 if entry
   // is not found. To find entries that don't have an id like the separators,
   // use the GetListOfEntries() method to get the complete entry
   // list and iterate over it and check the type of each entry
   // to find the separators.

   TGMenuEntry *ptr;
   TIter next(fEntryList);

   while ((ptr = (TGMenuEntry *) next()))
      if (ptr->fEntryId == id)
         return ptr;
   return 0;
}

//______________________________________________________________________________
TGMenuEntry *TGPopupMenu::GetEntry(const char *s)
{
   // Find entry with specified name. Name must match the original
   // name without hot key symbol, like "Print" and not "&Print".
   // Use the returned entry in DeleteEntry() or as the "before" item
   // in the AddXXXX() methods. Returns 0 if entry is not found.
   // To find entries that don't have a name like the separators,
   // use the GetListOfEntries() method to get the complete entry
   // list and iterate over it and check the type of each entry
   // to find the separators.

   return (TGMenuEntry*) fEntryList->FindObject(s);
}

//______________________________________________________________________________
void TGPopupMenu::DeleteEntry(Int_t id)
{
   // Delete entry with specified id from menu.

   TGMenuEntry *ptr;
   TIter next(fEntryList);

   while ((ptr = (TGMenuEntry *) next()))
      if (ptr->fEntryId == id) {
         fEntryList->Remove(ptr);
         delete ptr;
         Reposition();
         if (fCurrent == ptr)
            fCurrent = 0;
         return;
      }
}

//______________________________________________________________________________
void TGPopupMenu::DeleteEntry(TGMenuEntry *entry)
{
   // Delete specified entry from menu.

   TGMenuEntry *ptr;
   TIter next(fEntryList);

   while ((ptr = (TGMenuEntry *) next()))
      if (ptr == entry) {
         fEntryList->Remove(ptr);
         delete ptr;
         Reposition();
         if (fCurrent == ptr)
            fCurrent = 0;
         return;
      }
}

//______________________________________________________________________________
const TGGC &TGPopupMenu::GetDefaultGC()
{
   if (!fgDefaultGC)
      fgDefaultGC = gClient->GetResourcePool()->GetFrameGC();
   return *fgDefaultGC;
}

//______________________________________________________________________________
const TGGC &TGPopupMenu::GetDefaultSelectedGC()
{
   if (!fgDefaultSelectedGC)
      fgDefaultSelectedGC = gClient->GetResourcePool()->GetSelectedGC();
   return *fgDefaultSelectedGC;
}

//______________________________________________________________________________
const TGGC &TGPopupMenu::GetDefaultSelectedBackgroundGC()
{
   if (!fgDefaultSelectedBackgroundGC)
      fgDefaultSelectedBackgroundGC = gClient->GetResourcePool()->GetSelectedBckgndGC();
   return *fgDefaultSelectedBackgroundGC;
}

//______________________________________________________________________________
FontStruct_t TGPopupMenu::GetDefaultFontStruct()
{
   if (!fgDefaultFont)
      fgDefaultFont = gClient->GetResourcePool()->GetMenuFont();
   return fgDefaultFont->GetFontStruct();
}

//______________________________________________________________________________
FontStruct_t TGPopupMenu::GetHilightFontStruct()
{
   if (!fgHilightFont)
      fgHilightFont = gClient->GetResourcePool()->GetMenuHiliteFont();
   return fgHilightFont->GetFontStruct();
}


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGMenuTitle member functions.                                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
TGMenuTitle::TGMenuTitle(const TGWindow *p, TGHotString *s, TGPopupMenu *menu,
                         GContext_t norm, FontStruct_t font, UInt_t options)
    : TGFrame(p, 1, 1, options)
{
   // Create a menu title. This object is created by a menu bar when adding
   // a popup menu. The menu title adopts the hotstring.

   fLabel      = s;
   fMenu       = menu;
   fFontStruct = font;
   fSelGC      = GetDefaultSelectedGC()();
   fNormGC     = norm;
   fState      = kFALSE;
   fTitleId    = -1;

   Int_t hotchar;
   if ((hotchar = s->GetHotChar()) != 0)
      fHkeycode = gVirtualX->KeysymToKeycode(hotchar);
   else
      fHkeycode = 0;

   UInt_t tw;
   Int_t  max_ascent, max_descent;
   tw = gVirtualX->TextWidth(fFontStruct, fLabel->GetString(), fLabel->GetLength());
   gVirtualX->GetFontProperties(fFontStruct, max_ascent, max_descent);

   Resize(tw + 8, max_ascent + max_descent + 7);

   if (p->InheritsFrom(TGMenuBar::Class())) {
      TGMenuBar *bar = (TGMenuBar*)p;
      fMenu->SetMenuBar(bar);
   }
}

//______________________________________________________________________________
void TGMenuTitle::SetState(Bool_t state)
{
   // Set state of menu title.

   fState = state;
   if (state) {
      if (fMenu != 0) {
         Int_t    ax, ay;
         Window_t wdummy;

         gVirtualX->TranslateCoordinates(fId, (fMenu->GetParent())->GetId(),
                                         0, 0, ax, ay, wdummy);

         // place the menu just under the window:
         fMenu->PlaceMenu(ax-1, ay+fHeight, kTRUE, kFALSE); //kTRUE);
      }
   } else {
      if (fMenu != 0) {
         fTitleId = fMenu->EndMenu(fTitleData);
      }
   }
   fOptions &= ~(kSunkenFrame | kRaisedFrame);
   fClient->NeedRedraw(this);
}

//______________________________________________________________________________
void TGMenuTitle::DoRedraw()
{
   // Draw a menu title.

   TGFrame::DoRedraw();

   int x, y, max_ascent, max_descent;
   x = y = 4;

   gVirtualX->GetFontProperties(fFontStruct, max_ascent, max_descent);

   if (fState) {
      gVirtualX->SetForeground(fNormGC, GetDefaultSelectedBackground());
      gVirtualX->FillRectangle(fId,fNormGC, 0, 0, fWidth, fHeight);
      gVirtualX->SetForeground(fNormGC, GetForeground());
      fLabel->Draw(fId, fSelGC, x, y + max_ascent);
   } else {
      gVirtualX->SetForeground(fNormGC,GetDefaultFrameBackground());
      gVirtualX->FillRectangle(fId,fNormGC, 0, 0, fWidth, fHeight);
      gVirtualX->SetForeground(fNormGC, GetForeground());
      fLabel->Draw(fId, fNormGC, x, y + max_ascent);
   }
}

//______________________________________________________________________________
void TGMenuTitle::DoSendMessage()
{
   // Send final selected menu item to be processed.

   if (fMenu)
      if (fTitleId != -1) {
         SendMessage(fMenu->fMsgWindow, MK_MSG(kC_COMMAND, kCM_MENU), fTitleId,
                     (Long_t)fTitleData);
         fMenu->Activated(fTitleId);
      }
}

//______________________________________________________________________________
FontStruct_t TGMenuTitle::GetDefaultFontStruct()
{
   if (!fgDefaultFont)
      fgDefaultFont = gClient->GetResourcePool()->GetMenuFont();
   return fgDefaultFont->GetFontStruct();
}

//______________________________________________________________________________
const TGGC &TGMenuTitle::GetDefaultGC()
{
   if (!fgDefaultGC)
      fgDefaultGC = gClient->GetResourcePool()->GetFrameGC();
   return *fgDefaultGC;
}

//______________________________________________________________________________
const TGGC &TGMenuTitle::GetDefaultSelectedGC()
{
   if (!fgDefaultSelectedGC)
      fgDefaultSelectedGC = gClient->GetResourcePool()->GetSelectedGC();
   return *fgDefaultSelectedGC;
}

//______________________________________________________________________________
void TGPopupMenu::SavePrimitive(ofstream &out, Option_t *option)
{
   // Save a popup menu widget as a C++ statement(s) on output stream out.

   char quote = '"';

   out << "   TGPopupMenu *";
   out << GetName() << " = new TGPopupMenu(gClient->GetRoot()"
       << "," << GetWidth() << "," << GetHeight() << "," << GetOptionString() << ");" << endl;

   Bool_t hasradio = kFALSE;
   Int_t r_first, r_last, r_active;
   r_active = r_first = r_last = -1;

   TGMenuEntry *mentry;
   TIter next(GetListOfEntries());

   while ((mentry = (TGMenuEntry *) next())) {
      const char *text;
      Int_t i, lentext, hotpos;
      char *outext;

      switch (mentry->GetType()) {
         case kMenuEntry:
            text = mentry->GetName();
            lentext = mentry->fLabel->GetLength();
            hotpos = mentry->fLabel->GetHotPos();
            outext = new char[lentext+2];
            i=0;
            while (lentext) {
               if (i == hotpos-1) {
                  outext[i] = '&';
                  i++;
               }
               outext[i] = *text;
               i++; text++; lentext--;
            }
            outext[i]=0;

            out << "   " << GetName() << "->AddEntry(" << quote
                << gSystem->ExpandPathName(gSystem->UnixPathName(outext)) // can be a file name
                << quote << "," << mentry->GetEntryId();
            if (mentry->fUserData) {
               out << "," << mentry->fUserData;
            }
            if (mentry->fPic) {
               out << ",gClient->GetPicture(" << quote
                   << gSystem->ExpandPathName(gSystem->UnixPathName(mentry->fPic->GetName()))
                   << quote << ")";
            }
            out << ");" << endl;
            delete [] outext;
            break;
         case kMenuPopup:
            out << endl;
            out << "   // cascaded menu " << quote << mentry->GetName() << quote <<endl;
            mentry->fPopup->SavePrimitive(out, option);
            text = mentry->GetName();
            lentext = mentry->fLabel->GetLength();
            hotpos = mentry->fLabel->GetHotPos();
            outext = new char[lentext+2];
            i=0;
            while (lentext) {
               if (i == hotpos-1) {
                  outext[i] = '&';
                  i++;
               }
               outext[i] = *text;
               i++; text++; lentext--;
            }
            outext[i]=0;

            out << "   " << GetName() << "->AddPopup(" << quote
                << outext << quote << "," << mentry->fPopup->GetName()
                << ");" << endl;
            delete [] outext;
            break;
         case kMenuLabel:
            out << "   " << GetName() << "->AddLabel(" << quote
                << mentry->GetName() << quote;
            if (mentry->fPic) {
               out << ",gClient->GetPicture(" << quote
                   << mentry->fPic->GetName()
                   << quote << ")";
            }
            out << ");" << endl;
            break;
         case kMenuSeparator:
            out << "   " << GetName() << "->AddSeparator();" << endl;
            break;
      }

      if (!(mentry->GetStatus() & kMenuEnableMask)) {
          out << "   " << GetName() << "->DisableEntry(" << mentry->GetEntryId()
              << ");" << endl;
      }
      if (mentry->GetStatus() & kMenuHideMask) {
          out << "   " << GetName() << "->HideEntry(" << mentry->GetEntryId()
              << ");" << endl;
      }
      if (mentry->GetStatus() & kMenuCheckedMask) {
          out << "   " << GetName() << "->CheckEntry(" << mentry->GetEntryId()
              << ");" << endl;
      }
      if (mentry->GetStatus() & kMenuDefaultMask) {
          out << "   "<< GetName() << "->DefaultEntry(" << mentry->GetEntryId()
              << ");" << endl;
      }
      if (mentry->GetStatus() & kMenuRadioEntryMask) {
         switch (hasradio) {
            case kFALSE:
               r_first = mentry->GetEntryId();
               hasradio = kTRUE;
               if (IsEntryRChecked(mentry->GetEntryId())) r_active = mentry->GetEntryId();
               break;
            case kTRUE:
               r_last = mentry->GetEntryId();
               if (IsEntryRChecked(mentry->GetEntryId())) r_active = mentry->GetEntryId();
            break;
         }
      } else if (hasradio) {
         out << "   " << GetName() << "->RCheckEntry(" << r_active << "," << r_first
             << "," << r_last << ");" << endl;
         hasradio = kFALSE;
         r_active = r_first = r_last = -1;
      }
   }
}

//______________________________________________________________________________
void TGMenuTitle::SavePrimitive(ofstream &out, Option_t *option)
{
    // Save a title menu widget as a C++ statement(s) on output stream out.

   char quote = '"';

   out << endl;
   out << "   // " << quote << fLabel->GetString() << quote <<" menu" << endl;

   fMenu->SavePrimitive(out, option);

   const char *text = fLabel->GetString();
   Int_t lentext = fLabel->GetLength();
   Int_t hotpos = fLabel->GetHotPos();
   char *outext = new char[lentext+2];
   Int_t i=0;
   while (lentext) {
      if (i == hotpos-1) {
          outext[i] = '&';
          i++;
      }
      outext[i] = *text;
      i++; text++; lentext--;
   }
   outext[i]=0;
   out << "   " << fParent->GetName() << "->AddPopup(" << quote << outext
       << quote << "," << fMenu->GetName();

   delete [] outext;
}

//______________________________________________________________________________
void TGMenuBar::SavePrimitive(ofstream &out, Option_t *option)
{
    // Save a menu bar widget as a C++ statement(s) on output stream out.

   out << endl;
   out << "   // menu bar" << endl;

   out << "   TGMenuBar *";
   out << GetName() << " = new TGMenuBar(" << fParent->GetName()
       << "," << GetWidth() << "," << GetHeight() << "," << GetOptionString() << ");" << endl;

   if (!fList) return;

   TGFrameElement *el;
   TIter next(fList);

   while ((el = (TGFrameElement *)next())) {
	     el->fFrame->SavePrimitive(out, option);
      el->fLayout->SavePrimitive(out, option);
      out << ");" << endl;
   }
}
