/* @(#)root/minuit:$Name:  $:$Id: LinkDef.h,v 1.3 2005/03/04 09:06:37 brun Exp $ */

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

#pragma link C++ global gMinuit;

#pragma link C++ class TMinuit+;
#pragma link C++ class TFitter+;
#pragma link C++ class TLinearFitter+;

#endif
