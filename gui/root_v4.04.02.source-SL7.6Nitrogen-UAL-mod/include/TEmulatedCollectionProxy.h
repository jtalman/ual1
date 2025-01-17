// @(#)root/cont:$Name:  $:$Id: TEmulatedCollectionProxy.h,v 1.2 2005/02/25 17:06:34 brun Exp $
// Author: Markus Frank  28/10/04

/*************************************************************************
 * Copyright (C) 1995-2004, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/
#ifndef ROOT_TEmulatedCollectionProxy
#define ROOT_TEmulatedCollectionProxy

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TEmulatedCollectionProxy
//
// Streamer around an arbitrary STL like container, which implements basic 
// container functionality.
//
// Note:
// Although this class contains all the setup necessary to deal
// with maps, the map-like functionality is NOT supported.
// For optimization reasons this functionality is put into
// the class TEmulatedMapProxy.
//
//////////////////////////////////////////////////////////////////////////

#include "TGenCollectionProxy.h"

class TEmulatedCollectionProxy : public TGenCollectionProxy  {

  /// Friend declaration
  friend class TCollectionProxy;

public:
  /// Container type definition
  typedef std::vector<char>  Cont_t;
  /// Pointer to container type
  typedef Cont_t            *PCont_t;
protected:

  /// Some hack to avoid const-ness
  virtual TGenCollectionProxy* InitializeEx();

  /// Object input streamer
  void ReadItems(int nElements, TBuffer &b);

  /// Object output streamer
  void WriteItems(int nElements, TBuffer &b);

  /// Shrink the container
  void Shrink(UInt_t nCurr, UInt_t left, Bool_t force);

  /// Expand the container
  void Expand(UInt_t nCurr, UInt_t left);

public:
  /// Virtual copy constructor
  virtual TVirtualCollectionProxy* Generate() const;

  /// Copy constructor
  TEmulatedCollectionProxy(const TEmulatedCollectionProxy& copy);

  /// Initializing constructor
  TEmulatedCollectionProxy(const char* cl_name);

  /// Standard destructor
  virtual ~TEmulatedCollectionProxy();

  /// Virtual constructor
  virtual void* New()   const             {  return new Cont_t;         }

  /// Virtual in-place constructor
  virtual void* New(void* memory)   const {  return new(memory) Cont_t; }

  /// TVirtualCollectionProxy overload: Return the sizeof the collection object. 
  virtual UInt_t Sizeof() const           {  return sizeof(Cont_t);     }

  /// Return the address of the value at index 'idx'
  virtual void *At(UInt_t idx);

  /// Clear the container
  virtual void Clear(const char *opt = "");

  /// Resize the container
  virtual void Resize(UInt_t n, Bool_t force_delete);

  /// Return the current size of the container
  virtual UInt_t Size() const;                        

  /// Block allocation of containees
  virtual void* Allocate(UInt_t n, Bool_t forceDelete);

  /// Block commit of containees
  virtual void Commit(void* env);

  /// Streamer for I/O handling
  virtual void Streamer(TBuffer &refBuffer);

  /// Streamer I/O overload
  virtual void Streamer(TBuffer &buff, void *pObj, int siz) {
    TGenCollectionProxy::Streamer(buff,pObj,siz);
  }
};

#endif
