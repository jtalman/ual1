// @(#)root/g3d:$Name:  $:$Id: TNodeDiv.cxx,v 1.1.1.1 2000/05/16 17:00:43 rdm Exp $
// Author: Rene Brun   14/09/95

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include "TROOT.h"
#include "TVirtualPad.h"
#include "TNodeDiv.h"

ClassImp(TNodeDiv)


//______________________________________________________________________________
TNodeDiv::TNodeDiv()
{
//*-*-*-*-*-*-*-*-*-*-*NodeDiv default constructor*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                  ===========================

   fNdiv = 0;
   fAxis = 0;
}

//______________________________________________________________________________
TNodeDiv::TNodeDiv(const char *name, const char *title, const char *shapename, Int_t ndiv, Int_t axis, Option_t *option)
         :TNode(name,title,shapename,0,0,0,0,option)
{
//*-*-*-*-*-*-*-*-*-*-*NodeDiv normal constructor*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                  ==========================
//*-*
//*-*    name    is the name of the node
//*-*    title   is title
//*-*    shapename is the name of the referenced shape
//*-*    x,y,z   are the offsets of the volume with respect to his mother
//*-*    matrixname  is the name of the rotation matrix
//*-*
//*-*    This new node is added into the list of sons of the current node
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

   fNdiv = ndiv;
   fAxis = axis;
}


//______________________________________________________________________________
TNodeDiv::TNodeDiv(const char *name, const char *title, TShape *shape, Int_t ndiv, Int_t axis, Option_t *option)
         :TNode(name,title,shape,0,0,0,0,option)
{
//*-*-*-*-*-*-*-*-*-*-*NodeDiv normal constructor*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                  =========================
//*-*
//*-*    name    is the name of the node
//*-*    title   is title
//*-*    shape   is the pointer to the shape definition
//*-*    ndiv    number of divisions
//*-*    axis    number of the axis for the division
//*-*
//*-*    This new node is added into the list of sons of the current node
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

   fNdiv = ndiv;
   fAxis = axis;
}

//______________________________________________________________________________
TNodeDiv::~TNodeDiv()
{
//*-*-*-*-*-*-*-*-*-*-*NodeDiv default destructor*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                  ==========================

}

//______________________________________________________________________________
void TNodeDiv::Draw(Option_t *)
{
//*-*-*-*-*-*-*-*-*-*-*-*Draw Referenced node with current parameters*-*-*-*
//*-*                   =============================================

}

//______________________________________________________________________________
void TNodeDiv::Paint(Option_t *)
{
//*-*-*-*-*-*-*-*-*-*-*-*Paint Referenced node with current parameters*-*-*-*
//*-*                   ==============================================

}
