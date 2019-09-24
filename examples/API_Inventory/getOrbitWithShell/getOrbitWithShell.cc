#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>

#include <algorithm>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <gsl/gsl_math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#include "UAL/APDF/APDF_Builder.hh"
#include "PAC/Beam/Position.hh"
#include "PAC/Beam/Bunch.hh"
#include "UAL/UI/Shell.hh"

#include "PAC/Beam/Spin.hh"

#include "ETEAPOT2/Integrator/bend.cc"
#include "ETEAPOT2/Integrator/quad.cc"

#include "ETEAPOT2/Integrator/genMethods/spinExtern"
#include "ETEAPOT2/Integrator/genMethods/spinDef"
#include "ETEAPOT2/Integrator/genMethods/designExtern"
#include "ETEAPOT2/Integrator/genMethods/designDef"
#include "ETEAPOT2/Integrator/genMethods/bunchParticleExtern"
#include "ETEAPOT2/Integrator/genMethods/bunchParticleDef"

using namespace UAL;

int main(int argc,char * argv[]){
#include "include/getArgs"
#include "include/setStatic"
#include "userManifest/designBeamValues.hh"
#include "userManifest/extractParameters.h"
#include "userManifest/probeDataForTwiss"
#include "userManifest/userBunch"

 double f0=atof(argv[2]);

 UAL::Shell shell;

 mDcc=m0;
 qD=q0;
 betaD=b0;
 vD=betaD*UAL::clight;
 gammaD=gamma0;
 eD=e0;
 pDc=p0;
 fD=f0;
 tofDT=0;

 int spltBndsPrBend=pow(2,splitForBends);
 shell.setMapAttributes(UAL::Args() << UAL::Arg("order", order));

   shell.readSXF( UAL::Args() << UAL::Arg( "file",  "./working.sxf" ) );
// shell.readSXF(UAL::Args() << UAL::Arg("file",  sxfFile.c_str()));

 shell.addSplit(UAL::Args() << UAL::Arg("lattice", "ring") << UAL::Arg("types", "Sbend")      << UAL::Arg("ir", split-1));  // JDT 7/18/2012 new split specification
 shell.addSplit(UAL::Args() << UAL::Arg("lattice", "ring") << UAL::Arg("types", "Quadrupole") << UAL::Arg("ir", 0));
 shell.addSplit(UAL::Args() << UAL::Arg("lattice", "ring") << UAL::Arg("types", "Sextupole")  << UAL::Arg("ir", 0));

 shell.use(UAL::Args() << UAL::Arg("lattice", "ring"));

 UAL::APDF_Builder apBuilder;

 UAL::AcceleratorPropagator* ap = apBuilder.parse(apdfFile);

 if(ap == 0) {
   std::cout << "Accelerator Propagator has not been created " << std::endl;
   return 1;
 }

// std::cout << "size : " << ap->getRootNode().size() << " propagators " << endl;

 UAL::PropagatorSequence& apSeq = ap->getRootNode();

 int counter = 0;
 std::list<UAL::PropagatorNodePtr>::iterator it;
 for(it = apSeq.begin(); it != apSeq.end(); it++){
//std::cout << counter++ << " (*it)->getType() " << (*it)->getType() << std::endl;
 }

 turns=atoi( argv[5] );
// #include"userManifest/S"
// #include"userManifest/spin"

 decFac=atoi(argv[7]);
 for(int iturn = 0; iturn <= (turns-1); iturn++){
  ap -> propagate(bunch);
 }
 return 0;
}
