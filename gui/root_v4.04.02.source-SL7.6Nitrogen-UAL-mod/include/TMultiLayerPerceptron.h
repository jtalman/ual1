// @(#)root/mlp:$Name:  $:$Id: TMultiLayerPerceptron.h,v 1.8 2004/10/12 07:30:03 brun Exp $
// Author: Christophe.Delaere@cern.ch   20/07/03

/*************************************************************************
 * Copyright (C) 1995-2003, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TMultiLayerPerceptron
#define ROOT_TMultiLayerPerceptron

#ifndef ROOT_TObject
#include "TObject.h"
#endif
#ifndef ROOT_TString
#include "TString.h"
#endif
#ifndef ROOT_TObjArray
#include "TObjArray.h"
#endif
#ifndef ROOT_TMatrixD
#include "TMatrixD.h"
#endif

class TTree;
class TEventList;
class TTreeFormula;
class TTreeFormulaManager;

//____________________________________________________________________
//
// TMultiLayerPerceptron
//
// This class decribes a Neural network.
// There are facilities to train the network and use the output.
//
// The input layer is made of inactive neurons (returning the
// normalized input), hidden layers are made of sigmoids and output
// neurons are linear.
//
// The basic input is a TTree and two (training and test) TEventLists.
// For classification jobs, a branch (maybe in a TFriend) must contain
// the expected output.
// 6 learning methods are available: kStochastic, kBatch,
// kSteepestDescent, kRibierePolak, kFletcherReeves and kBFGS.
//
// This implementation is *inspired* from the mlpfit package from
// J.Schwindling et al.
//
//____________________________________________________________________

class TMultiLayerPerceptron : public TObject {
 friend class TMLPAnalyzer;

 public:
   enum LearningMethod { kStochastic, kBatch, kSteepestDescent,
                         kRibierePolak, kFletcherReeves, kBFGS };
   enum DataSet { kTraining, kTest };
   TMultiLayerPerceptron();
   TMultiLayerPerceptron(const char* layout, TTree* data = NULL,
                         const char* training = "Entry$%2==0",
                         const char* test = "");
   TMultiLayerPerceptron(const char* layout, const char* weight, TTree* data = NULL,
                         const char* training = "Entry$%2==0",
                         const char* test = "");
   TMultiLayerPerceptron(const char* layout, TTree* data,
                         TEventList* training,
                         TEventList* test);
   TMultiLayerPerceptron(const char* layout, const char* weight, TTree* data,
                         TEventList* training,
                         TEventList* test);
   virtual ~TMultiLayerPerceptron();
   void SetData(TTree*);
   void SetTrainingDataSet(TEventList* train);
   void SetTestDataSet(TEventList* test);
   void SetTrainingDataSet(const char* train);
   void SetTestDataSet(const char* test);
   void SetLearningMethod(TMultiLayerPerceptron::LearningMethod method);
   void SetEventWeight(const char*);
   void Train(Int_t nEpoch, Option_t* option = "text");
   Double_t Result(Int_t event, Int_t index = 0) const;
   Double_t GetError(Int_t event) const;
   Double_t GetError(TMultiLayerPerceptron::DataSet set) const;
   void ComputeDEDw() const;
   void Randomize() const;
   void SetEta(Double_t eta);
   void SetEpsilon(Double_t eps);
   void SetDelta(Double_t delta);
   void SetEtaDecay(Double_t ed);
   void SetTau(Double_t tau);
   void SetReset(Int_t reset);
   inline Double_t GetEta()      const { return fEta; }
   inline Double_t GetEpsilon()  const { return fEpsilon; }
   inline Double_t GetDelta()    const { return fDelta; }
   inline Double_t GetEtaDecay() const { return fEtaDecay; }
   inline Double_t GetTau()      const { return fTau; }
   inline Int_t GetReset()       const { return fReset; }
   inline TString GetStructure() const { return fStructure; }
   void DrawResult(Int_t index = 0, Option_t* option = "") const;
   void DumpWeights(Option_t* filename = "-") const;
   void LoadWeights(Option_t* filename = "");
   Double_t Evaluate(Int_t index, Double_t* params) const;
   void Export(Option_t* filename = "NNfunction", Option_t* language = "C++") const;
   virtual void Draw(Option_t *option="");

 protected:
   void AttachData();
   void BuildNetwork();
   void GetEntry(Int_t) const;
   // it's a choice not to force learning function being const, even if possible
   void MLP_Stochastic(Double_t*);
   void MLP_Batch(Double_t*);
   Bool_t LineSearch(Double_t*, Double_t*);
   void SteepestDir(Double_t*);
   void ConjugateGradientsDir(Double_t*, Double_t);
   void SetGammaDelta(TMatrixD&, TMatrixD&, Double_t*);
   bool GetBFGSH(TMatrixD&, TMatrixD &, TMatrixD&);
   void BFGSDir(TMatrixD&, Double_t*);
   Double_t DerivDir(Double_t*);

 private:
   void BuildFirstLayer(TString&);
   void BuildHiddenLayers(TString&);
   void BuildLastLayer(TString&, Int_t);
   void Shuffle(Int_t*, Int_t) const;
   void MLP_Line(Double_t*, Double_t*, Double_t);

   TTree* fData;                   //! pointer to the tree used as datasource
   Int_t fCurrentTree;             //! index of the current tree in a chain
   Double_t fCurrentTreeWeight;    //! weight of the current tree in a chain
   TObjArray fNetwork;             // Collection of all the neurons in the network
   TObjArray fFirstLayer;          // Collection of the input neurons; subset of fNetwork
   TObjArray fLastLayer;           // Collection of the output neurons; subset of fNetwork
   TObjArray fSynapses;            // Collection of all the synapses in the network
   TString fStructure;             // String containing the network structure
   TString fWeight;                // String containing the event weight
   TEventList *fTraining;          //! EventList defining the events in the training dataset
   TEventList *fTest;              //! EventList defining the events in the test dataset
   LearningMethod fLearningMethod; //! The Learning Method
   TTreeFormula* fEventWeight;     //! formula representing the event weight
   TTreeFormulaManager* fManager;  //! TTreeFormulaManager for the weight and neurons
   Double_t fEta;                  //! Eta - used in stochastic minimisation - Default=0.1
   Double_t fEpsilon;              //! Epsilon - used in stochastic minimisation - Default=0.
   Double_t fDelta;                //! Delta - used in stochastic minimisation - Default=0.
   Double_t fEtaDecay;             //! EtaDecay - Eta *= EtaDecay at each epoch - Default=1.
   Double_t fTau;                  //! Tau - used in line search - Default=3.
   Double_t fLastAlpha;            //! internal parameter used in line search
   Int_t fReset;                   //! number of epochs between two resets of the search direction to the steepest descent - Default=50
   Bool_t fTrainingOwner;          //! internal flag whether one has to delete fTraining or not
   Bool_t fTestOwner;              //! internal flag whether one has to delete fTest or not
   ClassDef(TMultiLayerPerceptron, 2)	// a Neural Network
};

#endif
