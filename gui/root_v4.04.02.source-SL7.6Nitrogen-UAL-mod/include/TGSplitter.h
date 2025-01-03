// @(#)root/gui:$Name:  $:$Id: TGSplitter.h,v 1.11 2004/09/20 19:11:54 rdm Exp $
// Author: Fons Rademakers   6/09/2000

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TGSplitter
#define ROOT_TGSplitter


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGSplitter, TGVSplitter and TGHSplitter                              //
//                                                                      //
// A splitter allows the frames left and right or above and below of    //
// it to be resized. The frame to be resized must have the kFixedWidth  //
// or kFixedHeight property set.                                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TGFrame
#include "TGFrame.h"
#endif


class TGSplitter : public TGFrame {

protected:
   Cursor_t    fSplitCursor;      // split cursor
   Bool_t      fDragging;         // true if in dragging mode

public:
   TGSplitter(const TGWindow *p = 0, UInt_t w = 2, UInt_t h = 4,
              UInt_t options = kChildFrame,
              Pixel_t back = GetDefaultFrameBackground());
   virtual ~TGSplitter() { }

   virtual Bool_t HandleButton(Event_t *event) = 0;
   virtual Bool_t HandleMotion(Event_t *event) = 0;
   virtual Bool_t HandleCrossing(Event_t *event) = 0;

   ClassDef(TGSplitter,0)  //A frame splitter abstract base class
};


class TGVSplitter : public TGSplitter {

protected:
   Int_t       fStartX;         // x position when dragging starts
   UInt_t      fFrameWidth;     // width of frame to be resized
   UInt_t      fFrameHeight;    // height of frame to be resized
   Int_t       fMin;            // min x position frame can be resized to
   Int_t       fMax;            // max x position frame can be resized to
   TGFrame    *fFrame;          // frame that should be resized
   Bool_t      fLeft;           // true if frame is on the left of splitter

public:
   TGVSplitter(const TGWindow *p = 0, UInt_t w = 2, UInt_t h = 4,
               UInt_t options = kChildFrame,
               Pixel_t back = GetDefaultFrameBackground());
   virtual ~TGVSplitter() { }

   virtual void   DrawBorder();
   virtual void   SetFrame(TGFrame *frame, Bool_t left);
   const TGFrame *GetFrame() const { return fFrame; }
   Bool_t         GetLeft() const { return fLeft; }
   Bool_t         IsLeft() const { return fLeft; }
   virtual void   SavePrimitive(ofstream &out, Option_t *option);

   virtual Bool_t HandleButton(Event_t *event);
   virtual Bool_t HandleMotion(Event_t *event);
   virtual Bool_t HandleCrossing(Event_t *event);

   ClassDef(TGVSplitter,0)  //A vertical frame splitter
};


class TGHSplitter : public TGSplitter {

protected:
   Int_t       fStartY;         // y position when dragging starts
   UInt_t      fFrameWidth;     // width of frame to be resized
   UInt_t      fFrameHeight;    // height of frame to be resized
   Int_t       fMin;            // min y position frame can be resized to
   Int_t       fMax;            // max y position frame can be resized to
   TGFrame    *fFrame;          // frame that should be resized
   Bool_t      fAbove;          // true if frame is above the splitter

public:
   TGHSplitter(const TGWindow *p = 0, UInt_t w = 4, UInt_t h = 2,
               UInt_t options = kChildFrame,
               Pixel_t back = GetDefaultFrameBackground());
   virtual ~TGHSplitter() { }

   virtual void   DrawBorder();
   virtual void   SetFrame(TGFrame *frame, Bool_t above);
   const TGFrame *GetFrame() const { return fFrame; }
   Bool_t         GetAbove() const { return fAbove; }
   Bool_t         IsAbove() const { return fAbove; }
   virtual void   SavePrimitive(ofstream &out, Option_t *option);

   virtual Bool_t HandleButton(Event_t *event);
   virtual Bool_t HandleMotion(Event_t *event);
   virtual Bool_t HandleCrossing(Event_t *event);

   ClassDef(TGHSplitter,0)  //A horizontal frame splitter
};

#endif
