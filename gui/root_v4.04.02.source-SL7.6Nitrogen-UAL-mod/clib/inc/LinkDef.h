/* @(#)root/clib:$Name:  $:$Id: LinkDef.h,v 1.3 2004/10/04 23:48:41 rdm Exp $ */

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ enum EGetLineMode;

#pragma link C++ function Getline(char*);
#pragma link C++ function Getlinem(EGetLineMode,char*);
#pragma link C++ function Gl_histadd(char*);

#endif
