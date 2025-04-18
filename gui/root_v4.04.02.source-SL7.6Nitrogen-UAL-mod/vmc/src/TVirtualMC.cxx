// @(#)root/mc:$Name:  $:$Id: TVirtualMC.cxx,v 1.5 2005/02/08 11:20:15 brun Exp $
// Authors: Ivana Hrivnacova, Rene Brun , Federico Carminati 13/04/2002
   
/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 **************************************************************************/

#include "TVirtualMC.h"

//______________________________________________________________________________
//   Virtual MC provides a virtual interface to Monte Carlo. 
//   It enables the user to build a virtual Monte Carlo application
//   independent of any actual underlying Monte Carlo implementation itself.
//
//   A user will have to implement a class derived from the abstract 
//   Monte Carlo application class, and provide functions like
//   ConstructGeometry(), BeginEvent(), FinishEvent(), ... . 
//   The concrete Monte Carlo (Geant3, Geant4) is selected at run time -
//   when processing a ROOT macro where the concrete Monte Carlo is instantiated.
//______________________________________________________________________________

ClassImp(TVirtualMC)

TVirtualMC* TVirtualMC::fgMC=0;
TVirtualMC* gMC;

//_____________________________________________________________________________
TVirtualMC::TVirtualMC(const char *name, const char *title,
                       Bool_t /*isRootGeometrySupported*/) 
  : TNamed(name,title),
    fStack(0),
    fDecayer(0),
    fRandom(0)
{
  //
  // Standard constructor
  //
  if(fgMC) {
    Warning("TVirtualMC","Cannot initialise twice MonteCarlo class");
  } else {
    fgMC=this;
    gMC=this;
 
    fApplication = TVirtualMCApplication::Instance();  
  
    if (!fApplication) {
      Error("TVirtualMC", "No user MC application is defined.");
    }
    
    fRandom = gRandom;
  }
}

//_____________________________________________________________________________
TVirtualMC::TVirtualMC()
  : TNamed(),
    fApplication(0),
    fStack(0),
    fDecayer(0),
    fRandom(0)
{    
  //
  // Default constructor
  //
}

//_____________________________________________________________________________
TVirtualMC::~TVirtualMC() 
{
  //
  // Destructor
  //
  fgMC=0;
  gMC=0;
}

//
// methods
//

//_____________________________________________________________________________
void TVirtualMC::SetStack(TVirtualMCStack* stack) 
{ 
// 
// Set particles stack.
//

  fStack = stack; 
}

//_____________________________________________________________________________
void TVirtualMC::SetExternalDecayer(TVirtualMCDecayer* decayer) 
{ 
// 
// Set external decayer.
//

  fDecayer = decayer;
}

//_____________________________________________________________________________
void TVirtualMC::SetRandom(TRandom* random) 
{ 
// 
// Set random number generator.
//
   gRandom = random;
   fRandom = random; 
}
