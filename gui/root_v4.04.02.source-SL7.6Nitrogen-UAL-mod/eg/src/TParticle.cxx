// @(#)root/eg:$Name:  $:$Id: TParticle.cxx,v 1.10 2003/09/01 09:39:39 brun Exp $
// Author: Rene Brun , Federico Carminati  26/04/99

#include "TView.h"
#include "TVirtualPad.h"
#include "TPolyLine3D.h"
#include "TDatabasePDG.h"
#include "TParticle.h"

ClassImp(TParticle)

//______________________________________________________________________________
TParticle::TParticle() :
  fPdgCode(0), fStatusCode(0), fWeight(0),fCalcMass(0), fPx(0), fPy(0),
  fPz(0), fE(0), fVx(0), fVy(0), fVz(0), fVt(0), fPolarTheta(0), fPolarPhi(0)
{
  fMother[0]   = 0;
  fMother[1]   = 0;
  fDaughter[0] = 0;
  fDaughter[1] = 0;
  fParticlePDG = 0;
}

//______________________________________________________________________________
TParticle::TParticle(Int_t pdg,       Int_t status,
                     Int_t mother1,   Int_t mother2,
                     Int_t daughter1, Int_t daughter2,
                     Double_t px, Double_t py, Double_t pz, Double_t etot,
                     Double_t vx, Double_t vy, Double_t vz, Double_t time):
  fPdgCode(pdg), fStatusCode(status), fWeight(1.),fPx(px), fPy(py),
  fPz(pz), fE(etot), fVx(vx), fVy(vy), fVz(vz), fVt(time)
{
  fMother[0]   = mother1;
  fMother[1]   = mother2;
  fDaughter[0] = daughter1;
  fDaughter[1] = daughter2;

  SetPolarisation(0,0,0);

  fParticlePDG = TDatabasePDG::Instance()->GetParticle(pdg);
  if (fParticlePDG) {
     fCalcMass    = fParticlePDG->Mass();
  } else {
     Double_t a2 = fE*fE -fPx*fPx -fPy*fPy -fPz*fPz;
     if (a2 >= 0) fCalcMass =  TMath::Sqrt(a2);
     else         fCalcMass = -TMath::Sqrt(-a2);
  }
}

//______________________________________________________________________________
TParticle::TParticle(Int_t pdg,       Int_t status,
                     Int_t mother1,   Int_t mother2,
                     Int_t daughter1, Int_t daughter2,
                     const TLorentzVector &p,
                     const TLorentzVector &v) :
  fPdgCode(pdg), fStatusCode(status), fWeight(1.),fPx(p.Px()), fPy(p.Py()),
  fPz(p.Pz()), fE(p.E()), fVx(v.X()), fVy(v.Y()), fVz(v.Z()), fVt(v.T())
{
  fMother[0]   = mother1;
  fMother[1]   = mother2;
  fDaughter[0] = daughter1;
  fDaughter[1] = daughter2;

  SetPolarisation(0,0,0);

  fParticlePDG = TDatabasePDG::Instance()->GetParticle(pdg);
  if (fParticlePDG) {
     fCalcMass    = fParticlePDG->Mass();
  } else {
     Double_t a2 = fE*fE -fPx*fPx -fPy*fPy -fPz*fPz;
     if (a2 >= 0) fCalcMass =  TMath::Sqrt(a2);
     else         fCalcMass = -TMath::Sqrt(-a2);
  }
}

//______________________________________________________________________________
TParticle::TParticle(const TParticle &p) : TObject(p), TAttLine(p), TAtt3D(p)
{
    // copy constructor

   *this = p;
}

//______________________________________________________________________________
TParticle::~TParticle()
{
}

//______________________________________________________________________________
Int_t TParticle::DistancetoPrimitive(Int_t px, Int_t py)
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
   Float_t pmom = this->P();
   if (pmom == 0) return big;
   view->GetRange(rmin,rmax);
   Float_t rbox = rmax[2];
   xv[0] = fVx;
   xv[1] = fVy;
   xv[2] = fVz;
   xe[0] = xv[0]+rbox*fPx/pmom;
   xe[1] = xv[1]+rbox*fPy/pmom;
   xe[2] = xv[2]+rbox*fPz/pmom;
   view->WCtoNDC(xv, xndc);
   Float_t x1 = xndc[0];
   Float_t y1 = xndc[1];
   view->WCtoNDC(xe, xndc);
   Float_t x2 = xndc[0];
   Float_t y2 = xndc[1];

   return DistancetoLine(px,py,x1,y1,x2,y2);
}


//______________________________________________________________________________
void TParticle::ExecuteEvent(Int_t, Int_t, Int_t)
{
//*-*-*-*-*-*-*-*-*-*-*Execute action corresponding to one event*-*-*-*
//*-*                  =========================================

   gPad->SetCursor(kPointer);
}

//______________________________________________________________________________
const char* TParticle::GetName() const {
   static char def[4] = "XXX";
   const TParticlePDG *ap = TDatabasePDG::Instance()->GetParticle(fPdgCode);
   if (ap) return ap->GetName();
   else    return def;
}


//______________________________________________________________________________
TParticlePDG*  TParticle::GetPDG(Int_t mode)
{
// returns a pointer to the TParticlePDG object using the pdgcode
// if mode == 0 (default) always get a fresh value for the pointer.
// if mode != 0 this function returns directly the previously
//              computed pointer from a previous call
// One can use mode=1 (faster) when the TParticle object is not part of a
// TClonesArray used in split mode in a Root TTree.

  if (!mode || !fParticlePDG) {
     (((TParticle*)this)->fParticlePDG = TDatabasePDG::Instance()->GetParticle(fPdgCode));
     // when mutable will be allowed, change above line to the following line
     //   fParticlePDG = TDatabasePDG::Instance()->GetParticle(fPdgCode);}
  }
  return fParticlePDG;
}

//______________________________________________________________________________
void TParticle::GetPolarisation(TVector3 &v)
{
  if(fPolarTheta == -99 && fPolarPhi == -99)
    //No polarisation to return
    v.SetXYZ(0.,0.,0.);
  else
    v.SetXYZ(TMath::Cos(fPolarPhi)*TMath::Sin(fPolarTheta),
             TMath::Sin(fPolarPhi)*TMath::Sin(fPolarTheta),
             TMath::Cos(fPolarTheta));
}

//______________________________________________________________________________
const char *TParticle::GetTitle() const
{
   static char def[4] = "XXX";
   const TParticlePDG *ap = TDatabasePDG::Instance()->GetParticle(fPdgCode);
   if (ap) return ap->GetTitle();
   else    return def;
}

//______________________________________________________________________________
void TParticle::Paint(Option_t *option)
{
//
//  Paint a primary track
//
   Float_t rmin[3], rmax[3];
   static TPolyLine3D *pline = 0;
   if (!pline) {
      pline = new TPolyLine3D(2);
   }
   Float_t pmom = this->P();
   if (pmom == 0) return;
   TView *view = gPad->GetView();
   if (!view) return;
   view->GetRange(rmin,rmax);
   Float_t rbox = rmax[2];
   pline->SetPoint(0,Vx(), Vy(), Vz());
   Float_t xend = Vx()+rbox*Px()/pmom;
   Float_t yend = Vy()+rbox*Py()/pmom;
   Float_t zend = Vz()+rbox*Pz()/pmom;
   pline->SetPoint(1, xend, yend, zend);
   pline->SetLineColor(GetLineColor());
   pline->SetLineStyle(GetLineStyle());
   pline->SetLineWidth(GetLineWidth());
   pline->Paint(option);
}

//______________________________________________________________________________
void TParticle::Print(Option_t *) const
{
//
//  Print the internals of the primary vertex particle
//
   //TParticlePDG* pdg = ((TParticle*)this)->GetPDG();
   Printf("TParticle: %-13s  p: %8f %8f %8f Vertex: %8e %8e %8e %5d %5d",
          GetName(),Px(),Py(),Pz(),Vx(),Vy(),Vz(),
          fMother[0],fMother[1]);
}

//______________________________________________________________________________
void TParticle::SetPolarisation(Double_t polx, Double_t poly, Double_t polz)
{
  if(polx || poly || polz) {
    fPolarTheta = TMath::ACos(polz/TMath::Sqrt(polx*polx+poly*poly+polz*polz));
    fPolarPhi   = TMath::Pi()+TMath::ATan2(-poly,-polx);
  } else {
    fPolarTheta = -99;
    fPolarPhi = -99;
  }
}

//______________________________________________________________________________
void TParticle::Sizeof3D() const
{
//*-*-*-*-*-*Return total X3D size of this primary*-*-*-*-*-*-*
//*-*        =====================================

   Float_t pmom = this->P();
   if (pmom == 0) return;
   Int_t npoints = 2;
   gSize3D.numPoints += npoints;
   gSize3D.numSegs   += (npoints-1);
   gSize3D.numPolys  += 0;

}

//______________________________________________________________________________
void TParticle::Streamer(TBuffer &R__b)
{
   // Stream an object of class TParticle.

   if (R__b.IsReading()) {
      UInt_t R__s, R__c;
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
      if (R__v > 1) {
         TParticle::Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
         fParticlePDG = TDatabasePDG::Instance()->GetParticle(fPdgCode);
         return;
      }
      //====process old versions before automatic schema evolution
      TObject::Streamer(R__b);
      TAttLine::Streamer(R__b);
      R__b >> fPdgCode;
      R__b >> fStatusCode;
      R__b.ReadStaticArray(fMother);
      R__b.ReadStaticArray(fDaughter);
      R__b >> fWeight;
      R__b >> fCalcMass;
      R__b >> fPx;
      R__b >> fPy;
      R__b >> fPz;
      R__b >> fE;
      R__b >> fVx;
      R__b >> fVy;
      R__b >> fVz;
      R__b >> fVt;
      R__b >> fPolarTheta;
      R__b >> fPolarPhi;
      fParticlePDG = TDatabasePDG::Instance()->GetParticle(fPdgCode);
      R__b.CheckByteCount(R__s, R__c, TParticle::IsA());
      //====end of old versions
      
   } else {
      TParticle::Class()->WriteBuffer(R__b,this);
   }
}
