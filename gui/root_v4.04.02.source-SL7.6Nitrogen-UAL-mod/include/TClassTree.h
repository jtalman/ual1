// @(#)root/gpad:$Name:  $:$Id: TClassTree.h,v 1.3 2000/12/13 15:13:49 brun Exp $
// Author: Rene Brun   01/12/98

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TClassTree
#define ROOT_TClassTree


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TClassTree                                                           //
//                                                                      //
// Manager class to draw classes inheritance tree and relations         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TClass
#include "TClass.h"
#endif

class TObjString;

class TClassTree : public TNamed {

protected:
   TString   fClasses;    //List of classes to be drawn
   Float_t   fYoffset;    //offset at top of picture in per cent of pad
   Float_t   fLabelDx;    //width along x of TPaveLabels in per cent of pad
   Int_t     fNclasses;   //current number of classes
   Int_t     fShowCod;    //if 1 show classes referenced by implementation
   Int_t     fShowMul;    //if 1 show multiple inheritance
   Int_t     fShowHas;    //if 1 show "has a" relationship
   Int_t     fShowRef;    //if 1 show classes relationship other than inheritance
   Int_t    *fCstatus;    //[fNclasses] classes status
   Int_t    *fNdata;      //[fNclasses] Number of data members per class
   Int_t    *fParents;    //[fNclasses] parent number of classes (permanent)
   Int_t    *fCparent;    //!parent number of classes (temporary)
   char    **fDerived;    //![fNclasses] table to indicate if i derives from j
   TClass  **fCpointer;   //![fNclasses] pointers to the TClass objects
   TString **fCnames;     //![fNclasses] class names
   TString **fCtitles;    //![fNclasses] class titles
   TString **fOptions;    //![fNclasses] List of options per class
   TString   fSourceDir;  //Concatenated source directories
   TList   **fLinks;      //![fNclasses] for each class, the list of referenced(ing) classes

   virtual  void FindClassPosition(const char *classname, Float_t &x, Float_t &y);
   virtual  void FindClassesUsedBy(Int_t iclass);
   virtual  void FindClassesUsing(Int_t iclass);
   virtual  void Init();
   TObjString   *Mark(const char *classname, TList *los, Int_t abit);
   virtual  void PaintClass(Int_t iclass, Float_t xleft, Float_t y);
   virtual  void ScanClasses(Int_t iclass);
   virtual  void ShowCod();
   virtual  void ShowHas();
   virtual  void ShowMul();
   virtual  void ShowRef();

public:
   TClassTree();
   TClassTree(const char *name, const char *classes="");
   virtual      ~TClassTree();
   virtual  void Draw(const char *classes ="");
   virtual Int_t FindClass(const char *classname);
   const char   *GetClasses() const {return fClasses.Data();}
   virtual const char  *GetSourceDir() const {return fSourceDir.Data();}
   virtual  void ls(Option_t *option="") const;
   virtual  void Paint(Option_t *option="");
   virtual  void SaveAs(const char *filename="");
   virtual  void SetClasses(const char *classes, Option_t *option="ID");
   virtual  void SetSourceDir(const char *dir="src") {fSourceDir = dir;}
   virtual  void SetYoffset(Float_t offset=0);
   virtual  void SetLabelDx(Float_t labeldx=0.15);
   virtual  void ShowClassesUsedBy(const char *classes);
   virtual  void ShowClassesUsing(const char *classes);
   virtual  void ShowLinks(Option_t *option="HMR");

   ClassDef(TClassTree,1)  //Manager class to draw classes inheritance tree and relations
};

#endif

