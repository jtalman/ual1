// @(#)root/thread:$Name:  $:$Id: TConditionImp.h,v 1.2 2004/12/10 12:13:33 rdm Exp $
// Author: Fons Rademakers   01/07/97

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TConditionImp
#define ROOT_TConditionImp


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TConditionImp                                                        //
//                                                                      //
// This class provides an abstract interface to the OS dependent        //
// condition classes (TPosixCondition and TWin32Condition).             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_Rtypes
#include "Rtypes.h"
#endif
#ifndef ROOT_TObject
#include "TObject.h"
#endif


class TConditionImp : public TObject {

public:
   TConditionImp() { }
   virtual ~TConditionImp() { }

   virtual Int_t  Wait() = 0;
   virtual Int_t  TimedWait(ULong_t secs, ULong_t nanoSecs = 0) = 0;
   virtual Int_t  Signal() = 0;
   virtual Int_t  Broadcast() = 0;

   ClassDef(TConditionImp,0)  // Condition variable implementation ABC
};

#endif
