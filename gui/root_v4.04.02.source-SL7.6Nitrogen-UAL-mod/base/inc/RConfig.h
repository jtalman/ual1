/* @(#)root/base:$Name:  $:$Id: RConfig.h,v 1.82 2005/04/18 16:05:48 rdm Exp $ */

/*************************************************************************
 * Copyright (C) 1995-2002, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_RConfig
#define ROOT_RConfig

/*************************************************************************
 *                                                                       *
 * RConfig                                                               *
 *                                                                       *
 * Defines used by ROOT.                                                 *
 *                                                                       *
 *************************************************************************/

#ifndef ROOT_RVersion
#include "RVersion.h"
#endif


/*---- new C++ features ------------------------------------------------------*/

#define R__USE_SHADOW_CLASS

/*---- machines --------------------------------------------------------------*/

#ifdef __hpux
    /* R__HPUX10 or R__HPUX11 is determined in the Makefile */
#   define R__HPUX
#   define R__UNIX
#   define ANSICPP
#   ifdef __LP64__
#      define R__B64
#   endif
#   ifdef R__HPUX10
#      define NEED_SNPRINTF
#   endif
#endif

#ifdef _AIX
#   define R__AIX
#   define R__UNIX
#   define ANSICPP
#   define R__SEEK64
#   define R__PLACEMENTINLINE /* placement new/delete is inline in <new> */
#   define NEED_STRCASECMP
#   define R__ANSISTREAM      /* ANSI C++ Standard Library conformant */
#endif

#ifdef __linux
#   ifndef linux
#      define linux
#   endif
#endif

#if defined(__CYGWIN__) && defined(__GNUC__)
#   ifndef linux
#      define linux
#   endif
#   ifndef R__WINGCC
#      define R__WINGCC
#   endif
#endif

#if defined(__alpha) && !defined(linux)
#   include <standards.h>
#   ifdef _XOPEN_SOURCE
#      if _XOPEN_SOURCE+0 > 0
#         define R__TRUE64
#      endif
#   endif
#   define R__ALPHA
#   define ANSICPP
#   ifndef R__TRUE64
#      define NEED_SNPRINTF
#   endif
#   ifndef __VMS
#      define R__UNIX
#      define R__B64
#      define R__BYTESWAP
#      if __DECCXX_VER >= 60060002
#         define R__VECNEWDELETE /* supports overloading of new[] and delete[] */
#         define R__PLACEMENTDELETE /* supports overloading placement delete */
#         define R__PLACEMENTINLINE /* placement new/delete is inline in <new> */
#         define R__THROWNEWDELETE  /* new/delete throw exceptions */
#      endif
#      if defined __GNUC__
#         define R__NAMESPACE_TEMPLATE_IMP_BUG
#         define R__PLACEMENTINLINE /* placement new/delete is inline in <new> */
#      else
#         define R__TEMPLATE_OVERLOAD_BUG
#      endif
#   else
#      define R__VMS
#      define cxxbug
#      define NEED_STRCASECMP
#      define R__NONSCALARFPOS
#   endif
#endif

#if defined(__sun) && !(defined(linux) || defined(__FCC_VERSION))
#   ifdef __SVR4
#      define R__SOLARIS
#      define R__SEEK64
#      define ANSICPP
#      ifdef __i386
#         define R__I386
#         define R__BYTESWAP
#      endif
#   else
#      define R__SUN
#      include <stdlib.h>
#   endif
#   define R__UNIX
#   define NEED_STRING
#   define NEED_SIGJMP
#   if __SUNPRO_CC > 0x420
#      define R__SOLARIS_CC50
#      define R__ANSISTREAM      /* ANSI C++ Standard Library conformant */
#      define R__PLACEMENTINLINE /* placement new/delete is inline in <new> */
#   endif
#   if __SUNPRO_CC >= 0x420
#      define R__SUNCCBUG        /* to work around a compiler bug */
#   endif
#   if __GNUC__ >= 3 || __GNUC_MINOR__ >= 90   /* modern egcs/gcc */
#      define R__SUNGCC3
#   endif
#endif

#if defined(__FCC_VERSION)    /* Solaris with Fujitsu compiler */
#   define R__SOLARIS
#   define R__SEEK64
#   define ANSICPP
#   define R__UNIX
#   define NEED_STRING
#   define NEED_SIGJMP
#   define R__ANSISTREAM      /* ANSI C++ Standard Library conformant */
#endif

#if defined(__sgi) && !defined(linux)
#   define R__SGI
#   define R__UNIX
#   define ANSICPP
#   define NEED_STRING
#   define NEED_SIGJMP
#   define R__SEEK64
#   if !defined(__KCC)
#      define R__THROWNEWDELETE  /* new/delete throw exceptions */
#   endif
#   define R__PLACEMENTINLINE /* placement new/delete is inline in <new> */
#   ifdef IRIX64
#      define R__SGI64
#   endif
#   if defined(__mips64) || defined(_ABI64)
#      define R__B64
#      undef R__SEEK64
#   endif
#   if !defined(__GNUC__) && !defined(__KCC)
#      define R__ANSISTREAM      /* ANSI C++ Standard Library conformant */
#   endif
#endif

#if defined(linux)
#   ifndef _LARGEFILE64_SOURCE
#      define _LARGEFILE64_SOURCE
#   endif
#   include <features.h>
#   if __GNU_LIBRARY__ == 6
#      ifndef R__GLIBC
#         define R__GLIBC
#      endif
#   endif
#   if __GLIBC__ == 2 && __GLIBC_MINOR__ >= 2
#      define R__NONSCALARFPOS2
#      define R__USESTHROW
#      define R__SEEK64
#   endif
#endif

#if defined(linux) && defined(__i386__)
#   define R__LINUX
#   define R__UNIX
#   define R__BYTESWAP
#   ifndef __i486__
#      define __i486__       /* turn off if you really want to run on an i386 */
#   endif
#   define NEED_SIGJMP
#endif

#if defined(linux) && defined(__ia64__)
#   define R__LINUX
#   define R__UNIX
#   define R__BYTESWAP
#   define R__B64
#   define NEED_SIGJMP
#endif

#if defined(linux) && defined(__x86_64__)
#   define R__LINUX
#   define R__UNIX
#   define R__BYTESWAP
#   define R__B64
#   define NEED_SIGJMP
#endif

#if defined(linux) && defined(__alpha__)
#   define R__LINUX
#   define R__UNIX
#   define R__BYTESWAP
#   define R__B64
#   define NEED_SIGJMP
#endif

#if defined(linux) && defined(__arm__)
#   define R__LINUX
#   define R__UNIX
#   define R__BYTESWAP
#   define NEED_SIGJMP
#endif

#if defined(linux) && defined(__sparc__)
#   define R__LINUX
#   define R__UNIX
#   define NEED_SIGJMP
/*#   define R__B64 */     /* enable when 64 bit machine */
#endif

#if defined(linux) && defined(__sgi)
#   define R__LINUX
#   define R__UNIX
#   define NEED_SIGJMP
#   if defined(__mips64) || defined(_ABI64)
#      define R__B64      /* enable when 64 bit machine */
#   endif
#endif

#if defined(linux) && defined(__powerpc__)
#   define R__LINUX
#   define R__UNIX
#   define NEED_SIGJMP
#   if defined(R__ppc64)
#      define R__B64
#   endif
#endif

#if defined(__MACH__) && defined(__i386__)
#   define R__HURD
#   define f2cFortran   /* cfortran.h does not know HURD - sigh */
#   define R__UNIX
#   define R__BYTESWAP
#   define R__GLIBC     /* GNU/Hurd always use GLIBC 2.x :-) */
#   define NEED_SIGJMP
#endif

#if defined(__Lynx__) && defined(__powerpc__)
#   define R__LYNXOS
#   define R__UNIX
#   define ANSICPP
#   define NEED_SIGJMP
#   define NEED_STRCASECMP
#   define NEED_SNPRINTF
#endif

#if defined(__FreeBSD__)
#   define R__FBSD
#   define R__UNIX
#   define R__BYTESWAP
#   ifndef __i486__
#      define __i486__       /* turn off if you really want to run on an i386 */
#   endif
#   define R__THROWNEWDELETE /* new/delete throw exceptions */
#endif

#if defined(__OpenBSD__)
#   define R__OBSD
#   define R__UNIX
#   define R__BYTESWAP
#   ifndef __i486__
#      define __i486__       /* turn off if you really want to run on an i386 */
#   endif
#   define R__THROWNEWDELETE /* new/delete throw exceptions */
#endif

#if defined(__APPLE__)     /* MacOS X support, initially following FreeBSD */
#   include <AvailabilityMacros.h>
#   define R__MACOSX
#   define R__UNIX
#   if defined(__xlC__) || defined(__xlc__)
#      define ANSICPP
#      define R__ANSISTREAM
#      define R__PLACEMENTINLINE /* placement new/delete is inline in <new> */
#   endif
#   if defined(__ppc64__)
#      define R__B64      /* enable when 64 bit machine */
#   endif
#endif

#ifdef _HIUX_SOURCE
#   define R__HIUX
#   define R__UNIX
#   define NEED_SIGJMP
#   define NEED_SNPRINTF
#   define ANSICPP
#endif

#ifdef __GNUC__
#   define R__GNU
#   define ANSICPP
#   if __GNUC__ >= 3 || __GNUC_MINOR__ >= 90    /* egcs 1.0.3 */
#      define R__VECNEWDELETE    /* supports overloading of new[] and delete[] */
#      define R__PLACEMENTDELETE /* supports overloading placement delete */
#   endif
#   if __GNUC__ >= 3 || ( __GNUC__ == 2 && __GNUC_MINOR__ >= 95)
#         define R__PLACEMENTINLINE /* placement new/delete is inline in <new> */
#   endif
#   if __GNUC__ >= 3 || __GNUC_MINOR__ >= 91    /* egcs 1.1.x */
#      define R__ANSISTREAM      /* ANSI C++ Standard Library conformant */
#   endif
#   if __GNUC__ >= 3 && __GNUC_MINOR__ >=0 && __GNUC_MINOR__ < 8
#      define R__SSTREAM         /* use sstream or strstream header */
#   endif
#   if defined(__ia64__) &&  __GNUC__ < 3       /* gcc 2.9x (MINOR is 9!) */
#      define R__VECNEWDELETE    /* supports overloading of new[] and delete[] */
#      define R__PLACEMENTDELETE /* supports overloading placement delete */
#      define R__ANSISTREAM      /* ANSI C++ Standard Library conformant */
#   endif
#endif

#ifdef __KCC
#   define R__KCC
#   define R__ANSISTREAM      /* ANSI C++ Standard Library conformant */
#   define R__VECNEWDELETE    /* supports overloading of new[] and delete[] */
#   define R__PLACEMENTDELETE /* supports overloading placement delete */
#   define R__PLACEMENTINLINE /* placement new/delete is inline in <new> */
#   define ANSICPP
#endif

#ifdef __INTEL_COMPILER
#   define R__INTEL_COMPILER
#   define R__ANSISTREAM      /* ANSI C++ Standard Library conformant */
#   define R__VECNEWDELETE    /* supports overloading of new[] and delete[] */
#   define R__PLACEMENTDELETE /* supports overloading placement delete */
#   define R__PLACEMENTINLINE /* placement new/delete is inline in <new> */
#   define ANSICPP
#endif

#ifdef __HP_aCC
#   define R__ACC
#   define R__VECNEWDELETE    /* supports overloading of new[] and delete[] */
#   define R__PLACEMENTINLINE /* placement new/delete is inline in <new> */
#   define R__THROWNEWDELETE  /* new/delete throw exceptions */
#   if __HP_aCC <= 015000
#      define R__OLDHPACC
#      define R__TEMPLATE_OVERLOAD_BUG
#      define R__GLOBALSTL       /* STL in global name space */
#   else
#      define R__PLACEMENTDELETE /* supports overloading placement delete */
#      define R__ANSISTREAM      /* ANSI C++ Standard Library conformant */
#      define R__TMPLTSTREAM     /* iostream implemented with templates */
#   endif
#   ifndef _INCLUDE_LONGLONG
#      define _INCLUDE_LONGLONG
#   endif
#endif

#ifdef _WIN32
#   define R__WIN32
#   ifndef WIN32
#      define WIN32
#   endif
#   define R__BYTESWAP
#   define R__ACCESS_IN_SYMBOL
#endif

#ifdef __SC__
#   define SC
#   define R__SC
#   if defined(WIN32)
#      define NEED_STRING
#      define NEED_STRCASECMP
#      define NEED_SNPRINTF
#      define ANSICPP
#   else
#      define MSDOS
#      define NEED_STRCASECMP
#      define R__BYTESWAP
#   endif
#endif

#ifdef _MSC_VER
#   define R__VISUAL_CPLUSPLUS
#   define NEED_STRING
#   define NEED_STRCASECMP
#   define NEED_SNPRINTF
#   define ANSICPP
#   define R__VECNEWDELETE    /* supports overloading of new[] and delete[] */
#   define R__PLACEMENTDELETE /* supports overloading placement delete */
#   define R__PLACEMENTINLINE /* placement new/delete is inline in <new> */
#   if _MSC_VER >= 1200
#     define R__ANSISTREAM    /* ANSI C++ Standard Library conformant */
#   endif
#   if _MSC_VER < 1310
#      define R__NO_CLASS_TEMPLATE_SPECIALIZATION
#   endif
#endif

/*--- memory and object statistics -------------------------------------------*/

/* #define R__NOSTATS */

/*--- cpp --------------------------------------------------------------------*/

#ifdef ANSICPP
    /* symbol concatenation operator */
#   define _NAME1_(name) name
#   define _NAME2_(name1,name2) name1##name2
#   define _NAME3_(name1,name2,name3) name1##name2##name3

    /* stringizing */
#   define _QUOTE_(name) #name

#else

#   define _NAME1_(name) name
#   define _NAME2_(name1,name2) _NAME1_(name1)name2
#   define _NAME3_(name1,name2,name3) _NAME2_(name1,name2)name3

#   define _QUOTE_(name) "name"

#endif

/* produce an identifier that is almost unique inside a file */
#ifndef __CINT__
#   define _R__JOIN_(X,Y) _NAME2_(X,Y)
#   define _R__UNIQUE_(X) _R__JOIN_(X,__LINE__)
#else
    /* Currently CINT does not really mind to have duplicates and     */
    /* does not work correctly as far as merging tokens is concerned. */
#   define _R__UNIQUE_(X) X
#endif

/*---- misc ------------------------------------------------------------------*/

#ifdef R__GNU
#   define SafeDelete(p) { if (p) { delete p; p = 0; } }
#else
#   define SafeDelete(p) { delete p; p = 0; }
#endif

#ifdef R__WIN32
#define R__LL(longlong) longlong
#else
#define R__LL(longlong) _NAME2_(longlong,LL)
#endif

#endif
