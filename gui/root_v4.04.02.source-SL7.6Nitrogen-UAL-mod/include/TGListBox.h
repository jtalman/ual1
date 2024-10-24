// @(#)root/gui:$Name:  $:$Id: TGListBox.h,v 1.23 2004/12/10 17:35:58 brun Exp $
// Author: Fons Rademakers   12/01/98

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TGListBox
#define ROOT_TGListBox


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGListBox, TGLBContainer, TGLBEntry and TGTextLBEntry                //
//                                                                      //
// A listbox is a box, possibly with scrollbar, containing entries.     //
// Currently entries are simple text strings (TGTextLBEntry).           //
// A TGListBox looks a lot like a TGCanvas. It has a TGViewPort         //
// containing a TGLBContainer which contains the entries and it also    //
// has a vertical scrollbar which becomes visible if there are more     //
// items than fit in the visible part of the container.                 //
//                                                                      //
// The TGListBox is user callable. The other classes are service        //
// classes of the listbox.                                              //
//                                                                      //
// Selecting an item in the listbox will generate the event:            //
// kC_COMMAND, kCM_LISTBOX, listbox id, item id.                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TGFrame
#include "TGFrame.h"
#endif
#ifndef ROOT_TGCanvas
#include "TGCanvas.h"
#endif
#ifndef ROOT_TGScrollBar
#include "TGScrollBar.h"
#endif
#ifndef ROOT_TList
#include "TList.h"
#endif

class TGListBox;


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGLBEntry                                                            //
//                                                                      //
// Basic listbox entries. Listbox entries are created by a TGListBox    //
// and not by the user.                                                 //                                              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class TGLBEntry : public TGFrame {

protected:
   Int_t      fEntryId;          // message id of listbox entry
   Pixel_t    fBkcolor;          // entry background color
   Bool_t     fActive;           // true if entry is active

   virtual void DoRedraw() { }

public:
   TGLBEntry(const TGWindow *p = 0, Int_t id = -1, UInt_t options = kHorizontalFrame,
             Pixel_t back = GetWhitePixel());

   virtual void Activate(Bool_t a);
   virtual void Toggle();
   virtual void Update(TGLBEntry *) { }  // this is needed on TGComboBoxes :(
   Int_t  EntryId() const { return fEntryId; }
   Bool_t IsActive() const { return fActive;  }

   ClassDef(TGLBEntry,0)  // Basic listbox entry
};


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGTextLBEntry                                                        //
//                                                                      //
// Text string listbox entries.                                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class TGTextLBEntry : public TGLBEntry {

protected:
   TGString     *fText;           // entry text string
   UInt_t        fTWidth;         // text width
   UInt_t        fTHeight;        // text height
   Bool_t        fTextChanged;    // true if text has been changed
   GContext_t    fNormGC;         // text drawing graphics context
   FontStruct_t  fFontStruct;     // font used to draw string

   virtual void DoRedraw();

   static const TGFont *fgDefaultFont;
   static TGGC         *fgDefaultGC;

public:
   static FontStruct_t  GetDefaultFontStruct();
   static const TGGC   &GetDefaultGC();

   TGTextLBEntry(const TGWindow *p = 0, TGString *s = 0, Int_t id = -1,
                 GContext_t norm = GetDefaultGC()(),
                 FontStruct_t font = GetDefaultFontStruct(),
                 UInt_t options = kHorizontalFrame,
                 Pixel_t back = GetWhitePixel());
   virtual ~TGTextLBEntry();

   virtual TGDimension GetDefaultSize() const { return TGDimension(fTWidth, fTHeight+1); }
   const TGString *GetText() const { return fText; }
   void SetText(TGString *new_text);
   virtual const char *GetTitle() const { return fText->Data(); }
   virtual void  SetTitle(const char *text) { *fText = text; }

   virtual void  DrawCopy(Handle_t id, Int_t x, Int_t y);
   virtual void Update(TGLBEntry *e)
       { SetText(new TGString(((TGTextLBEntry *)e)->GetText())); }
   virtual void SavePrimitive(ofstream &out, Option_t *);

   ClassDef(TGTextLBEntry,0)  // Text listbox entry
};


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGLineLBEntry                                                        //
//                                                                      //
// Line style & width listbox entry.                                    //
// Line example and width number                                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class TGLineLBEntry : public TGTextLBEntry {

protected:
   UInt_t      fLineWidth;       // line width
   Style_t     fLineStyle;       // line style
   UInt_t      fLineLength;      // line lenght
   TGGC        *fLineGC;         // line graphics context

   virtual void DoRedraw();

public:
   TGLineLBEntry(const TGWindow *p = 0, Int_t id = -1, const char *str = 0,
                     UInt_t w = 0, Style_t s = 0,
                     UInt_t options = kHorizontalFrame,
                     Pixel_t back = GetWhitePixel());
   virtual ~TGLineLBEntry();

   virtual TGDimension GetDefaultSize() const
      { return TGDimension(fTWidth, fTHeight+1); }
   virtual Int_t GetLineWidth() const { return fLineWidth; }
   virtual void  SetLineWidth(Int_t width);
   Style_t       GetLineStyle() const { return fLineStyle; }
   virtual void  SetLineStyle(Style_t style);
   TGGC         *GetLineGC() const { return fLineGC; }
   virtual void  Update(TGLBEntry *e);
   virtual void  DrawCopy(Handle_t id, Int_t x, Int_t y);

   ClassDef(TGLineLBEntry, 0)  // Line width listbox entry
};

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGLBContainer                                                        //
//                                                                      //
// A Composite frame that contains a list of TGLBEnties.                //
// A TGLBContainer is created by the TGListBox and not by the user.     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class TGLBContainer : public TGContainer {

friend class TGListBox;

protected:
   TGLBEntry      *fLastActive;    // last active listbox entry in single selection listbox
   TGListBox      *fListBox;       // list box which contains this container
   Bool_t          fMultiSelect;   // true if multi selection is switched on
   Int_t           fChangeStatus;  // defines the changes (select or unselect) while the mouse
                                   // moves over a multi selectable list box

public:
   TGLBContainer(const TGWindow *p = 0, UInt_t w = 1, UInt_t h = 1,
                 UInt_t options = kSunkenFrame,
                 Pixel_t back = GetDefaultFrameBackground());
   virtual ~TGLBContainer();

   virtual void AddEntry(TGLBEntry *lbe, TGLayoutHints *lhints);
   virtual void AddEntrySort(TGLBEntry *lbe, TGLayoutHints *lhints);
   virtual void InsertEntry(TGLBEntry *lbe, TGLayoutHints *lhints, Int_t afterID);
   virtual void RemoveEntry(Int_t id);
   virtual void RemoveEntries(Int_t from_ID, Int_t to_ID);

   virtual void   Associate(const TGWindow *w) { fMsgWindow = w; }
   virtual void   SetListBox(TGListBox *lb) { fListBox = lb; }
   virtual Bool_t HandleButton(Event_t *event);
   virtual Bool_t HandleMotion(Event_t *event);
   virtual Int_t  GetSelected() const;
   virtual Bool_t GetSelection(Int_t id);
   virtual Int_t  GetPos(Int_t id);
   virtual TGLBEntry *GetSelectedEntry() const { return fLastActive; }
   virtual void   GetSelectedEntries(TList *selected);
   virtual TGLBEntry *Select(Int_t id, Bool_t sel);
   virtual TGLBEntry *Select(Int_t id);

   virtual TGVScrollBar  *GetVScrollbar() const;
   virtual void   SetVsbPosition(Int_t newPos);

   virtual void   SetMultipleSelections(Bool_t multi);
   virtual Bool_t GetMultipleSelections() const { return fMultiSelect; }

   ClassDef(TGLBContainer,0)  // Listbox container
};


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGListBox                                                            //
//                                                                      //
// A TGListBox widget.                                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class TGListBox : public TGCompositeFrame, public TGWidget {

protected:
   UInt_t           fItemVsize;       // maximum height of single entry
   Bool_t           fIntegralHeight;  // true if height should be multiple of fItemVsize
   TGLBContainer   *fLbc;             // listbox container
   TGViewPort      *fVport;           // listbox viewport (see TGCanvas.h)
   TGVScrollBar    *fVScrollbar;      // vertical scrollbar

   void SetContainer(TGFrame *f) { fVport->SetContainer(f); }

   virtual void InitListBox();

public:
   TGListBox(const TGWindow *p = 0, Int_t id = -1,
             UInt_t options = kSunkenFrame | kDoubleBorder,
             Pixel_t back = GetWhitePixel());
   virtual ~TGListBox();

   virtual void AddEntry(TGString *s, Int_t id);
   virtual void AddEntry(const char *s, Int_t id);
   virtual void AddEntry(TGLBEntry *lbe, TGLayoutHints *lhints);
   virtual void AddEntrySort(TGString *s, Int_t id);
   virtual void AddEntrySort(const char *s, Int_t id);
   virtual void AddEntrySort(TGLBEntry *lbe, TGLayoutHints *lhints);
   virtual void InsertEntry(TGString *s, Int_t id, Int_t afterID);
   virtual void InsertEntry(const char *s , Int_t id, Int_t afterID);
   virtual void InsertEntry(TGLBEntry *lbe, TGLayoutHints *lhints, Int_t afterID);
   virtual void RemoveEntry(Int_t id) { fLbc->RemoveEntry(id); }
   virtual void RemoveEntries(Int_t from_ID, Int_t to_ID)
                                  { fLbc->RemoveEntries(from_ID, to_ID); }
   virtual void SetTopEntry(Int_t id);
   virtual void SetMultipleSelections(Bool_t multi)
                                  { fLbc->SetMultipleSelections(multi); }
   virtual Bool_t GetMultipleSelections() const
                                  { return fLbc->GetMultipleSelections(); }
   virtual Int_t GetNumberOfEntries() const
                                  { return fLbc->GetList()->GetSize(); }
   virtual TGLBEntry   *GetEntry(Int_t id) const;
   virtual TGFrame     *GetContainer() const { return fVport->GetContainer(); }
   virtual TGViewPort  *GetViewPort() const { return fVport; }
   virtual TGScrollBar *GetScrollBar() const { return fVScrollbar; }
   virtual TGVScrollBar *GetVScrollbar() const { return fVScrollbar; }

   virtual void DrawBorder();
   virtual void Resize(UInt_t w, UInt_t h);
   virtual void Resize(TGDimension size) { Resize(size.fWidth, size.fHeight); }
   virtual void MoveResize(Int_t x, Int_t y, UInt_t w, UInt_t h);
   virtual void Layout();
   virtual void IntegralHeight(Bool_t mode) { fIntegralHeight = mode; }
   virtual TGDimension GetDefaultSize() const;

   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

   virtual TGLBEntry *Select(Int_t id, Bool_t sel = kTRUE)
                                       { return fLbc->Select(id, sel); }
   virtual Int_t GetSelected() const;
   virtual Bool_t GetSelection(Int_t id) { return fLbc->GetSelection(id); }
   virtual TGLBEntry *GetSelectedEntry() const { return fLbc->GetSelectedEntry(); }
   virtual void GetSelectedEntries(TList *selected);
   virtual void SavePrimitive(ofstream &out, Option_t *option);

   virtual void Selected(Int_t widgetId, Int_t id);   //*SIGNAL*
   virtual void Selected(Int_t id) { Emit("Selected(Int_t)", id); } //*SIGNAL*
   virtual void Selected(const char *txt) { Emit("Selected(char*)", txt); } //*SIGNAL
   virtual void SelectionChanged() { Emit("SelectionChanged()"); } //*SIGNAL*

   ClassDef(TGListBox,0)  // Listbox widget
};

#endif
