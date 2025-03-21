// @(#)root/proof:$Name:  $:$Id: TProofDraw.h,v 1.12 2005/04/06 15:56:14 brun Exp $
// Author: Maarten Ballintijn   24/09/2003

#ifndef ROOT_TProofDraw
#define ROOT_TProofDraw


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TProofDraw                                                           //
//                                                                      //
// Implement Tree drawing using PROOF.                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef ROOT_TSelector
#include "TSelector.h"
#endif

#ifndef ROOT_TString
#include "TString.h"
#endif

#ifndef ROOT_TTreeDrawArgsParser
#include "TTreeDrawArgsParser.h"
#endif

#ifndef ROOT_TNamed
#include "TNamed.h"
#endif

#include <vector>


class TTree;
class TTreeFormulaManager;
class TTreeFormula;
class TStatus;
class TH1;
class TEventList;
class TProfile;
class TProfile2D;
class TGraph;
class TPolyMarker3D;
class TCollection;


class TProofDraw : public TSelector {

protected:
   TTreeDrawArgsParser  fTreeDrawArgsParser;
   TStatus             *fStatus;
   TString              fSelection;
   TString              fInitialExp;
   TTreeFormulaManager *fManager;
   TTree               *fTree;
   TTreeFormula        *fVar[4];         //  Pointer to variable formula
   TTreeFormula        *fSelect;         //  Pointer to selection formula
   Int_t                fMultiplicity;   //  Indicator of the variability of the size of entries
   Bool_t               fObjEval;        //  true if fVar1 returns an object (or pointer to).
   Int_t                fDimension;      //  Dimension of the current expression

   void     SetError(const char *sub, const char *mesg);

protected:
   enum { kWarn = BIT(12) };

   virtual Bool_t      CompileVariables();
   virtual void        ClearFormula();
   virtual Bool_t      ProcessSingle(Long64_t /*entry*/, Int_t /*i*/);
   virtual void        DoFill(Long64_t entry, Double_t w, const Double_t *v) = 0;

public:
   TProofDraw();
   virtual            ~TProofDraw();
   virtual int         Version() const { return 1; }
   virtual void        Init(TTree *);
   virtual void        Begin(TTree *);
   virtual void        SlaveBegin(TTree *);
   virtual Bool_t      Notify();
   virtual Bool_t      Process(Long64_t /*entry*/);
   virtual void        SlaveTerminate();
   virtual void        Terminate();

   ClassDef(TProofDraw,0)  //Tree drawing selector for PROOF
};


class TProofDrawHist : public TProofDraw {

protected:
   TH1                *fHistogram;

   virtual void        Begin1D(TTree *t);
   virtual void        Begin2D(TTree *t);
   virtual void        Begin3D(TTree *t);
   virtual void        DoFill(Long64_t entry, Double_t w, const Double_t *v);

public:
   TProofDrawHist() : fHistogram(0) { }
   virtual void        Begin(TTree *t);
   virtual void        Init(TTree *);
   virtual void        SlaveBegin(TTree *);
   virtual void        Terminate();

   ClassDef(TProofDrawHist,0)  //Tree drawing selector for PROOF
};


class TProofDrawEventList : public TProofDraw {

protected:
   TEventList*    fElist;          //  event list
   TList*         fEventLists;     //  a list of EventLists

   virtual void   DoFill(Long64_t entry, Double_t w, const Double_t *v);

public:
   TProofDrawEventList() : fElist(0), fEventLists(0) {}
   ~TProofDrawEventList();

   virtual void        Init(TTree *);
   virtual void        SlaveBegin(TTree *);
   virtual void        SlaveTerminate();
   virtual void        Terminate();

   ClassDef(TProofDrawEventList,0)  //Tree drawing selector for PROOF
};


class TProofDrawProfile : public TProofDraw {

protected:
   TProfile           *fProfile;

   virtual void        DoFill(Long64_t entry, Double_t w, const Double_t *v);

public:
   TProofDrawProfile() : fProfile(0) { }
   virtual void        Init(TTree *);
   virtual void        Begin(TTree *t);
   virtual void        SlaveBegin(TTree *);
   virtual void        Terminate();

   ClassDef(TProofDrawProfile,0)  //Tree drawing selector for PROOF
};


class TProofDrawProfile2D : public TProofDraw {

protected:
   TProfile2D         *fProfile;

   virtual void        DoFill(Long64_t entry, Double_t w, const Double_t *v);

public:
   TProofDrawProfile2D() : fProfile(0) { }
   virtual void        Init(TTree *);
   virtual void        Begin(TTree *t);
   virtual void        SlaveBegin(TTree *);
   virtual void        Terminate();

   ClassDef(TProofDrawProfile2D,0)  //Tree drawing selector for PROOF
};


class TProofDrawGraph : public TProofDraw {

protected:
   TGraph             *fGraph;

   virtual void        DoFill(Long64_t entry, Double_t w, const Double_t *v);

public:
   TProofDrawGraph() : fGraph(0) { }
   virtual void        Init(TTree *tree);
   virtual void        SlaveBegin(TTree *);
   virtual void        Terminate();

   ClassDef(TProofDrawGraph,0)  //Tree drawing selector for PROOF
};


class TProofDrawPolyMarker3D : public TProofDraw {

protected:
   TPolyMarker3D      *fPolyMarker3D;

   virtual void        DoFill(Long64_t entry, Double_t w, const Double_t *v);

public:
   TProofDrawPolyMarker3D() : fPolyMarker3D(0) { }
   virtual void        Init(TTree *tree);
   virtual void        SlaveBegin(TTree *);
   virtual void        Terminate();

   ClassDef(TProofDrawPolyMarker3D,0)  //Tree drawing selector for PROOF
};

template <typename T>
class TProofVectorContainer : public TNamed {
   // Owns an std::vector<T>.
   // Implements Merge(TCollection*) which merges vectors holded
   // by all the TProofVectorContainers in the collection.
protected:
   std::vector<T> *fVector;   // vector

public:
   TProofVectorContainer<T>(std::vector<T>* anVector) : fVector(anVector) { };
   TProofVectorContainer<T>() : fVector(0) { };
   ~TProofVectorContainer<T>() { delete fVector; }

   std::vector<T> *GetVector() const { return fVector; }
   Long64_t        Merge(TCollection* list);

   ClassDef(TProofVectorContainer,1)
};

class TProofDrawListOfGraphs : public TProofDraw {

public:
   struct Point3D_t {
   public:
      Double_t fX, fY, fZ;
      Point3D_t(Double_t x, Double_t y, Double_t z) : fX(x), fY(y), fZ(z) { }
      Point3D_t() : fX(0), fY(0), fZ(0) { }
   };

protected:
   TProofVectorContainer<Point3D_t> *fPoints;
   virtual void        DoFill(Long64_t entry, Double_t w, const Double_t *v);

public:
   TProofDrawListOfGraphs() : fPoints(0) { }
   virtual void        SlaveBegin(TTree *);
   virtual void        Terminate();

   ClassDef(TProofDrawListOfGraphs,0)  //Tree drawing selector for PROOF
};


class TProofDrawListOfPolyMarkers3D : public TProofDraw {

public:
   struct Point4D_t {
   public:
      Double_t fX, fY, fZ, fT;
      Point4D_t(Double_t x, Double_t y, Double_t z, Double_t t) : fX(x), fY(y), fZ(z), fT(t) { }
      Point4D_t() : fX(0), fY(0), fZ(0), fT(0) { }
   };

protected:
   TProofVectorContainer<Point4D_t> *fPoints;
   virtual void        DoFill(Long64_t entry, Double_t w, const Double_t *v);

public:
   TProofDrawListOfPolyMarkers3D() : fPoints(0) { }
   virtual void        SlaveBegin(TTree *);
   virtual void        Terminate();

   ClassDef(TProofDrawListOfPolyMarkers3D,0)  //Tree drawing selector for PROOF
};

#ifndef __CINT__
template <typename T>
Long64_t TProofVectorContainer<T>::Merge(TCollection* list)
{
   // Adds all vectors holded by all TProofVectorContainers in the collection
   // the vector holded by this TProofVectorContainer.
   // Returns the total number of poins in the result or -1 in case of an error.

   TIter next(list);

   std::back_insert_iterator<std::vector<T> > ii(*fVector);
   while (TObject* o = next()) {
      TProofVectorContainer<T> *vh = dynamic_cast<TProofVectorContainer<T>*> (o);
      if (!vh) {
         Error("Merge",
             "Cannot merge - an object which doesn't inherit from TProofVectorContainer<T> found in the list");
         return -1;
      }
      std::copy(vh->GetVector()->begin(), vh->GetVector()->end(), ii);
   }
   return fVector->size();
}
#endif

#endif
