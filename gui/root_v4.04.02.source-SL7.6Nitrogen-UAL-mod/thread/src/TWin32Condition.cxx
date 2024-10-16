// @(#)root/thread:$Name:  $:$Id: TWin32Condition.cxx,v 1.4 2004/12/16 11:15:01 rdm Exp $
// Author: Bertrand Bellenot  20/10/2004

/*************************************************************************
 * Copyright (C) 1995-2004, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TWin32Condition                                                      //
//                                                                      //
// This class provides an interface to the win32 condition variable     //
// routines.                                                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TWin32Condition.h"
#include "TWin32Mutex.h"
#include "TTimeStamp.h"
#include "Windows4Root.h"

#include <errno.h>

ClassImp(TWin32Condition)

//______________________________________________________________________________
TWin32Condition::TWin32Condition(TMutexImp *m)
{
   // Create Condition variable. Ctor must be given a pointer to an
   // existing mutex. The condition variable is then linked to the mutex,
   // so that there is an implicit unlock and lock around Wait() and
   // TimedWait().

   fMutex = (TWin32Mutex *) m;

   fCond.waiters_count_ = 0;
   fCond.was_broadcast_ = 0;
   fCond.sema_ = CreateSemaphore(0,          // no security
                                 0,          // initially 0
                                 0x7fffffff, // max count
                                 0);         // unnamed
   InitializeCriticalSection (&fCond.waiters_count_lock_);
   fCond.waiters_done_ = CreateEvent(0,     // no security
                                     FALSE, // auto-reset
                                     FALSE, // non-signaled initially
                                     0);    // unnamed
}

//______________________________________________________________________________
TWin32Condition::~TWin32Condition()
{
   // TCondition dtor.

}

//______________________________________________________________________________
Int_t TWin32Condition::Wait()
{
   // Wait for the condition variable to be signalled. The mutex is
   // implicitely released before waiting and locked again after waking up.
   // If Wait() is called by multiple threads, a signal may wake up more
   // than one thread. See POSIX threads documentation for details.

   // Avoid race conditions.
   EnterCriticalSection(&fCond.waiters_count_lock_);
   fCond.waiters_count_++;
   LeaveCriticalSection(&fCond.waiters_count_lock_);

   // This call atomically releases the mutex and waits on the
   // semaphore until <pthread_cond_signal> or <pthread_cond_broadcast>
   // are called by another thread.
   SignalObjectAndWait(fMutex->fHMutex, fCond.sema_, INFINITE, FALSE);

   // Reacquire lock to avoid race conditions.
   EnterCriticalSection(&fCond.waiters_count_lock_);

   // We're no longer waiting...
   fCond.waiters_count_--;

   // Check to see if we're the last waiter after <pthread_cond_broadcast>.
   int last_waiter = fCond.was_broadcast_ && fCond.waiters_count_ == 0;

   LeaveCriticalSection(&fCond.waiters_count_lock_);

   // If we're the last waiter thread during this particular broadcast
   // then let all the other threads proceed.
   if (last_waiter)
      // This call atomically signals the <waiters_done_> event and waits until
      // it can acquire the <fMutex->fHMutex>.  This is required to ensure fairness.
      SignalObjectAndWait(fCond.waiters_done_, fMutex->fHMutex, INFINITE, FALSE);
   else
      // Always regain the external mutex since that's the guarantee we
      // give to our callers.
      WaitForSingleObject(fMutex->fHMutex, INFINITE);

   return 0;
}

//______________________________________________________________________________
Int_t TWin32Condition::TimedWait(ULong_t secs, ULong_t nanoSecs)
{
   // TimedWait() is given an absolute time to wait until. To wait for a
   // relative time from now, use TThread::GetTime(). See POSIX threads
   // documentation for why absolute times are better than relative.
   // Returns 0 if successfully signalled, 1 if time expired.

   DWORD ret; 
   TTimeStamp t;
   // Get actual time
   ULong_t secNow     = t.GetSec();
   ULong_t nanosecNow = t.GetNanoSec();
   DWORD dwMillisecondsNow = (DWORD)((secNow * 1000) + (nanosecNow / 1000000));
   DWORD dwMilliseconds = (DWORD)((secs * 1000) + (nanoSecs / 1000000));
   // Calculate delta T to obtain the real time to wait for
   DWORD dwTimeWait = (DWORD)(dwMilliseconds - dwMillisecondsNow);
   // Avoid race conditions.
   EnterCriticalSection(&fCond.waiters_count_lock_);
   fCond.waiters_count_++;
   LeaveCriticalSection(&fCond.waiters_count_lock_);

   // This call atomically releases the mutex and waits on the
   // semaphore until <pthread_cond_signal> or <pthread_cond_broadcast>
   // are called by another thread.
   ret = SignalObjectAndWait(fMutex->fHMutex, fCond.sema_, dwTimeWait, FALSE);

   // Reacquire lock to avoid race conditions.
   EnterCriticalSection(&fCond.waiters_count_lock_);

   // We're no longer waiting...
   fCond.waiters_count_--;

   // Check to see if we're the last waiter after <pthread_cond_broadcast>.
   int last_waiter = fCond.was_broadcast_ && fCond.waiters_count_ == 0;

   LeaveCriticalSection(&fCond.waiters_count_lock_);

   // If we're the last waiter thread during this particular broadcast
   // then let all the other threads proceed.
   if (last_waiter)
      // This call atomically signals the <waiters_done_> event and waits until
      // it can acquire the <fMutex->fHMutex>.  This is required to ensure fairness.
      SignalObjectAndWait(fCond.waiters_done_, fMutex->fHMutex, dwTimeWait, FALSE);
   else
      // Always regain the external mutex since that's the guarantee we
      // give to our callers.
      WaitForSingleObject(fMutex->fHMutex, INFINITE);
   
   if(ret == WAIT_TIMEOUT)
      return 1;
   return 0;
}

//______________________________________________________________________________
Int_t TWin32Condition::Signal()
{
   // If one or more threads have called Wait(), Signal() wakes up at least
   // one of them, possibly more. See POSIX threads documentation for details.

   EnterCriticalSection (&fCond.waiters_count_lock_);
   int have_waiters = fCond.waiters_count_ > 0;
   LeaveCriticalSection (&fCond.waiters_count_lock_);

   // If there aren't any waiters, then this is a no-op.
   if (have_waiters)
      ReleaseSemaphore(fCond.sema_, 1, 0);

   return 0;
}


//______________________________________________________________________________
Int_t TWin32Condition::Broadcast()
{
   // Broadcast is like signal but wakes all threads which have called Wait().

   // This is needed to ensure that <waiters_count_> and <was_broadcast_> are
   // consistent relative to each other.
   EnterCriticalSection(&fCond.waiters_count_lock_);
   int have_waiters = 0;

   if (fCond.waiters_count_ > 0) {
      // We are broadcasting, even if there is just one waiter...
      // Record that we are broadcasting, which helps optimize
      // <pthread_cond_wait> for the non-broadcast case.
      fCond.was_broadcast_ = 1;
      have_waiters = 1;
   }

   if (have_waiters) {
      // Wake up all the waiters atomically.
      ReleaseSemaphore(fCond.sema_, fCond.waiters_count_, 0);

      LeaveCriticalSection(&fCond.waiters_count_lock_);

      // Wait for all the awakened threads to acquire the counting
      // semaphore.
      WaitForSingleObject(fCond.waiters_done_, INFINITE);
      // This assignment is okay, even without the <waiters_count_lock_> held
      // because no other waiter threads can wake up to access it.
      fCond.was_broadcast_ = 0;
   }
   else
      LeaveCriticalSection(&fCond.waiters_count_lock_);

   return 0;
}
