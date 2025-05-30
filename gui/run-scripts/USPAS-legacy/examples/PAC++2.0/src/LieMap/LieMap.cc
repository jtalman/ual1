// LieMap.cc

#include "LieMap/LieMap.hh"

// Static members

int     LieMap::numberTmpLieMap = 0;
int     LieMap::lastTmpLieMap = 0;

// ---------   Constructors & destructor  ----------

LieMap::LieMap()
{
     body       = 0.;
     parameters = 0;
     prmOrder   = LIEMAP_ORDER;
     tmpOrder   = 0;
     whoAreYou  = 0;
}

LieMap::LieMap(Series& extBody)
{
     body       = extBody;
     parameters = 0;
     prmOrder   = LIEMAP_ORDER;
     tmpOrder   = 0;
     whoAreYou  = 0;
}

LieMap::LieMap(LieMap& l)
{
     body      = l.body;
     createParameters(l.parameters);
     if(l.tmpOrder > 0){
      prmOrder   = l.tmpOrder;
      l.tmpOrder = 0;
     }
     tmpOrder  = 0;
     whoAreYou = 0;
}

LieMap::LieMap(void (*vectorPotential)(Map y, Series* vectorPotential,
       double* param), double* extParameters)
{
    createBody(vectorPotential, extParameters);
    tmpOrder  = 0;
    whoAreYou = 0;
}

LieMap::~LieMap()
{
   deleteParameters();
}

// Assignment operators

void LieMap::operator=(Series& extBody)
{
   body = extBody;
   deleteParameters();
}

void LieMap::operator=(LieMap& l)
{
   body     = l.body;
   createParameters(l.parameters);
   if(l.tmpOrder > 0){
     prmOrder = l.tmpOrder;
     l.tmpOrder = 0;
   }
   l.deleteTmpLieMap();
}

// Access Operators

LieMap& LieMap::operator()(int order)
{
  tmpOrder = order;
  return(*this);
}

LieMap& LieMap::operator()(void (*vectorPotential)(Map y, Series* vectorPotential,
       double* param), double* extParameters, int order)
{
  tmpOrder = order;
  createBody(vectorPotential, extParameters);
  return(*this);
}


// Additive & Multiplicative Operators

Series& LieMap::operator*(Series& g)
{
   Series tmpF, sum;
   int order0;
   int order1 = body.order();
   int order2 = g.order();

   if(order1 == 0 || order2 == 0) return(g);

   if(order1  == 1) order0 = order2;
   if(order1  == 2) order0 = prmOrder;
   if(order1   > 2) order0 = (body.maxOrder()-order2)/(order1-2);
   if(tmpOrder > 0) order0 = tmpOrder;

   tmpF = g;
   sum  = g;
   for(int i=1; i <= order0; i++){
      tmpF  = poisson(body,tmpF)/i;
      sum  += tmpF;
   }
   return(sum*1.);
}

Map& LieMap::operator*(Map& vf)
{
   Map sum;
   for(int i=1; i<= vf.dimension(); i++)
     sum[i] = (*this)*vf[i];
   return(sum*1.);
}

// Auxiliary Function

void LieMap::deleteTmpLieMap()
{
}

void LieMap::createParameters(double* extParameters)
{
    deleteParameters();
    if(!extParameters) return;

    int number = (int) extParameters[0];

    if(number <= 0){ 
        cerr << "LieMap::LieMap : parameters[0] <= 0 "
             << "- number of parameters \n";
        exit(1);
    }

    parameters = new double[number+1];
    for(int i=0; i <= number; i++)
        parameters[i] = extParameters[i];
}

void LieMap::deleteParameters()
{
    if(!parameters) return;
    delete parameters;
    parameters = 0;
}

void LieMap::createBody(void (*vectorPotential)(Map y, Series* vectorPotential,
       double* param), double* extParameters)
{
    Series a[4], tmp;
    Map y; y = 1.;
    int dim = y.dimension();

    createParameters(extParameters);

    (*vectorPotential)(y, a, parameters);
    tmp = y[2] - a[1];
    body = 1. - tmp*tmp;
    if(dim > 2) {
      tmp   = y[4] - a[2];
      body -= tmp*tmp;
    }
    if(dim > 4) {
      body += y[6]*y[6];
      body += (2./v0byc)*y[6];
    }
    body  = sqrt(body)*(1.+a[0]*y[1]);
    body += a[3];
    if(dim > 4) body -= y[6]/v0byc;
    body *= parameters[1];
}

