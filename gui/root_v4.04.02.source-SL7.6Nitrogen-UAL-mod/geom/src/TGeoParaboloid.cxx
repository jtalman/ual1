// @(#)root/geom:$Name:  $:$Id: TGeoParaboloid.cxx,v 1.15 2005/03/09 18:19:26 brun Exp $
// Author: Mihaela Gheata   20/06/04

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

// TGeoParaboloid - Paraboloid  class. A paraboloid is the solid bounded by
//            the following surfaces:
//            - 2 planes parallel with XY cutting the Z axis at Z=-dz and Z=+dz
//            - the surface of revolution of a parabola described by:
//                 z = a*(x*x + y*y) + b
//       The parameters a and b are automatically computed from:
//            - rlo - the radius of the circle of intersection between the 
//              parabolic surface and the plane z = -dz
//            - rhi - the radius of the circle of intersection between the 
//              parabolic surface and the plane z = +dz
//         | -dz = a*rlo*rlo + b
//         |  dz = a*rhi*rhi + b      where: rlo != rhi, both >= 0

#include "Riostream.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TVirtualGeoPainter.h"
#include "TGeoParaboloid.h"
#include "TVirtualPad.h"
#include "TBuffer3D.h"
#include "TBuffer3DTypes.h"

ClassImp(TGeoParaboloid)
   
//_____________________________________________________________________________
TGeoParaboloid::TGeoParaboloid()
{
// Dummy constructor
   fRlo = 0;
   fRhi = 0;
   fDz  = 0;
   fA   = 0;
   fB   = 0;
   SetShapeBit(TGeoShape::kGeoParaboloid);
}   

//_____________________________________________________________________________
TGeoParaboloid::TGeoParaboloid(Double_t rlo, Double_t rhi, Double_t dz)
           :TGeoBBox(0,0,0)
{
// Default constructor specifying X and Y semiaxis length
   fRlo = 0;
   fRhi = 0;
   fDz  = 0;
   fA   = 0;
   fB   = 0;
   SetShapeBit(TGeoShape::kGeoParaboloid);
   SetParaboloidDimensions(rlo, rhi, dz);
   ComputeBBox();
}

//_____________________________________________________________________________
TGeoParaboloid::TGeoParaboloid(const char *name, Double_t rlo, Double_t rhi, Double_t dz)
           :TGeoBBox(name, 0, 0, 0)
{
// Default constructor specifying X and Y semiaxis length
   fRlo = 0;
   fRhi = 0;
   fDz  = 0;
   fA   = 0;
   fB   = 0;
   SetShapeBit(TGeoShape::kGeoParaboloid);
   SetParaboloidDimensions(rlo, rhi, dz);
   ComputeBBox();
}

//_____________________________________________________________________________
TGeoParaboloid::TGeoParaboloid(Double_t *param)
{
// Default constructor specifying minimum and maximum radius
// param[0] =  rlo
// param[1] =  rhi
// param[2] = dz
   SetShapeBit(TGeoShape::kGeoParaboloid);
   SetDimensions(param);
   ComputeBBox();
}

//_____________________________________________________________________________
TGeoParaboloid::~TGeoParaboloid()
{
// destructor
}

//_____________________________________________________________________________   
void TGeoParaboloid::ComputeBBox()
{
// compute bounding box of the tube
   fDX = TMath::Max(fRlo, fRhi);
   fDY = fDX;
   fDZ = fDz;
}   

//_____________________________________________________________________________   
void TGeoParaboloid::ComputeNormal(Double_t *point, Double_t *dir, Double_t *norm)
{
// Compute normal to closest surface from POINT.
   if ((TMath::Abs(point[2])-fDz) > -1E-5) {
      norm[0] = norm[1] = 0.0;
      norm[2] = TMath::Sign(1., dir[2]);
      return;
   }   
   Double_t talf = 2.*TMath::Sqrt(fA*(point[2]-fB))*TMath::Sign(1.0, fA);
   Double_t calf = 1./TMath::Sqrt(1.+talf*talf);
   Double_t salf = talf * calf;
   Double_t phi = TMath::ATan2(point[1], point[0]);
   if (phi<0) phi+=2.*TMath::Pi();
   Double_t cphi = TMath::Cos(phi);
   Double_t sphi = TMath::Sin(phi);
   Double_t ct   = - calf*TMath::Sign(1.,fA);
   Double_t st   = salf*TMath::Sign(1.,fA); 

   norm[0] = st*cphi;
   norm[1] = st*sphi;
   norm[2] = ct;
   Double_t ndotd = norm[0]*dir[0]+norm[1]*dir[1]+norm[2]*dir[2];
   if (ndotd < 0) {
      norm[0] = -norm[0];
      norm[1] = -norm[1];
      norm[2] = -norm[2];
   }   
}

//_____________________________________________________________________________
Bool_t TGeoParaboloid::Contains(Double_t *point) const
{
// test if point is inside the elliptical tube
   if (TMath::Abs(point[2])>fDz) return kFALSE;
   Double_t aa = fA*(point[2]-fB);
   if (aa < 0) return kFALSE;
   Double_t rsq = point[0]*point[0]+point[1]*point[1];
   if (aa < fA*fA*rsq) return kFALSE;
   return kTRUE;
}

//_____________________________________________________________________________
Int_t TGeoParaboloid::DistancetoPrimitive(Int_t px, Int_t py)
{
// compute closest distance from point px,py to each vertex
   Int_t n = gGeoManager->GetNsegments();
   const Int_t numPoints=n*(n+1)+2;
   return ShapeDistancetoPrimitive(numPoints, px, py);
}

//_____________________________________________________________________________
Double_t TGeoParaboloid::DistToParaboloid(Double_t *point, Double_t *dir) const
{
// Compute distance from a point to the parabola given by:
//  z = a*rsq + b;   rsq = x*x+y*y
   Double_t a = fA * (dir[0]*dir[0] + dir[1]*dir[1]);
   Double_t b = 2.*fA*(point[0]*dir[0]+point[1]*dir[1])-dir[2];
   Double_t c = fA*(point[0]*point[0]+point[1]*point[1]) + fB - point[2];
   Double_t dist = TGeoShape::Big();
   if (a==0) {
      if (b==0) return dist; // big
      dist = -c/b;
      if (dist < 0) return TGeoShape::Big();
      return dist; // OK
   }
   Double_t ainv = 1./a;
   Double_t sum = - b*ainv;
   Double_t prod = c*ainv;
   Double_t delta = sum*sum - 4.*prod;
   if (delta<0) return dist; // big
   if (prod == 0) return 0.;
   if (prod < 0) {
      dist = 0.5*(sum + TMath::Sqrt(delta));
      return dist; // OK
   }
   if (sum < 0) return dist; // big
   dist = 0.5 * (sum - TMath::Sqrt(delta));
   return dist; // OK
}      

//_____________________________________________________________________________
Double_t TGeoParaboloid::DistFromInside(Double_t *point, Double_t *dir, Int_t iact, Double_t step, Double_t *safe) const
{
// compute distance from inside point to surface of the paraboloid
   if (iact<3 && safe) {
   // compute safe distance
      *safe = Safety(point, kTRUE);
      if (iact==0) return TGeoShape::Big();
      if (iact==1 && step<*safe) return TGeoShape::Big();
   }
   
   Double_t dz = TGeoShape::Big();
   if (dir[2]<0) {
      dz = -(point[2]+fDz)/dir[2];
   } else if (dir[2]>0) {
      dz = (fDz-point[2])/dir[2];
   }      
   Double_t dpara = DistToParaboloid(point, dir);
   return TMath::Min(dz, dpara);
}

//_____________________________________________________________________________
Double_t TGeoParaboloid::DistFromOutside(Double_t *point, Double_t *dir, Int_t iact, Double_t step, Double_t *safe) const
{
// compute distance from outside point to surface of the paraboloid and safe distance
   Double_t snxt = TGeoShape::Big();
   if (iact<3 && safe) {
   // compute safe distance
      *safe = Safety(point, kFALSE);
      if (iact==0) return TGeoShape::Big();
      if (iact==1 && step<*safe) return TGeoShape::Big();
   }
   Double_t xnew, ynew, znew;
   if (point[2]<=-fDz) {
      if (dir[2]<=0) return TGeoShape::Big();
      snxt = -(fDz+point[2])/dir[2];
      // find extrapolated X and Y
      xnew = point[0]+snxt*dir[0];
      ynew = point[1]+snxt*dir[1];
      if ((xnew*xnew+ynew*ynew) <= fRlo*fRlo) return snxt;
   } else if (point[2]>=fDz) {
      if (dir[2]>=0) return TGeoShape::Big();
      snxt = (fDz-point[2])/dir[2];
      // find extrapolated X and Y
      xnew = point[0]+snxt*dir[0];
      ynew = point[1]+snxt*dir[1];
      if ((xnew*xnew+ynew*ynew) <= fRhi*fRhi) return snxt;
   }
   snxt = DistToParaboloid(point, dir);
   if (snxt > 1E20) return snxt;
   znew = point[2]+snxt*dir[2];
   if (TMath::Abs(znew) <= fDz) return snxt;
   return TGeoShape::Big();
}

//_____________________________________________________________________________
TGeoVolume *TGeoParaboloid::Divide(TGeoVolume * /*voldiv*/, const char * /*divname*/, Int_t /*iaxis*/, Int_t /*ndiv*/, 
                             Double_t /*start*/, Double_t /*step*/) 
{
   Error("Divide", "Paraboloid divisions not implemented");
   return 0;
}   

//_____________________________________________________________________________
void TGeoParaboloid::GetBoundingCylinder(Double_t *param) const
{
//--- Fill vector param[4] with the bounding cylinder parameters. The order
// is the following : Rmin, Rmax, Phi1, Phi2
   param[0] = 0.;                  // Rmin
   param[1] = fDX;                 // Rmax
   param[1] *= param[1];
   param[2] = 0.;                  // Phi1
   param[3] = 360.;                // Phi2 
}   

//_____________________________________________________________________________
TGeoShape *TGeoParaboloid::GetMakeRuntimeShape(TGeoShape *, TGeoMatrix *) const
{
// in case shape has some negative parameters, these has to be computed
// in order to fit the mother
   return 0;
}

//_____________________________________________________________________________
void TGeoParaboloid::InspectShape() const
{
// print shape parameters
   printf("*** Shape %s: TGeoParaboloid ***\n", GetName());
   printf("    rlo    = %11.5f\n", fRlo);
   printf("    rhi    = %11.5f\n", fRhi);
   printf("    dz     = %11.5f\n", fDz);
   printf(" Bounding box:\n");
   TGeoBBox::InspectShape();
}

//_____________________________________________________________________________
TBuffer3D *TGeoParaboloid::MakeBuffer3D() const
{ 
   // Creates a TBuffer3D describing *this* shape.
   // Coordinates are in local reference frame.

   Int_t n = gGeoManager->GetNsegments();
   Int_t NbPnts = n*(n+1)+2;
   Int_t NbSegs = n*(2*n+3);
   Int_t NbPols = n*(n+2);

   TBuffer3D* buff = new TBuffer3D(TBuffer3DTypes::kGeneric,
                                   NbPnts, 3*NbPnts, NbSegs, 3*NbSegs, NbPols, 2*n*5 + n*n*6);

   if (buff)
   {
      SetPoints(buff->fPnts);
      SetSegsAndPols(*buff);
   }

   return buff; 
}

//_____________________________________________________________________________
void TGeoParaboloid::SetSegsAndPols(TBuffer3D &buff) const
{
// Fill TBuffer3D structure for segments and polygons.
   Int_t indx, i, j;
   Int_t n = gGeoManager->GetNsegments();

   Int_t c = GetBasicColor();

   Int_t nn1 = (n+1)*n+1;
   indx = 0;
   // Lower end-cap (n radial segments)
   for (j=0; j<n; j++) {
      buff.fSegs[indx++] = c+2;
      buff.fSegs[indx++] = 0;
      buff.fSegs[indx++] = j+1;
   }
   // Sectors (n)
   for (i=0; i<n+1; i++) {
      // lateral (circles) segments (n)
      for (j=0; j<n; j++) {
         buff.fSegs[indx++] = c;
         buff.fSegs[indx++] = n*i+1+j;
         buff.fSegs[indx++] = n*i+1+((j+1)%n);
      }
      if (i==n) break;  // skip i=n for generators
      // generator segments (n)
      for (j=0; j<n; j++) {
         buff.fSegs[indx++] = c;
         buff.fSegs[indx++] = n*i+1+j;
         buff.fSegs[indx++] = n*(i+1)+1+j;
      }
   }
   // Upper end-cap
   for (j=0; j<n; j++) {
      buff.fSegs[indx++] = c+1;
      buff.fSegs[indx++] = n*n+1+j;
      buff.fSegs[indx++] = nn1;
   }

   indx = 0;

   // lower end-cap (n polygons)
   for (j=0; j<n; j++) {
      buff.fPols[indx++] = c+2;
      buff.fPols[indx++] = 3;
      buff.fPols[indx++] = n+j;
      buff.fPols[indx++] = (j+1)%n;
      buff.fPols[indx++] = j;
   }
   // Sectors (n)
   for (i=0; i<n; i++) {
      // lateral faces (n)
      for (j=0; j<n; j++) {
         buff.fPols[indx++] = c;
         buff.fPols[indx++] = 4;
         buff.fPols[indx++] = (2*i+1)*n+j;
         buff.fPols[indx++] = 2*(i+1)*n+j;
         buff.fPols[indx++] = (2*i+3)*n+j;
         buff.fPols[indx++] = 2*(i+1)*n+((j+1)%n);
      }
   }
   // upper end-cap (n polygons)
   for (j=0; j<n; j++) {
      buff.fPols[indx++] = c+1;
      buff.fPols[indx++] = 3;
      buff.fPols[indx++] = 2*n*(n+1)+j;
      buff.fPols[indx++] = 2*n*(n+1)+((j+1)%n);
      buff.fPols[indx++] = (2*n+1)*n+j;
   }
}

//_____________________________________________________________________________
Double_t TGeoParaboloid::Safety(Double_t * /*point*/, Bool_t /*in*/) const
{
// computes the closest distance from given point to this shape, according
// to option. The matching point on the shape is stored in spoint.
   return TGeoShape::Big();
}

//_____________________________________________________________________________
void TGeoParaboloid::SetParaboloidDimensions(Double_t rlo, Double_t rhi, Double_t dz)
{
   if ((rlo<0) || (rlo<0) || (dz<=0) || (rlo==rhi)) {
      SetShapeBit(kGeoRunTimeShape);
      Error("SetParaboloidDimensions", "Dimensions of %s invalid: check (rlo>=0) (rhi>=0) (rlo!=rhi) dz>0",GetName());
      return;
   }
   fRlo = rlo;
   fRhi = rhi;
   fDz  = dz;
   Double_t dd = 1./(fRhi*fRhi - fRlo*fRlo);
   fA = 2.*fDz*dd;
   fB = - fDz * (fRlo*fRlo + fRhi*fRhi)*dd;
}   

//_____________________________________________________________________________
void TGeoParaboloid::SetDimensions(Double_t *param)
{
   Double_t rlo    = param[0];
   Double_t rhi    = param[1];
   Double_t dz     = param[2];
   SetParaboloidDimensions(rlo, rhi, dz);
}   

//_____________________________________________________________________________
void TGeoParaboloid::SetPoints(Double_t *points) const
{
// Create paraboloid mesh points.
// Npoints = n*(n+1) + 2
//   ifirst = 0
//   ipoint(i,j) = 1+i*n+j;                              i=[0,n]  j=[0,n-1]
//   ilast = 1+n*(n+1)
// Nsegments = n*(2*n+3)  
//   lower: (0, j+1);                                    j=[0,n-1]
//   circle(i): (n*i+1+j, n*i+1+(j+1)%n);                i=[0,n]  j=[0,n-1]
//   generator(i): (n*i+1+j, n*(i+1)+1+j);               i,j=[0,n-1]
//   upper: (n*n+1+j, (n+1)*n+1)                           j=[0,n-1]
// Npolygons = n*(n+2)
//   lower: (n+j, (j+1)%n, j)                              j=[0,n-1]
//   lateral(i): ((2*i+1)*n+j, 2*(i+1)*n+j, (2*i+3)*n+j, 2*(i+1)*n+(j+1)%n)
//                                                      i,j = [0,n-1]
//   upper: ((2n+1)*n+j, 2*n*(n+1)+(j+1)%n, 2*n*(n+1)+j)   j=[0,n-1]
   if (!points) return;
   Double_t ttmin, ttmax;
   ttmin = TMath::ATan2(-fDz, fRlo);
   ttmax = TMath::ATan2(fDz, fRhi);
   Int_t n = gGeoManager->GetNsegments();
   Double_t dtt = (ttmax-ttmin)/n;
   Double_t dphi = 360./n;
   Double_t tt;
   Double_t r, z, delta;
   Double_t phi, sph, cph;
   Int_t indx = 0;
   // center of the lower endcap:
   points[indx++] = 0; // x
   points[indx++] = 0; // y
   points[indx++] = -fDz;
   for (Int_t i=0; i<n+1; i++) {  // nz planes = n+1
      if (i==0) {
         r = fRlo;
         z = -fDz;
      } else if (i==n) {
         r = fRhi;
         z = fDz;
      } else {      
         tt = TMath::Tan(ttmin + i*dtt);
         delta = tt*tt - 4*fA*fB; // should be always positive (a*b<0)
         r = 0.5*(tt+TMath::Sqrt(delta))/fA;
         z = r*tt;
      }
      for (Int_t j=0; j<n; j++) {
         phi = j*dphi*TMath::DegToRad();
         sph=TMath::Sin(phi);
         cph=TMath::Cos(phi);
         points[indx++] = r*cph;
         points[indx++] = r*sph;
         points[indx++] = z;
      }
   } 
   // center of the upper endcap
   points[indx++] = 0; // x
   points[indx++] = 0; // y
   points[indx++] = fDz;
}

//_____________________________________________________________________________
Int_t TGeoParaboloid::GetNmeshVertices() const
{
   Int_t n = gGeoManager->GetNsegments();
   return (n*(n+1)+2);
}   
   
//_____________________________________________________________________________
void TGeoParaboloid::SavePrimitive(ofstream &out, Option_t * /*option*/)
{
// Save a primitive as a C++ statement(s) on output stream "out".
   if (TObject::TestBit(kGeoSavePrimitive)) return;
   out << "   // Shape: " << GetName() << " type: " << ClassName() << endl;
   out << "   rlo = " << fRlo << ";" << endl;
   out << "   rhi = " << fRhi << ";" << endl;
   out << "   dz  = " << fDZ << ";" << endl;  
   out << "   pShape = new TGeoParaboloid(\"" << GetName() << "\", rlo,rhi,dz);" << endl;
   TObject::SetBit(TGeoShape::kGeoSavePrimitive);
}         

//_____________________________________________________________________________
void TGeoParaboloid::SetPoints(Float_t *points) const
{
   if (!points) return;
   Double_t ttmin, ttmax;
   ttmin = TMath::ATan2(-fDz, fRlo);
   ttmax = TMath::ATan2(fDz, fRhi);
   Int_t n = gGeoManager->GetNsegments();
   Double_t dtt = (ttmax-ttmin)/n;
   Double_t dphi = 360./n;
   Double_t tt;
   Double_t r, z, delta;
   Double_t phi, sph, cph;
   Int_t indx = 0;
   // center of the lower endcap:
   points[indx++] = 0; // x
   points[indx++] = 0; // y
   points[indx++] = -fDz;
   for (Int_t i=0; i<n+1; i++) {  // nz planes = n+1
      if (i==0) {
         r = fRlo;
         z = -fDz;
      } else if (i==n) {
         r = fRhi;
         z = fDz;
      } else {      
         tt = TMath::Tan(ttmin + i*dtt);
         delta = tt*tt - 4*fA*fB; // should be always positive (a*b<0)
         r = 0.5*(tt+TMath::Sqrt(delta))/fA;
         z = r*tt;
      }
      for (Int_t j=0; j<n; j++) {
         phi = j*dphi*TMath::DegToRad();
         sph=TMath::Sin(phi);
         cph=TMath::Cos(phi);
         points[indx++] = r*cph;
         points[indx++] = r*sph;
         points[indx++] = z;
      }
   } 
   // center of the upper endcap
   points[indx++] = 0; // x
   points[indx++] = 0; // y
   points[indx++] = fDz;
}

//_____________________________________________________________________________
void TGeoParaboloid::Sizeof3D() const
{
///   Int_t n = gGeoManager->GetNsegments();
///   TVirtualGeoPainter *painter = gGeoManager->GetGeomPainter();
///   if (painter) painter->AddSize3D(n*(n+1)+2, n*(2*n+3), n*(n+2));
}

//_____________________________________________________________________________
const TBuffer3D & TGeoParaboloid::GetBuffer3D(Int_t reqSections, Bool_t localFrame) const
{
   static TBuffer3D buffer(TBuffer3DTypes::kGeneric);
   TGeoBBox::FillBuffer3D(buffer, reqSections, localFrame);

   if (reqSections & TBuffer3D::kRawSizes) {
      Int_t n = gGeoManager->GetNsegments();
      Int_t NbPnts = n*(n+1)+2;
      Int_t NbSegs = n*(2*n+3);
      Int_t NbPols = n*(n+2);
      if (buffer.SetRawSizes(NbPnts, 3*NbPnts, NbSegs, 3*NbSegs, NbPols, 2*n*5 + n*n*6)) {
         buffer.SetSectionsValid(TBuffer3D::kRawSizes);
      }
   }
   if ((reqSections & TBuffer3D::kRaw) && buffer.SectionsValid(TBuffer3D::kRawSizes)) {
      SetPoints(buffer.fPnts);
      if (!buffer.fLocalFrame) {
         TransformPoints(buffer.fPnts, buffer.NbPnts());
      }
      SetSegsAndPols(buffer);  
      buffer.SetSectionsValid(TBuffer3D::kRaw);
   }
      
   return buffer;
}
