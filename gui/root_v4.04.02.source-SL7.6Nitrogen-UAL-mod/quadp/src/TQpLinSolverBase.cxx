// @(#)root/quadp:$Name:  $:$Id: TQpLinSolverBase.cxx,v 1.2 2004/05/24 12:45:40 brun Exp $
// Author: Eddy Offermann   May 2004

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

/*************************************************************************
 * Parts of this file are copied from the OOQP distribution and          *
 * are subject to the following license:                                 *
 *                                                                       *
 * COPYRIGHT 2001 UNIVERSITY OF CHICAGO                                  *
 *                                                                       *
 * The copyright holder hereby grants you royalty-free rights to use,    *
 * reproduce, prepare derivative works, and to redistribute this software*
 * to others, provided that any changes are clearly documented. This     *
 * software was authored by:                                             *
 *                                                                       *
 *   E. MICHAEL GERTZ      gertz@mcs.anl.gov                             *
 *   Mathematics and Computer Science Division                           *
 *   Argonne National Laboratory                                         *
 *   9700 S. Cass Avenue                                                 *
 *   Argonne, IL 60439-4844                                              *
 *                                                                       *
 *   STEPHEN J. WRIGHT     swright@cs.wisc.edu                           *
 *   Computer Sciences Department                                        *
 *   University of Wisconsin                                             *
 *   1210 West Dayton Street                                             *
 *   Madison, WI 53706   FAX: (608)262-9777                              *
 *                                                                       *
 * Any questions or comments may be directed to one of the authors.      *
 *                                                                       *
 * ARGONNE NATIONAL LABORATORY (ANL), WITH FACILITIES IN THE STATES OF   *
 * ILLINOIS AND IDAHO, IS OWNED BY THE UNITED STATES GOVERNMENT, AND     *
 * OPERATED BY THE UNIVERSITY OF CHICAGO UNDER PROVISION OF A CONTRACT   *
 * WITH THE DEPARTMENT OF ENERGY.                                        *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TQpLinSolverBase                                                     //
//                                                                      //
// Implementation of main solver for linear systems                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Riostream.h"
#include "TQpLinSolverBase.h"
#include "TMatrixD.h"

ClassImp(TQpLinSolverBase)

//______________________________________________________________________________
TQpLinSolverBase::TQpLinSolverBase()
{
  fNx   = 0;
  fMy   = 0;
  fMz   = 0;
  fNxup = 0;
  fNxlo = 0;
  fMcup = 0;
  fMclo = 0;
}

//______________________________________________________________________________
TQpLinSolverBase::TQpLinSolverBase(TQpProbBase *factory,TQpDataBase *data)
{
  fFactory = factory;

  fNx = data->fNx;
  fMy = data->fMy;
  fMz = data->fMz;

  fXloIndex.ResizeTo(data->fXloIndex); fXloIndex = data->fXloIndex;
  fXupIndex.ResizeTo(data->fXupIndex); fXupIndex = data->fXupIndex;
  fCloIndex.ResizeTo(data->fCloIndex); fCloIndex = data->fCloIndex;
  fCupIndex.ResizeTo(data->fCupIndex); fCupIndex = data->fCupIndex;

  fNxlo = fXloIndex.NonZeros();
  fNxup = fXupIndex.NonZeros();
  fMclo = fCloIndex.NonZeros();
  fMcup = fCupIndex.NonZeros();

  if (fNxup+fNxlo > 0) {
    fDd.ResizeTo(fNx);
    fDq.ResizeTo(fNx);
    data->GetDiagonalOfQ(fDq);
  }
  fNomegaInv.ResizeTo(fMz);
  fRhs      .ResizeTo(fNx+fMy+fMz);
}

//______________________________________________________________________________
TQpLinSolverBase::TQpLinSolverBase(const TQpLinSolverBase &another) : TObject(another)
{ 
  *this = another;
} 

//______________________________________________________________________________
void TQpLinSolverBase::Factor(TQpDataBase * /* prob */,TQpVar *vars)
{
  Assert(vars->ValidNonZeroPattern());

  if (fNxlo+fNxup > 0) {
    fDd.ResizeTo(fDq);
    fDd = fDq;
  }
  this->ComputeDiagonals(fDd,fNomegaInv,
			 vars->fT,vars->fLambda,vars->fU,vars->fPi,
			 vars->fV,vars->fGamma,vars->fW,vars->fPhi);
  if (fNxlo+fNxup > 0) this->PutXDiagonal(fDd);

  fNomegaInv.Invert();
  fNomegaInv *= -1.;

  if (fMclo+fMcup > 0) this->PutZDiagonal(fNomegaInv);
}

//______________________________________________________________________________
void TQpLinSolverBase::ComputeDiagonals(TVectorD &dd,TVectorD &omega,
                                        TVectorD &t, TVectorD &lambda,
                                        TVectorD &u, TVectorD &pi,
                                        TVectorD &v, TVectorD &gamma,
                                        TVectorD &w, TVectorD &phi)
{
  if (fNxup+fNxlo > 0) {
    if (fNxlo > 0) AddElemDiv(dd,1.0,gamma,v,fXloIndex);
    if (fNxup > 0) AddElemDiv(dd,1.0,phi  ,w,fXupIndex);
  }
  omega.Zero();
  if (fMclo > 0) AddElemDiv(omega,1.0,lambda,t,fCloIndex);
  if (fMcup > 0) AddElemDiv(omega,1.0,pi,    u,fCupIndex);
}

//______________________________________________________________________________
void TQpLinSolverBase::Solve(TQpDataBase *prob,TQpVar *vars,TQpResidual *res,TQpVar *step)
{
  Assert(vars->ValidNonZeroPattern());
  Assert(res ->ValidNonZeroPattern());
  
  (step->fX).ResizeTo(res->fRQ); step->fX = res->fRQ;
  if (fNxlo > 0) {
    TVectorD &vInvGamma = step->fV;
    vInvGamma.ResizeTo(vars->fGamma); vInvGamma = vars->fGamma;
    ElementDiv(vInvGamma,vars->fV,fXloIndex);
	
    AddElemMult(step->fX,1.0,vInvGamma,res->fRv);
    AddElemDiv (step->fX,1.0,res->fRgamma,vars->fV,fXloIndex);
  }

  if (fNxup > 0) {
    TVectorD &wInvPhi = step->fW;
    wInvPhi.ResizeTo(vars->fPhi); wInvPhi = vars->fPhi;
    ElementDiv(wInvPhi,vars->fW,fXupIndex);
	  
    AddElemMult(step->fX,1.0,wInvPhi,res->fRw);
    AddElemDiv (step->fX,-1.0,res->fRphi,vars->fW,fXupIndex);
  }

  // start by partially computing step->fS
  (step->fS).ResizeTo(res->fRz); step->fS = res->fRz;
  if (fMclo > 0) {
    TVectorD &tInvLambda = step->fT;
    tInvLambda.ResizeTo(vars->fLambda); tInvLambda = vars->fLambda;
    ElementDiv(tInvLambda,vars->fT,fCloIndex);

    AddElemMult(step->fS,1.0,tInvLambda,res->fRt);
    AddElemDiv (step->fS,1.0,res->fRlambda,vars->fT,fCloIndex);
  }

  if (fMcup > 0) {
    TVectorD &uInvPi = step->fU;
	
    uInvPi.ResizeTo(vars->fPi); uInvPi = vars->fPi;
    ElementDiv(uInvPi,vars->fU,fCupIndex);
	
    AddElemMult(step->fS,1.0,uInvPi,res->fRu);
    AddElemDiv (step->fS,-1.0,res->fRpi,vars->fU,fCupIndex);
  }

  (step->fY).ResizeTo(res->fRA); step->fY = res->fRA;
  (step->fZ).ResizeTo(res->fRC); step->fZ = res->fRC;

  if (fMclo > 0)
    this->SolveXYZS(step->fX,step->fY,step->fZ,step->fS,step->fLambda,prob);
  else
    this->SolveXYZS(step->fX,step->fY,step->fZ,step->fS,step->fPi,prob);

  if (fMclo > 0) {
    (step->fT).ResizeTo(step->fS); step->fT = step->fS;
    Add(step->fT,-1.0,res->fRt);
    (step->fT).SelectNonZeros(fCloIndex);

    (step->fLambda).ResizeTo(res->fRlambda); step->fLambda = res->fRlambda;
    AddElemMult(step->fLambda,-1.0,vars->fLambda,step->fT);
    ElementDiv(step->fLambda,vars->fT,fCloIndex);
  }

  if (fMcup > 0) {
    (step->fU).ResizeTo(res->fRu); step->fU = res->fRu;
    Add(step->fU,-1.0,step->fS);
    (step->fU).SelectNonZeros(fCupIndex);

    (step->fPi).ResizeTo(res->fRpi); step->fPi = res->fRpi;
    AddElemMult(step->fPi,-1.0,vars->fPi,step->fU);
    ElementDiv(step->fPi,vars->fU,fCupIndex);
  }

  if (fNxlo > 0) {
    (step->fV).ResizeTo(step->fX); step->fV = step->fX;
    Add(step->fV,-1.0,res->fRv);
    (step->fV).SelectNonZeros(fXloIndex);
	
    (step->fGamma).ResizeTo(res->fRgamma); step->fGamma = res->fRgamma;
    AddElemMult(step->fGamma,-1.0,vars->fGamma,step->fV);
    ElementDiv(step->fGamma,vars->fV,fXloIndex);
  }

  if (fNxup > 0) {
    (step->fW).ResizeTo(res->fRw); step->fW = res->fRw;
    Add(step->fW,-1.0,step->fX);
    (step->fW).SelectNonZeros(fXupIndex);
	
    (step->fPhi).ResizeTo(res->fRphi); step->fPhi = res->fRphi;
    AddElemMult(step->fPhi,-1.0,vars->fPhi,step->fW);
    ElementDiv(step->fPhi,vars->fW,fXupIndex);
  }
  Assert(step->ValidNonZeroPattern());
}

//______________________________________________________________________________
void TQpLinSolverBase::SolveXYZS(TVectorD &stepx,TVectorD &stepy,
                                 TVectorD &stepz,TVectorD &steps,
                                 TVectorD & /* ztemp */, TQpDataBase * /* prob */ )
{
  AddElemMult(stepz,-1.0,fNomegaInv,steps);
  this->JoinRHS(fRhs,stepx,stepy,stepz);

  this->SolveCompressed(fRhs);

  this->SeparateVars(stepx,stepy,stepz,fRhs);

  stepy *= -1.;
  stepz *= -1.;
	
  Add(steps,-1.0,stepz);
  ElementMult(steps,fNomegaInv);
  steps *= -1.;
}

//______________________________________________________________________________
void TQpLinSolverBase::JoinRHS(TVectorD &rhs_in, TVectorD &rhs1_in,
                               TVectorD &rhs2_in,TVectorD &rhs3_in)
{
  // joinRHS has to be delegated to the factory. This is true because
  // the rhs may be distributed across processors, so the factory is the
  // only object that knows with certainly how to scatter the elements.
  fFactory->JoinRHS(rhs_in,rhs1_in,rhs2_in,rhs3_in);
}

//______________________________________________________________________________
void TQpLinSolverBase::SeparateVars(TVectorD &x_in,TVectorD &y_in,
                                    TVectorD &z_in,TVectorD &vars_in)
{
  // separateVars has to be delegated to the factory. This is true because
  // the rhs may be distributed across processors, so the factory is the
  // only object that knows with certainly how to scatter the elements.
  fFactory->SeparateVars(x_in,y_in,z_in,vars_in);
}

//______________________________________________________________________________
TQpLinSolverBase &TQpLinSolverBase::operator=(const TQpLinSolverBase &source)
{
  if (this != &source) {
    TObject::operator=(source);

    fNx   = source.fNx;
    fMy   = source.fMy;
    fMz   = source.fMz;
    fNxup = source.fNxup;
    fNxlo = source.fNxlo;
    fMcup = source.fMcup;
    fMclo = source.fMclo;

    fNomegaInv.ResizeTo(source.fNomegaInv); fNomegaInv = source.fNomegaInv;
    fRhs      .ResizeTo(source.fRhs);       fRhs       = source.fRhs;

    fDd       .ResizeTo(source.fDd);        fDd        = source.fDd;
    fDq       .ResizeTo(source.fDq);        fDq        = source.fDq;

    fXupIndex .ResizeTo(source.fXupIndex);  fXupIndex  = source.fXupIndex;
    fCupIndex .ResizeTo(source.fCupIndex);  fCupIndex  = source.fCupIndex;
    fXloIndex .ResizeTo(source.fXloIndex);  fXloIndex  = source.fXloIndex;
    fCloIndex .ResizeTo(source.fCloIndex);  fCloIndex  = source.fCloIndex;
  }
  return *this;
}
