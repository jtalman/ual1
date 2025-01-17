// @(#)root/g3d:$Name:  $:$Id: THYPE.h,v 1.2 2000/12/13 15:13:46 brun Exp $
// Author: Rene Brun   08/12/98

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/
// @(#)root/g3d:$Name:  $:$Id: THYPE.h,v 1.2 2000/12/13 15:13:46 brun Exp $
// Author: Nenad Buncic   18/09/95

#ifndef ROOT_THYPE
#define ROOT_THYPE


////////////////////////////////////////////////////////////////////////////
//                                                                        //
// THYPE                                                                  //
//                                                                        //
// HYPE is a DUMMY Root shape set equal to a TTUBE                        //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TTUBE
#include "TTUBE.h"
#endif

class THYPE : public TTUBE {

    protected:
        Float_t fPhi;        // stereo angle

    public:
        THYPE();
        THYPE(const char *name, const char *title, const char *material, Float_t rmin, Float_t rmax, Float_t dz,
               Float_t phi);
        virtual ~THYPE();

        virtual Float_t GetPhi() const {return fPhi;}

        ClassDef(THYPE,1)  //HYPE shape
};

#endif
