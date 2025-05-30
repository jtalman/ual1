/* @(#)root/clib:$Name:  $:$Id: getpagesize.c,v 1.2 2001/02/20 18:18:32 rdm Exp $ */
/* Author: */
#ifdef WIN32
#include <windows.h>
#include "mmalloc.h"

int getpagesize()
{
  SYSTEM_INFO siSysInfo;   /* struct for hardware information */
  /* Copy the hardware information to the SYSTEM_INFO structure. */

  GetSystemInfo(&siSysInfo);
  return  siSysInfo.dwPageSize;
}

#else

#ifndef __GNUC__
/* Prevent "empty translation unit" warnings. */
#include <stdio.h>
#endif

#endif
