// @(#)root/net:$Name:  $:$Id: TInetAddress.cxx,v 1.6 2004/07/22 07:17:32 brun Exp $
// Author: Fons Rademakers   16/12/96

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TInetAddress                                                         //
//                                                                      //
// This class represents an Internet Protocol (IP) address.             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TInetAddress.h"
#include "TClass.h"

ClassImp(TInetAddress)

//______________________________________________________________________________
TInetAddress::TInetAddress()
{
   // Default ctor. Used in case of unknown host. Not a valid address.

   fHostname  = "UnknownHost";
   AddAddress(0);
   fFamily    = -1;
   fPort      = -1;
}

//______________________________________________________________________________
TInetAddress::TInetAddress(const char *host, UInt_t addr, Int_t family, Int_t port)
{
   // Create TInetAddress. Private ctor. TInetAddress objects can only
   // be created via the friend classes TSystem, TServerSocket and TSocket.
   // Use the IsValid() method to check the validity of a TInetAddress.

   AddAddress(addr);
   if (!strcmp(host, "????"))
      fHostname = GetHostAddress();
   else
      fHostname = host;
   fFamily    = family;
   fPort      = port;
}

//______________________________________________________________________________
TInetAddress::TInetAddress(const TInetAddress &adr) : TObject(adr)
{
   // TInetAddress copy ctor.

   fHostname  = adr.fHostname;
   fFamily    = adr.fFamily;
   fPort      = adr.fPort;
   fAddresses = adr.fAddresses;
   fAliases   = adr.fAliases;
}

//______________________________________________________________________________
TInetAddress& TInetAddress::operator=(const TInetAddress &rhs)
{
   // TInetAddress assignment operator.

   if (this != &rhs) {
      TObject::operator=(rhs);
      fHostname  = rhs.fHostname;
      fFamily    = rhs.fFamily;
      fPort      = rhs.fPort;
      fAddresses = rhs.fAddresses;
      fAliases   = rhs.fAliases;
   }
   return *this;
}

//______________________________________________________________________________
UChar_t *TInetAddress::GetAddressBytes() const
{
   // Returns the raw IP address in host byte order. The highest
   // order byte position is in addr[0]. To be prepared for 64-bit
   // IP addresses an array of bytes is returned.

   static UChar_t addr[4];

   addr[0] = (UChar_t) ((fAddresses[0] >> 24) & 0xFF);
   addr[1] = (UChar_t) ((fAddresses[0] >> 16) & 0xFF);
   addr[2] = (UChar_t) ((fAddresses[0] >> 8) & 0xFF);
   addr[3] = (UChar_t)  (fAddresses[0] & 0xFF);

   return addr;
}

//______________________________________________________________________________
const char *TInetAddress::GetHostAddress(UInt_t addr)
{
   // Returns the IP address string "%d.%d.%d.%d", use it to convert
   // alternative addresses obtained via GetAddresses().
   // Copy string immediately, it will be reused. Static function.

   return Form("%d.%d.%d.%d", (addr >> 24) & 0xFF,
                              (addr >> 16) & 0xFF,
                              (addr >>  8) & 0xFF,
                               addr & 0xFF);
}

//______________________________________________________________________________
const char *TInetAddress::GetHostAddress() const
{
   // Returns the IP address string "%d.%d.%d.%d".
   // Copy string immediately, it will be reused.

   return GetHostAddress(fAddresses[0]);
}

//______________________________________________________________________________
void TInetAddress::Print(Option_t *) const
{
   // Print internet address as string.

   if (fPort == -1)
      Printf("%s/%s (not connected)", GetHostName(), GetHostAddress());
   else
      Printf("%s/%s (port %d)", GetHostName(), GetHostAddress(), fPort);

   int i = 0;
   AddressList_t::const_iterator ai;
   for (ai = fAddresses.begin(); ai != fAddresses.end(); ai++) {
      if (!i) printf("%s:", fAddresses.size() == 1 ? "Address" : "Addresses");
      printf(" %s", GetHostAddress(*ai));
      i++;
   }
   if (i) printf("\n");

   i = 0;
   AliasList_t::const_iterator ali;
   for (ali = fAliases.begin(); ali != fAliases.end(); ali++) {
      if (!i) printf("%s:", fAliases.size() == 1 ? "Alias" : "Aliases");
      printf(" %s", ali->Data());
      i++;
   }
   if (i) printf("\n");
}

//______________________________________________________________________________
void TInetAddress::AddAddress(UInt_t addr)
{
   // Add alternative address to list of addresses.

   fAddresses.push_back(addr);
}

//______________________________________________________________________________
void TInetAddress::AddAlias(const char *alias)
{
   // Add alias to list of aliases.

   fAliases.push_back(TString(alias));
}

//______________________________________________________________________________
void TInetAddress::Streamer(TBuffer &R__b)
{
   // Stream an object of class TInetAddress.

   if (R__b.IsReading()) {
      UInt_t R__s, R__c;
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      if (R__v > 2) {
         TInetAddress::Class()->ReadBuffer(R__b, this);
         return;
      }
      // process old versions before automatic schema evolution
      TObject::Streamer(R__b);
      fHostname.Streamer(R__b);
      R__b >> fAddress;
      R__b >> fFamily;
      R__b >> fPort;
      if (R__v > 1) {
         TInetAddress::AddressList_t &R__stl1 =  fAddresses;
         R__stl1.clear();
         int R__i, R__n;
         R__b >> R__n;
         R__stl1.reserve(R__n);
         for (R__i = 0; R__i < R__n; R__i++) {
            unsigned int R__t1;
            R__b >> R__t1;
            R__stl1.push_back(R__t1);
         }
         TInetAddress::AliasList_t &R__stl2 =  fAliases;
         R__stl2.clear();
         R__b >> R__n;
         R__stl2.reserve(R__n);
         for (R__i = 0; R__i < R__n; R__i++) {
            TString R__t2;
            R__t2.Streamer(R__b);
            R__stl2.push_back(R__t2);
         }
      }
      R__b.CheckByteCount(R__s, R__c, TInetAddress::IsA());
   } else {
      TInetAddress::Class()->WriteBuffer(R__b, this);
   }
}
