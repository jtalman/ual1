
double pc_tmp = 0.97;
double mass   =1.8756 ; // deuteron rest mass
double energy = sqrt(mass*mass + pc_tmp*pc_tmp);
double beta_tmp = pc_tmp/energy;
double circum = 183.4727;
double freq = beta_tmp/circum*UAL::clight;

double e0=energy;
double m0=mass;
double q0=1;
double t0=0;
double f0=freq;
double M0=1;
double G0=1;
double g0=1;

ba.setEnergy(e0);
ba.setMass(m0);
ba.setCharge(q0);
ba.setElapsedTime(t0);

ba.setRevfreq(f0);

ba.setMacrosize(M0);

ba.setG(G0);
ba.set_g(g0);

//shell.setBeamAttributes("designAngularMomentum",L0));
//shell.setBeamAttributes("designElectricField",  El0));
//shell.setBeamAttributes("designRadius",         IA));

std::cout << "ba.getRevfreq() " << ba.getRevfreq() << "\n";
