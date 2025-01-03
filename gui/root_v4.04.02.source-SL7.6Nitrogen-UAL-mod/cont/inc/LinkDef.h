/* @(#)root/cont:$Name:  $:$Id: LinkDef.h,v 1.13 2005/03/20 19:35:50 brun Exp $ */

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifdef __CINT__

#pragma link C++ global kIterForward;
#pragma link C++ global kIterBackward;
#pragma link C++ global gClassTable;
#pragma link C++ global gObjectTable;

#pragma link C++ typedef TAssoc;

#pragma link C++ class TArray!;
#pragma link C++ class TArrayC-!;
#pragma link C++ class TArrayD-!;
#pragma link C++ class TArrayF-!;
#pragma link C++ class TArrayI-!;
#pragma link C++ class TArrayL-!;
#pragma link C++ class TArrayS-!;
#pragma link C++ class TBits+;
#pragma link C++ class TCollection-;
#pragma link C++ class TBtree-;
#pragma link C++ class TBtreeIter;
#pragma link C++ class TClassTable;
#pragma link C++ class TClonesArray-;
#pragma link C++ class THashTable;
#pragma link C++ class THashTableIter;
#pragma link C++ class TIter;
#pragma link C++ class TIterator;
#pragma link C++ class TList-;
#pragma link C++ class TListIter;
#pragma link C++ class THashList;
#pragma link C++ class TMap-;
#pragma link C++ class TMapIter;
#pragma link C++ class TPair;
#pragma link C++ class TObjArray-;
#pragma link C++ class TObjArrayIter;
#pragma link C++ class TObjectTable;
#pragma link C++ class TOrdCollection;
#pragma link C++ class TOrdCollectionIter;
#pragma link C++ class TSeqCollection;
#pragma link C++ class TSortedList;
#pragma link C++ class TExMap-;
#pragma link C++ class TExMapIter;
#pragma link C++ class TRefArray-;
#pragma link C++ class TRefArrayIter;
#pragma link C++ class TRefTable+;
#pragma link C++ class TVirtualCollectionProxy-;

#pragma link C++ nestedclass;
#pragma link C++ nestedtypedef;

#pragma link C++ class TCollectionProxy-;
#pragma link C++ class TEmulatedCollectionProxy-;
#pragma link C++ class TEmulatedMapProxy-;
#pragma link C++ class TGenCollectionProxy-;
#pragma link C++ class TGenCollectionProxy::Value-;
#pragma link C++ class TGenCollectionProxy::Method-;
#pragma link C++ class TCollectionStreamer-;
#pragma link C++ class TCollectionClassStreamer-;
#pragma link C++ class TCollectionMemberStreamer-;

#endif
