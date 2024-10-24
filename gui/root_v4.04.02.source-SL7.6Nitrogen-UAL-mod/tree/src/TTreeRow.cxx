// @(#)root/tree:$Name:  $:$Id: TTreeRow.cxx,v 1.3 2002/01/08 09:59:39 brun Exp $
// Author: Fons Rademakers   30/11/99

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TTreeRow                                                             //
//                                                                      //
// Class defining interface to a row of a TTree query result.           //
// Objects of this class are created by TTreeResult methods.            //
//                                                                      //
// Related classes are TTreeResult.                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TTreeRow.h"
#include "TObjArray.h"
#include "TClass.h"

ClassImp(TTreeRow)

//______________________________________________________________________________
TTreeRow::TTreeRow()
{
   // Single row of a query result.

   fColumnCount = 0;
   fFields      = 0;
   fOriginal    = 0;
   fRow         = 0;
   
}

//______________________________________________________________________________
TTreeRow::TTreeRow(Int_t nfields)
{
   // Single row of a query result.

   fColumnCount = nfields;
   fFields      = 0;
   fOriginal    = 0;
   fRow         = 0;
   
}

//______________________________________________________________________________
TTreeRow::TTreeRow(Int_t nfields, const Int_t *fields, const char *row)
{
   // Single row of a query result.

   fColumnCount = nfields;
   fFields      = 0;
   fOriginal    = 0;
   SetRow(fields,row);
}

//______________________________________________________________________________
TTreeRow::TTreeRow(TSQLRow *original)
{
   // This is a shallow copy of a real row, i.e. it only contains
   // a pointer to the original.

   fFields      = 0;
   fOriginal    = 0;
   fColumnCount = 0;
   fRow         = 0;
   
   if (!original) {
      Error("TTreeRow", "original may not be 0");
      return;
   }
   if (original->IsA() != TTreeRow::Class()) {
      Error("TTreeRow", "original must be a TTreeRow");
      return;
   }

   fOriginal = (TTreeRow*) original;
   fColumnCount = fOriginal->fColumnCount;
}

//______________________________________________________________________________
TTreeRow::~TTreeRow()
{
   // Destroy row object.

   if (fFields)
      Close();
}

//______________________________________________________________________________
void TTreeRow::Close(Option_t *)
{
   // Close row.

   if (fRow)    delete [] fRow;
   if (fFields) delete [] fFields;
   fColumnCount = 0;
   fOriginal = 0;
}

//______________________________________________________________________________
Bool_t TTreeRow::IsValid(Int_t field)
{
   // Check if row is open and field index within range.

   if (!fFields && !fOriginal) {
      Error("IsValid", "row closed");
      return kFALSE;
   }
   if (field < 0 || field >= fColumnCount) {
      Error("IsValid", "field index out of bounds");
      return kFALSE;
   }
   return kTRUE;
}

//______________________________________________________________________________
ULong_t TTreeRow::GetFieldLength(Int_t field)
{
   // Get length in bytes of specified field.

   if (!IsValid(field))
      return 0;

   if (fOriginal)
      return fOriginal->GetFieldLength(field);

   if (field > 0) return fFields[field] - fFields[field-1] -1;
   else           return fFields[0] -1;
}

//______________________________________________________________________________
const char *TTreeRow::GetField(Int_t field) 
{
   // Get specified field from row (0 <= field < GetFieldCount()).

   if (!IsValid(field))
      return 0;

   if (fOriginal)
      return fOriginal->GetField(field);

   if (field > 0) return fRow +fFields[field-1];
   else           return fRow;
}

//______________________________________________________________________________
void TTreeRow::SetRow(const Int_t *fields, const char *row)
{
   if (!fColumnCount) return;
   if (fFields) delete [] fFields;
   Int_t nch    = fields[fColumnCount-1];
   fFields      = new Int_t[fColumnCount];
   fOriginal    = 0;
   fRow         = new char[nch];
   for (Int_t i=0;i<fColumnCount;i++) fFields[i] = fields[i];
   memcpy(fRow,row,nch);
}

//______________________________________________________________________________
void TTreeRow::Streamer(TBuffer &R__b)
{
   // Stream an object of class TTreeRow.
   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      R__b.ReadVersion(&R__s, &R__c);
      TSQLRow::Streamer(R__b);
      R__b >> fColumnCount;
      fFields = new Int_t[fColumnCount];
      R__b.ReadFastArray(fFields,fColumnCount);
      Int_t nch;
      R__b >> nch;
      fRow = new char[nch];
      R__b.ReadFastArray(fRow,nch);
      R__b.CheckByteCount(R__s, R__c, TTreeRow::IsA());
   } else {
      UInt_t R__c = R__b.WriteVersion(TTreeRow::Class(),kTRUE);
      TSQLRow::Streamer(R__b);
      R__b << fColumnCount;
      R__b.WriteFastArray(fFields,fColumnCount);
      Int_t nch = fFields[fColumnCount-1];
      R__b << nch;
      R__b.WriteFastArray(fRow,nch);
      R__b.SetByteCount(R__c,kTRUE);
   }
}
