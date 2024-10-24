// @(#)root/meta:$Name:  $:$Id: TClassMenuItem.cxx,v 1.1 2002/04/04 17:32:13 rdm Exp $
// Author: Damir Buskulic   23/11/2001

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Describes one element of the context menu associated to a class      //
// The menu item may describe                                           //
//    - a separator,                                                    //
//    - standard list of methods i.e. the methods defined in            //
//      the described class by a *MENU* in the comment field            //
//      of the header,                                                  //
//    - a method of an external class or a global function              //
//  All the standard methods of the class are described by only         //
//  one item. Since a complete context menu is described by a TList of  //
//  TClassMenuItem elements, it is possible to customize the context    //
//  menu of a class by removing the element "standard methods" and      //
//  replacing it by whatever one wants.                                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TClassMenuItem.h"
#include "TList.h"
#include "TClass.h"


ClassImp(TClassMenuItem)

//______________________________________________________________________________
TClassMenuItem::TClassMenuItem() : TObject()
{
   // Default TClassMenuItem ctor. TClassMenuItems are constructed in TClass
   // with a standard content but may be customized later
   // fType = 0 : external method/function
   // fType = 1 : separator
   // fType = 2 : standard methods list

   fType          = kPopupUserFunction;
   fSelf          = 0;
   fToggle        = 0;
   fCalledObject  = 0;
   fSubMenu       = 0;
   fParent        = 0;
   fSelfObjectPos = -1;
}

//______________________________________________________________________________
TClassMenuItem::TClassMenuItem(Int_t type, TClass *parentcl,
    const char *title, const char *functionname, TObject *obj,
    const char *args, Int_t selfobjposition, Bool_t self) : TObject()
{
   // TClassMenuItem ctor. TClassMenuItems are constructed in TClass
   // with a standard content but may be customized later
   // type = 0 : external method/function
   // type = 1 : separator
   // type = 2 : standard methods list
   // self indicates if the object to be called is the one selected
   // by the popup menu
   // selfobjposition, if non zero, indicates the position in the arguments
   // list of the argument corresponding to the selected (clicked) object.
   // This argument in the calling method should be a TObject*

   fType          = (EClassMenuItemType) type;
   fSelf          = self;
   fToggle        = 0;
   fTitle         = title;
   fCalledObject  = obj;
   fFunctionName  = functionname;
   fArgs          = args;
   fSubMenu       = 0;
   fParent        = parentcl;
   fSelfObjectPos = selfobjposition;
}

//______________________________________________________________________________
TClassMenuItem::~TClassMenuItem()
{
   // TClassMenuItem dtor.

   if (fParent) fParent->GetMenuList()->Remove(this);
}
