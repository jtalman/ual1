// @(#)root/g3d:$Name:  $:$Id: TPoints3DABC.cxx,v 1.3 2003/08/23 00:08:12 rdm Exp $
// Author: Valery Fine(fine@mail.cern.ch)   04/05/99

// @(#)root/g3d:$Name:  $:$Id: TPoints3DABC.cxx,v 1.3 2003/08/23 00:08:12 rdm Exp $
// Author: Valery Fine(fine@mail.cern.ch)   24/04/99

#include "TPoints3DABC.h"
#include "TMath.h"

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TPoints3DABC                                                         //
//                                                                      //
// Abstract class to define Arrays of 3D points                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

ClassImp(TPoints3DABC)

//______________________________________________________________________________
Int_t TPoints3DABC::Add(Float_t x, Float_t y, Float_t z)
{
 // Add one 3D point defined by x,y,z to the array of the points
 // as its last element
 return AddLast(x,y,z);
}
//______________________________________________________________________________
Int_t TPoints3DABC::AddLast(Float_t x, Float_t y, Float_t z)
{
 // Add one 3D point defined by x,y,z to the array of the points
 // as its last element
 return SetNextPoint(x,y,z);
}
//______________________________________________________________________________
Int_t TPoints3DABC::DistancetoLine(Int_t px, Int_t py, Float_t x1, Float_t y1, Float_t x2, Float_t y2, Int_t lineWidth )
{
//*-*-*-*-*Compute distance from point px,py to an axis of the band defined*-*-*-*
//*-*  by pair points  (x1,y1),(x2,y2) where lineWidth is the width of the band
//*-*  ========================================================================
//*-*
//*-*  Compute the closest distance of approach from point px,py to this line.
//*-*  The distance is computed in pixels units.
//*-*
//*-*
//*-*  Algorithm:
//*-*
//*-*    A(x1,y1)         P                             B(x2,y2)
//*-*    ------------------------------------------------
//*-*                     I
//*-*                     I
//*-*                     I
//*-*                     I
//*-*                    M(x,y)
//*-*
//*-*  Let us call  a = distance AM     A=a**2
//*-*               b = distance BM     B=b**2
//*-*               c = distance AB     C=c**2
//*-*               d = distance PM     D=d**2
//*-*               u = distance AP     U=u**2
//*-*               v = distance BP     V=v**2     c = u + v
//*-*
//*-*  D = A - U
//*-*  D = B - V  = B -(c-u)**2
//*-*     ==> u = (A -B +C)/2c
//*-*
//*-*   Float_t x1    = gPad->XtoAbsPixel(xp1);
//*-*   Float_t y1    = gPad->YtoAbsPixel(yp1);
//*-*   Float_t x2    = gPad->XtoAbsPixel(xp2);
//*-*   Float_t y2    = gPad->YtoAbsPixel(yp2);
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
   Float_t xl, xt, yl, yt;
   Float_t x     = px;
   Float_t y     = py;
   if (x1 < x2) {xl = x1; xt = x2;}
   else         {xl = x2; xt = x1;}
   if (y1 < y2) {yl = y1; yt = y2;}
   else         {yl = y2; yt = y1;}
   if (x < xl-2 || x> xt+2) return 9999;  //following algorithm only valid in the box
   if (y < yl-2 || y> yt+2) return 9999;  //surrounding the line
   Float_t xx1   = x  - x1;
   Float_t xx2   = x  - x2;
   Float_t x1x2  = x1 - x2;
   Float_t yy1   = y  - y1;
   Float_t yy2   = y  - y2;
   Float_t y1y2  = y1 - y2;
   Float_t A     = xx1*xx1   + yy1*yy1;
   Float_t B     = xx2*xx2   + yy2*yy2;
   Float_t C     = x1x2*x1x2 + y1y2*y1y2;
   if (C <= 0)  return 9999;
   Float_t c     = TMath::Sqrt(C);
   Float_t u     = (A - B + C)/(2*c);
   Float_t D     = TMath::Abs(A - u*u);
   if (D < 0)   return 9999;

   return Int_t(TMath::Sqrt(D) - 0.5*float(lineWidth));
}

//______________________________________________________________________________
Int_t TPoints3DABC::SetNextPoint(Float_t x, Float_t y, Float_t z)
{
 // Add one 3D point defined by x,y,z to the array of the points
 // as its last element
  return SetPoint(GetLastPosition()+1,x,y,z);
}

//______________________________________________________________________________
Int_t TPoints3DABC::GetN() const
{
  // GetN()  returns the number of allocated cells if any.
  //         GetN() > 0 shows how many cells
  //         can be available via GetP() method.
  //         GetN() == 0 then GetP() must return 0 as well
  return 0;
}

//______________________________________________________________________________
Float_t *TPoints3DABC::GetP() const
{
  // GetP()  returns the pointer to the float point array
  //         of points if available
  //         The number of the available celss can be found via
  //         GetN() method.
  //         GetN() > 0 shows how many cells
  return 0;
}
//______________________________________________________________________________
Float_t *TPoints3DABC::GetXYZ(Float_t *xyz,Int_t idx,Int_t num)  const
{
  //
  // GetXYZ(Float_t *xyz,Int_t idx,Int_t num=1) fills the buffer supplied
  // by the calling code with the points information.
  //
  //  Input parameters:
  //  ----------------
  //   Float_t *xyz - an external user supplied floating point array.
  //   Int_t    num - the total number of the points to be copied
  //                  the dimension of that array the size of the
  //                  array is num*sizeof(Float_t) at least
  //   Int_t    idx - The index of the first copy to be taken.
  //
  //  Return: The pointer to the buffer array supplied
  //  ------

  if (xyz) {
    Int_t size = TMath::Min(idx+num,Size());
    Int_t j=0;
    for (Int_t i=idx;i<size;i++) {
      xyz[j++] = GetX(i);
      xyz[j++] = GetY(i);
      xyz[j++] = GetZ(i);
    }
  }
  return xyz;
}
