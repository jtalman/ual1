// @(#)root/graf:$Name:  $:$Id: TLine.h,v 1.4 2002/10/31 07:27:34 brun Exp $
// Author: Rene Brun   12/12/94

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TLine
#define ROOT_TLine


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TLine                                                                //
//                                                                      //
// A line segment.                                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include "TObject.h"
#endif

#ifndef ROOT_TAttLine
#include "TAttLine.h"
#endif


class TLine : public TObject, public TAttLine {

protected:
        Double_t      fX1;           //X of 1st point
        Double_t      fY1;           //Y of 1st point
        Double_t      fX2;           //X of 2nd point
        Double_t      fY2;           //Y of 2nd point

public:
        // TLine status bits
        enum { kLineNDC = BIT(14) };

        TLine();
        TLine(Double_t x1, Double_t y1,Double_t x2, Double_t  y2);
        TLine(const TLine &line);
        virtual ~TLine();
                void   Copy(TObject &line) const;
        virtual Int_t  DistancetoPrimitive(Int_t px, Int_t py);
        virtual TLine *DrawLine(Double_t x1, Double_t y1,Double_t x2, Double_t y2);
        virtual TLine *DrawLineNDC(Double_t x1, Double_t y1,Double_t x2, Double_t y2);
        virtual void   ExecuteEvent(Int_t event, Int_t px, Int_t py);
        Double_t        GetX1() const {return fX1;}
        Double_t        GetX2() const {return fX2;}
        Double_t        GetY1() const {return fY1;}
        Double_t        GetY2() const {return fY2;}
        virtual void   ls(Option_t *option="") const;
        virtual void   Paint(Option_t *option="");
        virtual void   PaintLine(Double_t x1, Double_t y1,Double_t x2, Double_t  y2);
        virtual void   PaintLineNDC(Double_t u1, Double_t v1,Double_t u2, Double_t  v2);
        virtual void   Print(Option_t *option="") const;
        virtual void   SavePrimitive(ofstream &out, Option_t *option);
        virtual void   SetX1(Double_t x1) {fX1=x1;}
        virtual void   SetX2(Double_t x2) {fX2=x2;}
        virtual void   SetY1(Double_t y1) {fY1=y1;}
        virtual void   SetY2(Double_t y2) {fY2=y2;}

        ClassDef(TLine,2)  //A line segment
};

#endif
