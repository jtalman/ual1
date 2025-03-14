// @(#)root/gui:$Name:  $:$Id: TGMdiFrame.h,v 1.5 2004/10/25 12:06:50 rdm Exp $
// Author: Bertrand Bellenot   20/08/2004

/*************************************************************************
 * Copyright (C) 1995-2004, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

/**************************************************************************

    This file is part of TGMdi, an extension to the xclass toolkit.
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

#ifndef ROOT_TGMdiFrame
#define ROOT_TGMdiFrame

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGMdiFrame.                                                          //
//                                                                      //
// This file contains the TGMdiFrame class.                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TGFrame
#include "TGFrame.h"
#endif

class TGPicture;
class TGMdiMainFrame;
class TGMdiDecorFrame;


class TGMdiFrame : public TGCompositeFrame {

friend class TGMdiMainFrame;
friend class TGMdiDecorFrame;

protected:
   enum { kDontCallClose = BIT(14) };

   TGMdiMainFrame  *fMain;
   ULong_t          fMdiHints;

   TString GetMdiHintsString() const;

public:
   TGMdiFrame(TGMdiMainFrame *main, Int_t w, Int_t h,
              UInt_t options = 0,
              Pixel_t back = GetDefaultFrameBackground());
   virtual ~TGMdiFrame();

   virtual void      Move(Int_t x, Int_t y);
   virtual Bool_t    CloseWindow();     //*SIGNAL*
   virtual Bool_t    Help() { return kFALSE; }

   virtual void      SetMdiHints(ULong_t mdihints);
   ULong_t           GetMdiHints() const { return fMdiHints; }

   void              DontCallClose();
   void              SetWindowName(const char *name);
   void              SetWindowIcon(const TGPicture *pic);
   const char       *GetWindowName();
   const TGPicture  *GetWindowIcon();

   virtual void      SavePrimitive(ofstream &out, Option_t *option);

   ClassDef(TGMdiFrame, 0)
};

#endif
