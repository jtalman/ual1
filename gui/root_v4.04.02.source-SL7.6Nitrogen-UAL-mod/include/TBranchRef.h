// @(#)root/tree:$Name:  $:$Id: TBranchRef.h,v 1.2 2004/08/24 10:41:58 brun Exp $
// Author: Rene Brun   19/08/2004

/*************************************************************************
 * Copyright (C) 1995-2004, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TBranchRef
#define ROOT_TBranchRef


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TBranchRef                                                           //
//                                                                      //
// A Branch to support referenced objects on other branches             //
//////////////////////////////////////////////////////////////////////////


#ifndef ROOT_TBranch
#include "TBranch.h"
#endif
#ifndef ROOT_TRefTable
#include "TRefTable.h"
#endif

class TTree;

class TBranchRef : public TBranch {

protected:
    TRefTable       *fRefTable;        // pointer to the TRefTable

public:
    TBranchRef();
    TBranchRef(TTree *tree);
    virtual ~TBranchRef();
    virtual void    Clear(Option_t *option="");
    virtual Int_t   Fill();
    virtual void    FillLeaves(TBuffer &b);
    TRefTable      *GetRefTable() const {return fRefTable;}
    virtual Bool_t  Notify();
    virtual void    Print(Option_t *option="") const;
    virtual void    ReadLeaves(TBuffer &b);
    virtual void    Reset(Option_t *option="");
    virtual void    SetParent(const TObject *obj);
    virtual void    SetReadEntry(Long64_t entry) {fReadEntry = entry;}

    ClassDef(TBranchRef,1)  //to support referenced objects on other branches
};

#endif
