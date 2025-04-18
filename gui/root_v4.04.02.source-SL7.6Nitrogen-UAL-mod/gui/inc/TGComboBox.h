// @(#)root/gui:$Name:  $:$Id: TGComboBox.h,v 1.14 2004/09/08 08:13:11 brun Exp $
// Author: Fons Rademakers   13/01/98

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TGComboBox
#define ROOT_TGComboBox


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGComboBox, TGComboBoxPopup                                          //
//                                                                      //
// A combobox (also known as a drop down listbox) allows the selection  //
// of one item out of a list of items. The selected item is visible in  //
// a little window. To view the list of possible items one has to click //
// on a button on the right of the little window. This will drop down   //
// a listbox. After selecting an item from the listbox the box will     //
// disappear and the newly selected item will be shown in the little    //
// window.                                                              //
//                                                                      //
// The TGComboBox is user callable. The TGComboBoxPopup is a service    //
// class of the combobox.                                               //
//                                                                      //
// Selecting an item in the combobox will generate the event:           //
// kC_COMMAND, kCM_COMBOBOX, combobox id, item id.                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TGListBox
#include "TGListBox.h"
#endif

class TGScrollBarElement;
class TGTextEntry;

class TGComboBoxPopup : public TGCompositeFrame {

public:
   TGComboBoxPopup(const TGWindow *p = 0, UInt_t w = 1, UInt_t h = 1,
                   UInt_t options = kVerticalFrame,
                   Pixel_t back = GetWhitePixel());

   virtual Bool_t HandleButton(Event_t *);

   void PlacePopup(Int_t x, Int_t y, UInt_t w, UInt_t h);
   void EndPopup();

   ClassDef(TGComboBoxPopup,0)  // Combobox popup window
};


class TGComboBox : public TGCompositeFrame, public TGWidget {

protected:
   TGLBEntry           *fSelEntry;      // selected item frame
   TGTextEntry         *fTextEntry;     // text entry
   TGScrollBarElement  *fDDButton;      // button controlling drop down of popup
   TGComboBoxPopup     *fComboFrame;    // popup containing a listbox
   TGListBox           *fListBox;       // the listbox with text items
   const TGPicture     *fBpic;          // down arrow picture used in fDDButton
   TGLayoutHints       *fLhs;           // layout hints for selected item frame
   TGLayoutHints       *fLhb;           // layout hints for fDDButton
   TGLayoutHints       *fLhdd;          // layout hints for fListBox

   virtual void Init();

public:
   TGComboBox(const TGWindow *p = 0, Int_t id = -1,
              UInt_t options = kHorizontalFrame | kSunkenFrame | kDoubleBorder,
              Pixel_t back = GetWhitePixel());
   TGComboBox(const TGWindow *p, const char *text, Int_t id = -1,
              UInt_t options = kHorizontalFrame | kSunkenFrame | kDoubleBorder,
              Pixel_t back = GetWhitePixel());

   virtual ~TGComboBox();

   virtual void DrawBorder();
   virtual TGDimension GetDefaultSize() const { return TGDimension(fWidth, fHeight); }

   virtual Bool_t HandleButton(Event_t *event);
   virtual Bool_t HandleDoubleClick(Event_t *event);
   virtual Bool_t HandleMotion(Event_t *event);
   virtual Bool_t HandleSelection(Event_t *event);
   virtual Bool_t HandleSelectionRequest(Event_t *event);
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

   virtual void AddEntry(TGString *s, Int_t id)
           { fListBox->AddEntry(s, id); }
   virtual void AddEntry(const char *s, Int_t id)
           { fListBox->AddEntry(s, id); }
   virtual void AddEntry(TGLBEntry *lbe, TGLayoutHints *lhints)
           { fListBox->AddEntry(lbe, lhints); }
   virtual void InsertEntry(TGString *s, Int_t id, Int_t afterID)
           { fListBox->InsertEntry(s, id, afterID); }
   virtual void InsertEntry(const char *s, Int_t id, Int_t afterID)
           { fListBox->InsertEntry(s, id, afterID); }
   virtual void InsertEntry(TGLBEntry *lbe, TGLayoutHints *lhints, Int_t afterID)
           { fListBox->InsertEntry(lbe, lhints, afterID); }
   virtual void RemoveEntry(Int_t id)
           { fListBox->RemoveEntry(id); }
   virtual void RemoveEntries(Int_t from_ID, Int_t to_ID)
           { fListBox->RemoveEntries(from_ID, to_ID); }
   virtual Int_t GetNumberOfEntries() const
           { return fListBox->GetNumberOfEntries(); }

   virtual TGListBox    *GetListBox() const { return fListBox; }
   virtual TGTextEntry  *GetTextEntry() const { return fTextEntry; }

   virtual void  Select(Int_t id);
   virtual Int_t GetSelected() const { return fListBox->GetSelected(); }
   virtual TGLBEntry *GetSelectedEntry() const
           { return fListBox->GetSelectedEntry(); }

   virtual void SetTopEntry(TGLBEntry *e, TGLayoutHints *lh);

   virtual void Selected(Int_t widgetId, Int_t id); // *SIGNAL*
   virtual void Selected(Int_t id) { Emit("Selected(Int_t)", id); } // *SIGNAL*
   virtual void Selected(const char *txt) { Emit("Selected(char*)", txt); } // *SIGNAL*
   virtual void SavePrimitive(ofstream &out, Option_t *option);

   ClassDef(TGComboBox,0)  // Combo box widget
};


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// The TGLineStyleComboBox user callable and it creates                 //
// a combobox for selecting the line style.                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class TGLineStyleComboBox : public TGComboBox {

public:
   TGLineStyleComboBox(const TGWindow *p = 0, Int_t id = -1,
              UInt_t options = kHorizontalFrame | kSunkenFrame | kDoubleBorder,
              Pixel_t back = GetWhitePixel());
      
   ClassDef(TGLineStyleComboBox, 0)  // Line style combobox widget
    
};


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// The TGLineWidthComboBox user callable and it creates                 //
// a combobox for selecting the line width.                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


class TGLineWidthComboBox : public TGComboBox {

public:
   TGLineWidthComboBox(const TGWindow *p = 0, Int_t id = -1,
              UInt_t options = kHorizontalFrame | kSunkenFrame | kDoubleBorder,
              Pixel_t back = GetWhitePixel());
   
   ClassDef(TGLineWidthComboBox, 0)  // Line width combobox widget

     
};


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// The TGFontTypeComboBox is user callable and it creates               //
// a combobox for selecting the font.                                   //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

const Int_t kMaxFonts = 20;

class TGFontTypeComboBox : public TGComboBox { 

protected:
   FontStruct_t fFonts[kMaxFonts];      

public:
   TGFontTypeComboBox(const TGWindow *p = 0, Int_t id = -1, 
            UInt_t options = kHorizontalFrame | kSunkenFrame | kDoubleBorder,
            Pixel_t bask = GetWhitePixel());
   virtual ~TGFontTypeComboBox();

   ClassDef(TGFontTypeComboBox, 0)  // Font type combobox widget
};

#endif
