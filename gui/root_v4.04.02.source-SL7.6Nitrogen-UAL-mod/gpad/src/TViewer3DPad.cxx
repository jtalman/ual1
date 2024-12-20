// @(#)root/base:$Name:  $:$Id: TViewer3DPad.cxx,v 1.9 2005/04/21 08:13:25 brun Exp $
// Author: Richard Maunder  10/3/2005

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include "TViewer3DPad.h"
#include "TVirtualPad.h"
#include "TView.h"
#include "TBuffer3D.h"
#include "TBuffer3DTypes.h"

#include <assert.h>

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TViewer3DPad                                                         //
//                                                                      //
// Provides 3D viewer interface (TVirtualViewer3D) support on a pad.    //
// Will be merged with TView / TView3D eventually.                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

ClassImp(TViewer3DPad)

//______________________________________________________________________________
Bool_t TViewer3DPad::PreferLocalFrame() const
{
   // Indicates if we prefer positions in local frame. Always false - pad
   // drawing is always done in master frame.
   return kFALSE;
}


//______________________________________________________________________________
void TViewer3DPad::BeginScene()
{
   // Open a scene on the viewer
   assert(!fBuilding);

   // Create a 3D view if none exists
   TView *view = fPad.GetView();
   if (!view) {
      view = new TView(1); // Cartesian view by default
      if (!view) {
         assert(kFALSE);
         return;
      }
      fPad.SetView(view);

      // Set view to perform first auto-range (scaling) pass 
      view->SetAutoRange(kTRUE);
   }

   fBuilding = kTRUE;
}

//______________________________________________________________________________
void TViewer3DPad::EndScene()
{
   // Close the scene on the viewer
   assert(fBuilding);

   // If we are doing for auto-range pass on view invoke another pass
   TView *view = fPad.GetView();
   if (view) {
      if (view->GetAutoRange()) {
         view->SetAutoRange(kFALSE);
         fPad.Paint();
      }
   }   
   
   fBuilding = kFALSE;
}

//______________________________________________________________________________
Int_t TViewer3DPad::AddObject(const TBuffer3D & buffer, Bool_t * addChildren)
{
   // Add an 3D object described by the buffer to the viewer. Returns flags
   // to indicate:
   // i) if extra sections of the buffer need completing. 
   // ii) if child objects of the buffer object should be added (always true)

   // Accept any children
   if (addChildren) {
      *addChildren = kTRUE;
   }

   TView * view = fPad.GetView();
   if (!view) {
      assert(kFALSE);
      return TBuffer3D::kNone;
   }

   UInt_t reqSections = TBuffer3D::kCore|TBuffer3D::kRawSizes|TBuffer3D::kRaw;
   if (!buffer.SectionsValid(reqSections)) {
      return reqSections;
   }

   UInt_t i;
   Int_t i0, i1, i2;

   // Range pass
   if (view->GetAutoRange()) {
      Double_t x0, y0, z0, x1, y1, z1;

      x0 = x1 = buffer.fPnts[0];
      y0 = y1 = buffer.fPnts[1];
      z0 = z1 = buffer.fPnts[2];
      for (i=1; i<buffer.NbPnts(); i++) {
         i0 = 3*i; i1 = i0+1; i2 = i0+2;
         x0 = buffer.fPnts[i0] < x0 ? buffer.fPnts[i0] : x0;
         y0 = buffer.fPnts[i1] < y0 ? buffer.fPnts[i1] : y0;
         z0 = buffer.fPnts[i2] < z0 ? buffer.fPnts[i2] : z0;
         x1 = buffer.fPnts[i0] > x1 ? buffer.fPnts[i0] : x1;
         y1 = buffer.fPnts[i1] > y1 ? buffer.fPnts[i1] : y1;
         z1 = buffer.fPnts[i2] > z1 ? buffer.fPnts[i2] : z1;
      }
      view->SetRange(x0,y0,z0,x1,y1,z1,2);
   }
   // Actual drawing pass
   else {
      // Do not show semi transparent objects
      if (buffer.fTransparency > 50) {
         return TBuffer3D::kNone;
      }
      if (buffer.Type()== TBuffer3DTypes::kMarker ) {
         Double_t pndc[3], temp[3];
         for (i=0; i<buffer.NbPnts(); i++) {
            for ( i0=0; i0<3; i0++ ) temp[i0] = buffer.fPnts[3*i+i0];
            view->WCtoNDC(temp, pndc);
            fPad.PaintPolyMarker(1, &pndc[0], &pndc[1]);
         }
      } else {
         for (i=0; i<buffer.NbSegs(); i++) {
            i0 = 3*buffer.fSegs[3*i+1];
            Double_t *ptpoints_0 = &(buffer.fPnts[i0]);
            i0 = 3*buffer.fSegs[3*i+2];
            Double_t *ptpoints_3 = &(buffer.fPnts[i0]);
            fPad.PaintLine3D(ptpoints_0, ptpoints_3);
         }
      }
   }

   return TBuffer3D::kNone;
}

//______________________________________________________________________________
Int_t TViewer3DPad::AddObject(UInt_t /*placedID*/, const TBuffer3D & buffer, Bool_t * addChildren)
{
   // We don't support placed ID shapes - ID is discarded
   return AddObject(buffer,addChildren);
}


//______________________________________________________________________________
   // Composite shapes not supported on this viewer currently - ignore.
   // Will result in a set of individual component shapes

void TViewer3DPad::OpenComposite(const TBuffer3D & /*buffer*/, Bool_t * /*addChildren*/) 
{};

//______________________________________________________________________________
void TViewer3DPad::CloseComposite() 
{};

//______________________________________________________________________________
void TViewer3DPad::AddCompositeOp(UInt_t /*operation*/) 
{};
