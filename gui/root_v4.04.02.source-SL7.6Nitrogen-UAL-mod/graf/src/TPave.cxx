// @(#)root/graf:$Name:  $:$Id: TPave.cxx,v 1.17 2004/08/18 10:55:37 brun Exp $
// Author: Rene Brun   16/10/95

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include "Riostream.h"
#include "TROOT.h"
#include "TPave.h"
#include "TStyle.h"
#include "TVirtualPad.h"
#include "TClass.h"
#include "TMath.h"


ClassImp(TPave)

//______________________________________________________________________________
//
// a PAVE is a TBox with a bordersize and a shadow option
// The corners of a TPave can be rounded (option "arc")
// More functional objects like TPavelabel, TPaveText derive from TPave.
//
//Begin_Html
/*
<img src="gif/pave.gif">
*/
//End_Html
//

//______________________________________________________________________________
TPave::TPave(): TBox()
{
//*-*-*-*-*-*-*-*-*-*-*pave default constructor*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                  ========================
   fBorderSize   = 4;
   fOption       = "brNDC";
   fName         = "";
   fInit         = 1;
   fCornerRadius = 0;
   SetFillColor(gStyle->GetFillColor());
   SetFillStyle(gStyle->GetFillStyle());
   SetLineColor(gStyle->GetLineColor());
   SetLineStyle(gStyle->GetLineStyle());
}

//______________________________________________________________________________
TPave::TPave(Double_t x1, Double_t y1,Double_t x2, Double_t  y2,
             Int_t bordersize ,Option_t *option)
       :TBox(x1,y1,x2,y2)
{
//*-*-*-*-*-*-*-*-*-*-*pave normal constructor*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                  =======================
// a PAVE is a box with a bordersize and a shadow option
// the bordersize is in pixels
//  option = "T" Top frame
//  option = "B" Bottom frame
//  option = "R" Right frame
//  option = "L" Left frame
//  option = "NDC" x1,y1,x2,y2 are given in NDC
//  option = "ARC" corners are rounded
//
//  IMPORTANT NOTE:
//  Because TPave objects (and objects deriving from TPave) have their
//  master coordinate system in NDC, one cannot use the TBox functions
//  SetX1,SetY1,SetX2,SetY2 to change the corner coordinates. One should use
//  instead SetX1NDC, SetY1NDC, SetX2NDC, SetY2NDC.

   fBorderSize   = bordersize;
   fOption       = option;
   fName         = "";
   fInit         = 0;
   fCornerRadius = 0;
   if (fOption == "NDC" || fOption == "ndc") fOption = "brNDC";

   SetFillColor(gStyle->GetFillColor());
   SetFillStyle(gStyle->GetFillStyle());
   SetLineColor(gStyle->GetLineColor());
   SetLineStyle(gStyle->GetLineStyle());
   SetName((char*)ClassName());
}

//______________________________________________________________________________
TPave::~TPave()
{
//*-*-*-*-*-*-*-*-*-*-*pave default destructor*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                  =======================
}

//______________________________________________________________________________
TPave::TPave(const TPave &pave) : TBox(pave)
{
   ((TPave&)pave).TPave::Copy(*this);
}

//______________________________________________________________________________
void TPave::ConvertNDCtoPad()
{
//*-*-*-*-*-*-*Convert pave coordinates from NDC to Pad coordinates*-*-*-*-*-*
//*-*          ====================================================

   Double_t dpx  = gPad->GetX2() - gPad->GetX1();
   Double_t dpy  = gPad->GetY2() - gPad->GetY1();
   Double_t xp1  = gPad->GetX1();
   Double_t yp1  = gPad->GetY1();
//*-*- Check if pave initialisation has been done.
//*-*  This operation cannot take place in the Pave constructor because
//*-*  the Pad range may not be known at this time.
   if (!fInit) {
      fInit = 1;
      if (fOption.Contains("NDC")) {
         fX1NDC = fX1;
         fY1NDC = fY1;
         fX2NDC = fX2;
         fY2NDC = fY2;
         fX1    = xp1 + fX1NDC*dpx;
         fY1    = yp1 + fY1NDC*dpy;
         fX2    = xp1 + fX2NDC*dpx;
         fY2    = yp1 + fY2NDC*dpy;
      } else {
         if (gPad->GetLogx()) {
            if (fX1 > 0) fX1 = TMath::Log10(fX1);
            if (fX2 > 0) fX2 = TMath::Log10(fX2);
         }
         if (gPad->GetLogy()) {
            if (fY1 > 0) fY1 = TMath::Log10(fY1);
            if (fY2 > 0) fY2 = TMath::Log10(fY2);
         }
         fX1NDC = (fX1-xp1)/dpx;
         fY1NDC = (fY1-yp1)/dpy;
         fX2NDC = (fX2-xp1)/dpx;
         fY2NDC = (fY2-yp1)/dpy;
      }
   } else {
      fX1    = xp1 + fX1NDC*dpx;
      fY1    = yp1 + fY1NDC*dpy;
      fX2    = xp1 + fX2NDC*dpx;
      fY2    = yp1 + fY2NDC*dpy;
   }
}

//______________________________________________________________________________
void TPave::Copy(TObject &obj) const
{
//*-*-*-*-*-*-*-*-*-*-*Copy this pave to pave*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//*-*                  ======================

   TBox::Copy(obj);
   ((TPave&)obj).fX1NDC       = fX1NDC;
   ((TPave&)obj).fY1NDC       = fY1NDC;
   ((TPave&)obj).fX2NDC       = fX2NDC;
   ((TPave&)obj).fY2NDC       = fY2NDC;
   ((TPave&)obj).fBorderSize  = fBorderSize;
   ((TPave&)obj).fInit        = fInit;
   ((TPave&)obj).fOption      = fOption;
   ((TPave&)obj).fName        = fName;
}

//______________________________________________________________________________
void TPave::Draw(Option_t *option)
{
//*-*-*-*-*-*-*-*-*-*-*Draw this pave with its current attributes*-*-*-*-*-*-*
//*-*                  ==========================================

   Option_t *opt;
   if (strlen(option)) opt = option;
   else                opt = GetOption();

   AppendPad(opt);

}

//______________________________________________________________________________
void TPave::DrawPave(Double_t x1, Double_t y1,Double_t x2, Double_t  y2,
                     Int_t bordersize ,Option_t *option)
{
//*-*-*-*-*-*-*-*-*-*-*Draw this pave with new coordinates*-*-*-*-*-*-*-*-*-*
//*-*                  ===================================
   TPave *newpave = new TPave(x1,y1,x2,y2,bordersize,option);
   newpave->SetBit(kCanDelete);
   newpave->AppendPad(option);
}

//______________________________________________________________________________
void TPave::ExecuteEvent(Int_t event, Int_t px, Int_t py)
{
//*-*-*-*-*-*-*-*-*-*-*Execute action corresponding to one event*-*-*-*
//*-*                  =========================================
//  This member function is called when a PAVE object is clicked.
//

   if (!gPad->IsEditable()) return;

   TBox::ExecuteEvent(event, px, py);

//*-* In case pave coordinates have been modified, recompute NDC coordinates
   Double_t dpx  = gPad->GetX2() - gPad->GetX1();
   Double_t dpy  = gPad->GetY2() - gPad->GetY1();
   Double_t xp1  = gPad->GetX1();
   Double_t yp1  = gPad->GetY1();
   fX1NDC = (fX1-xp1)/dpx;
   fY1NDC = (fY1-yp1)/dpy;
   fX2NDC = (fX2-xp1)/dpx;
   fY2NDC = (fY2-yp1)/dpy;

//*-* In case the bit NameIsAction is activated, execute the action
//    in name via the interpreter.
   if (event == kButton1Double) {
      if (TestBit(kNameIsAction)) gROOT->ProcessLine(GetName());
   }
}

//______________________________________________________________________________
void TPave::ls(Option_t *) const
{
//*-*-*-*-*-*-*-*-*-*-*-*List this pave with its attributes*-*-*-*-*-*-*-*-*
//*-*                    =================================
   TROOT::IndentLevel();
   printf("OBJ: %s\t%s  \tX1= %f Y1=%f X2=%f Y2=%f\n",IsA()->GetName(),GetName(),fX1,fY1,fX2,fY2);
}

//______________________________________________________________________________
void TPave::Paint(Option_t *option)
{
//*-*-*-*-*-*-*-*-*-*-*Paint this pave with its current attributes*-*-*-*-*-*-*
//*-*                  ===========================================
//  option = "T" Top frame
//  option = "B" Bottom frame
//  option = "R" Right frame
//  option = "L" Left frame
//  option = "NDC" x1,y1,x2,y2 are given in NDC
//  option = "ARC" corners are rounded
//      In case of option "ARC", the corner radius is specified
//      via TPave::SetCornerRadius(rad) where rad is given in percent
//      of the pave height (default value is 0.2).

// Convert from NDC to pad coordinates

   ConvertNDCtoPad();

   PaintPave(fX1, fY1, fX2, fY2, fBorderSize, option);

}


//______________________________________________________________________________
void TPave::PaintPave(Double_t x1, Double_t y1,Double_t x2, Double_t  y2,
                      Int_t bordersize ,Option_t *option)
{
//*-*-*-*-*-*-*-*-*-*-*Draw this pave with new coordinates*-*-*-*-*-*-*-*-*-*
//*-*                  ===================================

   Double_t x[7],y[7];
   TString opt = option;
   opt.ToLower();
//*-*- if pave drawn with the arc option, goes through dedicated function
   if (opt.Contains("arc")) {
      PaintPaveArc(x1,y1,x2,y2,bordersize,option);
      return;
   }

//*-*- normal rectangular pave
   if (opt.Length() == 0) opt ="br";
   Int_t fillstyle = GetFillStyle();
   Int_t fillcolor = GetFillColor();
   Int_t linecolor = GetLineColor();

//*-*- Draw first pave as a normal filled box
   if (fBorderSize <= 0 && fillstyle <= 0) return;
   TBox::PaintBox(x1,y1,x2,y2);
   if (fBorderSize <= 0) return;
   if (fBorderSize == 1) {
      gPad->PaintLine(x1,y1,x2,y1);
      gPad->PaintLine(x2,y1,x2,y2);
      gPad->PaintLine(x2,y2,x1,y2);
      gPad->PaintLine(x1,y2,x1,y1);
      return;
   }

   Double_t wy = gPad->PixeltoY(0) - gPad->PixeltoY(fBorderSize);
   Double_t wx = gPad->PixeltoX(fBorderSize) - gPad->PixeltoX(0);
   Int_t mode = 0;
   //*-*- Draw the frame top right
   if (opt.Contains("t") && opt.Contains("r")) {
      mode = 1;
      x[0] = x1 + 1.5*wx;     y[0] = y2;
      x[1] = x[0];            y[1] = y2 + wy;
      x[2] = x2 + wx;         y[2] = y[1];
      x[3] = x[2];            y[3] = y1 + 1.5*wy;
      x[4] = x2;              y[4] = y[3];
      x[5] = x[4];            y[5] = y2;
   }
//*-*- Draw the frame top left
   if (opt.Contains("t") && opt.Contains("l")) {
      mode = 2;
      x[0] = x1 - wx;         y[0] = y1 + 1.5*wy;
      x[1] = x[0];            y[1] = y2 + wy;
      x[2] = x2 - 1.5*wx;     y[2] = y[1];
      x[3] = x[2];            y[3] = y2;
      x[4] = x1;              y[4] = y[3];
      x[5] = x1;              y[5] = y[0];
   }
//*-*- Draw the frame bottom right
   if (opt.Contains("b") && opt.Contains("r")) {
      mode = 3;
      x[0] = x1 + 1.5*wx;     y[0] = y1;
      x[1] = x[0];            y[1] = y1 - wy;
      x[2] = x2 + wx;         y[2] = y[1];
      x[3] = x[2];            y[3] = y2 - 1.5*wy;
      x[4] = x2;              y[4] = y[3];
      x[5] = x[4];            y[5] = y1;
   }
//*-*- Draw the frame bottom left
   if (opt.Contains("b") && opt.Contains("l")) {
      mode = 4;
      x[0] = x1 - wx;         y[0] = y2 - 1.5*wy;
      x[1] = x[0];            y[1] = y1 - wy;
      x[2] = x2 - 1.5*wx;     y[2] = y[1];
      x[3] = x[2];            y[3] = y1;
      x[4] = x1;              y[4] = y[3];
      x[5] = x[4];            y[5] = y[0];
   }
   if (!mode) return;  // nop border mode option specified
   for (Int_t i=0;i<6;i++) {
      if (x[i] < gPad->GetX1()) x[i] = gPad->GetX1();
      if (x[i] > gPad->GetX2()) x[i] = gPad->GetX2();
      if (y[i] < gPad->GetY1()) y[i] = gPad->GetY1();
      if (y[i] > gPad->GetY2()) y[i] = gPad->GetY2();
   }
   x[6] = x[0];   y[6] = y[0];
   SetFillStyle(1001);
   SetFillColor(linecolor);
   TAttFill::Modify();
   gPad->PaintFillArea(6,x,y);
   x[0] = x1;  y[0] = y1;
   x[1] = x1;  y[1] = y2;
   x[2] = x2;  y[2] = y2;
   x[3] = x2;  y[3] = y1;
   x[4] = x1;  y[4] = y1;
   gPad->PaintPolyLine(5,x,y);
   SetFillStyle(fillstyle);
   SetFillColor(fillcolor);
}



//______________________________________________________________________________
void TPave::PaintPaveArc(Double_t x1, Double_t y1, Double_t x2, Double_t y2,
                      Int_t, Option_t *option)
{
//*-*-*-*-*-*-*-*-*-*-*Draw this pave with rounded corners*-*-*-*-*-*-*-*-*-*
//*-*                  ===================================
   const Int_t kNPARC = 10;
   Double_t x[4*kNPARC+10],   y[4*kNPARC+10];
   Double_t px[4*kNPARC+10], py[4*kNPARC+10];
   Int_t i;
   TString opt = option;
   opt.ToLower();
   if (opt.Length() == 0) opt ="br";
   Int_t fillstyle = GetFillStyle();
   Int_t fillcolor = GetFillColor();
   Int_t linecolor = GetLineColor();

   static Double_t cosa[kNPARC], sina[kNPARC];
   static Bool_t done = kFALSE;
   if (!done) {
      done = kTRUE;
      Double_t dtheta = 0.5*3.141592/(kNPARC+1);
      Double_t theta = 0;
      for (i=0;i<kNPARC;i++) {
         theta += dtheta;
         cosa[i] = TMath::Cos(theta);
         sina[i] = TMath::Sin(theta);
      }
   }
   Int_t px1 = gPad->XtoAbsPixel(x1);
   Int_t py1 = gPad->YtoAbsPixel(y1);
   Int_t px2 = gPad->XtoAbsPixel(x2);
   Int_t py2 = gPad->YtoAbsPixel(y2);
//*-*- compute rounded corner radius
   Double_t rad = fCornerRadius;
   if (rad > 0 && rad < 0.5) rad = fCornerRadius;
   else                      rad = 0.2;
   Double_t r = rad*TMath::Abs(py1-py2);
   if (r > 0.5*TMath::Abs(px2-px1)) r = 0.5*TMath::Abs(px2-px1);
   if (r == 0) r = 1;

//*-*- Draw rounded box outline and fill area
   px[0] = px2;  py[0] = py1 - r;  //starts at bottom right
   px[1] = px2;  py[1] = py2 + r;
   Int_t np = 2;
   for (i=0;i<kNPARC;i++) {          //top right corner
      px[np] = px2 - r + r*cosa[i];
      py[np] = py2 + r - r*sina[i];
      np++;
   }
   px[np]   = px2 - r;  py[np]   = py2;
   px[np+1] = px1 + r;  py[np+1] = py2;
   np += 2;
   for (i=kNPARC-1;i>=0;i--) {       //top left corner
      px[np] = px1 + r - r*cosa[i];
      py[np] = py2 + r - r*sina[i];
      np++;
   }
   px[np]   = px1;  py[np]   = py2 + r;
   px[np+1] = px1;  py[np+1] = py1 - r;
   np += 2;
   for (i=0;i<kNPARC;i++) {          //bottom left corner
      px[np] = px1 + r - r*cosa[i];
      py[np] = py1 - r + r*sina[i];
      np++;
   }
   px[np]   = px1 + r;  py[np]   = py1;
   px[np+1] = px2 - r;  py[np+1] = py1;
   np += 2;
   for (i=kNPARC-1;i>=0;i--) {       //bottom right corner
      px[np] = px2 - r + r*cosa[i];
      py[np] = py1 - r + r*sina[i];
      np++;
   }
   px[np] = px[0];  py[np] =py[0];
   TAttLine::Modify();
   TAttFill::Modify();
   for (i=0;i<=np;i++) {
      x[i] = gPad->AbsPixeltoX(Int_t(px[i]));
      y[i] = gPad->AbsPixeltoY(Int_t(py[i]));
   }
   gPad->PaintFillArea(np  , x, y);
   gPad->PaintPolyLine(np+1, x, y);


   if (fBorderSize <= 0) return;

   Double_t wy    = fBorderSize;
   Double_t wx    = fBorderSize;
//*-*- Draw the frame top right
   if (opt.Contains("tr")) {
      px[0] = px2;           py[0] = py1 - r;
      px[1] = px2;           py[1] = py2 + r;
      np = 2;
      for (i=0;i<kNPARC;i++) {       //top right corner inside
         px[np] = px2 - r + r*cosa[i];
         py[np] = py2 + r - r*sina[i];
         np++;
      }
      px[np]   = px2 - r;       py[np]   = py2;
      px[np+1] = px1 + r;       py[np+1] = py2;
      px[np+2] = px1 + r;       py[np+2] = py2 - wy;
      px[np+3] = px2 - r;       py[np+3] = py2 - wy;
      np += 4;
      for (i=kNPARC-1;i>=0;i--) {       //top right corner outside
         px[np] = px2 - r + r*cosa[i]*(1+wx/r);
         py[np] = py2 + r - r*sina[i]*(1+wy/r);
         np++;
      }
      px[np]   = px2 + wx;    py[np]   = py2 + r;
      px[np+1] = px2 + wx;    py[np+1] = py1 - r;
      px[np+2] = px[0];       py[np+2] = py[0];
      np += 3;
   }
//*-*- Draw the frame top left
   if (opt.Contains("tl")) {
      px[0] = px2 - r;           py[0] = py2;
      px[1] = px1 + r;           py[1] = py2;
      np = 2;
      for (i=kNPARC-1;i>=0;i--) {       //top left corner inside
         px[np] = px1 + r - r*cosa[i];
         py[np] = py2 + r - r*sina[i];
         np++;
      }
      px[np]   = px1;       py[np]   = py2 + r;
      px[np+1] = px1;       py[np+1] = py1 - r;
      px[np+2] = px1 - wx;  py[np+2] = py1 - r;
      px[np+3] = px1 - wx;  py[np+3] = py2 + r;
      np += 4;
      for (i=0;i<kNPARC;i++) {       //top left corner outside
         px[np] = px1 + r - r*cosa[i]*(1+wx/r);
         py[np] = py2 + r - r*sina[i]*(1+wy/r);
         np++;
      }
      px[np]   = px1 + r;    py[np]   = py2 - wy;
      px[np+1] = px2 - r;    py[np+1] = py2 - wy;
      px[np+2] = px[0];      py[np+2] = y[0];
      np += 3;
   }
//*-*- Draw the frame bottom right
   if (opt.Contains("br")) {
      px[0] = px1 + r;           py[0] = py1;
      px[1] = px2 - r;           py[1] = py1;
      np = 2;
      for (i=kNPARC-1;i>=0;i--) {       //bottom right corner inside
         px[np] = px2 - r + r*cosa[i];
         py[np] = py1 - r + r*sina[i];
         np++;
      }
      px[np]   = px2;       py[np]   = py1 - r;
      px[np+1] = px2;       py[np+1] = py2 + r;
      px[np+2] = px2 + wx;  py[np+2] = py2 + r;
      px[np+3] = px2 + wx;  py[np+3] = py1 - r;
      np += 4;
      for (i=0;i<kNPARC;i++) {       //bottom right corner outside
         px[np] = px2 - r + r*cosa[i]*(1+wx/r);
         py[np] = py1 - r + r*sina[i]*(1+wy/r);
         np++;
      }
      px[np]   = px2 - r;    py[np]   = py1 + wy;
      px[np+1] = px[0];      py[np+1] = py[0] + wy;
      px[np+2] = px[0];      py[np+2] = py[0];
      np += 3;
   }
//*-*- Draw the frame bottom left
   if (opt.Contains("bl")) {
      px[0] = px1;           py[0] = py2 + r;
      px[1] = px1;           py[1] = py1 - r;
      np = 2;
      for (i=0;i<kNPARC;i++) {          //bottom left corner inside
         px[np] = px1 + r - r*cosa[i];
         py[np] = py1 + r - r*sina[i];
         np++;
      }
      px[np]   = px1 + r;       py[np]   = py1;
      px[np+1] = px2 - r;       py[np+1] = py1;
      px[np+2] = px2 - r;       py[np+2] = py1 + wy;
      px[np+3] = px1 + r;       py[np+3] = py1 + wy;
      np += 4;
      for (i=kNPARC-1;i>=0;i--) {       //bottom left corner outside
         px[np] = px1 + r - r*cosa[i]*(1+wx/r);
         py[np] = py1 - r + r*sina[i]*(1+wy/r);
         np++;
      }
      px[np]   = px1 - wx;    py[np]   = py1 - r;
      px[np+1] = px1 - wx;    py[np+1] = py[0];
      px[np+2] = px[0];       py[np+2] = py[0];
      np += 3;
   }
   SetFillStyle(1001);
   SetFillColor(linecolor);
   TAttFill::Modify();
   for (i=0;i<=np;i++) {
      x[i] = gPad->AbsPixeltoX(Int_t(px[i]));
      y[i] = gPad->AbsPixeltoY(Int_t(py[i]));
   }
   gPad->PaintFillArea(np,x,y);
   SetFillStyle(fillstyle);
   SetFillColor(fillcolor);
}

//______________________________________________________________________________
void TPave::Print(Option_t *option) const
{
//*-*-*-*-*-*-*-*-*-*-*Dump this pave with its attributes*-*-*-*-*-*-*-*-*-*
//*-*                  ==================================
   TBox::Print(option);
}

//______________________________________________________________________________
void TPave::SavePrimitive(ofstream &out, Option_t *)
{
    // Save primitive as a C++ statement(s) on output stream out

   char quote = '"';
   if (gROOT->ClassSaved(TPave::Class())) {
       out<<"   ";
   } else {
       out<<"   TPave *";
   }
   if (fOption.Contains("NDC")) {
      out<<"pave = new TPave("<<fX1NDC<<","<<fY1NDC<<","<<fX2NDC<<","<<fY2NDC
         <<","<<fBorderSize<<","<<quote<<fOption<<quote<<");"<<endl;
   } else {
      out<<"pave = new TPave("<<fX1<<","<<fY1<<","<<fX2<<","<<fY2
         <<","<<fBorderSize<<","<<quote<<fOption<<quote<<");"<<endl;
   }
   if (strcmp(GetName(),"TPave")) {
      out<<"   pave->SetName("<<quote<<GetName()<<quote<<");"<<endl;
   }
   if (fCornerRadius) {
      out<<"   pave->SetCornerRadius("<<fCornerRadius<<");"<<endl;
   }
   SaveFillAttributes(out,"pave",19,1001);
   SaveLineAttributes(out,"pave",1,1,1);
   out<<"   pave->Draw();"<<endl;
}

//______________________________________________________________________________
void TPave::Streamer(TBuffer &R__b)
{
   // Stream an object of class TPave.

   if (R__b.IsReading()) {
      UInt_t R__s, R__c;
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
      if (R__v > 1) {
         TPave::Class()->ReadBuffer(R__b, this, R__v, R__s, R__c);
         return;
      }
      //====process old versions before automatic schema evolution
      TBox::Streamer(R__b);
      Float_t x1ndc,y1ndc,x2ndc,y2ndc,rad;
      R__b >> x1ndc; fX1NDC = x1ndc;
      R__b >> y1ndc; fY1NDC = y1ndc;
      R__b >> x2ndc; fX2NDC = x2ndc;
      R__b >> y2ndc; fY2NDC = y2ndc;
      R__b >> fBorderSize;
      R__b >> fInit;
      R__b >> rad;   fCornerRadius = rad;
      fOption.Streamer(R__b);
      fName.Streamer(R__b);
      R__b.CheckByteCount(R__s, R__c, TPave::IsA());
      //====end of old versions

   } else {
      TPave::Class()->WriteBuffer(R__b,this);
   }
}
