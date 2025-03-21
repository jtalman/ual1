// @(#)root/g3d:$Name:  $:$Id: TPolyLine3D.cxx,v 1.22 2005/03/21 17:22:59 brun Exp $
// Author: Nenad Buncic   17/08/95

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include "Riostream.h"
#include "TROOT.h"
#include "TPolyLine3D.h"
#include "TVirtualPad.h"
#include "TView.h"
#include "TVirtualViewer3D.h"
#include "TBuffer3D.h"
#include "TBuffer3DTypes.h"
#include "TGeometry.h"

#include <assert.h>

ClassImp(TPolyLine3D)

//______________________________________________________________________________
// PolyLine3D is a 3-dimensional polyline. It has 4 different constructors.
//
//   First one, without any parameters TPolyLine3D(), we call 'default
// constructor' and it's used in a case that just an initialisation is
// needed (i.e. pointer declaration).
//
//       Example:
//                 TPolyLine3D *pl1 = new TPolyLine3D;
//
//
//   Second one is 'normal constructor' with, usually, one parameter
// n (number of points), and it just allocates a space for the points.
//
//       Example:
//                 TPolyLine3D pl1(150);
//
//
//   Third one allocates a space for the points, and also makes
// initialisation from the given array.
//
//       Example:
//                 TPolyLine3D pl1(150, pointerToAnArray);
//
//
//   Fourth one is, almost, similar to the constructor above, except
// initialisation is provided with three independent arrays (array of
// x coordinates, y coordinates and z coordinates).
//
//       Example:
//                 TPolyLine3D pl1(150, xArray, yArray, zArray);
//
// Example:
//   void pl3() {
//      TCanvas *c1 = new TCanvas("c1");
//      TView *view = new TView(1);
//      view->SetRange(0,0,0,2,2,2);
//      const Int_t n = 100;
//      TPolyLine3D *l = new TPolyLine3D(n);
//      for (Int_t i=0;i<n;i++) {
//         Double_t x = 2*gRandom->Rndm();
//         Double_t y = 2*gRandom->Rndm();
//         Double_t z = 2*gRandom->Rndm();
//         l->SetPoint(i,x,y,z);
//      }
//      l->Draw();
//   }


//______________________________________________________________________________
TPolyLine3D::TPolyLine3D()
{
   // 3-D polyline default constructor.

   fN = 0;
   fP = 0;
   fLastPoint = -1;
}

//______________________________________________________________________________
TPolyLine3D::TPolyLine3D(Int_t n, Option_t *option)
{
   // 3-D polyline normal constructor with initialization to 0.
   // If n < 0 the default size (2 points) is set.

   fOption = option;
   SetBit(kCanDelete);
   fLastPoint = -1;
   if (n <= 0) {
      fN = 0;
      fP = 0;
      return;
   }

   fN = n;
   fP = new Float_t[3*fN];
   for (Int_t i=0; i<3*fN; i++) fP[i] = 0;
}

//______________________________________________________________________________
TPolyLine3D::TPolyLine3D(Int_t n, Float_t *p, Option_t *option)
{
   // 3-D polyline normal constructor. Polyline is intialized with p.
   // If n < 0 the default size (2 points) is set.

   fOption = option;
   SetBit(kCanDelete);
   fLastPoint = -1;
   if (n <= 0) {
      fN = 0;
      fP = 0;
      return;
   }

   fN = n;
   fP = new Float_t[3*fN];
   if (n > 0) {
      for (Int_t i=0; i<3*n; i++) {
         fP[i] = p[i];
      }
      fLastPoint = fN-1;
   } else {
      for (Int_t i=0; i<3*fN; i++) {
         fP[i] = 0;
      }
      fLastPoint = -1;
   }
}

//______________________________________________________________________________
TPolyLine3D::TPolyLine3D(Int_t n, Double_t *p, Option_t *option)
{
   // 3-D polyline normal constructor. Polyline is initialized with p
   // (cast to float). If n < 0 the default size (2 points) is set.

   fOption = option;
   SetBit(kCanDelete);
   fLastPoint = -1;
   if (n <= 0) {
      fN = 0;
      fP = 0;
      return;
   }

   fN = n;
   fP = new Float_t[3*fN];
   if (n > 0) {
      for (Int_t i=0; i<3*n; i++) {
         fP[i] = (Float_t) p[i];
      }
      fLastPoint = fN-1;
   } else {
      for (Int_t i=0; i<3*fN; i++) {
         fP[i] = 0;
      }
      fLastPoint = -1;
   }
}

//______________________________________________________________________________
TPolyLine3D::TPolyLine3D(Int_t n, Float_t *x, Float_t *y, Float_t *z, Option_t *option)
{
   // 3-D polyline normal constructor. Polyline is initialized withe the
   // x, y ,z arrays. If n < 0 the default size (2 points) is set.

   fOption = option;
   SetBit(kCanDelete);
   fLastPoint = -1;
   if (n <= 0) {
      fN = 0;
      fP = 0;
      return;
   }

   fN = n;
   fP = new Float_t[3*fN];
   Int_t j = 0;
   if (n > 0) {
      for (Int_t i=0; i<n;i++) {
         fP[j]   = x[i];
         fP[j+1] = y[i];
         fP[j+2] = z[i];
         j += 3;
      }
      fLastPoint = fN-1;
   } else {
      for (Int_t i=0; i<3*fN; i++) {
         fP[i] = 0;
      }
   }
}

//______________________________________________________________________________
TPolyLine3D::TPolyLine3D(Int_t n, Double_t *x, Double_t *y, Double_t *z, Option_t *option)
{
   // 3-D polyline normal constructor. Polyline is initialized withe the
   // x, y, z arrays (which are cast to float).
   // If n < 0 the default size (2 points) is set.

   fOption = option;
   SetBit(kCanDelete);
   fLastPoint = -1;
   if (n <= 0) {
      fN = 0;
      fP = 0;
      return;
   }

   fN = n;
   fP = new Float_t[3*fN];
   Int_t j = 0;
   if (n > 0) {
      for (Int_t i=0; i<n;i++) {
         fP[j]   = (Float_t) x[i];
         fP[j+1] = (Float_t) y[i];
         fP[j+2] = (Float_t) z[i];
         j += 3;
      }
      fLastPoint = fN-1;
   } else {
      for (Int_t i=0; i<3*fN; i++) {
         fP[i] = 0;
      }
   }
}

//______________________________________________________________________________
TPolyLine3D::~TPolyLine3D()
{
   // 3-D polyline destructor.

   if (fP) delete [] fP;
}

//______________________________________________________________________________
TPolyLine3D::TPolyLine3D(const TPolyLine3D &polyline) : TObject(polyline), TAttLine(polyline), TAtt3D(polyline)
{
   // 3-D polyline copy ctor.

   fP = 0;
   ((TPolyLine3D&)polyline).TPolyLine3D::Copy(*this);
}

//______________________________________________________________________________
void TPolyLine3D::Copy(TObject &obj) const
{
   // Copy polyline to polyline obj.

   TObject::Copy(obj);
   TAttLine::Copy(((TPolyLine3D&)obj));
   ((TPolyLine3D&)obj).fN = fN;
   if (((TPolyLine3D&)obj).fP)
      delete [] ((TPolyLine3D&)obj).fP;
   if (fN > 0) {
      ((TPolyLine3D&)obj).fP = new Float_t[3*fN];
      for (Int_t i=0; i<3*fN;i++)  {((TPolyLine3D&)obj).fP[i] = fP[i];}
   } else {
      ((TPolyLine3D&)obj).fP = 0;
   }
   ((TPolyLine3D&)obj).fOption = fOption;
   ((TPolyLine3D&)obj).fLastPoint = fLastPoint;
}

//______________________________________________________________________________
Int_t TPolyLine3D::DistancetoPrimitive(Int_t px, Int_t py)
{
   // Compute distance from point px,py to a 3-D polyline.
   // Compute the closest distance of approach from point px,py to each segment
   // of the polyline.
   // Returns when the distance found is below DistanceMaximum.
   // The distance is computed in pixels units.

   const Int_t inaxis = 7;
   Int_t dist = 9999;

   Int_t puxmin = gPad->XtoAbsPixel(gPad->GetUxmin());
   Int_t puymin = gPad->YtoAbsPixel(gPad->GetUymin());
   Int_t puxmax = gPad->XtoAbsPixel(gPad->GetUxmax());
   Int_t puymax = gPad->YtoAbsPixel(gPad->GetUymax());

   // return if point is not in the user area
   if (px < puxmin - inaxis) return dist;
   if (py > puymin + inaxis) return dist;
   if (px > puxmax + inaxis) return dist;
   if (py < puymax - inaxis) return dist;

   TView *view = gPad->GetView();
   if (!view) return dist;

   Int_t i, dsegment;
   Double_t x1,y1,x2,y2;
   Float_t xndc[3];
   for (i=0;i<Size()-1;i++) {
      view->WCtoNDC(&fP[3*i], xndc);
      x1 = xndc[0];
      y1 = xndc[1];
      view->WCtoNDC(&fP[3*i+3], xndc);
      x2 = xndc[0];
      y2 = xndc[1];
      dsegment = DistancetoLine(px,py,x1,y1,x2,y2);
      if (dsegment < dist) dist = dsegment;
   }
   return dist;
}

//______________________________________________________________________________
void TPolyLine3D::Draw(Option_t *option)
{
   // Draw this 3-D polyline with its current attributes.

   AppendPad(option);
}

//______________________________________________________________________________
void TPolyLine3D::DrawOutlineCube(TList *outline, Double_t *rmin, Double_t *rmax)
{
   // Draw cube outline with 3d polylines.
   //
   //      x = fRmin[0]        X = fRmax[0]
   //      y = fRmin[1]        Y = fRmax[1]
   //      z = fRmin[2]        Z = fRmax[2]
   //
   //
   //            (x,Y,Z) +---------+ (X,Y,Z)
   //                   /         /|
   //                  /         / |
   //                 /         /  |
   //        (x,y,Z) +---------+   |
   //                |         |   + (X,Y,z)
   //                |         |  /
   //                |         | /
   //                |         |/
   //                +---------+
   //             (x,y,z)   (X,y,z)
   //

   Double_t x = rmin[0];     Double_t X = rmax[0];
   Double_t y = rmin[1];     Double_t Y = rmax[1];
   Double_t z = rmin[2];     Double_t Z = rmax[2];

   TPolyLine3D *pl3d = (TPolyLine3D *)outline->First();
   if (!pl3d) {
      TView *view = gPad->GetView();
      TPolyLine3D *p1 = new TPolyLine3D(4);
      TPolyLine3D *p2 = new TPolyLine3D(4);
      TPolyLine3D *p3 = new TPolyLine3D(4);
      TPolyLine3D *p4 = new TPolyLine3D(4);
      p1->SetLineColor(view->GetLineColor());
      p1->SetLineStyle(view->GetLineStyle());
      p1->SetLineWidth(view->GetLineWidth());
      p1->Copy(*p2);
      p1->Copy(*p3);
      p1->Copy(*p4);
      outline->Add(p1);
      outline->Add(p2);
      outline->Add(p3);
      outline->Add(p4);
   }

   pl3d = (TPolyLine3D *)outline->First();

   pl3d->SetPoint(0, x, y, z);
   pl3d->SetPoint(1, X, y, z);
   pl3d->SetPoint(2, X, Y, z);
   pl3d->SetPoint(3, x, Y, z);

   pl3d = (TPolyLine3D *)outline->After(pl3d);

   pl3d->SetPoint(0, X, y, z);
   pl3d->SetPoint(1, X, y, Z);
   pl3d->SetPoint(2, X, Y, Z);
   pl3d->SetPoint(3, X, Y, z);

   pl3d = (TPolyLine3D *)outline->After(pl3d);

   pl3d->SetPoint(0, X, y, Z);
   pl3d->SetPoint(1, x, y, Z);
   pl3d->SetPoint(2, x, Y, Z);
   pl3d->SetPoint(3, X, Y, Z);

   pl3d = (TPolyLine3D *)outline->After(pl3d);

   pl3d->SetPoint(0, x, y, Z);
   pl3d->SetPoint(1, x, y, z);
   pl3d->SetPoint(2, x, Y, z);
   pl3d->SetPoint(3, x, Y, Z);
}

//______________________________________________________________________________
void TPolyLine3D::DrawPolyLine(Int_t n, Float_t *p, Option_t *option)
{
   // Draw 3-D polyline with new coordinates. Creates a new polyline which
   // will be adopted by the pad in which it is drawn. Does not change the
   // original polyline (should be static method).

   TPolyLine3D *newpolyline = new TPolyLine3D();
   Int_t size = 3*Size();
   newpolyline->fN =n;
   newpolyline->fP = new Float_t[size];
   for (Int_t i=0; i<size;i++) { newpolyline->fP[i] = p[i];}
   TAttLine::Copy(*newpolyline);
   newpolyline->fOption = fOption;
   newpolyline->fLastPoint = fLastPoint;
   newpolyline->SetBit(kCanDelete);
   newpolyline->AppendPad(option);
}

//______________________________________________________________________________
void TPolyLine3D::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
   // Execute action corresponding to one event.

  if (gPad->GetView())
     gPad->GetView()->ExecuteRotateView(event, px, py);
}

//______________________________________________________________________________
void TPolyLine3D::ls(Option_t *option) const
{
   // List this 3-D polyline.

   TROOT::IndentLevel();
   cout <<"PolyLine3D  N=" <<fN<<" Option="<<option<<endl;
}

//______________________________________________________________________________
Int_t TPolyLine3D::Merge(TCollection *list)
{
// Merge polylines in the collection in this polyline

   if (!list) return 0;
   TIter next(list);

   //first loop to count the number of entries
   TPolyLine3D *pl;
   Int_t npoints = 0;
   while ((pl = (TPolyLine3D*)next())) {
      if (!pl->InheritsFrom(TPolyLine3D::Class())) {
         Error("Add","Attempt to add object of class: %s to a %s",pl->ClassName(),this->ClassName());
         return -1;
      }
      npoints += pl->Size();
   }

   //extend this polyline to hold npoints
   pl->SetPoint(npoints-1,0,0,0);

   //merge all polylines
   next.Reset();
   while ((pl = (TPolyLine3D*)next())) {
      Int_t np = pl->Size();
      Float_t *p = pl->GetP();
      for (Int_t i=0;i<np;i++) {
         SetPoint(i,p[3*i],p[3*i+1],p[3*i+2]);
      }
   }

   return npoints;
}

//______________________________________________________________________________
void TPolyLine3D::Paint(Option_t * /* option */ )
{
   // Paint a TPolyLine3D. 

   // No need to continue if there is nothing to paint
   if (Size() <= 0) return;

   static TBuffer3D buffer(TBuffer3DTypes::kLine);
   
   // TPolyLine3D can only be described by filling the TBuffer3D 'tesselation'
   // parts - so there are no 'optional' sections - we just fill everything.
   
   buffer.ClearSectionsValid();

   // Section kCore
   buffer.fID           = this;
   buffer.fColor        = GetLineColor();   
   buffer.fTransparency = 0;    
   buffer.fLocalFrame   = kFALSE; 
   buffer.SetSectionsValid(TBuffer3D::kCore);
   
   // We fill kCore and kRawSizes on first pass and try with viewer
   Int_t reqSections = gPad->GetViewer3D()->AddObject(buffer);
   if (reqSections == TBuffer3D::kNone) {
      return;
   }
   
   if (reqSections & TBuffer3D::kRawSizes) {
      Int_t NbPnts = Size();
      Int_t NbSegs = NbPnts-1;
      if (!buffer.SetRawSizes(NbPnts, 3*NbPnts, NbSegs, 3*NbSegs, 0, 0)) {
         return;
      }
      buffer.SetSectionsValid(TBuffer3D::kRawSizes);
   }

   if ((reqSections & TBuffer3D::kRaw) && buffer.SectionsValid(TBuffer3D::kRawSizes)) {
      // Points
      for (UInt_t i=0; i<3*buffer.NbPnts(); i++) {
         buffer.fPnts[i] = (Double_t)fP[i];
      }

      // Transform points
      if (gGeometry && !buffer.fLocalFrame) {   
         Double_t dlocal[3];
         Double_t dmaster[3];
         for (UInt_t j=0; j<buffer.NbPnts(); j++) {
            dlocal[0] = buffer.fPnts[3*j];
            dlocal[1] = buffer.fPnts[3*j+1];
            dlocal[2] = buffer.fPnts[3*j+2];
            gGeometry->Local2Master(&dlocal[0],&dmaster[0]);
            buffer.fPnts[3*j]   = dmaster[0];
            buffer.fPnts[3*j+1] = dmaster[1];
            buffer.fPnts[3*j+2] = dmaster[2];
         }
      }

      // Basic colors: 0, 1, ... 8
      Int_t c = (((GetLineColor()) %8) -1) * 4;
      if (c < 0) c = 0;

      // Segments
      for (UInt_t i = 0; i < buffer.NbSegs(); i++) {
          buffer.fSegs[3*i  ] = c;
          buffer.fSegs[3*i+1] = i;
          buffer.fSegs[3*i+2] = i+1;
      }

      TAttLine::Modify();
      
      buffer.SetSectionsValid(TBuffer3D::kRaw);
   }
   
   gPad->GetViewer3D()->AddObject(buffer);
}

//______________________________________________________________________________
void TPolyLine3D::Print(Option_t *option) const
{
   // Dump this 3-D polyline with its attributes on stdout.

   printf("    TPolyLine3D N=%d, Option=%s\n",fN,option);
   TString opt = option;
   opt.ToLower();
   if (opt.Contains("all")) {
      for (Int_t i=0;i<Size();i++) {
         printf(" x[%d]=%g, y[%d]=%g, z[%d]=%g\n",i,fP[3*i],i,fP[3*i+1],i,fP[3*i+2]);
      }
   }
}

//______________________________________________________________________________
void TPolyLine3D::SavePrimitive(ofstream &out, Option_t *)
{
   // Save primitive as a C++ statement(s) on output stream.

   char quote = '"';
   out<<"   "<<endl;
   if (gROOT->ClassSaved(TPolyLine3D::Class())) {
      out<<"   ";
   } else {
      out<<"   TPolyLine3D *";
   }
   Int_t size=Size();
   out<<"pline3D = new TPolyLine3D("<<fN<<","<<quote<<fOption<<quote<<");"<<endl;

   SaveLineAttributes(out,"pline3D",1,1,1);

   if (size > 0) {
      for (Int_t i=0;i<size;i++)
         out<<"   pline3D->SetPoint("<<i<<","<<fP[3*i]<<","<<fP[3*i+1]<<","<<fP[3*i+2]<<");"<<endl;
   }
   out<<"   pline3D->Draw();"<<endl;
}

//______________________________________________________________________________
Int_t TPolyLine3D::SetNextPoint(Double_t x, Double_t y, Double_t z)
{
   // Set point following LastPoint to x, y, z.
   // Returns index of the point (new last point).

   fLastPoint++;
   SetPoint(fLastPoint, x, y, z);
   return fLastPoint;
}

//______________________________________________________________________________
void TPolyLine3D::SetPoint(Int_t n, Double_t x, Double_t y, Double_t z)
{
   // Set point n to x, y, z.
   // If n is more then the current TPolyLine3D size (n > fN) then
   // the polyline will be resized to contain at least n points.

   if (n < 0) return;
   if (!fP || n >= fN) {
      // re-allocate the object
      Int_t newN = TMath::Max(2*fN,n+1);
      Float_t *savepoint = new Float_t [3*newN];
      if (fP && fN){
         memcpy(savepoint,fP,3*fN*sizeof(Float_t));
         memset(&savepoint[3*fN],0,(newN-fN)*sizeof(Float_t));
         delete [] fP;
      }
      fP = savepoint;
      fN = newN;
   }
   fP[3*n  ] = x;
   fP[3*n+1] = y;
   fP[3*n+2] = z;
   fLastPoint = TMath::Max(fLastPoint,n);
}

//______________________________________________________________________________
void TPolyLine3D::SetPolyLine(Int_t n, Option_t *option)
{
   // Re-initialize polyline with n points (0,0,0).
   // if n <= 0 the current array of points is deleted.

   fOption = option;
   if (n <= 0) {
      fN = 0;
      fLastPoint = -1;
      delete [] fP;
      fP = 0;
      return;
   }
   fN = n;
   if (fP) delete [] fP;
   fP = new Float_t[3*fN];
   memset(fP,0,3*fN*sizeof(Float_t));
   fLastPoint = fN-1;
}

//______________________________________________________________________________
void TPolyLine3D::SetPolyLine(Int_t n, Float_t *p, Option_t *option)
{
   // Re-initialize polyline with n points from p. If p=0 initialize with 0.
   // if n <= 0 the current array of points is deleted.

   fOption = option;
   if (n <= 0) {
      fN = 0;
      fLastPoint = -1;
      delete [] fP;
      fP = 0;
      return;
   }
   fN = n;
   if (fP) delete [] fP;
   fP = new Float_t[3*fN];
   if (p) {
      for (Int_t i=0; i<fN;i++) {
         fP[3*i]   = p[3*i];
         fP[3*i+1] = p[3*i+1];
         fP[3*i+2] = p[3*i+2];
      }
   } else {
      memset(fP,0,3*fN*sizeof(Float_t));
   }
   fLastPoint = fN-1;
}

//______________________________________________________________________________
void TPolyLine3D::SetPolyLine(Int_t n, Double_t *p, Option_t *option)
{
   // Re-initialize polyline with n points from p. If p=0 initialize with 0.
   // if n <= 0 the current array of points is deleted.

   fOption = option;
   if (n <= 0) {
      fN = 0;
      fLastPoint = -1;
      delete [] fP;
      fP = 0;
      return;
   }
   fN = n;
   if (fP) delete [] fP;
   fP = new Float_t[3*fN];
   if (p) {
      for (Int_t i=0; i<fN;i++) {
         fP[3*i]   = (Float_t) p[3*i];
         fP[3*i+1] = (Float_t) p[3*i+1];
         fP[3*i+2] = (Float_t) p[3*i+2];
      }
   } else {
      memset(fP,0,3*fN*sizeof(Float_t));
   }
   fLastPoint = fN-1;
}

//_______________________________________________________________________
void TPolyLine3D::Streamer(TBuffer &b)
{
   // Stream a 3-D polyline object.

   UInt_t R__s, R__c;
   if (b.IsReading()) {
      b.ReadVersion(&R__s, &R__c);
      TObject::Streamer(b);
      TAttLine::Streamer(b);
      b >> fN;
      if (fN) {
         fP = new Float_t[3*fN];
         b.ReadFastArray(fP,3*fN);
      }
      fOption.Streamer(b);
      fLastPoint = fN-1;
      b.CheckByteCount(R__s, R__c, TPolyLine3D::IsA());
   } else {
      R__c = b.WriteVersion(TPolyLine3D::IsA(), kTRUE);
      TObject::Streamer(b);
      TAttLine::Streamer(b);
      Int_t size = Size();
      b << size;
      if (size) b.WriteFastArray(fP, 3*size);
      fOption.Streamer(b);
      b.SetByteCount(R__c, kTRUE);
   }
}
