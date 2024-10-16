// @(#)root/tree:$Name:  $:$Id: TLeafI.h,v 1.7 2004/10/18 12:32:12 brun Exp $
// Author: Rene Brun   12/01/96

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TLeafI
#define ROOT_TLeafI


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TLeafI                                                               //
//                                                                      //
// A TLeaf for an Integer data type.                                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef ROOT_TLeaf
#include "TLeaf.h"
#endif

class TLeafI : public TLeaf {

protected:
    Int_t       fMinimum;         //Minimum value if leaf range is specified
    Int_t       fMaximum;         //Maximum value if leaf range is specified
    Int_t       *fValue;          //!Pointer to data buffer
    Int_t       **fPointer;       //!Address of pointer to data buffer

public:
    TLeafI();
    TLeafI(const char *name, const char *type);
    virtual ~TLeafI();

    virtual void    Export(TClonesArray *list, Int_t n);
    virtual void    FillBasket(TBuffer &b);
    const char     *GetTypeName() const;
    virtual Int_t   GetMaximum() const {return fMaximum;}
    virtual Int_t   GetMinimum() const {return fMinimum;}
    Double_t        GetValue(Int_t i=0) const;
    virtual void   *GetValuePointer() const {return fValue;}
    virtual void    Import(TClonesArray *list, Int_t n);
    virtual void    PrintValue(Int_t i=0) const;
    virtual void    ReadBasket(TBuffer &b);
    virtual void    ReadBasketExport(TBuffer &b, TClonesArray *list, Int_t n);
    virtual void    ReadValue(ifstream & s);
    virtual void    SetAddress(void *add=0);
    virtual void    SetMaximum(Int_t max) {fMaximum = max;}
    virtual void    SetMinimum(Int_t min) {fMinimum = min;}

    ClassDef(TLeafI,1)  //A TLeaf for an Integer data type.
};

#endif
