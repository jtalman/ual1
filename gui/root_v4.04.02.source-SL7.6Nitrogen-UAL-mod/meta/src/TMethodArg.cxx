// @(#)root/meta:$Name:  $:$Id: TMethodArg.cxx,v 1.3 2002/11/26 10:24:09 brun Exp $
// Author: Rene Brun   04/02/95

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include "TMethodArg.h"
#include "TROOT.h"
#include "TInterpreter.h"
#include "Strlen.h"
#include "Api.h"
#include "TMethod.h"
#include "TMethodCall.h"
#include "TDataMember.h"

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  Each ROOT method (see TMethod) has a linked list of its arguments.  //
//  This class describes one single method argument.                    //
//  The method argument info is obtained via the CINT api.              //
//  See class TCint.                                                    //
//                                                                      //
//  The method argument information is used a.o. in the TContextMenu    //
//  and THtml classes.                                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

ClassImp(TMethodArg)

//______________________________________________________________________________
TMethodArg::TMethodArg(G__MethodArgInfo *info, TFunction *method) : TDictionary()
{
   // Default TMethodArg ctor. TMethodArgs are constructed in TFunction
   // via a call to TCint::CreateListOfMethodArgs().

   fDataMember = 0;
   fInfo       = info;
   fMethod     = method;
   if (fInfo) {
      SetName(fInfo->Name());
      SetTitle(fInfo->Type()->Name());
   }
}

//______________________________________________________________________________
TMethodArg::~TMethodArg()
{
   // TMethodArg dtor deletes adopted G__MethodArgInfo object.

   if (fInfo) delete fInfo;
}

//______________________________________________________________________________
const char *TMethodArg::GetDefault() const
{
   // Get default value of method argument.

   return fInfo->DefaultValue();
}

//______________________________________________________________________________
const char *TMethodArg::GetTypeName() const
{
   // Get type of method argument, e.g.: "class TDirectory*" -> "TDirectory"
   // Result needs to be used or copied immediately.

   return gInterpreter->TypeName(fInfo->Type()->Name());
}

//______________________________________________________________________________
const char *TMethodArg::GetFullTypeName() const
{
   // Get full type description of method argument, e.g.: "class TDirectory*".

   return fInfo->Type()->Name();
}

//______________________________________________________________________________
Long_t TMethodArg::Property() const
{
   // Get property description word. For meaning of bits see EProperty.

   return fInfo->Property();
}

//______________________________________________________________________________
TList *TMethodArg::GetOptions() const
{
   // Returns list of possible options - according to pointed datamember.
   // If there is no datamember field assigned to this methodarg - returns 0.

   return (TList*)(fDataMember ? fDataMember->GetOptions() : 0);
}

//______________________________________________________________________________
TDataMember *TMethodArg::GetDataMember() const
{
   // Returns TDataMember pointed by this methodarg.
   // If you want to specify list of options or current value for your
   // MethodArg (i.e. it is used as initial values in argument-asking dialogs
   // popped up from context-meny),you can get this value from one of data
   // members of the class.
   // The only restriction is, that this DataMember object must have its
   // Getter/Setter methods set-up correctly - for details look at TDataMember.
   // To learn how to specify the data member to which the argument should
   // "point", look at TMethod. This is TMethod which sets up fDataMember,
   // so it could work correctly.

   return fDataMember;
}

