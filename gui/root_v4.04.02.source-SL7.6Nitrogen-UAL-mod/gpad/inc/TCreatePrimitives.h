// @(#)root/gpad:$Name:  $:$Id: TCreatePrimitives.h,v 1.0

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TCreatePrimitives
#define ROOT_TCreatePrimitives


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TCreatePrimitives                                                    //
//                                                                      //
// Creates new primitives.                                              //
//                                                                      //
// The functions in this static class are called by TPad::ExecuteEvent  //
// to create new primitives in gPad from the TPad toolbar.              //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class TCreatePrimitives {

public:

   TCreatePrimitives();
   virtual ~TCreatePrimitives();
   static void Ellipse(Int_t event, Int_t px, Int_t py,Int_t mode);
   static void Line(Int_t event, Int_t px, Int_t py, Int_t mode);
   static void Pad(Int_t event, Int_t px, Int_t py, Int_t);
   static void Pave(Int_t event, Int_t px, Int_t py, Int_t mode);
   static void PolyLine(Int_t event, Int_t px, Int_t py, Int_t mode);
   static void Text(Int_t event, Int_t px, Int_t py, Int_t mode);
};

#endif

