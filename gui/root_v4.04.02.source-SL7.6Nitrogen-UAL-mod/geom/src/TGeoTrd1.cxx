// @(#)root/geom:$Name:  $:$Id: TGeoTrd1.cxx,v 1.30 2005/03/09 18:19:26 brun Exp $
// Author: Andrei Gheata   24/10/01
// TGeoTrd1::Contains() and DistFromInside() implemented by Mihaela Gheata

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//_____________________________________________________________________________
// TGeoTrd1 - a trapezoid with only x length varying with z. It has 4
//   parameters, the half length in x at the low z surface, that at the
//   high z surface, the half length in y, and in z
//
//_____________________________________________________________________________
//Begin_Html
/*
<img src="gif/t_trd1.gif">
*/
//End_Html

//Begin_Html
/*
<img src="gif/t_trd1divY.gif">
*/
//End_Html

//Begin_Html
/*
<img src="gif/t_trd1divZ.gif">
*/
//End_Html

//Begin_Html
/*
<img src="gif/t_trd1divstepZ.gif">
*/
//End_Html

#include "Riostream.h"
#include "TROOT.h"

#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoTrd1.h"

ClassImp(TGeoTrd1)
   
//_____________________________________________________________________________
TGeoTrd1::TGeoTrd1()
{
   // dummy ctor
   fDz = fDx1 = fDx2 = fDy = 0;
   SetShapeBit(kGeoTrd1);
}

//_____________________________________________________________________________
TGeoTrd1::TGeoTrd1(Double_t dx1, Double_t dx2, Double_t dy, Double_t dz)
         :TGeoBBox(0,0,0)
{
// constructor. 
   SetShapeBit(kGeoTrd1);
   fDx1 = dx1;
   fDx2 = dx2;
   fDy = dy;
   fDz = dz;
   if ((dx1<0) || (dx2<0) || (dy<0) || (dz<0)) {
      SetShapeBit(kGeoRunTimeShape);
      printf("trd1 : dx1=%f, dx2=%f, dy=%f, dz=%f\n",
              dx1,dx2,dy,dz);
   }
   else ComputeBBox();
}

//_____________________________________________________________________________
TGeoTrd1::TGeoTrd1(const char *name, Double_t dx1, Double_t dx2, Double_t dy, Double_t dz)
         :TGeoBBox(name, 0,0,0)
{
// constructor. 
   SetShapeBit(kGeoTrd1);
   fDx1 = dx1;
   fDx2 = dx2;
   fDy = dy;
   fDz = dz;
   if ((dx1<0) || (dx2<0) || (dy<0) || (dz<0)) {
      SetShapeBit(kGeoRunTimeShape);
      printf("trd1 : dx1=%f, dx2=%f, dy=%f, dz=%f\n",
              dx1,dx2,dy,dz);
   }
   else ComputeBBox();
}

//_____________________________________________________________________________
TGeoTrd1::TGeoTrd1(Double_t *param)
         :TGeoBBox(0,0,0)
{
   // ctor with an array of parameters
   // param[0] = dx1
   // param[1] = dx2
   // param[2] = dy
   // param[3] = dz
   SetShapeBit(kGeoTrd1);
   SetDimensions(param);
   if ((fDx1<0) || (fDx2<0) || (fDy<=0) || (fDz<=0)) SetShapeBit(kGeoRunTimeShape);
   else ComputeBBox();
}

//_____________________________________________________________________________
TGeoTrd1::~TGeoTrd1()
{
// destructor
}

//_____________________________________________________________________________
void TGeoTrd1::ComputeBBox()
{
// compute bounding box for a trd1
   fDX = TMath::Max(fDx1, fDx2);
   fDY = fDy;
   fDZ = fDz;
   memset(fOrigin, 0, 3*sizeof(Double_t));
}

//_____________________________________________________________________________   
void TGeoTrd1::ComputeNormal(Double_t *point, Double_t *dir, Double_t *norm)
{
// Compute normal to closest surface from POINT.
   Double_t safe, safemin;
   //--- Compute safety first
   Double_t fx = 0.5*(fDx1-fDx2)/fDz;
   Double_t calf = 1./TMath::Sqrt(1.0+fx*fx);
   // check Z facettes
   safe = safemin = TMath::Abs(fDz-TMath::Abs(point[2]));
   norm[0] = norm[1] = 0;
   norm[2] = (dir[2]>=0)?1:-1;
   if (safe<1E-6) return;
   // check X facettes
   Double_t distx = 0.5*(fDx1+fDx2)-fx*point[2];
   if (distx>=0) {
      safe=TMath::Abs(distx-TMath::Abs(point[0]))*calf;
      if (safe<safemin) {
         safemin = safe;
         norm[0] = calf;
         norm[1] = 0;
         norm[2] = calf*fx;
         Double_t dot = norm[0]*dir[0]+norm[1]*dir[1]+norm[2]*dir[2];
         if (dot<0) norm[0]=-norm[0];
         if (safe<1E-6) return;
      }
   }      
   // check Y facettes
   safe = TMath::Abs(fDy-TMath::Abs(point[1]));
   if (safe<safemin) {
      norm[0] = norm[2] = 0;
      norm[1] = (dir[1]>=0)?1:-1;
   }
}

//_____________________________________________________________________________
Bool_t TGeoTrd1::Contains(Double_t *point) const
{
// test if point is inside this shape
   // check Z range
   if (TMath::Abs(point[2]) > fDz) return kFALSE;
   // then y
   if (TMath::Abs(point[1]) > fDy) return kFALSE;
   // then x
   Double_t dx = 0.5*(fDx2*(point[2]+fDz)+fDx1*(fDz-point[2]))/fDz;
   if (TMath::Abs(point[0]) > dx) return kFALSE;
   return kTRUE;
}

//_____________________________________________________________________________
Double_t TGeoTrd1::DistFromInside(Double_t *point, Double_t *dir, Int_t iact, Double_t step, Double_t *safe) const
{
// Compute distance from inside point to surface of the trd1
// Boundary safe algorithm.
   Double_t snxt = TGeoShape::Big();
   if (iact<3 && safe) {
   // compute safe distance
      *safe = Safety(point, kTRUE);
      if (iact==0) return TGeoShape::Big();
      if (iact==1 && step<*safe) return TGeoShape::Big();
   }

   //--- Compute safety first
   Double_t fx = 0.5*(fDx1-fDx2)/fDz;
   Double_t cn;
   Double_t distx = 0.5*(fDx1+fDx2)-fx*point[2];
   //--- Compute distance to this shape
   // first check if Z facettes are crossed
   Double_t dist[3];
   for (Int_t i=0; i<3; i++) dist[i]=TGeoShape::Big();
   if (dir[2]<0) {
      dist[0]=-(point[2]+fDz)/dir[2];
   } else if (dir[2]>0) {
      dist[0]=(fDz-point[2])/dir[2];
   } 
   if (dist[0]<=0) return 0.0;     
   // now check X facettes
   cn = -dir[0]+fx*dir[2];
   if (cn>0) {
      dist[1] = point[0]+distx;
      if (dist[1]<=0) return 0.0;
      dist[1] /= cn;
   }   
   cn = dir[0]+fx*dir[2];
   if (cn>0) {
      Double_t s = distx-point[0];
      if (s<=0) return 0.0;
      s /= cn;
      if (s<dist[1]) dist[1] = s;
   }
   // now check Y facettes
   if (dir[1]<0) {
      dist[2]=-(point[1]+fDy)/dir[1];
   } else if (dir[1]>0) {
      dist[2]=(fDy-point[1])/dir[1];
   }  
   if (dist[2]<=0) return 0.0;    
   snxt = dist[TMath::LocMin(3,dist)];
   return snxt;
}

//_____________________________________________________________________________
void TGeoTrd1::GetVisibleCorner(Double_t *point, Double_t *vertex, Double_t *normals) const
{
// get the most visible corner from outside point and the normals
   Double_t fx = 0.5*(fDx1-fDx2)/fDz;
   Double_t calf = 1./TMath::Sqrt(1.0+fx*fx);
   Double_t salf = calf*fx;
   // check visibility of X faces
   Double_t distx = 0.5*(fDx1+fDx2)-fx*point[2];
   memset(normals, 0, 9*sizeof(Double_t));
   TGeoTrd1 *trd1 = (TGeoTrd1*)this;
   if (point[0]>distx) {
   // hi x face visible
      trd1->SetShapeBit(kGeoVisX);
      normals[0]=calf;
      normals[2]=salf;
   } else {   
      trd1->SetShapeBit(kGeoVisX, kFALSE);
      normals[0]=-calf;
      normals[2]=salf;
   }
   if (point[1]>fDy) {
   // hi y face visible
      trd1->SetShapeBit(kGeoVisY);
      normals[4]=1;
   } else {
      trd1->SetShapeBit(kGeoVisY, kFALSE);
      normals[4]=-1;  
   }   
   if (point[2]>fDz) {
   // hi z face visible
      trd1->SetShapeBit(kGeoVisZ);
      normals[8]=1;
   } else {
      trd1->SetShapeBit(kGeoVisZ, kFALSE);
      normals[8]=-1;  
   }
   SetVertex(vertex);
}

//_____________________________________________________________________________
void TGeoTrd1::GetOppositeCorner(Double_t * /*point*/, Int_t inorm, Double_t *vertex, Double_t *normals) const
{
// get the opposite corner of the intersected face
   TGeoTrd1 *trd1 = (TGeoTrd1*)this;
   if (inorm != 0) {
   // change x face
      trd1->SetShapeBit(kGeoVisX, !TestShapeBit(kGeoVisX));
      normals[0]=-normals[0];
   }
   if (inorm != 1) {
   // change y face
      trd1->SetShapeBit(kGeoVisY, !TestShapeBit(kGeoVisY));
      normals[4]=-normals[4];
   } 
   if (inorm != 2) {
   // hi z face visible
      trd1->SetShapeBit(kGeoVisZ, !TestShapeBit(kGeoVisZ));
      normals[8]=-normals[8];
   } 
   SetVertex(vertex);
}

//_____________________________________________________________________________
Double_t TGeoTrd1::DistFromOutside(Double_t *point, Double_t *dir, Int_t iact, Double_t step, Double_t *safe) const
{
// Compute distance from outside point to surface of the trd1
// Boundary safe algorithm
   Double_t snxt = TGeoShape::Big();
   if (iact<3 && safe) {
   // compute safe distance
      *safe = Safety(point, kFALSE);
      if (iact==0) return TGeoShape::Big();
      if (iact==1 && step<*safe) return TGeoShape::Big();
   }
   // find a visible face
   Double_t xnew,ynew,znew;
   Double_t fx = 0.5*(fDx1-fDx2)/fDz;
   Double_t cn;
   Double_t distx = 0.5*(fDx1+fDx2)-fx*point[2];
   Bool_t in = kTRUE;
   Double_t safx = distx-TMath::Abs(point[0]);
   Double_t safy = fDy-TMath::Abs(point[1]);
   Double_t safz = fDz-TMath::Abs(point[2]);
   
   //--- Compute distance to this shape
   // first check if Z facettes are crossed
   if (point[2]<=-fDz) {
      if (dir[2]<=0) return TGeoShape::Big();
      in = kFALSE;
      snxt = -(fDz+point[2])/dir[2];
      // find extrapolated X and Y
      xnew = point[0]+snxt*dir[0];
      if (TMath::Abs(xnew) <= fDx1) {
         ynew = point[1]+snxt*dir[1];
         if (TMath::Abs(ynew) <= fDy) return snxt;
      }
   } else if (point[2]>=fDz) {
      if (dir[2]>=0) return TGeoShape::Big();
      in = kFALSE;
      snxt = (fDz-point[2])/dir[2];
      // find extrapolated X and Y
      xnew = point[0]+snxt*dir[0];
      if (TMath::Abs(xnew) <= fDx2) {
         ynew = point[1]+snxt*dir[1];
         if (TMath::Abs(ynew) <= fDy) return snxt;
      }
   }   
   // check if X facettes are crossed
   if (point[0]<=-distx) {
      cn = -dir[0]+fx*dir[2];
      if (cn>=0) return TGeoShape::Big();
      in = kFALSE;
      snxt = (point[0]+distx)/cn;
      // find extrapolated Y and Z
      ynew = point[1]+snxt*dir[1];
      if (TMath::Abs(ynew) <= fDy) {
         znew = point[2]+snxt*dir[2];
         if (TMath::Abs(znew) <= fDz) return snxt;
      }
   }            
   if (point[0]>=distx) {
      cn = dir[0]+fx*dir[2];
      if (cn>=0) return TGeoShape::Big();
      in = kFALSE;
      snxt = (distx-point[0])/cn;
      // find extrapolated Y and Z
      ynew = point[1]+snxt*dir[1];
      if (TMath::Abs(ynew) < fDy) {
         znew = point[2]+snxt*dir[2];
         if (TMath::Abs(znew) < fDz) return snxt;
      }
   }
   // finally check Y facettes
   if (point[1]<=-fDy) {
      cn = -dir[1];            
      if (cn>=0) return TGeoShape::Big();
      in = kFALSE;
      snxt = (point[1]+fDy)/cn;
      // find extrapolated X and Z
      znew = point[2]+snxt*dir[2];
      if (TMath::Abs(znew) < fDz) {
         xnew = point[0]+snxt*dir[0];
         Double_t dx = 0.5*(fDx1+fDx2)-fx*znew;
         if (TMath::Abs(xnew) < dx) return snxt;
      }
   } else if (point[1]>=fDy) {
      cn = dir[1];        
      if (cn>=0) return TGeoShape::Big();    
      in = kFALSE;
      snxt = (fDy-point[1])/cn;
      // find extrapolated X and Z
      znew = point[2]+snxt*dir[2];
      if (TMath::Abs(znew) < fDz) {
         xnew = point[0]+snxt*dir[0];
         Double_t dx = 0.5*(fDx1+fDx2)-fx*znew;
         if (TMath::Abs(xnew) < dx) return snxt;
      }
   }
   if (!in) return TGeoShape::Big();
   // Point actually inside
   if (safz<safx && safz<safy) {
      if (point[2]*dir[2]>=0) return TGeoShape::Big();
      return 0.0;
   }
   if (safy<safx) {
      if (point[1]*dir[1]>=0) return TGeoShape::Big();
      return 0.0;
   }
   cn = TMath::Sign(1.0,point[0])*dir[0]+fx*dir[2];     
   if (cn>=0) return TGeoShape::Big();
   return 0.0;
}

//_____________________________________________________________________________
TGeoVolume *TGeoTrd1::Divide(TGeoVolume *voldiv, const char *divname, Int_t iaxis, Int_t ndiv, 
                             Double_t start, Double_t step) 
{
//--- Divide this trd1 shape belonging to volume "voldiv" into ndiv volumes
// called divname, from start position with the given step. Returns pointer
// to created division cell volume in case of Y divisions. For Z divisions just
// return the pointer to the volume to be divided. In case a wrong 
// division axis is supplied, returns pointer to volume that was divided.
   TGeoShape *shape;           //--- shape to be created
   TGeoVolume *vol;            //--- division volume to be created
   TGeoVolumeMulti *vmulti;    //--- generic divided volume
   TGeoPatternFinder *finder;  //--- finder to be attached 
   TString opt = "";           //--- option to be attached
   Double_t zmin, zmax, dx1n, dx2n;
   Int_t id;
   Double_t end = start+ndiv*step;
   switch (iaxis) {
      case 1:
         Warning("Divide", "dividing a Trd1 on X not implemented");
         return 0;
      case 2:
         finder = new TGeoPatternY(voldiv, ndiv, start, end);
         voldiv->SetFinder(finder);
         finder->SetDivIndex(voldiv->GetNdaughters());            
         shape = new TGeoTrd1(fDx1, fDx2, step/2, fDz);
         vol = new TGeoVolume(divname, shape, voldiv->GetMedium()); 
         vmulti = gGeoManager->MakeVolumeMulti(divname, voldiv->GetMedium());
         vmulti->AddVolume(vol);
         opt = "Y";
         for (id=0; id<ndiv; id++) {
            voldiv->AddNodeOffset(vol, id, start+step/2+id*step, opt.Data());
            ((TGeoNodeOffset*)voldiv->GetNodes()->At(voldiv->GetNdaughters()-1))->SetFinder(finder);
         }
         return vmulti;
      case 3:
         finder = new TGeoPatternZ(voldiv, ndiv, start, end);
         voldiv->SetFinder(finder);
         finder->SetDivIndex(voldiv->GetNdaughters());            
         vmulti = gGeoManager->MakeVolumeMulti(divname, voldiv->GetMedium());
         for (id=0; id<ndiv; id++) {
            zmin = start+id*step;
            zmax = start+(id+1)*step;
            dx1n = 0.5*(fDx1*(fDz-zmin)+fDx2*(fDz+zmin))/fDz;
            dx2n = 0.5*(fDx1*(fDz-zmax)+fDx2*(fDz+zmax))/fDz;
            shape = new TGeoTrd1(dx1n, dx2n, fDy, step/2.);
            vol = new TGeoVolume(divname, shape, voldiv->GetMedium()); 
            vmulti->AddVolume(vol);
            opt = "Z";             
            voldiv->AddNodeOffset(vol, id, start+step/2+id*step, opt.Data());
            ((TGeoNodeOffset*)voldiv->GetNodes()->At(voldiv->GetNdaughters()-1))->SetFinder(finder);
         }
         return vmulti;
      default:
         Error("Divide", "Wrong axis type for division");
         return 0;
   }
}

//_____________________________________________________________________________
Double_t TGeoTrd1::GetAxisRange(Int_t iaxis, Double_t &xlo, Double_t &xhi) const
{
// Get range of shape for a given axis.
   xlo = 0;
   xhi = 0;
   Double_t dx = 0;
   switch (iaxis) {
      case 2:
         xlo = -fDy;
         xhi = fDy;
         dx = xhi-xlo;
         return dx;
      case 3:
         xlo = -fDz;
         xhi = fDz;
         dx = xhi-xlo;
         return dx;
   }
   return dx;
}         

//_____________________________________________________________________________
void TGeoTrd1::GetBoundingCylinder(Double_t *param) const
{
//--- Fill vector param[4] with the bounding cylinder parameters. The order
// is the following : Rmin, Rmax, Phi1, Phi2
   TGeoBBox::GetBoundingCylinder(param);
}   

//_____________________________________________________________________________
Int_t TGeoTrd1::GetFittingBox(const TGeoBBox *parambox, TGeoMatrix *mat, Double_t &dx, Double_t &dy, Double_t &dz) const
{
// Fills real parameters of a positioned box inside this. Returns 0 if successfull.
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
   //-> check if Y range is fixed
   if (dd[1]<0) {
      dd[1] = TMath::Min(origin[1]+fDy, fDy-origin[1]); 
      if (dd[1]<0) {
         Error("GetFittingBox", "wrong matrix");
         return 1;
      }
   }
   if (dd[0]>=0) {
      dx = dd[0];
      dy = dd[1];
      dz = dd[2];
      return 0;
   }
   //-> check now range at Z = origin[2] +/- dd[2]
   Double_t fx = 0.5*(fDx1-fDx2)/fDz;
   Double_t dx0 = 0.5*(fDx1+fDx2);
   Double_t z=origin[2]-dd[2];
   dd[0] = dx0-fx*z-origin[0]; 
   z=origin[2]+dd[2];
   dd[0] = TMath::Min(dd[0], dx0-fx*z-origin[0]);
   if (dd[0]<0) {
      Error("GetFittingBox", "wrong matrix");
      return 1;
   }   
   dx = dd[0];
   dy = dd[1];
   dz = dd[2];
   return 0;
}   

//_____________________________________________________________________________
TGeoShape *TGeoTrd1::GetMakeRuntimeShape(TGeoShape *mother, TGeoMatrix * /*mat*/) const
{
// in case shape has some negative parameters, these has to be computed
// in order to fit the mother
   if (!TestShapeBit(kGeoRunTimeShape)) return 0;
   if (!mother->TestShapeBit(kGeoTrd1)) {
      Error("GetMakeRuntimeShape", "invalid mother");
      return 0;
   }
   Double_t dx1, dx2, dy, dz;
   if (fDx1<0) dx1=((TGeoTrd1*)mother)->GetDx1();
   else dx1=fDx1;
   if (fDx2<0) dx2=((TGeoTrd1*)mother)->GetDx2();
   else dx2=fDx2;
   if (fDy<0) dy=((TGeoTrd1*)mother)->GetDy();
   else dy=fDy;
   if (fDz<0) dz=((TGeoTrd1*)mother)->GetDz();
   else dz=fDz;

   return (new TGeoTrd1(dx1, dx2, dy, dz));
}

//_____________________________________________________________________________
void TGeoTrd1::InspectShape() const
{
// print shape parameters
   printf("*** Shape %s: TGeoTrd1 ***\n", GetName());
   printf("    dx1 = %11.5f\n", fDx1);
   printf("    dx2 = %11.5f\n", fDx2);
   printf("    dy  = %11.5f\n", fDy);
   printf("    dz  = %11.5f\n", fDz);
   printf(" Bounding box:\n");
   TGeoBBox::InspectShape();
}

//_____________________________________________________________________________
Double_t TGeoTrd1::Safety(Double_t *point, Bool_t in) const
{
// computes the closest distance from given point to this shape, according
// to option. The matching point on the shape is stored in spoint.
   Double_t saf[3];
   //--- Compute safety first
   // check Z facettes
   saf[0] = fDz-TMath::Abs(point[2]);
   Double_t fx = 0.5*(fDx1-fDx2)/fDz;
   Double_t calf = 1./TMath::Sqrt(1.0+fx*fx);
   // check X facettes
   Double_t distx = 0.5*(fDx1+fDx2)-fx*point[2];
   if (distx<0) saf[1]=TGeoShape::Big();
   else         saf[1]=(distx-TMath::Abs(point[0]))*calf;
   // check Y facettes
   saf[2] = fDy-TMath::Abs(point[1]);
   if (in) return saf[TMath::LocMin(3,saf)];
   for (Int_t i=0; i<3; i++) saf[i]=-saf[i];
   return saf[TMath::LocMax(3,saf)];
}

//_____________________________________________________________________________
void TGeoTrd1::SavePrimitive(ofstream &out, Option_t * /*option*/)
{
// Save a primitive as a C++ statement(s) on output stream "out".
   if (TObject::TestBit(kGeoSavePrimitive)) return;
   out << "   // Shape: " << GetName() << " type: " << ClassName() << endl;
   out << "   dx1 = " << fDx1 << ";" << endl;
   out << "   dx2 = " << fDx2 << ";" << endl;
   out << "   dy  = " << fDy  << ";" << endl;
   out << "   dz  = " << fDZ  << ";" << endl;
   out << "   pShape = new TGeoTrd1(\"" << GetName() << "\", dx1,dx2,dy,dz);" << endl;  
   TObject::SetBit(TGeoShape::kGeoSavePrimitive);
}         

//_____________________________________________________________________________
void TGeoTrd1::SetDimensions(Double_t *param)
{
// set trd1 params in one step :
   fDx1 = param[0];
   fDx2 = param[1];
   fDy  = param[2];
   fDz  = param[3];
   ComputeBBox();
}   

//_____________________________________________________________________________
void TGeoTrd1::SetVertex(Double_t *vertex) const
{
// set vertex of a corner according to visibility flags
   if (TestShapeBit(kGeoVisX)) {
      if (TestShapeBit(kGeoVisZ)) {
         vertex[0] = fDx2;
         vertex[2] = fDz;
         vertex[1] = (TestShapeBit(kGeoVisY))?fDy:-fDy;
      } else {   
         vertex[0] = fDx1;
         vertex[2] = -fDz;
         vertex[1] = (TestShapeBit(kGeoVisY))?fDy:-fDy;
      }
   } else {
      if (TestShapeBit(kGeoVisZ)) {
         vertex[0] = -fDx2;
         vertex[2] = fDz;
         vertex[1] = (TestShapeBit(kGeoVisY))?fDy:-fDy;
      } else {   
         vertex[0] = -fDx1;
         vertex[2] = -fDz;
         vertex[1] = (TestShapeBit(kGeoVisY))?fDy:-fDy;
      }
   }            
} 

//_____________________________________________________________________________
void TGeoTrd1::SetPoints(Double_t *points) const
{
// create arb8 mesh points
   if (!points) return;
   points[ 0] = -fDx1; points[ 1] = -fDy; points[ 2] = -fDz;
   points[ 3] = -fDx1; points[ 4] =  fDy; points[ 5] = -fDz;
   points[ 6] =  fDx1; points[ 7] =  fDy; points[ 8] = -fDz;
   points[ 9] =  fDx1; points[10] = -fDy; points[11] = -fDz;
   points[12] = -fDx2; points[13] = -fDy; points[14] =  fDz;
   points[15] = -fDx2; points[16] =  fDy; points[17] =  fDz;
   points[18] =  fDx2; points[19] =  fDy; points[20] =  fDz;
   points[21] =  fDx2; points[22] = -fDy; points[23] =  fDz;
}

//_____________________________________________________________________________
void TGeoTrd1::SetPoints(Float_t *points) const
{
// create arb8 mesh points
   if (!points) return;
   points[ 0] = -fDx1; points[ 1] = -fDy; points[ 2] = -fDz;
   points[ 3] = -fDx1; points[ 4] =  fDy; points[ 5] = -fDz;
   points[ 6] =  fDx1; points[ 7] =  fDy; points[ 8] = -fDz;
   points[ 9] =  fDx1; points[10] = -fDy; points[11] = -fDz;
   points[12] = -fDx2; points[13] = -fDy; points[14] =  fDz;
   points[15] = -fDx2; points[16] =  fDy; points[17] =  fDz;
   points[18] =  fDx2; points[19] =  fDy; points[20] =  fDz;
   points[21] =  fDx2; points[22] = -fDy; points[23] =  fDz;
}

//_____________________________________________________________________________
void TGeoTrd1::Sizeof3D() const
{
// fill size of this 3-D object
   TGeoBBox::Sizeof3D();
}
