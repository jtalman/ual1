// @(#)root/thread:$Name:  $:$Id: TPosixCondition.cxx,v 1.3 2004/12/08 14:02:41 rdm Exp $
// Author: Fons Rademakers   01/07/97

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TPosixCondition                                                      //
//                                                                      //
// This class provides an interface to the posix condition variable     //
// routines.                                                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TPosixCondition.h"
#include "TPosixMutex.h"
#include "PosixThreadInc.h"

#include <errno.h>


ClassImp(TPosixCondition)

//______________________________________________________________________________
TPosixCondition::TPosixCondition(TMutexImp *m)
{
   // Create Condition variable. Ctor must be given a pointer to an
   // existing mutex. The condition variable is then linked to the mutex,
   // so that there is an implicit unlock and lock around Wait() and
   // TimedWait().

   fMutex = (TPosixMutex *) m;

#if (PthreadDraftVersion == 4)

   int rc = ERRNO(pthread_cond_init(&fCond, pthread_condattr_default));

#else

   int rc = ERRNO(pthread_cond_init(&fCond, 0));

#endif

   if (rc != 0)
      SysError("TCondition", "pthread_cond_init error");
}

//______________________________________________________________________________
TPosixCondition::~TPosixCondition()
{
   // TCondition dtor.

   int rc = ERRNO(pthread_cond_destroy(&fCond));

   if (rc != 0)
      SysError("~TCondition", "pthread_cond_destroy error");
}

//______________________________________________________________________________
Int_t TPosixCondition::Wait()
{
   // Wait for the condition variable to be signalled. The mutex is
   // implicitely released before waiting and locked again after waking up.
   // If Wait() is called by multiple threads, a signal may wake up more
   // than one thread. See POSIX threads documentation for details.

   return ERRNO(pthread_cond_wait(&fCond, &(fMutex->fMutex)));
}

//______________________________________________________________________________
Int_t TPosixCondition::TimedWait(ULong_t secs, ULong_t nanoSecs)
{
   // TimedWait() is given an absolute time to wait until. To wait for a
   // relative time from now, use TThread::GetTime(). See POSIX threads
   // documentation for why absolute times are better than relative.
   // Returns 0 if successfully signalled, 1 if time expired.

   timespec rqts = { secs, nanoSecs };

   int rc = ERRNO(pthread_cond_timedwait(&fCond, &(fMutex->fMutex), &rqts));

//#if (PthreadDraftVersion <= 6)
#if (PthreadDraftVersion == 4)
   if (rc == EAGAIN)
#else
   if (rc == ETIMEDOUT)
#endif
      rc = 1;

   return rc;
}

//______________________________________________________________________________
Int_t TPosixCondition::Signal()
{
   // If one or more threads have called Wait(), Signal() wakes up at least
   // one of them, possibly more. See POSIX threads documentation for details.

   return ERRNO(pthread_cond_signal(&fCond));
}


//______________________________________________________________________________
Int_t TPosixCondition::Broadcast()
{
   // Broadcast is like signal but wakes all threads which have called Wait().

   return ERRNO(pthread_cond_broadcast(&fCond));
}
