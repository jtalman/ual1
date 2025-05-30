// @(#)root/star:$Name:  $:$Id: TTableDescriptor.cxx,v 1.8 2003/05/28 15:17:03 brun Exp $
// Author: Valery Fine   09/08/99  (E-mail: fine@bnl.gov)
// $Id: TTableDescriptor.cxx,v 1.8 2003/05/28 15:17:03 brun Exp $
#include <stdlib.h>

#include "TROOT.h"
#include "TTableDescriptor.h"
#include "TTable.h"
#include "TClass.h"
#include "TDataMember.h"
#include "TDataType.h"
#include "Ttypes.h"
#include "TInterpreter.h"

TTableDescriptor *TTableDescriptor::fgColDescriptors = 0;
// TString TTableDescriptor::fCommentsName = TTableDescriptor::SetCommentsSetName();
TString TTableDescriptor::fCommentsName = ".comments";
TableClassImp(TTableDescriptor,tableDescriptor_st)

//______________________________________________________________________________
void TTableDescriptor::Streamer(TBuffer &R__b)
{
  // The custom Streamer for this table
  fSecondDescriptor = 0;
  TTable::Streamer(R__b);
}

//______________________________________________________________________________
TTableDescriptor::TTableDescriptor(const TTable *parentTable)
 : TTable("tableDescriptor",sizeof(tableDescriptor_st)), fRowClass(0),fSecondDescriptor(0)
{
  if (parentTable) {
     TClass *classPtr = parentTable->GetRowClass();
     Init(classPtr);
  }
  else MakeZombie();
}

//______________________________________________________________________________
TTableDescriptor::TTableDescriptor(TClass *classPtr)
 : TTable("tableDescriptor",sizeof(tableDescriptor_st)),fRowClass(0),fSecondDescriptor(0)
{
  // Create a descriptor of the C-structure defined by TClass
  // TClass *classPtr must be a valid pointer to TClass object for
  // "plain" C_struture only !!!
  Init(classPtr);
}
//______________________________________________________________________________
TTableDescriptor::~TTableDescriptor()
{
#ifdef NORESTRICTIONS
  if (!IsZombie()) {
    for (Int_t i=0;i<GetNRows();i++) {
      Char_t *name = (Char_t *)ColumnName(i);
      if (name) delete [] name;
      UInt_t  *indxArray = (UInt_t *)IndexArray(i);
      if (indxArray) delete [] indxArray;
    }
  }
#endif
  if (fSecondDescriptor != this) {
    delete fSecondDescriptor;
    fSecondDescriptor = 0;
  }
}

//____________________________________________________________________________
Int_t TTableDescriptor::AddAt(const void *c)
{ 
   if (!c) return -1;
   TDataSet *cmnt = MakeCommentField();
   assert(cmnt!=0);
   tableDescriptor_st *element = (tableDescriptor_st *)c;
   TDataSet *comment = new TDataSet(element->fColumnName);comment->SetTitle("N/A");
   cmnt->AddAtAndExpand(comment,(Int_t)GetNRows());

   return TTable::AddAt(c); 
}
//____________________________________________________________________________
void  TTableDescriptor::AddAt(const void *c, Int_t i) 
{
   AddAt(*(tableDescriptor_st *)c,"N/A",i);
}
//____________________________________________________________________________
void  TTableDescriptor::AddAt(TDataSet *dataset,Int_t idx){
   TTable::AddAt(dataset,idx);
}
//____________________________________________________________________________
void TTableDescriptor::AddAt(const tableDescriptor_st &element,const char *commentText,Int_t indx)
{
   // Add the descriptor element followed by its commentText
   // at the indx-th position of the descriptor (counted from zero)

   TTable::AddAt(&element,indx);
   TDataSet *cmnt = MakeCommentField();
   assert(cmnt!=0);
   TDataSet *comment = new TDataSet(element.fColumnName);
   comment->SetTitle(commentText);
   cmnt->AddAtAndExpand(comment,indx);
}
//____________________________________________________________________________
TString TTableDescriptor::CreateLeafList() const
{
   // Create a list of leaf to be useful for TBranch::TBranch ctor
   const Char_t TypeMapTBranch[]="\0FIISDiisbBC";
   Int_t maxRows = NumberOfColumns();
   TString string;
   for (Int_t i=0;i<maxRows;i++){
      if (i) string += ":";
      UInt_t nDim = Dimensions(i);

      UInt_t totalSize = 1;
      UInt_t k = 0;

      if (nDim) {
         const UInt_t *indx = IndexArray(i);
         if (!indx){
            string = "";
            Error("CreateLeafList()","Can not create leaflist for arrays");
            return string;
         }
         for (k=0;k< nDim; k++) totalSize *= indx[k];
    }
    const Char_t *colName = ColumnName(i);
    if (totalSize > 1) {
       for ( k = 0; k < totalSize; k++) {
          Char_t buf[10];
          sprintf(buf,"_%d",k);
          string += colName;
          string += buf;
          if (k==0) {
             string += "/";
             string += TypeMapTBranch[ColumnType(i)];
          }
          if (k != totalSize -1) string += ":";
       }
    } else {
       string += ColumnName(i);
       string += "/";
       string += TypeMapTBranch[ColumnType(i)];
    }
  }
  return string;
}

//______________________________________________________________________________
void TTableDescriptor::Init(TClass *classPtr)
{
   // Create a descriptor of the C-structure defined by TClass
   // TClass *classPtr must be a valid pointer to TClass object for
   // "plain" C_structure only !!!
   fSecondDescriptor = 0;
   SetType("tableDescriptor");
   if (classPtr) {
      fRowClass = classPtr; // remember my row class
      SetName(classPtr->GetName());
      LearnTable(classPtr);
   }
   else
      MakeZombie();
}
//____________________________________________________________________________
void TTableDescriptor::LearnTable(const TTable *parentTable)
{
   if (!parentTable) {
      MakeZombie();
      return;
   }
   LearnTable(parentTable->GetRowClass());
}

//____________________________________________________________________________
void TTableDescriptor::LearnTable(TClass *classPtr)
{
//
//  LearnTable() creates an array of the descriptors for elements of the row
//
// It creates a descriptor of the C-structure defined by TClass
// TClass *classPtr must be a valid pointer to TClass object for
// "plain" C-structure only !!!
//
//  This is to introduce an artificial restriction demanded by STAR database group
//
//    1. the name may be 31 symbols at most
//    2. the number the dimension is 3 at most
//
//  To lift this restriction one has to provide -DNORESTRICTIONS CPP symbol and
//  recompile code (and debug code NOW!)
//

   if (!classPtr) return;

   if (!(classPtr->GetNdata())) return;

   Char_t *varname;

   tableDescriptor_st elementDescriptor;

   ReAllocate(classPtr->GetListOfDataMembers()->GetSize());
   Int_t columnIndex = 0;
   TIter next(classPtr->GetListOfDataMembers());
   TDataMember *member = 0;
   while ( (member = (TDataMember *) next()) ) {
      memset(&elementDescriptor,0,sizeof(tableDescriptor_st));
      varname = (Char_t *) member->GetName();
#ifdef NORESTRICTIONS
//  This is remove to introduce an artificial restriction demanded by STAR infrastructure group
                                             elementDescriptor.fColumnName = StrDup(varname);
#else
                                             elementDescriptor.fColumnName[0] = '\0';
         strncat(elementDescriptor.fColumnName,varname,sizeof(elementDescriptor.fColumnName)-1);
#endif
    // define index
      if (member->IsaPointer() ) {
         elementDescriptor.fTypeSize = sizeof(void *);
         const char *typeName = member->GetTypeName();
         elementDescriptor.fType = TTable::GetTypeId(typeName);
      } else {
         TDataType *memberType = member->GetDataType();
         assert(memberType!=0);
         elementDescriptor.fTypeSize = memberType->Size();
         elementDescriptor.fType = TTable::GetTypeId(memberType->GetTypeName());
      }
      Int_t globalIndex = 1;
      if (elementDescriptor.fType != kNAN) {
         Int_t dim = 0;
         if ( (dim = member->GetArrayDim()) ) {
                                              elementDescriptor.fDimensions = dim;
#ifdef NORESTRICTIONS
                                              elementDescriptor.fIndexArray = new UInt_t(dim);
#else
             UInt_t maxDim = sizeof(elementDescriptor.fIndexArray)/sizeof(UInt_t *);
             if (UInt_t(dim) > maxDim) {
                Error("LearnTable","Too many dimenstions - %d", dim);
                dim =  maxDim;
             }
#endif
             for( Int_t indx=0; indx < dim; indx++ ){
                                              elementDescriptor.fIndexArray[indx] = member->GetMaxIndex(indx);
                globalIndex *= elementDescriptor.fIndexArray[indx];
             }
         }
      }
      else Error("LearnTable","Wrong data type for <%s> structure",classPtr->GetName());
      elementDescriptor.fSize   =  globalIndex * (elementDescriptor.fTypeSize);
      elementDescriptor.fOffset = member->GetOffset();
      AddAt(elementDescriptor,member->GetTitle(),columnIndex); columnIndex++;
   }
}

//______________________________________________________________________________
TTableDescriptor *TTableDescriptor::MakeDescriptor(const char *structName)
{
	///////////////////////////////////////////////////////////
	//
	// MakeDescriptor(const char *structName) - static method
	//                structName - the name of the C structure 
	//                             to create descriptor of
	// return a new instance of the TTableDescriptor or 0 
	// if the "structName is not present with the dictionary
	//
	///////////////////////////////////////////////////////////
	TTableDescriptor *dsc = 0;
      TClass *cl = gROOT->GetClass(structName, kTRUE);
//    TClass *cl = new TClass(structName,1,0,0);
	assert(cl!=0);
	dsc = new TTableDescriptor(cl);
	return dsc;
}
//______________________________________________________________________________
TDataSet *TTableDescriptor::MakeCommentField(Bool_t createFlag){
   // Instantiate a comment dataset if any
   TDataSet *comments = FindByName(fCommentsName.Data());
   if (!comments && createFlag) 
      comments =  new TDataSet(fCommentsName.Data(),this,kTRUE);
   return comments;
}
//______________________________________________________________________________
Int_t TTableDescriptor::UpdateOffsets(const TTableDescriptor *newDescriptor)
{
  //                  "Schema evolution"
  // Method updates the offsets with a new ones from another descriptor
  // 
   Int_t maxColumns = NumberOfColumns();
   Int_t mismathes = 0;
 
   if (   (UInt_t(maxColumns) == newDescriptor->NumberOfColumns())
      && (memcmp(GetArray(),newDescriptor->GetArray(),sizeof(tableDescriptor_st)*GetNRows()) == 0)
     ) return mismathes; // everything fine for sure !

  // Something wrong here, we have to check things piece by piece
   for (Int_t colCounter=0; colCounter < maxColumns; colCounter++) {
      Int_t colNewIndx = newDescriptor->ColumnByName(ColumnName(colCounter));
      // look for analog
      EColumnType newType = newDescriptor->ColumnType(colNewIndx);
#ifdef __STAR__
      if (newType == kInt)       newType = kLong;
      else if (newType == kUInt) newType = kULong;
#endif
      if (    colNewIndx >=0
         && Dimensions(colCounter) == newDescriptor->Dimensions(colNewIndx)
         && ColumnType(colCounter) == newType
       )  {
         SetOffset(newDescriptor->Offset(colNewIndx),colCounter);
         if (colNewIndx != colCounter) {
            printf("Schema evolution: \t%d column of the \"%s\" table has been moved to %d-th column\n",
            colCounter,ColumnName(colCounter),colNewIndx);
           mismathes++;
         }
       } else {
         printf("Schema evolution: \t%d column \"%s\" of %d type has been lost\n",
         colCounter,ColumnName(colCounter),ColumnType(colCounter));
         printf(" Indx = %d, name = %s \n", colNewIndx, ColumnName(colCounter));
         SetOffset(UInt_t(-1),colCounter);
         mismathes++;
       }
   }
   if (!mismathes && UInt_t(maxColumns) != newDescriptor->NumberOfColumns()) {
      mismathes++;
      printf("Warning: One extra column has been introduced\n");
   }
   return mismathes;
}

//____________________________________________________________________________
Int_t TTableDescriptor::ColumnByName(const Char_t *columnName) const
{
 // Find the column index but the column name
   const tableDescriptor_st *elementDescriptor = ((TTableDescriptor *)this)->GetTable();
   Int_t i = -1;
   if (!elementDescriptor) return i;
   Int_t nRows = GetNRows();
   char *bracket = 0;
   if (nRows) {
      char *name = StrDup(columnName);
      if ((bracket = strchr(name,'[')) )  *bracket = 0;
      for (i=0; i < nRows; i++,elementDescriptor++)
         if (strcmp(name,elementDescriptor->fColumnName) == 0) break;
      delete [] name;
   }
   if (i==nRows) i = -1;
   // Check array
   if (bracket && !Dimensions(i)) {
      i = -1;
      Warning("ColumnByName","%s column contains a scalar value",columnName);
   }
   return i;
}

//____________________________________________________________________________
Int_t TTableDescriptor::Offset(const Char_t *columnName) const
{
  // Return offset of the column defined by "columnName"
  // Take in account index if provided
  // Can not handle multidimensional indeces yet.

   Int_t indx = ColumnByName(columnName);
   Int_t offset = -1;
   if (indx >= 0 ) {
      offset = Offset(indx);
      const char *openBracket = 0;
      if ( (openBracket = strchr(columnName,'['))  )
         offset += atoi(openBracket+1)*TypeSize(indx);
   }
   return offset;
}

//____________________________________________________________________________
Int_t TTableDescriptor::ColumnSize(const Char_t *columnName) const
{
  Int_t indx = ColumnByName(columnName);
  if (indx >= 0 ) indx = ColumnSize(indx);
  return indx;
}

//____________________________________________________________________________
Int_t TTableDescriptor::TypeSize(const Char_t *columnName) const
{
  Int_t indx = ColumnByName(columnName);
  if (indx >= 0 ) indx = TypeSize(indx);
  return indx;
}

//____________________________________________________________________________
Int_t TTableDescriptor::Dimensions(const Char_t *columnName) const
{
  Int_t indx = ColumnByName(columnName);
  if (indx >= 0 ) indx = Dimensions(indx);
  return indx;
}

//____________________________________________________________________________
TTable::EColumnType TTableDescriptor::ColumnType(const Char_t *columnName) const
{
  Int_t indx = ColumnByName(columnName);
  if (indx >= 0 ) indx = ColumnType(indx);
  return EColumnType(indx);
}
//____________________________________________________________________________
Int_t   TTableDescriptor::Sizeof() const 
{ 
   Int_t fullRowSize = 0;
   if (RowClass() ) fullRowSize = RowClass()->Size();
   else {
     // Calculate the size myslef.
     Int_t iLastRows = GetNRows()-1;
     if (iLastRows >=0) fullRowSize = Offset(iLastRows)  + ColumnSize(iLastRows);
   }
   return fullRowSize;
}

