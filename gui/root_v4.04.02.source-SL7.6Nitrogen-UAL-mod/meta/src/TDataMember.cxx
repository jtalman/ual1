// @(#)root/meta:$Name:  $:$Id: TDataMember.cxx,v 1.24 2005/03/03 08:04:16 brun Exp $
// Author: Fons Rademakers   04/02/95

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//
//  TDataMember.
//
// All ROOT classes may have RTTI (run time type identification) support
// added. The data is stored in so called DICTIONARY (look at TDictionary).
// Information about a class is stored in TClass.
// This information may be obtained via the CINT api - see class TCint.
// TClass has a list of TDataMember objects providing information about all
// data members of described class.
//Begin_Html
/*
<img align=center src="gif/classinfo.gif">
*/
//End_Html
// TDataMember provides information about name of data member, its type,
// and comment field string. It also tries to find the TMethodCall objects
// responsible for getting/setting a value of it, and gives you pointers
// to these methods. This gives you a unique possibility to access
// protected and private (!) data members if only methods for doing that
// are defined.
// These methods could either be specified in a comment field, or found
// out automatically by ROOT: here's an example:
// suppose you have a class definition:
//Begin_Html <pre>
/*

        class MyClass{
            private:
                Float_t fX1;
                    ...
            public:
                void    SetX1(Float_t x) {fX1 = x;};
                Float_t GetX1()          {return fX1;};
                    ...
        }

*/
//</pre>
//End_Html
// Look at the data member name and method names: a data member name has
// a prefix letter (f) and has a base name X1 . The methods for getting and
// setting this value have names which consist of string Get/Set and the
// same base name. This convention of naming data fields and methods which
// access them allows TDataMember find this methods by itself completely
// automatically. To make this description complete, one should know,
// that names that are automatically recognized may be also:
// for data fields: either fXXX or fIsXXX; and for getter function
// GetXXX() or IsXXX() [where XXX is base name].
//
// As an example of using it let's analyse a few lines which get and set
// a fEditable field in TCanvas:
//Begin_Html <pre>
/*

    TCanvas     *c  = new TCanvas("c");   // create a canvas
    TClass      *cl = c-&gt;IsA();            // get its class description object.

    TDataMember *dm = cl-&gt;GetDataMember("fEditable"); //This is our data member

    TMethodCall *getter = dm-&gt;GetterMethod(c); //find a method that gets value!
    Long_t l;   // declare a storage for this value;

    getter-&gt;Execute(c,"",l);  // Get this Value !!!! It will appear in l !!!


    TMethodCall *setter = dm-&gt;SetterMethod(c);
    setter-&gt;Execute(c,"0",);   // Set Value 0 !!!

*/
//</pre>
//End_Html
//
// This trick is widely used in ROOT TContextMenu and dialogs for obtaining
// current values and put them as initial values in dialog fields.
//
// If you don't want to follow the convention of naming used by ROOT
// you still could benefit from Getter/Setter method support: the solution
// is to instruct ROOT what the names of these routines are.
// The way to do it is putting this information in a comment string to a data
// field in your class declaration:
//
//Begin_Html <pre>
/*

    class MyClass{
        Int_t mydata;  // <em> *OPTIONS={GetMethod="Get";SetMethod="Set"} </em>
         ...
        Int_t Get() const { return mydata;};
        void  Set(Int_t i) {mydata=i;};
        }
*/
//</pre>
//End_Html
//
// However, this getting/setting functions are not the only feature of
// this class. The next point is providing lists of possible settings
// for the concerned data member. The idea is to have a list of possible
// options for this data member, with strings identifying them. This
// is used in dialogs with parameters to set - for details see
// TMethodArg, TRootContextMenu, TContextMenu. This list not only specifies
// the allowed value, but also provides strings naming the options.
// Options are managed via TList of TOptionListItem objects. This list
// is also  created automatically: if a data type is an enum type,
// the list will have items describing every enum value, and named
// according to enum name. If type is Bool_t, two options "On" and "Off"
// with values 0 and 1 are created. For other types you need to instruct
// ROOT about possible options. The way to do it is the same as in case of
// specifying getter/setter method: a comment string to a data field in
// Your header file with class definition.
// The most general format of this string is:
//Begin_Html <pre>
/*

<em>*OPTIONS={GetMethod="</em>getter<em>";SetMethod="</em>setter<em>";Items=(</em>it1<em>="</em>title1<em>",</em>it2<em>="</em>title2<em>", ... ) } </em>

*/
//</pre>
//End_Html
//
// While parsing this string ROOT firstly looks for command-tokens:
// GetMethod, SetMethod, Items; They must be preceded by string
// *OPTIONS= , enclosed by {} and separated by semicolons ";".
// All command token should have a form TOKEN=VALUE.
// All tokens are optional.
// The names of getter and setter method must be enclosed by double-quote
// marks (") .
// Specifications of Items is slightly more complicated: you need to
// put token ITEMS= and then enclose all options in curly brackets "()".
// You separate options by comas ",".
// Each option item may have one of the following forms:
//Begin_Html <pre>
/*
         IntegerValue<em>  = "</em>Text Label<em>"</em>

         EnumValue   <em>  = "</em>Text Label<em>"</em>

        <em>"</em>TextValue<em>" = </em>Text Label<em>"</em>

*/
//</pre>
//End_Html
//
// One can sepcify values as Integers or Enums - when data field is an
// Integer, Float or Enum type; as texts - for Text_t (more precisely:
// Option_t).
//
// As mentioned above - this information are mainly used by contextmenu,
// but also in Dump() and Inspect() methods and by the THtml class.
//
//////////////////////////////////////////////////////////////////////////

#include "TDataMember.h"
#include "TDataType.h"
#include "TROOT.h"
#include "TGlobal.h"
#include "TInterpreter.h"
#include "Strlen.h"
#include "Api.h"
#include "TMethodCall.h"
#include "TClass.h"
#include "TClassEdit.h"
#include "TMethod.h"
#include "TIterator.h"
#include "TList.h"
#include "TGlobal.h"
#include "TRealData.h"


ClassImp(TDataMember)

//______________________________________________________________________________
TDataMember::TDataMember(G__DataMemberInfo *info, TClass *cl) : TDictionary()
{
   // Default TDataMember ctor. TDataMembers are constructed in TClass
   // via a call to TCint::CreateListOfDataMembers(). It parses the comment
   // string, initializes optionlist and getter/setter methods.

   fInfo        = info;
   fClass       = cl;
   fDataType    = 0;
   fOptions     = 0;
   fValueSetter = 0;
   fValueGetter = 0;
   fOffset      = -1;
   fProperty    = -1;
   fSTLCont     = -1;
   if (!fInfo && !fClass) return; // default ctor is called

   if (fInfo) {
      fFullTypeName = fInfo->Type()->Name();
      fTrueTypeName = fInfo->Type()->TrueName();
      fTypeName     = gInterpreter->TypeName(fFullTypeName);
      SetName(fInfo->Name());
      const char *t = fInfo->Title();
      SetTitle(t);
      if (t && t[0] != '!') SetBit(kObjIsPersistent);
      fDataType = 0;
      if (IsBasic() || IsEnum()) {
         if (IsBasic()) {
            const char *name = GetFullTypeName();
            if (strcmp(name, "unsigned char") != 0 &&
                strncmp(name, "unsigned short", sizeof ("unsigned short")) != 0 &&
                strcmp(name, "unsigned int") != 0 &&
                strncmp(name, "unsigned long", sizeof ("unsigned long")) != 0)
                // strncmp() also covers "unsigned long long"
               name = GetTypeName();
            fDataType = gROOT->GetType(name);

            if (fDataType==0) {
               // humm we did not find it ... maybe it's a typedef that has not been loaded yet.
               // (this can happen if the executable does not have a TApplication object).
               fDataType = gROOT->GetType(name,kTRUE);
            }
         } else {
            fDataType = gROOT->GetType("Int_t");
         }
//         if (!fDataType)
//            Error("TDataMember", "basic data type %s not found in list of basic types",
//                  GetTypeName());
      }
   }

   // If option string exist in comment - we'll parse it and create
   // list of options

   // Option-list string has a form:
   // *OPTION={GetMethod="GetXXX";SetMethod="SetXXX";
   //          Items=(0="NULL ITEM","one"="First Item",kRed="Red Item")}
   //
   // As one can see it is possible to specify value as either numerical
   // value , string  or enum.
   // One can also specify implicitly names of Getter/Setter methods.

   char cmt[2048];
   char opt[2048];
   char *opt_ptr = 0;
   char *ptr1    = 0;
   char *ptr2    = 0;
   char *ptr3    = 0;
   char *tok     = 0;
   Int_t cnt     = 0;
   Int_t token_cnt;
   Int_t i;

   strcpy(cmt,fInfo->Title());

   if ((opt_ptr=strstr(cmt,"*OPTION={"))) {

      // If we found it - parsing...

      //let's cut the part lying between {}
      ptr1 = strtok(opt_ptr  ,"{}");  //starts tokenizing:extracts "*OPTION={"
      ptr1 = strtok((char*)0,"{}");   //And now we have what we need in ptr1!!!

      //and save it:
      strcpy(opt,ptr1);

      // Let's extract sub-tokens extracted by ';' sign.
      // We'll put'em in an array for convenience;
      // You have to do it in this manner because you cannot use nested 'strtok'

      char *tokens[256];           // a storage for these sub-tokens.
      token_cnt = 0;
      cnt       = 0;

      do {                          //tokenizing loop
         ptr1=strtok((char*) (cnt++ ? 0:opt),";");
         if (ptr1){
            tok=new char[strlen(ptr1)+1];
            strcpy(tok,ptr1);
            tokens[token_cnt]=tok;
            token_cnt++;
         }
      } while (ptr1);

      // OK! Now let's check whether we have Get/Set methods encode in any string
      for (i=0;i<token_cnt;i++) {

         if (strstr(tokens[i],"GetMethod")) {
            ptr1 = strtok(tokens[i],"\"");    //tokenizing-strip text "GetMethod"
            ptr1 = strtok(0,"\"");         //tokenizing - name is in ptr1!

            if (GetClass()->GetMethod(ptr1,"")) // check whether such method exists
                // FIXME: wrong in case called derives via multiple inheritance from this class
                fValueGetter = new TMethodCall(GetClass(),ptr1,"");

            continue; //next item!
         }

         if (strstr(tokens[i],"SetMethod")) {
            ptr1 = strtok(tokens[i],"\"");
            ptr1 = strtok((char*)0,"\"");    //name of Setter in ptr1
            if (GetClass()->GetMethod(ptr1,"1"))
                // FIXME: wrong in case called derives via multiple inheritance from this class
                fValueSetter = new TMethodCall(GetClass(),ptr1,"1");
         }
      }

      //Now let's parse option strings...

      Int_t  opt_cnt    = 0;
      TList *optionlist = new TList();       //storage for options strings

      for (i=0;i<token_cnt;i++) {
         if (strstr(tokens[i],"Items")) {
            ptr1 = strtok(tokens[i],"()");
            ptr1 = strtok((char*)0,"()");

            char opt[2048];  //and save it!
            strcpy(opt,ptr1);

            //now parse it...
            //fistly we just store strings like: xxx="Label Name"
            //We'll store it in TOptionListItem objects, because they're derived
            //from TObject and thus can be stored in TList.
            //It's not elegant but works.

            do {
               ptr1 = strtok(opt_cnt++ ? (char*)0:opt,","); //options extraction
               if (ptr1) {
                  TOptionListItem *it = new TOptionListItem(this,1,0,0,ptr1,"");
                  optionlist->Add(it);
               }
            } while(ptr1);

         }
      }

      //having all options extracted and put into list, we finally can parse
      //them to create a list of options...

      fOptions = new TList();                //create the list

      TIter next(optionlist);                //we'll iterate through all
                                             //strings containing options
      TOptionListItem *it  = 0;
      TOptionListItem *it1 = 0;
      while ((it=(TOptionListItem*)next())) {

         ptr1 = it->fOptName;
         Bool_t islabel = (ptr1[0]=='\"');   // value is label or numerical?
         ptr2 = strtok(ptr1,"=\"");          //extract LeftHandeSide
         ptr3 = strtok(0,"=\"");             //extract RightHandedSize

         if (islabel) {
            it1=new TOptionListItem(this,-9999,0,0,ptr3,ptr2);
            fOptions->Add(it1);
         }  else {
            //We'll try to find global enum existing in ROOT...
            Long_t l=0;
            Int_t  *value;
            TGlobal *enumval = gROOT->GetGlobal(ptr1,kTRUE);
            if (enumval){
                value = (Int_t*)(enumval->GetAddress());
                l     = (Long_t)(*value);
            } else if (IsEnum()) {
               TObject *obj = fClass->GetListOfDataMembers()->FindObject(ptr1);
               if (obj) l = gROOT->ProcessLineFast(Form("%s::%s;",fClass->GetName(),ptr1));
            } else l = atol(ptr1);

            it1 = new TOptionListItem(this,l,0,0,ptr3,ptr1);
            fOptions->Add(it1);
         }

         optionlist->Remove(it);         //delete this option string from list
         delete it;                      // and dispose of it.

      }

      // Garbage colletion

      // dispose of temporary option list...
      delete optionlist;

      //And dispose tokens string...
      for (i=0;i<token_cnt;i++) if(tokens[i]) delete tokens[i];

   // if option string does not exist but it's an Enum - parse it!!!!
   //} else if (!strncmp(GetFullTypeName(),"enum",4)) {
   } else if (IsEnum()) {

      TGlobal *global = 0;
      TDataMember::fOptions = new TList();
      char etypename[65];
      strncpy(etypename,this->GetTypeName(),64); //save the typename!!! must do it!

      const char *gtypename = 0;
      TList *globals = (TList*)(gROOT->GetListOfGlobals(kTRUE)); //get all globals
      if (!globals) return;

      TIter nextglobal(globals);                //iterate through all global to find
      while ((global=(TGlobal*)nextglobal())) { // values belonging to this enum type
         gtypename = global->GetTypeName();
         if (strcmp(gtypename,etypename)==0) {
            Int_t *value = (Int_t*)(global->GetAddress());
            Long_t l     = (Long_t)(*value);
            char enumItem[128];
            strcpy(enumItem,global->GetName());
            TOptionListItem *it = new TOptionListItem(this,l,0,0,enumItem,enumItem);
            fOptions->Add(it);
         }
      }

   // and the case od Bool_t : we add items "ON" and "Off"
   } else if (!strncmp(GetFullTypeName(),"Bool_t",6)){

      fOptions = new TList();
      TOptionListItem *it = new TOptionListItem(this,1,0,0,"ON",0);
      fOptions->Add(it);
      it = new TOptionListItem(this,0,0,0,"Off",0);
      fOptions->Add(it);

   } else fOptions = 0;

}

//______________________________________________________________________________
TDataMember::~TDataMember()
{
   // TDataMember dtor deletes adopted G__DataMemberInfo object.

   delete fInfo;
   delete fValueSetter;
   delete fValueGetter;
   if (fOptions) {
      fOptions->Delete();
      delete fOptions;
   }
}

//______________________________________________________________________________
Int_t TDataMember::GetArrayDim() const
{
   // Return number of array dimensions.

   return fInfo->ArrayDim();
}

//______________________________________________________________________________
const char *TDataMember::GetArrayIndex() const
{
  // If the data member is pointer and has a valid array size in its comments
  // GetArrayIndex returns a string pointing to it;
  // otherwise it returns an empty string.

  const char* val = fInfo->ValidArrayIndex();
  return (val && IsaPointer() ) ? val : "";
}

//______________________________________________________________________________
Int_t TDataMember::GetMaxIndex(Int_t dim) const
{
   // Return maximum index for array dimension "dim".

   return fInfo->MaxIndex(dim);
}

//______________________________________________________________________________
const char *TDataMember::GetTypeName() const
{
   // Get type of data member, e,g.: "class TDirectory*" -> "TDirectory".

   if (fProperty==(-1)) Property();
   return fTypeName.Data();
}

//______________________________________________________________________________
const char *TDataMember::GetFullTypeName() const
{
   // Get full type description of data member, e,g.: "class TDirectory*".
   if (fProperty==(-1)) Property();

   return fFullTypeName.Data();
}

//______________________________________________________________________________
const char *TDataMember::GetTrueTypeName() const
{
   // Get full type description of data member, e,g.: "class TDirectory*".

   return fTrueTypeName.Data();
}

//______________________________________________________________________________
Int_t TDataMember::GetOffset() const
{
   // Get offset from "this".

   if (fOffset>=0) return fOffset;

   //case of an interpreted or emulated class
   if (fClass->GetDeclFileLine() < 0) {
     ((TDataMember*)this)->fOffset = fInfo->Offset();
     return fOffset;
   }
   //case of a compiled class
   //Note that the offset cannot be computed in case of an abstract class
   //for which the list of real data has not yet been computed via
   //a real daughter class.
   char dmbracket[256];
   sprintf(dmbracket,"%s[",GetName());
   fClass->BuildRealData();
   TIter next(fClass->GetListOfRealData());
   TRealData *rdm;
   Int_t offset = 0;
   while ((rdm = (TRealData*)next())) {
      char *rdmc = (char*)rdm->GetName();
      //next statement required in case a class and one of its parent class
      //have data members with the same name
      if (this->IsaPointer() && rdmc[0] == '*') rdmc++;

      if (rdm->GetDataMember() != this) continue;
      if (strcmp(rdmc,GetName()) == 0) {
         offset = rdm->GetThisOffset();
         break;
      }
      if (strcmp(rdm->GetName(),GetName()) == 0) {
         if (rdm->IsObject()) {
            offset = rdm->GetThisOffset();
            break;
         }
      }
      if (strstr(rdm->GetName(),dmbracket)) {
         offset = rdm->GetThisOffset();
         break;
      }
   }
   ((TDataMember*)this)->fOffset = offset;
   return fOffset;
}

//______________________________________________________________________________
Int_t TDataMember::GetOffsetCint() const
{
   // Get offset from "this" using the information in CINT only.

   return fInfo->Offset();
}

//______________________________________________________________________________
Int_t TDataMember::GetUnitSize() const
{
   if (IsaPointer()) return sizeof(void*);
   if (IsEnum()    ) return sizeof(Int_t);
   if (IsBasic()   ) return GetDataType()->Size();

   TClass *cl = gROOT->GetClass(GetTypeName());
   if (!cl) cl = gROOT->GetClass(GetTrueTypeName());
   if ( cl) return cl->Size();

   Warning("GetUnitSize","Can not determine sizeof(%s)",GetTypeName());
   return 0;
}

//______________________________________________________________________________
Bool_t TDataMember::IsBasic() const
{
   // Return true if data member is a basic type, e.g. char, int, long...

   if (fProperty == -1) Property();
   return (fProperty & kIsFundamental) ? kTRUE : kFALSE;
}

//______________________________________________________________________________
Bool_t TDataMember::IsEnum() const
{
   // Return true if data member is an enum.

   if (fProperty == -1) Property();
   return (fProperty & kIsEnum) ? kTRUE : kFALSE;
}

//______________________________________________________________________________
Bool_t TDataMember::IsaPointer() const
{
   // Return true if data member is a pointer.

   if (fProperty == -1) Property();
   return (fProperty & kIsPointer) ? kTRUE : kFALSE;
}

//______________________________________________________________________________
int TDataMember::IsSTLContainer()
{
   // The return type is defined in TDictionary (kVector, kList, etc.)

   if (fSTLCont != -1) return fSTLCont;
   fSTLCont = abs(TClassEdit::IsSTLCont(GetTrueTypeName()));
   return fSTLCont;
}

//______________________________________________________________________________
Long_t TDataMember::Property() const
{
   // Get property description word. For meaning of bits see EProperty.

   if (fProperty!=(-1)) return fProperty;

   TDataMember *t = (TDataMember*)this;
   if (!fInfo) return 0;
   t->fProperty = fInfo->Property()|fInfo->Type()->Property();
   t->fTypeName = gInterpreter->TypeName(fInfo->Type()->Name());
   t->fFullTypeName = fInfo->Type()->Name();
   t->fName = fInfo->Name();
   t->fTitle = fInfo->Title();

   return fProperty;
}

//______________________________________________________________________________
TList *TDataMember::GetOptions() const
{
   // Returns list of options - list of TOptionListItems

   return fOptions;
}

//______________________________________________________________________________
TMethodCall *TDataMember::GetterMethod(TClass *cl)
{
   // Return a TMethodCall method responsible for getting the value
   // of data member. The cl argument specifies the class of the object
   // which will be used to call this method (in case of multiple
   // inheritance TMethodCall needs to know this to calculate the proper
   // offset).

   if (!fValueGetter || cl) {

      if (!cl) cl = fClass;

      if (fValueGetter) {
         TString methodname = fValueGetter->GetMethodName();
         delete fValueGetter;
         fValueGetter = new TMethodCall(cl, methodname.Data(), "");

      } else { 
         // try to guess Getter function:
         // we strip the fist character of name of data field ('f') and then
         // try to find the name of Getter by applying "Get", "Is" or "Has"
         // as a prefix

         const char *dataname = GetName();

         char gettername[128];
         sprintf(gettername, "Get%s", dataname+1);
         if (GetClass()->GetMethod(gettername, ""))
            return fValueGetter = new TMethodCall(cl, gettername, "");
         sprintf(gettername, "Is%s", dataname+1);
         if (GetClass()->GetMethod(gettername, ""))
            return fValueGetter = new TMethodCall(cl, gettername, "");
         sprintf(gettername, "Has%s", dataname+1);
         if (GetClass()->GetMethod(gettername, ""))
            return fValueGetter = new TMethodCall(cl, gettername, "");
      }
   }

   return fValueGetter;
}

//______________________________________________________________________________
TMethodCall *TDataMember::SetterMethod(TClass *cl)
{
   // Return a TMethodCall method responsible for setting the value
   // of data member. The cl argument specifies the class of the object
   // which will be used to call this method (in case of multiple
   // inheritance TMethodCall needs to know this to calculate the proper
   // offset).

   if (!fValueSetter || cl) {

      if (!cl) cl = fClass;

      if (fValueSetter) {
      
         TString methodname = fValueSetter->GetMethodName();
         TString params = fValueSetter->GetParams();
         delete fValueSetter;
         fValueSetter = new TMethodCall(cl, methodname.Data(), params.Data());
      
      } else {
         
         // try to guess Setter function:
         // we strip the fist character of name of data field ('f') and then
         // try to find the name of Setter by applying "Set" as a prefix

         const char *dataname = GetName();

         char settername[64];
         sprintf(settername, "Set%s", dataname+1);
         if (strstr(settername, "Is")) sprintf(settername, "Set%s", dataname+3);
         if (GetClass()->GetMethod(settername, "1"))
            fValueSetter = new TMethodCall(cl, settername, "1");
         if (!fValueSetter)
            if (GetClass()->GetMethod(settername, "true"))
               fValueSetter = new TMethodCall(cl, settername, "true");
      }
   }

   return fValueSetter;
}

//______________________________________________________________________________
TOptionListItem::TOptionListItem(TDataMember *d, Long_t val, Long_t valmask,
                 Long_t tglmask,const char *name, const char *label)
{
   fDataMember    = d;
   fValue         = val;
   fValueMaskBit  = valmask;
   fToggleMaskBit = tglmask;
   if (name){

      fOptName = new char[strlen(name)+1];
      strcpy(fOptName,name);

   } else fOptName = 0;

   if(label){

      fOptLabel = new char[strlen(label)+1];
      strcpy(fOptLabel,label);

   } else fOptLabel = 0;
}

//______________________________________________________________________________
TOptionListItem::~TOptionListItem()
{
   if (fOptName)  delete [] fOptName;
   if (fOptLabel) delete [] fOptLabel;
}
