// @(#)root/gui:$Name:  $:$Id: TGIdleHandler.h,v 1.1 2004/08/02 11:43:12 rdm Exp $
// Author: Fons Rademakers   2/8/2004

/*************************************************************************
 * Copyright (C) 1995-2004, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TGIdleHandler
#define ROOT_TGIdleHandler

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGIdleHandler                                                        //
//                                                                      //
// Handle idle events, i.e. process GUI actions when there is nothing   //
// else anymore to do.                                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include "TObject.h"
#endif

class TGWindow;


class TGIdleHandler : public TObject {

private:
   TGWindow *fWindow;

public:
   TGIdleHandler(TGWindow *w);
   virtual ~TGIdleHandler();

   virtual Bool_t HandleEvent();

   ClassDef(TGIdleHandler,0)  // Idle event handler
};

#endif
