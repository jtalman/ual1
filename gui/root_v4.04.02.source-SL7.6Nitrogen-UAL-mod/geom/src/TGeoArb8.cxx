// @(#)root/geom:$Name:  $:$Id: TGeoArb8.cxx,v 1.42 2005/03/09 18:19:26 brun Exp $
// Author: Andrei Gheata   31/01/02

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include "Riostream.h"
#include "TROOT.h"

#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoArb8.h"

ClassImp(TGeoArb8)
    
//________________________________________________________________________
// TGeoArb8 - a arbitrary trapezoid with less than 8 vertices standing on
//   two paralel planes perpendicular to Z axis. Parameters :
//            - dz - half length in Z;
//            - xy[8][2] - vector of (x,y) coordinates of vertices
//               - first four points (xy[i][j], i<4, j<2) are the (x,y)
//                 coordinates of the vertices sitting on the -dz plane;
//               - last four points (xy[i][j], i>=4, j<2) are the (x,y)
//                 coordinates of the vertices sitting on the +dz plane;
//   The order of defining the vertices of an arb8 is the following :
//      - point 0 is connected with points 1,3,4
//      - point 1 is connected with points 0,2,5
//      - point 2 is connected with points 1,3,6
//      - point 3 is connected with points 0,2,7
//      - point 4 is connected with points 0,5,7
//      - point 5 is connected with points 1,4,6
//      - point 6 is connected with points 2,5,7
//      - point 7 is connected with points 3,4,6
//   Points can be identical in order to create shapes with less than 
//   8 vertices.
//

//Begin_Html
/*
<img src="gif/t_arb8.gif">
*/
//End_Html

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// TGeoTrap                                                               //
//                                                                        //
// TRAP is a general trapezoid, i.e. one for which the faces perpendicular//
// to z are trapezia and their centres are not the same x, y. It has 11   //
// parameters: the half length in z, the polar angles from the centre of  //
// the face at low z to that at high z, H1 the half length in y at low z, //
// LB1 the half length in x at low z and y low edge, LB2 the half length  //
// in x at low z and y high edge, TH1 the angle w.r.t. the y axis from the//
// centre of low y edge to the centre of the high y edge, and H2, LB2,    //
// LH2, TH2, the corresponding quantities at high z.                      //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
//Begin_Html
/*
<img src="gif/t_trap.gif">
*/
//End_Html
//
//Begin_Html
/*
<img src="gif/t_trapdivZ.gif">
*/
//End_Html

////////////////////////////////////////////////////////////////////////////
//                                                                        //
// TGeoGtra                                                               //
//                                                                        //
// Gtra is a twisted trapezoid, i.e. one for which the faces perpendicular//
// to z are trapezia and their centres are not the same x, y. It has 12   //
// parameters: the half length in z, the polar angles from the centre of  //
// the face at low z to that at high z, twist, H1 the half length in y at low z, //
// LB1 the half length in x at low z and y low edge, LB2 the half length  //
// in x at low z and y high edge, TH1 the angle w.r.t. the y axis from the//
// centre of low y edge to the centre of the high y edge, and H2, LB2,    //
// LH2, TH2, the corresponding quantities at high z.                      //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
//Begin_Html
/*
<img src="gif/t_gtra.gif">
*/
//End_Html
//
//Begin_Html
/*
<img src="gif/t_gtradivstepZ.gif">
*/
//End_Html


//_____________________________________________________________________________
TGeoArb8::TGeoArb8()
{
   // dummy ctor
   fDz = 0;
   fTwist = 0;
   for (Int_t i=0; i<8; i++) {
      fXY[i][0] = 0.0;
      fXY[i][1] = 0.0;
   }  
   SetShapeBit(kGeoArb8); 
}

//_____________________________________________________________________________
TGeoArb8::TGeoArb8(Double_t dz, Double_t *vertices)
         :TGeoBBox(0,0,0)
{
// constructor. If the array of vertices is not null, this should be
// in the format : (x0, y0, x1, y1, ... , x7, y7) 
   fDz = dz;
   fTwist = 0;
   SetShapeBit(kGeoArb8); 
   if (vertices) {
      for (Int_t i=0; i<8; i++) {
         fXY[i][0] = vertices[2*i];
         fXY[i][1] = vertices[2*i+1];
      }
      ComputeTwist();
      ComputeBBox();
   } else {
      for (Int_t i=0; i<8; i++) {
         fXY[i][0] = 0.0;
         fXY[i][1] = 0.0;
      }   
   }
}

//_____________________________________________________________________________
TGeoArb8::TGeoArb8(const char *name, Double_t dz, Double_t *vertices)
         :TGeoBBox(name, 0,0,0)
{
// constructor. If the array of vertices is not null, this should be
// in the format : (x0, y0, x1, y1, ... , x7, y7) 
   fDz = dz;
   fTwist = 0;
   SetShapeBit(kGeoArb8); 
   if (vertices) {
      for (Int_t i=0; i<8; i++) {
         fXY[i][0] = vertices[2*i];
         fXY[i][1] = vertices[2*i+1];
      }
      ComputeTwist();
      ComputeBBox();
   } else {
      for (Int_t i=0; i<8; i++) {
         fXY[i][0] = 0.0;
         fXY[i][1] = 0.0;
      }   
   }
}

//_____________________________________________________________________________
TGeoArb8::~TGeoArb8()
{
// destructor
   if (fTwist) delete [] fTwist;
}

//_____________________________________________________________________________
void TGeoArb8::ComputeBBox()
{
// compute bounding box for a Arb8
   Double_t xmin, xmax, ymin, ymax;
   xmin = xmax = fXY[0][0];
   ymin = ymax = fXY[0][1];
   
   for (Int_t i=1; i<8; i++) {
      if (xmin>fXY[i][0]) xmin=fXY[i][0];
      if (xmax<fXY[i][0]) xmax=fXY[i][0];
      if (ymin>fXY[i][1]) ymin=fXY[i][1];
      if (ymax<fXY[i][1]) ymax=fXY[i][1];
   }
   fDX = 0.5*(xmax-xmin);
   fDY = 0.5*(ymax-ymin);
   fDZ = fDz;
   fOrigin[0] = 0.5*(xmax+xmin);
   fOrigin[1] = 0.5*(ymax+ymin);
   fOrigin[2] = 0;
   SetShapeBit(kGeoClosedShape);
}   

//_____________________________________________________________________________
void TGeoArb8::ComputeTwist()
{
// compute tangents of twist angles (angles between projections on XY plane
// of corresponding -dz +dz edges). Called after last point [7] was set.
   Double_t twist[4];
   Bool_t twisted = kFALSE;
   Double_t dx1, dy1, dx2, dy2;
   for (Int_t i=0; i<4; i++) {
      dx1 = fXY[(i+1)%4][0]-fXY[i][0];
      dy1 = fXY[(i+1)%4][1]-fXY[i][1];
      if (dx1==0 && dy1==0) {
         twist[i] = 0;
         continue;
      }   
      dx2 = fXY[4+(i+1)%4][0]-fXY[4+i][0];
      dy2 = fXY[4+(i+1)%4][1]-fXY[4+i][1];
      if (dx2==0 && dy2==0) {
         twist[i] = 0;
         continue;
      }
      twist[i] = dy1*dx2 - dx1*dy2;
      if (TMath::Abs(twist[i])<1E-3) {
         twist[i] = 0;
         continue;
      }
      twist[i] = TMath::Sign(1.,twist[i]);
      twisted = kTRUE;
   }
   if (!twisted) return;
   if (fTwist) delete [] fTwist;
   fTwist = new Double_t[4];
   memcpy(fTwist, &twist[0], 4*sizeof(Double_t));
}

//_____________________________________________________________________________
Double_t TGeoArb8::GetTwist(Int_t iseg) const
{
// Get twist for segment I in range [0,3]
   if (!fTwist) return 0.;
   if (iseg<0 || iseg>3) return 0.;
   return fTwist[iseg];
}   

//_____________________________________________________________________________
void TGeoArb8::ComputeNormal(Double_t *point, Double_t *dir, Double_t *norm)
{
// Compute normal to closest surface from POINT. 
   Double_t safe = TGeoShape::Big();
   Double_t safc;
   Int_t i;          // current facette index
   Double_t x0, y0, z0, x1, y1, z1, x2, y2;
   Double_t ax, ay, az, bx, by;
   Double_t fn;
   safc = fDz-TMath::Abs(point[2]);
   if (safc<1E-4) {
      memset(norm,0,3*sizeof(Double_t));
      norm[2] = (dir[2]>0)?1:(-1);
      return;
   }   
   Double_t vert[8], lnorm[3];
   SetPlaneVertices(point[2], vert);
   //---> compute safety for lateral planes
   for (i=0; i<4; i++) {
      x0 = vert[2*i];
      y0 = vert[2*i+1];
      z0 = point[2];
      x1 = fXY[i+4][0];
      y1 = fXY[i+4][1];
      z1 = fDz;
      ax = x1-x0;
      ay = y1-y0;
      az = z1-z0;
      x2 = vert[2*((i+1)%4)];
      y2 = vert[2*((i+1)%4)+1];
      bx = x2-x0;
      by = y2-y0;

      lnorm[0] = -az*by;
      lnorm[1] = az*bx;
      lnorm[2] = ax*by-ay*bx;
      fn = TMath::Sqrt(lnorm[0]*lnorm[0]+lnorm[1]*lnorm[1]+lnorm[2]*lnorm[2]);
      if (fn<1E-10) continue;
      lnorm[0] /= fn;
      lnorm[1] /= fn;
      lnorm[2] /= fn;
      safc = (x0-point[0])*lnorm[0]+(y0-point[1])*lnorm[1]+(z0-point[2])*lnorm[2];
      safc = TMath::Abs(safc);
//      printf("plane %i : (%g, %g, %g) safe=%g\n", i, lnorm[0],lnorm[1],lnorm[2],safc);
      if (safc<safe) {
         safe = safc;
         memcpy(norm,lnorm,3*sizeof(Double_t));
      }      
   }
   if (dir[0]*norm[0]+dir[1]*norm[1]+dir[2]*norm[2] < 0) {
      norm[0] = -norm[0];
      norm[1] = -norm[1];
      norm[2] = -norm[2];
   }
}

//_____________________________________________________________________________
Bool_t TGeoArb8::Contains(Double_t *point) const
{
// test if point is inside this sphere
   // first check Z range
   if (TMath::Abs(point[2]) > fDz) return kFALSE;
   // compute intersection between Z plane containing point and the arb8
   Double_t poly[8];
//   memset(&poly[0], 0, 8*sizeof(Double_t));
   //SetPlaneVertices(point[2], &poly[0]);
   Double_t cf = 0.5*(fDz-point[2])/fDz;
   Int_t i;
   for (i=0; i<4; i++) {
      poly[2*i]   = fXY[i+4][0]+cf*(fXY[i][0]-fXY[i+4][0]);
      poly[2*i+1] = fXY[i+4][1]+cf*(fXY[i][1]-fXY[i+4][1]);
   }
   return InsidePolygon(point[0],point[1],poly);
}

//_____________________________________________________________________________
Double_t TGeoArb8::DistToPlane(Double_t *point, Double_t *dir, Int_t ipl, Bool_t in) const 
{
// compute distance to plane ipl :
// ipl=0 : points 0,4,1,5
// ipl=1 : points 1,5,2,6
// ipl=2 : points 2,6,3,7
// ipl=3 : points 3,7,0,4
   Double_t xa,xb,xc,xd;
   Double_t ya,yb,yc,yd;
   Int_t j = (ipl+1)%4;
   xa=fXY[ipl][0];
   ya=fXY[ipl][1];
   xb=fXY[ipl+4][0];
   yb=fXY[ipl+4][1];
   xc=fXY[j][0];
   yc=fXY[j][1];
   xd=fXY[4+j][0];
   yd=fXY[4+j][1];
   Double_t dz2 =0.5/fDz;
   Double_t tx1 =dz2*(xb-xa);
   Double_t ty1 =dz2*(yb-ya);
   Double_t tx2 =dz2*(xd-xc);
   Double_t ty2 =dz2*(yd-yc);
   Double_t dzp =fDz+point[2];
   Double_t xs1 =xa+tx1*dzp;
   Double_t ys1 =ya+ty1*dzp;
   Double_t xs2 =xc+tx2*dzp;
   Double_t ys2 =yc+ty2*dzp;
   Double_t dxs =xs2-xs1;
   Double_t dys =ys2-ys1;
   Double_t dtx =tx2-tx1;
   Double_t dty =ty2-ty1;
   Double_t a=(dtx*dir[1]-dty*dir[0]+(tx1*ty2-tx2*ty1)*dir[2])*dir[2];
   Double_t b=dxs*dir[1]-dys*dir[0]+(dtx*point[1]-dty*point[0]+ty2*xs1-ty1*xs2
              +tx1*ys2-tx2*ys1)*dir[2];
   Double_t c=dxs*point[1]-dys*point[0]+xs1*ys2-xs2*ys1;
   Double_t s=TGeoShape::Big();
   Double_t x1,x2,y1,y2,xp,yp,zi;
   if (TMath::Abs(a)<1E-10) {           
      if (b==0) return TGeoShape::Big();
      s=-c/b;
      if (s>0) {
         if (in) return s;
         zi=point[2]+s*dir[2];
         if (TMath::Abs(zi)<fDz) {
            x1=xs1+tx1*dir[2]*s;
            x2=xs2+tx2*dir[2]*s;
            xp=point[0]+s*dir[0];
            y1=ys1+ty1*dir[2]*s;
            y2=ys2+ty2*dir[2]*s;
            yp=point[1]+s*dir[1];
            zi = (xp-x1)*(xp-x2)+(yp-y1)*(yp-y2);
            if (zi<=0) return s;
         }      
      }
      return TGeoShape::Big();
   }      
   b=0.5*b/a;
   c=c/a;
   Double_t d=b*b-c;
   if (d>=0) {
      Double_t sqd = TMath::Sqrt(d);
      s=-b-sqd;
      if (s>0) {
         if (in) return s;
         zi=point[2]+s*dir[2];
         if (TMath::Abs(zi)<fDz) {
            x1=xs1+tx1*dir[2]*s;
            x2=xs2+tx2*dir[2]*s;
            xp=point[0]+s*dir[0];
            y1=ys1+ty1*dir[2]*s;
            y2=ys2+ty2*dir[2]*s;
            yp=point[1]+s*dir[1];
            zi = (xp-x1)*(xp-x2)+(yp-y1)*(yp-y2);
            if (zi<=0) return s;
         }      
      }
      s=-b+sqd;
      if (s>0) {
         if (in) return s;
         zi=point[2]+s*dir[2];
         if (TMath::Abs(zi)<fDz) {
            x1=xs1+tx1*dir[2]*s;
            x2=xs2+tx2*dir[2]*s;
            xp=point[0]+s*dir[0];
            y1=ys1+ty1*dir[2]*s;
            y2=ys2+ty2*dir[2]*s;
            yp=point[1]+s*dir[1];
            zi = (xp-x1)*(xp-x2)+(yp-y1)*(yp-y2);
            if (zi<=0) return s;
         }      
      }
   }
   return TGeoShape::Big();
}      
      
//_____________________________________________________________________________
Double_t TGeoArb8::DistFromOutside(Double_t *point, Double_t *dir, Int_t /*iact*/, Double_t /*step*/, Double_t * /*safe*/) const
{
// compute distance from outside point to surface of the arb8
   Double_t snxt=TGeoShape::Big();
   if (!TGeoBBox::Contains(point)) {
      snxt=TGeoBBox::DistFromOutside(point,dir,3);
      if (snxt>1E20) return snxt;
   }   
   Double_t dist[5];
   // check lateral faces
   Int_t i;
   for (i=0; i<4; i++) {
      dist[i]=DistToPlane(point, dir, i, kFALSE);  
   }   
   // check Z planes
   dist[4]=TGeoShape::Big();
   if (TMath::Abs(point[2])>fDz) {
      if (dir[2]!=0) {
         Double_t pt[3];
         if (point[2]>0) {
            dist[4] = (fDz-point[2])/dir[2];
            pt[2]=fDz;
         } else {   
            dist[4] = (-fDz-point[2])/dir[2];
            pt[2]=-fDz;
         }   
         if (dist[4]<0) {
            dist[4]=TGeoShape::Big();
         } else {   
            for (Int_t j=0; j<2; j++) pt[j]=point[j]+dist[4]*dir[j];
            if (!Contains(&pt[0])) dist[4]=TGeoShape::Big();
         }   
      }
   }   
   Double_t distmin = dist[0];
   for (i=1;i<5;i++) if (dist[i] < distmin) distmin = dist[i];
   return distmin;
}   

//_____________________________________________________________________________
Double_t TGeoArb8::DistFromInside(Double_t *point, Double_t *dir, Int_t /*iact*/, Double_t /*step*/, Double_t * /*safe*/) const
{
// compute distance from inside point to surface of the arb8
#ifdef OLDALGORITHM
   Int_t i;
   Double_t dist[6];
   dist[0]=dist[1]=TGeoShape::Big();
   if (dir[2]<0) {
      dist[0]=(-fDz-point[2])/dir[2];
   } else {
      if (dir[2]>0) dist[1]=(fDz-point[2])/dir[2];
   }      
   for (i=0; i<4; i++) {
      dist[i+2]=DistToPlane(point, dir, i, kTRUE);   
   }   
      
   Double_t distmin = dist[0];
   for (i=1;i<6;i++) if (dist[i] < distmin) distmin = dist[i];
   return distmin;
#else
// compute distance to plane ipl :
// ipl=0 : points 0,4,1,5
// ipl=1 : points 1,5,2,6
// ipl=2 : points 2,6,3,7
// ipl=3 : points 3,7,0,4
   Double_t distmin;
   if (dir[2]<0) {
      distmin=(-fDz-point[2])/dir[2];
   } else {
      if (dir[2]>0) distmin =(fDz-point[2])/dir[2];
      else          distmin = TGeoShape::Big();
   }      
   Double_t dz2 =0.5/fDz;
   Double_t xa,xb,xc,xd;
   Double_t ya,yb,yc,yd;
   for (Int_t ipl=0;ipl<4;ipl++) {
      Int_t j = (ipl+1)%4;
      xa=fXY[ipl][0];
      ya=fXY[ipl][1];
      xb=fXY[ipl+4][0];
      yb=fXY[ipl+4][1];
      xc=fXY[j][0];
      yc=fXY[j][1];
      xd=fXY[4+j][0];
      yd=fXY[4+j][1];
      
      Double_t tx1 =dz2*(xb-xa);
      Double_t ty1 =dz2*(yb-ya);
      Double_t tx2 =dz2*(xd-xc);
      Double_t ty2 =dz2*(yd-yc);
      Double_t dzp =fDz+point[2];
      Double_t xs1 =xa+tx1*dzp;
      Double_t ys1 =ya+ty1*dzp;
      Double_t xs2 =xc+tx2*dzp;
      Double_t ys2 =yc+ty2*dzp;
      Double_t dxs =xs2-xs1;
      Double_t dys =ys2-ys1;
      Double_t dtx =tx2-tx1;
      Double_t dty =ty2-ty1;
      Double_t a=(dtx*dir[1]-dty*dir[0]+(tx1*ty2-tx2*ty1)*dir[2])*dir[2];
      Double_t b=dxs*dir[1]-dys*dir[0]+(dtx*point[1]-dty*point[0]+ty2*xs1-ty1*xs2
              +tx1*ys2-tx2*ys1)*dir[2];
      Double_t c=dxs*point[1]-dys*point[0]+xs1*ys2-xs2*ys1;
      Double_t s=TGeoShape::Big();
      if (TMath::Abs(a)<1E-10) {           
         if (b==0) continue;
         s=-c/b;
         if (s>0 && s < distmin) distmin =s;
         continue;
      }      
      b=0.5*b/a;
      c=c/a;
      Double_t d=b*b-c;
      if (d>=0) {
         Double_t sqd = TMath::Sqrt(d);
         s=-b-sqd;
         if (s>0) {
            if (s < distmin) distmin = s;
         } else {
            s=-b+sqd;
            if (s>0 && s < distmin) distmin =s;
         }
      }
   }
   return distmin;
#endif
}   

//_____________________________________________________________________________
TGeoVolume *TGeoArb8::Divide(TGeoVolume *voldiv, const char * /*divname*/, Int_t /*iaxis*/, Int_t /*ndiv*/, 
                             Double_t /*start*/, Double_t /*step*/) 
{
   Error("Divide", "Division of an arbitrary trapezoid not implemented");
   return voldiv;
}      

//_____________________________________________________________________________
Double_t TGeoArb8::GetAxisRange(Int_t iaxis, Double_t &xlo, Double_t &xhi) const
{
   xlo = 0;
   xhi = 0;
   Double_t dx = 0;
   if (iaxis==3) {
      xlo = -fDz;
      xhi = fDz;
      dx = xhi-xlo;
      return dx;
   }
   return dx;
}
      
//_____________________________________________________________________________
void TGeoArb8::GetBoundingCylinder(Double_t *param) const
{
//--- Fill vector param[4] with the bounding cylinder parameters. The order
// is the following : Rmin, Rmax, Phi1, Phi2
   //--- first compute rmin/rmax
   Double_t rmaxsq = 0;
   Double_t rsq;
   Int_t i;
   for (i=0; i<8; i++) {
      rsq = fXY[i][0]*fXY[i][0] + fXY[i][1]*fXY[i][1];
      rmaxsq = TMath::Max(rsq, rmaxsq);
   }
   param[0] = 0.;                  // Rmin
   param[1] = rmaxsq;              // Rmax
   param[2] = 0.;                  // Phi1
   param[3] = 360.;                // Phi2 
}   

//_____________________________________________________________________________
Int_t TGeoArb8::GetFittingBox(const TGeoBBox *parambox, TGeoMatrix *mat, Double_t &dx, Double_t &dy, Double_t &dz) const
{
// Fills real parameters of a positioned box inside this arb8. Returns 0 if successfull.
   dx=dy=dz=0;
   if (mat->IsRotation()) {
      Error("GetFittingBox", "cannot handle parametrized rotated volumes");
      return 1; // ### rotation not accepted ###
   }
   //--> translate the origin of the parametrized box to the frame of this box.
   Double_t origin[3];
   mat->LocalToMaster(parambox->GetOrigin(), origin);
   if (!Contains(origin)) {
      Error("GetFittingBox", "wrong matrix - parametrized box is outside this");
      return 1; // ### wrong matrix ###
   }
   //--> now we have to get the valid range for all parametrized axis
   Double_t dd[3];
   dd[0] = parambox->GetDX();
   dd[1] = parambox->GetDY();
   dd[2] = parambox->GetDZ();
   //-> check if Z range is fixed
   if (dd[2]<0) {
      dd[2] = TMath::Min(origin[2]+fDz, fDz-origin[2]); 
      if (dd[2]<0) {
         Error("GetFittingBox", "wrong matrix");
         return 1;
      }
   }
   if (dd[0]>=0 && dd[1]>=0) {
      dx = dd[0];
      dy = dd[1];
      dz = dd[2];
      return 0;
   }
   //-> check now vertices at Z = origin[2] +/- dd[2]
   Double_t upper[8];
   Double_t lower[8];
   SetPlaneVertices(origin[2]-dd[2], lower);
   SetPlaneVertices(origin[2]+dd[2], upper);
   Double_t ddmin=TGeoShape::Big();
   for (Int_t iaxis=0; iaxis<2; iaxis++) {
      if (dd[iaxis]>=0) continue;
      ddmin=TGeoShape::Big();
      for (Int_t ivert=0; ivert<4; ivert++) {
         ddmin = TMath::Min(ddmin, TMath::Abs(origin[iaxis]-lower[2*ivert+iaxis]));
         ddmin = TMath::Min(ddmin, TMath::Abs(origin[iaxis]-upper[2*ivert+iaxis]));
      }
      dd[iaxis] = ddmin;
   }
   dx = dd[0];
   dy = dd[1];
   dz = dd[2];
   return 0;
}   

//_____________________________________________________________________________
void TGeoArb8::GetPlaneNormal(Double_t *p1, Double_t *p2, Double_t *p3, Double_t *norm)
{
// Compute normal to plane defined by P1, P2 and P3
   Double_t cross = 0.;
   Double_t v1[3], v2[3];
   Int_t i;
   for (i=0; i<3; i++) {
      v1[i] = p2[i] - p1[i];
      v2[i] = p3[i] - p1[i];
   }
   norm[0] = v1[1]*v2[2]-v1[2]*v2[1];
   cross += norm[0]*norm[0];
   norm[1] = v1[2]*v2[0]-v1[0]*v2[2];
   cross += norm[1]*norm[1];
   norm[2] = v1[0]*v2[1]-v1[1]*v2[0];
   cross += norm[2]*norm[2];
   if (cross == 0.) return;
   cross = 1./TMath::Sqrt(cross);
   for (i=0; i<3; i++) norm[i] *= cross;
}   

//_____________________________________________________________________________
Bool_t TGeoArb8::InsidePolygon(Double_t x, Double_t y, Double_t *pts)
{
// Find if a point in XY plane is inside the polygon defines by PTS.
   Int_t i,j;
   Double_t x1,y1,x2,y2;
   Double_t cross;
   for (i=0; i<4; i++) {
      j = (i+1)%4;
      x1 = pts[i<<1];
      y1 = pts[(i<<1)+1];
      x2 = pts[j<<1];
      y2 = pts[(j<<1)+1];
      cross = (x-x1)*(y2-y1)-(y-y1)*(x2-x1);
      if (cross<0) return kFALSE;
   }
   return kTRUE;   
}

//_____________________________________________________________________________
void TGeoArb8::InspectShape() const
{
// print shape parameters
   printf("*** Shape %s: TGeoArb8 ***\n", GetName());
   if (IsTwisted()) printf("  = TWISTED\n");
   for (Int_t ip=0; ip<8; ip++) {
      printf("    point #%i : x=%11.5f y=%11.5f z=%11.5f\n", 
             ip, fXY[ip][0], fXY[ip][1], fDz*((ip<4)?-1:1));
   }
   printf(" Bounding box:\n");
   TGeoBBox::InspectShape();
}

//_____________________________________________________________________________
Double_t TGeoArb8::Safety(Double_t *point, Bool_t in) const
{
// computes the closest distance from given point to this shape, according
// to option. The matching point on the shape is stored in spoint.
   Double_t safz = fDz-TMath::Abs(point[2]);
   if (!in) safz = -safz;
   Int_t iseg;
   Double_t safmin = TGeoShape::Big();
   Double_t safe = TGeoShape::Big();
   Double_t lsq, ssq, dx, dy, dpx, dpy, u;
   if (IsTwisted()) {
      if (!in) {
         if (!TGeoBBox::Contains(point)) return TGeoBBox::Safety(point,kFALSE);
      }
      // Point is also in the bounding box ;-(  
      // Compute closest distance to any segment
      Double_t vert[8];
      Double_t *p1, *p2;
      Int_t isegmin=0;
      Double_t umin = 0.;
      SetPlaneVertices (point[2], vert);
      for (iseg=0; iseg<4; iseg++) {
         if (safe==0.) return 0.;
         p1 = &vert[2*iseg];
         p2 = &vert[2*((iseg+1)%4)];
         dx = p2[0] - p1[0];
         dy = p2[1] - p1[1];
         dpx = point[0] - p1[0];
         dpy = point[1] - p1[1];
      
         lsq = dx*dx + dy*dy;
         u = (dpx*dx + dpy*dy)/lsq;
         if (u>1) {
            dpx = point[0]-p2[0];
            dpy = point[1]-p2[1];
         } else {
            if (u>=0) {
               dpx -= u*dx;
               dpy -= u*dy;
            }
         }
         ssq = dpx*dpx + dpy*dpy;      
         if (ssq < safe) {
            isegmin = iseg;
            umin = u;
            safe = ssq;
         }   
      }
      if (umin<0) umin = 0.;
      if (umin>1) {
         isegmin = (isegmin+1)%4;
         umin = 0.;
      }
      Int_t i1 = isegmin;
      Int_t i2 = (isegmin+1)%4;
      Double_t dx1 = fXY[i2][0]-fXY[i1][0];   
      Double_t dx2 = fXY[i2+4][0]-fXY[i1+4][0];   
      Double_t dy1 = fXY[i2][1]-fXY[i1][1];   
      Double_t dy2 = fXY[i2+4][1]-fXY[i1+4][1];
      dx = dx1 + umin*(dx2-dx1);
      dy = dy1 + umin*(dy2-dy1);
      safe *= 1.- 4.*fDz*fDz/(dx*dx+dy*dy+4.*fDz*fDz);
      safe = TMath::Sqrt(safe);      
      return safe;   
   }  
      
      
   for (iseg=0; iseg<4; iseg++) {
      safe = SafetyToFace(point,iseg,in);
      if (safe>0) {
         if (in && safe<safmin) {
            safmin = safe;
            continue;
         }
         if (!in && safe<1E10) {
            if (safmin<1E10) safe = TMath::Max(safe,safmin);
            else safmin=safe;
         }
      }           
   }
   if (in) return TMath::Min(safmin, safz);
   return TMath::Max(safmin, safz);   
}

//_____________________________________________________________________________
Double_t TGeoArb8::SafetyToFace(Double_t *point, Int_t iseg, Bool_t in) const
{
// Estimate safety to lateral plane defined by segment iseg in range [0,3]
// might be negative: plane seen only from inside
   Double_t vertices[12];
   Int_t ipln = (iseg+1)%4;
   // point 1
   vertices[0] = fXY[iseg][0];
   vertices[1] = fXY[iseg][1];
   vertices[2] = -fDz;
   // point 2
   vertices[3] = fXY[ipln][0];
   vertices[4] = fXY[ipln][1];
   vertices[5] = -fDz;
   // point 3
   vertices[6] = fXY[ipln+4][0];
   vertices[7] = fXY[ipln+4][1];
   vertices[8] = fDz;
   // point 4
   vertices[9] = fXY[iseg+4][0];
   vertices[10] = fXY[iseg+4][1];
   vertices[11] = fDz;
   Double_t twist = GetTwist(iseg);
   Double_t safe;
   Double_t norm[3];
   Double_t *p1, *p2, *p3;
   if (twist ==0) {
      p1 = &vertices[0];
      p2 = &vertices[9];
      p3 = &vertices[6];
      if (IsSamePoint(p2,p3)) {
         p3 = &vertices[3];
         if (IsSamePoint(p1,p3)) return TGeoShape::Big(); // skip single segment
      }
      GetPlaneNormal(p1,p2,p3,norm);
      safe = (point[0]-p1[0])*norm[0]+(point[1]-p1[1])*norm[1]+(point[2]-p1[2])*norm[2];
      if (in) return (-safe);
      return safe;
   }
   // The face is twisted
   return TGeoShape::Big();
}
   
//_____________________________________________________________________________
void TGeoArb8::SavePrimitive(ofstream &out, Option_t * /*option*/)
{
// Save a primitive as a C++ statement(s) on output stream "out".
   if (TObject::TestBit(kGeoSavePrimitive)) return;
   out << "   // Shape: " << GetName() << " type: " << ClassName() << endl;
   out << "   dz       = " << fDz << ";" << endl;
   out << "   vert[0]  = " << fXY[0][0] << ";" << endl;
   out << "   vert[1]  = " << fXY[0][1] << ";" << endl;
   out << "   vert[2]  = " << fXY[1][0] << ";" << endl;
   out << "   vert[3]  = " << fXY[1][1] << ";" << endl;
   out << "   vert[4]  = " << fXY[2][0] << ";" << endl;
   out << "   vert[5]  = " << fXY[2][1] << ";" << endl;
   out << "   vert[6]  = " << fXY[3][0] << ";" << endl;
   out << "   vert[7]  = " << fXY[3][1] << ";" << endl;
   out << "   vert[8]  = " << fXY[4][0] << ";" << endl;
   out << "   vert[9]  = " << fXY[4][1] << ";" << endl;
   out << "   vert[10] = " << fXY[5][0] << ";" << endl;
   out << "   vert[11] = " << fXY[5][1] << ";" << endl;
   out << "   vert[12] = " << fXY[6][0] << ";" << endl;
   out << "   vert[13] = " << fXY[6][1] << ";" << endl;
   out << "   vert[14] = " << fXY[7][0] << ";" << endl;
   out << "   vert[15] = " << fXY[7][1] << ";" << endl;
   out << "   pShape = new TGeoArb8(\"" << GetName() << "\", dz,vert);" << endl;
   TObject::SetBit(TGeoShape::kGeoSavePrimitive);
}

//_____________________________________________________________________________
void TGeoArb8::SetPlaneVertices(Double_t zpl, Double_t *vertices) const
{
 // compute intersection points between plane at zpl and non-horizontal edges
   Double_t cf = 0.5*(fDz-zpl)/fDz;
   for (Int_t i=0; i<4; i++) {
      vertices[2*i]   = fXY[i+4][0]+cf*(fXY[i][0]-fXY[i+4][0]);
      vertices[2*i+1] = fXY[i+4][1]+cf*(fXY[i][1]-fXY[i+4][1]);
   }
}

//_____________________________________________________________________________
void TGeoArb8::SetDimensions(Double_t *param)
{
// set arb8 params in one step :
   fDz      = param[0];
   for (Int_t i=0; i<8; i++) {
      fXY[i][0] = param[2*i];
      fXY[i][1] = param[2*i+1];
   }
   ComputeTwist();
   ComputeBBox();
}   

//_____________________________________________________________________________
void TGeoArb8::SetPoints(Double_t *points) const
{
// create arb8 mesh points
   for (Int_t i=0; i<8; i++) {
      points[3*i] = fXY[i][0];
      points[3*i+1] = fXY[i][1];
      points[3*i+2] = (i<4)?-fDz:fDz;
   }
}

//_____________________________________________________________________________
void TGeoArb8::SetPoints(Float_t *points) const
{
// create arb8 mesh points
   for (Int_t i=0; i<8; i++) {
      points[3*i] = fXY[i][0];
      points[3*i+1] = fXY[i][1];
      points[3*i+2] = (i<4)?-fDz:fDz;
   }
}

//_____________________________________________________________________________
void TGeoArb8::SetVertex(Int_t vnum, Double_t x, Double_t y)
{
//  set values for a given vertex
   if (vnum<0 || vnum >7) {
      Error("SetVertex", "Invalid vertex number");
      return;
   }
   fXY[vnum][0] = x;
   fXY[vnum][1] = y;
   if (vnum == 7) {
      ComputeTwist();
      ComputeBBox();
   }
}

//_____________________________________________________________________________
void TGeoArb8::Sizeof3D() const
{
// fill size of this 3-D object
   TGeoBBox::Sizeof3D();
}

ClassImp(TGeoTrap)

//_____________________________________________________________________________
TGeoTrap::TGeoTrap()
{
   // dummy ctor
   fDz = 0;
   fTheta = 0;
   fPhi = 0;
   fH1 = fH2 = fBl1 = fBl2 = fTl1 = fTl2 = fAlpha1 = fAlpha2 = 0;
}

//_____________________________________________________________________________
TGeoTrap::TGeoTrap(Double_t dz, Double_t theta, Double_t phi)
         :TGeoArb8("", 0, 0)
{
   fDz = dz;
   fTheta = theta;
   fPhi = phi;
   fH1 = fH2 = fBl1 = fBl2 = fTl1 = fTl2 = fAlpha1 = fAlpha2 = 0;
}

//_____________________________________________________________________________
TGeoTrap::TGeoTrap(Double_t dz, Double_t theta, Double_t phi, Double_t h1,
              Double_t bl1, Double_t tl1, Double_t alpha1, Double_t h2, Double_t bl2, 
              Double_t tl2, Double_t alpha2)
         :TGeoArb8("", 0, 0)
{
// constructor.
   fDz = dz;
   fTheta = theta;
   fPhi = phi;
   fH1 = h1;
   fH2 = h2;
   fBl1 = bl1;
   fBl2 = bl2;
   fTl1 = tl1;
   fTl2 = tl2;
   fAlpha1 = alpha1;
   fAlpha2 = alpha2;
   Double_t tx = TMath::Tan(theta*TMath::DegToRad())*TMath::Cos(phi*TMath::DegToRad());
   Double_t ty = TMath::Tan(theta*TMath::DegToRad())*TMath::Sin(phi*TMath::DegToRad());
   Double_t ta1 = TMath::Tan(alpha1*TMath::DegToRad());
   Double_t ta2 = TMath::Tan(alpha2*TMath::DegToRad());
   fXY[0][0] = -dz*tx-h1*ta1-bl1;    fXY[0][1] = -dz*ty-h1;
   fXY[1][0] = -dz*tx+h1*ta1-tl1;    fXY[1][1] = -dz*ty+h1;
   fXY[2][0] = -dz*tx+h1*ta1+tl1;    fXY[2][1] = -dz*ty+h1;
   fXY[3][0] = -dz*tx-h1*ta1+bl1;    fXY[3][1] = -dz*ty-h1;
   fXY[4][0] = dz*tx-h2*ta2-bl2;    fXY[4][1] = dz*ty-h2;
   fXY[5][0] = dz*tx+h2*ta2-tl2;    fXY[5][1] = dz*ty+h2;
   fXY[6][0] = dz*tx+h2*ta2+tl2;    fXY[6][1] = dz*ty+h2;
   fXY[7][0] = dz*tx-h2*ta2+bl2;    fXY[7][1] = dz*ty-h2;
   ComputeTwist();
   if ((dz<0) || (h1<0) || (bl1<0) || (tl1<0) || 
       (h2<0) || (bl2<0) || (tl2<0)) {
      SetShapeBit(kGeoRunTimeShape);
   } 
   else TGeoArb8::ComputeBBox();
}

//_____________________________________________________________________________
TGeoTrap::TGeoTrap(const char *name, Double_t dz, Double_t theta, Double_t phi, Double_t h1,
              Double_t bl1, Double_t tl1, Double_t alpha1, Double_t h2, Double_t bl2, 
              Double_t tl2, Double_t alpha2)
         :TGeoArb8(name, 0, 0)
{
// constructor with name
   SetName(name);
   fDz = dz;
   fTheta = theta;
   fPhi = phi;
   fH1 = h1;
   fH2 = h2;
   fBl1 = bl1;
   fBl2 = bl2;
   fTl1 = tl1;
   fTl2 = tl2;
   fAlpha1 = alpha1;
   fAlpha2 = alpha2;
   for (Int_t i=0; i<8; i++) {
      fXY[i][0] = 0.0;
      fXY[i][1] = 0.0;
   }   
   Double_t tx = TMath::Tan(theta*TMath::DegToRad())*TMath::Cos(phi*TMath::DegToRad());
   Double_t ty = TMath::Tan(theta*TMath::DegToRad())*TMath::Sin(phi*TMath::DegToRad());
   Double_t ta1 = TMath::Tan(alpha1*TMath::DegToRad());
   Double_t ta2 = TMath::Tan(alpha2*TMath::DegToRad());
   fXY[0][0] = -dz*tx-h1*ta1-bl1;    fXY[0][1] = -dz*ty-h1;
   fXY[1][0] = -dz*tx+h1*ta1-tl1;    fXY[1][1] = -dz*ty+h1;
   fXY[2][0] = -dz*tx+h1*ta1+tl1;    fXY[2][1] = -dz*ty+h1;
   fXY[3][0] = -dz*tx-h1*ta1+bl1;    fXY[3][1] = -dz*ty-h1;
   fXY[4][0] = dz*tx-h2*ta2-bl2;    fXY[4][1] = dz*ty-h2;
   fXY[5][0] = dz*tx+h2*ta2-tl2;    fXY[5][1] = dz*ty+h2;
   fXY[6][0] = dz*tx+h2*ta2+tl2;    fXY[6][1] = dz*ty+h2;
   fXY[7][0] = dz*tx-h2*ta2+bl2;    fXY[7][1] = dz*ty-h2;
   ComputeTwist();
   if ((dz<0) || (h1<0) || (bl1<0) || (tl1<0) || 
       (h2<0) || (bl2<0) || (tl2<0)) {
      SetShapeBit(kGeoRunTimeShape);
   } 
   else TGeoArb8::ComputeBBox();
}

//_____________________________________________________________________________
TGeoTrap::~TGeoTrap()
{
// destructor
}

//_____________________________________________________________________________
Double_t TGeoTrap::DistFromInside(Double_t *point, Double_t *dir, Int_t iact, Double_t step, Double_t *safe) const
{
// compute distance from inside point to surface of the arb8
   if (iact<3 && safe) {
   // compute safe distance
      *safe = Safety(point, kTRUE);
      if (iact==0) return TGeoShape::Big();
      if (iact==1 && step<*safe) return TGeoShape::Big();
   }
   // compute distance to get ouside this shape
//   return TGeoArb8::DistFromInside(point, dir, iact, step, safe);

// compute distance to plane ipl :
// ipl=0 : points 0,4,1,5
// ipl=1 : points 1,5,2,6
// ipl=2 : points 2,6,3,7
// ipl=3 : points 3,7,0,4
   Double_t distmin;
   if (dir[2]<0) {
      distmin=(-fDz-point[2])/dir[2];
   } else {
      if (dir[2]>0) distmin =(fDz-point[2])/dir[2];
      else          distmin = TGeoShape::Big();
   }      
   Double_t xa,xb,xc;
   Double_t ya,yb,yc;
   for (Int_t ipl=0;ipl<4;ipl++) {
      Int_t j = (ipl+1)%4;
      xa=fXY[ipl][0];
      ya=fXY[ipl][1];
      xb=fXY[ipl+4][0];
      yb=fXY[ipl+4][1];
      xc=fXY[j][0];
      yc=fXY[j][1];
      Double_t ax,ay,az;
      ax = xb-xa;
      ay = yb-ya;
      az = 2.*fDz;
      Double_t bx,by;
      bx = xc-xa;
      by = yc-ya;
      Double_t ddotn = -dir[0]*az*by + dir[1]*az*bx+dir[2]*(ax*by-ay*bx);
      if (ddotn<=0) continue; // entering
      Double_t saf = -(point[0]-xa)*az*by + (point[1]-ya)*az*bx + (point[2]+fDz)*(ax*by-ay*bx);
      if (saf>=0.0) return 0.0;
      Double_t s = -saf/ddotn;
      if (s<distmin) distmin=s;
   }
   return distmin;   
}   

//_____________________________________________________________________________
Double_t TGeoTrap::DistFromOutside(Double_t *point, Double_t *dir, Int_t iact, Double_t step, Double_t *safe) const
{
// compute distance from outside point to surface of the arb8
   if (iact<3 && safe) {
   // compute safe distance
      *safe = Safety(point, kFALSE);
      if (iact==0) return TGeoShape::Big();
      if (iact==1 && step<*safe) return TGeoShape::Big();
   }
   // compute distance to get ouside this shape
   Bool_t in = kTRUE;
   Double_t pts[8];
   Double_t snxt;
   Double_t xnew, ynew, znew;
   Int_t i,j;
   if (point[2]<-fDz+TGeoShape::Tolerance()) {
      if (dir[2]<=0) return TGeoShape::Big();
      in = kFALSE;
      snxt = -(fDz+point[2])/dir[2];
      xnew = point[0] + snxt*dir[0];
      ynew = point[1] + snxt*dir[1];
      for (i=0;i<4;i++) {
         j = i<<1;
         pts[j] = fXY[i][0];
         pts[j+1] = fXY[i][1];
      }
      if (InsidePolygon(xnew,ynew,pts)) return snxt;   
   } else if (point[2]>fDz-TGeoShape::Tolerance()) {
      if (dir[2]>=0) return TGeoShape::Big();
      in = kFALSE;
      snxt = (fDz-point[2])/dir[2];
      xnew = point[0] + snxt*dir[0];
      ynew = point[1] + snxt*dir[1];
      for (i=0;i<4;i++) {
         j = i<<1;
         pts[j] = fXY[i+4][0];
         pts[j+1] = fXY[i+4][1];
      }
      if (InsidePolygon(xnew,ynew,pts)) return snxt;    
   }   
   snxt = TGeoShape::Big();   
    

   // check lateral faces
   Double_t dz2 =0.5/fDz;
   Double_t xa,xb,xc,xd;
   Double_t ya,yb,yc,yd;
   Double_t ax,ay,az;
   Double_t bx,by;
   Double_t ddotn, saf;
   Double_t safmin = TGeoShape::Big();
   Bool_t exiting = kFALSE;
   for (i=0; i<4; i++) {
      j = (i+1)%4;
      xa=fXY[i][0];
      ya=fXY[i][1];
      xb=fXY[i+4][0];
      yb=fXY[i+4][1];
      xc=fXY[j][0];
      yc=fXY[j][1];
      xd=fXY[4+j][0];
      yd=fXY[4+j][1];
      ax = xb-xa;
      ay = yb-ya;
      az = 2.*fDz;
      bx = xc-xa;
      by = yc-ya;
      ddotn = -dir[0]*az*by + dir[1]*az*bx+dir[2]*(ax*by-ay*bx);
      saf = (point[0]-xa)*az*by - (point[1]-ya)*az*bx - (point[2]+fDz)*(ax*by-ay*bx);
      
      if (saf<=0) {
         // face visible from point outside
         in = kFALSE;
         if (ddotn>=0) return TGeoShape::Big();      
         snxt = saf/ddotn;
         znew = point[2]+snxt*dir[2];
         if (TMath::Abs(znew)<=fDz) {
            xnew = point[0]+snxt*dir[0];
            ynew = point[1]+snxt*dir[1];
            Double_t tx1 =dz2*(xb-xa);
            Double_t ty1 =dz2*(yb-ya);
            Double_t tx2 =dz2*(xd-xc);
            Double_t ty2 =dz2*(yd-yc);
            Double_t dzp =fDz+znew;
            Double_t xs1 =xa+tx1*dzp;
            Double_t ys1 =ya+ty1*dzp;
            Double_t xs2 =xc+tx2*dzp;
            Double_t ys2 =yc+ty2*dzp;
            if (TMath::Abs(xs1-xs2)>TMath::Abs(ys1-ys2)) {
               if ((xnew-xs1)*(xs2-xnew)>=0) return snxt;
            } else {
               if ((ynew-ys1)*(ys2-ynew)>=0) return snxt;
            }      
         }
      } else {
         if (saf<safmin) {
            safmin = saf;
            if (ddotn>=0) exiting = kTRUE;
            else exiting = kFALSE;
         }   
      }   
   }   
   if (!in) return TGeoShape::Big();
   if (exiting) return TGeoShape::Big();
   return 0.0;
}   

//_____________________________________________________________________________
TGeoVolume *TGeoTrap::Divide(TGeoVolume *voldiv, const char *divname, Int_t iaxis, Int_t ndiv, 
                             Double_t start, Double_t step) 
{
//--- Divide this trapezoid shape belonging to volume "voldiv" into ndiv volumes
// called divname, from start position with the given step. Only Z divisions
// are supported. For Z divisions just return the pointer to the volume to be 
// divided. In case a wrong division axis is supplied, returns pointer to 
// volume that was divided.
   TGeoShape *shape;           //--- shape to be created
   TGeoVolume *vol;            //--- division volume to be created
   TGeoVolumeMulti *vmulti;    //--- generic divided volume
   TGeoPatternFinder *finder;  //--- finder to be attached 
   TString opt = "";           //--- option to be attached
   if (iaxis!=3) {
      Error("Divide", "cannot divide trapezoids on other axis than Z");
      return 0;
   }
   Double_t end = start+ndiv*step;
   Double_t points_lo[8];
   Double_t points_hi[8];
   finder = new TGeoPatternTrapZ(voldiv, ndiv, start, end);
   voldiv->SetFinder(finder);
   finder->SetDivIndex(voldiv->GetNdaughters());
   opt = "Z";
   vmulti = gGeoManager->MakeVolumeMulti(divname, voldiv->GetMedium());
   Double_t txz = ((TGeoPatternTrapZ*)finder)->GetTxz();
   Double_t tyz = ((TGeoPatternTrapZ*)finder)->GetTyz();
   Double_t zmin, zmax, ox,oy,oz;
   for (Int_t idiv=0; idiv<ndiv; idiv++) {
      zmin = start+idiv*step;
      zmax = start+(idiv+1)*step;
      oz = start+idiv*step+step/2;
      ox = oz*txz;
      oy = oz*tyz;
      SetPlaneVertices(zmin, &points_lo[0]);
      SetPlaneVertices(zmax, &points_hi[0]);
      shape = new TGeoTrap(step/2, fTheta, fPhi);
      for (Int_t vert1=0; vert1<4; vert1++)
         ((TGeoArb8*)shape)->SetVertex(vert1, points_lo[2*vert1]-ox, points_lo[2*vert1+1]-oy);
      for (Int_t vert2=0; vert2<4; vert2++)
         ((TGeoArb8*)shape)->SetVertex(vert2+4, points_hi[2*vert2]-ox, points_hi[2*vert2+1]-oy);
      vol = new TGeoVolume(divname, shape, voldiv->GetMedium());
      vmulti->AddVolume(vol);
      voldiv->AddNodeOffset(vol, idiv, oz, opt.Data());
      ((TGeoNodeOffset*)voldiv->GetNodes()->At(voldiv->GetNdaughters()-1))->SetFinder(finder);
   }
   return vmulti;
}   

//_____________________________________________________________________________
TGeoShape *TGeoTrap::GetMakeRuntimeShape(TGeoShape *mother, TGeoMatrix * /*mat*/) const
{
// in case shape has some negative parameters, these has to be computed
// in order to fit the mother
   if (!TestShapeBit(kGeoRunTimeShape)) return 0;
   if (mother->IsRunTimeShape()) {
      Error("GetMakeRuntimeShape", "invalid mother");
      return 0;
   }
   Double_t dz, h1, bl1, tl1, h2, bl2, tl2;
   if (fDz<0) dz=((TGeoTrap*)mother)->GetDz();
   else dz=fDz;
   if (fH1<0) 
      h1 = ((TGeoTrap*)mother)->GetH1();
    else 
      h1 = fH1;
   if (fH2<0) 
      h2 = ((TGeoTrap*)mother)->GetH2();
    else 
      h2 = fH2;
   if (fBl1<0) 
      bl1 = ((TGeoTrap*)mother)->GetBl1();
    else 
      bl1 = fBl1;
   if (fBl2<0) 
      bl2 = ((TGeoTrap*)mother)->GetBl2();
    else 
      bl2 = fBl2;
   if (fTl1<0) 
      tl1 = ((TGeoTrap*)mother)->GetTl1();
    else 
      tl1 = fTl1;
   if (fTl2<0) 
      tl2 = ((TGeoTrap*)mother)->GetTl2();
    else 
      tl2 = fTl2;
   return (new TGeoTrap(dz, fTheta, fPhi, h1, bl1, tl1, fAlpha1, h2, bl2, tl2, fAlpha2));
}

//_____________________________________________________________________________
Double_t TGeoTrap::Safety(Double_t *point, Bool_t in) const
{
// Computes the closest distance from given point to this shape, according
// to option. 
   Double_t safe = TGeoShape::Big();
   Double_t saf[5];
   Double_t norm[3]; // normal to current facette
   Int_t i,j;       // current facette index
   Double_t x0, y0, z0=-fDz, x1, y1, z1=fDz, x2, y2;
   Double_t ax, ay, az=z1-z0, bx, by;
   Double_t fn;
   //---> compute safety for lateral planes
   for (i=0; i<4; i++) {
      x0 = fXY[i][0];
      y0 = fXY[i][1];
      x1 = fXY[i+4][0];
      y1 = fXY[i+4][1];
      ax = x1-x0;
      ay = y1-y0;
      az = z1-z0;
      j  = (i+1)%4;
	  x2 = fXY[j][0];
      y2 = fXY[j][1];
      bx = x2-x0;
      by = y2-y0;
      if (bx==0 && by==0) {
         x2 = fXY[4+j][0];
         y2 = fXY[4+j][1];
         bx = x2-x1;
         by = y2-y1;
         if (bx==0 && by==0) continue;
      }
      norm[0] = -az*by;
      norm[1] = az*bx;
      norm[2] = ax*by-ay*bx;
      fn = TMath::Sqrt(norm[0]*norm[0]+norm[1]*norm[1]+norm[2]*norm[2]);
      if (fn<1E-10) continue;
      saf[i] = (x0-point[0])*norm[0]+(y0-point[1])*norm[1]+(-fDz-point[2])*norm[2];
      if (in) {
         saf[i]=TMath::Abs(saf[i])/fn; // they should be all positive anyway
      } else {
         saf[i] = -saf[i]/fn;   // only negative values are interesting
      }   
   }
   saf[4] = fDz-TMath::Abs(point[2]);
   if (in) {
      safe = saf[0];
	  for (j=1;j<5;j++) if (saf[j] <safe) safe = saf[j];
   } else {
      saf[4]=-saf[4];
      safe = saf[0];
	  for (j=1;j<5;j++) if (saf[j] >safe) safe = saf[j];
   }
   return safe;
}

//_____________________________________________________________________________
void TGeoTrap::SavePrimitive(ofstream &out, Option_t * /*option*/)
{
// Save a primitive as a C++ statement(s) on output stream "out".
   if (TObject::TestBit(kGeoSavePrimitive)) return;
   out << "   // Shape: " << GetName() << " type: " << ClassName() << endl;
   out << "   dz     = " << fDz << ";" << endl;
   out << "   theta  = " << fTheta << ";" << endl;
   out << "   phi    = " << fPhi << ";" << endl;
   out << "   h1     = " << fH1<< ";" << endl;
   out << "   bl1    = " << fBl1<< ";" << endl;
   out << "   tl1    = " << fTl1<< ";" << endl;
   out << "   alpha1 = " << fAlpha1 << ";" << endl;
   out << "   h2     = " << fH2 << ";" << endl;
   out << "   bl2    = " << fBl2<< ";" << endl;
   out << "   tl2    = " << fTl2<< ";" << endl;
   out << "   alpha2 = " << fAlpha2 << ";" << endl;
   out << "   pShape = new TGeoTrap(\"" << GetName() << "\", dz,theta,phi,h1,bl1,tl1,alpha1,h2,bl2,tl2,alpha2);" << endl;
   TObject::SetBit(TGeoShape::kGeoSavePrimitive);
}

ClassImp(TGeoGtra)

//_____________________________________________________________________________
TGeoGtra::TGeoGtra()
{
   // dummy ctor
   fTwistAngle = 0;
}

//_____________________________________________________________________________
TGeoGtra::TGeoGtra(Double_t dz, Double_t theta, Double_t phi, Double_t twist, Double_t h1,
              Double_t bl1, Double_t tl1, Double_t alpha1, Double_t h2, Double_t bl2, 
              Double_t tl2, Double_t alpha2)
         :TGeoTrap(dz, theta, phi, h1, bl1, tl1, alpha1, h2, bl2, tl2, alpha2)     
{
// constructor. 
   fTheta = theta;
   fPhi = phi;
   fH1 = h1;
   fH2 = h2;
   fBl1 = bl1;
   fBl2 = bl2;
   fTl1 = tl1;
   fTl2 = tl2;
   fAlpha1 = alpha1;
   fAlpha2 = alpha2;
   Double_t x, y, dx, dy, dx1, dx2, th, ph, al1, al2;
   al1 = alpha1*TMath::DegToRad();
   al2 = alpha2*TMath::DegToRad();
   th = theta*TMath::DegToRad();
   ph = phi*TMath::DegToRad();
   dx = 2*dz*TMath::Sin(th)*TMath::Cos(ph);
   dy = 2*dz*TMath::Sin(th)*TMath::Sin(ph);
   fDz = dz;
   dx1 = 2*h1*TMath::Tan(al1);
   dx2 = 2*h2*TMath::Tan(al2);

   fTwistAngle = twist;

   Int_t i;
   for (i=0; i<8; i++) {
      fXY[i][0] = 0.0;
      fXY[i][1] = 0.0;
   }   

   fXY[0][0] = -bl1;                fXY[0][1] = -h1;
   fXY[1][0] = -tl1+dx1;            fXY[1][1] = h1;
   fXY[2][0] = tl1+dx1;             fXY[2][1] = h1;
   fXY[3][0] = bl1;                 fXY[3][1] = -h1;
   fXY[4][0] = -bl2+dx;             fXY[4][1] = -h2+dy;
   fXY[5][0] = -tl2+dx+dx2;         fXY[5][1] = h2+dy;
   fXY[6][0] = tl2+dx+dx2;          fXY[6][1] = h2+dy;
   fXY[7][0] = bl2+dx;              fXY[7][1] = -h2+dy;
   for (i=4; i<8; i++) {
      x = fXY[i][0];
      y = fXY[i][1];
      fXY[i][0] = x*TMath::Cos(twist*TMath::DegToRad()) + y*TMath::Sin(twist*TMath::DegToRad());
      fXY[i][1] = -x*TMath::Sin(twist*TMath::DegToRad()) + y*TMath::Cos(twist*TMath::DegToRad());      
   }
   ComputeTwist();
   if ((dz<0) || (h1<0) || (bl1<0) || (tl1<0) || 
       (h2<0) || (bl2<0) || (tl2<0)) SetShapeBit(kGeoRunTimeShape);
   else TGeoArb8::ComputeBBox();
}

//_____________________________________________________________________________
TGeoGtra::TGeoGtra(const char *name, Double_t dz, Double_t theta, Double_t phi, Double_t twist, Double_t h1,
              Double_t bl1, Double_t tl1, Double_t alpha1, Double_t h2, Double_t bl2, 
              Double_t tl2, Double_t alpha2)
         :TGeoTrap(name, dz, theta, phi, h1, bl1, tl1, alpha1, h2, bl2, tl2, alpha2)     
{
// constructor. 
   SetName(name);
   fTheta = theta;
   fPhi = phi;
   fH1 = h1;
   fH2 = h2;
   fBl1 = bl1;
   fBl2 = bl2;
   fTl1 = tl1;
   fTl2 = tl2;
   fAlpha1 = alpha1;
   fAlpha2 = alpha2;
   Double_t x, y, dx, dy, dx1, dx2, th, ph, al1, al2;
   al1 = alpha1*TMath::DegToRad();
   al2 = alpha2*TMath::DegToRad();
   th = theta*TMath::DegToRad();
   ph = phi*TMath::DegToRad();
   dx = 2*dz*TMath::Sin(th)*TMath::Cos(ph);
   dy = 2*dz*TMath::Sin(th)*TMath::Sin(ph);
   fDz = dz;
   dx1 = 2*h1*TMath::Tan(al1);
   dx2 = 2*h2*TMath::Tan(al2);

   fTwistAngle = twist;

   Int_t i;
   for (i=0; i<8; i++) {
      fXY[i][0] = 0.0;
      fXY[i][1] = 0.0;
   }   

   fXY[0][0] = -bl1;                fXY[0][1] = -h1;
   fXY[1][0] = -tl1+dx1;            fXY[1][1] = h1;
   fXY[2][0] = tl1+dx1;             fXY[2][1] = h1;
   fXY[3][0] = bl1;                 fXY[3][1] = -h1;
   fXY[4][0] = -bl2+dx;             fXY[4][1] = -h2+dy;
   fXY[5][0] = -tl2+dx+dx2;         fXY[5][1] = h2+dy;
   fXY[6][0] = tl2+dx+dx2;          fXY[6][1] = h2+dy;
   fXY[7][0] = bl2+dx;              fXY[7][1] = -h2+dy;
   for (i=4; i<8; i++) {
      x = fXY[i][0];
      y = fXY[i][1];
      fXY[i][0] = x*TMath::Cos(twist*TMath::DegToRad()) + y*TMath::Sin(twist*TMath::DegToRad());
      fXY[i][1] = -x*TMath::Sin(twist*TMath::DegToRad()) + y*TMath::Cos(twist*TMath::DegToRad());      
   }
   ComputeTwist();
   if ((dz<0) || (h1<0) || (bl1<0) || (tl1<0) || 
       (h2<0) || (bl2<0) || (tl2<0)) SetShapeBit(kGeoRunTimeShape);
   else TGeoArb8::ComputeBBox();
}

//_____________________________________________________________________________
TGeoGtra::~TGeoGtra()
{
// destructor
}

//_____________________________________________________________________________
Double_t TGeoGtra::DistFromInside(Double_t *point, Double_t *dir, Int_t iact, Double_t step, Double_t *safe) const
{
// compute distance from inside point to surface of the arb8
   if (iact<3 && safe) {
   // compute safe distance
      *safe = Safety(point, kTRUE);
      if (iact==0) return TGeoShape::Big();
      if (iact==1 && step<*safe) return TGeoShape::Big();
   }
   // compute distance to get ouside this shape
   return TGeoArb8::DistFromInside(point, dir, iact, step, safe);
}   

//_____________________________________________________________________________
Double_t TGeoGtra::DistFromOutside(Double_t *point, Double_t *dir, Int_t iact, Double_t step, Double_t *safe) const
{
// compute distance from inside point to surface of the arb8
   if (iact<3 && safe) {
   // compute safe distance
      *safe = Safety(point, kTRUE);
      if (iact==0) return TGeoShape::Big();
      if (iact==1 && step<*safe) return TGeoShape::Big();
   }
   // compute distance to get ouside this shape
   return TGeoArb8::DistFromOutside(point, dir, iact, step, safe);
}   

//_____________________________________________________________________________
TGeoShape *TGeoGtra::GetMakeRuntimeShape(TGeoShape *mother, TGeoMatrix * /*mat*/) const
{
// in case shape has some negative parameters, these has to be computed
// in order to fit the mother
   if (!TestShapeBit(kGeoRunTimeShape)) return 0;
   if (mother->IsRunTimeShape()) {
      Error("GetMakeRuntimeShape", "invalid mother");
      return 0;
   }
   Double_t dz, h1, bl1, tl1, h2, bl2, tl2;
   if (fDz<0) dz=((TGeoTrap*)mother)->GetDz();
   else dz=fDz;
   if (fH1<0) 
      h1 = ((TGeoTrap*)mother)->GetH1();
    else 
      h1 = fH1;
   if (fH2<0) 
      h2 = ((TGeoTrap*)mother)->GetH2();
    else 
      h2 = fH2;
   if (fBl1<0) 
      bl1 = ((TGeoTrap*)mother)->GetBl1();
    else 
      bl1 = fBl1;
   if (fBl2<0) 
      bl2 = ((TGeoTrap*)mother)->GetBl2();
    else 
      bl2 = fBl2;
   if (fTl1<0) 
      tl1 = ((TGeoTrap*)mother)->GetTl1();
    else 
      tl1 = fTl1;
   if (fTl2<0) 
      tl2 = ((TGeoTrap*)mother)->GetTl2();
    else 
      tl2 = fTl2;
   return (new TGeoGtra(dz, fTheta, fPhi, fTwistAngle ,h1, bl1, tl1, fAlpha1, h2, bl2, tl2, fAlpha2));
}

//_____________________________________________________________________________
void TGeoGtra::SavePrimitive(ofstream &out, Option_t * /*option*/)
{
// Save a primitive as a C++ statement(s) on output stream "out".
   if (TObject::TestBit(kGeoSavePrimitive)) return;  
   out << "   // Shape: " << GetName() << " type: " << ClassName() << endl;
   out << "   dz     = " << fDz << ";" << endl;
   out << "   theta  = " << fTheta << ";" << endl;
   out << "   phi    = " << fPhi << ";" << endl;
   out << "   twist  = " << fTwistAngle << ";" << endl;
   out << "   h1     = " << fH1<< ";" << endl;
   out << "   bl1    = " << fBl1<< ";" << endl;
   out << "   tl1    = " << fTl1<< ";" << endl;
   out << "   alpha1 = " << fAlpha1 << ";" << endl;
   out << "   h2     = " << fH2 << ";" << endl;
   out << "   bl2    = " << fBl2<< ";" << endl;
   out << "   tl2    = " << fTl2<< ";" << endl;
   out << "   alpha2 = " << fAlpha2 << ";" << endl;
   out << "   pShape = new TGeoGtra(\"" << GetName() << "\", dz,theta,phi,twist,h1,bl1,tl1,alpha1,h2,bl2,tl2,alpha2);" << endl;
   TObject::SetBit(TGeoShape::kGeoSavePrimitive);
}
