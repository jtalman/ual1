// @(#)rooeeplayer:$Name:  $:$Id: TFriendProxyDescriptor.h,v 1.2 2005/02/07 18:02:37 rdm Exp $
// Author: Philippe Canal 06/06/2004

/*************************************************************************
 * Copyright (C) 1995-2004, Rene Brun and Fons Rademakers and al.        *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TFriendProxyDescriptor
#define ROOT_TFriendProxyDescriptor

#ifndef ROOT_TNamed
#include "TNamed.h"
#endif


namespace ROOT {

   class TFriendProxyDescriptor : public TNamed {

      Bool_t fDuplicate;
      Int_t  fIndex;
      TList  fListOfTopProxies;

   public:
      TFriendProxyDescriptor(const char *treename, const char *aliasname, Int_t index);

      Int_t  GetIndex() const { return fIndex; }
      TList *GetListOfTopProxies() { return &fListOfTopProxies; }

      Bool_t IsEquivalent(const TFriendProxyDescriptor *other);

      void OutputClassDecl(FILE *hf, int offset, UInt_t maxVarname);
      void OutputDecl(FILE *hf, int offset, UInt_t maxVarname);

      Bool_t IsDuplicate() { return fDuplicate; }
      void   SetDuplicate() { fDuplicate = kTRUE; }

      ClassDef(TFriendProxyDescriptor,0);
   };
}

#endif
