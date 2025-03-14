// @(#)root/hist:$Name:  $:$Id: TFormula.h,v 1.29 2005/04/29 20:34:51 brun Exp $
// Author: Nicolas Brun   19/08/95

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/
// ---------------------------------- Formula.h

#ifndef ROOT_TFormula
#define ROOT_TFormula



//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TFormula                                                             //
//                                                                      //
// The formula base class  f(x,y,z,par)                                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TNamed
#include "TNamed.h"
#endif
#ifndef ROOT_TBits
#include "TBits.h"
#endif
#ifndef ROOT_TObjArray
#include "TObjArray.h"
#endif

const Int_t kMAXFOUND = 200;
const Int_t kTFOperMask = 0x7fffff;
const UChar_t kTFOperShift = 23;

class TFormula : public TNamed {

protected:

   Int_t      fNdim;            //Dimension of function (1=1-Dim, 2=2-Dim,etc)
   Int_t      fNpar;            //Number of parameters
   Int_t      fNoper;           //Number of operators
   Int_t      fNconst;          //Number of constants
   Int_t      fNumber;          //formula number identifier
   Int_t      fNval;            //Number of different variables in expression
   Int_t      fNstring;         //Number of different constants character strings
   TString   *fExpr;            //[fNoper] List of expressions
private:
   Int_t     *fOper;            //[fNoper] List of operators. (See documentation for changes made at version 7)
protected:
   Double_t  *fConst;           //[fNconst] Array of fNconst formula constants
   Double_t  *fParams;          //[fNpar] Array of fNpar parameters
   TString   *fNames;           //[fNpar] Array of parameter names
   TObjArray  fFunctions;       //Array of function calls to make
   TObjArray  fLinearParts;     //! Linear parts if the formula is linear (contains '|')

   TBits      fAlreadyFound;    //! cache for information

   inline Int_t     *GetOper() const { return fOper; }
   inline Short_t    GetAction(Int_t code) const { return fOper[code] >> kTFOperShift; }
   inline Int_t      GetActionParam(Int_t code) const { return fOper[code] & kTFOperMask; }

   inline void       SetAction(Int_t code, Int_t value, Int_t param = 0) { 
      fOper[code]  = (value) << kTFOperShift; 
      fOper[code] += param;
   }

           void    ClearFormula(Option_t *option="");
   virtual Bool_t  IsString(Int_t oper) const;

   virtual void    Convert(UInt_t fromVersion);

   // Action code for Version 6 and above.
   enum {
      kEnd      = 0,
      kAdd      = 1, kSubstract = 2, 
      kMultiply = 3, kDivide    = 4,
      kModulo   = 5, 

      kcos      = 10, ksin  = 11 , ktan  = 12, 
      kacos     = 13, kasin = 14 , katan = 15, 
      katan2    = 16,
      kfmod     = 17, 

      kpow      = 20, ksq = 21, ksqrt     = 22, 

      kstrstr   = 23,

      kmin      = 24, kmax = 25,

      klog      = 30, kexp = 31, klog10 = 32,
      
      kpi     = 40,

      kabs    = 41 , ksign= 42, 
      kint    = 43 , 
      kSignInv= 44 ,
      krndm   = 50 ,

      kAnd      = 60, kOr          = 61,
      kEqual    = 62, kNotEqual    = 63,
      kLess     = 64, kGreater     = 65,
      kLessThan = 66, kGreaterThan = 67,
      kNot      = 68,

      kcosh   = 70 , ksinh  = 71, ktanh  = 72,
      kacosh  = 73 , kasinh = 74, katanh = 75,

      kStringEqual = 76, kStringNotEqual = 77,

      kBitAnd    = 78, kBitOr     = 79,
      kLeftShift = 80, kRightShift = 81,
      
      kexpo   = 100 , kxexpo   = 100, kyexpo   = 101, kzexpo   = 102, kxyexpo   = 105,
      kgaus   = 110 , kxgaus   = 110, kygaus   = 111, kzgaus   = 112, kxygaus   = 115,
      klandau = 120 , kxlandau = 120, kylandau = 121, kzlandau = 122, kxylandau = 125,
      kpol    = 130 , kxpol    = 130, kypol    = 131, kzpol    = 132,

      kParameter       = 140,
      kConstant     = 141,
      kBoolOptimize = 142,
      kStringConst     = 143,
      kVariable     = 144,
      kFunctionCall = 145,

      kDefinedVariable = 150,
      kDefinedString   = 151

   };

public:
   // TFormula status bits
   enum {
      kNotGlobal     = BIT(10),  // don't store in gROOT->GetListOfFunction
      kNormalized    = BIT(14),   // set to true if the function (ex gausn) is normalized
      kLinear        = BIT(16)    //set to true if the function is for linear fitting
   };
 
              TFormula();
              TFormula(const char *name,const char *formula);
              TFormula(const TFormula &formula);
   TFormula& operator=(const TFormula &rhs);
   virtual   ~TFormula();

 public:
   virtual void        Analyze(const char *schain, Int_t &err, Int_t offset=0);
   virtual Bool_t      AnalyzeFunction(TString &chaine, Int_t &err, Int_t offset=0);
   virtual Int_t       Compile(const char *expression="");
   virtual void        Copy(TObject &formula) const;
   virtual void        Clear(Option_t *option="");
   virtual char       *DefinedString(Int_t code);
   virtual Double_t    DefinedValue(Int_t code);
   virtual Int_t       DefinedVariable(TString &variable,Int_t &action);
   virtual Double_t    Eval(Double_t x, Double_t y=0, Double_t z=0, Double_t t=0) const;
   virtual Double_t    EvalPar(const Double_t *x, const Double_t *params=0);
   virtual const TObject *GetLinearPart(Int_t i);
   virtual Int_t       GetNdim() const {return fNdim;}
   virtual Int_t       GetNpar() const {return fNpar;}
   virtual Int_t       GetNumber() const {return fNumber;}
   virtual TString     GetExpFormula() const;
   Double_t            GetParameter(Int_t ipar) const;
   Double_t            GetParameter(const char *name) const;
   virtual Double_t   *GetParameters() const {return fParams;}
   virtual void        GetParameters(Double_t *params){for(Int_t i=0;i<fNpar;i++) params[i] = fParams[i];}
   virtual const char *GetParName(Int_t ipar) const;
   virtual Int_t       GetParNumber(const char *name) const;
   virtual Bool_t      IsLinear() {return TestBit(kLinear);}
   virtual Bool_t      IsNormalized() {return TestBit(kNormalized);}
   virtual void        Print(Option_t *option="") const; // *MENU*
   virtual void        ProcessLinear(TString &replaceformula);
   virtual void        SetNumber(Int_t number) {fNumber = number;}
   virtual void        SetParameter(const char *name, Double_t parvalue);
   virtual void        SetParameter(Int_t ipar, Double_t parvalue);
   virtual void        SetParameters(const Double_t *params);
   virtual void        SetParameters(Double_t p0,Double_t p1,Double_t p2=0,Double_t p3=0,Double_t p4=0,
                                     Double_t p5=0,Double_t p6=0,Double_t p7=0,Double_t p8=0,
                                     Double_t p9=0,Double_t p10=0); // *MENU*
   virtual void        SetParName(Int_t ipar, const char *name);
   virtual void        SetParNames(const char *name0="p0",const char *name1="p1",const char
                                   *name2="p2",const char *name3="p3",const char
                                   *name4="p4", const char *name5="p5",const char *name6="p6",const char *name7="p7",const char
                                   *name8="p8",const char *name9="p9",const char *name10="p10"); // *MENU*
   virtual void        Update() {;}

   ClassDef(TFormula,7)  //The formula base class  f(x,y,z,par)
};

#endif
