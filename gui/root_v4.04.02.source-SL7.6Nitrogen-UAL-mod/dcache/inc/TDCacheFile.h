// @(#)root/dcache:$Name:  $:$Id: TDCacheFile.h,v 1.9 2005/01/14 10:21:05 rdm Exp $
// Author: Grzegorz Mazur   20/01/2002
// Updated: William Tanenbaum 21/11/2003
// Updated: Tgiran Mkrtchyan 28/06/2004

/*************************************************************************
 * Copyright (C) 1995-2000, Rene Brun and Fons Rademakers.               *
 * All rights reserved.                                                  *
 *                                                                       *
 * For the licensing terms see $ROOTSYS/LICENSE.                         *
 * For the list of contributors see $ROOTSYS/README/CREDITS.             *
 *************************************************************************/

#ifndef ROOT_TDCacheFile
#define ROOT_TDCacheFile


//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TDCacheFile                                                          //
//                                                                      //
// A TDCacheFile is like a normal TFile except that it reads and writes //
// its data via a dCache server.                                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


#ifndef ROOT_TFile
#include "TFile.h"
#endif
#ifndef ROOT_TSystem
#include "TSystem.h"
#endif
#ifndef ROOT_TString
#include "TString.h"
#endif

#include <sys/stat.h>


class TDCacheFile : public TFile {

private:
   Bool_t fStatCached;       //! (transient) is file status cached?
   struct stat64 fStatBuffer;  //! (transient) Cached file status buffer (for performance)

   TDCacheFile() : fStatCached(kFALSE) { }

   // Interface to basic system I/O routines
   Int_t    SysOpen(const char *pathname, Int_t flags, UInt_t mode);
   Int_t    SysClose(Int_t fd);
   Int_t    SysRead(Int_t fd, void *buf, Int_t len);
   Int_t    SysWrite(Int_t fd, const void *buf, Int_t len);
   Long64_t SysSeek(Int_t fd, Long64_t offset, Int_t whence);
   Int_t    SysStat(Int_t fd, Long_t *id, Long64_t *size, Long_t *flags, Long_t *modtime);
   Int_t    SysSync(Int_t fd);

public:
   TDCacheFile(const char *path, Option_t *option="",
               const char *ftitle="", Int_t compress=1);

   ~TDCacheFile();

   Bool_t  ReadBuffer(char *buf, Int_t len);
   Bool_t  WriteBuffer(const char *buf, Int_t len);

   void    ResetErrno() const;

   static Bool_t Stage(const char *path, UInt_t secs,
                       const char *location = 0);
   static Bool_t CheckFile(const char *path, const char *location = 0);

   // Note: This must be kept in sync with values #defined in dcap.h
   enum OnErrorAction {
      kOnErrorRetry   =  1,
      kOnErrorFail    =  0,
      kOnErrorDefault = -1
   };

   static void SetOpenTimeout(UInt_t secs);
   static void SetOnError(OnErrorAction = kOnErrorDefault);

   static void SetReplyHostName(const char *host_name);
   static const char *GetDcapVersion();
   static TString GetDcapPath(const char *path);


   ClassDef(TDCacheFile,1)  //A ROOT file that reads/writes via a dCache server
};


class TDCacheSystem : public TSystem {

private:
   void    *fDirp;   // directory handler

   void    *GetDirPtr() const { return fDirp; }

public:
   TDCacheSystem();
   virtual ~TDCacheSystem() { }

   Int_t       MakeDirectory(const char *name);
   void       *OpenDirectory(const char *name);
   void        FreeDirectory(void *dirp);
   const char *GetDirEntry(void *dirp);
   Int_t       GetPathInfo(const char *path, FileStat_t &buf);
   Bool_t      AccessPathName(const char *path, EAccessMode mode);

   ClassDef(TDCacheSystem,0)  // Directory handler for DCache
};

#endif
