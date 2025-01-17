// @(#)root/x3d:$Name:  $:$Id: TViewerX3D.cxx,v 1.14 2005/03/16 17:18:13 brun Exp $
// Author: Rene Brun   05/09/99
/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TViewerX3D                                                           //
//                                                                      //
// C++ interface to the X3D viewer                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TViewerX3D.h"
#include "X3DBuffer.h"
#include "TVirtualPad.h"
#include "TView.h"
#include "TMath.h"
#include "TROOT.h"
#include "TClass.h"

#include "TRootHelpDialog.h"
#include "TGClient.h"
#include "TGCanvas.h"
#include "TGMenu.h"
#include "TGWidget.h"
#include "TGMsgBox.h"
#include "TVirtualX.h"

#include "TBuffer3D.h"
#include "TBuffer3DTypes.h"

#include "HelpText.h"

#include <assert.h>

const char gHelpX3DViewer[] = "\
     PRESS \n\
     \tw\t--- wireframe mode\n\
     \te\t--- hidden line mode\n\
     \tr\t--- hidden surface mode\n\
     \tu\t--- move object down\n\
     \ti\t--- move object up\n\
     \to\t--- toggle controls style\n\
     \ts\t--- toggle stereo display\n\
     \td\t--- toggle blue stereo view\n\
     \tf\t--- toggle double buffer\n\
     \th\t--- move object right\n\
     \tj\t--- move object forward\n\
     \tk\t--- move object backward\n\
     \tl\t--- move object left\n\
     \tx a\t--- rotate about x\n\
     \ty b\t--- rotate about y\n\
     \tz c\t--- rotate about z\n\
     \t1 2 3\t--- autorotate about x\n\
     \t4 5 6\t--- autorotate about y\n\
     \t7 8 9\t--- autorotate about z\n\
     \t[ ] { }\t--- adjust focus\n\n\
     HOLD the left mouse button and MOVE mouse to ROTATE object\n\n\
";


extern "C" {
  Window_t x3d_main(Float_t *longitude, Float_t *latitude, Float_t *psi,
                    Option_t *option, Window_t parent);
  void     x3d_set_display(Display_t display);
  int      x3d_dispatch_event(Handle_t event);
  void     x3d_update();
  void     x3d_get_position(Float_t *longitude, Float_t *latitude, Float_t *psi);
  int      x3d_exec_command(int px, int py, char command);
  void     x3d_terminate();
}


// Canvas menu command ids
enum EX3DViewerCommands {
   kFileNewViewer,
   kFileSave,
   kFileSaveAs,
   kFilePrint,
   kFileCloseViewer,

   kHelpAbout,
   kHelpOnViewer
};

Bool_t TViewerX3D::fgCreated = kFALSE;


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TX3DContainer                                                        //
//                                                                      //
// Utility class used by TViewerX3D. The TX3DContainer is the frame     //
// embedded in the TGCanvas widget. The X3D graphics goes into this     //
// frame. This class is used to enable input events on this graphics    //
// frame and forward the events to X3D.                                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class TX3DContainer : public TGCompositeFrame {
private:
   TViewerX3D  *fViewer;    // pointer back to viewer imp
public:
   TX3DContainer(TViewerX3D *c, Window_t id, const TGWindow *parent);

   Bool_t  HandleButton(Event_t *ev)
                { x3d_dispatch_event(gVirtualX->GetNativeEvent());
                  return fViewer->HandleContainerButton(ev); }
   Bool_t  HandleConfigureNotify(Event_t *ev)
                { TGFrame::HandleConfigureNotify(ev);
                  return x3d_dispatch_event(gVirtualX->GetNativeEvent()); }
   Bool_t  HandleKey(Event_t *)
                { return x3d_dispatch_event(gVirtualX->GetNativeEvent()); }
   Bool_t  HandleMotion(Event_t *)
                { return x3d_dispatch_event(gVirtualX->GetNativeEvent()); }
   Bool_t  HandleExpose(Event_t *)
                { return x3d_dispatch_event(gVirtualX->GetNativeEvent()); }
   Bool_t  HandleColormapChange(Event_t *)
                { return x3d_dispatch_event(gVirtualX->GetNativeEvent()); }
};


//______________________________________________________________________________
TX3DContainer::TX3DContainer(TViewerX3D *c, Window_t id, const TGWindow *p)
   : TGCompositeFrame(gClient, id, p)
{
   // Create a canvas container.

   fViewer = c;
}


ClassImp(TViewerX3D)


//______________________________________________________________________________
TViewerX3D::TViewerX3D(TVirtualPad *pad)
   : TVirtualViewer3D(),
     TGMainFrame(gClient->GetRoot(), 800, 600),
     fCanvas(0), fContainer(0), fMenuBar(0), fFileMenu(0),
     fHelpMenu(0), fMenuBarLayout(0), fMenuBarItemLayout(0), 
     fMenuBarHelpLayout(0), fCanvasLayout(0),
     fPad(pad), fBuildingScene(kFALSE), fPass(kSize)
{
   // Create ROOT X3D viewer.

   fOption = "x3d";
   fX3DWin = 0;
   fWidth  = 800;
   fHeight = 600;
   fXPos   = 0;
   fYPos   = 0;
   fTitle  = "x3d";
}


//______________________________________________________________________________
TViewerX3D::TViewerX3D(TVirtualPad *pad, Option_t *option, const char *title,
                       UInt_t width, UInt_t height)
   : TVirtualViewer3D(),
     TGMainFrame(gClient->GetRoot(), width, height),
     fCanvas(0), fContainer(0), fMenuBar(0), fFileMenu(0),
     fHelpMenu(0), fMenuBarLayout(0), fMenuBarItemLayout(0), 
     fMenuBarHelpLayout(0), fCanvasLayout(0),
     fPad(pad), fBuildingScene(kFALSE), fPass(kSize)
{
   // Create ROOT X3D viewer.

   fOption = option;
   fX3DWin = 0;
   fWidth  = width;
   fHeight = height;
   fXPos   = 0;
   fYPos   = 0;
   fTitle  = title;
}


//______________________________________________________________________________
TViewerX3D::TViewerX3D(TVirtualPad *pad, Option_t *option, const char *title,
                       Int_t x, Int_t y, UInt_t width, UInt_t height)
   : TVirtualViewer3D(),
     TGMainFrame(gClient->GetRoot(), width, height),
     fCanvas(0), fContainer(0), fMenuBar(0), fFileMenu(0),
     fHelpMenu(0), fMenuBarLayout(0), fMenuBarItemLayout(0), 
     fMenuBarHelpLayout(0), fCanvasLayout(0),
     fPad(pad), fBuildingScene(kFALSE), fPass(kSize)
{
   // Create ROOT X3D viewer.

   fOption = option;
   fX3DWin = 0;
   fWidth  = width;
   fHeight = height;
   fXPos   = x;
   fYPos   = y;
   fTitle  = title;
}


//______________________________________________________________________________
TViewerX3D::~TViewerX3D()
{
   // Delete ROOT X3D viewer.

   if (!fPad) return;

   if (fgCreated) { 
      DeleteX3DWindow();
   }
   delete fCanvasLayout;
   delete fMenuBarHelpLayout;
   delete fMenuBarItemLayout;
   delete fMenuBarLayout;
   delete fHelpMenu;
   delete fFileMenu;
   delete fMenuBar;   
   delete fContainer;
   delete fCanvas;
   fgCreated = kFALSE;
}


//______________________________________________________________________________
void TViewerX3D::CloseWindow()
{
   assert(!fBuildingScene);
   fPad->ReleaseViewer3D();      
   delete this;
}


//______________________________________________________________________________
void TViewerX3D::CreateViewer(const char *name)
{
   // Create the actual canvas.

   // Create menus
   fFileMenu = new TGPopupMenu(fClient->GetRoot());
   fFileMenu->AddEntry("&New Viewer",         kFileNewViewer);
   fFileMenu->AddSeparator();
   fFileMenu->AddEntry("Save",                kFileSave);
   fFileMenu->AddEntry("Save As...",          kFileSaveAs);
   fFileMenu->AddSeparator();
   fFileMenu->AddEntry("&Print...",           kFilePrint);
   fFileMenu->AddSeparator();
   fFileMenu->AddEntry("&Close Viewer",       kFileCloseViewer);

   //fFileMenu->DefaultEntry(kFileNewViewer);
   fFileMenu->DisableEntry(kFileNewViewer);
   fFileMenu->DisableEntry(kFileSave);
   fFileMenu->DisableEntry(kFileSaveAs);
   fFileMenu->DisableEntry(kFilePrint);

   fHelpMenu = new TGPopupMenu(fClient->GetRoot());
   fHelpMenu->AddEntry("&About ROOT...",           kHelpAbout);
   fHelpMenu->AddSeparator();
   fHelpMenu->AddEntry("Help On X3D Viewer...", kHelpOnViewer);

   // This main frame will process the menu commands
   fFileMenu->Associate(this);
   fHelpMenu->Associate(this);

   // Create menubar layout hints
   fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0, 0, 1, 1);
   fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
   fMenuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);

   // Create menubar
   fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
   fMenuBar->AddPopup("&File",    fFileMenu,    fMenuBarItemLayout);
   fMenuBar->AddPopup("&Help",    fHelpMenu,    fMenuBarHelpLayout);

   AddFrame(fMenuBar, fMenuBarLayout);

   // Create canvas and canvas container that will host the ROOT graphics
   fCanvas = new TGCanvas(this, GetWidth()+4, GetHeight()+4,
                          kSunkenFrame | kDoubleBorder);
   InitX3DWindow();
   if (!fX3DWin) {
      fContainer    = 0;
      fCanvasLayout = 0;
      return;
   }
   fContainer = new TX3DContainer(this, fX3DWin, fCanvas->GetViewPort());
   fCanvas->SetContainer(fContainer);
   fCanvasLayout = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY);
   AddFrame(fCanvas, fCanvasLayout);

   // Misc

   SetWindowName(name);
   SetIconName(name);
   SetClassHints("X3DViewer", "X3DViewer");

   SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputModeless);

   MapSubwindows();

   // we need to use GetDefaultSize() to initialize the layout algorithm...
   Resize(GetDefaultSize());

   MoveResize(fXPos, fYPos, fWidth, fHeight);
   SetWMPosition(fXPos, fYPos);
   fgCreated = kTRUE;
}


//______________________________________________________________________________
void TViewerX3D::InitX3DWindow()
{
   // Setup geometry and initialize X3D.

   TView *view = fPad->GetView();
   if (!view) {
      Error("InitX3DWindow", "view is not set");
      return;
   }

   const Float_t kPI = Float_t (TMath::Pi());

   Float_t longitude_rad = ( 90 + view->GetLongitude()) * kPI/180.0;
   Float_t  latitude_rad = (-90 + view->GetLatitude() ) * kPI/180.0;
   Float_t       psi_rad = (      view->GetPsi()      ) * kPI/180.0;

   // Call 'x3d' package
   x3d_set_display(gVirtualX->GetDisplay());
   fX3DWin = (Window_t) x3d_main(&longitude_rad, &latitude_rad, &psi_rad,
   fOption.Data(), fCanvas->GetViewPort()->GetId());
}


//______________________________________________________________________________
void TViewerX3D::BeginScene()
{
	// The x3d viewer cannot rebuild a scene once created
   if (fgCreated) {
      return;
   }
      
   fBuildingScene = kTRUE;
   
   if (fPass == kSize) {
      gSize3D.numPoints = 0;
      gSize3D.numSegs   = 0;
      gSize3D.numPolys  = 0;
   }
}


//______________________________________________________________________________
void  TViewerX3D::EndScene()
{
	// The x3d viewer cannot rebuild a scene once created
   if (fgCreated) {
		return;
	}
	
   fBuildingScene = kFALSE;   
   
   // Size pass done - and some points actually added
   if (gSize3D.numPoints != 0) {
      if (fPass == kSize) {
         // Allocate the X3D viewer buffer with sizes if any
         if (!AllocateX3DBuffer()) {
            Error("InitX3DWindow", "x3d buffer allocation failure");
            return;
         }
      
         // Enter draw pass and invoke another paint
         fPass = kDraw;
         fPad->Paint();
         fPass = kSize;
         CreateViewer(fTitle); 
         Show();
      }
   } else {
      Int_t retval;
      new TGMsgBox(gClient->GetRoot(), gClient->GetRoot(),
                   "X3D Viewer", "Cannot display this content in the X3D viewer",
                   kMBIconExclamation, kMBOk, &retval);
      CloseWindow();
   }
}


//______________________________________________________________________________
Int_t TViewerX3D::AddObject(const TBuffer3D & buffer, Bool_t * addChildren)
{
	// The x3d viewer cannot rebuild a scene once created
   if (fgCreated) {
      if (addChildren) {
         *addChildren = kFALSE;
      }
      return TBuffer3D::kNone;
   }
   else if (addChildren) {
      *addChildren = kTRUE;
   }
   // Ensure we have the required sections
   UInt_t reqSections = TBuffer3D::kCore|TBuffer3D::kRawSizes;
   
   // Sizing does not require actual raw tesselation information
   if (fPass == kDraw) {
      reqSections |= TBuffer3D::kRaw;
   }
   
   if (!buffer.SectionsValid(reqSections)) {
      return reqSections;
   }
   
   if (buffer.Type() == TBuffer3DTypes::kMarker) {
      PaintPolyMarker(buffer);
      return TBuffer3D::kNone;
   }
      
   switch(fPass) {
      case(kSize): {
         gSize3D.numPoints += buffer.NbPnts();
         gSize3D.numSegs   += buffer.NbSegs();
         gSize3D.numPolys  += buffer.NbPols();
         break;
      }
      case (kDraw): {
         X3DBuffer *X3Dbuff = new X3DBuffer;
         X3Dbuff->numPoints = buffer.NbPnts();
         X3Dbuff->numSegs   = buffer.NbSegs();
         X3Dbuff->numPolys  = buffer.NbPols();
         X3Dbuff->points    = new Float_t[3*buffer.NbPnts()];
         for (UInt_t i=0; i<3*buffer.NbPnts();i++)
            X3Dbuff->points[i] = (Float_t)buffer.fPnts[i];
         X3Dbuff->segs      = buffer.fSegs;
         X3Dbuff->polys     = buffer.fPols;
         FillX3DBuffer(X3Dbuff);
         delete [] X3Dbuff->points;
         break;
      }
      default: {
         assert(kFALSE);
         break;
      }
   }
   
   return TBuffer3D::kNone;
}


//______________________________________________________________________________
Int_t TViewerX3D::AddObject(UInt_t /* placedID */, const TBuffer3D & buffer, Bool_t * addChildren)
{
   // We don't support placed IDs - discard
   return AddObject(buffer,addChildren);
}


//______________________________________________________________________________
void TViewerX3D::PaintPolyMarker(const TBuffer3D & buffer) const
{
   if (fgCreated) {
      return;
   }
   UInt_t mode;

   if (buffer.NbPnts() > 10000) mode = 1;     // One line marker    '-'
   else if (buffer.NbPnts() > 3000) mode = 2; // Two lines marker   '+'
   else mode = 3;                           // Three lines marker '*'

   switch(fPass) {
      case(kSize): {
         gSize3D.numPoints += 2*mode*buffer.NbPnts();
         gSize3D.numSegs   += mode*buffer.NbPnts();
         break;
      }
      case (kDraw): {
         X3DBuffer *X3Dbuff = new X3DBuffer;
         X3Dbuff->numPoints = 2*mode*buffer.NbPnts();
         X3Dbuff->numSegs   = mode*buffer.NbPnts();
         X3Dbuff->numPolys  = 0;
         X3Dbuff->points    = new Float_t[3*X3Dbuff->numPoints];
         X3Dbuff->segs      = new Int_t[3*X3Dbuff->numSegs];
         X3Dbuff->polys     = NULL;

         Double_t delta = 0.002;
         
         for (UInt_t i = 0; i < buffer.NbPnts(); i++) {
            for (UInt_t j = 0; j < mode; j++) {
               for (UInt_t k = 0; k < 2; k++) {
                  delta *= -1;
                  for (UInt_t n = 0; n < 3; n++) {
                     X3Dbuff->points[mode*6*i+6*j+3*k+n] =
                     buffer.fPnts[3*i+n] * (1 + (j == n ? delta : 0));
                  }
               }
            }
         }

         for (Int_t i=0; i<X3Dbuff->numSegs; i++) {
            X3Dbuff->segs[3*i  ] = buffer.fSegs[0];
            X3Dbuff->segs[3*i+1] = 2*i;
            X3Dbuff->segs[3*i+2] = 2*i+1;
         }

         FillX3DBuffer(X3Dbuff);
         delete [] X3Dbuff->points;
         delete [] X3Dbuff->segs;
         break;
      }
   }
}


//______________________________________________________________________________
Int_t TViewerX3D::ExecCommand(Int_t px, Int_t py, char command)
{
// This function may be called from a script to animate an X3D picture
// px, py  mouse position
//command = 0       --- move to px,py
//        = w       --- wireframe mode
//        = e       --- hidden line mode
//        = r       --- hidden surface mode
//        = u       --- move object down
//        = i       --- move object up
//        = o       --- toggle controls style
//        = s       --- toggle stereo display
//        = d       --- toggle blue stereo view
//        = f       --- toggle double buffer
//        = h       --- move object right
//        = j       --- move object forward
//        = k       --- move object backward
//        = l       --- move object left
//        = x a     --- rotate about x
//        = y b     --- rotate about y
//        = z c     --- rotate about z
//        = 1 2 3   --- autorotate about x
//        = 4 5 6   --- autorotate about y
//        = 7 8 9   --- autorotate about z
//        = [ ] { } --- adjust focus
// Example:
/*
{
   gSystem->Load("libX3d");
   TCanvas *c1 = new TCanvas("c1");
   TFile *f = new TFile("hsimple.root");
   TTree *ntuple = (TTree*)f->Get("ntuple");
   ntuple->SetMarkerColor(kYellow);
   ntuple->Draw("px:py:pz");
   TViewerX3D *x3d = new TViewerX3D(c1,"");
   for (Int_t i=0;i<500;i++) {
      Int_t px = i%500;
      Int_t py = (2*i)%200;
      x3d->ExecCommand(px,py,0);  //rotate
      if (i%20 >10) x3d->ExecCommand(px,py,'j'); //zoom
      if (i%20 <10) x3d->ExecCommand(px,py,'k'); //unzoom
   }
}
*/

   return x3d_exec_command(px,py,command);
}


//______________________________________________________________________________
void TViewerX3D::GetPosition(Float_t &longitude, Float_t &latitude, Float_t &psi)
{
   x3d_get_position(&longitude, &latitude, &psi);
}

//______________________________________________________________________________
void TViewerX3D::DeleteX3DWindow()
{
   // Close X3D window.

   x3d_terminate();
}


//______________________________________________________________________________
void TViewerX3D::Update()
{
   // Update X3D viewer.

   x3d_update();
}


//______________________________________________________________________________
Bool_t TViewerX3D::ProcessMessage(Long_t msg, Long_t parm1, Long_t)
{
   // Handle menu and other command generated by the user.

   TRootHelpDialog *hd;

   switch (GET_MSG(msg)) {

      case kC_COMMAND:

         switch (GET_SUBMSG(msg)) {

            case kCM_BUTTON:
            case kCM_MENU:

               switch (parm1) {
                  // Handle File menu items...
                  case kFileNewViewer:
                     if (fPad) fPad->GetViewer3D("x3d");
                     break;
                  case kFileSave:
                  case kFileSaveAs:
                  case kFilePrint:
                     break;
                  case kFileCloseViewer:
                     SendCloseMessage();
                     break;

                  // Handle Help menu items...
                  case kHelpAbout:
                     {
                        char str[32];
                        sprintf(str, "About ROOT %s...", gROOT->GetVersion());
                        hd = new TRootHelpDialog(this, str, 600, 400);
                        hd->SetText(gHelpAbout);
                        hd->Popup();
                     }
                     break;
                  case kHelpOnViewer:
                     hd = new TRootHelpDialog(this, "Help on X3D Viewer...", 600, 400);
                     hd->SetText(gHelpX3DViewer);
                     hd->Popup();
                     break;
               }
            default:
               break;
         }
      default:
         break;
   }
   return kTRUE;
}


//______________________________________________________________________________
Bool_t TViewerX3D::HandleContainerButton(Event_t * /*ev */ )
{
   // After button release get current position and update associated pad.

   // Currently disabled as only drawing into one view at a time
   // Re-enalbe when multiple viewer implemented on pad
   /*if (ev->fType == kButtonRelease) {
      Float_t longitude_rad;
      Float_t latitude_rad;
      Float_t psi_rad;
      const Float_t kPI = Float_t (TMath::Pi());

      x3d_get_position(&longitude_rad, &latitude_rad, &psi_rad);

      Int_t irep;

      Float_t longitude_deg = longitude_rad * 180.0/kPI - 90;
      Float_t  latitude_deg = latitude_rad  * 180.0/kPI + 90;
      Float_t       psi_deg = psi_rad       * 180.0/kPI;

      fPad->GetView()->SetView(longitude_deg, latitude_deg, psi_deg, irep);

      fPad->SetPhi(-90 - longitude_deg);
      fPad->SetTheta(90 - latitude_deg);

      fPad->Modified(kTRUE);
      fPad->Update();
}*/
   return kTRUE;
}
