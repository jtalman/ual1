// @(#)root/hist:$Name:  $:$Id: THStack.h,v 1.8 2004/05/10 07:33:51 brun Exp $
// Author: Rene Brun   10/12/2001

/*************************************************************************
 * Copyright (C) 1995-2001, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_THStack
#define ROOT_THStack


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// THStack                                                              //
//                                                                      //
// A collection of histograms                                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TH1
#include "TH1.h"
#endif
#ifndef ROOT_TObjArray
#include "TObjArray.h"
#endif


class TBrowser;

class THStack : public TNamed {

protected:
    TList      *fHists;      //Pointer to array of TH1
    TObjArray  *fStack;      //!Pointer to array of sums of TH1
    TH1        *fHistogram;  //!Pointer to histogram used for drawing axis
    Double_t    fMaximum;    //Maximum value for plotting along y
    Double_t    fMinimum;    //Minimum value for plotting along y

    void BuildStack();

public:

        THStack();
        THStack(const char *name, const char *title);
        THStack(const TH1* hist, Option_t *axis="x", 
                const char *name=0, const char *title=0,
                Int_t firstbin=-1, Int_t lastbin=-1, 
                Int_t firstbin2=-1, Int_t lastbin2=-1, 
                Option_t* proj_option="", Option_t* draw_option="");
        THStack(const THStack &hstack);
        virtual ~THStack();
        virtual void     Add(TH1 *h, Option_t *option="");
        virtual void     Browse(TBrowser *b);
        virtual Int_t    DistancetoPrimitive(Int_t px, Int_t py);
        virtual void     Draw(Option_t *chopt="");
        TH1             *GetHistogram() const;
        TList           *GetHists()  const { return fHists; }
        TObjArray       *GetStack();
        virtual Double_t GetMaximum(Option_t *option="");
        virtual Double_t GetMinimum(Option_t *option="");
        TAxis           *GetXaxis() const;
        TAxis           *GetYaxis() const;
        virtual void     ls(Option_t *option="") const;
        virtual void     Modified();
        virtual void     Paint(Option_t *chopt="");
        virtual void     Print(Option_t *chopt="") const;
        virtual void     RecursiveRemove(TObject *obj);
        virtual void     SavePrimitive(ofstream &out, Option_t *option);
        virtual void     SetMaximum(Double_t maximum=-1111); // *MENU*
        virtual void     SetMinimum(Double_t minimum=-1111); // *MENU*

    ClassDef(THStack,1)  //A collection of histograms
};

#endif

