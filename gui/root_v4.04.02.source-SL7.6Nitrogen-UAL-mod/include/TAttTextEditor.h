// @(#)root/ged:$Name:  $:$Id: TAttTextEditor.h,v 1.2 2004/06/25 17:13:23 brun Exp $
// Author: Ilka  Antcheva 11/05/04

/*************************************************************************
 * Copyright (C) 1995-2002, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TGedTextEditor
#define ROOT_TGedTextEditor

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  TAttTextEditor                                                      //
//                                                                      //
//  Implements GUI for editing text attributes.                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TGButton
#include "TGWidget.h"
#endif
#ifndef ROOT_TGedFrame
#include "TGedFrame.h"
#endif

class TGComboBox;
class TGFontTypeComboBox;
class TGColorSelect;
class TAttText;
class TVirtualPad;

class TAttTextEditor : public TGedFrame {

protected:
   TAttText            *fAttText;         // text attribute object
   TGFontTypeComboBox  *fTypeCombo;       // font style combo box
   TGComboBox          *fSizeCombo;       // font size combo box
   TGComboBox          *fAlignCombo;      // font aligh combo box
   TGColorSelect       *fColorSelect;     // color selection widget

   static  TGComboBox *BuildFontSizeComboBox(TGFrame *parent, Int_t id);
   static  TGComboBox *BuildTextAlignComboBox(TGFrame *parent, Int_t id);

public:
   TAttTextEditor(const TGWindow *p, Int_t id,
                  Int_t width = 140, Int_t height = 30,
                  UInt_t options = kChildFrame,
                  Pixel_t back = GetDefaultFrameBackground());
   virtual ~TAttTextEditor();

   virtual void   SetModel(TVirtualPad *pad, TObject *obj, Int_t event);
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);

   ClassDef(TAttTextEditor,0)  //GUI for editing text attributes
};

#endif
