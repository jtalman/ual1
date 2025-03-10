// @(#)root/gpad:$Name:  $:$Id: TCanvas.h,v 1.31 2005/04/23 10:55:06 brun Exp $
// Author: Rene Brun   12/12/94

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TCanvas
#define ROOT_TCanvas


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TCanvas                                                              //
//                                                                      //
// Graphics canvas.                                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TPad
#include "TPad.h"
#endif

#ifndef ROOT_TAttCanvas
#include "TAttCanvas.h"
#endif

#ifndef ROOT_TVirtualX
#include "TVirtualX.h"
#endif

#ifndef ROOT_TString
#include "TString.h"
#endif

#ifndef ROOT_TCanvasImp
#include "TCanvasImp.h"
#endif

class TContextMenu;
class TControlBar;
class TBrowser;

class TCanvas : public TPad {

friend class TCanvasImp;
friend class TThread;
friend class TInterpreter;

protected:
   TAttCanvas    fCatt;            //Canvas attributes
   TString       fDISPLAY;         //Name of destination screen
   Size_t        fXsizeUser;       //User specified size of canvas along X in CM
   Size_t        fYsizeUser;       //User specified size of canvas along Y in CM
   Size_t        fXsizeReal;       //Current size of canvas along X in CM
   Size_t        fYsizeReal;       //Current size of canvas along Y in CM
   Color_t       fHighLightColor;  //Highlight color of active pad
   Int_t         fDoubleBuffer;    //Double buffer flag (0=off, 1=on)
   Int_t         fWindowTopX;      //Top X position of window (in pixels)
   Int_t         fWindowTopY;      //Top Y position of window (in pixels)
   UInt_t        fWindowWidth;     //Width of window (including borders, etc.)
   UInt_t        fWindowHeight;    //Height of window (including menubar, borders, etc.)
   UInt_t        fCw;              //Width of the canvas along X (pixels)
   UInt_t        fCh;              //Height of the canvas along Y (pixels)
   Int_t         fEvent;           //!Type of current or last handled event
   Int_t         fEventX;          //!Last X mouse position in canvas
   Int_t         fEventY;          //!Last Y mouse position in canvas
   Int_t         fCanvasID;        //!Canvas identifier
   TObject      *fSelected;        //!Currently selected object
   Int_t         fSelectedX;       //!X of selected object
   Int_t         fSelectedY;       //!Y of selected object
   TString       fSelectedOpt;     //!Drawing option of selected object
   TPad         *fSelectedPad;     //!Pad containing currently selected object
   TPad         *fPadSave;         //!Pointer to saved pad in HandleInput
   TCanvasImp   *fCanvasImp;       //!Window system specific canvas implementation
   TContextMenu *fContextMenu;     //!Context menu pointer
   Bool_t        fBatch;           //!True when in batchmode
   Bool_t        fUpdating;        //!True when Updating the canvas
   Bool_t        fRetained;        //Retain structure flag
   Bool_t        fShowEventStatus; //Show event status panel
   Bool_t        fAutoExec;        //To auto exec the list of pad TExecs
   Bool_t        fMoveOpaque;      //Move objects in opaque mode
   Bool_t        fResizeOpaque;    //Resize objects in opaque mode
   Bool_t        fMenuBar;         //False if no menubar is displayed
   static Bool_t fgIsFolder;       //Indicates if canvas can be browsed as a folder

   Bool_t        fShowToolBar;     //Show toolbar
   Bool_t        fShowEditor;      //Show side frame or old Editor

private:
   TCanvas(const TCanvas &canvas);  // cannot copy canvas, use TObject::Clone()
   TCanvas &operator=(const TCanvas &rhs);  // idem
   void     Build();
   void     CopyPixmaps();
   void     DrawEventStatus(Int_t event, Int_t x, Int_t y, TObject *selected);
   void     RunAutoExec();

protected:
   virtual void ExecuteEvent(Int_t event, Int_t px, Int_t py);
   //-- used by friend TThread class
   void Constructor();
   void Constructor(const char *name, const char *title, Int_t form);
   void Constructor(const char *name, const char *title, Int_t ww, Int_t wh);
   void Constructor(const char *name, const char *title,
           Int_t wtopx, Int_t wtopy, Int_t ww, Int_t wh);
   void Destructor();
   //-- used by friend TThread class
   void Init();

public:
   TCanvas(Bool_t build=kTRUE);
   TCanvas(const char *name, const char *title="", Int_t form=1);
   TCanvas(const char *name, const char *title, Int_t ww, Int_t wh);
   TCanvas(const char *name, const char *title, Int_t wtopx, Int_t wtopy,
           Int_t ww, Int_t wh);
   TCanvas(const char *name, Int_t ww, Int_t wh, Int_t winid);
   virtual ~TCanvas();

   TVirtualPad      *cd(Int_t subpadnumber=0);
   virtual void      Browse(TBrowser *b);
   void              Clear(Option_t *option="");
   void              Close(Option_t *option="");
   virtual void      Delete(Option_t * = "") { MayNotUse("Delete()"); }
   void              DisconnectWidget();  // used by TCanvasImp
   virtual void      Draw(Option_t *option="");
   virtual TObject  *DrawClone(Option_t *option="") const; // *MENU*
   virtual TObject  *DrawClonePad(); // *MENU*
   virtual void      EditorBar();
   void              EnterLeave(TPad *prevSelPad, TObject *prevSelObj);
   void              FeedbackMode(Bool_t set);
   void              Flush();
   void              UseCurrentStyle(); // *MENU*
   void              ForceUpdate() { fCanvasImp->ForceUpdate(); }
   const char       *GetDISPLAY() const {return fDISPLAY.Data();}
   TContextMenu     *GetContextMenu() const {return fContextMenu;};
   Int_t             GetDoubleBuffer() const {return fDoubleBuffer;}
   Int_t             GetEvent() const { return fEvent; }
   Int_t             GetEventX() const { return fEventX; }
   Int_t             GetEventY() const { return fEventY; }
   Color_t           GetHighLightColor() const { return fHighLightColor; }
   TVirtualPad      *GetPadSave() const { return fPadSave; }
   TObject          *GetSelected() const {return fSelected;}
   Int_t             GetSelectedX() const {return fSelectedX;}
   Int_t             GetSelectedY() const {return fSelectedY;}
   Option_t         *GetSelectedOpt() const {return fSelectedOpt.Data();}
   TVirtualPad      *GetSelectedPad() const { return fSelectedPad; }
   Bool_t            GetShowEventStatus() const { return fShowEventStatus; }
   Bool_t            GetShowToolBar() const { return fShowToolBar; }
   Bool_t            GetShowEditor() const { return fShowEditor; }
   Bool_t            GetAutoExec() const { return fAutoExec; }
   Size_t            GetXsizeUser() const {return fXsizeUser;}
   Size_t            GetYsizeUser() const {return fYsizeUser;}
   Size_t            GetXsizeReal() const {return fXsizeReal;}
   Size_t            GetYsizeReal() const {return fYsizeReal;}
   Int_t             GetCanvasID() const {return fCanvasID;}
   TCanvasImp       *GetCanvasImp() const {return fCanvasImp;}
   Int_t             GetWindowTopX();
   Int_t             GetWindowTopY();
   UInt_t            GetWindowWidth() const { return fWindowWidth; }
   UInt_t            GetWindowHeight() const { return fWindowHeight; }
   UInt_t            GetWw() const { return fCw; }
   UInt_t            GetWh() const { return fCh; }
   virtual void      GetCanvasPar(Int_t &wtopx, Int_t &wtopy, UInt_t &ww, UInt_t &wh)
                     {wtopx=GetWindowTopX(); wtopy=fWindowTopY; ww=fWindowWidth; wh=fWindowHeight;}
   virtual void      HandleInput(EEventType button, Int_t x, Int_t y);
   Bool_t            HasMenuBar() const { return fMenuBar; }
   void              Iconify() { fCanvasImp->Iconify(); }
   Bool_t            IsBatch() const { return fBatch; }
   Bool_t            IsFolder() const;
   Bool_t            IsRetained() const { return fRetained; }
   virtual void      ls(Option_t *option="") const;
   void              MoveOpaque(Int_t set=1);
   Bool_t            OpaqueMoving() const { return fMoveOpaque; }
   Bool_t            OpaqueResizing() const { return fResizeOpaque; }
   virtual void      Paint(Option_t *option="");
   virtual TPad     *Pick(Int_t px, Int_t py, TObjLink *&pickobj) { return TPad::Pick(px, py, pickobj); }
   virtual TPad     *Pick(Int_t px, Int_t py, TObject *prevSelObj);
   virtual void      Picked(TPad *selpad, TObject *selected, Int_t event);             // *SIGNAL*
   virtual void      ProcessedEvent(Int_t event, Int_t x, Int_t y, TObject *selected); // *SIGNAL*
   virtual void      Selected(TVirtualPad *pad, TObject *obj, Int_t event);            // *SIGNAL*
   virtual void      Closed();                                                         // *SIGNAL*
   virtual void      Resize(Option_t *option="");
   void              ResizeOpaque(Int_t set=1) { fResizeOpaque = set; }
   void              SaveSource(const char *filename="", Option_t *option="");
   void              SavePrimitive(ofstream &out, Option_t *option);
   virtual void      SetCursor(ECursor cursor);
   virtual void      SetDoubleBuffer(Int_t mode=1);
   virtual void      SetFixedAspectRatio(Bool_t fixed = kTRUE);  // *TOGGLE*
   void              SetWindowPosition(Int_t x, Int_t y) { fCanvasImp->SetWindowPosition(x, y); }
   void              SetWindowSize(UInt_t ww, UInt_t wh) { fCanvasImp->SetWindowSize(ww, wh); }
   void              SetCanvasSize(UInt_t ww, UInt_t wh); // *MENU*
   void              SetHighLightColor(Color_t col) { fHighLightColor = col; }
   void              SetSelected(TObject *obj);
   void              SetSelectedPad(TPad *pad) { fSelectedPad = pad; }
   void              Show() { fCanvasImp->Show(); }
   virtual void      Size(Float_t xsizeuser=0, Float_t ysizeuser=0);
   void              SetBatch(Bool_t batch=kTRUE);
   static  void      SetFolder(Bool_t isfolder=kTRUE);
   void              SetRetained(Bool_t retained=kTRUE) { fRetained=retained;}
   void              SetTitle(const char *title="");
   virtual void      ToggleEventStatus();
   virtual void      ToggleAutoExec();
   virtual void      ToggleToolBar();
   virtual void      ToggleEditor();
   virtual void      Update();

   static void       MakeDefCanvas();

   ClassDef(TCanvas,5)  //Graphics canvas
};

#endif
