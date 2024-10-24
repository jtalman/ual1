// @(#)root/pyroot:$Name:  $:$Id: MemoryRegulator.cxx,v 1.9 2005/03/04 07:44:11 brun Exp $
// Author: Wim Lavrijsen, Apr 2004

// Bindings
#include "PyROOT.h"
#include "MemoryRegulator.h"
#include "ObjectProxy.h"

// ROOT
#include "TObject.h"
#include "TClass.h"

// Standard
#include <string.h>
#include <Riostream.h>


//- static data -----------------------------------------------------------------
PyROOT::MemoryRegulator::ObjectMap_t PyROOT::MemoryRegulator::fgObjectTable;


namespace {

// memory regulater callback for deletion of registered objects
   PyMethodDef methoddef_ = {
      const_cast< char* >( "MemoryRegulator_internal_ObjectEraseCallback" ),
      (PyCFunction) PyROOT::MemoryRegulator::ObjectEraseCallback,
      METH_O,
      NULL
   };

   PyObject* gObjectEraseCallback = PyCFunction_New( &methoddef_, NULL );


// pseudo-None type for masking out objects on the python side
   PyTypeObject PyROOT_NoneType;

//____________________________________________________________________________
   int AlwaysNullLength( PyObject* )
   {
      return 0;
   }

//____________________________________________________________________________
   PyMappingMethods PyROOT_NoneType_mapping = {
        (inquiry) AlwaysNullLength,
        (binaryfunc)             0,
        (objobjargproc)          0
   };

//____________________________________________________________________________
   struct Init_PyROOT_NoneType {
      Init_PyROOT_NoneType()
      {
         memset( &PyROOT_NoneType, 0, sizeof( PyROOT_NoneType ) );

         PyROOT_NoneType.ob_type   = &PyType_Type;
         PyROOT_NoneType.ob_size   = 0;
         PyROOT_NoneType.ob_refcnt = 1;

         PyROOT_NoneType.tp_name        = const_cast< char* >( "PyROOT_NoneType" );
         PyROOT_NoneType.tp_flags       = Py_TPFLAGS_HAVE_RICHCOMPARE | Py_TPFLAGS_HAVE_GC;

         PyROOT_NoneType.tp_traverse    = (traverseproc) 0;
         PyROOT_NoneType.tp_clear       = (inquiry) 0;
         PyROOT_NoneType.tp_dealloc     = (destructor)  &Init_PyROOT_NoneType::dealloc;
         PyROOT_NoneType.tp_repr        = Py_None->ob_type->tp_repr;
         PyROOT_NoneType.tp_richcompare = (richcmpfunc) &Init_PyROOT_NoneType::richcompare;
         PyROOT_NoneType.tp_compare     = (cmpfunc) &Init_PyROOT_NoneType::compare;
         PyROOT_NoneType.tp_hash        = (hashfunc) &Init_PyROOT_NoneType::ptrhash;

         PyROOT_NoneType.tp_as_mapping  = &PyROOT_NoneType_mapping;

         PyType_Ready( &PyROOT_NoneType );
      }

      static void dealloc( PyObject* obj ) { obj->ob_type->tp_free( obj ); }
      static int ptrhash( PyObject* obj ) { return (int) long(obj); }

      static PyObject* richcompare( PyObject*, PyObject* other, int opid )
      {
         return PyObject_RichCompare( other, Py_None, opid );
      }

      static int compare( PyObject*, PyObject* other )
      {
         return PyObject_Compare( other, Py_None );
      }
   };

} // unnamed namespace


//- constructor -----------------------------------------------------------------
PyROOT::MemoryRegulator::MemoryRegulator()
{
   static Init_PyROOT_NoneType init_PyROOT_NoneType;
}


//- public members --------------------------------------------------------------
void PyROOT::MemoryRegulator::RecursiveRemove( TObject* object )
{
   if ( ! object || fgObjectTable.size() == 0 )   // table can be deleted before libCore is done
      return;

// see whether we're tracking this object
   ObjectMap_t::iterator ppo = fgObjectTable.find( object );

   if ( ppo != fgObjectTable.end() ) {
   // get the tracked object
      ObjectProxy* pyobj = (ObjectProxy*)PyWeakref_GetObject( ppo->second );
      if ( ! pyobj )
         return;

   // clean up the weak reference.
      Py_DECREF( ppo->second );

   // nullify the object
      if ( ObjectProxy_Check( pyobj ) ) {
         if ( ! PyROOT_NoneType.tp_traverse ) {
         // take a reference as we're copying its function pointers
            Py_INCREF( pyobj->ob_type );

         // all object that arrive here are expected to be of the same type ("instance")
            PyROOT_NoneType.tp_traverse   = pyobj->ob_type->tp_traverse;
            PyROOT_NoneType.tp_clear      = pyobj->ob_type->tp_clear;
            PyROOT_NoneType.tp_free       = pyobj->ob_type->tp_free;
         } else if ( PyROOT_NoneType.tp_traverse != pyobj->ob_type->tp_traverse ) {
            std::cerr << "in PyROOT::MemoryRegulater, unexpected object of type: "
                      << pyobj->ob_type->tp_name << std::endl;

         // leave before too much damage is done
            return;
         }

      // notify any other weak referents by playing dead
         int refcnt = pyobj->ob_refcnt;
         pyobj->ob_refcnt = 0;
         PyObject_ClearWeakRefs( (PyObject*)pyobj );
         pyobj->ob_refcnt = refcnt;

      // reset type object
         Py_INCREF( &PyROOT_NoneType );
         Py_DECREF( pyobj->ob_type );
         pyobj->ob_type = &PyROOT_NoneType;
      }

   // erase the object from tracking
      fgObjectTable.erase( ppo );
   }
}

//____________________________________________________________________________
void PyROOT::MemoryRegulator::RegisterObject( ObjectProxy* pyobj, TObject* object )
{
   if ( ! ( pyobj && object ) )
      return;

   ObjectMap_t::iterator ppo = fgObjectTable.find( object );
   if ( ppo == fgObjectTable.end() ) {
      object->SetBit( kMustCleanup );
      fgObjectTable[ object ] = PyWeakref_NewRef( (PyObject*)pyobj, gObjectEraseCallback );
   }
}

//____________________________________________________________________________
PyObject* PyROOT::MemoryRegulator::RetrieveObject( TObject* object )
{
   if ( ! object )
      return 0;

   ObjectMap_t::iterator ppo = fgObjectTable.find( object );
   if ( ppo != fgObjectTable.end() ) {
      PyObject* pyobj = PyWeakref_GetObject( ppo->second );
      Py_XINCREF( pyobj );
      return pyobj;
   }

   return 0;
}


//- private static members ------------------------------------------------------
PyObject* PyROOT::MemoryRegulator::ObjectEraseCallback( PyObject*, PyObject* pyref )
{
// called when one of the python objects we've registered is going away
   ObjectProxy* pyobj = (ObjectProxy*)PyWeakref_GetObject( pyref );

   if ( ObjectProxy_Check( pyobj ) && pyobj->GetObject() != 0 ) {
   // get TObject pointer to the object
      TObject* object = (TObject*)pyobj->ObjectIsA()->DynamicCast(
         TObject::Class(), pyobj->GetObject() );

      if ( object != 0 ) {
      // erase if tracked
         ObjectMap_t::iterator ppo = fgObjectTable.find( object );
         if ( ppo != fgObjectTable.end() ) {
         // cleanup weak reference, and table entry
            Py_DECREF( ppo->second );
            fgObjectTable.erase( ppo );
         }
      }
   } else {
      ObjectMap_t::iterator ppo = fgObjectTable.begin();
      for ( ; ppo != fgObjectTable.end(); ++ppo ) {
         if ( ppo->second == pyref )
            break;
      }

      if ( ppo != fgObjectTable.end() ) {
      // cleanup weak reference, and table entry
         Py_DECREF( ppo->second );
         fgObjectTable.erase( ppo );
      }
   }

   Py_INCREF( Py_None );
   return Py_None;
}
