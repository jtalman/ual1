//
// File generated by utils/src/rootcint_tmp at Sun May 10 15:58:05 2020.
// Do NOT change. Changes will be lost next time file is generated
//

#include "RConfig.h"
#if !defined(R__ACCESS_IN_SYMBOL)
//Break the privacy of classes -- Disabled for the moment
#define private public
#define protected public
#endif

// Since CINT ignores the std namespace, we need to do so in this file.
namespace std {} using namespace std;

#include "G__ASImageGui.h"
#include "TClass.h"
#include "TBuffer.h"
#include "TStreamerInfo.h"
#include "TMemberInspector.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"

#include "TCollectionProxy.h"

namespace ROOT {
   namespace Shadow {
   } // Of namespace ROOT::Shadow
} // Of namespace ROOT

namespace ROOT {
   void TASPaletteEditor_ShowMembers(void *obj, TMemberInspector &R__insp, char *R__parent);
   static TClass *TASPaletteEditor_IsA(const void*);
   static void delete_TASPaletteEditor(void *p);
   static void deleteArray_TASPaletteEditor(void *p);
   static void destruct_TASPaletteEditor(void *p);

   // Function generating the singleton type initializer
   TGenericClassInfo *GenerateInitInstance(const ::TASPaletteEditor*)
   {
      ::TASPaletteEditor *ptr = 0;
      static ::ROOT::TGenericClassInfo 
         instance("TASPaletteEditor", ::TASPaletteEditor::Class_Version(), "asimage/inc/TASPaletteEditor.h", 44,
                  typeid(::TASPaletteEditor), DefineBehavior(ptr, ptr),
                  &::TASPaletteEditor::Dictionary, &TASPaletteEditor_IsA, 0,
                  sizeof(::TASPaletteEditor) );
      instance.SetDelete(&delete_TASPaletteEditor);
      instance.SetDeleteArray(&deleteArray_TASPaletteEditor);
      instance.SetDestructor(&destruct_TASPaletteEditor);
      return &instance;
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_(Init) = GenerateInitInstance((const ::TASPaletteEditor*)0x0); R__UseDummy(_R__UNIQUE_(Init));
}

//______________________________________________________________________________
TClass *TASPaletteEditor::fgIsA = 0;  // static to hold class pointer

//______________________________________________________________________________
const char *TASPaletteEditor::Class_Name()
{
   return "TASPaletteEditor";
}

//______________________________________________________________________________
const char *TASPaletteEditor::ImplFileName()
{
   return ::ROOT::GenerateInitInstance((const ::TASPaletteEditor*)0x0)->GetImplFileName();
}

//______________________________________________________________________________
int TASPaletteEditor::ImplFileLine()
{
   return ::ROOT::GenerateInitInstance((const ::TASPaletteEditor*)0x0)->GetImplFileLine();
}

//______________________________________________________________________________
void TASPaletteEditor::Dictionary()
{
   fgIsA = ::ROOT::GenerateInitInstance((const ::TASPaletteEditor*)0x0)->GetClass();
}

//______________________________________________________________________________
TClass *TASPaletteEditor::Class()
{
   if (!fgIsA) fgIsA = ::ROOT::GenerateInitInstance((const ::TASPaletteEditor*)0x0)->GetClass();
   return fgIsA;
}

//______________________________________________________________________________
void TASPaletteEditor::Streamer(TBuffer &R__b)
{
   // Stream an object of class TASPaletteEditor.

   TPaletteEditor::Streamer(R__b);
   TGMainFrame::Streamer(R__b);
}

//______________________________________________________________________________
void TASPaletteEditor::ShowMembers(TMemberInspector &R__insp, char *R__parent)
{
      // Inspect the data members of an object of class TASPaletteEditor.

      TClass *R__cl = ::TASPaletteEditor::IsA();
      Int_t R__ncp = strlen(R__parent);
      if (R__ncp || R__cl || R__insp.IsA()) { }
      R__insp.Inspect(R__cl, R__parent, "fMinValue", &fMinValue);
      R__insp.Inspect(R__cl, R__parent, "fMaxValue", &fMaxValue);
      R__insp.Inspect(R__cl, R__parent, "*fHisto", &fHisto);
      R__insp.Inspect(R__cl, R__parent, "*fPaletteCanvas", &fPaletteCanvas);
      R__insp.Inspect(R__cl, R__parent, "*fHistCanvas", &fHistCanvas);
      R__insp.Inspect(R__cl, R__parent, "*fPaletteList", &fPaletteList);
      R__insp.Inspect(R__cl, R__parent, "*fPalette", &fPalette);
      R__insp.Inspect(R__cl, R__parent, "*fImagePad", &fImagePad);
      R__insp.Inspect(R__cl, R__parent, "*fPaintPalette", &fPaintPalette);
      R__insp.Inspect(R__cl, R__parent, "*fLimitLine[2]", &fLimitLine);
      R__insp.Inspect(R__cl, R__parent, "*fUnDoButton", &fUnDoButton);
      R__insp.Inspect(R__cl, R__parent, "*fReDoButton", &fReDoButton);
      R__insp.Inspect(R__cl, R__parent, "*fAutoUpdate", &fAutoUpdate);
      R__insp.Inspect(R__cl, R__parent, "*fStepButton", &fStepButton);
      R__insp.Inspect(R__cl, R__parent, "*fRamps[3]", &fRamps);
      R__insp.Inspect(R__cl, R__parent, "fRampFactor", &fRampFactor);
      R__insp.Inspect(R__cl, R__parent, "*fComboBox", &fComboBox);
      TPaletteEditor::ShowMembers(R__insp, R__parent);
      TGMainFrame::ShowMembers(R__insp, R__parent);
}

namespace ROOT {
   // Return the actual TClass for the object argument
   static TClass *TASPaletteEditor_IsA(const void *obj) {
      return ((::TASPaletteEditor*)obj)->IsA();
   }
   // Wrapper around operator delete
   static void delete_TASPaletteEditor(void *p) {
      delete ((::TASPaletteEditor*)p);
   }
   static void deleteArray_TASPaletteEditor(void *p) {
      delete [] ((::TASPaletteEditor*)p);
   }
   static void destruct_TASPaletteEditor(void *p) {
      typedef ::TASPaletteEditor current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::TASPaletteEditor

/********************************************************
* asimage/src/G__ASImageGui.cxx
* CAUTION: DON'T CHANGE THIS FILE. THIS FILE IS AUTOMATICALLY GENERATED
*          FROM HEADER FILES LISTED IN G__setup_cpp_environmentXXX().
*          CHANGE THOSE HEADER FILES AND REGENERATE THIS FILE.
********************************************************/

#ifdef G__MEMTEST
#undef malloc
#undef free
#endif

extern "C" void G__cpp_reset_tagtableG__ASImageGui();

extern "C" void G__set_cpp_environmentG__ASImageGui() {
  G__add_compiledheader("base/inc/TROOT.h");
  G__add_compiledheader("base/inc/TMemberInspector.h");
  G__add_compiledheader("asimage/inc/TASPaletteEditor.h");
  G__cpp_reset_tagtableG__ASImageGui();
}
class G__asimagedIsrcdIG__ASImageGuidOcxx_tag {};

void* operator new(size_t size,G__asimagedIsrcdIG__ASImageGuidOcxx_tag* p) {
  if(p && G__PVOID!=G__getgvp()) return((void*)p);
#ifndef G__ROOT
  return(malloc(size));
#else
  return(::operator new(size));
#endif
}

/* dummy, for exception */
#ifdef G__EH_DUMMY_DELETE
void operator delete(void *p,G__asimagedIsrcdIG__ASImageGuidOcxx_tag* x) {
  if((long)p==G__getgvp() && G__PVOID!=G__getgvp()) return;
#ifndef G__ROOT
  free(p);
#else
  ::operator delete(p);
#endif
}
#endif

static void G__operator_delete(void *p) {
  if((long)p==G__getgvp() && G__PVOID!=G__getgvp()) return;
#ifndef G__ROOT
  free(p);
#else
  ::operator delete(p);
#endif
}

void G__DELDMY_asimagedIsrcdIG__ASImageGuidOcxx() { G__operator_delete(0); }

extern "C" int G__cpp_dllrevG__ASImageGui() { return(30051515); }

/*********************************************************
* Member function Interface Method
*********************************************************/

/* TASPaletteEditor */
static int G__G__ASImageGui_256_2_1(G__value *result7,G__CONST char *funcname,struct G__param *libp,int hash) {
   TASPaletteEditor *p=NULL;
      p = new TASPaletteEditor(
(TAttImage*)G__int(libp->para[0]),(UInt_t)G__int(libp->para[1])
,(UInt_t)G__int(libp->para[2]));
      result7->obj.i = (long)p;
      result7->ref = (long)p;
      result7->type = 'u';
      result7->tagnum = G__get_linked_tagnum(&G__G__ASImageGuiLN_TASPaletteEditor);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__G__ASImageGui_256_3_1(G__value *result7,G__CONST char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,103,(long)((TASPaletteEditor*)(G__getstructoffset()))->ProcessMessage((Long_t)G__int(libp->para[0]),(Long_t)G__int(libp->para[1])
,(Long_t)G__int(libp->para[2])));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__G__ASImageGui_256_4_1(G__value *result7,G__CONST char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((TASPaletteEditor*)(G__getstructoffset()))->UpdateRange();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__G__ASImageGui_256_5_1(G__value *result7,G__CONST char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((TASPaletteEditor*)(G__getstructoffset()))->CloseWindow();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__G__ASImageGui_256_6_1(G__value *result7,G__CONST char *funcname,struct G__param *libp,int hash) {
   G__letint(result7,85,(long)TASPaletteEditor::Class());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__G__ASImageGui_256_7_1(G__value *result7,G__CONST char *funcname,struct G__param *libp,int hash) {
   G__letint(result7,67,(long)TASPaletteEditor::Class_Name());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__G__ASImageGui_256_8_1(G__value *result7,G__CONST char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,115,(long)TASPaletteEditor::Class_Version());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__G__ASImageGui_256_9_1(G__value *result7,G__CONST char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      TASPaletteEditor::Dictionary();
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__G__ASImageGui_256_0_2(G__value *result7,G__CONST char *funcname,struct G__param *libp,int hash) {
   G__letint(result7,85,(long)((const TASPaletteEditor*)(G__getstructoffset()))->IsA());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__G__ASImageGui_256_1_2(G__value *result7,G__CONST char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((TASPaletteEditor*)(G__getstructoffset()))->ShowMembers(*(TMemberInspector*)libp->para[0].ref,(char*)G__int(libp->para[1]));
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__G__ASImageGui_256_2_2(G__value *result7,G__CONST char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((TASPaletteEditor*)(G__getstructoffset()))->Streamer(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__G__ASImageGui_256_3_2(G__value *result7,G__CONST char *funcname,struct G__param *libp,int hash) {
      G__setnull(result7);
      ((TASPaletteEditor*)(G__getstructoffset()))->StreamerNVirtual(*(TBuffer*)libp->para[0].ref);
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__G__ASImageGui_256_4_2(G__value *result7,G__CONST char *funcname,struct G__param *libp,int hash) {
   G__letint(result7,67,(long)TASPaletteEditor::DeclFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__G__ASImageGui_256_5_2(G__value *result7,G__CONST char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)TASPaletteEditor::ImplFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__G__ASImageGui_256_6_2(G__value *result7,G__CONST char *funcname,struct G__param *libp,int hash) {
   G__letint(result7,67,(long)TASPaletteEditor::ImplFileName());
   return(1 || funcname || hash || result7 || libp) ;
}

static int G__G__ASImageGui_256_7_2(G__value *result7,G__CONST char *funcname,struct G__param *libp,int hash) {
      G__letint(result7,105,(long)TASPaletteEditor::DeclFileLine());
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic copy constructor
static int G__G__ASImageGui_256_8_2(G__value *result7,G__CONST char *funcname,struct G__param *libp,int hash)
{
   TASPaletteEditor *p;
   void *xtmp = (void*)G__int(libp->para[0]);
   p=new TASPaletteEditor(*(TASPaletteEditor*)xtmp);
   result7->obj.i = (long)p;
   result7->ref = (long)p;
   result7->type = 'u';
   result7->tagnum = G__get_linked_tagnum(&G__G__ASImageGuiLN_TASPaletteEditor);
   return(1 || funcname || hash || result7 || libp) ;
}

// automatic destructor
typedef TASPaletteEditor G__TTASPaletteEditor;
static int G__G__ASImageGui_256_9_2(G__value *result7,G__CONST char *funcname,struct G__param *libp,int hash) {
   if(0==G__getstructoffset()) return(1);
   if(G__getaryconstruct())
     if(G__PVOID==G__getgvp())
       delete[] (TASPaletteEditor *)(G__getstructoffset());
     else
       for(int i=G__getaryconstruct()-1;i>=0;i--)
         delete (TASPaletteEditor *)((G__getstructoffset())+sizeof(TASPaletteEditor)*i);
   else  delete (TASPaletteEditor *)(G__getstructoffset());
      G__setnull(result7);
   return(1 || funcname || hash || result7 || libp) ;
}


/* Setting up global function */

/*********************************************************
* Member function Stub
*********************************************************/

/* TASPaletteEditor */

/*********************************************************
* Global function Stub
*********************************************************/

/*********************************************************
* Get size of pointer to member function
*********************************************************/
class G__Sizep2memfuncG__ASImageGui {
 public:
  G__Sizep2memfuncG__ASImageGui() {p=&G__Sizep2memfuncG__ASImageGui::sizep2memfunc;}
    size_t sizep2memfunc() { return(sizeof(p)); }
  private:
    size_t (G__Sizep2memfuncG__ASImageGui::*p)();
};

size_t G__get_sizep2memfuncG__ASImageGui()
{
  G__Sizep2memfuncG__ASImageGui a;
  G__setsizep2memfunc((int)a.sizep2memfunc());
  return((size_t)a.sizep2memfunc());
}


/*********************************************************
* virtual base class offset calculation interface
*********************************************************/

   /* Setting up class inheritance */

/*********************************************************
* Inheritance information setup/
*********************************************************/
extern "C" void G__cpp_setup_inheritanceG__ASImageGui() {

   /* Setting up class inheritance */
   if(0==G__getnumbaseclass(G__get_linked_tagnum(&G__G__ASImageGuiLN_TASPaletteEditor))) {
     TASPaletteEditor *G__Lderived;
     G__Lderived=(TASPaletteEditor*)0x1000;
     {
       TPaletteEditor *G__Lpbase=(TPaletteEditor*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__G__ASImageGuiLN_TASPaletteEditor),G__get_linked_tagnum(&G__G__ASImageGuiLN_TPaletteEditor),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TGMainFrame *G__Lpbase=(TGMainFrame*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__G__ASImageGuiLN_TASPaletteEditor),G__get_linked_tagnum(&G__G__ASImageGuiLN_TGMainFrame),(long)G__Lpbase-(long)G__Lderived,1,1);
     }
     {
       TGCompositeFrame *G__Lpbase=(TGCompositeFrame*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__G__ASImageGuiLN_TASPaletteEditor),G__get_linked_tagnum(&G__G__ASImageGuiLN_TGCompositeFrame),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TGFrame *G__Lpbase=(TGFrame*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__G__ASImageGuiLN_TASPaletteEditor),G__get_linked_tagnum(&G__G__ASImageGuiLN_TGFrame),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TGWindow *G__Lpbase=(TGWindow*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__G__ASImageGuiLN_TASPaletteEditor),G__get_linked_tagnum(&G__G__ASImageGuiLN_TGWindow),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TGObject *G__Lpbase=(TGObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__G__ASImageGuiLN_TASPaletteEditor),G__get_linked_tagnum(&G__G__ASImageGuiLN_TGObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TObject *G__Lpbase=(TObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__G__ASImageGuiLN_TASPaletteEditor),G__get_linked_tagnum(&G__G__ASImageGuiLN_TObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
     {
       TQObject *G__Lpbase=(TQObject*)G__Lderived;
       G__inheritance_setup(G__get_linked_tagnum(&G__G__ASImageGuiLN_TASPaletteEditor),G__get_linked_tagnum(&G__G__ASImageGuiLN_TQObject),(long)G__Lpbase-(long)G__Lderived,1,0);
     }
   }
}

/*********************************************************
* typedef information setup/
*********************************************************/
extern "C" void G__cpp_setup_typetableG__ASImageGui() {

   /* Setting up typedef entry */
   G__search_typename2("UInt_t",104,-1,0,
-1);
   G__setnewtype(-1,"Unsigned integer 4 bytes (unsigned int)",0);
   G__search_typename2("Long_t",108,-1,0,
-1);
   G__setnewtype(-1,"Signed long integer 8 bytes (long)",0);
   G__search_typename2("Bool_t",103,-1,0,
-1);
   G__setnewtype(-1,"Boolean (0=false, 1=true) (bool)",0);
   G__search_typename2("Version_t",115,-1,0,
-1);
   G__setnewtype(-1,"Class version identifier (short)",0);
   G__search_typename2("vector<TStreamerInfo*>",117,G__get_linked_tagnum(&G__G__ASImageGuiLN_vectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgR),0,-1);
   G__setnewtype(-1,"// @(#)root/base:$Name:  $:$Id: TROOT.h,v 1.42 2005/03/10 17:57:04 rdm Exp $",0);
   G__search_typename2("reverse_iterator<const_iterator>",117,G__get_linked_tagnum(&G__G__ASImageGuiLN_reverse_iteratorlEvectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgRcLcLiteratorgR),0,G__get_linked_tagnum(&G__G__ASImageGuiLN_vectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgR));
   G__setnewtype(-1,"// @(#)root/base:$Name:  $:$Id: TROOT.h,v 1.42 2005/03/10 17:57:04 rdm Exp $",0);
   G__search_typename2("reverse_iterator<iterator>",117,G__get_linked_tagnum(&G__G__ASImageGuiLN_reverse_iteratorlEvectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgRcLcLiteratorgR),0,G__get_linked_tagnum(&G__G__ASImageGuiLN_vectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgR));
   G__setnewtype(-1,"// @(#)root/base:$Name:  $:$Id: TROOT.h,v 1.42 2005/03/10 17:57:04 rdm Exp $",0);
}

/*********************************************************
* Data Member information setup/
*********************************************************/

   /* Setting up class,struct,union tag member variable */

   /* TASPaletteEditor */
static void G__setup_memvarTASPaletteEditor(void) {
   G__tag_memvar_setup(G__get_linked_tagnum(&G__G__ASImageGuiLN_TASPaletteEditor));
   { TASPaletteEditor *p; p=(TASPaletteEditor*)0x1000; if (p) { }
   G__memvar_setup((void*)NULL,100,0,0,-1,G__defined_typename("Double_t"),-1,2,"fMinValue=",0,"min value of image");
   G__memvar_setup((void*)NULL,100,0,0,-1,G__defined_typename("Double_t"),-1,2,"fMaxValue=",0,"max value of image");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__G__ASImageGuiLN_TH1D),-1,-1,2,"fHisto=",0,"hitogram of image pixels");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__G__ASImageGuiLN_TRootEmbeddedCanvas),-1,-1,2,"fPaletteCanvas=",0,"canvas to draw the current palette");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__G__ASImageGuiLN_TRootEmbeddedCanvas),-1,-1,2,"fHistCanvas=",0,"canvas to draw the histogram");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__G__ASImageGuiLN_TList),-1,-1,2,"fPaletteList=",0,"list of palettes for undo and redo");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__G__ASImageGuiLN_TImagePalette),-1,-1,2,"fPalette=",0,"current palette");
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__G__ASImageGuiLN_TVirtualPad),-1,-1,2,"fImagePad=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__G__ASImageGuiLN_TASPaletteEditorcLcLPaintPalette),-1,-1,2,"fPaintPalette=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__G__ASImageGuiLN_TASPaletteEditorcLcLLimitLine),-1,-1,2,"fLimitLine[2]=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__G__ASImageGuiLN_TGTextButton),-1,-1,2,"fUnDoButton=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__G__ASImageGuiLN_TGTextButton),-1,-1,2,"fReDoButton=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__G__ASImageGuiLN_TGCheckButton),-1,-1,2,"fAutoUpdate=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__G__ASImageGuiLN_TGCheckButton),-1,-1,2,"fStepButton=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__G__ASImageGuiLN_TGRadioButton),-1,-1,2,"fRamps[3]=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,105,0,0,-1,G__defined_typename("Int_t"),-1,2,"fRampFactor=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__G__ASImageGuiLN_TGComboBox),-1,-1,2,"fComboBox=",0,(char*)NULL);
   G__memvar_setup((void*)NULL,85,0,0,G__get_linked_tagnum(&G__G__ASImageGuiLN_TClass),-1,-2,4,"fgIsA=",0,(char*)NULL);
   }
   G__tag_memvar_reset();
}

extern "C" void G__cpp_setup_memvarG__ASImageGui() {
}
/***********************************************************
************************************************************
************************************************************
************************************************************
************************************************************
************************************************************
************************************************************
***********************************************************/

/*********************************************************
* Member function information setup for each class
*********************************************************/
static void G__setup_memfuncTASPaletteEditor(void) {
   /* TASPaletteEditor */
   G__tag_memfunc_setup(G__get_linked_tagnum(&G__G__ASImageGuiLN_TASPaletteEditor));
   G__memfunc_setup("InsertNewPalette",1646,(G__InterfaceMethod)NULL,121,-1,-1,0,1,1,2,0,"U 'TImagePalette' - 0 - newPalette",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Save",399,(G__InterfaceMethod)NULL,121,-1,-1,0,0,1,2,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("Open",402,(G__InterfaceMethod)NULL,121,-1,-1,0,0,1,2,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("LogPalette",1009,(G__InterfaceMethod)NULL,121,-1,-1,0,0,1,2,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ExpPalette",1020,(G__InterfaceMethod)NULL,121,-1,-1,0,0,1,2,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("LinPalette",1010,(G__InterfaceMethod)NULL,121,-1,-1,0,0,1,2,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("InvertPalette",1351,(G__InterfaceMethod)NULL,121,-1,-1,0,0,1,2,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("NewPalette",1017,(G__InterfaceMethod)NULL,121,-1,-1,0,1,1,2,0,"l - 'Long_t' 0 - id",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("SetStep",712,(G__InterfaceMethod)NULL,121,-1,-1,0,0,1,2,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("SetRamp",700,(G__InterfaceMethod)NULL,121,-1,-1,0,1,1,2,0,"l - 'Long_t' 0 - ramp",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("UpdateScreen",1219,(G__InterfaceMethod)NULL,121,-1,-1,0,1,1,2,0,"g - 'Bool_t' 0 - histoUpdate",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("TASPaletteEditor",1566,G__G__ASImageGui_256_2_1,105,G__get_linked_tagnum(&G__G__ASImageGuiLN_TASPaletteEditor),-1,0,3,1,1,0,
"U 'TAttImage' - 0 - attImage h - 'UInt_t' 0 - w "
"h - 'UInt_t' 0 - h",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("ProcessMessage",1444,G__G__ASImageGui_256_3_1,103,-1,G__defined_typename("Bool_t"),0,3,1,1,0,
"l - 'Long_t' 0 - msg l - 'Long_t' 0 - param1 "
"l - 'Long_t' 0 - param2",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("UpdateRange",1104,G__G__ASImageGui_256_4_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("CloseWindow",1134,G__G__ASImageGui_256_5_1,121,-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Class",502,G__G__ASImageGui_256_6_1,85,G__get_linked_tagnum(&G__G__ASImageGuiLN_TClass),-1,0,0,3,1,0,"",(char*)NULL,(void*)(TClass* (*)())(&TASPaletteEditor::Class),0);
   G__memfunc_setup("Class_Name",982,G__G__ASImageGui_256_7_1,67,-1,-1,0,0,3,1,1,"",(char*)NULL,(void*)(const char* (*)())(&TASPaletteEditor::Class_Name),0);
   G__memfunc_setup("Class_Version",1339,G__G__ASImageGui_256_8_1,115,-1,G__defined_typename("Version_t"),0,0,3,1,0,"",(char*)NULL,(void*)(Version_t (*)())(&TASPaletteEditor::Class_Version),0);
   G__memfunc_setup("Dictionary",1046,G__G__ASImageGui_256_9_1,121,-1,-1,0,0,3,1,0,"",(char*)NULL,(void*)(void (*)())(&TASPaletteEditor::Dictionary),0);
   G__memfunc_setup("IsA",253,G__G__ASImageGui_256_0_2,85,G__get_linked_tagnum(&G__G__ASImageGuiLN_TClass),-1,0,0,1,1,8,"",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("ShowMembers",1132,G__G__ASImageGui_256_1_2,121,-1,-1,0,2,1,1,0,
"u 'TMemberInspector' - 1 - insp C - - 0 - parent",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("Streamer",835,G__G__ASImageGui_256_2_2,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,1);
   G__memfunc_setup("StreamerNVirtual",1656,G__G__ASImageGui_256_3_2,121,-1,-1,0,1,1,1,0,"u 'TBuffer' - 1 - b",(char*)NULL,(void*)NULL,0);
   G__memfunc_setup("DeclFileName",1145,G__G__ASImageGui_256_4_2,67,-1,-1,0,0,3,1,1,"",(char*)NULL,(void*)(const char* (*)())(&TASPaletteEditor::DeclFileName),0);
   G__memfunc_setup("ImplFileLine",1178,G__G__ASImageGui_256_5_2,105,-1,-1,0,0,3,1,0,"",(char*)NULL,(void*)(int (*)())(&TASPaletteEditor::ImplFileLine),0);
   G__memfunc_setup("ImplFileName",1171,G__G__ASImageGui_256_6_2,67,-1,-1,0,0,3,1,1,"",(char*)NULL,(void*)(const char* (*)())(&TASPaletteEditor::ImplFileName),0);
   G__memfunc_setup("DeclFileLine",1152,G__G__ASImageGui_256_7_2,105,-1,-1,0,0,3,1,0,"",(char*)NULL,(void*)(int (*)())(&TASPaletteEditor::DeclFileLine),0);
   // automatic copy constructor
   G__memfunc_setup("TASPaletteEditor",1566,G__G__ASImageGui_256_8_2,(int)('i'),G__get_linked_tagnum(&G__G__ASImageGuiLN_TASPaletteEditor),-1,0,1,1,1,0,"u 'TASPaletteEditor' - 11 - -",(char*)NULL,(void*)NULL,0);
   // automatic destructor
   G__memfunc_setup("~TASPaletteEditor",1692,G__G__ASImageGui_256_9_2,(int)('y'),-1,-1,0,0,1,1,0,"",(char*)NULL,(void*)NULL,1);
   G__tag_memfunc_reset();
}


/*********************************************************
* Member function information setup
*********************************************************/
extern "C" void G__cpp_setup_memfuncG__ASImageGui() {
}

/*********************************************************
* Global variable information setup for each class
*********************************************************/
static void G__cpp_setup_global0() {

   /* Setting up global variables */
   G__resetplocal();

}

static void G__cpp_setup_global1() {
}

static void G__cpp_setup_global2() {
}

static void G__cpp_setup_global3() {
}

static void G__cpp_setup_global4() {
}

static void G__cpp_setup_global5() {

   G__resetglobalenv();
}
extern "C" void G__cpp_setup_globalG__ASImageGui() {
  G__cpp_setup_global0();
  G__cpp_setup_global1();
  G__cpp_setup_global2();
  G__cpp_setup_global3();
  G__cpp_setup_global4();
  G__cpp_setup_global5();
}

/*********************************************************
* Global function information setup for each class
*********************************************************/
static void G__cpp_setup_func0() {
   G__lastifuncposition();

}

static void G__cpp_setup_func1() {
}

static void G__cpp_setup_func2() {
}

static void G__cpp_setup_func3() {

   G__resetifuncposition();
}

extern "C" void G__cpp_setup_funcG__ASImageGui() {
  G__cpp_setup_func0();
  G__cpp_setup_func1();
  G__cpp_setup_func2();
  G__cpp_setup_func3();
}

/*********************************************************
* Class,struct,union,enum tag information setup
*********************************************************/
/* Setup class/struct taginfo */
G__linked_taginfo G__G__ASImageGuiLN_TClass = { "TClass" , 99 , -1 };
G__linked_taginfo G__G__ASImageGuiLN_TBuffer = { "TBuffer" , 99 , -1 };
G__linked_taginfo G__G__ASImageGuiLN_TMemberInspector = { "TMemberInspector" , 99 , -1 };
G__linked_taginfo G__G__ASImageGuiLN_TObject = { "TObject" , 99 , -1 };
G__linked_taginfo G__G__ASImageGuiLN_TList = { "TList" , 99 , -1 };
G__linked_taginfo G__G__ASImageGuiLN_vectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgR = { "vector<TStreamerInfo*,allocator<TStreamerInfo*> >" , 99 , -1 };
G__linked_taginfo G__G__ASImageGuiLN_reverse_iteratorlEvectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgRcLcLiteratorgR = { "reverse_iterator<vector<TStreamerInfo*,allocator<TStreamerInfo*> >::iterator>" , 99 , -1 };
G__linked_taginfo G__G__ASImageGuiLN_TVirtualPad = { "TVirtualPad" , 99 , -1 };
G__linked_taginfo G__G__ASImageGuiLN_TAttImage = { "TAttImage" , 99 , -1 };
G__linked_taginfo G__G__ASImageGuiLN_TPaletteEditor = { "TPaletteEditor" , 99 , -1 };
G__linked_taginfo G__G__ASImageGuiLN_TImagePalette = { "TImagePalette" , 99 , -1 };
G__linked_taginfo G__G__ASImageGuiLN_TGObject = { "TGObject" , 99 , -1 };
G__linked_taginfo G__G__ASImageGuiLN_TGWindow = { "TGWindow" , 99 , -1 };
G__linked_taginfo G__G__ASImageGuiLN_TQObject = { "TQObject" , 99 , -1 };
G__linked_taginfo G__G__ASImageGuiLN_TGFrame = { "TGFrame" , 99 , -1 };
G__linked_taginfo G__G__ASImageGuiLN_TGCompositeFrame = { "TGCompositeFrame" , 99 , -1 };
G__linked_taginfo G__G__ASImageGuiLN_TGMainFrame = { "TGMainFrame" , 99 , -1 };
G__linked_taginfo G__G__ASImageGuiLN_TH1D = { "TH1D" , 99 , -1 };
G__linked_taginfo G__G__ASImageGuiLN_TRootEmbeddedCanvas = { "TRootEmbeddedCanvas" , 99 , -1 };
G__linked_taginfo G__G__ASImageGuiLN_TGTextButton = { "TGTextButton" , 99 , -1 };
G__linked_taginfo G__G__ASImageGuiLN_TGCheckButton = { "TGCheckButton" , 99 , -1 };
G__linked_taginfo G__G__ASImageGuiLN_TGComboBox = { "TGComboBox" , 99 , -1 };
G__linked_taginfo G__G__ASImageGuiLN_TGRadioButton = { "TGRadioButton" , 99 , -1 };
G__linked_taginfo G__G__ASImageGuiLN_TASPaletteEditor = { "TASPaletteEditor" , 99 , -1 };
G__linked_taginfo G__G__ASImageGuiLN_TASPaletteEditorcLcLPaintPalette = { "TASPaletteEditor::PaintPalette" , 99 , -1 };
G__linked_taginfo G__G__ASImageGuiLN_TASPaletteEditorcLcLLimitLine = { "TASPaletteEditor::LimitLine" , 99 , -1 };

/* Reset class/struct taginfo */
extern "C" void G__cpp_reset_tagtableG__ASImageGui() {
  G__G__ASImageGuiLN_TClass.tagnum = -1 ;
  G__G__ASImageGuiLN_TBuffer.tagnum = -1 ;
  G__G__ASImageGuiLN_TMemberInspector.tagnum = -1 ;
  G__G__ASImageGuiLN_TObject.tagnum = -1 ;
  G__G__ASImageGuiLN_TList.tagnum = -1 ;
  G__G__ASImageGuiLN_vectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgR.tagnum = -1 ;
  G__G__ASImageGuiLN_reverse_iteratorlEvectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgRcLcLiteratorgR.tagnum = -1 ;
  G__G__ASImageGuiLN_TVirtualPad.tagnum = -1 ;
  G__G__ASImageGuiLN_TAttImage.tagnum = -1 ;
  G__G__ASImageGuiLN_TPaletteEditor.tagnum = -1 ;
  G__G__ASImageGuiLN_TImagePalette.tagnum = -1 ;
  G__G__ASImageGuiLN_TGObject.tagnum = -1 ;
  G__G__ASImageGuiLN_TGWindow.tagnum = -1 ;
  G__G__ASImageGuiLN_TQObject.tagnum = -1 ;
  G__G__ASImageGuiLN_TGFrame.tagnum = -1 ;
  G__G__ASImageGuiLN_TGCompositeFrame.tagnum = -1 ;
  G__G__ASImageGuiLN_TGMainFrame.tagnum = -1 ;
  G__G__ASImageGuiLN_TH1D.tagnum = -1 ;
  G__G__ASImageGuiLN_TRootEmbeddedCanvas.tagnum = -1 ;
  G__G__ASImageGuiLN_TGTextButton.tagnum = -1 ;
  G__G__ASImageGuiLN_TGCheckButton.tagnum = -1 ;
  G__G__ASImageGuiLN_TGComboBox.tagnum = -1 ;
  G__G__ASImageGuiLN_TGRadioButton.tagnum = -1 ;
  G__G__ASImageGuiLN_TASPaletteEditor.tagnum = -1 ;
  G__G__ASImageGuiLN_TASPaletteEditorcLcLPaintPalette.tagnum = -1 ;
  G__G__ASImageGuiLN_TASPaletteEditorcLcLLimitLine.tagnum = -1 ;
}


extern "C" void G__cpp_setup_tagtableG__ASImageGui() {

   /* Setting up class,struct,union tag entry */
   G__get_linked_tagnum(&G__G__ASImageGuiLN_TClass);
   G__get_linked_tagnum(&G__G__ASImageGuiLN_TBuffer);
   G__get_linked_tagnum(&G__G__ASImageGuiLN_TMemberInspector);
   G__get_linked_tagnum(&G__G__ASImageGuiLN_TObject);
   G__get_linked_tagnum(&G__G__ASImageGuiLN_TList);
   G__get_linked_tagnum(&G__G__ASImageGuiLN_vectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgR);
   G__get_linked_tagnum(&G__G__ASImageGuiLN_reverse_iteratorlEvectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgRcLcLiteratorgR);
   G__get_linked_tagnum(&G__G__ASImageGuiLN_TVirtualPad);
   G__get_linked_tagnum(&G__G__ASImageGuiLN_TAttImage);
   G__get_linked_tagnum(&G__G__ASImageGuiLN_TPaletteEditor);
   G__get_linked_tagnum(&G__G__ASImageGuiLN_TImagePalette);
   G__get_linked_tagnum(&G__G__ASImageGuiLN_TGObject);
   G__get_linked_tagnum(&G__G__ASImageGuiLN_TGWindow);
   G__get_linked_tagnum(&G__G__ASImageGuiLN_TQObject);
   G__get_linked_tagnum(&G__G__ASImageGuiLN_TGFrame);
   G__get_linked_tagnum(&G__G__ASImageGuiLN_TGCompositeFrame);
   G__get_linked_tagnum(&G__G__ASImageGuiLN_TGMainFrame);
   G__get_linked_tagnum(&G__G__ASImageGuiLN_TH1D);
   G__get_linked_tagnum(&G__G__ASImageGuiLN_TRootEmbeddedCanvas);
   G__get_linked_tagnum(&G__G__ASImageGuiLN_TGTextButton);
   G__get_linked_tagnum(&G__G__ASImageGuiLN_TGCheckButton);
   G__get_linked_tagnum(&G__G__ASImageGuiLN_TGComboBox);
   G__get_linked_tagnum(&G__G__ASImageGuiLN_TGRadioButton);
   G__tagtable_setup(G__get_linked_tagnum(&G__G__ASImageGuiLN_TASPaletteEditor),sizeof(TASPaletteEditor),-1,62464,"GUI to edit a color palette",G__setup_memvarTASPaletteEditor,G__setup_memfuncTASPaletteEditor);
   G__get_linked_tagnum(&G__G__ASImageGuiLN_TASPaletteEditorcLcLPaintPalette);
   G__get_linked_tagnum(&G__G__ASImageGuiLN_TASPaletteEditorcLcLLimitLine);
}
extern "C" void G__cpp_setupG__ASImageGui(void) {
  G__check_setup_version(30051515,"G__cpp_setupG__ASImageGui()");
  G__set_cpp_environmentG__ASImageGui();
  G__cpp_setup_tagtableG__ASImageGui();

  G__cpp_setup_inheritanceG__ASImageGui();

  G__cpp_setup_typetableG__ASImageGui();

  G__cpp_setup_memvarG__ASImageGui();

  G__cpp_setup_memfuncG__ASImageGui();
  G__cpp_setup_globalG__ASImageGui();
  G__cpp_setup_funcG__ASImageGui();

   if(0==G__getsizep2memfunc()) G__get_sizep2memfuncG__ASImageGui();
  return;
}
class G__cpp_setup_initG__ASImageGui {
  public:
    G__cpp_setup_initG__ASImageGui() { G__add_setup_func("G__ASImageGui",(G__incsetup)(&G__cpp_setupG__ASImageGui)); G__call_setup_funcs(); }
   ~G__cpp_setup_initG__ASImageGui() { G__remove_setup_func("G__ASImageGui"); }
};
G__cpp_setup_initG__ASImageGui G__cpp_setup_initializerG__ASImageGui;
