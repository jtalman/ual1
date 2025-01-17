// @(#)root/proof:$Name:  $:$Id: TProofDebug.h,v 1.7 2004/12/28 20:51:25 brun Exp $
// Author: Maarten Ballintijn 19/6/2002

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TProofDebug
#define ROOT_TProofDebug


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TProofDebug                                                          //
//                                                                      //
// Detailed logging / debug scheme.                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_Rtypes
#include "Rtypes.h"
#endif

class TProofDebug {
public:
   enum EProofDebugMask {
      kNone          = 0,
      kPacketizer    = 1,
      kLoop          = 2,
      kSelector      = 4,
      kOutput        = 8,
      kInput         = 16,
      kGlobal        = 32,
      kPackage       = 64,
      kFeedback      = 128,
      kCondor        = 256,
      kDraw          = 512,

      kAll           = 0xFFFFFFFF
   };
};

R__EXTERN TProofDebug::EProofDebugMask gProofDebugMask;
R__EXTERN Int_t gProofDebugLevel;

#define PDB(mask,level) \
   if ((TProofDebug::mask & gProofDebugMask) && gProofDebugLevel >= (level))

#endif
