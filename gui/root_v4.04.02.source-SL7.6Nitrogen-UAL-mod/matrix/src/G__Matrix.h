/********************************************************************
* matrix/src/G__Matrix.h
* CAUTION: DON'T CHANGE THIS FILE. THIS FILE IS AUTOMATICALLY GENERATED
*          FROM HEADER FILES LISTED IN G__setup_cpp_environmentXXX().
*          CHANGE THOSE HEADER FILES AND REGENERATE THIS FILE.
********************************************************************/
#ifdef __CINT__
#error matrix/src/G__Matrix.h/C is only for compilation. Abort cint.
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
extern void G__cpp_setup_tagtableG__Matrix();
extern void G__cpp_setup_inheritanceG__Matrix();
extern void G__cpp_setup_typetableG__Matrix();
extern void G__cpp_setup_memvarG__Matrix();
extern void G__cpp_setup_globalG__Matrix();
extern void G__cpp_setup_memfuncG__Matrix();
extern void G__cpp_setup_funcG__Matrix();
extern void G__set_cpp_environmentG__Matrix();
}


#include "base/inc/TROOT.h"
#include "base/inc/TMemberInspector.h"
#include "matrix/inc/TDecompBK.h"
#include "matrix/inc/TMatrixDLazy.h"
#include "matrix/inc/TDecompBase.h"
#include "matrix/inc/TMatrixFBase.h"
#include "matrix/inc/TDecompSparse.h"
#include "matrix/inc/TMatrix.h"
#include "matrix/inc/TDecompQRH.h"
#include "matrix/inc/TMatrixDBase.h"
#include "matrix/inc/TVectorD.h"
#include "matrix/inc/TMatrixFUtils.h"
#include "matrix/inc/TDecompLU.h"
#include "matrix/inc/TMatrixDEigen.h"
#include "matrix/inc/TMatrixD.h"
#include "matrix/inc/TDecompChol.h"
#include "matrix/inc/TVector.h"
#include "matrix/inc/TMatrixFCramerInv.h"
#include "matrix/inc/TDecompSVD.h"
#include "matrix/inc/TMatrixDSparse.h"
#include "matrix/inc/TVectorF.h"
#include "matrix/inc/TMatrixDCramerInv.h"
#include "matrix/inc/TMatrixFSym.h"
#include "matrix/inc/TMatrixFSymCramerInv.h"
#include "matrix/inc/TMatrixF.h"
#include "matrix/inc/TMatrixDUtils.h"
#include "matrix/inc/TMatrixDSymEigen.h"
#include "matrix/inc/TMatrixDSymCramerInv.h"
#include "matrix/inc/TMatrixFLazy.h"
#include "matrix/inc/TMatrixDSym.h"
#include <algorithm>
namespace std { }
using namespace std;

#ifndef G__MEMFUNCBODY
#endif

extern G__linked_taginfo G__G__MatrixLN_TClass;
extern G__linked_taginfo G__G__MatrixLN_TBuffer;
extern G__linked_taginfo G__G__MatrixLN_TMemberInspector;
extern G__linked_taginfo G__G__MatrixLN_TObject;
extern G__linked_taginfo G__G__MatrixLN_vectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgR;
extern G__linked_taginfo G__G__MatrixLN_reverse_iteratorlEvectorlETStreamerInfomUcOallocatorlETStreamerInfomUgRsPgRcLcLiteratorgR;
extern G__linked_taginfo G__G__MatrixLN_TMatrixFBase;
extern G__linked_taginfo G__G__MatrixLN_TMatrixDBase;
extern G__linked_taginfo G__G__MatrixLN_TVectorF;
extern G__linked_taginfo G__G__MatrixLN_TVectorD;
extern G__linked_taginfo G__G__MatrixLN_TElementActionD;
extern G__linked_taginfo G__G__MatrixLN_TElementPosActionD;
extern G__linked_taginfo G__G__MatrixLN_TMatrixDBasecLcLdA;
extern G__linked_taginfo G__G__MatrixLN_TMatrixDBasecLcLEMatrixStatusBits;
extern G__linked_taginfo G__G__MatrixLN_TMatrixDBasecLcLEMatrixCreatorsOp1;
extern G__linked_taginfo G__G__MatrixLN_TMatrixDBasecLcLEMatrixCreatorsOp2;
extern G__linked_taginfo G__G__MatrixLN_TMatrixD;
extern G__linked_taginfo G__G__MatrixLN_TMatrixDSym;
extern G__linked_taginfo G__G__MatrixLN_TMatrixDSparse;
extern G__linked_taginfo G__G__MatrixLN_TMatrixDRow_const;
extern G__linked_taginfo G__G__MatrixLN_TMatrixDRow;
extern G__linked_taginfo G__G__MatrixLN_TMatrixDColumn_const;
extern G__linked_taginfo G__G__MatrixLN_TMatrixDColumn;
extern G__linked_taginfo G__G__MatrixLN_TMatrixDDiag_const;
extern G__linked_taginfo G__G__MatrixLN_TMatrixDDiag;
extern G__linked_taginfo G__G__MatrixLN_TMatrixDFlat_const;
extern G__linked_taginfo G__G__MatrixLN_TMatrixDFlat;
extern G__linked_taginfo G__G__MatrixLN_TMatrixDSub_const;
extern G__linked_taginfo G__G__MatrixLN_TMatrixDSub_constcLcLdA;
extern G__linked_taginfo G__G__MatrixLN_TMatrixDSub;
extern G__linked_taginfo G__G__MatrixLN_TMatrixDSparseRow_const;
extern G__linked_taginfo G__G__MatrixLN_TMatrixDSparseRow;
extern G__linked_taginfo G__G__MatrixLN_TMatrixDSparseDiag_const;
extern G__linked_taginfo G__G__MatrixLN_TMatrixDSparseDiag;
extern G__linked_taginfo G__G__MatrixLN_TMatrixF;
extern G__linked_taginfo G__G__MatrixLN_TMatrixDLazy;
extern G__linked_taginfo G__G__MatrixLN_TMatrixDSymLazy;
extern G__linked_taginfo G__G__MatrixLN_TMatrixFSym;
extern G__linked_taginfo G__G__MatrixLN_TVectorDcLcLdA;
extern G__linked_taginfo G__G__MatrixLN_TDecompBase;
extern G__linked_taginfo G__G__MatrixLN_TDecompBasecLcLEMatrixDecompStat;
extern G__linked_taginfo G__G__MatrixLN_TDecompBasecLcLdA;
extern G__linked_taginfo G__G__MatrixLN_TDecompBK;
extern G__linked_taginfo G__G__MatrixLN_THaarMatrixD;
extern G__linked_taginfo G__G__MatrixLN_THilbertMatrixD;
extern G__linked_taginfo G__G__MatrixLN_THilbertMatrixDSym;
extern G__linked_taginfo G__G__MatrixLN_TElementActionF;
extern G__linked_taginfo G__G__MatrixLN_TElementPosActionF;
extern G__linked_taginfo G__G__MatrixLN_TMatrixFBasecLcLdA;
extern G__linked_taginfo G__G__MatrixLN_TMatrixFBasecLcLEMatrixStatusBits;
extern G__linked_taginfo G__G__MatrixLN_TMatrixFBasecLcLEMatrixCreatorsOp1;
extern G__linked_taginfo G__G__MatrixLN_TMatrixFBasecLcLEMatrixCreatorsOp2;
extern G__linked_taginfo G__G__MatrixLN_TArrayD;
extern G__linked_taginfo G__G__MatrixLN_TArrayI;
extern G__linked_taginfo G__G__MatrixLN_TDecompSparse;
extern G__linked_taginfo G__G__MatrixLN_TMatrixFRow_const;
extern G__linked_taginfo G__G__MatrixLN_TMatrixFRow;
extern G__linked_taginfo G__G__MatrixLN_TMatrixFColumn_const;
extern G__linked_taginfo G__G__MatrixLN_TMatrixFColumn;
extern G__linked_taginfo G__G__MatrixLN_TMatrixFDiag_const;
extern G__linked_taginfo G__G__MatrixLN_TMatrixFDiag;
extern G__linked_taginfo G__G__MatrixLN_TMatrixFFlat_const;
extern G__linked_taginfo G__G__MatrixLN_TMatrixFFlat;
extern G__linked_taginfo G__G__MatrixLN_TMatrixFSub_const;
extern G__linked_taginfo G__G__MatrixLN_TMatrixFSub_constcLcLdA;
extern G__linked_taginfo G__G__MatrixLN_TMatrixFSub;
extern G__linked_taginfo G__G__MatrixLN_TMatrixRow;
extern G__linked_taginfo G__G__MatrixLN_TMatrixColumn;
extern G__linked_taginfo G__G__MatrixLN_TMatrixDiag;
extern G__linked_taginfo G__G__MatrixLN_TMatrixFlat;
extern G__linked_taginfo G__G__MatrixLN_TMatrixFLazy;
extern G__linked_taginfo G__G__MatrixLN_TMatrix;
extern G__linked_taginfo G__G__MatrixLN_TDecompQRH;
extern G__linked_taginfo G__G__MatrixLN_TDecompQRHcLcLdA;
extern G__linked_taginfo G__G__MatrixLN_TDecompLU;
extern G__linked_taginfo G__G__MatrixLN_TMatrixDEigen;
extern G__linked_taginfo G__G__MatrixLN_TMatrixDEigencLcLdA;
extern G__linked_taginfo G__G__MatrixLN_TDecompChol;
extern G__linked_taginfo G__G__MatrixLN_TMatrixFSymLazy;
extern G__linked_taginfo G__G__MatrixLN_TVectorFcLcLdA;
extern G__linked_taginfo G__G__MatrixLN_TVector;
extern G__linked_taginfo G__G__MatrixLN_TDecompSVD;
extern G__linked_taginfo G__G__MatrixLN_TDecompSVDcLcLdA;
extern G__linked_taginfo G__G__MatrixLN_TMatrixDSymEigen;
extern G__linked_taginfo G__G__MatrixLN_TMatrixDSymEigencLcLdA;
extern G__linked_taginfo G__G__MatrixLN_TMatrixLazy;
extern G__linked_taginfo G__G__MatrixLN_THaarMatrixF;
extern G__linked_taginfo G__G__MatrixLN_THilbertMatrixF;
extern G__linked_taginfo G__G__MatrixLN_THilbertMatrixFSym;

/* STUB derived class for protected member access */