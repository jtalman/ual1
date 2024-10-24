// @(#)root/xmlparser:$Name:  $:$Id: TSAXParser.h,v 1.3 2005/04/06 13:09:07 rdm Exp $
// Author: Jose Lo   12/1/2005

/*************************************************************************
 * Copyright (C) 1995-2005, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TSAXParser
#define ROOT_TSAXParser

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TSAXParser                                                           //
//                                                                      //
// TSAXParser is a subclass of TXMLParser, it is a wraper class to      //
// libxml library.                                                      //
//                                                                      //
// SAX (Simple API for XML) is an event based interface, which doesn't  //
// maintain the DOM tree in memory, in other words, it's much more      //
// efficient for large document.                                        //
//                                                                      //
// TSAXParserCallback contains a number of callback routines to the     //
// parser in a xmlSAXHandler structure. The parser will then parse the  //
// document and call the appropriate callback when certain conditions   //
// occur.                                                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef ROOT_TXMLParser
#include "TXMLParser.h"
#endif


class TList;
class TSAXParserCallback;
struct _xmlSAXHandler;


class TSAXParser : public TXMLParser {

friend class TSAXParserCallback;

private:
   _xmlSAXHandler         *fSAXHandler;  // libxml2 SAX handler

   virtual Int_t           Parse();

public:
   TSAXParser();
   virtual ~TSAXParser();

   virtual Int_t           ParseFile(const char *filename);
   virtual Int_t           ParseBuffer(const char *contents, Int_t len);

   virtual void            OnStartDocument();
   virtual void            OnEndDocument();
   virtual void            OnStartElement(const char *name, const TList *attr);
   virtual void            OnEndElement(const char *name);
   virtual void            OnCharacters(const char *characters);
   virtual void            OnComment(const char *text);
   virtual void            OnWarning(const char *text);
   virtual Int_t           OnError(const char *text);
   virtual Int_t           OnFatalError(const char *text);
   virtual void            OnCdataBlock(const char *text, Int_t len);

   virtual void            ConnectToHandler(const char *handlerName, void *handler);

   ClassDef(TSAXParser,0);
};

#endif
