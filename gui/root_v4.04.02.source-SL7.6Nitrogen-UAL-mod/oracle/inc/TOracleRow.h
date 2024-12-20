// @(#)root/physics:$Name:  $:$Id: TOracleRow.h,v 1.2 2005/04/25 17:21:11 rdm Exp $
// Author: Yan Liu and Shaowen Wang   23/11/04

/*************************************************************************
 * Copyright (C) 1995-2005, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TOracleRow
#define ROOT_TOracleRow

#ifndef ROOT_TSQLRow
#include "TSQLRow.h"
#endif

#if !defined(__CINT__)
#ifndef R__WIN32
#include <sys/time.h>
#endif
#include <occi.h>
using namespace oracle::occi;
#else
class ResultSet;
class MetaData;
#endif

class TOracleRow : public TSQLRow {

private:
   ResultSet                *fResult;      // current result set
   std::vector<MetaData>    *fFieldInfo;   // metadata for columns
   UInt_t                    fFieldCount;
   std::vector<std::string> *fFields;
   UInt_t                    fUpdateCount; // for dml queries
   Int_t                     fResultType;  // 0 - Update(dml); 1 - Select; -1 - empty

   Bool_t  IsValid(Int_t field);

protected:
   int         GetRowData();
   int         GetRowData2();

public:
   TOracleRow(ResultSet *rs, std::vector<MetaData> *fieldMetaData);
   TOracleRow(UInt_t updateCount);
   ~TOracleRow();

   void        Close(Option_t *opt="");
   ULong_t     GetFieldLength(Int_t field);
   const char *GetField(Int_t field);

   ClassDef(TOracleRow,0)  // One row of Oracle query result
};

#endif
