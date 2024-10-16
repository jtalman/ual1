// @(#)root/base:$Name:  $:$Id: TQConnection.cxx,v 1.19 2005/03/13 15:05:31 rdm Exp $
// Author: Valeriy Onuchin & Fons Rademakers   15/10/2000

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TQConnection class is an internal class, used in the object          //
// communication mechanism.                                             //
//                                                                      //
// TQConnection:                                                        //
//    -  is a list of signal_lists containing pointers                  //
//       to this connection                                             //
//    -  receiver is the object to which slot-method is applied         //
//                                                                      //
// This implementation is provided by                                   //
// Valeriy Onuchin (onuchin@sirius.ihep.su).                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TQConnection.h"
#include "TRefCnt.h"
#include "TClass.h"
#include "TMethod.h"
#include "TMethodArg.h"
#include "TDataType.h"
#include "Api.h"
#include "G__ci.h"
#include "Riostream.h"
#include "TVirtualMutex.h"
#include "THashTable.h"


ClassImpQ(TQConnection)

char *gTQSlotParams;          // used to pass string parameter

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//  TQSlot = slightly modified TMethodCall class                        //
//           used in the object communication mechanism                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
class TQSlot : public TObject, public TRefCnt {

protected:
   G__CallFunc   *fFunc;      // CINT method invocation environment
   TFunction     *fMethod;    // slot method or global function
   Long_t         fOffset;    // offset added to object pointer
   TString        fName;      // full name of method
   Int_t          fExecuting; // true if one of this slot's ExecuteMethod methods is being called

public:
   TQSlot(TClass *cl, const char *method, const char *funcname);
   TQSlot(const char *class_name, const char *funcname);
   virtual ~TQSlot();

   const char *GetName() const { return fName.Data(); }

   void ExecuteMethod(void *object);
   void ExecuteMethod(void *object, Int_t nargs, va_list ap);
   void ExecuteMethod(void *object, Long_t param);
   void ExecuteMethod(void *object, Long64_t param);
   void ExecuteMethod(void *object, Double_t param);
   void ExecuteMethod(void *object, const char *params);
   void ExecuteMethod(void *object, Long_t *paramArr, Int_t nparam = -1);
   void Print(Option_t *opt= "") const;
   void ls(Option_t *opt= "") const { Print(opt); }

   Bool_t IsExecuting() const { return fExecuting > 0; }
};


//______________________________________________________________________________
TQSlot::TQSlot(TClass *cl, const char *method_name,
               const char *funcname) : TObject(), TRefCnt()
{
   // Create the method invocation environment. Necessary input
   // information: the class, full method name with prototype
   // string of the form: method(char*,int,float).
   // To initialize class method with default arguments, method
   // string with default parameters should be of the form:
   // method(=\"ABC\",1234,3.14) (!! parameter string should
   // consists of '=').
   // To execute the method call TQSlot::ExecuteMethod(object,...).

   fFunc      = 0;
   fOffset    = 0;
   fMethod    = 0;
   fName      = "";
   fExecuting = 0;

   // cl==0, is the case of interpreted function.

   fName = method_name;

   char *method = new char[strlen(method_name)+1];
   if (method) strcpy(method, method_name);

   char *proto;
   char *tmp;
   char *params = 0;

   // separate method and protoype strings

   if ((proto = strchr(method,'('))) {

      // substitute first '(' symbol with '\0'
      *proto++ = '\0';

      // last ')' symbol with '\0'
      if ((tmp = strrchr(proto,')'))) *tmp = '\0';
      if ((params = strchr(proto,'='))) *params = ' ';
   }

   R__LOCKGUARD(gCINTMutex);
   fFunc = new G__CallFunc;

   // initiate class method (function) with proto
   // or with default params

   if (cl) {
      if (params) {
         fFunc->SetFunc(cl->GetClassInfo(), method, params, &fOffset);
         fMethod = cl->GetMethod(method, params);
      } else {
         fFunc->SetFuncProto(cl->GetClassInfo(), method, proto, &fOffset);
         fMethod = cl->GetMethodWithPrototype(method, proto);
      }
   } else {
      G__ClassInfo gcl;
      if (params) {
         fFunc->SetFunc(&gcl, (char*)funcname, params, &fOffset);
         fMethod = gROOT->GetGlobalFunction(funcname, params, kTRUE);
      } else {
         fFunc->SetFuncProto(&gcl, (char*)funcname, proto, &fOffset);
         fMethod = gROOT->GetGlobalFunctionWithPrototype(funcname, proto, kTRUE);
      }
   }

   // cleaning
   delete [] method;
}

//______________________________________________________________________________
TQSlot::TQSlot(const char *class_name, const char *funcname) :
   TObject(), TRefCnt()
{
   // Create the method invocation environment. Necessary input
   // information: the name of class (could be interpreted class),
   // full method name with prototype or parameter string
   // of the form: method(char*,int,float).
   // To initialize class method with default arguments, method
   // string with default parameters  should be of the form:
   // method(=\"ABC\",1234,3.14) (!! parameter string should
   // consists of '=').
   // To execute the method call TQSlot::ExecuteMethod(object,...).

   fFunc      = 0;
   fOffset    = 0;
   fMethod    = 0;
   fName      = funcname;
   fExecuting = 0;

   char *method = new char[strlen(funcname)+1];
   if (method) strcpy(method, funcname);

   char *proto;
   char *tmp;
   char *params = 0;

   // separate method and protoype strings

   if ((proto =  strchr(method,'('))) {
      *proto++ = '\0';
      if ((tmp = strrchr(proto,')'))) *tmp  = '\0';
      if ((params = strchr(proto,'='))) *params = ' ';
   }

   R__LOCKGUARD(gCINTMutex);
   fFunc = new G__CallFunc;

   G__ClassInfo gcl;
   TClass *cl = 0;

   if (!class_name)
      ;                       // function
   else {
      gcl.Init(class_name);   // class
      cl = gROOT->GetClass(class_name);
   }

   if (params) {
      fFunc->SetFunc(&gcl, method, params, &fOffset);
      if (cl)
         fMethod = cl->GetMethod(method, params);
      else
         fMethod = gROOT->GetGlobalFunction(method, params, kTRUE);
   } else {
      fFunc->SetFuncProto(&gcl, method, proto , &fOffset);
      if (cl)
         fMethod = cl->GetMethodWithPrototype(method, proto);
      else
         fMethod = gROOT->GetGlobalFunctionWithPrototype(method, proto, kTRUE);
   }

   delete [] method;
   return;
}

//______________________________________________________________________________
TQSlot::~TQSlot()
{
   // TQSlot dtor.

   // don't delete executing environment of a slot that is being executed
   if (!fExecuting)
      delete fFunc;
}

//______________________________________________________________________________
inline void TQSlot::ExecuteMethod(void *object)
{
   // ExecuteMethod the method (with preset arguments) for
   // the specified object.

   void *address = 0;
   if (object) address = (void*)((Long_t)object + fOffset);
   R__LOCKGUARD(gCINTMutex);
   fExecuting++;
   fFunc->Exec(address);
   fExecuting--;
   if (!TestBit(kNotDeleted) && !fExecuting)
      delete fFunc;
}

//______________________________________________________________________________
inline void TQSlot::ExecuteMethod(void *object, Int_t nargs, va_list ap)
{
   // ExecuteMethod the method for the specified object and
   // with variable argument list.

   if (!fMethod) {
      Error("ExecuteMethod", "method %s not found,"
            "\n(note: interpreted methods are not supported with varargs)",
            fName.Data());
      return;
   }

   if (nargs < fMethod->GetNargs() - fMethod->GetNargsOpt() ||
       nargs > fMethod->GetNargs()) {
      Error("ExecuteMethod", "nargs (%d) not consistent with expected number of arguments ([%d-%d])",
            nargs, fMethod->GetNargs() - fMethod->GetNargsOpt(),
            fMethod->GetNargs());
      return;
   }

   void *address = 0;
   R__LOCKGUARD(gCINTMutex);

   fFunc->ResetArg();

   if (nargs > 0) {
      TIter next(fMethod->GetListOfMethodArgs());
      TMethodArg *arg;

      for (int i = 0; i < nargs; i++) {
         arg = (TMethodArg*) next();
         TString type = arg->GetFullTypeName();
         TDataType *dt = gROOT->GetType(type);
         if (dt)
            type = dt->GetFullTypeName();
         if (arg->Property() & (kIsPointer | kIsArray | kIsReference))
            fFunc->SetArg((Long_t) va_arg(ap, void*));
            //fCallEnv->SetParam((Long_t) va_arg(ap, void*));
         else if (type == "bool")
            fFunc->SetArg((Long_t) va_arg(ap, int));  // bool is promoted to int
            //fCallEnv->SetParam((Long_t) va_arg(ap, int));  // bool is promoted to int
         else if (type == "char" || type == "unsigned char")
            fFunc->SetArg((Long_t) va_arg(ap, int));  // char is promoted to int
            //fCallEnv->SetParam((Long_t) va_arg(ap, int));  // char is promoted to int
         else if (type == "short" || type == "unsigned short")
            fFunc->SetArg((Long_t) va_arg(ap, int));  // short is promoted to int
            //fCallEnv->SetParam((Long_t) va_arg(ap, int));  // short is promoted to int
         else if (type == "int" || type == "unsigned int")
            fFunc->SetArg((Long_t) va_arg(ap, int));
            //fCallEnv->SetParam((Long_t) va_arg(ap, int));
         else if (type == "long" || type == "unsigned long")
            fFunc->SetArg((Long_t) va_arg(ap, long));
            //fCallEnv->SetParam((Long_t) va_arg(ap, long));
         else if (type == "long long")
            fFunc->SetArg((Long64_t) va_arg(ap, Long64_t));
            //fCallEnv->SetParam((Long64_t) va_arg(ap, Long64_t));
         else if (type == "unsigned long long")
            fFunc->SetArg((ULong64_t) va_arg(ap, ULong64_t));
            //fCallEnv->SetParam((ULong64_t) va_arg(ap, ULong64_t));
         else if (type == "float")
            fFunc->SetArg((Double_t) va_arg(ap, double));  // float is promoted to double
            //fCallEnv->SetParam((Double_t) va_arg(ap, double));  // float is promoted to double
         else if (type == "double")
            fFunc->SetArg((Double_t) va_arg(ap, double));
            //fCallEnv->SetParam((Double_t) va_arg(ap, double));
      }
   }

   if (object) address = (void*)((Long_t)object + fOffset);
   fExecuting++;
   fFunc->Exec(address);
   fExecuting--;
   if (!TestBit(kNotDeleted) && !fExecuting)
      delete fFunc;
}

//______________________________________________________________________________
inline void TQSlot::ExecuteMethod(void *object, Long_t param)
{
   // ExecuteMethod the method for the specified object and
   // with single argument value.

   void *address = 0;
   R__LOCKGUARD(gCINTMutex);
   fFunc->ResetArg();
   fFunc->SetArg(param);
   if (object) address = (void*)((Long_t)object + fOffset);
   fExecuting++;
   fFunc->Exec(address);
   fExecuting--;
   if (!TestBit(kNotDeleted) && !fExecuting)
      delete fFunc;
}

//______________________________________________________________________________
inline void TQSlot::ExecuteMethod(void *object, Long64_t param)
{
   // ExecuteMethod the method for the specified object and
   // with single argument value.

   void *address = 0;
   R__LOCKGUARD(gCINTMutex);
   fFunc->ResetArg();
   fFunc->SetArg(param);
   if (object) address = (void*)((Long_t)object + fOffset);
   fExecuting++;
   fFunc->Exec(address);
   fExecuting--;
   if (!TestBit(kNotDeleted) && !fExecuting)
      delete fFunc;
}

//______________________________________________________________________________
inline void TQSlot::ExecuteMethod(void *object, Double_t param)
{
   // ExecuteMethod the method for the specified object and
   // with single argument value.

   void *address = 0;
   R__LOCKGUARD(gCINTMutex);
   fFunc->ResetArg();
   fFunc->SetArg(param);
   if (object) address = (void*)((Long_t)object + fOffset);
   fExecuting++;
   fFunc->Exec(address);
   fExecuting--;
   if (!TestBit(kNotDeleted) && !fExecuting)
      delete fFunc;
}

//______________________________________________________________________________
inline void TQSlot::ExecuteMethod(void *object, const char *param)
{
   // ExecuteMethod the method for the specified object and text param.

   void *address = 0;
   R__LOCKGUARD(gCINTMutex);
   gTQSlotParams = (char*)param;
   fFunc->SetArgs("gTQSlotParams");
   if (object) address = (void*)((Long_t)object + fOffset);
   fExecuting++;
   fFunc->Exec(address);
   fExecuting--;
   if (!TestBit(kNotDeleted) && !fExecuting)
      delete fFunc;
}

//______________________________________________________________________________
inline void TQSlot::ExecuteMethod(void *object, Long_t *paramArr, Int_t nparam)
{
   // ExecuteMethod the method for the specified object and with
   // several argument values.
   // ParamArr is an array containing the function argument values.
   // If nparam = -1 then paramArr must contain values for all function
   // arguments, otherwise Nargs-NargsOpt <= nparam <= Nargs, where
   // Nargs is the number of all arguments and NargsOpt is the number
   // of default arguments.

   void *address = 0;
   R__LOCKGUARD(gCINTMutex);
   fFunc->SetArgArray(paramArr, nparam);
   if (object) address = (void*)((Long_t)object + fOffset);
   fExecuting++;
   fFunc->Exec(address);
   fExecuting--;
   if (!TestBit(kNotDeleted) && !fExecuting)
      delete fFunc;
}

//______________________________________________________________________________
void TQSlot::Print(Option_t *) const
{
   // Print info about slot.

   cout <<IsA()->GetName() << "\t" << GetName() << "\t"
   << "Number of Connections = " << References() << endl;
}

////////////////////////////////////////////////////////////////////////////////

class TQSlotPool : public THashTable {
public:
   TQSlotPool() : THashTable(50) { }
   virtual ~TQSlotPool() { Delete(); }

   TQSlot  *New(const char *class_name, const char *funcname);
   TQSlot  *New(TClass *cl, const char *method, const char *func);
   void     Free(TQSlot *slot);
};

//______________________________________________________________________________
TQSlot *TQSlotPool::New(const char *class_name, const char *funcname)
{
   // Create new slot or return already existing one.

   TString name = class_name;
   name += "::";
   name += funcname;

   TQSlot *slot = (TQSlot*)FindObject(name.Data());

   if (!slot) {
      slot = new TQSlot(class_name, funcname);
      Add(slot);
   }
   slot->AddReference();
   return slot;
}

//______________________________________________________________________________
TQSlot *TQSlotPool::New(TClass *cl, const char *method, const char *func)
{
   // Create new slot or return already existing one.

   TString name;

   if (cl) {
      name = cl->GetName();
      name += "::";
      name += method;
   } else {
      name = "::";
      name += func;
   }

   TQSlot *slot = (TQSlot*)FindObject(name.Data());

   if (!slot) {
      slot = new TQSlot(cl, method, func);
      Add(slot);
   }
   slot->AddReference();
   return slot;
}

//______________________________________________________________________________
void TQSlotPool::Free(TQSlot *slot)
{
   // Delete slot if there is no refernce to it.

   slot->RemoveReference();  // decrease references to slot

   if (slot->References() <= 0) {
      Remove(slot);
      if (!slot->IsExecuting()) SafeDelete(slot);
   }
}

static TQSlotPool gSlotPool;  // global pool of slots

////////////////////////////////////////////////////////////////////////////////

//______________________________________________________________________________
TQConnection::TQConnection() : TList(), TQObject()
{
   // Default constructor.

   fReceiver = 0;
   fSlot     = 0;
}

//______________________________________________________________________________
TQConnection::TQConnection(TClass *cl, void *receiver, const char *method_name)
   : TList(), TQObject()
{
   // TQConnection ctor.
   //    cl != 0  - connection to object == receiver of class == cl
   //               and method == method_name
   //    cl == 0  - connection to function with name == method_name

   char *funcname = 0;
   fReceiver = receiver;      // fReceiver is pointer to receiver

   if (!cl) {
      funcname = G__p2f2funcname(fReceiver);
      if (!funcname)
         Warning("TQConnection", "%s cannot be compiled", method_name);
   }

   if (cl) fClassName = cl->GetName();
   fSlot = gSlotPool.New(cl, method_name, funcname);
}

//______________________________________________________________________________
TQConnection::TQConnection(const char *class_name, void *receiver,
                           const char *funcname) : TList(), TQObject()
{
   // TQConnection ctor.
   //    Creates connection to method of class specified by name,
   //    it could be interpreted class and with method == funcname.

   fClassName = class_name;
   fSlot = gSlotPool.New(class_name, funcname);  // new slot-method
   fReceiver = receiver;      // fReceiver is pointer to receiver
}

//______________________________________________________________________________
TQConnection::TQConnection(const TQConnection &con): TList(), TQObject()
{
   // Copy constructor. Ignore connections to this TQConnections

   fClassName = con.fClassName;
   fSlot = con.fSlot;
   fSlot->AddReference();
   fReceiver = con.fReceiver;
}

//______________________________________________________________________________
TQConnection::~TQConnection()
{
   // TQConnection dtor.
   //    - remove this connection from all signal lists
   //    - we do not delete fSlot if it has other connections,
   //      TQSlot::fCounter > 0 .

   TIter next(this);
   register TList *list;

   while ((list = (TList*)next())) {
      list->Remove(this);
      if (list->IsEmpty()) delete list;   // delete empty list
   }
   Clear("nodelete");

   if (!fSlot) return;
   gSlotPool.Free(fSlot);
}

//______________________________________________________________________________
const char *TQConnection::GetName() const
{
   // Returns name of connection (aka name of slot)

   return fSlot->GetName();
}

//______________________________________________________________________________
void TQConnection::Destroyed()
{
   // Signal Destroyed tells that connection is destroyed.

   MakeZombie();
   Emit("Destroyed()");
}

//______________________________________________________________________________
void TQConnection::ls(Option_t *option) const
{
   // List TQConnection full method name and list all signals
   // connected to this connection.

   cout << "\t" <<  IsA()->GetName() << "\t" << GetName() << endl;
   ((TQConnection*)this)->ForEach(TList,ls)(option);
}

//______________________________________________________________________________
void TQConnection::Print(Option_t *) const
{
   // Print TQConnection full method name and print all
   // signals connected to this connection.

   cout <<  "\t\t\t" << IsA()->GetName() << "\t" << fReceiver <<
            "\t" << GetName() << endl;
}

//______________________________________________________________________________
void TQConnection::ExecuteMethod()
{
   // Apply slot-method to the fReceiver object without arguments.

   // This connection might be deleted in result of the method execution
   // (for example in case of a Disconnect).  Hence we do not assume
   // the object is still valid on return.
   TQSlot *s = fSlot;
   fSlot->ExecuteMethod(fReceiver);
   if (s->References() <= 0) delete s;
}

//______________________________________________________________________________
void TQConnection::ExecuteMethod(Int_t nargs, va_list va)
{
   // Apply slot-method to the fReceiver object with
   // variable argument list.

   // This connection might be deleted in result of the method execution
   // (for example in case of a Disconnect).  Hence we do not assume
   // the object is still valid on return.
   TQSlot *s = fSlot;
   fSlot->ExecuteMethod(fReceiver, nargs, va);
   if (s->References() <= 0) delete s;
}

//______________________________________________________________________________
void TQConnection::ExecuteMethod(Long_t param)
{
   // Apply slot-method to the fReceiver object with
   // single argument value.

   // This connection might be deleted in result of the method execution
   // (for example in case of a Disconnect).  Hence we do not assume
   // the object is still valid on return.
   TQSlot *s = fSlot;
   fSlot->ExecuteMethod(fReceiver, param);
   if (s->References() <= 0) delete s;
}

//______________________________________________________________________________
void TQConnection::ExecuteMethod(Long64_t param)
{
   // Apply slot-method to the fReceiver object with
   // single argument value.

   // This connection might be deleted in result of the method execution
   // (for example in case of a Disconnect).  Hence we do not assume
   // the object is still valid on return.
   TQSlot *s = fSlot;
   fSlot->ExecuteMethod(fReceiver, param);
   if (s->References() <= 0) delete s;
}

//______________________________________________________________________________
void TQConnection::ExecuteMethod(Double_t param)
{
   // Apply slot-method to the fReceiver object with
   // single argument value.

   // This connection might be deleted in result of the method execution
   // (for example in case of a Disconnect).  Hence we do not assume
   // the object is still valid on return.
   TQSlot *s = fSlot;
   fSlot->ExecuteMethod(fReceiver, param);
   if (s->References() <= 0) delete s;
}

//______________________________________________________________________________
void TQConnection::ExecuteMethod(Long_t *params, Int_t nparam)
{
   // Apply slot-method to the fReceiver object with variable
   // number of argument values.

   // This connection might be deleted in result of the method execution
   // (for example in case of a Disconnect).  Hence we do not assume
   // the object is still valid on return.
   TQSlot *s = fSlot;
   fSlot->ExecuteMethod(fReceiver, params, nparam);
   if (s->References() <= 0) delete s;
}

//______________________________________________________________________________
void TQConnection::ExecuteMethod(const char *param)
{
   // Apply slot-method to the fReceiver object and
   // with string parameter.

   // This connection might be deleted in result of the method execution
   // (for example in case of a Disconnect).  Hence we do not assume
   // the object is still valid on return.
   TQSlot *s = fSlot;
   fSlot->ExecuteMethod(fReceiver, param);
   if (s->References() <= 0) delete s;
}
