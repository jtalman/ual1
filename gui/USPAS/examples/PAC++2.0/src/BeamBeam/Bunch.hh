#ifndef BUNCH_H
#define BUNCH_H

#include "Monitor/Particle.hh"

class Bunch
{
public:

  // Constructors & destructor
  Bunch() {number = 0; pls = 0;}
  Bunch(double* parameters, int number, Particle& offset);
  ~Bunch(){ if(pls) delete [] pls;}

  // Access function

  int size() { return number; }
  Particle& operator[] (int index);

protected:

  int       number;
  Particle* pls;

private:


};

#endif