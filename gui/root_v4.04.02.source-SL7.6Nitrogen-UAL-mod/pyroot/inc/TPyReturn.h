// @(#)root/pyroot:$Name:  $:$Id: TPyReturn.h,v 1.5 2005/04/28 07:33:55 brun Exp $
// Author: Wim Lavrijsen   May 2004

#ifndef ROOT_TPyReturn
#define ROOT_TPyReturn

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// TPyReturn                                                                //
//                                                                          //
// Morphing return type from evaluating python expressions.                 //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


// ROOT
#ifndef ROOT_Rtypes
#include "Rtypes.h"
#endif

// Python
struct _object;
typedef _object PyObject;


class TPyReturn {
public:
   TPyReturn();
   TPyReturn( PyObject* pyobject );
   TPyReturn( const TPyReturn& );
   TPyReturn& operator=( const TPyReturn& );
   virtual ~TPyReturn();

// conversions to standard types, may fail if unconvertible
   operator const char*() const;
   operator Char_t() const;

   operator Long_t() const;
   operator Int_t() const { return (Int_t)operator Long_t(); }
   operator Short_t() const { return (Short_t)operator Long_t(); }

   operator ULong_t() const;
   operator UInt_t() const { return (UInt_t)operator ULong_t(); }
   operator UShort_t() const { return (UShort_t)operator UShort_t(); }

   operator Double_t() const;
   operator Float_t() const { return (Float_t)operator Double_t(); }

// used for both TObject and PyObject conversions
   operator void*() const;

   ClassDef(TPyReturn,1)   //Python morphing return object

private:
   PyObject* fPyObject;            //! actual python object
};

#endif
