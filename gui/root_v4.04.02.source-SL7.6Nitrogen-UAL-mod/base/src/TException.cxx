// @(#)root/base:$Name:  $:$Id: TException.cxx,v 1.1.1.1 2000/05/16 17:00:38 rdm Exp $
// Author: Fons Rademakers   21/09/95

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Exception Handling                                                   //
//                                                                      //
// Provide some macro's to simulate the coming C++ try, catch and throw //
// exception handling functionality.                                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TException.h"

ExceptionContext_t *gException;


//______________________________________________________________________________
void Throw(int code)
{
   // If an exception context has been set (using the TRY and RETRY macros)
   // jump back to where it was set.

   if (gException)
#ifdef NEED_SIGJMP
      siglongjmp(gException->buf, code);
#else
      longjmp(gException->buf, code);
#endif
}


