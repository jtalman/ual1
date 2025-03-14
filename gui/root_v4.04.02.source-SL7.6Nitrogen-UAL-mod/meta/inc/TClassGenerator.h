// @(#)root/base:$Name:  $:$Id: TClassGenerator.h,v 1.2 2003/06/25 18:06:45 rdm Exp $
// Author: Philippe Canal 24/06/2003

/*************************************************************************
 * Copyright (C) 1995-2003, Rene Brun and Fons Rademakers, and al.       *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TClassGenerator
#define ROOT_TClassGenerator

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TClassGenerator                                                      //
//                                                                      //
// Objects following this interface can be passed onto the TROOT object //
// to implement a user customized way to create the TClass objects.     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include "TObject.h"
#endif

class TClass;

class TClassGenerator : public TObject {

protected:
   TClassGenerator() : TObject() { }
   virtual ~TClassGenerator() { }

public:
   virtual TClass *GetClass(const char* classname, Bool_t load) = 0;
   virtual TClass *GetClass(const type_info& typeinfo, Bool_t load) = 0;

   ClassDef(TClassGenerator,1);  // interface for TClass generators
};

#endif
