/* @(#)root/base:$Name:  $:$Id: LinkDef4.h,v 1.2 2004/07/01 04:55:05 brun Exp $ */

/*************************************************************************
 * Copyright (C) 1995-2004, Rene Brun, Fons Rademakers and al.           *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifdef __CINT__

// This linkdef is used to generate the skeleton for the customized
// dictionary to emulated access to the templated methods TDirectory::WriteObject
// and TDirectory::ReadObjectAny

// The procedure is as follow (this should be needed only if and when the
// dictionary format changes).

//     rm base/src/ManualBase4.cxx
//     gmake base/src/ManualBase4.cxx
//     replace the implementation of the 2 wrappers by 
//         #include "Base4Body.cxx"
//     you might have to update the syntax in ManualBase4Body.cxx

#pragma link C++ function TDirectory::WriteObject(void*,const char*,Option_t*);
#pragma link C++ function TDirectory::GetObject(const char*,void*&);

#endif
