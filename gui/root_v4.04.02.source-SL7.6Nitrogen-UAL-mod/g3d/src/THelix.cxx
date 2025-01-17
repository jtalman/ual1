// @(#)root/g3d:$Name:  $:$Id: THelix.cxx,v 1.10 2004/08/03 16:01:18 brun Exp $
// Author: Ping Yeh   19/12/97

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//______________________________________________________________________________
// Helix is, hmmm, well, a helix.  It has 3 different constructors.
//
// Comments/suggestions/etc on this class should be sent to the author:
//             pyeh@cdfsga.fnal.gov (Ping Yeh)
//
//   If a particle with charge q passes through a point (x,y,z)
//   with momentum (px,py,pz) with magnetic field B along an axis (nx,ny,nz),
//   this helix can be constrcuted like
//
//      THelix p(x0,y0,z0, px,py,pz, q*B, nx,ny,nz);
//
//     (nx,ny,nz) defaults to (0,0,1).
//
//   A helix in its own frame can be defined with initial position
//   (x0,y0,z0) and "velocity" (vx0,vy0,vz0), both 3-vectors, and
//   an angular frequency w.  The parametric equation of the helix is
//
//    x = x0 - vt / w * sin(-w * t + phi0)
//    y = y0 + vt / w * cos(-w * t + phi0)
//    z = z0 + vz * t
//
//
//   The 'normal constructor' has 6 parameters,
//
//       Example:
//                 THelix pl1(xyz0, v0, w, range, rtype, axis);
//
//         where:
//             xyz0  : array of initial position
//             v0    : array of initial velocity
//             w     : angular frequency
//             range : helix range
//             rtype : kHelixZ specifies allowed drawing range in helix Z direction, i.e., along B field.
//                     kLabZ specifies drawing range in lab frame.
//                     kHelixX, kHelixY, kLabX, kLabY, kUnchanged ... etc can also be specified
//             axis  : helix axis
//
//
//
// A Third constructor uses several default values:
//
//       Example:
//   c1 = new TCanvas("c1");
//   TView *view = new TView(1);
//   view->SetRange(-1,-1,-1,1,1,1);
//   THelix *helix = new THelix(0.0, 0.0, 0.0, 1.0, 0.0, 0.3, 10.0);
//   helix->Draw();
//
// will initializes a helix with its axis in Z direction (rtype=kHelixZ).
// range[0] = 0 and range[1] = 1
//______________________________________________________________________________

#include "Riostream.h"
#include "TROOT.h"
#include "TVirtualPad.h"
#include "THelix.h"


Int_t THelix::fgMinNSeg=5;        // at least 5 line segments in TPolyLine3D

ClassImp(THelix)

//______________________________________________________________________________
void  THelix::SetHelix(Double_t *p,  Double_t *v,  Double_t w,
                       Double_t *range, EHelixRangeType rType,
                       Double_t *axis )
{
   // Set all helix parameters.

   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   // Define the helix frame by setting the helix axis and rotation matrix
   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   SetAxis(axis);

   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   // Calculate initial position and velocity in helix frame
   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   fW    = w;
   Double_t * M = fRotMat->GetMatrix();
   Double_t vx0, vy0, vz0;
   vx0   = v[0] * M[0] + v[1] * M[1] + v[2] * M[2];
   vy0   = v[0] * M[3] + v[1] * M[4] + v[2] * M[5];
   vz0   = v[0] * M[6] + v[1] * M[7] + v[2] * M[8];
   fVt   = TMath::Sqrt(vx0*vx0 + vy0*vy0);
   fPhi0 = TMath::ATan2(vy0,vx0);
   fVz   = vz0;
   fX0   = p[0] * M[0] +  p[1] * M[1] +  p[2] * M[2];
   fY0   = p[0] * M[3] +  p[1] * M[4] +  p[2] * M[5];
   fZ0   = p[0] * M[6] +  p[1] * M[7] +  p[2] * M[8];
   if (fW != 0) {
      fX0 += fVt / fW * TMath::Sin(fPhi0);
      fY0 -= fVt / fW * TMath::Cos(fPhi0);
   }

   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   // Then calculate the range in t and set the polyline representation
   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   Double_t r1 = 0;
   Double_t r2 = 1;
   if (range) {r1 = range[0]; r2 = range[1];}
   if (rType != kUnchanged) {
      fRange[0] = 0.0;   fRange[1] = TMath::Pi();   // initialize to half round
      SetRange(r1,r2,rType);
   }

   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   // Calculate real (x0,y0,z0)
   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   //Double_t x0 = fX0 - fVt/fW * TMath::Sin(fPhi0);
   //Double_t y0 = fY0 + fVt/fW * TMath::Cos(fPhi0);
   //Double_t z0 = fZ0;

}

//______________________________________________________________________________
THelix::THelix()
{
   // Helix default constructor.

   fX0 = fY0 = fZ0 = fVt = fPhi0 = fVz = fAxis[0] = fAxis[1] = 0.0;
   fAxis[2]  = 1.0;
   fW        = 1.5E7;   // roughly the cyclon frequency of proton in AMS
   fRange[0] = 0.0;
   fRange[1] = 1.0;
   fRotMat   = 0;
}

//______________________________________________________________________________
THelix::THelix(Double_t x,  Double_t y,  Double_t z,
               Double_t vx, Double_t vy, Double_t vz,
               Double_t w)
        : TPolyLine3D()
{
   // Helix normal constructor.
   Double_t p[3], v[3];
   p[0] = x;
   p[1] = y;
   p[2] = z;
   v[0] = vx;
   v[1] = vy;
   v[2] = vz;
   Double_t *range = 0;
   fRotMat   = 0;

   SetHelix(p, v, w, range, kHelixZ);
   fOption = "";
}


//______________________________________________________________________________
THelix::THelix(Double_t * p, Double_t * v, Double_t w,
               Double_t * range, EHelixRangeType rType, Double_t * axis)
        : TPolyLine3D()
{
   // Helix normal constructor.

   Double_t r[2];
   if ( range ) {
      r[0] = range[0];   r[1] = range[1];
   } else {
      r[0] = 0.0;        r[1] = 1.0;
   }

   fRotMat   = 0;
   if ( axis ) {                        // specify axis
      SetHelix(p, v, w, r, rType, axis);
   } else {                             // default axis
      SetHelix(p, v, w, r, rType);
   }

   fOption = "";
}

#if 0
//______________________________________________________________________________
 THelix::THelix(const THelix &h) : TPolyLine3D()
{
   // Helix copy constructor.

   fX0   = h.fX0;
   fY0   = h.fY0;
   fZ0   = h.fZ0;
   fVt   = h.fVt;
   fPhi0 = h.fPhi0;
   fVz   = h.fVz;
   fW    = h.fW;
   for (Int_t i=0; i<3; i++) fAxis[i] = h.fAxis[i];
   fRotMat = new TRotMatrix(*(h.fRotMat));
   fRange[0] = h.fRange[0];
   fRange[1] = h.fRange[1];

   fOption = h.fOption;
}
#endif

//______________________________________________________________________________
THelix::~THelix()
{
   // Helix destructor.

   if (fRotMat) delete fRotMat;
}


//______________________________________________________________________________
THelix::THelix(const THelix &helix) : TPolyLine3D(helix)
{
   fRotMat=0;
   ((THelix&)helix).THelix::Copy(*this);
}


//______________________________________________________________________________
void THelix::Copy(TObject &obj) const
{
   // Copy this helix to obj.

   TObject::Copy(obj);
   TAttLine::Copy(((THelix&)obj));

   ((THelix&)obj).fX0        = fX0;
   ((THelix&)obj).fY0        = fY0;
   ((THelix&)obj).fZ0        = fZ0;
   ((THelix&)obj).fVt        = fVt;
   ((THelix&)obj).fPhi0      = fPhi0;
   ((THelix&)obj).fVz        = fVz;
   ((THelix&)obj).fW         = fW;
   for (Int_t i=0; i<3; i++)
      ((THelix&)obj).fAxis[i] = fAxis[i];

   if (((THelix&)obj).fRotMat)
      delete ((THelix&)obj).fRotMat;
   ((THelix&)obj).fRotMat    = new TRotMatrix(*fRotMat);

   ((THelix&)obj).fRange[0]  = fRange[0];
   ((THelix&)obj).fRange[1]  = fRange[1];

   ((THelix&)obj).fOption    = fOption;

   //
   // Set range and make the graphic representation
   //
   ((THelix&)obj).SetRange(fRange[0], fRange[1], kHelixT);
}

//______________________________________________________________________________
void THelix::Draw(Option_t *option)
{
   // Draw this helix with its current attributes.

   AppendPad(option);
}

//______________________________________________________________________________
void THelix::Print(Option_t *option) const
{
   // Dump this helix with its attributes.

   cout <<"    THelix Printing N=" <<fN<<" Option="<<option<<endl;
}

//______________________________________________________________________________
void THelix::SavePrimitive(ofstream &out, Option_t *)
{
   // Save primitive as a C++ statement(s) on output stream out.

   char quote = '"';
   out<<"   "<<endl;
   if (gROOT->ClassSaved(THelix::Class())) {
      out<<"   ";
   } else {
      out<<"   THelix *";
   }
   out<<"helix = new THelix("<<fX0<<","<<fY0<<","<<fZ0<<","
      <<fVt*TMath::Cos(fPhi0)<<","<<fVt*TMath::Sin(fPhi0)<<","<<fVz<<","
      <<fW<<","<<fRange[0]<<","<<fRange[1]<<","<<(Int_t)kHelixT<<","
      <<fAxis[0]<<","<<fAxis[1]<<","<<fAxis[2]<<","
      <<quote<<fOption<<quote<<");"<<endl;

   SaveLineAttributes(out,"helix",1,1,1);

   out<<"   helix->Draw();"<<endl;
}

//______________________________________________________________________________
void THelix::SetAxis(Double_t * axis)
{
   // Set a new axis for the helix.  This will make a new rotation matrix.

   if (axis) {
      Double_t len = TMath::Sqrt(axis[0]*axis[0] + axis[1]*axis[1] + axis[2]*axis[2]);
      if (len <= 0) {
         Error("SetAxis()", "Impossible! axis length %lf <= 0!", len);
         return;
      }
      fAxis[0] = axis[0]/len;
      fAxis[1] = axis[1]/len;
      fAxis[2] = axis[2]/len;
   } else {
      fAxis[0] = 0;
      fAxis[1] = 0;
      fAxis[2] = 1;
   }

   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   // Construct the rotational matrix from the axis
   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   SetRotMatrix();

// Double_t * M = fRotMat->GetMatrix();
// printf(" matrix:  %lf   %lf   %lf\n", M[0], M[1], M[2]);
// printf("          %lf   %lf   %lf\n", M[3], M[4], M[5]);
// printf("          %lf   %lf   %lf\n", M[6], M[7], M[8]);
}

//______________________________________________________________________________
void THelix::SetAxis(Double_t x, Double_t y, Double_t z)
{
   Double_t axis[3];    axis[0] = x;    axis[1] = y;    axis[2] = z;
   SetAxis(axis);
}


//______________________________________________________________________________
void THelix::SetRange(Double_t * range, EHelixRangeType rType)
{
   // set a new range for the helix.  This will remake the polyline.
   //

   Double_t a[2];
   Double_t halfpi = TMath::Pi()/2.0;
   Int_t i;
   Double_t vx = fVt * TMath::Cos(fPhi0);
   Double_t vy = fVt * TMath::Sin(fPhi0);
   Double_t phase;

   if ( fW != 0 && fVz != 0 ) {         // general case
      switch ( rType ) {
         case kHelixT :
            fRange[0] = range[0];  fRange[1] = range[1];  break;

         case kHelixX :
            for (i=0; i<2; i++ ) {
               a[i] = fW / fVt * (range[i] - fX0);
               if ( a[i] < -1 || a[i] > 1 ) {
                  Error("SetRange()",
                        "range out of bound (%lf:%lf): %lf.  Default used: %lf",
                        fX0-fVt/fW, fX0+fVt/fW, range[i], fRange[i]);
                  return;
               }
               phase = FindClosestPhase(fPhi0+halfpi, a[i]);
               fRange[i] = ( fPhi0 + halfpi - phase ) / fW;
            }
            break;

         case kHelixY :
            for (i=0; i<2; i++ ) {
               a[i] = fW / fVt * (range[i] - fY0);
               if ( a[i] < -1 || a[i] > 1 ) {
                  Error("SetRange()",
                        "range out of bound (%lf:%lf): %lf.  Default used: %lf",
                         fY0-fVt/fW, fY0+fVt/fW, range[i], fRange[i]);
                  return;
               }
               phase = FindClosestPhase(fPhi0, a[i]);
               fRange[i] = ( fPhi0 - phase ) / fW;
            }
            break;

         case kHelixZ :
            if ( fVz != 0 ) {
               for (i=0; i<2; i++ ) {
                  fRange[i] = (range[i] - fZ0) / fVz;
               }
            } else {                // fVz = 0, z = constant = fZ0
               Error("SetRange()",
                     "Vz = 0 and attempts to set range along helix axis!");
               return;
            }
            break;

         case kLabX :
         case kLabY :
         case kLabZ :
            printf("setting range in lab axes is not implemented yet\n");
            break;
         default:
            Error("SetRange()","unknown range type %d", rType);
            break;
      }
   } else if ( fW == 0 ) {                // straight line: x = x0 + vx * t
      switch ( rType ) {
         case kHelixT :
            fRange[0] = range[0];  fRange[1] = range[1];
            break;
         case kHelixX :
            if ( vx != 0 ) {
               fRange[0] = (range[0] - fX0) / vx;
               fRange[1] = (range[1] - fX0) / vx;
            } else {
               Error("SetRange()",
                     "Vx = 0 and attempts to set range on helix x axis!");
               return;
            }
            break;
         case kHelixY :
            if ( vy != 0 ) {
               fRange[0] = (range[0] - fY0) / vy;
               fRange[1] = (range[1] - fY0) / vy;
            } else {
               Error("SetRange()",
                     "Vy = 0 and attempts to set range on helix y axis!");
               return;
            }
            break;
         case kHelixZ :
            if ( fVz != 0 ) {
               fRange[0] = (range[0] - fZ0) / fVz;
               fRange[1] = (range[1] - fZ0) / fVz;
            } else {
               Error("SetRange()",
                     "Vz = 0 and attempts to set range on helix z axis!");
               return;
            }
            break;
         case kLabX   :
         case kLabY   :
         case kLabZ   :
            printf("setting range in lab axes is not implemented yet\n");
            break;
         default      :
            Error("SetRange()","unknown range type %d", rType);
            break;
      }
   } else if ( fVz == 0 ) {               // a circle, not fully implemented yet
      switch ( rType ) {
         case kHelixT :
            fRange[0] = range[0];  fRange[1] = range[1];  break;
         case kHelixX :
            if ( vx != 0 ) {
               fRange[0] = (range[0] - fX0) / vx;
               fRange[1] = (range[1] - fX0) / vx;
            } else {
               Error("SetRange()",
                     "Vx = 0 and attempts to set range on helix x axis!");
               return;
            }
            break;
         case kHelixY :
            if ( vy != 0 ) {
               fRange[0] = (range[0] - fY0) / vy;
               fRange[1] = (range[1] - fY0) / vy;
            } else {
               Error("SetRange()",
                     "Vy = 0 and attempts to set range on helix y axis!");
               return;
            }
            break;
         case kHelixZ :
            Error("SetRange()",
                  "Vz = 0 and attempts to set range on helix z axis!");
            return;
         case kLabX   :
         case kLabY   :
         case kLabZ   :
            printf("setting range in lab axes is not implemented yet\n");
            break;
         default      :
            Error("SetRange()","unknown range type %d", rType);
            break;
      }
   }

   if ( fRange[0] > fRange[1] ) {
      Double_t temp = fRange[1];   fRange[1] = fRange[0];  fRange[0] = temp;
   }

   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   // Set the polylines in global coordinates
   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   Double_t degrad  = TMath::Pi() / 180.0;
   Double_t segment = 5.0 * degrad;             // 5 degree segments
// Double_t dt      = segment / fW;             // parameter span on each segm.
   Double_t dt      = segment / TMath::Abs(fW); // parameter span on each segm.

   Int_t    nSeg    = Int_t((fRange[1]-fRange[0]) / dt) + 1;
   if (nSeg < THelix::fgMinNSeg) {
      nSeg = THelix::fgMinNSeg;
      dt = (fRange[1]-fRange[0])/nSeg;
   }

   Double_t * xl    = new Double_t[nSeg+1];     // polyline in local coordinates
   Double_t * yl    = new Double_t[nSeg+1];
   Double_t * zl    = new Double_t[nSeg+1];

  // printf("use %d points in %lf <= t <= %lf\n", nSeg+1, fRange[0], fRange[1]);

   for (i=0; i<=nSeg; i++) {                    // calculate xl[], yl[], zl[];
      Double_t t, phase;
      if (i==nSeg) t = fRange[1];                // the last point
      else         t = fRange[0] + dt * i;
      phase = -fW * t + fPhi0;
      xl[i] = fX0 - fVt/fW * TMath::Sin(phase);
      yl[i] = fY0 + fVt/fW * TMath::Cos(phase);
      zl[i] = fZ0 + fVz * t;
   }

   Float_t xg, yg,zg;     // global coordinates
                          // must be Float_t to call TPolyLine3D::SetPoint()
   Double_t * M = fRotMat->GetMatrix();
   TPolyLine3D::SetPolyLine(nSeg+1);
   for (i=0; i<=nSeg; i++) {                    // M^{-1} = transpose of M
      xg =  xl[i] * M[0]  +  yl[i] * M[3]  +  zl[i] * M[6] ;
      yg =  xl[i] * M[1]  +  yl[i] * M[4]  +  zl[i] * M[7] ;
      zg =  xl[i] * M[2]  +  yl[i] * M[5]  +  zl[i] * M[8] ;
      TPolyLine3D::SetPoint(i,xg,yg,zg);
   }

   delete[] xl;  delete[] yl;    delete[] zl;
}

//______________________________________________________________________________
void THelix::SetRange(Double_t r1, Double_t r2, EHelixRangeType rType)
{
   Double_t range[2];
   range[0] = r1;       range[1] = r2;
   SetRange(range, rType);
}

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                                                                          //
//                  Protected     Member     Functions                      //
//                                                                          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
void THelix::SetRotMatrix()
{
   // set the rotational matrix according to the helix axis
   //

   //
   // Calculate all 6 angles.
   // Note that TRotMatrix::TRotMatrix() expects angles in degrees.
   //
   Double_t raddeg = 180.0 / TMath::Pi();
   Double_t halfpi = TMath::Pi()/2.0 * raddeg;
                                 // (theta3,phi3) is the helix axis
   Double_t theta3 = TMath::ACos(fAxis[2]) * raddeg;
   Double_t phi3   = TMath::ATan2(fAxis[1], fAxis[0]) * raddeg;
                                 //  (theta1,phi1) is the x-axis in helix frame
   Double_t theta1 = theta3 + halfpi;
   Double_t phi1   = phi3;
                                 //  (theta2,phi2) is the y-axis in helix frame
   Double_t theta2 = halfpi;
   Double_t phi2   = phi1 + halfpi;

   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   // Delete the old rotation matrix
   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   if (fRotMat) delete fRotMat;

   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   // Make a new rotation matrix
   //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
   fRotMat = new TRotMatrix("HelixRotMat", "Master frame -> Helix frame",
                            theta1, phi1,  theta2, phi2,  theta3, phi3 );
   return;
}

//______________________________________________________________________________
Double_t  THelix::FindClosestPhase(Double_t phi0,  Double_t cosine)
{
   // Finds the closest phase to phi0 that gives cos(phase) = cosine
   //

   const Double_t pi    = TMath::Pi();
   const Double_t twopi = TMath::Pi() * 2.0;
   Double_t phi1 = TMath::ACos(cosine);
   Double_t phi2 = - phi1;

   while ( phi1 - phi0 >  pi )   phi1 -= twopi;
   while ( phi1 - phi0 < -pi )   phi1 += twopi;

   while ( phi2 - phi0 >  pi )   phi2 -= twopi;
   while ( phi2 - phi0 < -pi )   phi2 += twopi;
   //
   // Now phi1, phi2 and phi0 are within the same 2pi range
   // and cos(phi1) = cos(phi2) = cosine
   //
   if ( TMath::Abs(phi1-phi0) < TMath::Abs(phi2-phi0) )  return phi1;
   else                                                  return phi2;
}

//______________________________________________________________________________
void THelix::Streamer(TBuffer &R__b)
{
   // Stream an object of class THelix.

   if (R__b.IsReading()) {
      UInt_t R__s, R__c;
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      if (R__v > 1) {
         THelix::Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
         return;
      }
      //====process old versions before automatic schema evolution
      TPolyLine3D::Streamer(R__b);
      R__b >> fX0;
      R__b >> fY0;
      R__b >> fZ0;
      R__b >> fVt;
      R__b >> fPhi0;
      R__b >> fVz;
      R__b >> fW;
      R__b.ReadStaticArray(fAxis);
      R__b >> fRotMat;
      R__b.ReadStaticArray(fRange);
      R__b.CheckByteCount(R__s, R__c, THelix::IsA());
      //====end of old versions

   } else {
      THelix::Class()->WriteBuffer(R__b,this);
   }
}
