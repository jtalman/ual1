// @(#)root/cont:$Name:  $:$Id: TRef.cxx,v 1.27 2005/01/28 05:45:41 brun Exp $
// Author: Rene Brun   28/09/2001

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//
// TRef
//
// Persistent Reference link to a TObject
// A TRef is a lightweight object pointing to any TObject.
// This object can be used instead of normal C++ pointers in case
//  - the referenced object R and the pointer P are not written to the same file
//  - P is read before R
//  - R and P are written to different Tree branches
//
// When a top level object (eg Event *event) is a tree/graph of many objects,
// the normal ROOT Streaming mechanism ensures that only one copy of each object
// in the tree/graph is written to the output buffer to avoid circular
// dependencies.
// However if the object event is split into several files or into several
// branches of one or more Trees, normal C++ pointers cannot be used because
// each I/O operation will write the referenced objects.
// When a TRef is used to point to a TObject *robj.
// For example in a class with
//     TRef  fRef;
// one can do:
//     fRef = robj;  //to set the pointer
// this TRef and robj can be written with two different I/O calls
// in the same or different files, in the same or different branches of a Tree.
// If the TRef is read and the referenced object has not yet been read,
// the TRef will return a null pointer. As soon as the referenced object
// will be read, the TRef will point to it.
//
// TRef also supports the complex situation where a TFile is updated
// multiple times on the same machine or a different machine.
//
// How does it work
// ----------------
// A TRef is itself a TObject with an additional transient pointer fPID.
// When the statement fRef = robj is executed, the following actions happen:
//   - The pointer fPID is set to the current TProcessID.
//   - The current ObjectNumber (see below) is incremented by one.
//   - robj::fUniqueID is set to ObjectNumber.
//   - In the fPID object, the element fObjects[ObjectNumber] is set to robj
//   - ref::fUniqueID is also set to ObjectNumber.
// After having set fRef, one can immediatly return the value of robj
// using fRef.GetObject(). This function returns directly fObjects[fUniqueID]
// from the fPID object.
//
// When the TRef is written, the process id number pidf of fPID is written
// in addition to the TObject part of TRef (fBits,fUniqueID).
// When the TRef is read, its pointer fPID is set to the value
// stored in the TObjArray of TFile::fProcessIDs (fProcessIDs[pidf]).
//
// When a referenced object robj is written, TObject::Streamer writes
// in addition to the standard (fBits,fUniqueID) the pidf.
// When this robj is read by TObject::Streamer, the pidf is read.
// At this point, robj is entered into the table of objects of the TProcessID
// corresponding to pidf.
//
// WARNING: If MyClass is the class of the referenced object, The TObject
//          part of MyClass must be Streamed. One should not
//          call MyClass::Class()->IgnoreTObjectStreamer()
//
// ObjectNumber
// ------------
// When an object is referenced (see TRef assignement operator or TRefArray::Add)
// a unique identifier is computed and stored in both the fUniqueID of the
// referenced and referencing object. This uniqueID is computed by incrementing
// by one the static global in TProcessID::fgNumber. fUniqueID is some sort of
// serial object number in the current session. One can retrieve at any time
// the current value of fgNumber by calling the static function TProcessID::GetObjectCount
// or set this number via TProcessID::SetObjectCount.
// To avoid a growing table of fObjects in TProcessID, in case, for example,
// one processes many events in a loop, it might be necessary to reset the
// ObjectNumber at the end of processing of one event. See an example
// in $ROOTSYS/test/Event.cxx (look at function Build).
// The value of ObjectNumber (say saveNumber=TProcessID::GetObjectCount()) may be
// saved at the beginning of one event and reset to this original value
// at the end of the event via TProcessID::SetObjectCount(saveNumber). These
// actions may be stacked.
//
// Action on Demand
// ----------------
// The normal behaviour of a TRef has been described above. In addition,
// TRef supports also "Actions on Demand". It may happen that the object
// referenced is not yet in memory, on a separate file or not yet computed.
// In this case TRef is able to automatically execute an action:
//   - call to a compiled function (static function of member function)
//   - call to an interpreted function
//   - execution of a CINT script
//
// How to select this option?
// In the definition of the TRef data member in the original class, do:
//   TRef  fRef;   //EXEC:execName. points to something
// When the special keyword "EXEC:" is found in the comment field of the member,
// the next string is assumed to be the name of a TExec object.
// When a file is connected, the dictionary of the classes on the file
// is read in memory (see TFile::ReadStreamerInfo). When the TStreamerElement
// object is read, a TExec object is automatically created with the name
// specified after the keywork "EXEC:" in case a TExec with a same name does
// not already exist.
// The action to be executed via this TExec can be specified with:
//    - a call to the TExec constructor, if the constructor is called before
//      opening the file.
//    - a call to TExec::SetAction at any time.
//      One can compute a pointer to an existing TExec with a name with:
//        TExec *myExec = gROOT->GetExec(execName);
//      myExec->SetAction(actionCommand); where
//      - actionCommand is a string containing a CINT instruction. Examples:
//          myExec->SetAction("LoadHits()");
//          myExec->SetAction(".x script.C");
//
// When a TRef is dereferenced via TRef::GetObject, its TExec will be
// automatically executed. In the function/script being executed, one or more
// of the following actions can be executed:
//  - load a file containing the referenced object. This function typically
//    looks in the file catalog (GRID).
//  - compute a pointer to the referenced object and communicate this pointer
//    back to the calling function TRef::GetObject via:
//      TRef::SetObject(object).
// As soon as an object is returned to GetObject, the fUniqueID of the TRef is set
// to the fUniqueID of the referenced object. At the next call to GetObject,
// the pointer stored in fPid:fObjects[fUniqueID] will be returned directly.
//
// An example of action on demand is shown in $ROOTSYS/test/Event.h with
// the member:
//      TRef    fWebHistogram;   //EXEC:GetWebHistogram
// When calling fWebHistogram.GetObject(), the function GetObject
// will automatically invoke a script GetWebHistogram.C via the interpreter.
// An example of a GetWebHistogram.C script is shown below
//    void GetWebHistogram() {
//       TFile *f= TFile::Open("http://root.cern.ch/files/pippa.root");
//       f->cd("DM/CJ");
//       TH1 *h6 = (TH1*)gDirectory->Get("h6");
//       h6->SetDirectory(0);
//       delete f;
//       TRef::SetObject(h6);
//    }
// In the above example, a call to fWebHistogram.GetObject() executes the
// script with the function GetWebHistogram. This script connects a file
// with histograms: pippa.root on the ROOT Web site and returns the object h6
// to TRef::GetObject.
// Note that if the definition of the TRef fWebHistogram had been:
//      TRef    fWebHistogram;   //EXEC:GetWebHistogram()
// then, the compiled or interpreted function GetWebHistogram() would have
// been called instead of the CINT script GetWebHistogram.C
//
// Special case of a TRef pointing to an object with a TUUID
// ----------------------------------------------------------
// If the referenced object has a TUUID, its bit kHasUUID has been set.
// This case is detected by the TRef assignement operator.
// (For example, TFile and TDirectory have a TUUID)
// The TRef fPID points directly to the single object TProcessUUID (deriving
// from TProcessID) and managing the list of TUUIDs for a process.
// The TRef kHasUUID bit is set and its fUniqueID is set to the fUniqueID
// of the referenced object.
// When the TRef is streamed to a buffer, the corresponding TUUID is also
// streamed with the TRef. When a TRef is read from a buffer, the corresponding
// TUUID is also read and entered into the global list of TUUIDs (if not
// already there). The TRef fUniqueID is set to the UUIDNumber.
// see TProcessUUID for more details.
//
// Array of TRef
// -------------
// The special class TRefArray should be used to store multiple references.
// A TRefArray has one single pointer fPID for all objects in the array.
// It has a dynamic compact table of fUniqueIDs. Use a TRefArray rather
// then a collection of TRefs.
//
// Example:
// Suppose a TObjArray *mytracks containing a list of Track objects
// Suppose a TRefArray *pions containing pointers to the pion tracks in mytracks.
// This list is created with statements like: pions->Add(track);
// Suppose a TRefArray *muons containing pointers to the muon tracks in mytracks.
// The 3 arrays mytracks,pions and muons may be written separately.
//
//////////////////////////////////////////////////////////////////////////

#include "TRef.h"
#include "TROOT.h"
#include "TProcessUUID.h"
#include "TFile.h"
#include "TRefTable.h"
#include "TObjArray.h"
#include "TExec.h"
#include "TSystem.h"
#include "TObjString.h"
#include "TStreamerInfo.h"
#include "TStreamerElement.h"

TObjArray  *TRef::fgExecs  = 0;
TObject    *TRef::fgObject = 0;

ClassImp(TRef)

//______________________________________________________________________________
TRef::TRef(TObject *obj)
{
   // Create a ref to obj.

   *this = obj;
}

//______________________________________________________________________________
TRef::TRef(const TRef &ref) : TObject(ref)
{
   // TRef copy ctor.

   *this = ref;
}

//______________________________________________________________________________
void TRef::operator=(TObject *obj)
{
   // Assign object to reference.

   UInt_t uid = 0;
   fPID = 0;
   if (obj) {
      if (obj->IsA()->CanIgnoreTObjectStreamer()) {
         Error("operator= ","Class: %s IgnoreTObjectStreamer. Cannot reference object",obj->ClassName());
         return;
      }
      if (obj->TestBit(kHasUUID)) {
         fPID = gROOT->GetUUIDs();
         obj->SetBit(kIsReferenced);
         SetBit(kHasUUID);
         uid = obj->GetUniqueID();
      } else {
         if (obj->TestBit(kIsReferenced)) {
            uid = obj->GetUniqueID();
            fPID = TProcessID::GetProcessWithUID(uid,obj);
         } else {
            fPID = TProcessID::GetSessionProcessID();
            uid = TProcessID::AssignID(obj);
         }
         ResetBit(kHasUUID);
      }
   }
   SetUniqueID(uid);
}

//______________________________________________________________________________
TRef &TRef::operator=(const TRef &ref)
{
   // TRef assignment operator.

   SetUniqueID(ref.GetUniqueID());
   fPID = ref.fPID;
   SetBit(kHasUUID,ref.TestBit(kHasUUID));
   return *this;
}

//______________________________________________________________________________
Bool_t operator==(const TRef &r1, const TRef &r2)
{
   // Return kTRUE if r1 and r2 point to the same object.

   if (r1.GetPID() == r2.GetPID() && r1.GetUniqueID() == r2.GetUniqueID()) return kTRUE;
   else return kFALSE;
}

//______________________________________________________________________________
Bool_t operator!=(const TRef &r1, const TRef &r2)
{
   // Return kTRUE if r1 and r2 do not point to the same object.

   if (r1.GetPID() == r2.GetPID() && r1.GetUniqueID() == r2.GetUniqueID()) return kFALSE;
   else return kTRUE;
}

//______________________________________________________________________________
Int_t TRef::AddExec(const char *name)
{
   // If Exec with name does not exist in the list of Execs, it is created.
   // returns the index of the Exec in the list.

   if (!fgExecs) fgExecs = new TObjArray(10);

   TExec *exec = (TExec*)fgExecs->FindObject(name);
   if (!exec) {
       //we register this Exec to the list of Execs.
       exec = new TExec(name,"");
       fgExecs->Add(exec);
   }
   return fgExecs->IndexOf(exec);
}

//______________________________________________________________________________
TObjArray *TRef::GetListOfExecs()
{
   // Return a pointer to the static TObjArray holding the list of Execs.

   if (!fgExecs) fgExecs = new TObjArray(10);

   return fgExecs;
}


//______________________________________________________________________________
TObject *TRef::GetObject() const
{
   // Return a pointer to the referenced object.

   //TObject *obj = 0;
   if (!fPID) return 0;
   if (!TProcessID::IsValid(fPID)) return 0;
   UInt_t uid = GetUniqueID();
   //Try to find the object from the table of the corresponding PID
   TObject *obj = fPID->GetObjectWithID(uid);

   //the reference may be in the TRefTable
   TRefTable *table = TRefTable::GetRefTable();
   if (table) {
      table->SetUID(uid);
      table->Notify();
      obj = fPID->GetObjectWithID(uid);
   }
   
   //if object not found, then exec action if an action has been defined
   if (!obj) {
      //execid in the first 8 bits
      Int_t execid = TestBits(0xff0000);
      if (execid > 0) {
         execid = execid>>16;
         TExec *exec = (TExec*)fgExecs->At(execid-1);
         if (exec) {
            //we expect the object to be returned via TRef::SetObject
            fgObject = 0;
            exec->Exec();
            obj = fgObject;
            if (obj){
               uid = TProcessID::AssignID(obj);
               ((TRef*)this)->SetUniqueID(uid);
               fPID->PutObjectWithID(obj,uid);
            } else {
               //well may be the Exec has loaded the object
               obj = fPID->GetObjectWithID(uid);
            }
         }
      }
   }

   return obj;
}

//______________________________________________________________________________
void TRef::SetAction(const char *name)
{
   // Store the exec number (in the ROOT list of Execs)
   // into the fBits of this TRef.

   TExec *exec = (TExec*)fgExecs->FindObject(name);
   if (!exec) {
      Error("SetAction","Unknow TExec: %s",name);
      return;
   }
   Int_t execid = 1 + fgExecs->IndexOf(exec);
   SetBit(execid << 8);
}

//______________________________________________________________________________
void TRef::SetAction(TObject *parent)
{
   // Find the action to be executed in the dictionary of the parent class
   // and store the corresponding exec number into fBits.
   // This function searches a data member in the class of parent with an
   // offset corresponding to this.
   // If a comment "TEXEC:" is found in the comment field of the data member,
   // the function stores the exec identifier of the exec statement
   // following this keyword.

   if (!parent) return;
   Int_t offset = (char*)this - (char*)parent;
   TClass *cl = parent->IsA();
   cl->BuildRealData(parent);
   TStreamerInfo *info = cl->GetStreamerInfo();
   TIter next(info->GetElements());
   TStreamerElement *element;
   while((element = (TStreamerElement*)next())) {
      if (element->GetOffset() != offset) continue;
      Int_t execid = element->GetExecID();
      if (execid > 0) SetBit(execid << 8);
      return;
   }
}

//______________________________________________________________________________
void TRef::SetObject(TObject *obj)
{
   // Static function to set the object found on the Action on Demand function.
   // This function may be called by the user in the function called
   // when a "EXEC:" keyword is specified in the data member field of the TRef.

   fgObject = obj;
}

//______________________________________________________________________________
void TRef::Streamer(TBuffer &R__b)
{
   // Stream an object of class TRef.

   UShort_t pidf;
   TFile *file = (TFile*)R__b.GetParent();
   if (R__b.IsReading()) {
      TObject::Streamer(R__b);
      if (TestBit(kHasUUID)) {
         TString s;
         s.Streamer(R__b);
         TProcessUUID *pid = gROOT->GetUUIDs();
         UInt_t number = pid->AddUUID(s.Data());
         fPID = pid;
         SetUniqueID(number);
         if (gDebug > 1) {
            printf("Reading TRef (HasUUID) uid=%d, obj=%lx\n",GetUniqueID(),(Long_t)GetObject());
         }
      } else {
         R__b >> pidf;
         fPID = TProcessID::ReadProcessID(pidf,file);
         //The execid has been saved in the unique id of the TStreamerElement
         //being read by TStreamerElement::Streamer
         //The current element (fgElement) is set as a static global
         //by TStreamerInfo::ReadBuffer (Clones) when reading this TRef
         Int_t execid = TStreamerInfo::GetCurrentElement()->GetUniqueID();
         if (execid) SetBit(execid<<16);
         if (gDebug > 1) {
            printf("Reading TRef, pidf=%d, fPID=%lx, uid=%d, obj=%lx\n",pidf,(Long_t)fPID,GetUniqueID(),(Long_t)GetObject());
         }
      }
   } else {
      TObject::Streamer(R__b);

      if (TestBit(kHasUUID)) {
         TObjString *objs = gROOT->GetUUIDs()->FindUUID(GetUniqueID());
         objs->String().Streamer(R__b);
         if (gDebug > 1) {
            printf("Writing TRef (HasUUID) uid=%d, obj=%lx\n",GetUniqueID(),(Long_t)GetObject());
         }
      } else {
         pidf = TProcessID::WriteProcessID(fPID,file);
         R__b << pidf;
         if (gDebug > 1) {
            printf("Writing TRef, pidf=%d, fPID=%lx, uid=%d, obj=%lx\n",pidf,(Long_t)fPID,GetUniqueID(),(Long_t)GetObject());
         }
      }
   }
}
