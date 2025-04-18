// @(#)root/gpad:$Name:  $:$Id: TControlBarButton.h,v 1.1.1.1 2000/05/16 17:00:41 rdm Exp $
// Author: Nenad Buncic   20/02/96

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TControlBarButton
#define ROOT_TControlBarButton


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// TControlBarButton                                                          //
//                                                                            //
// This class defines the control bar buttons.                                //
//                                                                            //
//                                                                            //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef ROOT_Rtypes
#include "Rtypes.h"
#endif

#ifndef ROOT_TNamed
#include "TNamed.h"
#endif


class TControlBarButton : public TNamed {

protected:
   Int_t    fType;       //button type
   TString  fAction;     //action to be executed

public:
   enum { kButton = 1, kDrawnButton, kSeparator };

   TControlBarButton();
   TControlBarButton(const char *label, const char *action="", const char *hint="", const char *type="button");
   virtual ~TControlBarButton() { }

   virtual void        Create() { }
   virtual void        Action();
   virtual const char *GetAction() const { return fAction.Data(); }
   virtual Int_t       GetType() const { return fType; }
   virtual void        SetAction(const char *action);
   virtual void        SetType(const char *type);
   virtual void        SetType(Int_t type);

   ClassDef(TControlBarButton,0) //The Control bar button
};

#endif
