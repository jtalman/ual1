// @(#)root/thread:$Name:  $:$Id: TThreadFactory.h,v 1.2 2004/12/10 12:13:33 rdm Exp $
// Author: Fons Rademakers   01/07/97

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/


#ifndef ROOT_TThreadFactory
#define ROOT_TThreadFactory

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TThreadFactory                                                       //
//                                                                      //
// This ABC is a factory for thread components. Depending on which      //
// factory is active one gets either Posix or Win32 threads.            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TNamed
#include "TNamed.h"
#endif

class TMutexImp;
class TConditionImp;
class TThreadImp;
class TThread;

class TThreadFactory : public TNamed {

public:
   TThreadFactory(const char *name = "Unknown", const char *title = "Unknown Thread Factory");
   virtual ~TThreadFactory() { }

   virtual TMutexImp      *CreateMutexImp() = 0;
   virtual TConditionImp  *CreateConditionImp(TMutexImp *m) = 0;
   virtual TThreadImp     *CreateThreadImp() = 0;

   ClassDef(TThreadFactory,0)  // Thread factory ABC
};

R__EXTERN TThreadFactory *gThreadFactory;

#endif



