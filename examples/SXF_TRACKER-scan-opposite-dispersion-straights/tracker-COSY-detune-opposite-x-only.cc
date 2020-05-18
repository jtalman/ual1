#include <iostream>
#include <fstream>
#include <iomanip>

#include <stdio.h>

#include "UAL/APDF/APDF_Builder.hh"
#include "PAC/Beam/Position.hh"
#include "SMF/PacSmf.h"
#include "PAC/Beam/Bunch.hh"
#include "Main/Teapot.h"
#include "UAL/UI/Shell.hh"

#include "PAC/Beam/Particle.hh"
#include "PAC/Beam/Spin.hh"

#include "UAL/SMF/AcceleratorNodeFinder.hh"

#include "timer.h"
#include "positionPrinter.hh"

using namespace UAL;

int main(int argc,char * argv[]){

  UAL::Shell shell;

  std::string variantName = "ring";

  // ************************************************************************
  std::cout << "\nDefine the space of Taylor maps." << std::endl;
  // ************************************************************************

  shell.setMapAttributes(UAL::Args() << UAL::Arg("order", 5));

  // ************************************************************************
  std::cout << "\nBuild lattice." << std::endl;
  // ************************************************************************

  std::string sxfFile = "./data/";
  sxfFile += variantName;
  sxfFile += ".sxf";

  shell.readSXF(UAL::Args() << UAL::Arg("file",  sxfFile.c_str()));

  // ************************************************************************
  std::cout << "\nAdd split ." << std::endl;
  // ************************************************************************

  
  shell.addSplit(UAL::Args() << UAL::Arg("lattice", "ring") << UAL::Arg("types", "Sbend")
		 << UAL::Arg("ir", 4));

  shell.addSplit(UAL::Args() << UAL::Arg("lattice", "ring") << UAL::Arg("types", "Quadrupole")
		 << UAL::Arg("ir", 4));
  

  // ************************************************************************
  std::cout << "Select lattice." << std::endl;
  // ************************************************************************

  shell.use(UAL::Args() << UAL::Arg("lattice", "ring"));

  // ************************************************************************
  std::cout << "\nWrite SXF file ." << std::endl;
  // ************************************************************************

  std::string outputFile = "./out/cpp/";
  outputFile += variantName;
  outputFile += ".sxf";

  shell.writeSXF(UAL::Args() << UAL::Arg("file",  outputFile.c_str()));

 PAC::BeamAttributes& ba = shell.getBeamAttributes();

// ************************************************************************
  std::cout << "\nDefine beam parameters.  You can use, from ../ual1/codes/UAL/src/UAL/Common" << std::endl;
double UALemass = UAL::emass;
double UALpmass = UAL::pmass;
std::cout << "UAL::emass  = " <<  UALemass << " GeV" << endl;
std::cout << "UAL::pmass  = " <<  UALpmass << " GeV" << endl;
// ************************************************************************

#include "setBeamAttributes.hh"

  shell.setBeamAttributes(UAL::Args() << UAL::Arg("mass", mass));
  shell.setBeamAttributes(UAL::Args() << UAL::Arg("energy", energy));
 // shell.setBeamAttributes(UAL::Args() << UAL::Arg("energy", energy) << UAL::Arg("mass", mass));
  shell.setBeamAttributes(UAL::Args() << UAL::Arg("freq", freq));

  // ************************************************************************
  std::cout << "\nLinear analysis." << std::endl;
  // ************************************************************************
  
  // Make linear matrix

  std::string mapFile = "./out/cpp/";
  mapFile += variantName;
  mapFile += ".map2";

  std::cout << " matrix" << std::endl;
  shell.map(UAL::Args() << UAL::Arg("order", 2) << UAL::Arg("print", mapFile.c_str()));

  // ************************************************************************
  std::cout << "\nTune and chromaticity fitting. " << std::endl;
  // ************************************************************************

  double tuneX = 3.644;
  double tuneY = 3.688; 
  double chromX = 0.0; 
   double chromY = 0.0;

   const char* b1f="^qu2$|^qu3$|^qu10$|^qu11$|^qu14$|^qu15$|^qu22$|^qu23$|^qu6$|^qu7$|^qu18$|^qu19$";
   const char* b1d="^qu1$|^qu4$|^qu5$|^qu8$|^qu9$|^qu12$|^qu13$|^qu16$|^qu17$|^qu20$|^qu21$|^qu24$";

  //  const char* b2f="^mx07$|^mx16$|^mx05$|^mx09$^mx14$|^mx18$";      //  MXG MXS 
  //  const char* b2d="^mx06$|^mx08$|^mx15$|^mx17$|";                                  //  MXL 

   const char* b2f="^mx10$";       //  MXS
   const char* b2d="^mx06$";      //  MXL, 

   // shell.tunefit(Args() << Arg("tunex", tuneX) << Arg("tuney", tuneY) << Arg("b1f") << Arg("b1d"));
   // shell.chromfit(Args() << Arg("chromx", chromX) << Arg("chromy", chromY)<< Arg("b2f") << Arg("b2d"));

  // ************************************************************************
  // Calculate twiss
  // ************************************************************************

  std::string twissFile = "./out/cpp/";
  twissFile += variantName;
  twissFile += ".twiss";

  std::cout << " twiss (ring)" << std::endl;

  shell.twiss(UAL::Args() << UAL::Arg("print", twissFile.c_str()));

  std::cout << " calculate suml" << std::endl;
  shell.analysis(UAL::Args());

  // ************************************************************************
  std::cout << "\nAlgorithm Part. " << std::endl;
  // ************************************************************************

  // std::string apdfFile = argv[1];
  std::string apdfFile = "data/sxf_tracker.apdf";

  UAL::APDF_Builder apBuilder;

  apBuilder.setBeamAttributes(ba);

  UAL::AcceleratorPropagator* ap = apBuilder.parse(apdfFile);

  if(ap == 0) {
    std::cout << "Accelerator Propagator has not been created " << std::endl;
    return 1;
  }

  std::cout << "\n SXF_TRACKER tracker, ";
  std::cout << "size : " << ap->getRootNode().size() << " propagators " << endl;

  // ************************************************************************
  std::cout << "\nBunch Part." << std::endl;
  // ************************************************************************

  ba.setG(0.85699);             // adds deuteron G factor 

  PAC::Bunch bunch(3);               // bunch with 3 particles
  bunch.setBeamAttributes(ba);

  PAC::Spin spin;
  spin.setSX(0.0);
  spin.setSY(0.0);
  spin.setSZ(1.0);

  double Del_d;

  double Delta_delta_e = 0.0001;   // fractional energy step size
  // so Delta_delta = 0.0001/beta = 0.0001/0.459 = 0.0002178;

  double x_nom = 0.00001;  // 0.005;      // nominal horizontal amplitude 
  double y_nom = 0.0;   // 0.00001;  // 0.005;      // nominal vertical amplitude 

// Dx_nom = 13.0,  Dxp_nom =  0.25; ->  <x> = -1300           -900   < x <  -1900   micron
// Dx_nom = 14.0,   Dxp_nom = 0.25; ->  <x> =  -1400         -1000  < x <  -1800   micron
// Dx_nom =       ,   Dxp_nom =        ; ->  <x> =                               < x <             micron

  double Dx_nom = 14.0;    
  double Dxp_nom = 0.25;   

  for(int ip=0; ip < bunch.size(); ip ++){
    Del_d = Delta_delta_e*(ip-1);
    bunch[ip].getPosition().set(x_nom+Dx_nom*Del_d ,   Dxp_nom*Del_d ,  y_nom , 0.0, 0.01, Del_d);
    bunch[ip].setSpin(spin);
  }

 // ************************************************************************
  std::cout << "\nTracking. " << std::endl;
  // ************************************************************************

 double t; // time variable

//int turns = 3000;
 int turns = 8192;
  int iturn;

  std::string orbitFile = "./out/cpp/";
  orbitFile += variantName;
  orbitFile += ".orbit";

  positionPrinter pP;
  // pP.open(orbitFile.c_str());
  pP.open( "NikolayOut" );

  ba.setElapsedTime(0.0);

  start_ms();

  for(int iturn = 1; iturn <= turns; iturn++){
       for(int ip=0; ip < bunch.size(); ip++){
            pP.write(iturn, ip, bunch);
       }
           ap -> propagate(bunch);
  }

  t = (end_ms());
  std::cout << "time  = " << t << " ms" << endl;

  pP.close();

  return 1;
}

