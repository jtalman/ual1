// @(#)root/net:$Name:  $:$Id: TServerSocket.h,v 1.6 2004/10/11 12:34:34 rdm Exp $
// Author: Fons Rademakers   18/12/96

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TServerSocket
#define ROOT_TServerSocket


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TServerSocket                                                        //
//                                                                      //
// This class implements server sockets. A server socket waits for      //
// requests to come in over the network. It performs some operation     //
// based on that request and then possibly returns a full duplex socket //
// to the requester. The actual work is done via the TSystem class      //
// (either TUnixSystem, TWin32System or TMacSystem).                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TSocket
#include "TSocket.h"
#endif
#include <string>

typedef Int_t (*SrvAuth_t)(TSocket *sock, const char *, const char *, 
                           std::string&, Int_t &, Int_t &, std::string &);
typedef Int_t (*SrvClup_t)(const char *);

// These mask are globally available to manipulate the option to Accept
const UChar_t kSrvAuth   = 0x1;            // Require client authentication
const UChar_t kSrvNoAuth = (kSrvAuth<<4);  // Force no client authentication

class TServerSocket : public TSocket {

private:
   TSeqCollection  *fSecContexts; // List of TSecContext with cleanup info
   static SrvAuth_t fgSrvAuthHook;
   static SrvClup_t fgSrvAuthClupHook;
   static UChar_t fgAcceptOpt;     // Default accept options 

   TServerSocket() { }
   TServerSocket(const TServerSocket &);
   void operator=(const TServerSocket &);
   Bool_t Authenticate(TSocket *);

public:
   enum { kDefaultBacklog = 10 };

   TServerSocket(Int_t port, Bool_t reuse = kFALSE, Int_t backlog = kDefaultBacklog,
                 Int_t tcpwindowsize = -1);
   TServerSocket(const char *service, Bool_t reuse = kFALSE,
                 Int_t backlog = kDefaultBacklog, Int_t tcpwindowsize = -1);
   virtual ~TServerSocket();

   virtual TSocket      *Accept(UChar_t Opt = 0);
   virtual TInetAddress  GetLocalInetAddress();
   virtual Int_t         GetLocalPort();

   Int_t         Send(const TMessage &)
                    { MayNotUse("Send(const TMessage &)"); return 0; }
   Int_t         Send(Int_t)
                    { MayNotUse("Send(Int_t)"); return 0; }
   Int_t         Send(Int_t, Int_t)
                    { MayNotUse("Send(Int_t, Int_t)"); return 0; }
   Int_t         Send(const char *, Int_t = kMESS_STRING)
                    { MayNotUse("Send(const char *, Int_t)"); return 0; }
   Int_t         SendObject(const TObject *, Int_t = kMESS_OBJECT)
                    { MayNotUse("SendObject(const TObject *, Int_t)"); return 0; }
   Int_t         SendRaw(const void *, Int_t, ESendRecvOptions = kDefault)
                    { MayNotUse("SendRaw(const void *, Int_t, ESendRecvOptions)"); return 0; }
   Int_t         Recv(TMessage *&)
                    { MayNotUse("Recv(TMessage *&)"); return 0; }
   Int_t         Recv(Int_t &, Int_t &)
                    { MayNotUse("Recv(Int_t &, Int_t &)"); return 0; }
   Int_t         Recv(char *, Int_t)
                    { MayNotUse("Recv(char *, Int_t)"); return 0; }
   Int_t         Recv(char *, Int_t, Int_t &)
                    { MayNotUse("Recv(char *, Int_t, Int_t &)"); return 0; }
   Int_t         RecvRaw(void *, Int_t, ESendRecvOptions = kDefault)
                    { MayNotUse("RecvRaw(void *, Int_t, ESendRecvOptions)"); return 0; }

   static UChar_t     GetAcceptOptions();
   static void        SetAcceptOptions(UChar_t Opt);
   static void        ShowAcceptOptions();

   ClassDef(TServerSocket,1)  //This class implements server sockets
};

#endif
