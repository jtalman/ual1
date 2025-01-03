// @(#)root/treeviewer:$Name:  $:$Id: TTreeViewer.cxx,v 1.47 2005/03/11 17:57:45 brun Exp $
//Author : Andrei Gheata   16/08/00

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//
// TreeViewer is a graphic user interface designed to handle ROOT trees and to
// take advantage of TTree class features.
//
// It uses ROOT native GUI widgets adapted for 'drag and drop' functionality.
// in the same session.
// The following capabilities are making the viewer a helpful tool for analysis:
//  - several trees may be opened in the same session;
//  - branches and leaves can be easily browsed or scanned;
//  - fast drawing of branch expressions by double-clicking;
//  - new variables/selections easy to compose with the built-in editor;
//  - histograms can be composed by dragging leaves or user-defined expressions
//  to X, Y and Z axis items;
//  - the tree entries to be processed can be selected with a double slider;
//  - selections can be defined and activated by dragging them to the 'Cut' item;
//  - all expressions can be aliased and aliases can be used in composing others;
//  - input/output event lists easy to handle;
//  - menu with histogram drawing options;
//  - user commands may be executed within the viewer and the current command
//  can be echoed;
//  - current 'Draw' event loop is reflected by a progress bar and may be
//  interrupted by the user;
//  - all widgets have self-explaining tool tips and/or context menus;
//  - expressions/leaves can be dragged to a 'scan box' and scanned by
//  double-clicking this item. The result can be redirected to an ASCII file;
//
// The layout has the following items:
//
//  - a menu bar with entries : File, Edit, Run, Options and Help;
//  - a toolbar in the upper part where you can issue user commands, change
//  the drawing option and the histogram name, three check buttons Hist, Rec
//  and Scan.HIST toggles histogram drawing mode, REC enables recording of the
//  last command issued and SCAN enables redirecting of TTree::Scan command in
//  an ASCII file (see -Scanning expressions-);
//  - a button bar in the lower part with : buttons DRAW/STOP that issue histogram
//  drawing and stop the current command respectively, two text widgets where
//  input and output event lists can be specified, a message box and a RESET
//  button on the right that clear edited expression content (see Editing...)
//  - a tree-type list on the main left panel where you can select among trees or
//  branches. The tree/branch will be detailed in the right panel.
//  Mapped trees are provided with context menus, activated by right-clicking;
//  - a view-type list on the right panel. The first column contain X, Y and
//  Z expression items, an optional cut and ten optional editable expressions.
//  Expressions and leaf-type items can be dragged or deleted. A right click on
//  the list-box or item activates context menus.
//
// Opening a new tree and saving a session :
//
//   To open a new tree in the viewer use <File/Open tree file> menu
// The content of the file (keys) will be listed. Use <SetTreeName> function
// from the context menu of the right panel, entering a tree name among those
// listed.
//   To save the current session, use <File/Save> menu or the <SaveSource>
// function from the context menu of the right panel (to specify the name of the
// file - name.C)
//   To open a previously saved session for the tree MyTree, first open MyTree
// in the browser, then use <File/Open session> menu.
//
// Dragging items:
//
// Items that can be dragged from the list in the right : expressions and
// leaves. Dragging an item and dropping to another will copy the content of first
// to the last (leaf->expression, expression->expression). Items far to the right
// side of the list can be easily dragged to the left (where expressions are
// placed) by dragging them to the left at least 10 pixels.
//
// Editing expressions
//
//   Any editable expression from the right panel has two components : a
// true name (that will be used when TTree::Draw() commands are issued) and an
// alias. The visible name is the alias. Aliases of user defined expressions have
// a leading ~ and may be used in new expressions. Expressions containing boolean
// operators have a specific icon and may be dragged to the active cut (scissors
// item) position.
//    The expression editor can be activated by double-clicking empty expression,
// using <EditExpression> from the selected expression context menu or using
// <Edit/Expression> menu.
//    The editor will pop-up in the left part, but it can be moved.
// The editor usage is the following :
//   - you can write C expressions made of leaf names by hand or you can insert
//   any item from the right panel by clicking on it (recommandable);
//   - you can click on other expressions/leaves to paste them in the editor;
//   - you should write the item alias by hand since it not only make the expression
//  meaningfull, but it also highly improve the layout for big expressions
//   - you may redefine an old alias - the other expressions depending on it will
//   be modified accordingly. An alias must not be the leading string of other aliases.
//  When Draw commands are issued, the name of the corresponding histogram axes
//  will become the aliases of the expressions.
//
// User commands can be issued directly from the textbox labeled "Command"
// from the upper-left toolbar by typing and pressing Enter at the end.
//   An other way is from the right panel context menu : ExecuteCommand.
// All commands can be interrupted at any time by pressing the STOP button
// from the bottom-left
// You can toggle recording of the current command in the history file by
// checking the Rec button from the top-right
//
// Context menus
//
//   You can activate context menus by right-clicking on items or inside the
// right panel.
// Context menus for mapped items from the left tree-type list :
//   The items from the left that are provided with context menus are tree and
// branch items. You can directly activate the *MENU* marked methods of TTree
// from this menu.
// Context menu for the right panel :
//   A general context menu is acivated if the user right-clicks the right panel.
//   Commands are :
//   - EmptyAll        : clears the content of all expressions;
//   - ExecuteCommand  : execute a ROOT command;
//   - MakeSelector    : equivalent of TTree::MakeSelector();
//   - NewExpression   : add an expression item in the right panel;
//   - Process         : equivalent of TTree::Process();
//   - SaveSource      : save the current session as a C++ macro;
//   - SetScanFileName : define a name for the file where TTree::Scan command
//   is redirected when the <Scan> button is checked;
//   - SetTreeName     : open a new tree whith this name in the viewer;
//   A specific context menu is activated if expressions/leaves are right-clicked.
//   Commands are :
//   - Draw            : draw a histogram for this item;
//   - EditExpression  : pops-up the expression editor;
//   - Empty           : empty the name and alias of this item;
//   - RemoveItem      : removes clicked item from the list;
//   - Scan            : scan this expression;
//   - SetExpression   : edit name and alias for this item by hand;
//
// Starting the viewer
//
//   1) From the TBrowser :
//  Select a tree in the TBrowser, then call the StartViewer() method from its
// context menu (right-click on the tree).
//   2) From the command line :
//  Start a ROOT session in the directory where you have your tree.
// You will need first to load the library for TTreeViewer and optionally other
// libraries for user defined classes (you can do this later in the session) :
//    root [0] gSystem->Load(\"TTreeViewer\");
// Supposing you have the tree MyTree in the file MyFile, you can do :
//    root [1] TFile file(\"Myfile\");
//    root [2] new TTreeViewer(\"Mytree\");
// or :
//    root [2] TreeViewer *tv = new TTreeViewer();
//    root [3] tv->SetTreeName(\"Mytree\");
//
//Begin_Html
/*
<img src="treeview.gif">
*/
//End_Html
//

#ifdef HAVE_CONFIG
#include "config.h"
#endif

#include "snprintf.h"

#include "Riostream.h"
#include "TTreeViewer.h"
#include "HelpText.h"
#include "HelpTextTV.h"
#include "TTVLVContainer.h"
#include "TTVSession.h"

#include "TROOT.h"
#include "TError.h"
#include "TGMsgBox.h"
#include "TTreePlayer.h"
#include "TContextMenu.h"
#include "TInterpreter.h"
#include "TLeaf.h"
#include "TRootHelpDialog.h"
#include "TSystem.h"
#include "TApplication.h"
#include "TVirtualX.h"
#include "TGClient.h"
#include "TKey.h"
#include "TFile.h"
#include "TGMenu.h"
#include "TGFrame.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TTree.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TGButton.h"
#include "TGButtonGroup.h"
#include "TGTextEntry.h"
#include "TGComboBox.h"
#include "TGLabel.h"
#include "TGListView.h"
#include "TGListTree.h"
#include "TGMimeTypes.h"
#include "TGSplitter.h"
#include "TGDoubleSlider.h"
#include "TGToolBar.h"
#include "TGStatusBar.h"
#include "Getline.h"
#include "TTimer.h"
#include "TG3DLine.h"
#include "TGFileDialog.h"
#include "TGProgressBar.h"
#include "TClonesArray.h"

#ifdef WIN32
#include "TWin32SplashThread.h"
#endif

// drawing options
static const char* optgen[16] =
{
   "","AXIS","HIST","SAME","CYL","POL","SPH","PSR","LEGO","LEGO1","LEGO2",
   "SURF","SURF1","SURF2","SURF3","SURF4"
};
static const char* opt1D[12] =
{
   "","AH","B","C","E","E1","E2","E3","E4","L","P","*H"
};
static const char* opt2D[14] =
{
   "","ARR","BOX","COL","COL2","CONT","CONT0","CONT1","CONT2","CONT3",
   "FB","BB","SCAT","PROF"
};

static const char* gOpenTypes[] = {"Root files",   "*.root",
                                   0,              0       };

static const char* gMacroTypes[] = {"C++ macros",   "*.C",
                                   0,              0       };

// Menu command id's
enum ERootTreeViewerCommands {
   kFileCanvas,
   kFileBrowse,
   kFileLoadLibrary = 3,
   kFileOpenSession,
   kFileSaveMacro,
   kFilePrint,
   kFileClose,
   kFileQuit,

   kEditExpression,
   kEditCut,
   kEditMacro,
   kEditEvent,

   kRunCommand,
   kRunMacro,

   kOptionsReset,
   kOptionsGeneral = 20,
   kOptions1D = 50,
   kOptions2D = 70,

   kHelpAbout = 100,
   kHelpAboutTV,
   kHelpStart,
   kHelpLayout,
   kHelpOpenSave,
   kHelpDragging,
   kHelpEditing,
   kHelpSession,
   kHelpCommands,
   kHelpContext,
   kHelpDrawing,
   kHelpMacros,

   kBarCommand,
   kBarOption,
   kBarCut,
   kAxis
};

// button Id's
enum EButtonIdentifiers {
   kDRAW,
   kRESET,
   kSTOP,
   kCLOSE,
   kSLIDER,
   kBGFirst,
   kBGPrevious,
   kBGRecord,
   kBGNext,
   kBGLast
};

ClassImp(TTreeViewer)

//______________________________________________________________________________
TTreeViewer::TTreeViewer(const char* treeName)
          :TGMainFrame(0,10,10,kVerticalFrame)
{
   // TTreeViewer default constructor

   fTree = 0;
   if (!gClient) return;
   char command[128];
   sprintf(command, "TTreeViewer *gTV = (TTreeViewer*)0x%lx", (ULong_t)this);
   gROOT->ProcessLine(command);
   gROOT->ProcessLine("TTree *tv__tree = 0;");
   fTreeList = new TList;
   gROOT->ProcessLine("TList *tv__tree_list = new TList;");
   fFilename = 0;
   gROOT->ProcessLine("TFile *tv__tree_file = 0;");
   gInterpreter->SaveContext();
   BuildInterface();
   SetTreeName(treeName);
}

//______________________________________________________________________________
TTreeViewer::TTreeViewer(const TTree *tree)
          :TGMainFrame(gClient->GetRoot(),10,10,kVerticalFrame)
{
   // TTreeViewer constructor with a pointer to a Tree

   fTree = 0;
   char command[128];
   sprintf(command, "TTreeViewer *gTV = (TTreeViewer*)0x%lx", (ULong_t)this);
   gROOT->ProcessLine(command);
   if (!tree) return;
   gROOT->ProcessLine("TTree *tv__tree = 0;");
   fTreeList = new TList;
   gROOT->ProcessLine("TList *tv__tree_list = new TList;");
   fFilename = 0;
   gROOT->ProcessLine("TFile *tv__tree_file = 0;");
   gInterpreter->SaveContext();
   BuildInterface();
   TDirectory *dirsav = gDirectory;
   TDirectory *cdir = tree->GetDirectory();
   if (cdir) cdir->cd();

   SetTreeName(tree->GetName());
   // If the tree is a chain, the tree directory will be changed by SwitchTree
   // (called by SetTreeName)
   cdir = tree->GetDirectory();
   if (cdir) {
      if (cdir->GetFile()) fFilename = cdir->GetFile()->GetName();
   }
   if (dirsav) dirsav->cd();
}
//______________________________________________________________________________
void TTreeViewer::AppendTree(TTree *tree)
{
   // Allow geting the tree from the context menu.

   if (!tree) return;
   TTree *ftree;
   if (fTreeList) {
      if (fTreeList->FindObject(tree)) {
         printf("Tree found\n");
         TIter next(fTreeList);
         Int_t index = 0;
         while ((ftree = (TTree*)next())) {
            if (ftree==tree) {printf("found at index %i\n", index);break;}
            index++;
         }
         SwitchTree(index);
         if (fTree != fMappedTree) {
            // switch also the global "tree" variable
            fLVContainer->RemoveNonStatic();
            // map it on the right panel
            MapTree(fTree);
            fListView->Layout();
            TGListTreeItem *base = 0;
            TGListTreeItem *parent = fLt->FindChildByName(base, "TreeList");
            TGListTreeItem *item = fLt->FindChildByName(parent, fTree->GetName());
            fLt->ClearHighlighted();
            fLt->HighlightItem(item);
            fClient->NeedRedraw(fLt);
         }
         return;
      }
   }
   if (fTree != tree) {
      fTree = tree;
      // load the tree via the interpreter
      char command[100];
      command[0] = 0;
      // define a global "tree" variable for the same tree
      sprintf(command, "tv__tree = (TTree *)0x%lx;", (ULong_t)tree);
      ExecuteCommand(command);
   }
   //--- add the tree to the list if it is not already in
   fTreeList->Add(fTree);
   ExecuteCommand("tv__tree_list->Add(tv__tree);");
   //--- map this tree
   TGListTreeItem *base = 0;
   TGListTreeItem *parent = fLt->FindChildByName(base, "TreeList");
   if (!parent) parent = fLt->AddItem(base, "TreeList", new ULong_t(kLTNoType));
   ULong_t *itemType = new ULong_t((fTreeIndex << 8) | kLTTreeType);
   fTreeIndex++;
   TGListTreeItem *lTreeItem = fLt->AddItem(parent, tree->GetName(), itemType,
               gClient->GetPicture("tree_t.xpm"), gClient->GetPicture("tree_t.xpm"));
   MapTree(fTree, lTreeItem, kFALSE);
   fLt->OpenItem(parent);
   fLt->HighlightItem(lTreeItem);
   fClient->NeedRedraw(fLt);

   //--- map slider and list view
   SwitchTree(fTreeIndex-1);
   fLVContainer->RemoveNonStatic();
   MapTree(fTree);
   fListView->Layout();
   SetFile();
}
//______________________________________________________________________________
void TTreeViewer::SetNexpressions(Int_t expr)
{
   // Change the number of expression widgets.

   Int_t diff = expr - fNexpressions;
   if (diff <= 0) return;
   if (!fLVContainer) return;
   for (Int_t i=0; i<TMath::Abs(diff); i++) NewExpression();
}
//______________________________________________________________________________
void TTreeViewer::SetScanFileName(const char *name)
{
   // Set the name of the file where to redirect <Scan> output.

   if (fTree) ((TTreePlayer *)fTree->GetPlayer())->SetScanFileName(name);
}
//______________________________________________________________________________
void TTreeViewer::SetScanRedirect(Bool_t mode)
{
   // Set the state of Scan check button.

   if (mode)
      fBarScan->SetState(kButtonDown);
   else
      fBarScan->SetState(kButtonUp);
}
//______________________________________________________________________________
void TTreeViewer::SetTreeName(const char* treeName)
{
   // Allow geting the tree from the context menu.

   if (!treeName) return;
   TTree *tree = (TTree *) gROOT->FindObject(treeName);
   if (fTreeList) {
      if (fTreeList->FindObject(treeName)) {
         printf("Tree found\n");
         TIter next(fTreeList);
         Int_t index = 0;
         while ((tree = (TTree*)next())) {
            if (!strcmp(treeName, tree->GetName())) {printf("found at index %i\n", index);break;}
            index++;
         }
         SwitchTree(index);
         if (fTree != fMappedTree) {
            // switch also the global "tree" variable
            fLVContainer->RemoveNonStatic();
            // map it on the right panel
            MapTree(fTree);
            fListView->Layout();
            TGListTreeItem *base = 0;
            TGListTreeItem *parent = fLt->FindChildByName(base, "TreeList");
            TGListTreeItem *item = fLt->FindChildByName(parent, fTree->GetName());
            fLt->ClearHighlighted();
            fLt->HighlightItem(item);
            fClient->NeedRedraw(fLt);
         }
         return;
      }
   }
   if (!tree) return;
//   ((TTreePlayer *)tree->GetPlayer())->SetViewer(this);
   if (fTree != tree) {
      fTree = tree;
      // load the tree via the interpreter
      char command[100];
      command[0] = 0;
      // define a global "tree" variable for the same tree
      sprintf(command, "tv__tree = (TTree *) gROOT->FindObject(\"%s\");", treeName);
      ExecuteCommand(command);
   }
   //--- add the tree to the list if it is not already in
   fTreeList->Add(fTree);
   ExecuteCommand("tv__tree_list->Add(tv__tree);");
   //--- map this tree
   TGListTreeItem *base = 0;
   TGListTreeItem *parent = fLt->FindChildByName(base, "TreeList");
   if (!parent) parent = fLt->AddItem(base, "TreeList", new ULong_t(kLTNoType));
   ULong_t *itemType = new ULong_t((fTreeIndex << 8) | kLTTreeType);
   fTreeIndex++;
   TGListTreeItem *lTreeItem = fLt->AddItem(parent, treeName, itemType,
               gClient->GetPicture("tree_t.xpm"), gClient->GetPicture("tree_t.xpm"));
   MapTree(fTree, lTreeItem, kFALSE);
   fLt->OpenItem(parent);
   fLt->HighlightItem(lTreeItem);
   fClient->NeedRedraw(fLt);

   //--- map slider and list view
   SwitchTree(fTreeIndex-1);
   fLVContainer->RemoveNonStatic();
   MapTree(fTree);
   fListView->Layout();
   SetFile();
}
//______________________________________________________________________________
void TTreeViewer::SetFile()
{
   // Set file name containing the tree.

   if (!fTree) return;
   TSeqCollection *list = gROOT->GetListOfFiles();
   TTree *tree;
   TIter next(list);
   TObject *obj;
   TFile   *file;
   while ((obj=next())) {
      file = (TFile*)obj;
      if (file) {
         tree = (TTree*)file->Get(fTree->GetName());
         if (tree) {
            fFilename = file->GetName();
            cout << "File name : "<< fFilename << endl;
            return;
         } else {
            fFilename = 0;
         }
      }
   }
   fFilename = 0;
}
//______________________________________________________________________________
void TTreeViewer::BuildInterface()
{
   // Create all viewer widgets.

   //--- timer & misc
   fCounting = kFALSE;
   fScanMode = kFALSE;
   fEnableCut = kTRUE;
   fTimer = new TTimer(this, 20, kTRUE);
   fLastOption = "";
   fSession = new TTVSession(this);
   //--- cursors
   fDefaultCursor = gVirtualX->CreateCursor(kPointer);
   fWatchCursor = gVirtualX->CreateCursor(kWatch);
   //--- colours
   ULong_t color;
   gClient->GetColorByName("blue",color);
   //--- pictures for X, Y and Z expression items
   fPicX = gClient->GetPicture("x_pic.xpm");
   fPicY = gClient->GetPicture("y_pic.xpm");
   fPicZ = gClient->GetPicture("z_pic.xpm");

   //--- general context menu
   fContextMenu = new TContextMenu("TreeViewer context menu","");
   fMappedTree = 0;
   fMappedBranch = 0;
   fDialogBox = 0;
   fDimension = 0;
   fVarDraw = kFALSE;
   fStopMapping = kFALSE;
//   fFilename = 0;
   fSourceFile = "treeviewer.C";
   //--- lists : trees and widgets to be removed
//   fTreeList = 0;
   fTreeIndex = 0;
   fWidgets = new TList();
   //--- create menus --------------------------------------------------------
   //--- File menu
   fFileMenu = new TGPopupMenu(fClient->GetRoot());
   fFileMenu->AddEntry("&New canvas",      kFileCanvas);
   fFileMenu->AddEntry("Open &tree file...", kFileBrowse);
   fFileMenu->AddEntry("&Load Library...", kFileLoadLibrary);
   fFileMenu->AddEntry("&Open session",   kFileOpenSession);
   fFileMenu->AddEntry("&Save source",    kFileSaveMacro);
   fFileMenu->AddSeparator();
   fFileMenu->AddEntry("&Print",           kFilePrint);
   fFileMenu->AddEntry("&Close",           kFileClose);
   fFileMenu->AddSeparator();
   fFileMenu->AddEntry("&Quit ROOT",       kFileQuit);

   fFileMenu->DisableEntry(kFilePrint);

   //--- Edit menu
   fEditMenu = new TGPopupMenu(gClient->GetRoot());
   fEditMenu->AddEntry("&Expression...",   kEditExpression);
   fEditMenu->AddEntry("&Cut...",          kEditCut);
   fEditMenu->AddEntry("&Macro...",        kEditMacro);
   fEditMenu->AddEntry("E&Vent...",        kEditEvent);

   fEditMenu->DisableEntry(kEditMacro);
   fEditMenu->DisableEntry(kEditEvent);
   //---Run menu
   fRunMenu = new TGPopupMenu(gClient->GetRoot());
   fRunMenu->AddEntry("&Macro...",         kRunMacro);
   fRunMenu->DisableEntry(kRunMacro);
   //--- Options menu
   //--- General options
   fOptionsGen = new TGPopupMenu(gClient->GetRoot());
   fOptionsGen->AddEntry("Default",        kOptionsGeneral);
   fOptionsGen->AddSeparator();
   fOptionsGen->AddEntry("Axis only",      kOptionsGeneral+1);  // "AXIS"
   fOptionsGen->AddEntry("Contour only",   kOptionsGeneral+2);  // "HIST"
   fOptionsGen->AddEntry("Superimpose",    kOptionsGeneral+3);  //"SAME"
   fOptionsGen->AddEntry("Cylindrical",    kOptionsGeneral+4);  //"CYL"
   fOptionsGen->AddEntry("Polar",          kOptionsGeneral+5);  //"POL"
   fOptionsGen->AddEntry("Spherical",      kOptionsGeneral+6);  //"SPH"
   fOptionsGen->AddEntry("PsRap/Phi",      kOptionsGeneral+7);  //"PSR"
   fOptionsGen->AddEntry("Lego HLR",       kOptionsGeneral+8);  //"LEGO"
   fOptionsGen->AddEntry("Lego HSR",       kOptionsGeneral+9);  //"LEGO1"
   fOptionsGen->AddEntry("Lego Color",     kOptionsGeneral+10); //"LEGO2"
   fOptionsGen->AddEntry("Surface HLR",    kOptionsGeneral+11); //"SURF"
   fOptionsGen->AddEntry("Surface HSR",    kOptionsGeneral+12); //"SURF1"
   fOptionsGen->AddEntry("Surface Col",    kOptionsGeneral+13); //"SURF2"
   fOptionsGen->AddEntry("Surf+Cont",      kOptionsGeneral+14); //"SURF3"
   fOptionsGen->AddEntry("Gouraud",        kOptionsGeneral+15); //"SURF4"
   fOptionsGen->Associate(this);
   //--- 1D options
   fOptions1D = new TGPopupMenu(gClient->GetRoot());
   fOptions1D->AddEntry("Default",         kOptions1D);
   fOptions1D->AddSeparator();
   fOptions1D->AddEntry("No labels/ticks", kOptions1D+1);       // "AH"
   fOptions1D->AddEntry("Bar chart",       kOptions1D+2);       // "B"
   fOptions1D->AddEntry("Smooth curve",    kOptions1D+3);       // "C"
   fOptions1D->AddEntry("Errors",          kOptions1D+4);       // "E"
   fOptions1D->AddEntry("Errors 1",        kOptions1D+5);       // "E1"
   fOptions1D->AddEntry("Errors 2",        kOptions1D+6);       // "E2"
   fOptions1D->AddEntry("Errors 3",        kOptions1D+7);       // "E3"
   fOptions1D->AddEntry("Errors 4",        kOptions1D+8);       // "E4"
   fOptions1D->AddEntry("Line",            kOptions1D+9);       // "L"
   fOptions1D->AddEntry("Markers",         kOptions1D+10);      // "P"
   fOptions1D->AddEntry("Stars",           kOptions1D+11);      // "*H"
   fOptions1D->Associate(this);
   //--- 2D options
   fOptions2D = new TGPopupMenu(gClient->GetRoot());
   fOptions2D->AddEntry("Default",         kOptions2D);
   fOptions2D->AddSeparator();
   fOptions2D->AddEntry("Arrows",          kOptions2D+1);       // "ARR"
   fOptions2D->AddEntry("Box/Surf",        kOptions2D+2);       // "BOX"
   fOptions2D->AddEntry("Box/Color",       kOptions2D+3);       // "COL"
   fOptions2D->AddEntry("Box/ColMap",      kOptions2D+4);       // "COLZ"
   fOptions2D->AddEntry("Contour",         kOptions2D+5);       // "CONT"
   fOptions2D->AddEntry("Contour 0",       kOptions2D+6);       // "CONT0"
   fOptions2D->AddEntry("Contour 1",       kOptions2D+7);       // "CONT1"
   fOptions2D->AddEntry("Contour 2",       kOptions2D+8);       // "CONT2"
   fOptions2D->AddEntry("Contour 3",       kOptions2D+9);       // "CONT3"
   fOptions2D->AddEntry("No front-box",    kOptions2D+10);      // "FB"
   fOptions2D->AddEntry("No back-box",     kOptions2D+11);      // "BB"
   fOptions2D->AddEntry("Scatter",         kOptions2D+12);      // "SCAT"
   fOptions2D->AddEntry("Profile",         kOptions2D+13);      // "SCAT"
   fOptions2D->Associate(this);

   fOptionsMenu = new TGPopupMenu(gClient->GetRoot());
   fOptionsMenu->AddPopup("&General Options...", fOptionsGen);
   fOptionsMenu->AddPopup("&1D Options",         fOptions1D);
   fOptionsMenu->AddPopup("&2D Options",         fOptions2D);
   fOptionsMenu->AddSeparator();
   fOptionsMenu->AddEntry("&Reset options",      kOptionsReset);
   //--- Help menu
   fHelpMenu = new TGPopupMenu(gClient->GetRoot());
   fHelpMenu->AddEntry("&About ROOT...",         kHelpAbout);
   fHelpMenu->AddEntry("&About TreeViewer...",   kHelpAboutTV);
   fHelpMenu->AddSeparator();
   fHelpMenu->AddEntry("&Starting...",           kHelpStart);
   fHelpMenu->AddEntry("&Layout...",             kHelpLayout);
   fHelpMenu->AddEntry("&Open/Save",             kHelpOpenSave);
   fHelpMenu->AddEntry("&Dragging...",           kHelpDragging);
   fHelpMenu->AddEntry("&Editing expressions...",kHelpEditing);
   fHelpMenu->AddEntry("&Session...",            kHelpSession);
   fHelpMenu->AddEntry("&User commands...",      kHelpCommands);
   fHelpMenu->AddEntry("&Context menus...",      kHelpContext);
   fHelpMenu->AddEntry("D&rawing...",            kHelpDrawing);
   fHelpMenu->AddEntry("&Macros...",             kHelpMacros);

   fFileMenu->Associate(this);
   fEditMenu->Associate(this);
   fRunMenu->Associate(this);
   fOptionsMenu->Associate(this);
   fHelpMenu->Associate(this);

   //--- menubar layout hints
   fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX, 0,0,1,1);
   fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
   fMenuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);
   //--- create menubar and add popup menus
   fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);

   fMenuBar->AddPopup("&File", fFileMenu, fMenuBarItemLayout);
   fMenuBar->AddPopup("&Edit", fEditMenu, fMenuBarItemLayout);
   fMenuBar->AddPopup("&Run",  fRunMenu,  fMenuBarItemLayout);
   fMenuBar->AddPopup("&Options", fOptionsMenu, fMenuBarItemLayout);
   fMenuBar->AddPopup("&Help", fHelpMenu, fMenuBarHelpLayout);

   AddFrame(fMenuBar, fMenuBarLayout);
   //--- toolbar ----------------------------------------------------------------
   fToolBar = new TGToolBar(this, 10, 10, kHorizontalFrame);
   fBarLayout = new TGLayoutHints(kLHintsTop | kLHintsExpandX);

   TGLayoutHints *lo;
   lo = new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 4,4,0,0);
   fWidgets->Add(lo);
   //--- label for Command text entry
   fBarLbl1 = new TGLabel(fToolBar,"Command");
   fToolBar->AddFrame(fBarLbl1,lo);
   //--- command text entry
   fBarCommand = new TGTextEntry(fToolBar, new TGTextBuffer(250),kBarCommand);
   fBarCommand->SetWidth(120);
   fBarCommand->Associate(this);
   fBarCommand->SetToolTipText("User commands executed via interpreter. Type <ENTER> to execute");
   fToolBar->AddFrame(fBarCommand, lo);
   //--- first vertical separator
   TGVertical3DLine *vSeparator = new TGVertical3DLine(fToolBar);
   lo = new TGLayoutHints(kLHintsLeft | kLHintsExpandY, 4,4,0,0);
   fWidgets->Add(lo);
   fWidgets->Add(vSeparator);
   fToolBar->AddFrame(vSeparator, lo);

   lo = new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 4,4,0,0);
   fWidgets->Add(lo);
   //--- label for Option text entry
   fBarLbl2 = new TGLabel(fToolBar,"Option");
   fToolBar->AddFrame(fBarLbl2, lo);
   //--- drawing option text entry
   fBarOption = new TGTextEntry(fToolBar, new TGTextBuffer(200),kBarOption);
   fBarOption->SetWidth(100);
   fBarOption->Associate(this);
   fBarOption->SetToolTipText("Histogram graphics option. Type option here and click <Draw> (or  <ENTER> to update current histogram).");
   fToolBar->AddFrame(fBarOption, lo);
   //--- second vertical separator
   vSeparator = new TGVertical3DLine(fToolBar);
   lo = new TGLayoutHints(kLHintsLeft | kLHintsExpandY, 4,4,0,0);
   fWidgets->Add(lo);
   fWidgets->Add(vSeparator);
   fToolBar->AddFrame(vSeparator, lo);

   lo = new TGLayoutHints(kLHintsLeft | kLHintsCenterY, 4,4,0,0);
   fWidgets->Add(lo);
   //--- label for Histogram text entry
   fBarLbl3 = new TGLabel(fToolBar,"Histogram");
   fToolBar->AddFrame(fBarLbl3, lo);
   //--- histogram name text entry
   fBarHist = new TGTextEntry(fToolBar, new TGTextBuffer(100));
   fBarHist->SetWidth(50);
   fBarHist->SetText("htemp");
   fBarHist->SetToolTipText("Name of the histogram created by <Draw> command.");
   fToolBar->AddFrame(fBarHist, lo);
   //--- Hist check button
   fBarH = new TGCheckButton(fToolBar, "Hist");
   fBarH->SetToolTipText("Checked : redraw only current histogram");
   fBarH->SetState(kButtonUp);
   fToolBar->AddFrame(fBarH, lo);
   //--- Scan check button
   fBarScan = new TGCheckButton(fToolBar, "Scan");
   fBarScan->SetState(kButtonUp);
   fBarScan->SetToolTipText("Check to redirect TTree::Scan command in a file");
   fToolBar->AddFrame(fBarScan, lo);
   //--- Rec check button
   fBarRec = new TGCheckButton(fToolBar, "Rec");
   fBarRec->SetState(kButtonDown);
   fBarRec->SetToolTipText("Check to record commands in history file and be verbose");
   fToolBar->AddFrame(fBarRec, lo);
   //--- 1'st horizontal tool bar separator ----------------------------------------
   TGHorizontal3DLine *toolBarSep = new TGHorizontal3DLine(this);
   fWidgets->Add(toolBarSep);
   AddFrame(toolBarSep, fBarLayout);
   AddFrame(fToolBar, fBarLayout);
   //--- 2'nd horizontal tool bar separator ----------------------------------------
   toolBarSep = new TGHorizontal3DLine(this);
   fWidgets->Add(toolBarSep);
   AddFrame(toolBarSep, fBarLayout);

   //--- Horizontal mother frame ---------------------------------------------------
   fHf = new TGHorizontalFrame(this, 10, 10);
   //--- Vertical frames
   fSlider = new TGDoubleVSlider(fHf, 10, kDoubleScaleBoth, kSLIDER);
//   fSlider->SetBackgroundColor(color);
   fSlider->Associate(this);

   //--- fV1 -----------------------------------------------------------------------
   fV1 = new TGVerticalFrame(fHf, 10, 10, kFixedWidth);
   fTreeHdr = new TGCompositeFrame(fV1, 10, 10, kSunkenFrame | kVerticalFrame);

   fLbl1 = new TGLabel(fTreeHdr, "Current Folder");
   lo = new TGLayoutHints(kLHintsLeft | kLHintsTop | kLHintsCenterY, 3, 0, 0, 0);
   fWidgets->Add(lo);
   fTreeHdr->AddFrame(fLbl1, lo);

   lo = new TGLayoutHints(kLHintsTop | kLHintsExpandX, 2, 0, 1, 0);
   fWidgets->Add(lo);
   fV1->AddFrame(fTreeHdr, lo);

   //--- tree view canvas on the left
   fTreeView = new TGCanvas(fV1, fV1->GetWidth(), 10, kSunkenFrame | kDoubleBorder);
   //--- container frame
   fLt = new TGListTree(fTreeView->GetViewPort(), 10, 10, kHorizontalFrame,
                        GetWhitePixel());
   fLt->Associate(this);
   fTreeView->SetContainer(fLt);

   lo = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2,0,0,0);
   fWidgets->Add(lo);
   fV1->AddFrame(fTreeView, lo);

   //--- button horizontal frame
   fHpb = new TGHorizontalFrame(fV1, fTreeHdr->GetWidth(), 10, kSunkenFrame);

   //--- DRAW button
   fPicDraw = gClient->GetPicture("draw_t.xpm");
   fDRAW  = new TGPictureButton(fHpb,fPicDraw,kDRAW);
   fDRAW->SetToolTipText("Draw current selection");
   fDRAW->Associate(this);

   lo = new TGLayoutHints(kLHintsTop | kLHintsLeft, 2,2,4,2);
   fWidgets->Add(lo);
   fHpb->AddFrame(fDRAW, lo);

   //--- STOP button (breaks current operation)
//   fPicStop = gClient->GetPicture("mb_stop_s.xpm");
   fPicStop = gClient->GetPicture("stop_t.xpm");
   fSTOP  = new TGPictureButton(fHpb,fPicStop,kSTOP);
   fSTOP->SetToolTipText("Abort current operation");
   fSTOP->Associate(this);

   lo = new TGLayoutHints(kLHintsTop | kLHintsLeft, 2,2,4,2);
   fWidgets->Add(lo);
   fHpb->AddFrame(fSTOP, lo);

   //--- REFR button (breaks current operation)
   fPicRefr = gClient->GetPicture("refresh2.xpm");
   fREFR  = new TGPictureButton(fHpb,fPicRefr,kDRAW);
   fREFR->SetToolTipText("Update the tree viewer");
   lo = new TGLayoutHints(kLHintsTop | kLHintsLeft, 2,2,4,2);
   fWidgets->Add(lo);
   fHpb->AddFrame(fREFR, lo);
   //---connect REFR button to DoRefresh() method
   fREFR->Connect("Clicked()", "TTreeViewer", this, "DoRefresh()");

   lo = new TGLayoutHints(kLHintsTop | kLHintsLeft, 2,2,2,2);
   fWidgets->Add(lo);
   fV1->AddFrame(fHpb, lo);

   //--- fV2
   fV2 = new TGVerticalFrame(fHf, 10, 10);
   fListHdr = new TGCompositeFrame(fV2, 10, 10, kSunkenFrame | kFitHeight);
   fLbl2 = new TGLabel(fListHdr, "Current Tree:                 ");
   lo = new TGLayoutHints(kLHintsTop | kLHintsLeft, 3, 0, 0, 0);
   fWidgets->Add(lo);
   fListHdr->AddFrame(fLbl2, lo);

   //--- progress bar
   fProgressBar = new TGHProgressBar(fListHdr);
   fProgressBar->SetBarColor("red");
   fProgressBar->SetFillType(TGProgressBar::kBlockFill);
   lo = new TGLayoutHints(kLHintsBottom | kLHintsExpandX, 2,2,4,2);
   fWidgets->Add(lo);
   fListHdr->AddFrame(fProgressBar, lo);
   lo = new TGLayoutHints(kLHintsTop | kLHintsExpandX | kLHintsExpandY, 2,0,1,2);
   fWidgets->Add(lo);
   fV2->AddFrame(fListHdr, lo);

   fV1->Resize(fTreeHdr->GetDefaultWidth()+100, fV1->GetDefaultHeight());
   lo = new TGLayoutHints(kLHintsLeft | kLHintsExpandY);
   fWidgets->Add(lo);
   fHf->AddFrame(fSlider, lo);
   lo = new TGLayoutHints(kLHintsLeft | kLHintsExpandY);
   fWidgets->Add(lo);
   fHf->AddFrame(fV1, lo);

   //--- vertical splitter
   TGVSplitter *splitter = new TGVSplitter(fHf);
   splitter->SetFrame(fV1,kTRUE);
   lo = new TGLayoutHints(kLHintsLeft | kLHintsExpandY);
   fWidgets->Add(splitter);
   fWidgets->Add(lo);
   fHf->AddFrame(splitter,lo);



   //-- listview for the content of the tree/branch -----------------------------
   fListView = new TGListView(fListHdr,400,300);
   //--- container frame
   fLVContainer = new TTVLVContainer(fListView->GetViewPort(),400,300);
   fLVContainer->Associate(this);
   fLVContainer->SetListView(fListView);
   fLVContainer->SetViewer(this);
   fLVContainer->SetBackgroundColor(GetWhitePixel());
   fListView->GetViewPort()->SetBackgroundColor(GetWhitePixel());
   fListView->SetContainer(fLVContainer);
   fListView->SetViewMode(kLVList);
   lo = new TGLayoutHints(kLHintsRight | kLHintsTop | kLHintsExpandX | kLHintsExpandY);
   fWidgets->Add(lo);

   fListHdr->AddFrame(fListView,lo);

   lo = new TGLayoutHints(kLHintsRight | kLHintsExpandX | kLHintsExpandY);
   fWidgets->Add(lo);
   fHf->AddFrame(fV2,lo);

   AddFrame(fHf, lo);
   //--- 3rd horizontal tool bar separator ----------------------------------------
   toolBarSep = new TGHorizontal3DLine(this);
   fWidgets->Add(toolBarSep);
   AddFrame(toolBarSep, fBarLayout);

   //--- label for IList text entry
   fBFrame = new TGHorizontalFrame(this,10,10);
   fBLbl4 = new TGLabel(fBFrame,"IList");
   lo = new TGLayoutHints(kLHintsLeft | kLHintsBottom, 2,2,2,2);
   fWidgets->Add(lo);
   fBFrame->AddFrame(fBLbl4, lo);
   //--- IList text entry
   fBarListIn =  new TGTextEntry(fBFrame, new TGTextBuffer(100));
   fBarListIn->SetWidth(60);
   fBarListIn->SetToolTipText("Name of a previously created event list");
   fBFrame->AddFrame(fBarListIn, lo);
   //--- label for OList text entry
   fBLbl5 = new TGLabel(fBFrame,"OList");
   fBFrame->AddFrame(fBLbl5, lo);
   //--- OList text entry
   fBarListOut =  new TGTextEntry(fBFrame, new TGTextBuffer(100));
   fBarListOut->SetWidth(60);
   fBarListOut->SetToolTipText("Output event list. Use <Draw> to generate it.");
   fBFrame->AddFrame(fBarListOut, lo);
   //--- Status bar
   fStatusBar = new TGStatusBar(fBFrame, 10, 10);
   fStatusBar->SetWidth(200);
   fStatusBar->Draw3DCorner(kFALSE);
   lo = new TGLayoutHints(kLHintsCenterX | kLHintsCenterY | kLHintsLeft | kLHintsExpandX, 2,2,2,2);
   fWidgets->Add(lo);
   fBFrame->AddFrame(fStatusBar, lo);
   //--- RESET button
   fReset = new TGTextButton(fBFrame,"RESET",kRESET);
   fReset->SetToolTipText("Reset variable's fields and drawing options");
   fReset->Associate(this);
   lo = new TGLayoutHints(kLHintsTop | kLHintsRight, 2,2,2,2);
   fWidgets->Add(lo);
   fBFrame->AddFrame(fReset,lo);
   //---  group of buttons for session handling
   fBGFirst = new TGPictureButton(fBFrame,
                                  gClient->GetPicture("first_t.xpm"), kBGFirst);
   fBGFirst->SetToolTipText("First record");
   fBGFirst->Associate(this);
   fBGPrevious = new TGPictureButton(fBFrame,
                                  gClient->GetPicture("previous_t.xpm"), kBGPrevious);
   fBGPrevious->SetToolTipText("Previous record");
   fBGPrevious->Associate(this);
   fBGRecord = new TGPictureButton(fBFrame,
                                  gClient->GetPicture("record_t.xpm"), kBGRecord);
   fBGRecord->SetToolTipText("Record");
   fBGRecord->Associate(this);
   fBGNext = new TGPictureButton(fBFrame,
                                 gClient->GetPicture("next_t.xpm"), kBGNext);
   fBGNext->SetToolTipText("Next record");
   fBGNext->Associate(this);
   fBGLast = new TGPictureButton(fBFrame,
                                 gClient->GetPicture("last_t.xpm"), kBGLast);
   fBGLast->SetToolTipText("Last record");
   fBGLast->Associate(this);

   fCombo = new TGComboBox(fBFrame, 0);
   fCombo->SetHeight(fReset->GetDefaultHeight());
   fCombo->SetWidth(100);
   fCombo->Associate(this);

   lo = new TGLayoutHints(kLHintsCenterY | kLHintsRight, 0,0,2,0);
   fWidgets->Add(lo);
   fBFrame->AddFrame(fCombo,      lo);
   fBFrame->AddFrame(fBGLast,     lo);
   fBFrame->AddFrame(fBGNext,     lo);
   fBFrame->AddFrame(fBGRecord,   lo);
   fBFrame->AddFrame(fBGPrevious, lo);
   fBFrame->AddFrame(fBGFirst,    lo);
   lo = new TGLayoutHints(kLHintsExpandX,2,2,2,0);
   fWidgets->Add(lo);
   AddFrame(fBFrame,lo);

   // map the window
   SetWindowName("TreeViewer");
   MapSubwindows();
   Resize(GetDefaultSize());
   MapWindow();

   // put default items in the listview on the right
   const TGPicture *pic, *spic;

   fLVContainer->RemoveAll();
   TTVLVEntry* entry;
   Char_t symbol;
   entry = new TTVLVEntry(fLVContainer,fPicX,fPicX,new TGString(),0,kLVSmallIcons);
   symbol = 'X';
   entry->SetUserData(new ULong_t((symbol << 8) | kLTExpressionType | kLTTreeType));
   entry->SetToolTipText("X expression. Drag and drop expressions here");
   //--- X item
   fLVContainer->AddThisItem(entry);
   entry->Empty();
   entry->MapWindow();

   entry = new TTVLVEntry(fLVContainer,fPicY,fPicY,new TGString(),0,kLVSmallIcons);
   symbol = 'Y';
   entry->SetUserData(new ULong_t((symbol << 8) | kLTExpressionType | kLTTreeType));
   entry->SetToolTipText("Y expression. Drag and drop expressions here");
   //--- Y item
   fLVContainer->AddThisItem(entry);
   entry->Empty();
   entry->MapWindow();

   entry = new TTVLVEntry(fLVContainer,fPicZ,fPicZ,new TGString(),0,kLVSmallIcons);
   symbol = 'Z';
   entry->SetUserData(new ULong_t((symbol << 8) | kLTExpressionType | kLTTreeType));
   entry->SetToolTipText("Z expression. Drag and drop expressions here");
   //--- Z item
   fLVContainer->AddThisItem(entry);
   entry->Empty();
   entry->MapWindow();

   pic = gClient->GetPicture("cut_t.xpm");
   spic = gClient->GetPicture("cut_t.xpm");
   entry = new TTVLVEntry(fLVContainer,pic,spic,new TGString(),0,kLVSmallIcons);
   entry->SetUserData(new ULong_t(kLTExpressionType | kLTCutType));
   entry->SetToolTipText("Active cut. Double-click to enable/disable");
   //--- Cut item (scissors icon)
   fLVContainer->AddThisItem(entry);
   entry->Empty();
   entry->MapWindow();

   pic = gClient->GetPicture("pack_t.xpm");
   spic = gClient->GetPicture("pack-empty_t.xpm");
   entry = new TTVLVEntry(fLVContainer,pic,spic,new TGString("Scan box"),0,kLVSmallIcons);
   entry->SetUserData(new ULong_t(kLTExpressionType | kLTPackType));
   entry->SetToolTipText("Drag and drop expressions/leaves here. Double-click to scan. Check <Scan> to redirect on file.");
   //--- Scan Box
   fLVContainer->AddThisItem(entry);
   entry->MapWindow();
   entry->SetTrueName("");

   //--- 10 expression items
   fNexpressions = 10;
   for (Int_t i=0; i<fNexpressions; i++) {
      pic = gClient->GetPicture("expression_t.xpm");
      spic = gClient->GetPicture("expression_t.xpm");
      entry = new TTVLVEntry(fLVContainer,pic,spic,new TGString(),0,kLVSmallIcons);
      entry->SetUserData(new ULong_t(kLTExpressionType | kLTDragType));
      entry->SetToolTipText("User defined expression/cut. Double-click to edit");
      fLVContainer->AddThisItem(entry);
      entry->Empty();
      entry->MapWindow();
   }

   fListView->Layout();
   fListView->Resize();
//   EmptyAll();
   // map the tree if it was supplied in the constructor

   SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputModeless);
   if (!fTree) {
      fSlider->SetRange(0,1000000);
      fSlider->SetPosition(0,1000000);
   } else {
      fSlider->SetRange(0,fTree->GetEntries()-1);
      fSlider->SetPosition(0,fTree->GetEntries()-1);
   }
   PrintEntries();
   fProgressBar->SetPosition(0);
   fProgressBar->ShowPosition();
   ActivateButtons(kFALSE, kFALSE, kFALSE, kFALSE);

   // map the window
   ///SetWindowName("TreeViewer");
   MapSubwindows();
   Resize(GetDefaultSize());
   MapWindow();
}

//______________________________________________________________________________
TTreeViewer::~TTreeViewer()
{
   // TTreeViewer destructor.

   if (!gClient) return;
   gClient->FreePicture(fPicX);
   gClient->FreePicture(fPicY);
   gClient->FreePicture(fPicZ);
   gClient->FreePicture(fPicDraw);
   gClient->FreePicture(fPicStop);
   gClient->FreePicture(fPicRefr);

   fDialogBox = TGSelectBox::GetInstance();
   if (fDialogBox) delete fDialogBox;

   delete fContextMenu;

   delete fBarLbl1;
   delete fBarLbl2;
   delete fBarLbl3;
   delete fBLbl4;
   delete fBLbl5;
   delete fBarCommand;
   delete fBarOption;
   delete fBarHist;
   delete fBarListIn;
   delete fBarListOut;

   delete fBarH;
   delete fBarScan;
   delete fBarRec;

   delete fToolBar;

   delete fSlider;
   delete fV1;
   delete fV2;
   delete fLbl1;
   delete fLbl2;
   delete fHf;
   delete fTreeHdr;
   delete fListHdr;
   delete fLt;
   delete fTreeView;
   delete fLVContainer;
   delete fListView;

   delete fProgressBar;
   delete fHpb;

   delete fDRAW;
   delete fSTOP;
   delete fReset;
   delete fBGFirst;
   delete fBGPrevious;
   delete fBGRecord;
   delete fBGNext;
   delete fBGLast;
   delete fCombo;
   delete fBFrame;

   delete fMenuBar;
   delete fFileMenu;
   delete fEditMenu;

   delete fOptionsGen;
   delete fOptions1D;
   delete fOptions2D;
   delete fOptionsMenu;
   delete fHelpMenu;
   delete fMenuBarLayout;
   delete fMenuBarItemLayout;
   delete fMenuBarHelpLayout;
   delete fBarLayout;

   fWidgets->Delete();
   delete fWidgets;
   if (fTreeList) {
      delete fTreeList;
   }
   delete fTimer;
   delete fSession;
}
//______________________________________________________________________________
void TTreeViewer::ActivateButtons(Bool_t first, Bool_t previous,
                                  Bool_t next, Bool_t last)
{
   // Enable/disable session buttons.

   if (first)    fBGFirst->SetState(kButtonUp);
   else          fBGFirst->SetState(kButtonDisabled);
   if (previous) fBGPrevious->SetState(kButtonUp);
   else          fBGPrevious->SetState(kButtonDisabled);
   if (next)     fBGNext->SetState(kButtonUp);
   else          fBGNext->SetState(kButtonDisabled);
   if (last)     fBGLast->SetState(kButtonUp);
   else          fBGLast->SetState(kButtonDisabled);
}

//______________________________________________________________________________
const char* TTreeViewer::Cut()
{
   return fLVContainer->Cut();
}

//______________________________________________________________________________
const char* TTreeViewer::ScanList()
{
   return fLVContainer->ScanList();
}

//______________________________________________________________________________
void TTreeViewer::SetSession(TTVSession *session)
{
   if (session) {
      delete fSession;
      fSession = session;
   }
}

//______________________________________________________________________________
const char* TTreeViewer::EmptyBrackets(const char* name)
{
   // Empty the bracket content of a string.

   TString stripped(name);
   if (!stripped.Contains("[")) return name;
   TString retstr(name);
   TObjString *objstr;
   Int_t index = 0;
   while (stripped.Index("[", index) != kNPOS) {
      Int_t start = stripped.Index("[", index);
      Int_t end   = stripped.Index("]", index);
      if (end == kNPOS) {
         objstr = new TObjString(retstr.Data());
         fWidgets->Add(objstr);
         return (objstr->GetString()).Data();
      }
      index = start+2;
      retstr = stripped.Remove(start+1, end-start-1);
      stripped = retstr;
   }
   objstr = new TObjString(retstr.Data());
   fWidgets->Add(objstr);
   return (objstr->GetString()).Data();
}

//______________________________________________________________________________
void TTreeViewer::EmptyAll()
{
   // Clear the content of all items in the list view.

   fLVContainer->EmptyAll();
}

//______________________________________________________________________________
void TTreeViewer::Empty()
{
   // Empty the content of the selected expression.

   void *p = 0;
   TTVLVEntry *item = 0;
   if ((item = (TTVLVEntry *) fLVContainer->GetNextSelected(&p)) == 0) {
      Warning("Empty", "No item selected.");
      return;
   }
   ULong_t *itemType = (ULong_t *) item->GetUserData();
   if (!(*itemType & kLTExpressionType)) {
      Warning("Empty", "Not expression type.");
      return;
   }
   if (*itemType & kLTPackType) {
      item->SetSmallPic(fClient->GetPicture("pack-empty_t.xpm"));
      item->SetTrueName("");
      return;
   }
   item->Empty();
}

//______________________________________________________________________________
TTVLVEntry * TTreeViewer::ExpressionItem(Int_t index)
{
   // Get the item from a specific position.

   return fLVContainer->ExpressionItem(index);
}

//______________________________________________________________________________
TList* TTreeViewer::ExpressionList()
{
   // Get the list of expression items.

   return fLVContainer->ExpressionList();
}

//______________________________________________________________________________
Int_t TTreeViewer::Dimension()
{
   // Compute dimension of the histogram.

   fDimension = 0;
   if (strlen(Ex())) fDimension++;
   if (strlen(Ey())) fDimension++;
   if (strlen(Ez())) fDimension++;
   return fDimension;
}

//______________________________________________________________________________
void TTreeViewer::ExecuteDraw()
{
   // Called when the DRAW button is executed.

   char varexp[2000];
   varexp[0] = 0;
   char command[2000];
   command[0] = 0;
   Int_t dimension = 0;
   TString alias[3];
   TTVLVEntry *item;
   Int_t i;
   // fill in expressions
   if (fVarDraw) {
      void *p = 0;
      dimension = 1;
      if (!(item = (TTVLVEntry *) fLVContainer->GetNextSelected(&p))) return;
      alias[0] = item->GetAlias();
      if (alias[0].BeginsWith("~")) alias[0].Remove(0, 1);
      sprintf(varexp, item->ConvertAliases());
   } else {
      if (strlen(Ez())) {
         dimension++;
         sprintf(varexp, Ez());
         item = ExpressionItem(2);
         alias[2] = item->GetAlias();
         if (alias[2].BeginsWith("~")) alias[2].Remove(0, 1);
      }
      if (strlen(Ez()) && (strlen(Ex()) || strlen(Ey()))) strcat(varexp, ":");
      if (strlen(Ey())) {
         dimension++;
         strcat(varexp, Ey());
         item = ExpressionItem(1);
         alias[1] = item->GetAlias();
         if (alias[1].BeginsWith("~")) alias[1].Remove(0, 1);
      }
      if (strlen(Ey()) && strlen(Ex())) strcat(varexp, ":");
      if (strlen(Ex())) {
         dimension++;
         strcat(varexp, Ex());
         item = ExpressionItem(0);
         alias[0] = item->GetAlias();
         if (alias[0].BeginsWith("~")) alias[0].Remove(0, 1);
      }
   }
   if (!dimension) {
      Warning("ExecuteDraw", "Nothing to draw on X,Y,Z.");
      return;
   }
   // find ListIn
   fTree->SetEventList(0);
   TEventList *elist = 0;
   if (strlen(fBarListIn->GetText())) {
      elist = (TEventList *) gROOT->FindObject(fBarListIn->GetText());
      if (elist) fTree->SetEventList(elist);
   }
   // find ListOut
   if (strlen(fBarListOut->GetText())) sprintf(varexp, ">>%s", fBarListOut->GetText());
   // find histogram name
   if (strcmp("htemp", fBarHist->GetText())) {
      strcat(varexp, ">>");
      strcat(varexp, fBarHist->GetText());
   }
   // find canvas/pad where to draw
   TPad *pad = (TPad*)gROOT->GetSelectedPad();
   if (pad) pad->cd();
   // find graphics option
   const char* gopt = fBarOption->GetText();
   // just in case a previous interrupt was posted
   gROOT->SetInterrupt(kFALSE);
   // check if cut is enabled
   const char *cut = "";
   if (fEnableCut) cut = Cut();

   // get entries to be processed
   Long64_t nentries = (Long64_t)(fSlider->GetMaxPosition() -
                            fSlider->GetMinPosition() + 1);
   Long64_t firstentry =(Long64_t) fSlider->GetMinPosition();
//printf("firstentry=%lld, nentries=%lld\n",firstentry,nentries);
   // check if Scan is checked and if there is something in the box
   if (fScanMode) {
//      fBarScan->SetState(kButtonUp);
      fScanMode = kFALSE;
      if (strlen(ScanList())) sprintf(varexp, ScanList());
      sprintf(command, "tv__tree->Scan(\"%s\",\"%s\",\"%s\", %lld, %lld);",
              varexp, cut, gopt, nentries, firstentry);
      if (fBarScan->GetState() == kButtonDown) {
         ((TTreePlayer *)fTree->GetPlayer())->SetScanRedirect(kTRUE);
      } else {
         ((TTreePlayer *)fTree->GetPlayer())->SetScanRedirect(kFALSE);
      }
      ExecuteCommand(command, kTRUE);
      return;
   }
   // check if only histogram has to be updated
   if (fBarH->GetState() == kButtonDown) {
      // reset 'Hist' mode
      fBarH->SetState(kButtonUp);
      TH1 *hist = fTree->GetHistogram();
      if (hist && gPad) {
         //hist = (TH1*)gPad->GetListOfPrimitives()->FindObject(fBarHist->GetText());
         if (hist) {
            // check if graphic option was modified
            TString last(fLastOption);
            TString current(gopt);
            current.ToUpper();
            last.ToUpper();
            if (current == last) {
               gPad->Update();
               return;
            }
            if (dimension == 3 && strlen(gopt)) {
               cout << "Graphics option " << gopt << " not valid for 3D histograms" << endl;
               return;
            }
            cout << " Graphics option for current histogram changed to " << gopt << endl;
            hist->Draw(gopt);
            fLastOption = fBarOption->GetText();
            gPad->Update();
            return;
         }
      }
   }
   // send draw command
   fLastOption = fBarOption->GetText();
   if (!strlen(gopt) && dimension!=3)
   //{
   //   gopt = "hist";
   //   fLastOption = "hist";
   //}
   if (dimension == 3 && strlen(gopt)) {
      cout << "Graphics option " << gopt << " not valid for 3D histograms" << endl;
      gopt = "";
      fLastOption = "";
   }
   sprintf(command, "tv__tree->Draw(\"%s\",\"%s\",\"%s\", %lld, %lld);",
           varexp, cut, gopt, nentries, firstentry);
   if (fCounting) return;
   fCounting = kTRUE;
   fTree->SetTimerInterval(200);
   fTimer->TurnOn();
   ExecuteCommand(command);
   HandleTimer(fTimer);
   fTimer->TurnOff();
   fTree->SetTimerInterval(0);
   fCounting = kFALSE;
   fProgressBar->SetPosition(0);
   fProgressBar->ShowPosition();
   TH1 *hist = fTree->GetHistogram();
   if (hist) {
   // put expressions aliases on axes
      Int_t current = 0;
      for (i=0; i<3; i++) {
         if (alias[i].Length()) {
            if (i != current) {
               alias[current] = alias[i];
               alias[i] = "";
            }
            current++;
         }
      }
      //hist = (TH1*)gPad->GetListOfPrimitives()->FindObject(fBarHist->GetText());
      TAxis *axis[3];
      axis[0] = hist->GetXaxis();
      axis[1] = hist->GetYaxis();
      axis[2] = hist->GetZaxis();
      for (Int_t ind=0; ind<3; ind++) axis[ind]->SetTitle(alias[ind].Data());
   }
   if (gPad) gPad->Update();
}

//______________________________________________________________________________
const char* TTreeViewer::Ex()
{
   // Get the expression to be drawn on X axis.

   return fLVContainer->Ex();
}

//______________________________________________________________________________
const char* TTreeViewer::Ey()
{
   // Get the expression to be drawn on Y axis.

   return fLVContainer->Ey();
}

//______________________________________________________________________________
const char* TTreeViewer::Ez()
{
   // Get the expression to be drawn on Z axis.

   return fLVContainer->Ez();
}

//______________________________________________________________________________
void TTreeViewer::EditExpression()
{
   // Start the expression editor.

   void *p = 0;
   // get the selected item
   TTVLVEntry *item = 0;
   if ((item = (TTVLVEntry *) fLVContainer->GetNextSelected(&p)) == 0) {
      Warning("EditExpression", "No item selected.");
      return;
   }
   // check if it is an expression
   ULong_t *itemType = (ULong_t *) item->GetUserData();
   if (!(*itemType & kLTExpressionType)) {
      Warning("EditExpression", "Not expression type.");
      return;
   }
   // check if the editor is already active
   fDialogBox = TGSelectBox::GetInstance();
   if (!fDialogBox) {
      fDialogBox = new TGSelectBox(fClient->GetRoot(), this, fV1->GetWidth() - 10);
   }
   // copy current item data into editor boxes
   fDialogBox->SetEntry(item);
   fDialogBox->SetWindowName("Expression editor");
   // check if you are editing the cut expression
   if (*itemType & kLTCutType || item->IsCut()) {
      fDialogBox->SetLabel("Selection");
   } else {
      fDialogBox->SetLabel("Expression");
   }
}

//______________________________________________________________________________
Int_t TTreeViewer::MakeSelector(const char* selector)
{
   // Get use of TTree::MakeSelector() via the context menu.

   if (!fTree) return 0;
   return fTree->MakeSelector(selector);
}

//______________________________________________________________________________
Long64_t TTreeViewer::Process(const char* filename, Option_t *option, Long64_t nentries, Long64_t firstentry)
{
   // Get use of TTree::Process() via the context menu.

   if (!fTree) return 0;
   return fTree->Process(filename, option, nentries, firstentry);
}

//______________________________________________________________________________
const char *TTreeViewer::GetGrOpt()
{
   return fBarOption->GetText();
}

//______________________________________________________________________________
void TTreeViewer::SetGrOpt(const char *option)
{
   fBarOption->SetText(option);
}

//______________________________________________________________________________
Bool_t TTreeViewer::IsScanRedirected()
{
   return (fBarScan->GetState()==kButtonDown);
}

//______________________________________________________________________________
void TTreeViewer::RemoveItem()
{
   // Remove the selected item from the list.

   void *p = 0;
   TTVLVEntry *item = 0;
   // get the selected item
   if ((item = (TTVLVEntry *) fLVContainer->GetNextSelected(&p)) == 0) {
      Warning("RemoveItem", "No item selected.");
      return;
   }
   // check if it is removable
   ULong_t *itemType = (ULong_t *) item->GetUserData();
   if (!(*itemType & kLTDragType)) {
      Warning("RemoveItem", "Not removable type.");
      return;
   }
   fLVContainer->RemoveItem(item);
   fListView->Layout();
}

//______________________________________________________________________________
void TTreeViewer::RemoveLastRecord()
{
   // Remove the current record.

   fSession->RemoveLastRecord();
}

//______________________________________________________________________________
Bool_t TTreeViewer::HandleTimer(TTimer *timer)
{
   // This function is called by the fTimer object.

   if (fCounting) {
      Float_t first = fSlider->GetMinPosition();
      Float_t last  = fSlider->GetMaxPosition();
      Float_t current = (Float_t)fTree->GetReadEntry();
      Float_t percent = (current-first+1)/(last-first+1);
      fProgressBar->SetPosition(100.*percent);
      fProgressBar->ShowPosition();
   }
   timer->Reset();
   return kFALSE;
}

//______________________________________________________________________________
Bool_t TTreeViewer::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
   // Handle menu and other commands generated.

   TRootHelpDialog *hd;

   switch (GET_MSG(msg)) {
      case kC_VSLIDER :
         // handle slider messages
         PrintEntries();
      break;
      case kC_TEXTENTRY:
         switch (GET_SUBMSG(msg)) {
         // handle enter posted by the Command text entry
            case kTE_ENTER:
               if ((Int_t)parm1 == kBarCommand) {
                  ExecuteCommand(fBarCommand->GetText());
                  fBarCommand->Clear();
               }
               if ((Int_t)parm1 == kBarOption) {
                  fVarDraw = kFALSE;
                  fBarH->SetState(kButtonDown);
                  ExecuteDraw();
                  fBarH->SetState(kButtonUp);
               }
               break;
            default:
               break;
         }
         break;
      case kC_LISTTREE:
         switch (GET_SUBMSG(msg)) {
         // handle mouse messages in the list-tree (left panel)
            case kCT_ITEMCLICK :
               if ((parm1==kButton1) || (parm1==kButton3)) {
                  TGListTreeItem *ltItem = 0;
                  // get item that sent this
                  if ((ltItem = fLt->GetSelected()) != 0) {
                  // get item type
                     ULong_t *itemType = (ULong_t *)ltItem->GetUserData();
                     if (*itemType & kLTTreeType) {
                     // already mapped tree item clicked
                        Int_t index = (Int_t)(*itemType >> 8);
                        SwitchTree(index);
                        if (fTree != fMappedTree) {
                           // switch also the global "tree" variable
                           fLVContainer->RemoveNonStatic();
                           // map it on the right panel
                           MapTree(fTree);
                           fListView->Layout();
                        }
                        // activate context menu for this tree
                        if (parm1 == kButton3) {
                           Int_t x = (Int_t)(parm2 &0xffff);
                           Int_t y = (Int_t)((parm2 >> 16) & 0xffff);
                           fContextMenu->Popup(x, y, fTree);
                        }
                     }

                     if (*itemType & kLTBranchType) {
                     // branch item clicked
                        SetParentTree(ltItem);
                        if (!fTree) break; // really needed ?
                        TBranch *branch = fTree->GetBranch(ltItem->GetText());
                        if (!branch) break;
                        // check if it is mapped on the right panel
                        if (branch != fMappedBranch) {
                           fLVContainer->RemoveNonStatic();
                           MapBranch(branch);
                           fStopMapping = kFALSE;
                           fListView->Layout();
                        }
                        // activate context menu for this branch (no *MENU* methods ):)
                        if (parm1 == kButton3) {
                           Int_t x = (Int_t)(parm2 &0xffff);
                           Int_t y = (Int_t)((parm2 >> 16) & 0xffff);
                           fContextMenu->Popup(x, y, branch);
                        }
                     }

                     if (*itemType & kLTLeafType) {
                     // leaf item clicked
                        SetParentTree(ltItem);
                        if (!fTree) break;
                        // find parent branch
                        TBranch *branch = fTree->GetBranch(ltItem->GetParent()->GetText());
                        if (!branch) {
                           if (fTree != fMappedTree) {
                              fLVContainer->RemoveNonStatic();
                              MapTree(fTree);
                              fListView->Layout();
                           }
                        } else {
                           // check if it is already mapped
                           if (branch!=fMappedBranch) {
                              fLVContainer->RemoveNonStatic();
                              MapBranch(branch);
                              fStopMapping = kFALSE;
                              fListView->Layout();
                           }
                        }
                        // select corresponding leaf on the right panel
                        fLVContainer->SelectItem(ltItem->GetText());
                        if (parm1 == kButton3) {
                        // activate context menu for this leaf
                           ProcessMessage(MK_MSG(kC_CONTAINER, kCT_ITEMCLICK), kButton3, parm2);
                        }
                     }
                  }
               }
               break;
            case kCT_ITEMDBLCLICK :
               fClient->NeedRedraw(fLt);
               if (parm1 == kButton1) {
               // execute double-click action for corresponding item in the right panel
                  ProcessMessage(MK_MSG(kC_CONTAINER, kCT_ITEMDBLCLICK), kButton1, parm2);
               }
               break;
            default:
               break;
         }
         break;
      case kC_COMMAND:
         switch (GET_SUBMSG(msg)){
            case kCM_COMBOBOX:
               fSession->Show(fSession->GetRecord((Int_t)parm2));
            break;
            case kCM_BUTTON:
               switch (parm1) {
               // handle button messages
                  case kRESET:
                     EmptyAll();
                     break;
                  case kDRAW:
                     fVarDraw = kFALSE;
                     ExecuteDraw();
                     break;
                  case kSTOP:
                     if (fCounting)
                        gROOT->SetInterrupt(kTRUE);
                     break;
                  case kCLOSE:
                     SendCloseMessage();
                     break;
                  case kBGFirst:
                     fSession->Show(fSession->First());
                     break;
                  case kBGPrevious:
                     fSession->Show(fSession->Previous());
                     break;
                  case kBGRecord:
                     fSession->AddRecord();
                     break;
                  case kBGNext:
                     fSession->Show(fSession->Next());
                     break;
                  case kBGLast:
                     fSession->Show(fSession->Last());
                     break;
                  default:
                     break;
               }
               break;
            case kCM_MENU:
            // handle menu messages
               // check if sent by Options menu
               if ((parm1>=kOptionsReset) && (parm1<kHelpAbout)) {
                  Dimension();
                  if ((fDimension==0) && (parm1>=kOptions1D)) {
                     Warning("ProcessMessage", "Edit expressions first.");
                     break;
                  }
                  if ((fDimension==1) && (parm1>=kOptions2D)) {
                     Warning("ProcessMessage", "You have only one expression active.");
                     break;
                  }
                  if ((fDimension==2) && (parm1>=kOptions1D) &&(parm1<kOptions2D)) {
                     Warning("ProcessMessage", "1D drawing options not apply to 2D histograms.");
                     break;
                  }
                  // make composed option
                  MapOptions(parm1);
                  break;
               }
               switch (parm1) {
                  case kFileCanvas:
                     gROOT->GetMakeDefCanvas()();
                     break;
                  case kFileBrowse:
                     if (1) {
                        static TString dir(".");
                        TGFileInfo info;
                        info.fFileTypes = gOpenTypes;
                        info.fIniDir    = StrDup(dir);
                        new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &info);
                        if (!info.fFilename) return kTRUE;
                        dir = info.fIniDir;
                        char command[1024];
                        command[0] = 0;
                        sprintf(command, "tv__tree_file = new TFile(\"%s\");", info.fFilename);
                        ExecuteCommand(command);
                        ExecuteCommand("tv__tree_file->ls();");
                        cout << "Use SetTreeName() from context menu and supply a tree name" << endl;
                        cout << "The context menu is activated by right-clicking the panel from right" << endl;
                     }
                     break;
                  case kFileLoadLibrary:
                     fBarCommand->SetText("gSystem->Load(\"\");");
                     if (1) {
                        Event_t event;
                        event.fType = kButtonPress;
                        event.fCode = kButton1;

                        fBarCommand->HandleButton(&event);
                     }
                     fBarCommand->SetCursorPosition(15);
                     break;
                  case kFileOpenSession:
                     if (1) {
                        static TString dir(".");
                        TGFileInfo info;
                        info.fFileTypes = gMacroTypes;
                        info.fIniDir    = StrDup(dir);
                        new TGFileDialog(fClient->GetRoot(), this, kFDOpen, &info);
                        if (!info.fFilename) return kTRUE;
                        dir = info.fIniDir;
                        gInterpreter->Reset();
                        if (!gInterpreter->IsLoaded(info.fFilename)) gInterpreter->LoadMacro(info.fFilename);
                        char command[1024];
                        command[0] = 0;
                        sprintf(command,"open_session((void*)0x%lx);", (Long_t)this);
                        ExecuteCommand(command);
                     }
                     break;
                  case kFileSaveMacro:
                     SaveSource();
                     break;
                  case kFilePrint:
                     break;
                  case kFileClose:
                     SendCloseMessage();
                     break;
                  case kFileQuit:
                     gApplication->Terminate(0);
                     break;
                  case kEditExpression:
                     EditExpression();
                     break;
                  case kEditCut:
                     EditExpression();
                     break;
                  case kEditMacro:
                     break;
                  case kEditEvent:
                     break;
                  case kRunMacro:
                     break;
                  case kHelpAbout:
                     {
#ifdef R__UNIX
                        TString rootx;
# ifdef ROOTBINDIR
                        rootx = ROOTBINDIR;
# else
                        rootx = gSystem->Getenv("ROOTSYS");
                        if (!rootx.IsNull()) rootx += "/bin";
# endif
                        rootx += "/root -a &";
                        gSystem->Exec(rootx);
#else
#ifdef WIN32
                        new TWin32SplashThread(kTRUE);
#else
                        char str[32];
                        sprintf(str, "About ROOT %s...", gROOT->GetVersion());
                        hd = new TRootHelpDialog(this, str, 600, 400);
                        hd->SetText(gHelpAbout);
                        hd->Popup();
#endif
#endif
                     }
                     break;
                  case kHelpAboutTV:
                     hd = new TRootHelpDialog(this, "About TreeViewer...", 600, 400);
                     hd->SetText(gTVHelpAbout);
                     hd->Resize(hd->GetDefaultSize());
                     hd->Popup();
                     break;
                  case kHelpStart:
                     hd = new TRootHelpDialog(this, "Quick start...", 600, 400);
                     hd->SetText(gTVHelpStart);
                     hd->Popup();
                     break;
                  case kHelpLayout:
                     hd = new TRootHelpDialog(this, "Layout...", 600, 400);
                     hd->SetText(gTVHelpLayout);
                     hd->Popup();
                     break;
                  case kHelpOpenSave:
                     hd = new TRootHelpDialog(this, "Open/Save...", 600, 400);
                     hd->SetText(gTVHelpOpenSave);
                     hd->Popup();
                     break;
                  case kHelpDragging:
                     hd = new TRootHelpDialog(this, "Dragging items...", 600, 400);
                     hd->SetText(gTVHelpDraggingItems);
                     hd->Popup();
                     break;
                  case kHelpEditing:
                     hd = new TRootHelpDialog(this, "Editing expressions...", 600, 400);
                     hd->SetText(gTVHelpEditExpressions);
                     hd->Popup();
                     break;
                  case kHelpSession:
                     hd = new TRootHelpDialog(this, "Session...", 600, 400);
                     hd->SetText(gTVHelpSession);
                     hd->Popup();
                     break;
                  case kHelpCommands:
                     hd = new TRootHelpDialog(this, "Executing user commands...", 600, 400);
                     hd->SetText(gTVHelpUserCommands);
                     hd->Popup();
                     break;
                  case kHelpContext:
                     hd = new TRootHelpDialog(this, "Context menus...", 600, 400);
                     hd->SetText(gTVHelpContext);
                     hd->Popup();
                     break;
                  case kHelpDrawing:
                     hd = new TRootHelpDialog(this, "Drawing histograms...", 600, 400);
                     hd->SetText(gTVHelpDrawing);
                     hd->Popup();
                     break;
                  case kHelpMacros:
                     hd = new TRootHelpDialog(this, "Using macros...", 600, 400);
                     hd->SetText(gTVHelpMacros);
                     hd->Popup();
                     break;
                  default:
                     break;
               }
               break;
            default:
               break;
         }
         break;
      case kC_CONTAINER:
         switch (GET_SUBMSG(msg)) {
         // handle messages sent from the listview (right panel)
            case kCT_SELCHANGED:
               break;
            case kCT_ITEMCLICK:
            // handle mouse messages
               switch (parm1) {
                  case kButton1:
                     if (fLVContainer->NumSelected()) {
                     // get item that sent this
                        void *p = 0;
                        TTVLVEntry *item;
                        if ((item = (TTVLVEntry *) fLVContainer->GetNextSelected(&p)) != 0) {
                           const char* vname = item->GetTrueName();
                           TString trueName(vname);
                           if (trueName.Contains("[]")) {
                              TIter next(fTree->GetListOfLeaves());
                              TLeaf *leaf;
                              while((leaf=(TLeaf*)next())) {
                                 if (!strcmp(vname, EmptyBrackets(leaf->GetName())))
                                    vname = leaf->GetName();
                              }
                           }
                           char* msg = new char[2000];
                           // get item type
                           ULong_t *itemType = (ULong_t *) item->GetUserData();
                           if (*itemType & kLTTreeType) {
                           // X, Y or Z clicked
                              char symbol = (char)((*itemType) >> 8);
                              sprintf(msg, "%c expression : %s", symbol, vname);
                           } else {
                              if (*itemType & kLTCutType) {
                              // scissors clicked
                                 sprintf(msg, "Cut : %s", vname);
                              } else {
                                 if (*itemType & kLTPackType) {
                                    sprintf(msg, "Box : %s", vname);
                                 } else {
                                    if (*itemType & kLTExpressionType) {
                                       // expression clicked
                                       sprintf(msg, "Expression : %s", vname);
                                    } else {
                                       if (*itemType & kLTBranchType) {
                                          sprintf(msg, "Branch : %s", vname);
                                       } else {
                                          sprintf(msg, "Leaf : %s", vname);
                                       }
                                    }
                                 }
                              }
                           }
                           // write who is responsable for this
                           TString message = msg;
                           message = message(0,150);
                           Message(msg);
                           delete[] msg;
                           // check if this should be pasted into the expression editor
                           if ((*itemType & kLTBranchType) || (*itemType & kLTCutType)) break;
                           fDialogBox = TGSelectBox::GetInstance();
                           if (!fDialogBox || !strlen(vname)) break;
                           if (item == fDialogBox->EditedEntry()) break;
                           // paste it
//                           char first = (char) vname[0];
                           TString insert(item->GetAlias());
//                           if (first != '(') insert += "(";
//                           insert += item->GetAlias();
//                           if (first != '(') insert += ")";

                           fDialogBox->GrabPointer();
                           fDialogBox->InsertText(insert.Data());
                           // put the cursor at the right position
                        }
                     }
                     break;
                  case kButton2:
                     break;
                  case kButton3:
                  // activate general context menu
                     if (fLVContainer->NumSelected()) {
                        void *p = 0;
                        Int_t x = (Int_t)(parm2 &0xffff);
                        Int_t y = (Int_t)((parm2 >> 16) & 0xffff);
                        TTVLVEntry *item = 0;
                        if ((item = (TTVLVEntry *) fLVContainer->GetNextSelected(&p)) != 0) {
                           fContextMenu->Popup(x, y, item->GetContext());
                        }
                     } else {        // empty click
                        Int_t x = (Int_t)(parm2 &0xffff);
                        Int_t y = (Int_t)((parm2 >> 16) & 0xffff);
                        fContextMenu->Popup(x, y, this);
                     }
                     break;
                  default:
                     break;
               }
               break;
            case kCT_ITEMDBLCLICK:
               switch (parm1) {
                  case kButton1:
                     if (fLVContainer->NumSelected()) {
                     // get item that sent this
                        void *p = 0;
                        TTVLVEntry *item;
                        if ((item = (TTVLVEntry *) fLVContainer->GetNextSelected(&p)) != 0) {
                        // get item type
                           ULong_t *itemType = (ULong_t *) item->GetUserData();
                           if (!(*itemType & kLTCutType) && !(*itemType & kLTBranchType)
                               && !(*itemType & kLTPackType)) {
                              if (strlen(item->GetTrueName())) {
                                 fVarDraw = kTRUE;
                                 // draw on double-click
                                 ExecuteDraw();
                                 break;
                              } else {
                              // open expression in editor
                                 EditExpression();
                              }
                           }
                           if (*itemType & kLTCutType) {
                              fEnableCut = !fEnableCut;
                              if (fEnableCut) {
                                 item->SetSmallPic(gClient->GetPicture("cut_t.xpm"));
                              } else {
                                 item->SetSmallPic(gClient->GetPicture("cut-disable_t.xpm"));
                              }
                           }
                           if (*itemType & kLTPackType) {
                              fScanMode = kTRUE;
                              ExecuteDraw();
                           }
                        }
                     }
                     break;
                  case kButton2:
                     break;
                  case kButton3:
                     break;
                  default:
                     break;
               }
               break;
            case 4:
//               cout << "Dragging Item" << endl;
            default:
               break;
         }
         break;
      default:
         break;
   }
   return kTRUE;
}

//______________________________________________________________________________
void TTreeViewer::CloseWindow()
{
   // Close the viewer.

   DeleteWindow();
}

//______________________________________________________________________________
void TTreeViewer::ExecuteCommand(const char* command, Bool_t fast)
{
   // Execute all user commands.

   // Execute the command, write it to history file and echo it to output
   if (fBarRec->GetState() == kButtonDown) {
   // show the command on the command line
      //printf("%s\n", command);
      char comm[2000];
      comm[0] = 0;
      if (strlen(command) > 1999) {
         Warning("ExecuteCommand", "Command too long: aborting.");
         return;
      }
      sprintf(comm, command);
      // print the command to history file
      Gl_histadd(comm);
   }
   // execute it
   if (fast) {
      gROOT->ProcessLineFast(command);
   } else {
      gROOT->ProcessLine(command);
   }
   // make sure that 'draw on double-click' flag is reset
   fVarDraw = kFALSE;
}
//______________________________________________________________________________
void TTreeViewer::MapOptions(Long_t parm1)
{
   // Scan the selected options from option menu.

   Int_t ind;
   if (parm1 == kOptionsReset) {
      for (ind=kOptionsGeneral; ind<kOptionsGeneral+16; ind++)
         fOptionsGen->UnCheckEntry(ind);
      for (ind=kOptions1D; ind<kOptions1D+12; ind++)
         fOptions1D->UnCheckEntry(ind);
      for (ind=kOptions2D; ind<kOptions2D+14; ind++)
         fOptions2D->UnCheckEntry(ind);
   }
   if ((parm1 < kOptions1D) && (parm1 != kOptionsReset)) {
      if (fOptionsGen->IsEntryChecked((Int_t)parm1)) {
         fOptionsGen->UnCheckEntry((Int_t)parm1);
      } else {
         fOptionsGen->CheckEntry((Int_t)parm1);
         if ((Int_t)parm1 != kOptionsGeneral) fOptionsGen->UnCheckEntry((Int_t)kOptionsGeneral);
      }
      if (fOptionsGen->IsEntryChecked((Int_t)kOptionsGeneral)) {
      // uncheck all in this menu
         for (ind=kOptionsGeneral+1; ind<kOptionsGeneral+16; ind++) {
            fOptionsGen->UnCheckEntry(ind);
         }
      }
   }

   if ((parm1 < kOptions2D) && (parm1 >= kOptions1D)) {
      if (fOptions1D->IsEntryChecked((Int_t)parm1)) {
         fOptions1D->UnCheckEntry((Int_t)parm1);
      } else {
         fOptions1D->CheckEntry((Int_t)parm1);
         if ((Int_t)parm1 != kOptions1D) fOptions1D->UnCheckEntry((Int_t)kOptions1D);
      }
      if (fOptions1D->IsEntryChecked((Int_t)kOptions1D)) {
      // uncheck all in this menu
         for (ind=kOptions1D+1; ind<kOptions1D+12; ind++) {
            fOptions1D->UnCheckEntry(ind);
         }
      }
   }

   if (parm1 >= kOptions2D) {
      if (fOptions2D->IsEntryChecked((Int_t)parm1)) {
         fOptions2D->UnCheckEntry((Int_t)parm1);
      } else {
         fOptions2D->CheckEntry((Int_t)parm1);
         if ((Int_t)parm1 != kOptions2D) fOptions2D->UnCheckEntry((Int_t)kOptions2D);
      }
      if (fOptions2D->IsEntryChecked((Int_t)kOptions2D)) {
      // uncheck all in this menu
         for (ind=kOptions2D+1; ind<kOptions2D+14; ind++) {
            fOptions2D->UnCheckEntry(ind);
         }
      }
   }
   // concatenate options
   fBarOption->SetText("");
   for (ind=kOptionsGeneral; ind<kOptionsGeneral+16; ind++) {
      if (fOptionsGen->IsEntryChecked(ind))
         fBarOption->AppendText(optgen[ind-kOptionsGeneral]);
   }
   if (Dimension() == 1) {
      for (ind=kOptions1D; ind<kOptions1D+12; ind++) {
         if (fOptions1D->IsEntryChecked(ind))
            fBarOption->AppendText(opt1D[ind-kOptions1D]);
      }
   }
   if (Dimension() == 2) {
      for (ind=kOptions2D; ind<kOptions2D+14; ind++) {
         if (fOptions2D->IsEntryChecked(ind))
            fBarOption->AppendText(opt2D[ind-kOptions2D]);
      }
   }
}

//______________________________________________________________________________
void TTreeViewer::MapTree(TTree *tree, TGListTreeItem *parent, Bool_t listIt)
{
   // Map current tree and expand its content in the lists.

   if (!tree) return;
   TObjArray *Branches = tree->GetListOfBranches();
   TBranch   *branch;
   // loop on branches
   for (Int_t id=0; id<Branches->GetEntries(); id++) {
      branch = (TBranch *)Branches->At(id);
      if (branch->TestBit(kDoNotProcess))  continue;
      TString name = branch->GetName();
      if (name.Contains("fBits") || name.Contains("fUniqueID")) continue;
      // now map sub-branches
      MapBranch(branch, parent, listIt);
      fStopMapping = kFALSE;
   }
   // tell who was last mapped
   if (listIt) {
      fMappedTree    = tree;
      fMappedBranch  = 0;
   }
}

//______________________________________________________________________________
void TTreeViewer::MapBranch(TBranch *branch, TGListTreeItem *parent, Bool_t listIt)
{
   // Map current branch and expand its content in the list view.

   if (!branch) return;
   TString   name = branch->GetName();
   Int_t     ind;
   TGListTreeItem *branchItem = 0;
   ULong_t *itemType;
   // map this branch
   if (name.Contains("fBits") || name.Contains("fUniqueID")) return;
   if (parent) {
   // make list tree items for each branch according to the type
      const TGPicture *pic, *spic;
      if ((branch->GetListOfBranches()->GetEntries()) ||
          (branch->GetNleaves())) {
         if (branch->GetListOfBranches()->GetEntries()) {
            itemType = new ULong_t(kLTBranchType);
            if (branch->InheritsFrom("TBranchObject")) {
               pic = gClient->GetPicture("branch-ob_t.xpm");
               spic = gClient->GetPicture("branch-ob_t.xpm");
            } else {
               if (branch->InheritsFrom("TBranchClones")) {
                  pic = gClient->GetPicture("branch-cl_t.xpm");
                  spic = gClient->GetPicture("branch-cl_t.xpm");
               } else {
                  pic = gClient->GetPicture("branch_t.xpm");
                  spic = gClient->GetPicture("branch_t.xpm");
               }
            }
            branchItem = fLt->AddItem(parent, EmptyBrackets(branch->GetName()), itemType, pic, spic);
         } else {
            if (branch->GetNleaves() > 1) {
               itemType = new ULong_t(kLTBranchType);
               pic = gClient->GetPicture("branch_t.xpm");
               spic = gClient->GetPicture("branch_t.xpm");
               branchItem = fLt->AddItem(parent, EmptyBrackets(branch->GetName()), itemType,pic, spic);
               TObjArray *Leaves = branch->GetListOfLeaves();
               TLeaf *leaf = 0;
               TString leafName;
               for (Int_t lf=0; lf<Leaves->GetEntries(); lf++) {
                  leaf = (TLeaf *)Leaves->At(lf);
                  leafName = name;
                  leafName.Append(".").Append(EmptyBrackets(leaf->GetName()));
                  itemType = new ULong_t(kLTLeafType);
                  pic = gClient->GetPicture("leaf_t.xpm");
                  spic = gClient->GetPicture("leaf_t.xpm");
                  fLt->AddItem(branchItem, leafName.Data(), itemType, pic, spic);
               }
           } else {
               itemType = new ULong_t(kLTLeafType);
               pic = gClient->GetPicture("leaf_t.xpm");
               spic = gClient->GetPicture("leaf_t.xpm");
               branchItem = fLt->AddItem(parent, EmptyBrackets(branch->GetName()), itemType, pic, spic);
            }
         }
      }
   }
   // list branch in list view if necessary
   if (listIt) {
      TGString *textEntry;
      const TGPicture *pic, *spic;
      TTVLVEntry *entry;
      // make list view items in the right frame
      if (!fStopMapping) {
         fMappedBranch = branch;
         fMappedTree = 0;
         fStopMapping = kTRUE;
      }
      textEntry = new TGString(EmptyBrackets(name.Data()));
      if ((branch->GetListOfBranches()->GetEntries()) ||
          (branch->GetNleaves())) {
         if (branch->GetListOfBranches()->GetEntries()) {
            if (branch->InheritsFrom("TBranchObject")) {
               pic = gClient->GetPicture("branch-ob_t.xpm");
               spic = gClient->GetPicture("branch-ob_t.xpm");
            } else {
               if (branch->InheritsFrom("TBranchClones")) {
                  pic = gClient->GetPicture("branch-cl_t.xpm");
                  spic = gClient->GetPicture("branch-cl_t.xpm");
               } else {
                  pic = gClient->GetPicture("branch_t.xpm");
                  spic = gClient->GetPicture("branch_t.xpm");
               }
            }
            entry = new TTVLVEntry(fLVContainer,pic,spic,textEntry,0,kLVSmallIcons);
            entry->SetUserData(new UInt_t(kLTBranchType));
            entry->SetToolTipText("Branch with sub-branches. Can not be dragged");
            fLVContainer->AddThisItem(entry);
            entry->MapWindow();
            entry->SetAlias(textEntry->GetString());
         } else {
            if (branch->GetNleaves() > 1) {
               itemType = new ULong_t(kLTBranchType);
               textEntry = new TGString(EmptyBrackets(name.Data()));
               pic = gClient->GetPicture("branch_t.xpm");
               spic = gClient->GetPicture("branch_t.xpm");
               entry = new TTVLVEntry(fLVContainer, pic, spic, textEntry,0,kLVSmallIcons);
               entry->SetUserData(new UInt_t(kLTBranchType));
               entry->SetToolTipText("Branch with more than one leaf. Can not be dragged");
               fLVContainer->AddThisItem(entry);
               entry->MapWindow();
               entry->SetAlias(textEntry->GetString());

               TObjArray *Leaves = branch->GetListOfLeaves();
               TLeaf *leaf = 0;
               TString leafName;
               for (Int_t lf=0; lf<Leaves->GetEntries(); lf++) {
                  leaf = (TLeaf *)Leaves->At(lf);
                  leafName = name;
                  leafName.Append(".").Append(EmptyBrackets(leaf->GetName()));
                  textEntry = new TGString(leafName.Data());
                  pic = gClient->GetPicture("leaf_t.xpm");
                  spic = gClient->GetPicture("leaf_t.xpm");
                  entry = new TTVLVEntry(fLVContainer, pic, spic, textEntry,0,kLVSmallIcons);
                  entry->SetUserData(new UInt_t(kLTDragType | kLTLeafType));
                  entry->SetToolTipText("Double-click to draw. Drag to X, Y, Z or scan box.");
                  fLVContainer->AddThisItem(entry);
                  entry->MapWindow();
                  entry->SetAlias(textEntry->GetString());
               }
            } else {
               pic = (gClient->GetMimeTypeList())->GetIcon("TLeaf",kFALSE);
               if (!pic) pic = gClient->GetPicture("leaf_t.xpm");
               spic = gClient->GetMimeTypeList()->GetIcon("TLeaf",kTRUE);
               if (!spic) spic = gClient->GetPicture("leaf_t.xpm");
               entry = new TTVLVEntry(fLVContainer,pic,spic,textEntry,0,kLVSmallIcons);
               entry->SetUserData(new UInt_t(kLTDragType | kLTLeafType));
               entry->SetToolTipText("Double-click to draw. Drag to X, Y, Z or scan box.");
               fLVContainer->AddThisItem(entry);
               entry->MapWindow();
               entry->SetAlias(textEntry->GetString());
            }
         }
      }
   }

   TObjArray *Branches = branch->GetListOfBranches();
   TBranch   *branchDaughter = 0;

   // loop all sub-branches
   for (ind=0; ind<Branches->GetEntries(); ind++) {
      branchDaughter = (TBranch *)Branches->UncheckedAt(ind);
      // map also all sub-branches
      MapBranch(branchDaughter, branchItem, listIt);
   }
}

//______________________________________________________________________________
void TTreeViewer::NewExpression()
{
   fLVContainer->RemoveNonStatic();
   const TGPicture  *pic = gClient->GetPicture("expression_t.xpm");
   const TGPicture *spic = gClient->GetPicture("expression_t.xpm");

   TTVLVEntry *entry = new TTVLVEntry(fLVContainer,pic,spic,
                                            new TGString(),0,kLVSmallIcons);
   entry->SetUserData(new ULong_t(kLTExpressionType | kLTDragType));
   fLVContainer->AddThisItem(entry);
   entry->MapWindow();
   entry->Empty();
   if (fMappedTree) MapTree(fTree);
   if (fMappedBranch) MapBranch(fMappedBranch);
   fListView->Layout();
   fNexpressions++;
}

//______________________________________________________________________________
void TTreeViewer::SetParentTree(TGListTreeItem *item)
{
   // Find parent tree of a clicked item.

   if (!item) return;
   ULong_t *itemType = (ULong_t *)item->GetUserData();
   TGListTreeItem *parent = 0;
   Int_t index;
   if (!(*itemType & kLTTreeType)) {
      parent = item->GetParent();
      SetParentTree(parent);
   } else {
      index = (Int_t)(*itemType >> 8);
      SwitchTree(index);
   }
}

//______________________________________________________________________________
void TTreeViewer::Message(const char* msg)
{
   // Send a message on the status bar.

   fStatusBar->SetText(msg);
}

//______________________________________________________________________________
void TTreeViewer::DoError(int level, const char *location, const char *fmt, va_list va) const
{
   // Put error/warning into TMsgBox and also forward to console.

   TObject::DoError(level, location, fmt, va);

   // in case level will abort we will not come here...

   static const int buf_size = 2048;
   char buf[buf_size], *bp;

   int n = vsnprintf(buf, buf_size, fmt, va);
   // old vsnprintf's return -1 if string is truncated new ones return
   // total number of characters that would have been written
   if (n == -1 || n >= buf_size) {
      TObject::Warning("DoError", "Error message string truncated...");
   }
   if (level >= kSysError && level < kFatal)
      bp = Form("%s (%s)", buf, gSystem->GetError());
   else
      bp = buf;

   const char *title = "";
   if (level == kInfo)
      title = "Info";
   if (level == kWarning)
      title = "Warning";
   if (level == kError)
      title = "Error";
   if (level == kSysError)
      title = "System Error";

   new TGMsgBox(fClient->GetRoot(), this, title, bp, kMBIconExclamation);
}

//______________________________________________________________________________
void TTreeViewer::PrintEntries()
{
   // Print the number of selected entries on status-bar.

   if (!fTree) return;
   char * msg = new char[100];
   sprintf(msg, "First entry : %lld Last entry : %lld",
           (Long64_t)fSlider->GetMinPosition(), (Long64_t)fSlider->GetMaxPosition());
   Message(msg);
   delete[] msg;
}

//______________________________________________________________________________
void TTreeViewer::SaveSource(const char* filename, Option_t *)
{
   // Save current session as a C++ macro file.

   if (!fTree) return;
   char quote = '"';
   ofstream out;
   Int_t lenfile = strlen(filename);
   char * fname;
   if (!lenfile) {
      fname = (char*)fSourceFile;
      lenfile = strlen(fname);
   } else {
      fname = (char*)filename;
      fSourceFile = filename;
   }
   // if filename is given, open this file, otherwise create a file
   // with a name : treeviewer.C
   if (lenfile) {
      out.open(fname, ios::out);
   } else {
      fname = new char[13];
      strcpy(fname, "treeviewer.C");
      out.open(fname, ios::out);
   }
   if (!out.good ()) {
      printf("SaveSource cannot open file : %s\n", fname);
      fSourceFile = "treeviewer.C";
      if (!lenfile) delete [] fname;
      return;
   }
   //   Write macro header and date/time stamp
   TDatime t;
   TString sname(fname);
   sname = sname.ReplaceAll(".C", "");
   out <<"void "<<sname.Data()<<"() {"<<endl;
   out <<"//=========Macro generated by ROOT version"<<gROOT->GetVersion()<<endl;
   out <<"//=========for tree "<<quote<<fTree->GetName()<<quote<<" ("<<t.AsString()<<")"<<endl;
   out <<"//===This macro can be opened from a TreeViewer session after loading"<<endl;
   out <<"//===the corresponding tree, or by running root with the macro name argument"<<endl<<endl;
   out <<"   open_session();"<<endl;
   out <<"}"<<endl<<endl;
   out <<"open_session(void *p = 0) {"<<endl;
   out <<"   gSystem->Load("<<quote<<"libTreeViewer"<<quote<<");"<<endl;
   out <<"   TTreeViewer *treeview = (TTreeViewer *) p;"<<endl;
   out <<"   if (!treeview) treeview = new TTreeViewer();"<<endl;
   out <<"   TTree *tv_tree = (TTree*)gROOT->FindObject("<<quote<<fTree->GetName()<<quote<<");"<<endl;
   out <<"   TFile *tv_file = (TFile*)gROOT->GetListOfFiles()->FindObject("<<quote<<fFilename<<quote<<");"<<endl;
   out <<"   if (!tv_tree) {"<<endl;
   out <<"      if (!tv_file) tv_file = new TFile("<<quote<<fFilename<<quote<<");"<<endl;
   out <<"      if (tv_file)  tv_tree = (TTree*)tv_file->Get("<<quote<<fTree->GetName()<<quote<<");"<<endl;
   out <<"      if(!tv_tree) {"<<endl;
   out <<"         printf(\"Tree %s not found\", fTree->GetName());"<<endl;
   out <<"         return;"<<endl;
   out <<"      }"<<endl;
   out <<"   }"<<endl<<endl;
   out <<"   treeview->SetTreeName("<<quote<<fTree->GetName()<<quote<<");"<<endl;
   out <<"   treeview->SetNexpressions("<<fNexpressions<<");"<<endl;
   // get expressions
   TTVLVEntry *item;
   out <<"//         Set expressions on axis and cut"<<endl;
   out <<"   TTVLVEntry *item;"<<endl;
   for (Int_t i=0; i<4; i++) {
      switch (i) {
         case 0:
            out <<"//   X expression"<<endl;
            break;
         case 1:
            out <<"//   Y expression"<<endl;
            break;
         case 2:
            out <<"//   Z expression"<<endl;
            break;
         case 3:
            out <<"//   Cut expression"<<endl;
            break;
         default:
            break;
      }
      item = ExpressionItem(i);
      out <<"   item = treeview->ExpressionItem("<<i<<");"<<endl;
      out <<"   item->SetExpression("<<quote<<item->GetTrueName()<<quote
          <<", "<<quote<<item->GetAlias()<<quote<<");"<<endl;
   }
   out <<"//         Scan list"<<endl;
   item = ExpressionItem(4);
   out <<"   item = treeview->ExpressionItem(4);"<<endl;
   out <<"   item->SetExpression("<<quote<<item->GetTrueName()<<quote
          <<", "<<quote<<"Scan box"<<quote<<");"<<endl;
   out <<"//         User defined expressions"<<endl;
   TString itemType;
   for (Int_t crt=5; crt<fNexpressions+5; crt++) {
      item = ExpressionItem(crt);
      if (item->IsCut())
         itemType = "kTRUE";
      else
         itemType = "kFALSE";
      out <<"   item = treeview->ExpressionItem("<<crt<<");"<<endl;
      out <<"   item->SetExpression("<<quote<<item->GetTrueName()<<quote
          <<", "<<quote<<item->GetAlias()<<quote<<", "<<itemType.Data()<<");"<<endl;
   }
   fSession->SaveSource(out);
   out <<"}"<<endl;
   out.close();
   printf("C++ Macro file: %s has been generated\n", fname);
   if (!lenfile) delete [] fname;
}

//______________________________________________________________________________
Bool_t TTreeViewer::SwitchTree(Int_t index)
{
   // Makes current the tree at a given index in the list.

   TTree *tree = (TTree *) fTreeList->At(index);
   if (!tree) {
      Warning("SwitchTree", "No tree found.");
      return kFALSE;
   }
   if ((tree == fTree) && (tree == fMappedTree)) return kFALSE;     // nothing to switch
   char *command = new char[50];
   if (tree != fTree) {
      sprintf(command, "tv__tree = (TTree *) tv__tree_list->At(%i);", index);
      ExecuteCommand(command);
   }

   fTree = tree;
   fSlider->SetRange(0,fTree->GetEntries()-1);
   fSlider->SetPosition(0,fTree->GetEntries()-1);
   sprintf(command, "Current Tree : %s", fTree->GetName());
   fLbl2->SetText(new TGString(command));
   fTreeHdr->Layout();
   MapSubwindows();
   Resize(GetDefaultSize());
   MapWindow();
   ///Resize();  //ia
   delete[] command;
   PrintEntries();
   return kTRUE;
}

//______________________________________________________________________________
void TTreeViewer::SetRecordName(const char *name)
{
   fSession->SetRecordName(name);
}

//______________________________________________________________________________
void TTreeViewer::SetCurrentRecord(Long64_t entry)
{
   fCombo->Select(entry);
}

//______________________________________________________________________________
void TTreeViewer::SetHistogramTitle(const char *title)
{
   if (!gPad) return;
   TH1 *hist = (TH1*)gPad->GetListOfPrimitives()->FindObject(fBarHist->GetText());
   if (hist) {
      hist->SetTitle(title);
      gPad->Update();
   }
}
//______________________________________________________________________________
void TTreeViewer::SetUserCode(const char *code, Bool_t autoexec)
{
// user defined command for current record
   TTVRecord *rec = fSession->GetCurrent();
   if (rec) rec->SetUserCode(code, autoexec);
}
//______________________________________________________________________________
void TTreeViewer::UpdateCombo()
{
   // Updates combo box to current session entries.

   fCombo->RemoveEntries(0, 1000);
   for (Long64_t entry=0; entry<fSession->GetEntries(); entry++) {
      fCombo->AddEntry(fSession->GetRecord(entry)->GetName(), entry);
   }
}

//______________________________________________________________________________
void TTreeViewer::UpdateRecord(const char *name)
{
   // Updates current record to new X, Y, Z items.

   fSession->UpdateRecord(name);
}

//______________________________________________________________________________
void TTreeViewer::DoRefresh()
{
   // This slot is called when button REFR is clicked

   fTree->Refresh();
   Float_t min = fSlider->GetMinPosition();
   Float_t max = (Float_t)fTree->GetEntries()-1;
   fSlider->SetRange(min,max);
   fSlider->SetPosition(min,max);
   ExecuteDraw();
}
