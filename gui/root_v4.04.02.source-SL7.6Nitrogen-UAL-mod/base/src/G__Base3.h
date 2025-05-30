
/* Includes added by #pragma extra_include */
#include "string"
#include "Rpair.h"
#include "Rtypes.h"
/********************************************************************
* base/src/G__Base3.h
* CAUTION: DON'T CHANGE THIS FILE. THIS FILE IS AUTOMATICALLY GENERATED
*          FROM HEADER FILES LISTED IN G__setup_cpp_environmentXXX().
*          CHANGE THOSE HEADER FILES AND REGENERATE THIS FILE.
********************************************************************/
#ifdef __CINT__
#error base/src/G__Base3.h/C is only for compilation. Abort cint.
#endif
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define G__ANSIHEADER
#define G__DICTIONARY
#include "G__ci.h"
extern "C" {
extern void G__cpp_setup_tagtableG__Base3();
extern void G__cpp_setup_inheritanceG__Base3();
extern void G__cpp_setup_typetableG__Base3();
extern void G__cpp_setup_memvarG__Base3();
extern void G__cpp_setup_globalG__Base3();
extern void G__cpp_setup_memfuncG__Base3();
extern void G__cpp_setup_funcG__Base3();
extern void G__set_cpp_environmentG__Base3();
}


#include "base/inc/TROOT.h"
#include "base/inc/TMemberInspector.h"
#include "base/inc/GuiTypes.h"
#include "base/inc/KeySymbols.h"
#include "base/inc/Buttons.h"
#include "base/inc/TTimeStamp.h"
#include "base/inc/TVirtualMutex.h"
#include "base/inc/TVirtualProof.h"
#include "base/inc/TVirtualPerfStats.h"
#include "base/inc/TVirtualX.h"
#include "base/inc/TParameter.h"
#include "base/inc/TArchiveFile.h"
#include "base/inc/TZIPFile.h"

#include <algorithm>
namespace std { }
using namespace std;

#ifndef G__MEMFUNCBODY
#endif

extern G__linked_taginfo G__G__Base3LN_TClass;
extern G__linked_taginfo G__G__Base3LN_TBuffer;
extern G__linked_taginfo G__G__Base3LN_TMemberInspector;
extern G__linked_taginfo G__G__Base3LN_TObject;
extern G__linked_taginfo G__G__Base3LN_basic_ostreamlEcharcOchar_traitslEchargRsPgR;
extern G__linked_taginfo G__G__Base3LN_string;
extern G__linked_taginfo G__G__Base3LN_TList;
extern G__linked_taginfo G__G__Base3LN_TObjArray;
extern G__linked_taginfo G__G__Base3LN_vectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgR;
extern G__linked_taginfo G__G__Base3LN_reverse_iteratorlEvectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__G__Base3LN_TString;
extern G__linked_taginfo G__G__Base3LN_TDatime;
extern G__linked_taginfo G__G__Base3LN_TFile;
extern G__linked_taginfo G__G__Base3LN_TVirtualProof;
extern G__linked_taginfo G__G__Base3LN_EGuiConstants;
extern G__linked_taginfo G__G__Base3LN_EGEventType;
extern G__linked_taginfo G__G__Base3LN_EGraphicsFunction;
extern G__linked_taginfo G__G__Base3LN_dA;
extern G__linked_taginfo G__G__Base3LN_SetWindowAttributes_t;
extern G__linked_taginfo G__G__Base3LN_WindowAttributes_t;
extern G__linked_taginfo G__G__Base3LN_Event_t;
extern G__linked_taginfo G__G__Base3LN_EMouseButton;
extern G__linked_taginfo G__G__Base3LN_EXMagic;
extern G__linked_taginfo G__G__Base3LN_GCValues_t;
extern G__linked_taginfo G__G__Base3LN_ColorStruct_t;
extern G__linked_taginfo G__G__Base3LN_PictureAttributes_t;
extern G__linked_taginfo G__G__Base3LN_EInitialState;
extern G__linked_taginfo G__G__Base3LN_Segment_t;
extern G__linked_taginfo G__G__Base3LN_Point_t;
extern G__linked_taginfo G__G__Base3LN_Rectangle_t;
extern G__linked_taginfo G__G__Base3LN_EKeySym;
extern G__linked_taginfo G__G__Base3LN_EEventType;
extern G__linked_taginfo G__G__Base3LN_tm;
extern G__linked_taginfo G__G__Base3LN_timespec;
extern G__linked_taginfo G__G__Base3LN_TTimeStamp;
extern G__linked_taginfo G__G__Base3LN_TVirtualMutex;
extern G__linked_taginfo G__G__Base3LN_TLockGuard;
extern G__linked_taginfo G__G__Base3LN_TArchiveFile;
extern G__linked_taginfo G__G__Base3LN_TQObject;
extern G__linked_taginfo G__G__Base3LN_TDSet;
extern G__linked_taginfo G__G__Base3LN_TEventList;
extern G__linked_taginfo G__G__Base3LN_TTree;
extern G__linked_taginfo G__G__Base3LN_TDrawFeedback;
extern G__linked_taginfo G__G__Base3LN_TChain;
extern G__linked_taginfo G__G__Base3LN_TVirtualPerfStats;
extern G__linked_taginfo G__G__Base3LN_TVirtualPerfStatscLcLEEventType;
extern G__linked_taginfo G__G__Base3LN_ECursor;
extern G__linked_taginfo G__G__Base3LN_TArchiveMember;
extern G__linked_taginfo G__G__Base3LN_TZIPMember;
extern G__linked_taginfo G__G__Base3LN_TZIPFile;
extern G__linked_taginfo G__G__Base3LN_TZIPFilecLcLEZIPConstants;
extern G__linked_taginfo G__G__Base3LN_pairlEcharmUcOintgR;
extern G__linked_taginfo G__G__Base3LN_pairlEcharmUcOlonggR;
extern G__linked_taginfo G__G__Base3LN_pairlEcharmUcOfloatgR;
extern G__linked_taginfo G__G__Base3LN_pairlEcharmUcOdoublegR;
extern G__linked_taginfo G__G__Base3LN_pairlEcharmUcOvoidmUgR;
extern G__linked_taginfo G__G__Base3LN_pairlEcharmUcOcharmUgR;
extern G__linked_taginfo G__G__Base3LN_pairlEstringcOintgR;
extern G__linked_taginfo G__G__Base3LN_pairlEstringcOlonggR;
extern G__linked_taginfo G__G__Base3LN_pairlEstringcOfloatgR;
extern G__linked_taginfo G__G__Base3LN_pairlEstringcOdoublegR;
extern G__linked_taginfo G__G__Base3LN_pairlEstringcOvoidmUgR;
extern G__linked_taginfo G__G__Base3LN_pairlElongcOintgR;
extern G__linked_taginfo G__G__Base3LN_pairlElongcOlonggR;
extern G__linked_taginfo G__G__Base3LN_pairlElongcOfloatgR;
extern G__linked_taginfo G__G__Base3LN_pairlElongcOdoublegR;
extern G__linked_taginfo G__G__Base3LN_pairlElongcOvoidmUgR;
extern G__linked_taginfo G__G__Base3LN_pairlElongcOcharmUgR;
extern G__linked_taginfo G__G__Base3LN_pairlEdoublecOintgR;
extern G__linked_taginfo G__G__Base3LN_pairlEdoublecOlonggR;
extern G__linked_taginfo G__G__Base3LN_pairlEdoublecOfloatgR;
extern G__linked_taginfo G__G__Base3LN_pairlEdoublecOdoublegR;
extern G__linked_taginfo G__G__Base3LN_pairlEdoublecOvoidmUgR;
extern G__linked_taginfo G__G__Base3LN_pairlEdoublecOcharmUgR;
extern G__linked_taginfo G__G__Base3LN_pairlEconstsPcharmUcOintgR;
extern G__linked_taginfo G__G__Base3LN_pairlEconstsPcharmUcOlonggR;
extern G__linked_taginfo G__G__Base3LN_pairlEconstsPcharmUcOfloatgR;
extern G__linked_taginfo G__G__Base3LN_pairlEconstsPcharmUcOdoublegR;
extern G__linked_taginfo G__G__Base3LN_pairlEconstsPcharmUcOvoidmUgR;
extern G__linked_taginfo G__G__Base3LN_pairlEconstsPcharmUcOcharmUgR;
extern G__linked_taginfo G__G__Base3LN_pairlEconstsPstringcOintgR;
extern G__linked_taginfo G__G__Base3LN_pairlEconstsPstringcOlonggR;
extern G__linked_taginfo G__G__Base3LN_pairlEconstsPstringcOfloatgR;
extern G__linked_taginfo G__G__Base3LN_pairlEconstsPstringcOdoublegR;
extern G__linked_taginfo G__G__Base3LN_pairlEconstsPstringcOvoidmUgR;
extern G__linked_taginfo G__G__Base3LN_pairlEconstsPlongcOintgR;
extern G__linked_taginfo G__G__Base3LN_pairlEconstsPlongcOlonggR;
extern G__linked_taginfo G__G__Base3LN_pairlEconstsPlongcOfloatgR;
extern G__linked_taginfo G__G__Base3LN_pairlEconstsPlongcOdoublegR;
extern G__linked_taginfo G__G__Base3LN_pairlEconstsPlongcOvoidmUgR;
extern G__linked_taginfo G__G__Base3LN_pairlEconstsPlongcOcharmUgR;
extern G__linked_taginfo G__G__Base3LN_pairlEconstsPdoublecOintgR;
extern G__linked_taginfo G__G__Base3LN_pairlEconstsPdoublecOlonggR;
extern G__linked_taginfo G__G__Base3LN_pairlEconstsPdoublecOfloatgR;
extern G__linked_taginfo G__G__Base3LN_pairlEconstsPdoublecOdoublegR;
extern G__linked_taginfo G__G__Base3LN_pairlEconstsPdoublecOvoidmUgR;
extern G__linked_taginfo G__G__Base3LN_pairlEconstsPdoublecOcharmUgR;
extern G__linked_taginfo G__G__Base3LN_TParameterlEdoublegR;
extern G__linked_taginfo G__G__Base3LN_TParameterlElonggR;
extern G__linked_taginfo G__G__Base3LN_TParameterlElongsPlonggR;

/* STUB derived class for protected member access */
typedef pair<char*,int> G__pairlEcharmUcOintgR;
typedef pair<char*,long> G__pairlEcharmUcOlonggR;
typedef pair<char*,float> G__pairlEcharmUcOfloatgR;
typedef pair<char*,double> G__pairlEcharmUcOdoublegR;
typedef pair<char*,void*> G__pairlEcharmUcOvoidmUgR;
typedef pair<char*,char*> G__pairlEcharmUcOcharmUgR;
typedef pair<string,int> G__pairlEstringcOintgR;
typedef pair<string,long> G__pairlEstringcOlonggR;
typedef pair<string,float> G__pairlEstringcOfloatgR;
typedef pair<string,double> G__pairlEstringcOdoublegR;
typedef pair<string,void*> G__pairlEstringcOvoidmUgR;
typedef pair<long,int> G__pairlElongcOintgR;
typedef pair<long,long> G__pairlElongcOlonggR;
typedef pair<long,float> G__pairlElongcOfloatgR;
typedef pair<long,double> G__pairlElongcOdoublegR;
typedef pair<long,void*> G__pairlElongcOvoidmUgR;
typedef pair<long,char*> G__pairlElongcOcharmUgR;
typedef pair<double,int> G__pairlEdoublecOintgR;
typedef pair<double,long> G__pairlEdoublecOlonggR;
typedef pair<double,float> G__pairlEdoublecOfloatgR;
typedef pair<double,double> G__pairlEdoublecOdoublegR;
typedef pair<double,void*> G__pairlEdoublecOvoidmUgR;
typedef pair<double,char*> G__pairlEdoublecOcharmUgR;
typedef pair<const char*,int> G__pairlEconstsPcharmUcOintgR;
typedef pair<const char*,long> G__pairlEconstsPcharmUcOlonggR;
typedef pair<const char*,float> G__pairlEconstsPcharmUcOfloatgR;
typedef pair<const char*,double> G__pairlEconstsPcharmUcOdoublegR;
typedef pair<const char*,void*> G__pairlEconstsPcharmUcOvoidmUgR;
typedef pair<const char*,char*> G__pairlEconstsPcharmUcOcharmUgR;
typedef pair<const string,int> G__pairlEconstsPstringcOintgR;
typedef pair<const string,long> G__pairlEconstsPstringcOlonggR;
typedef pair<const string,float> G__pairlEconstsPstringcOfloatgR;
typedef pair<const string,double> G__pairlEconstsPstringcOdoublegR;
typedef pair<const string,void*> G__pairlEconstsPstringcOvoidmUgR;
typedef pair<const long,int> G__pairlEconstsPlongcOintgR;
typedef pair<const long,long> G__pairlEconstsPlongcOlonggR;
typedef pair<const long,float> G__pairlEconstsPlongcOfloatgR;
typedef pair<const long,double> G__pairlEconstsPlongcOdoublegR;
typedef pair<const long,void*> G__pairlEconstsPlongcOvoidmUgR;
typedef pair<const long,char*> G__pairlEconstsPlongcOcharmUgR;
typedef pair<const double,int> G__pairlEconstsPdoublecOintgR;
typedef pair<const double,long> G__pairlEconstsPdoublecOlonggR;
typedef pair<const double,float> G__pairlEconstsPdoublecOfloatgR;
typedef pair<const double,double> G__pairlEconstsPdoublecOdoublegR;
typedef pair<const double,void*> G__pairlEconstsPdoublecOvoidmUgR;
typedef pair<const double,char*> G__pairlEconstsPdoublecOcharmUgR;
typedef TParameter<double> G__TParameterlEdoublegR;
typedef TParameter<long> G__TParameterlElonggR;
typedef TParameter<long long> G__TParameterlElongsPlonggR;
