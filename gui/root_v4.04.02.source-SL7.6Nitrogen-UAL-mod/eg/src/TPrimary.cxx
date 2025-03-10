// @(#)root/eg:$Name:  $:$Id: TPrimary.cxx,v 1.2 2000/12/13 15:13:46 brun Exp $
// Author: Ola Nordmann   21/09/95

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include "TObject.h"
#include "Rtypes.h"
#include "TString.h"
#include "TAttParticle.h"
#include "TPrimary.h"
#include "TView.h"
#include "TVirtualPad.h"
#include "TPolyLine3D.h"

ClassImp(TPrimary)

//______________________________________________________________________________
TPrimary::TPrimary()
{
//
//  Primary vertex particle default constructor
//

   //do nothing

}

//______________________________________________________________________________
TPrimary::TPrimary(Int_t part, Int_t first, Int_t second, Int_t gener,
                   Double_t px, Double_t py, Double_t pz,
                   Double_t etot, Double_t vx, Double_t vy, Double_t vz,
                   Double_t time, Double_t timend, const char *type)
{
//
//  TPrimary vertex particle normal constructor
//
  fPart         = part;
  fFirstMother  = first;
  fSecondMother = second;
  fGeneration   = gener;
  fPx           = px;
  fPy           = py;
  fPz           = pz;
  fEtot         = etot;
  fVx           = vx;
  fVy           = vy;
  fVz           = vz;
  fTime         = time;
  fTimeEnd      = timend;
  fType         = type;
}

//______________________________________________________________________________
TPrimary::~TPrimary()
{
//
//   Primaray vertex particle default destructor
//

   //do nothing
}


//______________________________________________________________________________
Int_t TPrimary::DistancetoPrimitive(Int_t px, Int_t py)
{
//*-*-*-*-*-*-*-*Compute distance from point px,py to a primary track*-*-*-*
//*-*            ====================================================
//*-*
//*-*  Compute the closest distance of approach from point px,py to each segment
//*-*  of a track.
//*-*  The distance is computed in pixels units.
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

   const Int_t big = 9999;
   Float_t xv[3], xe[3], xndc[3];
   Float_t rmin[3], rmax[3];
   TView *view = gPad->GetView();
   if(!view) return big;

   // compute first and last point in pad coordinates
   Float_t pmom = TMath::Sqrt(fPx*fPx+fPy*fPy+fPz*fPz);
   if (pmom == 0) return big;
   view->GetRange(rmin,rmax);
   Float_t rbox = rmax[2];
   xv[0] = fVx;
   xv[1] = fVy;
   xv[2] = fVz;
   xe[0] = fVx+rbox*fPx/pmom;
   xe[1] = fVy+rbox*fPy/pmom;
   xe[2] = fVz+rbox*fPz/pmom;
   view->WCtoNDC(xv, xndc);
   Float_t x1 = xndc[0];
   Float_t y1 = xndc[1];
   view->WCtoNDC(xe, xndc);
   Float_t x2 = xndc[0];
   Float_t y2 = xndc[1];

   return DistancetoLine(px,py,x1,y1,x2,y2);
}


//______________________________________________________________________________
void TPrimary::ExecuteEvent(Int_t, Int_t, Int_t)
{
//*-*-*-*-*-*-*-*-*-*-*Execute action corresponding to one event*-*-*-*
//*-*                  =========================================

   gPad->SetCursor(kPointer);
}

//______________________________________________________________________________
const char *TPrimary::GetName() const
{
   static char def[4] = "XXX";
   const TAttParticle *ap = GetParticle();
   if (ap) return ap->GetName();
   else    return def;
}

//______________________________________________________________________________
const TAttParticle *TPrimary::GetParticle() const
{
//
//  returning a pointer to the particle attributes
//
  if (!TAttParticle::fgList) TAttParticle::DefinePDG();
  return TAttParticle::GetParticle(fPart);
}

//______________________________________________________________________________
const char *TPrimary::GetTitle() const
{
   static char title[128];
   Float_t pmom = TMath::Sqrt(fPx*fPx+fPy*fPy+fPz*fPz);
   sprintf(title,"pmom=%f GeV",pmom);
   return title;
}

//______________________________________________________________________________
void TPrimary::Paint(Option_t *option)
{
//
//  Paint a primary track
//
   Float_t rmin[3], rmax[3];
   static TPolyLine3D *pline = 0;
   if (!pline) {
      pline = new TPolyLine3D(2);
   }
   Float_t pmom = TMath::Sqrt(fPx*fPx+fPy*fPy+fPz*fPz);
   if (pmom == 0) return;
   TView *view = gPad->GetView();
   if (!view) return;
   view->GetRange(rmin,rmax);
   Float_t rbox = rmax[2];
   pline->SetPoint(0,fVx, fVy, fVz);
   Float_t xend = fVx+rbox*fPx/pmom;
   Float_t yend = fVy+rbox*fPy/pmom;
   Float_t zend = fVz+rbox*fPz/pmom;
   pline->SetPoint(1, xend, yend, zend);
   pline->SetLineColor(GetLineColor());
   pline->SetLineStyle(GetLineStyle());
   pline->SetLineWidth(GetLineWidth());
   pline->Paint(option);
}

//______________________________________________________________________________
void TPrimary::Print(Option_t *) const
{
//
//  Print the internals of the primary vertex particle
//
   char def[8] = "XXXXXXX";
   const char *name;
   TAttParticle *ap = (TAttParticle*)GetParticle();
   if (ap) name = ap->GetName();
   else    name = def;
   Printf("TPrimary: %-13s  p: %8f %8f %8f Vertex: %8e %8e %8e %5d %5d %s",
   name,fPx,fPy,fPz,fVx,fVy,fVz,
   fFirstMother,fSecondMother,fType.Data());
}

//______________________________________________________________________________
void TPrimary::Sizeof3D() const
{
//*-*-*-*-*-*Return total X3D size of this primary*-*-*-*-*-*-*
//*-*        =====================================

   Float_t pmom = TMath::Sqrt(fPx*fPx+fPy*fPy+fPz*fPz);
   if (pmom == 0) return;
   Int_t npoints = 2;
   gSize3D.numPoints += npoints;
   gSize3D.numSegs   += (npoints-1);
   gSize3D.numPolys  += 0;

}

