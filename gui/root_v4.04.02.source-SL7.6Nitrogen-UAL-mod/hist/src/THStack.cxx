// @(#)root/hist:$Name:  $:$Id: THStack.cxx,v 1.35 2005/03/08 17:43:54 brun Exp $
// Author: Rene Brun   10/12/2001

/*************************************************************************
 * Copyright (C) 1995-2001, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#include "TROOT.h"
#include "THStack.h"
#include "TVirtualPad.h"
#include "TFrame.h"
#include "TH2.h"
#include "TH3.h"
#include "TList.h"
#include "Riostream.h"
#include "TBrowser.h"

ClassImp(THStack)

//______________________________________________________________________________
//
//   A THStack is a collection of TH1 (or derived) objects
//   Use THStack::Add to add a new histogram to the list.
//   The THStack owns the objects in the list.
//   By default (if option "nostack" is not specified), histograms will be paint
//   stacked on top of each other.
//   Example;
//      THStack hs("hs","test stacked histograms");
//      TH1F *h1 = new TH1F("h1","test hstack",100,-4,4);
//      h1->FillRandom("gaus",20000);
//      h1->SetFillColor(kRed);
//      hs.Add(h1);
//      TH1F *h2 = new TH1F("h2","test hstack",100,-4,4);
//      h2->FillRandom("gaus",15000);
//      h2->SetFillColor(kBlue);
//      hs.Add(h2);
//      TH1F *h3 = new TH1F("h3","test hstack",100,-4,4);
//      h3->FillRandom("gaus",10000);
//      h3->SetFillColor(kGreen);
//      hs.Add(h3);
//      TCanvas c1("c1","stacked hists",10,10,700,900);
//      c1.Divide(1,2);
//      c1.cd(1);
//      hs.Draw();
//      c1.cd(2);
//      hs->Draw("nostack");
//
//  See a more complex example in $ROOTSYS/tutorials/hstack.C
//
//  Note that picking is supported for all drawing modes.

//______________________________________________________________________________
THStack::THStack(): TNamed()
{
// THStack default constructor

   fHists     = 0;
   fStack     = 0;
   fHistogram = 0;
   fMaximum   = -1111;
   fMinimum   = -1111;
}

//______________________________________________________________________________
THStack::THStack(const char *name, const char *title)
       : TNamed(name,title)
{
// constructor with name and title
   fHists     = 0;
   fStack     = 0;
   fHistogram = 0;
   fMaximum   = -1111;
   fMinimum   = -1111;
   gROOT->GetListOfCleanups()->Add(this);   
}


//______________________________________________________________________________
THStack::THStack(const TH1* hist, Option_t *axis /*="x"*/, 
                 const char *name /*=0*/, const char *title /*=0*/,
                 Int_t firstbin /*=-1*/, Int_t lastbin /*=-1*/, 
                 Int_t firstbin2 /*=-1*/, Int_t lastbin2 /*=-1*/, 
                 Option_t* proj_option /*=""*/, Option_t* draw_option /*=""*/): TNamed(name, title) {
// Creates a new THStack from a TH2 or TH3
// It is filled with the 1D histograms from GetProjectionX or GetProjectionY
// for each bin of the histogram. It illustrates the differences and total 
// sum along an axis.
// 
// Parameters:
// - hist:  the histogram used for the projections. Can be an object deriving 
//          from TH2 or TH3.
// - axis:  for TH2: "x" for ProjectionX, "y" for ProjectionY.
//          for TH3: see TH3::Project3D.
// - name:  fName is set to name if given, otherwise to histo's name with 
//          "_stack_<axis>" appended, where <axis> is the value of the 
//          parameter axis.
// - title: fTitle is set to title if given, otherwise to histo's title
//          with ", stack of <axis> projections" appended.
// - firstbin, lastbin:
//          for each bin within [firstbin,lastbin] a stack entry is created.
//          See TH2::ProjectionX/Y for use overflow bins.
//          Defaults to "all bins"
// - firstbin2, lastbin2:
//          Other axis range for TH3::Project3D, defaults to "all bins".
//          Ignored for TH2s
// - proj_option:
//          option passed to TH2::ProjectionX/Y and TH3::Project3D (along 
//          with axis)
// - draw_option:
//          option passed to THStack::Add.
   fHists     = 0;
   fStack     = 0;
   fHistogram = 0;
   fMaximum   = -1111;
   fMinimum   = -1111;
   gROOT->GetListOfCleanups()->Add(this);   

   if (!axis) {
      Warning("THStack", "Need an axis.");
      return;
   }
   if (!hist) {
      Warning("THStack", "Need a histogram.");
      return;
   }
   Bool_t isTH2=hist->IsA()->InheritsFrom(TH2::Class());
   Bool_t isTH3=hist->IsA()->InheritsFrom(TH3::Class());
   if (!isTH2 && !isTH3) {
      Warning("THStack", "Need a histogram deriving from TH2 or TH3.");
      return;
   }

   if (!fName.Length())
      fName=Form("%s_stack%s", hist->GetName(), axis);
   if (!fTitle.Length())
      if (hist->GetTitle() && strlen(hist->GetTitle()))
         fTitle=Form("%s, stack of %s projections", hist->GetTitle(), axis);
      else
         fTitle=Form("stack of %s projections", axis);

   if (isTH2) {
      TH2* hist2=(TH2*) hist;
      Bool_t useX=(strchr(axis,'x')) || (strchr(axis,'X'));
      Bool_t useY=(strchr(axis,'y')) || (strchr(axis,'Y'));
      if (!useX && !useY || (useX && useY)) {
         Warning("THStack", "Need parameter axis=\"x\" or \"y\" for a TH2, not none or both.");
         return;
      }
      TAxis* haxis= useX ? hist->GetYaxis() : hist->GetXaxis();
      if (!haxis) {
         Warning("HStack","Histogram axis is NULL");
         return;
      }
      Int_t nbins = haxis->GetNbins();
      if (firstbin < 0) firstbin = 1;
      if (lastbin  < 0) lastbin  = nbins;
      if (lastbin  > nbins+1) lastbin = nbins;
      for (Int_t iBin=firstbin; iBin<=lastbin; iBin++) {
         TH1* hProj=0;
         if (useX) hProj=hist2->ProjectionX(Form("%s_px%d",hist2->GetName(), iBin), 
					    iBin, iBin, proj_option);
         else hProj=hist2->ProjectionY(Form("%s_py%d",hist2->GetName(), iBin), 
				       iBin, iBin, proj_option);
         Add(hProj, draw_option);
      }
   } else {
      // hist is a TH3
      TH3* hist3=(TH3*) hist;
      TString sAxis(axis);
      sAxis.ToLower();
      Int_t dim=3-sAxis.Length();
      if (dim<1 || dim>2) {
         Warning("THStack", "Invalid length for parameter axis.");
         return;
      }

      if (dim==1) {
         TAxis* haxis = 0;
         // look for the haxis _not_ in axis
         if (sAxis.First('x')==kNPOS) 
            haxis=hist->GetXaxis();
         else if (sAxis.First('y')==kNPOS) 
            haxis=hist->GetYaxis();
         else if (sAxis.First('z')==kNPOS) 
            haxis=hist->GetZaxis();
         if (!haxis) {
            Warning("HStack","Histogram axis is NULL");
            return;
         }

         Int_t nbins = haxis->GetNbins();
         if (firstbin < 0) firstbin = 1;
         if (lastbin  < 0) lastbin  = nbins;
         if (lastbin  > nbins+1) lastbin = nbins;
         Int_t iFirstOld=haxis->GetFirst();
         Int_t iLastOld=haxis->GetLast();
         for (Int_t iBin=firstbin; iBin<=lastbin; iBin++) {
            haxis->SetRange(iBin, iBin);
            // build projection named axis_iBin (passed through "option")
            TH1* hProj=hist3->Project3D(Form("%s_%s%s_%d", hist3->GetName(), 
					     axis, proj_option, iBin));
            Add(hProj, draw_option);
         }
         haxis->SetRange(iFirstOld, iLastOld);
      }  else {
         // if dim==2
         TAxis* haxis1 = 0;
         TAxis* haxis2 = 0;
         // look for the haxis _not_ in axis
         if (sAxis.First('x')!=kNPOS) {
            haxis1=hist->GetYaxis();
            haxis2=hist->GetZaxis();
         } else if (sAxis.First('y')!=kNPOS) {
            haxis1=hist->GetXaxis();
            haxis2=hist->GetZaxis();
         } else if (sAxis.First('z')!=kNPOS) {
            haxis1=hist->GetXaxis();
            haxis2=hist->GetYaxis();
         }
         if (!haxis1 || !haxis2) {
            Warning("HStack","Histogram axis is NULL");
            return;
         }

         Int_t nbins1 = haxis1->GetNbins();
         Int_t nbins2 = haxis2->GetNbins();
         if (firstbin < 0) firstbin = 1;
         if (lastbin  < 0) lastbin  = nbins1;
         if (lastbin  > nbins1+1) lastbin = nbins1;
         if (firstbin2 < 0) firstbin2 = 1;
         if (lastbin2  < 0) lastbin2  = nbins2;
         if (lastbin2  > nbins2+1) lastbin2 = nbins2;
         Int_t iFirstOld1=haxis1->GetFirst();
         Int_t iLastOld1=haxis1->GetLast();
         Int_t iFirstOld2=haxis2->GetFirst();
         Int_t iLastOld2=haxis2->GetLast();
         for (Int_t iBin=firstbin; iBin<=lastbin; iBin++) {
            haxis1->SetRange(iBin, iBin);
            for (Int_t jBin=firstbin2; jBin<=lastbin2; jBin++) {
               haxis2->SetRange(jBin, jBin);
               // build projection named axis_iBin (passed through "option")
               TH1* hProj=hist3->Project3D(Form("%s_%s%s_%d", hist3->GetName(), 
						axis, proj_option, iBin));
               Add(hProj, draw_option);
            }
         }
         haxis1->SetRange(iFirstOld1, iLastOld1);
         haxis2->SetRange(iFirstOld2, iLastOld2);
      }
   } // if hist is TH2 or TH3
}

//______________________________________________________________________________
THStack::~THStack()
{
// THStack destructor


   gROOT->GetListOfCleanups()->Remove(this);   
   if (!fHists) return;
   fHists->Delete();
   delete fHists;
   fHists = 0;
   if (fStack) {fStack->Delete(); delete fStack;}
   delete fHistogram;
   fHistogram = 0;
}

//______________________________________________________________________________
THStack::THStack(const THStack &hstack) : TNamed(hstack)
{
// THStack copy constructor

   fHistogram = 0;
   fMaximum = hstack.fMaximum;
   fMinimum = hstack.fMinimum;
   fStack = 0;
   fHists = 0;
   fName  = hstack.fName;
   fTitle = hstack.fTitle;
   if (hstack.GetHists() == 0) return;
   TIter next(hstack.GetHists());
   TH1 *h;
   while ((h=(TH1*)next())) {
      Add(h);
   }
}

//______________________________________________________________________________
void THStack::Add(TH1 *h1, Option_t *option)
{
   // add a new histogram to the list
   // Only 1-d and 2-d histograms currently supported.
   // A drawing option may be specified

   if (!h1) return;
   if (h1->GetDimension() > 2) {
      Error("Add","THStack supports only 1-d and 2-d histograms");
      return;
   }
   if (!fHists) fHists = new TList();
   fHists->Add(h1,option);
   Modified(); //invalidate stack
}

//______________________________________________________________________________
void THStack::Browse(TBrowser *b)
{
    Draw(b ? b->GetDrawOption() : "");
    gPad->Update();
}

//______________________________________________________________________________
void THStack::BuildStack()
{
//  build sum of all histograms
//  Build a separate list fStack containing the running sum of all histograms

   if (fStack) return;
   if (!fHists) return;
   Int_t nhists = fHists->GetSize();
   if (!nhists) return;
   fStack = new TObjArray(nhists);
   Bool_t add = TH1::AddDirectoryStatus();
   TH1::AddDirectory(kFALSE);
   TH1 *h = (TH1*)fHists->At(0)->Clone();
   fStack->Add(h);
   for (Int_t i=1;i<nhists;i++) {
      h = (TH1*)fHists->At(i)->Clone();
      h->Add((TH1*)fStack->At(i-1));
      fStack->AddAt(h,i);
   }
   TH1::AddDirectory(add);
}

//______________________________________________________________________________
Int_t THStack::DistancetoPrimitive(Int_t px, Int_t py)
{
// Compute distance from point px,py to each graph
//

//*-*- Are we on the axis?
   const Int_t kMaxDiff = 10;
   Int_t distance = 9999;
   if (fHistogram) {
      distance = fHistogram->DistancetoPrimitive(px,py);
      if (distance <= 0) {return distance;}
      if (distance <= 1) {gPad->SetSelected(fHistogram);return distance;}
   }


//*-*- Loop on the list of histograms
   if (!fHists) return distance;
   TH1 *h = 0;
   const char *doption = GetDrawOption();
   Int_t nhists = fHists->GetSize();
   for (Int_t i=0;i<nhists;i++) {
      h = (TH1*)fHists->At(i);
      if (fStack && !strstr(doption,"nostack")) h = (TH1*)fStack->At(i);
      Int_t dist = h->DistancetoPrimitive(px,py);
      if (dist <= 0) return 0;
      if (dist < kMaxDiff) {
         gPad->SetSelected(fHists->At(i));
         gPad->SetCursor(kPointer);
         return dist;
      }
   }
   return distance;
}

//______________________________________________________________________________
void THStack::Draw(Option_t *option)
{
//*-*-*-*-*-*-*-*-*-*-*Draw this multihist with its current attributes*-*-*-*-*-*-*
//*-*                  ==========================================
//
//   Options to draw histograms  are described in THistPainter::Paint
// By default (if option "nostack" is not specified), histograms will be paint
// stacked on top of each other.

   TString opt = option;
   opt.ToLower();
   if (gPad) {
      if (!gPad->IsEditable()) (gROOT->GetMakeDefCanvas())();
      if (!opt.Contains("same")) {
         //the following statement is necessary in case one attempts to draw
         //a temporary histogram already in the current pad
         if (TestBit(kCanDelete)) gPad->GetListOfPrimitives()->Remove(this);
         gPad->Clear();
      }
   }
   AppendPad(opt.Data());
}

//______________________________________________________________________________
TH1 *THStack::GetHistogram() const
{
//    Returns a pointer to the histogram used to draw the axis
//    Takes into account the two following cases.
//       1- option 'A' was specified in THStack::Draw. Return fHistogram
//       2- user had called TPad::DrawFrame. return pointer to hframe histogram
//
// IMPORTANT NOTE
//  You must call Draw before calling this function. The returned histogram
//  depends on the selected Draw options.
   
   if (fHistogram) return fHistogram;
   if (!gPad) return 0;
   gPad->Modified();
   gPad->Update();
   if (fHistogram) return fHistogram;
   TH1 *h1 = (TH1*)gPad->FindObject("hframe");
   return h1;
}

//______________________________________________________________________________
Double_t THStack::GetMaximum(Option_t *option)
{
//  returns the maximum of all added histograms
//  returns the maximum of all histograms if option "nostack".

   TString opt = option;
   opt.ToLower();
   Double_t them=0, themax = -1e300;
   Int_t nhists = fHists->GetSize();
   TH1 *h;
   if (!opt.Contains("nostack")) {
       BuildStack();
       h = (TH1*)fStack->At(nhists-1);
       themax = h->GetMaximum();
       if (strstr(opt.Data(),"e1")) themax += TMath::Sqrt(TMath::Abs(themax));
   } else {
      for (Int_t i=0;i<nhists;i++) {
         h = (TH1*)fHists->At(i);
         them = h->GetMaximum();
         if (strstr(opt.Data(),"e1")) them += TMath::Sqrt(TMath::Abs(them));
         if (them > themax) themax = them;
      }
   }
   return themax;
}

//______________________________________________________________________________
Double_t THStack::GetMinimum(Option_t *option)
{
//  returns the minimum of all added histograms
//  returns the minimum of all histograms if option "nostack".

   TString opt = option;
   opt.ToLower();
   Double_t them=0, themin = 1e300;
   Int_t nhists = fHists->GetSize();
   TH1 *h;
   if (!opt.Contains("nostack")) {
       BuildStack();
       h = (TH1*)fStack->At(nhists-1);
       themin = h->GetMinimum();
   } else {
      for (Int_t i=0;i<nhists;i++) {
         h = (TH1*)fHists->At(i);
         them = h->GetMinimum();
         if (them <= 0 && gPad && gPad->GetLogy()) them = h->GetMinimum(0);
         if (them < themin) themin = them;
      }
   }
   return themin;
}

//______________________________________________________________________________
TObjArray *THStack::GetStack()
{
   // Return pointer to Stack. Build it if not yet done

   BuildStack();
   return fStack;
}

//______________________________________________________________________________
TAxis *THStack::GetXaxis() const
{
// Get x axis of the histogram used to draw the stack.
//
// IMPORTANT NOTE
//  You must call Draw before calling this function. The returned histogram
//  depends on the selected Draw options.

   if (!gPad) return 0;
   return GetHistogram()->GetXaxis();
}

//______________________________________________________________________________
TAxis *THStack::GetYaxis() const
{
// Get x axis of the histogram used to draw the stack.
//
// IMPORTANT NOTE
//  You must call Draw before calling this function. The returned histogram
//  depends on the selected Draw options.

   if (!gPad) return 0;
   return GetHistogram()->GetYaxis();
}

//______________________________________________________________________________
void THStack::ls(Option_t *option) const
{
   // List histograms in the stack

   TROOT::IndentLevel();
   cout <<IsA()->GetName()
        <<" Name= "<<GetName()<<" Title= "<<GetTitle()<<" Option="<<option<<endl;
   TROOT::IncreaseDirLevel();
   if (fHists) fHists->ls(option);
   TROOT::DecreaseDirLevel();
}

//______________________________________________________________________________
void THStack::Modified()
{
// invalidate sum of histograms

   if (!fStack) return;
   fStack->Delete();
   delete fStack;
   fStack = 0;
}

//______________________________________________________________________________
void THStack::Paint(Option_t *option)
{
// paint the list of histograms
// By default, histograms are shown stacked.
//    -the first histogram is paint
//    -then the sum of the first and second, etc
//
// If option "nostack" is specified, histograms are all paint in the same pad
// as if the option "same" had been specified.
//
// if option "pads" is specified, the current pad/canvas is subdivided into
// a number of pads equal to the number of histograms and each histogram
// is paint into a separate pad.
//
// See THistPainter::Paint for a list of valid options.

   if (!fHists) return;
   if (!fHists->GetSize()) return;

   TString opt = option;
   opt.ToLower();
   Bool_t lsame = kFALSE;
   if (opt.Contains("same")) {
      lsame = kTRUE;
      opt.ReplaceAll("same","");
   }
   if (opt.Contains("pads")) {
      Int_t npads = fHists->GetSize();
      TVirtualPad *padsav = gPad;
      //if pad is not already divided into subpads, divide it
      Int_t nps = 0;
      TObject *obj;
      TIter nextp(padsav->GetListOfPrimitives());
      while ((obj = nextp())) {
         if (obj->InheritsFrom(TVirtualPad::Class())) nps++;
      }
      if (nps < npads) {
         padsav->Clear();
         Int_t nx = (Int_t)TMath::Sqrt((Double_t)npads);
         if (nx*nx < npads) nx++;
         padsav->Divide(nx,nx);
      }
      TH1 *h;
      Int_t i = 0;
      TObjOptLink *lnk = (TObjOptLink*)fHists->FirstLink();
      while (lnk) {
         i++;
         padsav->cd(i);
         h = (TH1*)lnk->GetObject();
         h->Paint(lnk->GetOption());
         lnk = (TObjOptLink*)lnk->Next();
      }
      padsav->cd();
      return;
   }

   // compute the min/max of each axis
   TH1 *h;
   TIter next(fHists);
   Double_t xmin = 1e100;
   Double_t xmax = -xmin;
   Double_t ymin = 1e100;
   Double_t ymax = -xmin;
   while ((h=(TH1*)next())) {
      if (h->GetXaxis()->GetXmin() < xmin) xmin = h->GetXaxis()->GetXmin();
      if (h->GetXaxis()->GetXmax() > xmax) xmax = h->GetXaxis()->GetXmax();
      if (h->GetYaxis()->GetXmin() < ymin) ymin = h->GetYaxis()->GetXmin();
      if (h->GetYaxis()->GetXmax() > ymax) ymax = h->GetYaxis()->GetXmax();
   }

   char loption[32];
   sprintf(loption,"%s",opt.Data());
   char *nostack = strstr(loption,"nostack");
   // do not delete the stack. Another pad may contain the same object
   // drawn in stack mode!
   //if (nostack && fStack) {fStack->Delete(); delete fStack; fStack = 0;}

   if (!opt.Contains("nostack")) BuildStack();

   Double_t themax,themin;
   if (fMaximum == -1111) themax = GetMaximum(option);
   else                   themax = fMaximum;
   if (fMinimum == -1111) {
     themin = GetMinimum(option);
     if (gPad->GetLogy()){
       if (themin>0)  themin *= .9;
       else           themin = themax*1.e-3;
     }
     else if (themin > 0)
       themin = 0;
   }
   else                   themin = fMinimum;
   if (!fHistogram) {
      Bool_t add = TH1::AddDirectoryStatus();
      TH1::AddDirectory(kFALSE);
      TH1 *h = (TH1*)fHists->At(0);
      TAxis *xaxis = h->GetXaxis();
      TAxis *yaxis = h->GetYaxis();
      if (h->GetDimension() > 1) {
         if (strlen(option) == 0) strcpy(loption,"lego1");
	    const TArrayD *xbins = xaxis->GetXbins();
	    const TArrayD *ybins = yaxis->GetXbins();
	    if (xbins->fN != 0 && ybins->fN != 0) {
            fHistogram = new TH2F(GetName(),GetTitle(),
                                  xaxis->GetNbins(), xbins->GetArray(),
                                  yaxis->GetNbins(), ybins->GetArray());
	    } else if (xbins->fN != 0 && ybins->fN == 0) {
            fHistogram = new TH2F(GetName(),GetTitle(),
                                  xaxis->GetNbins(), xbins->GetArray(),
                                  yaxis->GetNbins(), ymin, ymax);
	    } else if (xbins->fN == 0 && ybins->fN != 0) {
            fHistogram = new TH2F(GetName(),GetTitle(),
                                  xaxis->GetNbins(), xmin, xmax,
                                  yaxis->GetNbins(), ybins->GetArray());
	    } else {
            fHistogram = new TH2F(GetName(),GetTitle(),
                                  xaxis->GetNbins(), xmin, xmax,
                                  yaxis->GetNbins(), ymin, ymax);
         }
      } else {
         fHistogram = new TH1F(GetName(),GetTitle(),xaxis->GetNbins(),xmin, xmax);
      }
      fHistogram->SetStats(0);
      TH1::AddDirectory(add);
   } else {
      fHistogram->SetTitle(GetTitle());
   }

   if (nostack) {*nostack = 0; strcat(nostack,nostack+7);}
   //if (nostack) {strncpy(nostack,"       ",7);}
   else fHistogram->GetPainter()->SetStack(fHists);

   if (!fHistogram->TestBit(TH1::kIsZoomed)) {
      if (nostack && fMaximum != -1111) fHistogram->SetMaximum(fMaximum);
      else {
         if (gPad->GetLogy())           fHistogram->SetMaximum(themax*(1+0.2*TMath::Log10(themax/themin)));
         else                           fHistogram->SetMaximum(1.1*themax);
      }
      if (nostack && fMinimum != -1111) fHistogram->SetMinimum(fMinimum);
      else {
         if (gPad->GetLogy())           fHistogram->SetMinimum(themin/(1+0.5*TMath::Log10(themax/themin)));
         else                           fHistogram->SetMinimum(themin);
      }
   }
   if (!lsame) fHistogram->Paint(loption);

   if (fHistogram->GetDimension() > 1) SetDrawOption(loption);
   if (strstr(loption,"lego")) return;

   char noption[32];
   strcpy(noption,loption);
   Int_t nhists = fHists->GetSize();
   if (nostack) {
      TObjOptLink *lnk = (TObjOptLink*)fHists->FirstLink();
      for (Int_t i=0;i<nhists;i++) {
         sprintf(loption,"%ssame%s",noption,lnk->GetOption());
         fHists->At(i)->Paint(loption);
         lnk = (TObjOptLink*)lnk->Next();
      }
   } else {
      TObjOptLink *lnk = (TObjOptLink*)fHists->LastLink();
      TH1 *h1;
      Int_t h1col, h1fill;
      for (Int_t i=0;i<nhists;i++) {
         sprintf(loption,"%ssame%s",noption,lnk->GetOption());
         h1 = (TH1*)fStack->At(nhists-i-1);
         if (i>0) {
            // Erase before drawing the histogram 
            h1col  = h1->GetFillColor();
            h1fill = h1->GetFillStyle();
            h1->SetFillColor(1000);
            h1->SetFillStyle(1001);
            h1->Paint(loption);
            h1->SetFillColor(gPad->GetFrame()->GetFillColor());
            h1->SetFillStyle(gPad->GetFrame()->GetFillStyle());
            h1->Paint(loption);
            h1->SetFillColor(h1col);
            h1->SetFillStyle(h1fill);
          }
         h1->Paint(loption);
         lnk = (TObjOptLink*)lnk->Prev();
      }
   }
   fHistogram->Paint("axissame");
}

//______________________________________________________________________________
void THStack::Print(Option_t *option) const
{
// Print the list of histograms

   TH1 *h;
   if (fHists) {
     TIter   next(fHists);
     while ((h = (TH1*) next())) {
       h->Print(option);
     }
   }
}

//______________________________________________________________________________
void THStack::RecursiveRemove(TObject *obj)
{
// Recursively remove object from the list of histograms

   if (!fHists) return;
   fHists->RecursiveRemove(obj);
   while (fHists->IndexOf(obj) >= 0) fHists->Remove(obj);
}

//______________________________________________________________________________
void THStack::SavePrimitive(ofstream &out, Option_t *option)
{
    // Save primitive as a C++ statement(s) on output stream out

   char quote = '"';
   out<<"   "<<endl;
   if (gROOT->ClassSaved(THStack::Class())) {
       out<<"   ";
   } else {
       out<<"   THStack *";
   }
   out<<GetName()<<" = new THStack();"<<endl;
   out<<"   "<<GetName()<<"->SetName("<<quote<<GetName()<<quote<<");"<<endl;
   out<<"   "<<GetName()<<"->SetTitle("<<quote<<GetTitle()<<quote<<");"<<endl;

   if (fMinimum != -1111) {
      out<<"   "<<GetName()<<"->SetMinimum("<<fMinimum<<");"<<endl;
   }
   if (fMaximum != -1111) {
      out<<"   "<<GetName()<<"->SetMaximum("<<fMaximum<<");"<<endl;
   }

   TH1 *h;
   if (fHists) {
      TObjOptLink *lnk = (TObjOptLink*)fHists->FirstLink();
      while (lnk) {
         h = (TH1*)lnk->GetObject();
         h->SavePrimitive(out,"nodraw");
         out<<"   "<<GetName()<<"->Add("<<h->GetName()<<","<<quote<<lnk->GetOption()<<quote<<");"<<endl;
         lnk = (TObjOptLink*)lnk->Next();
      }
   }
   out<<"   "<<GetName()<<"->Draw("
      <<quote<<option<<quote<<");"<<endl;
}

//______________________________________________________________________________
void THStack::SetMaximum(Double_t maximum)
{
   fMaximum = maximum;
   if (fHistogram)  fHistogram->SetMaximum(maximum);
}

//______________________________________________________________________________
void THStack::SetMinimum(Double_t minimum)
{
   fMinimum = minimum;
   if (fHistogram) fHistogram->SetMinimum(minimum);
}
