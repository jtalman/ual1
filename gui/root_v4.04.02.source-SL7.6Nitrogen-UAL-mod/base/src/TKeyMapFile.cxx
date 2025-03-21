// @(#)root/base:$Name:  $:$Id: TKeyMapFile.cxx,v 1.1.1.1 2000/05/16 17:00:39 rdm Exp $
// Author: Rene Brun   23/07/97

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// Utility class for browsing TMapFile objects.                         //
// When the browser is invoked for a TMapFile, a TKeyMapFile object     //
// is created for each object in the mapped file.                       //
// When a TKeyMapFile object is clicked in the browser, a new copy      //
// of this object is copied into the local directory and the action     //
// corresponding to object->Browse is executed (typically Draw).        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TKeyMapFile.h"
#include "TDirectory.h"
#include "TMapFile.h"
#include "TBrowser.h"

ClassImp(TKeyMapFile)

//______________________________________________________________________________
TKeyMapFile::TKeyMapFile() : TNamed()
{
}

//______________________________________________________________________________
TKeyMapFile::TKeyMapFile(const char *name, const char *classname, TMapFile *mapfile)
      : TNamed(name,classname)
{
   fMapFile = mapfile;
}

//______________________________________________________________________________
void TKeyMapFile::Browse(TBrowser *b)
{
    TObject *obj = gDirectory->Get((char*)GetName());
    delete obj;
    obj = fMapFile->Get(GetName(),0);

    if( b && obj )
        obj->Browse( b );
}
