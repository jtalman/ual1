// @(#)root/thread:$Name:  $:$Id: TPosixThread.h,v 1.4 2004/12/14 15:06:18 rdm Exp $
// Author: Fons Rademakers   02/07/97

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TPosixThread
#define ROOT_TPosixThread


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TPosixThread                                                         //
//                                                                      //
// This class provides an interface to the posix thread routines.       //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TThreadImp
#include "TThreadImp.h"
#endif

#ifndef __CINT__
#include <pthread.h>
#endif

R__EXTERN "C" unsigned int sleep(unsigned int seconds);

class TPosixThreadCleanUp;


class TPosixThread : public TThreadImp {

private:
   TPosixThreadCleanUp *fCleanUp;

public:
   TPosixThread() { }
   ~TPosixThread() { }

   virtual Int_t  Join(TThread *th, void **ret);
   virtual Long_t SelfId();
   virtual Int_t  Run(TThread *th);

   virtual Int_t  Kill(TThread *th);
   virtual Int_t  SetCancelOff();
   virtual Int_t  SetCancelOn();
   virtual Int_t  SetCancelAsynchronous();
   virtual Int_t  SetCancelDeferred();
   virtual Int_t  CancelPoint();
   virtual Int_t  CleanUpPush(void **main, void *free,void *arg);
   virtual Int_t  CleanUpPop(void **main, Int_t exe);
   virtual Int_t  CleanUp(void **main);

   virtual Int_t  Exit(void *ret);

   ClassDef(TPosixThread,0)  // TPosixThread class
};


class TPosixThreadCleanUp {

friend class TPosixThread;

private:
   void                *fRoutine;
   void                *fArgument;
   TPosixThreadCleanUp *fNext;

public:
   TPosixThreadCleanUp(void **main, void *routine, void *arg);
   ~TPosixThreadCleanUp() { }
};

#endif
