// @(#)root/base:$Name:  $:$Id: TView.h,v 1.14 2005/04/25 21:12:08 rdm Exp $
// Author: Rene Brun, Nenad Buncic, Evgueni Tcherniaev, Olivier Couet   18/08/95

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/


#ifndef ROOT_TView
#define ROOT_TView


/////////////////////////////////////////////////////////////////////////
//                                                                     //
// TView                                                               //
//                                                                     //
/////////////////////////////////////////////////////////////////////////


#ifndef ROOT_TObject
#include "TObject.h"
#endif

#ifndef ROOT_TAttLine
#include "TAttLine.h"
#endif

class TSeqCollection;
class TVirtualPad;

class TView : public TObject, public TAttLine {


protected:
        Double_t        fLatitude;         //View angle latitude
        Double_t        fLongitude;        //View angle longitude
        Double_t        fPsi;              //View angle psi
	     Double_t        fDview;            //Distance from COP to COV
	     Double_t        fDproj;            //Distance from COP to projection plane
        Double_t        fUpix;             // pad X size in pixels
        Double_t        fVpix;             // pad Y size in pixels
        Double_t        fTN[16];           //
        Double_t        fTB[16];           //
        Double_t        fRmax[3];          //Upper limits of object
        Double_t        fRmin[3];          //Lower limits of object
        Double_t        fUVcoord[4];       //Viewing window limits
        Double_t        fTnorm[16];        //Transformation matrix
        Double_t        fTback[16];        //Back transformation matrix
        Double_t        fX1[3];            //First coordinate of X axis
        Double_t        fX2[3];            //Second coordinate of X axis
        Double_t        fY1[3];            //First coordinate of Y axis
        Double_t        fY2[3];            //Second coordinate of Y axis
        Double_t        fZ1[3];            //First coordinate of Z axis
        Double_t        fZ2[3];            //Second coordinate of Z axis
        Int_t           fSystem;           //Coordinate system
        TSeqCollection *fOutline;          //Collection of outline's objects
        Bool_t          fDefaultOutline;   //Set to TRUE if outline is default cube
        Bool_t          fAutoRange;        //Set to TRUE if range computed automatically
        Bool_t          fChanged;          //! Set to TRUE after ExecuteRotateView

        void            ResetView(Double_t longitude, Double_t latitude, Double_t psi, Int_t &irep);


public:
   // TView status bits
   enum {
      kPerspective  = BIT(6)
   };
                TView();
                TView(Int_t system);
                TView(const Float_t *rmin, const Float_t *rmax, Int_t system = 1);
                TView(const Double_t *rmin, const Double_t *rmax, Int_t system = 1);
   virtual     ~TView();
   virtual void AxisVertex(Double_t ang, Double_t *av, Int_t &ix1, Int_t &ix2, Int_t &iy1, Int_t &iy2, Int_t &iz1, Int_t &iz2);
   virtual void DefinePerspectiveView();
   virtual void DefineViewDirection(const Double_t *s, const Double_t *c,
                                    Double_t cosphi, Double_t sinphi,
                                    Double_t costhe, Double_t sinthe,
                                    Double_t cospsi, Double_t sinpsi,
                                    Double_t *tnorm, Double_t *tback);
   virtual void  ExecuteEvent(Int_t event, Int_t px, Int_t py);
   virtual void  ExecuteRotateView(Int_t event, Int_t px, Int_t py);
   virtual void  FindScope(Double_t *scale, Double_t *center, Int_t &irep);
   virtual Int_t GetDistancetoAxis(Int_t axis, Int_t px, Int_t py, Double_t &ratio);
   Double_t      GetDview() const {return fDview;}
   Double_t      GetDproj() const {return fDproj;}
   Double_t      GetExtent() const;
   Bool_t        GetAutoRange() {return fAutoRange;}
   Double_t      GetLatitude() {return fLatitude;}
   Double_t      GetLongitude() {return fLongitude;}
   Double_t      GetPsi() {return fPsi;}
   virtual void  GetRange (Float_t *min, Float_t *max);
   virtual void  GetRange (Double_t *min, Double_t *max);
   Double_t     *GetRmax() {return fRmax;}
   Double_t     *GetRmin() {return fRmin;}
   TSeqCollection *GetOutline() {return fOutline; }
   Double_t     *GetTback() {return fTback;}
   Double_t     *GetTN() {return fTN;}
   Double_t     *GetTnorm() {return fTnorm;}
   Int_t         GetSystem() {return fSystem;}
   void          GetWindow(Double_t &u0, Double_t &v0, Double_t &du, Double_t &dv) const;
   Double_t      GetWindowWidth() const {return 0.5*(fUVcoord[1]-fUVcoord[0]);}
   Double_t      GetWindowHeight() const {return 0.5*(fUVcoord[3]-fUVcoord[2]);}
   virtual void  FindNormal(Double_t x, Double_t  y, Double_t z, Double_t &zn);
   virtual void  FindPhiSectors(Int_t iopt, Int_t &kphi, Double_t *aphi, Int_t &iphi1, Int_t &iphi2);
   virtual void  FindThetaSectors(Int_t iopt, Double_t phi, Int_t &kth, Double_t *ath, Int_t &ith1, Int_t &ith2);
   Bool_t        IsClippedNDC(Double_t *p) const;
   Bool_t        IsPerspective() const {return TestBit(kPerspective);}
   Bool_t        IsViewChanged() const {return fChanged;}
   virtual void  NDCtoWC(const Float_t *pn, Float_t *pw);
   virtual void  NDCtoWC(const Double_t *pn, Double_t *pw);
   virtual void  NormalWCtoNDC(const Float_t *pw, Float_t *pn);
   virtual void  NormalWCtoNDC(const Double_t *pw, Double_t *pn);
   virtual void  PadRange(Int_t rback);
   void          ResizePad();
   virtual void  SetAutoRange(Bool_t autorange=kTRUE) {fAutoRange=autorange;}
   virtual void  SetAxisNDC(const Double_t *x1, const Double_t *x2, const Double_t *y1, const Double_t *y2, const Double_t *z1, const Double_t *z2);
   void          SetDefaultWindow();
   void          SetDview(Double_t dview) {fDview=dview;}
   void          SetDproj(Double_t dproj) {fDproj=dproj;}
   void          SetLatitude(Double_t latitude) {fLatitude = latitude;}
   void          SetLongitude(Double_t longitude) {fLongitude = longitude;}
   void          SetPsi(Double_t psi) {fPsi = psi;}
   virtual void  SetOutlineToCube();
   virtual void  SetParallel(); // *MENU*
   virtual void  SetPerspective(); // *MENU*
   virtual void  SetRange(const Double_t *min, const Double_t *max);
   virtual void  SetRange(Double_t x0, Double_t y0, Double_t z0, Double_t x1, Double_t y1, Double_t z1, Int_t flag=0);
   virtual void  SetSystem(Int_t system) {fSystem = system;}
   virtual void  SetView(Double_t longitude, Double_t latitude, Double_t psi, Int_t &irep);
   void          SetViewChanged(Bool_t flag=kTRUE) {fChanged = flag;}
   void          SetWindow(Double_t u0, Double_t v0, Double_t du, Double_t dv);
   virtual void  WCtoNDC(const Float_t *pw, Float_t *pn);
   virtual void  WCtoNDC(const Double_t *pw, Double_t *pn);

//--
    void         MoveFocus(Double_t *center, Double_t dx, Double_t dy, Double_t dz, Int_t nsteps=10,
                            Double_t dlong=0, Double_t dlat=0, Double_t dpsi=0);
    virtual void MoveViewCommand(Char_t chCode, Int_t count=1);
    void         MoveWindow(Char_t option);

    static  void AdjustPad(TVirtualPad *pad=0);
    virtual void AdjustScales(TVirtualPad *pad=0); // *MENU*
    virtual void Centered3DImages(TVirtualPad *pad=0);
    virtual void Centered();                       // *MENU*
    virtual void FrontView(TVirtualPad *pad=0);
    virtual void Front();                          // *MENU*

    virtual void ZoomIn(); // *MENU*
    virtual void ZoomOut(); // *MENU*
    virtual void ZoomView(TVirtualPad *pad=0, Double_t zoomFactor = 1.25 );
    virtual void UnzoomView(TVirtualPad *pad=0,Double_t unZoomFactor = 1.25);

    virtual void RotateView(Double_t phi, Double_t theta, TVirtualPad *pad=0);
    virtual void SideView(TVirtualPad *pad=0);
    virtual void Side();                          // *MENU*
    virtual void TopView(TVirtualPad *pad=0);
    virtual void Top();                           // *MENU*

    virtual void ToggleRulers(TVirtualPad *pad=0);
    virtual void ShowAxis();                      // *MENU*
    virtual void ToggleZoom(TVirtualPad *pad=0);
    virtual void ZoomMove();                      // *MENU*
    virtual void Zoom();                          // *MENU*
    virtual void UnZoom();                        // *MENU*

   ClassDef(TView,2)  //3-D View
};

//      Shortcuts for menus
inline void TView::Centered(){Centered3DImages();}
inline void TView::Front()   {FrontView();}
inline void TView::ShowAxis(){ToggleRulers(); }
inline void TView::Side()    {SideView();}
inline void TView::Top()     {TopView();}
inline void TView::ZoomMove(){ToggleZoom();}
inline void TView::Zoom()    {ZoomView();}
inline void TView::UnZoom()  {UnzoomView();}

#endif

