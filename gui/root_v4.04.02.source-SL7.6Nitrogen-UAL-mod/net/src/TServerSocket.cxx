// @(#)root/net:$Name:  $:$Id: TServerSocket.cxx,v 1.6 2005/04/28 16:24:23 rdm Exp $
// Author: Fons Rademakers   18/12/96

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

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

#include "TServerSocket.h"
#include "TSocket.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TError.h"
#include <string>

// Hook to server authentication wrapper
SrvAuth_t TServerSocket::fgSrvAuthHook = 0;
SrvClup_t TServerSocket::fgSrvAuthClupHook = 0;

// Defaul options for accept
UChar_t TServerSocket::fgAcceptOpt = kSrvNoAuth;

ClassImp(TServerSocket)

//______________________________________________________________________________
static void setaccopt(UChar_t &Opt, UChar_t Mod)
{
   // Kind of macro to parse input options
   // Modify Opt according to modifier Mod

   if (!Mod) return;

   if ((Mod & kSrvAuth))      Opt |= kSrvAuth;
   if ((Mod & kSrvNoAuth))    Opt &= ~kSrvAuth;
}

//______________________________________________________________________________
TServerSocket::TServerSocket(const char *service, Bool_t reuse, Int_t backlog,
                             Int_t tcpwindowsize)
{
   // Create a server socket object for a named service. Set reuse to true
   // to force reuse of the server socket (i.e. do not wait for the time
   // out to pass). Using backlog one can set the desirable queue length
   // for pending connections.
   // Use tcpwindowsize to specify the size of the receive buffer, it has
   // to be specified here to make sure the window scale option is set (for
   // tcpwindowsize > 65KB and for platforms supporting window scaling).
   // Use IsValid() to check the validity of the
   // server socket. In case server socket is not valid use GetErrorCode()
   // to obtain the specific error value. These values are:
   //  0 = no error (socket is valid)
   // -1 = low level socket() call failed
   // -2 = low level bind() call failed
   // -3 = low level listen() call failed
   // Every valid server socket is added to the TROOT sockets list which
   // will make sure that any open sockets are properly closed on
   // program termination.

   Assert(gROOT);
   Assert(gSystem);

   SetName("ServerSocket");

   fSecContext = 0;
   fSecContexts = new TList;
   int port = gSystem->GetServiceByName(service);
   fService = service;

   if (port != -1) {
      fSocket = gSystem->AnnounceTcpService(port, reuse, backlog, tcpwindowsize);
      if (fSocket >= 0) gROOT->GetListOfSockets()->Add(this);
   } else
      fSocket = -1;
}

//______________________________________________________________________________
TServerSocket::TServerSocket(Int_t port, Bool_t reuse, Int_t backlog,
                             Int_t tcpwindowsize)
{
   // Create a server socket object on a specified port. Set reuse to true
   // to force reuse of the server socket (i.e. do not wait for the time
   // out to pass). Using backlog one can set the desirable queue length
   // for pending connections. If port is 0 a port scan will be done to
   // find a free port. This option is mutual exlusive with the reuse option.
   // Use tcpwindowsize to specify the size of the receive buffer, it has
   // to be specified here to make sure the window scale option is set (for
   // tcpwindowsize > 65KB and for platforms supporting window scaling).
   // Use IsValid() to check the validity of the
   // server socket. In case server socket is not valid use GetErrorCode()
   // to obtain the specific error value. These values are:
   //  0 = no error (socket is valid)
   // -1 = low level socket() call failed
   // -2 = low level bind() call failed
   // -3 = low level listen() call failed
   // Every valid server socket is added to the TROOT sockets list which
   // will make sure that any open sockets are properly closed on
   // program termination.

   Assert(gROOT);
   Assert(gSystem);

   SetName("ServerSocket");

   fSecContext = 0;
   fSecContexts = new TList;
   fService = gSystem->GetServiceByPort(port);
   SetTitle(fService);

   fSocket = gSystem->AnnounceTcpService(port, reuse, backlog, tcpwindowsize);
   if (fSocket >= 0) gROOT->GetListOfSockets()->Add(this);
}

//______________________________________________________________________________
TServerSocket::~TServerSocket()
{
   // Destructor: cleanup authentication stuff (if any) and close

   if (fSecContexts && fgSrvAuthClupHook) {
      TIter next(fSecContexts);
      TSecContext *nsc ;
      while ((nsc = (TSecContext *)next())) {
         if (!strncmp(nsc->GetDetails(),"server",6))
            (*fgSrvAuthClupHook)(nsc->GetToken());
      }
      // Remove the list
      fSecContexts->Delete();
      SafeDelete(fSecContexts);
      fSecContexts = 0;
   }

   Close();
}

//______________________________________________________________________________
TSocket *TServerSocket::Accept(UChar_t Opt)
{
   // Accept a connection on a server socket. Returns a full-duplex
   // communication TSocket object. If no pending connections are
   // present on the queue and nonblocking mode has not been enabled
   // with SetOption(kNoBlock,1) the call blocks until a connection is
   // present. The returned socket must be deleted by the user. The socket
   // is also added to the TROOT sockets list which will make sure that
   // any open sockets are properly closed on program termination.
   // In case of error 0 is returned and in case non-blocking I/O is
   // enabled and no connections are available -1 is returned.
   //
   // Opt can be used to require client authentication; valid options are 
   //       
   //    kSrvAuth   =   require client authentication
   //    kSrvNoAuth =   force no client authentication
   //
   // Example: use Opt = kSrvAuth to require client authentication. 
   //
   // Default options are taken from fgAcceptOpt and are initially
   // equivalent to kSrvNoAuth; they can be changed with the static
   // method TServerSocket::SetAcceptOptions(Opt).
   // The active defaults can be visualized using the static method
   // TServerSocket::ShowAcceptOptions().
   //

   if (fSocket == -1) { return 0; }

   TSocket *socket = new TSocket;

   Int_t soc = gSystem->AcceptConnection(fSocket);
   if (soc == -1) { delete socket; return 0; }
   if (soc == -2) { delete socket; return (TSocket*) -1; }

   // Parse Opt
   UChar_t AcceptOpt = fgAcceptOpt;
   setaccopt(AcceptOpt,Opt);
   Bool_t Auth = (Bool_t)(AcceptOpt & kSrvAuth);

   socket->fSocket  = soc;
   socket->fSecContext = 0;
   socket->fService = fService;
   socket->fAddress = gSystem->GetPeerName(socket->fSocket);
   if (socket->fSocket >= 0) gROOT->GetListOfSockets()->Add(socket);


   // Perform authentication, if required
   if (Auth) {
      if (!Authenticate(socket)) {
         delete socket;
         socket = 0;
      }
   }

   return socket;
}

//______________________________________________________________________________
TInetAddress TServerSocket::GetLocalInetAddress()
{
   // Return internet address of host to which the server socket is bound,
   // i.e. the local host. In case of error TInetAddress::IsValid() returns
   // kFALSE.

   if (fSocket != -1) {
      if (fAddress.GetPort() == -1)
         fAddress = gSystem->GetSockName(fSocket);
      return fAddress;
   }
   return TInetAddress();
}

//______________________________________________________________________________
Int_t TServerSocket::GetLocalPort()
{
   // Get port # to which server socket is bound. In case of error returns -1.

   if (fSocket != -1) {
      if (fAddress.GetPort() == -1)
         fAddress = GetLocalInetAddress();
      return fAddress.GetPort();
   }
   return -1;
}


//______________________________________________________________________________
UChar_t TServerSocket::GetAcceptOptions()
{
   // Return default options for Accept

   return fgAcceptOpt;
}

//______________________________________________________________________________
void TServerSocket::SetAcceptOptions(UChar_t mod)
{
   // Set default options for Accept according to modifier 'mod'.
   // Use:
   //   kSrvAuth                 require client authentication
   //   kSrvNoAuth               do not require client authentication

   setaccopt(fgAcceptOpt,mod);
}

//______________________________________________________________________________
void TServerSocket::ShowAcceptOptions()
{
   // Print default options for Accept

   ::Info("ShowAcceptOptions","    Auth: %d",(Bool_t)(fgAcceptOpt & kSrvAuth));
}

//______________________________________________________________________________
Bool_t TServerSocket::Authenticate(TSocket *sock) 
{
   // Check authentication request from the client on new 
   // open connection

   if (!fgSrvAuthHook) {

      // Load libraries needed for (server) authentication ...
#ifdef ROOTLIBDIR
      TString srvlib = TString(ROOTLIBDIR) + "/libSrvAuth";
#else
      TString srvlib = TString(gRootDir) + "/lib/libSrvAuth";
#endif
      char *p = 0;
      // The generic one
      if ((p = gSystem->DynamicPathName(srvlib, kTRUE))) {
         delete[] p;
         if (gSystem->Load(srvlib) == -1) {
            Error("Authenticate", "can't load %s",srvlib.Data());
            return kFALSE;
         }
      } else {
         Error("Authenticate", "can't locate %s",srvlib.Data());
         return kFALSE;
      }
      //   
      // Locate SrvAuthenticate
      Func_t f = gSystem->DynFindSymbol(srvlib,"SrvAuthenticate");
      if (f)
         fgSrvAuthHook = (SrvAuth_t)(f);
      else {
         Error("Authenticate", "can't find SrvAuthenticate");
         return kFALSE;
      }
      //   
      // Locate SrvAuthCleanup
      f = gSystem->DynFindSymbol(srvlib,"SrvAuthCleanup");
      if (f)
         fgSrvAuthClupHook = (SrvClup_t)(f);
      else {
         Warning("Authenticate", "can't find SrvAuthCleanup");
      }
   }

   TString confdir;
#ifndef ROOTPREFIX
   // try to guess the config directory...
   if (gSystem->Getenv("ROOTSYS")) {
      confdir = TString(gSystem->Getenv("ROOTSYS"));
   } else {
      // Try to guess it from 'root.exe' path
      confdir = TString(gSystem->Which(gSystem->Getenv("PATH"), 
                        "root.exe", kExecutePermission));
      confdir.Resize(confdir.Last('/'));
   }
#else
   confdir = TString(ROOTPREFIX);
#endif
   if (!confdir.Length()) {
      Error("Authenticate", "config dir undefined");
      return kFALSE;
   }

   // dir for temporary files
   TString tmpdir = TString(gSystem->TempDirectory());
   if (gSystem->AccessPathName(tmpdir, kWritePermission))
      tmpdir = TString("/tmp");

   // Get Host name
   TString openhost(sock->GetInetAddress().GetHostName());
   if (gDebug > 2)
      Info("Authenticate","OpenHost = %s", openhost.Data());

   // Run Authentication now
   std::string user;
   Int_t meth = -1;
   Int_t auth = 0;
   Int_t type = 0;
   std::string ctkn = "";
   if (fgSrvAuthHook)
      auth = (*fgSrvAuthHook)(sock,confdir,tmpdir,user,meth,type,ctkn);

   if (gDebug > 2)
      Info("Authenticate","auth = %d, type= %d, ctkn= %s",
            auth, type, ctkn.c_str());

   TSecContext *seccontext = 0;
   if (auth > 0) {

      if (type == 1) {
         // An existing authentication has been re-used: retrieve 
         // the related security context
         TIter next(gROOT->GetListOfSecContexts());
         while ((seccontext = (TSecContext *)next())) {
            if (!(strncmp(seccontext->GetDetails(),"server",6))) {
               if (seccontext->GetMethod() == meth) {
                  if (openhost == seccontext->GetHost()) {
                     if (!strcmp(user.c_str(),seccontext->GetUser()))
                        break;
                  }
               }
            }
         }
      }

      if (!seccontext) {
         // New authentication: Fill a SecContext for cleanup
         // in case of interrupt
         seccontext = new TSecContext(user.c_str(), openhost, meth, -1,
                                      "server", ctkn.c_str());
         if (seccontext) { 
            // Add to the list
            fSecContexts->Add(seccontext);
            // Store SecContext
            sock->SetSecContext(seccontext);
         } else {
            if (gDebug > 0)
               Warning("Authenticate","could not create sec context object"
                                      ": potential problems in cleaning");
         }
      }
   }

   return auth;
}
