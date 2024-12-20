// @(#)root/pgsql:$Name:  $:$Id: TPgSQLServer.h,v 1.2 2003/02/11 12:30:28 rdm Exp $
// Author: g.p.ciceri <gp.ciceri@acm.org> 01/06/2001

/*************************************************************************
 * Copyright (C) 1995-2001, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TPgSQLServer
#define ROOT_TPgSQLServer

#ifndef ROOT_TSQLServer
#include "TSQLServer.h"
#endif

#if !defined(__CINT__)
#include <libpq-fe.h>
#else
struct PGconn;
#endif



class TPgSQLServer : public TSQLServer {

private:
   PGconn  *fPgSQL;    // connection to PgSQL server

public:
   TPgSQLServer(const char *db, const char *uid, const char *pw);
   ~TPgSQLServer();

   void Close(Option_t *opt="");
   TSQLResult *Query(const char *sql);
   Int_t       SelectDataBase(const char *dbname);
   TSQLResult *GetDataBases(const char *wild = 0);
   TSQLResult *GetTables(const char *dbname, const char *wild = 0);
   TSQLResult *GetColumns(const char *dbname, const char *table, const char *wild = 0);
   Int_t       CreateDataBase(const char *dbname);
   Int_t       DropDataBase(const char *dbname);
   Int_t       Reload();
   Int_t       Shutdown();
   const char *ServerInfo();

   ClassDef(TPgSQLServer,0)  // Connection to PgSQL server
};

#endif
