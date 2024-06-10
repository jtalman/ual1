#include<iostream>
#include<iomanip>
#include<fstream>
#include<string>
#include<cstdlib>

#include<typeinfo> 

#include"SMF/PacLattice.h"
#include"SMF/PacElemRfCavity.h"

#include"ETEAPOT/Integrator/DipoleData.hh"
#include"ETEAPOT/Integrator/MltData.hh"

//#include "ETEAPOT2/Integrator/BasicTracker.hh"
#include"UAL/APF/PropagatorComponent.hh"

#include"UAL/Common/Def.hh"
#include"SMF/PacLattElement.h"
#include"PAC/Beam/Position.hh"
#include"SMF/PacElemAperture.h"
#include"SMF/PacElemOffset.h"

#include"ETEAPOT2/Integrator/genMethods/Matrices.hh"
#include"ETEAPOT2/Integrator/genMethods/Vectors.h"
#include"ETEAPOT2/Integrator/genMethods/spinExtern"
#include"ETEAPOT2/Integrator/genMethods/designExtern"
#include"ETEAPOT2/Integrator/genMethods/bunchParticleExtern"

namespace E_m{

  class compromise_quad : public UAL::PropagatorNode {
//class quad : public ETEAPOT::BasicTracker {

  public:
const char*  getType(){
  return "JDT:::E_m::compromise_quad";
}

#include"ETEAPOT2/Integrator/bendMethods/classGlobals"
#include"quadMethods/classMethods"

   inline ETEAPOT::MltData& getMltData();

#include "quadMethods/propagate.method"
#include "ETEAPOT2/Integrator/genMethods/get_vlcyMKS.method"
#include "ETEAPOT2/Integrator/genMethods/passDrift.method"

    /** Complexity number */
    double m_ir;

    double m_q;

/*
    double m_D_MinusTwo;      //   CW!
    double m_D_MinusOne;      //   CW!
    double m_D_PlusOne;       //   CW!
    double m_D_PlusTwo;       //   CW!

    double m_q_MinusOne;      //   CW!
    double m_q_Zero;          //   CW!
    double m_q_PlusOne;       //   CW!

    double m_l_MinusOne;      //   CW!
    double m_l_Zero;          //   CW!
    double m_l_PlusOne;       //   CW!
*/

    double m_ehd;             //    entry half drift;
    double m_mhd;             //   middle half drift;

    double m_el;              //    entry length;
    double m_ml;              //   middle length;

    double m_eq;              //    entry strength;
    double m_mq;              //   middle strength;

    double m_L;               //   
                              //  
    //m_L=m_ehd+m_el+m_ehd+ m_mhd+m_ml+m_mhd+ m_ehd+m_el+m_ehd;

  };

}
