// @(#)root/gui:$Name:  $:$Id: TGFrame.h,v 1.59 2005/01/12 18:39:29 brun Exp $
// Author: Fons Rademakers   03/01/98

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TGFrame
#define ROOT_TGFrame


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGFrame, TGCompositeFrame, TGVerticalFrame, TGHorizontalFrame,       //
// TGMainFrame, TGTransientFrame and TGGroupFrame                       //
//                                                                      //
// This header contains all different Frame classes.                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TGWindow
#include "TGWindow.h"
#endif
#ifndef ROOT_TQObject
#include "TQObject.h"
#endif
#ifndef ROOT_TGDimension
#include "TGDimension.h"
#endif
#ifndef ROOT_TGGC
#include "TGGC.h"
#endif
#ifndef ROOT_TGFont
#include "TGFont.h"
#endif
#ifndef ROOT_TGLayout
#include "TGLayout.h"
#endif
#ifndef ROOT_TGString
#include "TGString.h"
#endif

class TList;
class TGResourcePool;
class TContextMenu;

//---- frame states

enum EFrameState {
   kIsVisible  = BIT(0),
   kIsMapped   = kIsVisible,
   kIsArranged = BIT(1)
};

//---- frame cleanup
enum EFrameCleanup {
   kNoCleanup    = 0,
   kLocalCleanup = 1,
   kDeepCleanup  = -1
};

//---- types of frames (and borders)

enum EFrameType {
   kChildFrame      = 0,
   kMainFrame       = BIT(0),
   kVerticalFrame   = BIT(1),
   kHorizontalFrame = BIT(2),
   kSunkenFrame     = BIT(3),
   kRaisedFrame     = BIT(4),
   kDoubleBorder    = BIT(5),
   kFitWidth        = BIT(6),
   kFixedWidth      = BIT(7),
   kFitHeight       = BIT(8),
   kFixedHeight     = BIT(9),
   kFixedSize       = (kFixedWidth | kFixedHeight),
   kOwnBackground   = BIT(10),
   kTransientFrame  = BIT(11),
   kTempFrame       = BIT(12),
   kMdiMainFrame    = BIT(13),
   kMdiFrame        = BIT(14)
};

//---- MWM hints stuff

enum EMWMHints {
   // functions
   kMWMFuncAll      = BIT(0),
   kMWMFuncResize   = BIT(1),
   kMWMFuncMove     = BIT(2),
   kMWMFuncMinimize = BIT(3),
   kMWMFuncMaximize = BIT(4),
   kMWMFuncClose    = BIT(5),

   // input mode
   kMWMInputModeless                = 0,
   kMWMInputPrimaryApplicationModal = 1,
   kMWMInputSystemModal             = 2,
   kMWMInputFullApplicationModal    = 3,

   // decorations
   kMWMDecorAll      = BIT(0),
   kMWMDecorBorder   = BIT(1),
   kMWMDecorResizeH  = BIT(2),
   kMWMDecorTitle    = BIT(3),
   kMWMDecorMenu     = BIT(4),
   kMWMDecorMinimize = BIT(5),
   kMWMDecorMaximize = BIT(6)
};


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGFrame                                                              //
//                                                                      //
// This class subclasses TGWindow, used as base class for some simple   //
// widgets (buttons, labels, etc.).                                     //
// It provides:                                                         //
//  - position & dimension fields                                       //
//  - an 'options' attribute (see constant above)                       //
//  - a generic event handler                                           //
//  - a generic layout mechanism                                        //
//  - a generic border                                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class TGFrame : public TGWindow, public TQObject {

protected:
   Int_t    fX;             // frame x position
   Int_t    fY;             // frame y position
   UInt_t   fWidth;         // frame width
   UInt_t   fHeight;        // frame height
   UInt_t   fMinWidth;      // minimal frame width
   UInt_t   fMinHeight;     // minimal frame height
   UInt_t   fMaxWidth;      // maximal frame width
   UInt_t   fMaxHeight;     // maximal frame height
   Int_t    fBorderWidth;   // frame border width
   UInt_t   fOptions;       // frame options
   Pixel_t  fBackground;    // frame background color
   UInt_t   fEventMask;     // currenty active event mask
   TGFrameElement *fFE;     // pointer to frame element

   static Bool_t      fgInit;
   static Pixel_t     fgDefaultFrameBackground;
   static Pixel_t     fgDefaultSelectedBackground;
   static Pixel_t     fgWhitePixel;
   static Pixel_t     fgBlackPixel;
   static const TGGC *fgBlackGC;
   static const TGGC *fgWhiteGC;
   static const TGGC *fgHilightGC;
   static const TGGC *fgShadowGC;
   static const TGGC *fgBckgndGC;
   static Time_t      fgLastClick;
   static UInt_t      fgLastButton;
   static Int_t       fgDbx, fgDby;
   static Window_t    fgDbw;
   static UInt_t      fgUserColor;

   static Time_t      GetLastClick();

   virtual void  *GetSender() { return this; }  //used to set gTQSender
   virtual void   Draw3dRectangle(UInt_t type, Int_t x, Int_t y,
                                  UInt_t w, UInt_t h);
   virtual void   DoRedraw();

   const TGResourcePool *GetResourcePool() const
      { return fClient->GetResourcePool(); }

   TString GetOptionString() const;                //used in SavePrimitive()

   virtual void StartGuiBuilding(Bool_t on = kTRUE);

public:
   // Default colors and graphics contexts
   static Pixel_t     GetDefaultFrameBackground();
   static Pixel_t     GetDefaultSelectedBackground();
   static Pixel_t     GetWhitePixel();
   static Pixel_t     GetBlackPixel();
   static const TGGC &GetBlackGC();
   static const TGGC &GetWhiteGC();
   static const TGGC &GetHilightGC();
   static const TGGC &GetShadowGC();
   static const TGGC &GetBckgndGC();

   TGFrame(const TGWindow *p = 0, UInt_t w = 1, UInt_t h = 1,
           UInt_t options = 0, Pixel_t back = GetDefaultFrameBackground());
   TGFrame(TGClient *c, Window_t id, const TGWindow *parent = 0);
   virtual ~TGFrame();
   virtual void DeleteWindow();
   virtual void ReallyDelete() { delete this; }

   UInt_t GetEventMask() const { return fEventMask; }
   void   AddInput(UInt_t emask);
   void   RemoveInput(UInt_t emask);

   virtual Bool_t HandleEvent(Event_t *event);
   virtual Bool_t HandleConfigureNotify(Event_t *event);
   virtual Bool_t HandleButton(Event_t *) { return kFALSE; }
   virtual Bool_t HandleDoubleClick(Event_t *) { return kFALSE; }
   virtual Bool_t HandleCrossing(Event_t *) { return kFALSE; }
   virtual Bool_t HandleMotion(Event_t *) { return kFALSE; }
   virtual Bool_t HandleKey(Event_t *) { return kFALSE; }
   virtual Bool_t HandleFocusChange(Event_t *) { return kFALSE; }
   virtual Bool_t HandleClientMessage(Event_t *event);
   virtual Bool_t HandleSelection(Event_t *) { return kFALSE; }
   virtual Bool_t HandleSelectionRequest(Event_t *) { return kFALSE; }
   virtual Bool_t HandleSelectionClear(Event_t *) { return kFALSE; }
   virtual Bool_t HandleColormapChange(Event_t *) { return kFALSE; }
   virtual Bool_t HandleDragEnter(TGFrame *) { return kFALSE; }
   virtual Bool_t HandleDragLeave(TGFrame *) { return kFALSE; }
   virtual Bool_t HandleDragMotion(TGFrame *) { return kFALSE; }
   virtual Bool_t HandleDragDrop(TGFrame *, Int_t /*x*/, Int_t /*y*/, TGLayoutHints*)
                 { return kFALSE; }
   virtual void   ProcessedEvent(Event_t *event)
                 { Emit("ProcessedEvent(Event_t*)", (Long_t)event); } //*SIGNAL*

   virtual void   SendMessage(const TGWindow *w, Long_t msg, Long_t parm1, Long_t parm2);
   virtual Bool_t ProcessMessage(Long_t, Long_t, Long_t) { return kFALSE; }

   virtual TGDimension GetDefaultSize() const { return TGDimension(fWidth, fHeight); }
   virtual void    Move(Int_t x, Int_t y);
   virtual void    Resize(UInt_t w = 0, UInt_t h = 0);
   virtual void    Resize(TGDimension size);
   virtual void    MoveResize(Int_t x, Int_t y, UInt_t w = 0, UInt_t h = 0);
   virtual UInt_t  GetDefaultWidth() const { return GetDefaultSize().fWidth; }
   virtual UInt_t  GetDefaultHeight() const { return GetDefaultSize().fHeight; }
   virtual Pixel_t GetBackground() const { return fBackground; }
   virtual void    ChangeBackground(Pixel_t back);
   virtual void    SetBackgroundColor(Pixel_t back);
   virtual Pixel_t GetForeground() const;
   virtual void    SetForegroundColor(Pixel_t /*fore*/) {}
   virtual UInt_t  GetOptions() const { return fOptions; }
   virtual void    ChangeOptions(UInt_t options);
   virtual void    Layout() { }
   virtual void    MapSubwindows() { }  // Simple frames do not have subwindows
                                        // Redefine this in TGCompositeFrame!
   virtual void    ReparentWindow(const TGWindow *p, Int_t x = 0, Int_t y = 0)
                            { TGWindow::ReparentWindow(p, x, y); fX = x; fY = y; }
   virtual void    MapWindow() { TGWindow::MapWindow(); if (fFE) fFE->fState |= kIsVisible; }
   virtual void    MapRaised() { TGWindow::MapRaised(); if (fFE) fFE->fState |= kIsVisible; }
   virtual void    UnmapWindow() { TGWindow::UnmapWindow(); if (fFE) fFE->fState &= ~kIsVisible; }

   virtual void    DrawBorder();
   virtual void    DrawCopy(Handle_t /*id*/, Int_t /*x*/, Int_t /*y*/) { }
   virtual void    Activate(Bool_t) { }
   virtual Bool_t  IsActive() const { return kFALSE; }
   virtual Bool_t  IsComposite() const { return kFALSE; }
   virtual Bool_t  IsEditable() const { return kFALSE; }
   virtual void    SetEditable(Bool_t) {}
   virtual void    SetLayoutBroken(Bool_t = kTRUE) {}
   virtual Bool_t  IsLayoutBroken() const { return kFALSE; }
   virtual void    SetCleanup(Int_t = kLocalCleanup) { /* backward compatebility */ }

   virtual void    SetDragType(Int_t type);
   virtual void    SetDropType(Int_t type);
   virtual Int_t   GetDragType() const;
   virtual Int_t   GetDropType() const;

   UInt_t GetWidth() const { return fWidth; }
   UInt_t GetHeight() const { return fHeight; }
   UInt_t GetMinWidth() const { return fMinWidth; }
   UInt_t GetMinHeight() const { return fMinHeight; }
   UInt_t GetMaxWidth() const { return fMaxWidth; }
   UInt_t GetMaxHeight() const { return fMaxHeight; }
   TGDimension GetSize() const { return TGDimension(fWidth, fHeight); }
   Int_t  GetX() const { return fX; }
   Int_t  GetY() const { return fY; }
   Int_t  GetBorderWidth() const { return fBorderWidth; }

   TGFrameElement *GetFrameElement() const { return fFE; }
   void SetFrameElement(TGFrameElement *fe) { fFE = fe; }

   Bool_t Contains(Int_t x, Int_t y) const
      { return ((x >= 0) && (x < (Int_t)fWidth) && (y >= 0) && (y < (Int_t)fHeight)); }
   virtual TGFrame *GetFrameFromPoint(Int_t x, Int_t y)
      { return (Contains(x, y) ? this : 0); }

   // Modifiers (without graphic update)
   virtual void SetX(Int_t x) { fX = x; }
   virtual void SetY(Int_t y) { fY = y; }
   virtual void SetWidth(UInt_t w) { fWidth = w; }
   virtual void SetHeight(UInt_t h) { fHeight = h; }
   virtual void SetMinWidth(UInt_t w) { fMinWidth = w; }
   virtual void SetMinHeight(UInt_t h) { fMinHeight = h; }
   virtual void SetMaxWidth(UInt_t w) { fMaxWidth = w; }
   virtual void SetMaxHeight(UInt_t h) { fMaxHeight = h; }
   virtual void SetSize(const TGDimension &s) { fWidth = s.fWidth; fHeight = s.fHeight; }

   // Printing and saving
   virtual void Print(Option_t *option="") const;
   void SaveUserColor(ofstream &out, Option_t *);
   virtual void SavePrimitive(ofstream &out, Option_t *option);

   ClassDef(TGFrame,0)  // Base class for simple widgets (button, etc.)
};

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGCompositeFrame                                                     //
//                                                                      //
// This class is the base class for composite widgets                   //
// (menu bars, list boxes, etc.).                                       //
//                                                                      //
// It provides:                                                         //
//  - a layout manager                                                  //
//  - a frame container (TList *)                                       //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class TGCompositeFrame : public TGFrame {

protected:
   TGLayoutManager *fLayoutManager;   // layout manager
   TList           *fList;            // container of frame elements
   Bool_t           fLayoutBroken;    // no layout manager is used
   Int_t            fMustCleanup;     // cleanup mode (see EFrameCleanup)
   Bool_t           fMapSubwindows;   // kTRUE - map subwindows

   static TContextMenu  *fgContextMenu;   // context menu for setting GUI attributes
   static TGLayoutHints *fgDefaultHints;  // default hints used by AddFrame()

   virtual void SavePrimitiveSubframes(ofstream &out, Option_t *option);

public:
   TGCompositeFrame(const TGWindow *p = 0, UInt_t w = 1, UInt_t h = 1,
                    UInt_t options = 0,
                    Pixel_t back = GetDefaultFrameBackground());
   TGCompositeFrame(TGClient *c, Window_t id, const TGWindow *parent = 0);
   virtual ~TGCompositeFrame();
   virtual TList *GetList() const { return fList; }

   virtual UInt_t GetDefaultWidth() const
                 { return GetDefaultSize().fWidth; }
   virtual UInt_t GetDefaultHeight() const
                 { return GetDefaultSize().fHeight; }
   virtual TGDimension GetDefaultSize() const
                 { return fLayoutManager->GetDefaultSize(); }
   virtual TGFrame *GetFrameFromPoint(Int_t x, Int_t y);
   virtual Bool_t TranslateCoordinates(TGFrame *child, Int_t x, Int_t y,
                                       Int_t &fx, Int_t &fy);
   virtual void   MapSubwindows();
   virtual void   Layout();
   virtual Bool_t HandleButton(Event_t *) { return kFALSE; }
   virtual Bool_t HandleDoubleClick(Event_t *) { return kFALSE; }
   virtual Bool_t HandleCrossing(Event_t *) { return kFALSE; }
   virtual Bool_t HandleMotion(Event_t *) { return kFALSE; }
   virtual Bool_t HandleKey(Event_t *) { return kFALSE; }
   virtual Bool_t HandleFocusChange(Event_t *) { return kFALSE; }
   virtual Bool_t HandleSelection(Event_t *) { return kFALSE; }
   virtual Bool_t HandleDragEnter(TGFrame *);
   virtual Bool_t HandleDragLeave(TGFrame *);
   virtual Bool_t HandleDragMotion(TGFrame *);
   virtual Bool_t HandleDragDrop(TGFrame *frame, Int_t x, Int_t y, TGLayoutHints *lo);
   virtual void   ChangeOptions(UInt_t options);
   virtual Bool_t ProcessMessage(Long_t, Long_t, Long_t) { return kFALSE; }

   TGLayoutManager *GetLayoutManager() const { return fLayoutManager; }
   void             SetLayoutManager(TGLayoutManager *l);

   virtual void   AddFrame(TGFrame *f, TGLayoutHints *l = 0);
   virtual void   RemoveFrame(TGFrame *f);
   virtual void   ShowFrame(TGFrame *f);
   virtual void   HideFrame(TGFrame *f);
   Int_t          GetState(TGFrame *f) const;
   Bool_t         IsVisible(TGFrame *f) const;
   Bool_t         IsVisible(TGFrameElement *ptr) const { return (ptr->fState & kIsVisible); }
   Bool_t         IsArranged(TGFrame *f) const;
   Bool_t         IsArranged(TGFrameElement *ptr) const { return (ptr->fState & kIsArranged); }
   Bool_t         IsComposite() const { return kTRUE; }
   virtual Bool_t IsEditable() const;
   virtual void   SetEditable(Bool_t on = kTRUE);
   virtual void   SetLayoutBroken(Bool_t on = kTRUE);
   virtual Bool_t IsLayoutBroken() const
                  { return fLayoutBroken || !fLayoutManager || IsEditable(); }
   virtual void   SetEditDisabled(Bool_t on = kTRUE);
   virtual void   SetCleanup(Int_t mode = kLocalCleanup);
   virtual Int_t  MustCleanup() const { return fMustCleanup; }
   virtual void   Cleanup();
   virtual void   SetMapSubwindows(Bool_t on) {  fMapSubwindows = on; }
   virtual Bool_t IsMapSubwindows() const { return fMapSubwindows; }

   virtual void   Print(Option_t *option="") const;
   virtual void   SavePrimitive(ofstream &out, Option_t *option);

   ClassDef(TGCompositeFrame,0)  // Base class for composite widgets (menubars, etc.)
};


class TGVerticalFrame : public TGCompositeFrame {
public:
   TGVerticalFrame(const TGWindow *p = 0, UInt_t w = 1, UInt_t h = 1,
                   UInt_t options = kChildFrame,
                   Pixel_t back = GetDefaultFrameBackground()) :
      TGCompositeFrame(p, w, h, options | kVerticalFrame, back) { SetWindowName(); }
   virtual void SavePrimitive(ofstream &out, Option_t *option);

   ClassDef(TGVerticalFrame,0)  // Composite frame with vertical child layout
};

class TGHorizontalFrame : public TGCompositeFrame {
public:
   TGHorizontalFrame(const TGWindow *p = 0, UInt_t w = 1, UInt_t h = 1,
                     UInt_t options = kChildFrame,
                     Pixel_t back = GetDefaultFrameBackground()) :
      TGCompositeFrame(p, w, h, options | kHorizontalFrame, back) { SetWindowName(); }
   virtual void SavePrimitive(ofstream &out, Option_t *option);

   ClassDef(TGHorizontalFrame,0)  // Composite frame with horizontal child layout
};


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGMainFrame                                                          //
//                                                                      //
// This class defines top level windows that interact with the system   //
// Window Manager (WM or MWM for Motif Window Manager).                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class TGMainFrame : public TGCompositeFrame {

protected:
   enum { kDontCallClose = BIT(14) };

   // mapping between key and window
   class TGMapKey : public TObject {
   public:
      UInt_t     fKeyCode;
      TGWindow  *fWindow;
      TGMapKey(UInt_t keycode, TGWindow *w) { fKeyCode = keycode; fWindow = w; }
   };

   TList        *fBindList;     // list with key bindings
   TString       fWindowName;   // window name
   TString       fIconName;     // icon name
   TString       fIconPixmap;   // icon pixmap name
   TString       fClassName;    // WM class name
   TString       fResourceName; // WM resource name
   UInt_t        fMWMValue;     // MWM decoration hints
   UInt_t        fMWMFuncs;     // MWM functions
   UInt_t        fMWMInput;     // MWM input modes
   Int_t         fWMX;          // WM x position
   Int_t         fWMY;          // WM y position
   UInt_t        fWMWidth;      // WM width
   UInt_t        fWMHeight;     // WM height
   UInt_t        fWMMinWidth;   // WM min width
   UInt_t        fWMMinHeight;  // WM min height
   UInt_t        fWMMaxWidth;   // WM max width
   UInt_t        fWMMaxHeight;  // WM max height
   UInt_t        fWMWidthInc;   // WM width increments
   UInt_t        fWMHeightInc;  // WM height increments
   EInitialState fWMInitState;  // WM initial state

   TString GetMWMvalueString() const;  //used in SaveSource()
   TString GetMWMfuncString() const;   //used in SaveSource()
   TString GetMWMinpString() const;    //used in SaveSource()

public:
   TGMainFrame(const TGWindow *p = 0, UInt_t w = 1, UInt_t h = 1,
               UInt_t options = kVerticalFrame);
   virtual ~TGMainFrame();

   virtual Bool_t HandleKey(Event_t *event);
   virtual Bool_t HandleClientMessage(Event_t *event);
   virtual void   SendCloseMessage();
   virtual void   CloseWindow();   //*SIGNAL*

   void DontCallClose();
   void SetWindowName(const char *name = 0);
   void SetIconName(const char *name);
   void SetIconPixmap(const char *iconName);
   void SetClassHints(const char *className, const char *resourceName);
   void SetMWMHints(UInt_t value, UInt_t funcs, UInt_t input);
   void SetWMPosition(Int_t x, Int_t y);
   void SetWMSize(UInt_t w, UInt_t h);
   void SetWMSizeHints(UInt_t wmin, UInt_t hmin, UInt_t wmax, UInt_t hmax,
                       UInt_t winc, UInt_t hinc);
   void SetWMState(EInitialState state);

   virtual Bool_t BindKey(const TGWindow *w, Int_t keycode, Int_t modifier) const;
   virtual void   RemoveBind(const TGWindow *w, Int_t keycode, Int_t modifier) const;
   TList *GetBindList() const { return fBindList; }

   const char *GetWindowName() const { return fWindowName; }
   const char *GetIconName() const { return fIconName; }
   const char *GetIconPixmap() const { return fIconPixmap; }
   void GetClassHints(const char *&className, const char *&resourceName) const
      { className = fClassName.Data(); resourceName = fResourceName.Data(); }
   void GetMWMHints(UInt_t &value, UInt_t &funcs, UInt_t &input) const
      { value = fMWMValue; funcs = fMWMFuncs; input = fMWMInput; }
   void GetWMPosition(Int_t &x, Int_t &y) const { x = fWMX; y = fWMY; }
   void GetWMSize(UInt_t &w, UInt_t &h) const { w = fWMWidth; h = fWMHeight; }
   void GetWMSizeHints(UInt_t &wmin, UInt_t &hmin, UInt_t &wmax, UInt_t &hmax,
                       UInt_t &winc, UInt_t &hinc) const
      { wmin = fWMMinWidth; hmin = fWMMinHeight; wmax = fWMMaxWidth;
        hmax = fWMMaxHeight; winc = fWMWidthInc; hinc = fWMHeightInc; }
   EInitialState GetWMState() const { return fWMInitState; }

   virtual void SavePrimitive(ofstream &out, Option_t *option);
   virtual void SaveSource(const char *filename = "Rootappl.C", Option_t *option = ""); // *MENU*

   ClassDef(TGMainFrame,0)  // Top level window frame
};


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGTransientFrame                                                     //
//                                                                      //
// This class defines transient windows that typically are used for     //
// dialogs.                                                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class TGTransientFrame : public TGMainFrame {

protected:
   const TGWindow   *fMain;  // window over which to popup dialog

public:
   TGTransientFrame(const TGWindow *p = 0, const TGWindow *main = 0, UInt_t w = 1, UInt_t h = 1,
                    UInt_t options = kVerticalFrame);

   enum EPlacement { kCenter, kLeft, kRight, kTop, kBottom, kTopLeft, kTopRight,
                     kBottomLeft, kBottomRight };
   virtual void    CenterOnParent(Bool_t croot = kTRUE, EPlacement pos = kCenter);
   const TGWindow *GetMain() const { return fMain; }
   virtual void    SavePrimitive(ofstream &out, Option_t *option);
   virtual void    SaveSource(const char *filename = "Rootdlog.C", Option_t *option = ""); // *MENU*

   ClassDef(TGTransientFrame,0)  // Frame for dialog (transient) windows
};


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TGGroupFrame                                                         //
//                                                                      //
// A group frame is a composite frame with a border and a title.        //
// It is typically used to group a number of logically related widgets  //
// visually together.                                                   //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class TGGroupFrame : public TGCompositeFrame {

protected:
   TGString      *fText;         // title text
   FontStruct_t   fFontStruct;   // title fontstruct
   GContext_t     fNormGC;       // title graphics context
   Int_t          fTitlePos;     // title position

   virtual void DoRedraw();

   static const TGFont *fgDefaultFont;
   static const TGGC   *fgDefaultGC;

public:
   enum ETitlePos { kLeft = -1, kCenter = 0, kRight = 1 };

   static FontStruct_t  GetDefaultFontStruct();
   static const TGGC   &GetDefaultGC();

   TGGroupFrame(const TGWindow *p, TGString *title,
                UInt_t options = kVerticalFrame,
                GContext_t norm = GetDefaultGC()(),
                FontStruct_t font = GetDefaultFontStruct(),
                Pixel_t back = GetDefaultFrameBackground());
   TGGroupFrame(const TGWindow *p = 0, const char *title = 0,
                UInt_t options = kVerticalFrame,
                GContext_t norm = GetDefaultGC()(),
                FontStruct_t font = GetDefaultFontStruct(),
                Pixel_t back = GetDefaultFrameBackground());
   virtual ~TGGroupFrame();

   virtual TGDimension GetDefaultSize() const;
   virtual void DrawBorder();
   virtual void SetTitlePos(ETitlePos pos = kLeft) { fTitlePos = pos; }
   Int_t        GetTitlePos() const { return fTitlePos; }
   virtual void SetTitle(TGString *title);
   virtual void SetTitle(const char *title);
   virtual const char *GetTitle() const { return fText->GetString(); }
   virtual void SavePrimitive(ofstream &out, Option_t *option);

   ClassDef(TGGroupFrame,0)  // A composite frame with border and title
};

#endif
