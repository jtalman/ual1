// @(#)root/hist:$Name:  $:$Id: TConfidenceLevel.cxx,v 1.4 2003/06/23 20:37:56 brun Exp $
// Author: Christophe.Delaere@cern.ch   21/08/2002

///////////////////////////////////////////////////////////////////////////
//
// TConfidenceLevel
//
// Class to compute 95% CL limits
// 
///////////////////////////////////////////////////////////////////////////

/*************************************************************************
 * C.Delaere                                                             *
 * adapted from the mclimit code from Tom Junk                           *
 * see http://cern.ch/thomasj/searchlimits/ecl.html                      *
 *************************************************************************/

#include "TConfidenceLevel.h"
#include "TH1F.h"

ClassImp(TConfidenceLevel)

Double_t const TConfidenceLevel::fgMCLM2S = 0.025;
Double_t const TConfidenceLevel::fgMCLM1S = 0.16;
Double_t const TConfidenceLevel::fgMCLMED = 0.5;
Double_t const TConfidenceLevel::fgMCLP1S = 0.84;
Double_t const TConfidenceLevel::fgMCLP2S = 0.975;
// LHWG "one-sided" definition
Double_t const TConfidenceLevel::fgMCL3S1S = 2.6998E-3;
Double_t const TConfidenceLevel::fgMCL5S1S = 5.7330E-7;
// the other definition (not chosen by the LHWG)
Double_t const TConfidenceLevel::fgMCL3S2S = 1.349898E-3;
Double_t const TConfidenceLevel::fgMCL5S2S = 2.866516E-7;

TConfidenceLevel::TConfidenceLevel()
{
   // Default constructor
   fStot = 0;
   fBtot = 0;
   fDtot = 0;
   fTSD  = 0;
   fTSB  = NULL;
   fTSS  = NULL;
   fLRS  = NULL;
   fLRB  = NULL;
   fNMC  = 0;
   fNNMC = 0;
   fISS  = NULL;
   fISB  = NULL;
   fMCL3S = fgMCL3S1S;
   fMCL5S = fgMCL5S1S;
}

TConfidenceLevel::TConfidenceLevel(Int_t mc, bool onesided)
{
   // a constructor that fix some conventions:
   // mc is the number of Monte Carlo experiments
   // while onesided specifies if the intervals are one-sided or not.
   fStot = 0;
   fBtot = 0;
   fDtot = 0;
   fTSD  = 0;
   fTSB  = NULL;
   fTSS  = NULL;
   fLRS  = NULL;
   fLRB  = NULL;
   fNMC  = mc;
   fNNMC = mc;
   fISS  = new Int_t[mc];
   fISB  = new Int_t[mc];
   fMCL3S = onesided ? fgMCL3S1S : fgMCL3S2S;
   fMCL5S = onesided ? fgMCL5S1S : fgMCL5S2S;
}

TConfidenceLevel::~TConfidenceLevel()
{
   // The destructor
   if (fISS)
      delete[]fISS;
   if (fISB)
      delete[]fISB;
   if (fTSB)
      delete[]fTSB;
   if (fTSS)
      delete[]fTSS;
   if (fLRS)
      delete[]fLRS;
   if (fLRB)
      delete[]fLRB;
}

Double_t TConfidenceLevel::GetExpectedStatistic_b(Int_t sigma) const
{
   // Get the expected statistic value in the background only hypothesis
   switch (sigma) {
   case -2:
      return (-2 *((fTSB[fISB[TMath::Min((Int_t) fNMC,(Int_t) TMath::Max((Int_t) 1,(Int_t) (fNMC * fgMCLP2S)))]]) - fStot));
   case -1:
      return (-2 *((fTSB[fISB[TMath::Min((Int_t) fNMC,(Int_t) TMath::Max((Int_t) 1,(Int_t) (fNMC * fgMCLP1S)))]]) - fStot));
   case 0:
      return (-2 *((fTSB[fISB[TMath::Min((Int_t) fNMC,(Int_t) TMath::Max((Int_t) 1,(Int_t) (fNMC * fgMCLMED)))]]) - fStot));
   case 1:
      return (-2 *((fTSB[fISB[TMath::Min((Int_t) fNMC,(Int_t) TMath::Max((Int_t) 1,(Int_t) (fNMC * fgMCLM1S)))]]) - fStot));
   case 2:
      return (-2 *((fTSB[fISB[TMath::Min((Int_t) fNMC,(Int_t) TMath::Max((Int_t) 1,(Int_t) (fNMC * fgMCLM2S)))]]) - fStot));
   default:
      return 0;
   }
}

Double_t TConfidenceLevel::GetExpectedStatistic_sb(Int_t sigma) const
{
   // Get the expected statistic value in the signal plus background hypothesis
   switch (sigma) {
   case -2:
      return (-2 *((fTSS[fISS[TMath::Min((Int_t) fNMC,(Int_t) TMath::Max((Int_t) 1,(Int_t) (fNMC * fgMCLP2S)))]]) - fStot));
   case -1:
      return (-2 *((fTSS[fISS[TMath::Min((Int_t) fNMC,(Int_t) TMath::Max((Int_t) 1,(Int_t) (fNMC * fgMCLP1S)))]]) - fStot));
   case 0:
      return (-2 *((fTSS[fISS[TMath::Min((Int_t) fNMC,(Int_t) TMath::Max((Int_t) 1,(Int_t) (fNMC * fgMCLMED)))]]) - fStot));
   case 1:
      return (-2 *((fTSS[fISS[TMath::Min((Int_t) fNMC,(Int_t) TMath::Max((Int_t) 1,(Int_t) (fNMC * fgMCLM1S)))]]) - fStot));
   case 2:
      return (-2 *((fTSS[fISS[TMath::Min((Int_t) fNMC,(Int_t) TMath::Max((Int_t) 1,(Int_t) (fNMC * fgMCLM2S)))]]) - fStot));
   default:
      return 0;
   }
}

Double_t TConfidenceLevel::CLb(bool use_sMC) const
{
   // Get the Confidence Level for the background only
   Double_t result = 0;
   switch (use_sMC) {
   case kFALSE:
      {
         for (Int_t i = 0; i < fNMC; i++)
            if (fTSB[fISB[i]] <= fTSD)
               result = (Double_t(i + 1)) / fNMC;
         return result;
      }
   case kTRUE:
      {
         for (Int_t i = 0; i < fNMC; i++)
            if (fTSS[fISS[i]] <= fTSD)
               result += (1 / (fLRS[fISS[i]] * fNMC));
         return result;
      }
   }
   return result;
}

Double_t TConfidenceLevel::CLsb(bool use_sMC) const
{
   // Get the Confidence Level for the signal plus background hypothesis
   Double_t result = 0;
   switch (use_sMC) {
   case kFALSE:
      {
         for (Int_t i = 0; i < fNMC; i++)
            if (fTSB[fISB[i]] <= fTSD)
               result += (fLRB[fISB[i]]) / fNMC;
         return result;
      }
   case kTRUE:
      {
         for (Int_t i = 0; i < fNMC; i++)
            if (fTSS[fISS[i]] <= fTSD)
               result = i / fNMC;
         return result;
      }
   }
   return result;
}

Double_t TConfidenceLevel::CLs(bool use_sMC) const
{
   // Get the Confidence Level defined by CLs = CLsb/CLb.
   // This quantity is stable w.r.t. background fluctuations.
   return CLsb(use_sMC) / CLb(kFALSE);
}

Double_t TConfidenceLevel::GetExpectedCLsb_b(Int_t sigma) const
{
   // Get the expected Confidence Level for the signal plus background hypothesis
   // if there is only background.
   Double_t result = 0;
   switch (sigma) {
   case -2:
      {
         for (Int_t i = 0; i < fNMC; i++)
            if (fTSB[fISB[i]] <= fTSB[fISB[TMath::Min((Int_t) fNMC,(Int_t) TMath::Max((Int_t) 1,(Int_t) (fNMC * fgMCLP2S)))]])
               result += fLRB[fISB[i]] / fNMC;
         return result;
      }
   case -1:
      {
         for (Int_t i = 0; i < fNMC; i++)
            if (fTSB[fISB[i]] <= fTSB[fISB[TMath::Min((Int_t) fNMC,(Int_t) TMath::Max((Int_t) 1,(Int_t) (fNMC * fgMCLP1S)))]])
               result += fLRB[fISB[i]] / fNMC;
         return result;
      }
   case 0:
      {
         for (Int_t i = 0; i < fNMC; i++)
            if (fTSB[fISB[i]] <= fTSB[fISB[TMath::Min((Int_t) fNMC,(Int_t) TMath::Max((Int_t) 1,(Int_t) (fNMC * fgMCLMED)))]])
               result += fLRB[fISB[i]] / fNMC;
         return result;
      }
   case 1:
      {
         for (Int_t i = 0; i < fNMC; i++)
            if (fTSB[fISB[i]] <= fTSB[fISB[TMath::Min((Int_t) fNMC,(Int_t) TMath::Max((Int_t) 1,(Int_t) (fNMC * fgMCLM1S)))]])
               result += fLRB[fISB[i]] / fNMC;
         return result;
      }
   case 2:
      {
         for (Int_t i = 0; i < fNMC; i++)
            if (fTSB[fISB[i]] <= fTSB[fISB[TMath::Min((Int_t) fNMC,(Int_t) TMath::Max((Int_t) 1,(Int_t) (fNMC * fgMCLM2S)))]])
               result += fLRB[fISB[i]] / fNMC;
         return result;
      }
   default:
      return 0;
   }
}

Double_t TConfidenceLevel::GetExpectedCLb_sb(Int_t sigma) const
{
   // Get the expected Confidence Level for the background only 
   // if there is signal and background.
   Double_t result = 0;
   switch (sigma) {
   case 2:
      {
         for (Int_t i = 0; i < fNMC; i++)
            if (fTSS[fISS[i]] <= fTSS[fISS[TMath::Min((Int_t) fNMC,(Int_t) TMath::Max((Int_t) 1,(Int_t) (fNMC * fgMCLP2S)))]])
               result += fLRS[fISS[i]] / fNMC;
         return result;
      }
   case 1:
      {
         for (Int_t i = 0; i < fNMC; i++)
            if (fTSS[fISS[i]] <= fTSS[fISS[TMath::Min((Int_t) fNMC,(Int_t) TMath::Max((Int_t) 1,(Int_t) (fNMC * fgMCLP1S)))]])
               result += fLRS[fISS[i]] / fNMC;
         return result;
      }
   case 0:
      {
         for (Int_t i = 0; i < fNMC; i++)
            if (fTSS[fISS[i]] <= fTSS[fISS[TMath::Min((Int_t) fNMC,(Int_t) TMath::Max((Int_t) 1,(Int_t) (fNMC * fgMCLMED)))]])
               result += fLRS[fISS[i]] / fNMC;
         return result;
      }
   case -1:
      {
         for (Int_t i = 0; i < fNMC; i++)
            if (fTSS[fISS[i]] <= fTSS[fISS[TMath::Min((Int_t) fNMC,(Int_t) TMath::Max((Int_t) 1,(Int_t) (fNMC * fgMCLM1S)))]])
               result += fLRS[fISS[i]] / fNMC;
         return result;
      }
   case -2:
      {
         for (Int_t i = 0; i < fNMC; i++)
            if (fTSS[fISS[i]] <= fTSS[fISS[TMath::Min((Int_t) fNMC,(Int_t) TMath::Max((Int_t) 1,(Int_t) (fNMC * fgMCLM2S)))]])
               result += fLRS[fISS[i]] / fNMC;
         return result;
      }
   default:
      return 0;
   }
}

Double_t TConfidenceLevel::GetExpectedCLb_b(Int_t sigma) const
{
   // Get the expected Confidence Level for the background only
   // if there is only background.
   Double_t result = 0;
   switch (sigma) {
   case 2:
      {
         for (Int_t i = 0; i < fNMC; i++)
            if (fTSB[fISB[i]] <= fTSB[fISB[TMath::Min((Int_t) fNMC,(Int_t) TMath::Max((Int_t) 1,(Int_t) (fNMC * fgMCLM2S)))]])
               result = (i + 1) / double (fNMC);
         return result;
      }
   case 1:
      {
         for (Int_t i = 0; i < fNMC; i++)
            if (fTSB[fISB[i]] <= fTSB[fISB[TMath::Min((Int_t) fNMC,(Int_t) TMath::Max((Int_t) 1,(Int_t) (fNMC * fgMCLM1S)))]])
               result = (i + 1) / double (fNMC);
         return result;
      }
   case 0:
      {
         for (Int_t i = 0; i < fNMC; i++)
            if (fTSB[fISB[i]] <= fTSB[fISB[TMath::Min((Int_t) fNMC,(Int_t) TMath::Max((Int_t) 1,(Int_t) (fNMC * fgMCLMED)))]])
               result = (i + 1) / double (fNMC);
         return result;
      }
   case -1:
      {
         for (Int_t i = 0; i < fNMC; i++)
            if (fTSB[fISB[i]] <= fTSB[fISB[TMath::Min((Int_t) fNMC,(Int_t) TMath::Max((Int_t) 1,(Int_t) (fNMC * fgMCLP1S)))]])
               result = (i + 1) / double (fNMC);
         return result;
      }
   case -2:
      {
         for (Int_t i = 0; i < fNMC; i++)
            if (fTSB[fISB[i]] <= fTSB[fISB[TMath::Min((Int_t) fNMC,(Int_t) TMath::Max((Int_t) 1,(Int_t) (fNMC * fgMCLP2S)))]])
               result = (i + 1) / double (fNMC);
         return result;
      }
   }
   return result;
}

Double_t TConfidenceLevel::GetAverageCLsb() const
{
   Double_t result = 0;
   Double_t psumsb = 0;
   for (Int_t i = 0; i < fNMC; i++) {
      psumsb += fLRB[fISB[i]] / fNMC;
      result += psumsb / fNMC;
   }
   return result;
}

Double_t TConfidenceLevel::GetAverageCLs() const
{
   Double_t result = 0;
   Double_t psumsb = 0;
   for (Int_t i = 0; i < fNMC; i++) {
      psumsb += fLRB[fISB[i]] / fNMC;
      result += ((psumsb / fNMC) / ((i + 1) / fNMC));
   }
   return result;
}

Double_t TConfidenceLevel::Get3sProbability() const
{
   Double_t result = 0;
   Double_t psumbs = 0;
   for (Int_t i = 0; i < fNMC; i++) {
      psumbs += 1 / (Double_t) (fLRS[(fISS[(Int_t) (fNMC - i)])] * fNMC);
      if (psumbs <= fMCL3S)
         result = i / fNMC;
   }
   return result;
}

Double_t TConfidenceLevel::Get5sProbability() const
{
   Double_t result = 0;
   Double_t psumbs = 0;
   for (Int_t i = 0; i < fNMC; i++) {
      psumbs += 1 / (Double_t) (fLRS[(fISS[(Int_t) (fNMC - i)])] * fNMC);
      if (psumbs <= fMCL5S)
         result = i / fNMC;
   }
   return result;
}

void  TConfidenceLevel::Draw(const Option_t*)
{
   // Display sort of a "canonical" -2lnQ plot.
   // This results in a plot with 2 elements: 
   // - The histogram of -2lnQ for background hypothesis (full)
   // - The histogram of -2lnQ for signal and background hypothesis (dashed)
   // The 2 histograms are respectively named b_hist and sb_hist.
   TH1F h("TConfidenceLevel_Draw","",50,0,0);
   Int_t i;
   for (i=0; i<fNMC; i++) {
      h.Fill(-2*(fTSB[i]-fStot));
      h.Fill(-2*(fTSS[i]-fStot));
   }
   TH1F* b_hist  = new TH1F("b_hist", "-2lnQ",50,h.GetXaxis()->GetXmin(),h.GetXaxis()->GetXmax());
   TH1F* sb_hist = new TH1F("sb_hist","-2lnQ",50,h.GetXaxis()->GetXmin(),h.GetXaxis()->GetXmax());
   for (i=0; i<fNMC; i++) {
      b_hist->Fill(-2*(fTSB[i]-fStot));
      sb_hist->Fill(-2*(fTSS[i]-fStot));
   }
   b_hist->Draw();
   sb_hist->Draw("Same");
   sb_hist->SetLineStyle(3);
}
