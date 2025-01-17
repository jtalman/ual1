#define GAMMA_FROZEN_SPIN 1.24810735
#define INJECTION_AMBIT   argv[3]

std::cout     << "#################################   Design Beam Orientation\n";
//double MPGeV   = 0.938;                          // fundamental kinematic parameter
//double EPGeV   = 0.03;                           // fundamental kinematic parameter
//double gamma0  = 1.003;                          // fundamental kinematic parameter
//double gamma0  = (MPGeV+EPGeV)/MPGeV;            // fundamental kinematic parameter
//double gamma0  = UAL::pFSG;                      // fundamental kinematic parameter
  double gamma0  = GAMMA_FROZEN_SPIN;              // fundamental kinematic parameter
double c       = 1.;                               // other units (mks) have 2.99792458e8 m/s
double b0      = sqrt(1.-1./gamma0/gamma0);        // equivalent fundamental kinematic parameter
double v0      = b0*c;                             // equivalent fundamental kinematic parameter

// $UAL/codes/PAC/src/PAC/Beam/BeamAttributes.hh   // # (index) of member variable
double m0      = UAL::pmass;                       // 2
double e0      = gamma0*m0;                        // 1
double p0      = gamma0*m0*v0;                     //

// double q0      = UAL::elemCharge;               // 3
double q0      = 1.0;                              // 3
double t0      = 0.;                               // 4
// double f0      = 1;                             // 5
// double f0      = 646617.830;//678910;//541426.7816;                      // 5
double M0      = 1.;                               // 6
double G0      = UAL::pG;                          // 7
double g0      = UAL::pg;                          // 7
double IA      = atof(INJECTION_AMBIT);            // (10) not used
double L0      = IA*p0;                            // 8
double El0     = +1.048270839000000e+07;//10.5e6;  // 9

  double pc_by_e = 0.938*1.25*0.6*0.3;
//double pc_by_e = 9.1e-31*1836*gamma0*b0*UAL::clight*UAL::clight/UAL::elemCharge/1e9;
//double pc_by_e = m0*gamma0*b0*UAL::clight*UAL::clight/UAL::elemCharge;
//               0.938 1.25 0.6 3e8   1e9         1e9

std::cout     << "#################################   Design Beam Orientation\n";
