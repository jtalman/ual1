// @(#)root/meta:$Name:  $:$Id: TDataType.h,v 1.12 2005/01/19 18:30:58 brun Exp $
// Author: Rene Brun   04/02/95

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TDataType
#define ROOT_TDataType


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TDataType                                                            //
//                                                                      //
// Basic data type descriptor (datatype information is obtained from    //
// CINT).                                                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TDictionary
#include "TDictionary.h"
#endif
#ifndef ROOT_TString
#include "TString.h"
#endif


enum EDataType {
   kChar_t   = 1,  kUChar_t  = 11, kShort_t    = 2,  kUShort_t = 12,
   kInt_t    = 3,  kUInt_t   = 13, kLong_t     = 4,  kULong_t  = 14,
   kFloat_t  = 5,  kDouble_t =  8, kDouble32_t = 9,  kchar     = 10,
   kBool_t   = 18, kLong64_t = 16, kULong64_t  = 17, kOther_t  = -1,
   kNoType_t = 0,
   kCounter =  6,  kCharStar = 7,  kBits     = 15 /* for compatibility with TStreamerInfo */
};

class G__TypedefInfo;


class TDataType : public TDictionary {

private:
   G__TypedefInfo   *fInfo;     //pointer to CINT typedef info
   Int_t             fSize;     //size of type
   EDataType         fType;     //type id
   Long_t            fProperty; //The property information for the (potential) underlying class
   TString           fTrueName; //True name of the (potential) underlying class 

   void CheckInfo();
   void SetType(const char *name);

public:
   TDataType(G__TypedefInfo *info = 0);
   TDataType(const char *typenam);
   virtual       ~TDataType();
   Int_t          Size() const;
   Int_t          GetType() const { return (Int_t)fType; }
   const char    *GetTypeName() const;
   const char    *GetFullTypeName() const;
   const char    *AsString(void *buf) const;
   Long_t         Property() const;

   static const char *GetTypeName(EDataType type);
   static EDataType GetType(const type_info &typeinfo);

   ClassDef(TDataType,0)  //Basic data type descriptor
};

#endif

