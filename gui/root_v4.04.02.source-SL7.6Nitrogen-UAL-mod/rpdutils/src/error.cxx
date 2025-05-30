// @(#)root/rpdutils:$Name:  $:$Id: error.cxx,v 1.4 2004/05/18 11:56:38 rdm Exp $
// Author: Fons Rademakers   11/08/97
// Modifified: Gerardo Ganis 8/04/2003

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// error                                                                //
//                                                                      //
// Set of error handling routines for daemon process.                   //
// Merging of rootd and proofd/src/error.cxx                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>
#include "snprintf.h"

#if defined(hpux9)
extern "C" {
   extern void openlog(const char *, int, int);
   extern void syslog(int, const char *, ...);
}
#endif

#ifdef __sun
#   ifndef _REENTRANT
#      if __SUNPRO_CC > 0x420
#         define GLOBAL_ERRNO
#      endif
#   endif
#endif

#include "rpderr.h"
#include "rpdp.h"

// This is the only really global 
extern int     gDebug;

namespace ROOT {

extern bool gSysLog;

//______________________________________________________________________________
int GetErrno()
{
#ifdef GLOBAL_ERRNO
   return ::errno;
#else
   return errno;
#endif
}

//______________________________________________________________________________
void ResetErrno()
{
#ifdef GLOBAL_ERRNO
   ::errno = 0;
#else
   errno = 0;
#endif
}

//______________________________________________________________________________
void ErrorInfo(const char *va_(fmt), ...)
{
   // Write info message to syslog.

   char    buf[kMAXSECBUF];
   va_list ap;

   va_start(ap,va_(fmt));
   vsnprintf(buf, sizeof(buf), fmt, ap);
   va_end(ap);

   if (gSysLog) {
    syslog(LOG_INFO, buf);
   } else {
     fprintf(stderr, "%s\n",buf);
   }
}

//______________________________________________________________________________
void ErrorInit(const char *ident)
{
   // Open syslog.

   openlog(ident, (LOG_PID | LOG_CONS), LOG_DAEMON);
}

//______________________________________________________________________________
void Perror(char *buf, int size)
{
   // Return in buf the message belonging to errno.

   int len = strlen(buf);
#if (defined(__sun) && defined (__SVR4)) || defined (__linux) || \
   defined(_AIX) || defined(__MACH__)
   snprintf(buf+len, size, " (%s)", strerror(GetErrno()));
#else
   if (GetErrno() >= 0 && GetErrno() < sys_nerr)
      snprintf(buf+len, size, " (%s)", sys_errlist[GetErrno()]);
#endif
}

//______________________________________________________________________________
void Error(ErrorHandler_t func, int code, const char *va_(fmt), ...)
{
   // Write fatal message to syslog and exit.

   char    buf[kMAXSECBUF];
   va_list ap;

   va_start(ap,va_(fmt));
   vsnprintf(buf, sizeof(buf), fmt, ap);
   va_end(ap);

   if (gSysLog) {
      syslog(LOG_ERR, buf);
   } else {
      fprintf(stderr, "%s\n",buf);
   }

   // Actions are defined by the specific error handler function
   // (see rootd.cxx and proofd.cxx)
   if (func) (*func)(code,(const char *)buf);
}

} // namespace ROOT
