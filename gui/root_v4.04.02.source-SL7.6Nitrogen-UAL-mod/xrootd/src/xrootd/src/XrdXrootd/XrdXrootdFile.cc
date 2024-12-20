/******************************************************************************/
/*                                                                            */
/*                      X r d X r o o t d F i l e . c c                       */
/*                                                                            */
/* (c) 2004 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*       All Rights Reserved. See XrdInfo.cc for complete License Terms       */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/

//         $Id: XrdXrootdFile.cc,v 1.10 2005/02/25 05:36:10 abh Exp $

const char *XrdXrootdFileCVSID = "$Id: XrdXrootdFile.cc,v 1.10 2005/02/25 05:36:10 abh Exp $";

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
  
#include "XrdOuc/XrdOucPthread.hh"
#include "XrdSfs/XrdSfsInterface.hh"
#include "XrdXrootd/XrdXrootdFile.hh"
#include "XrdXrootd/XrdXrootdFileLock.hh"
#define  TRACELINK this
#include "XrdXrootd/XrdXrootdTrace.hh"
 
/******************************************************************************/
/*                               G l o b a l s                                */
/******************************************************************************/

#ifndef NODEBUG  
extern XrdOucTrace      *XrdXrootdTrace;
#endif

       XrdXrootdFileLock *XrdXrootdFile::Locker;

       const char      *XrdXrootdFile::TraceID      = "File";
       const char      *XrdXrootdFileTable::TraceID = "FileTable";

/******************************************************************************/
/*                        x r d _ F i l e   C l a s s                         */
/******************************************************************************/
/******************************************************************************/
/*                           C o n s t r u c t o r                            */
/******************************************************************************/
  
XrdXrootdFile::XrdXrootdFile(char *id, XrdSfsFile *fp, char mode, char async)
{
    static XrdOucMutex seqMutex;
    struct stat buf;
    int i;

    XrdSfsp  = fp;
    mmAddr   = 0;
    mmSize   = 0;
    FileMode = mode;
    AsyncMode= async;
    ID       = id;
    FileID   = 0;
    readCnt  = 0;
    writeCnt = 0;

// Develop a unique hash for this file
//
   fp->stat(&buf);
   i = bin2hex((char *)FileKey,    (char *)&buf.st_dev, sizeof(buf.st_dev));
   i = bin2hex((char *)&FileKey[i],(char *)&buf.st_ino, sizeof(buf.st_ino));
}
  
/******************************************************************************/
/*                            D e s t r u c t o r                             */
/******************************************************************************/
  
XrdXrootdFile::~XrdXrootdFile()
{
   char *fn;

   if (XrdSfsp) {Locker->Unlock(this);
               if (TRACING(TRACE_FS))
                  {if (!(fn = (char *)XrdSfsp->FName())) fn = (char *)"?";
                   TRACEI(FS, "closing " <<FileMode <<' ' <<fn);
                  }
               delete XrdSfsp;
               XrdSfsp = 0;
              }
}

/******************************************************************************/
/*                   x r d _ F i l e T a b l e   C l a s s                    */
/******************************************************************************/
/******************************************************************************/
/*                            D e s t r u c t o r                             */
/******************************************************************************/
  
// WARNING! The object subject to this destructor must be serialized. There can
// be no active requests on link associated with this object at the time the
// destructor is called. The same restrictions apply to Add() and Del().
//
XrdXrootdFileTable::~XrdXrootdFileTable()
{
   int i;

// Delete all objects from the internal table (see warning)
//
   for (i = 0; i < XRD_FTABSIZE; i++) if (FTab[i]) Del(i);

// Delete all objects from the external table (see warning)
//
   if (XTab)
      {for (i = 0; i < XTnum; i++)    if (XTab[i]) Del(i+XRD_FTABSIZE);
       free(*XTab);
      }
}

/******************************************************************************/
/*                                   A d d                                    */
/******************************************************************************/
  
int XrdXrootdFileTable::Add(XrdXrootdFile *fp)
{
   const int allocsz = XRD_FTABSIZE*sizeof(fp);
   XrdXrootdFile **newXTab, **oldXTab;
   int i;

// Find a free spot in the internal table
//
   for (i = FTfree; i < XRD_FTABSIZE; i++) if (!FTab[i]) break;

   if (i < XRD_FTABSIZE)
      {FTab[i] = fp; FTfree = i+1; return i;}

// Allocate an external table if we do not have one
//
   if (!XTab)
      {if (!(XTab = (XrdXrootdFile **)malloc(allocsz))) return -1;
       memset((void *)XTab, 0, allocsz);
       XTnum   = XRD_FTABSIZE;
       XTfree  = 1;
       XTab[0] = fp;
       return XRD_FTABSIZE;
      }

// Find a free spot in the external table
//
   for (i = XTfree; i < XTnum; i++) if (!XTab[i]) break;
   if (i < XTnum)
      {XTab[i] = fp; XTfree = i+1; return i+XRD_FTABSIZE;}

// Extend the table
//
   if (!(newXTab = (XrdXrootdFile **)malloc(XTnum*sizeof(XrdXrootdFile *)+allocsz)))
      return -1;
   memcpy((void *)newXTab, (const void *)XTab, XTnum*sizeof(XrdXrootdFile *));
   memset((void *)(newXTab+XTnum), 0, allocsz);
   oldXTab = XTab;
   XTab = newXTab;
   XTab[XTnum] = fp;
   i = XTnum;
   XTfree = XTnum+1;
   XTnum += XRD_FTABSIZE;
   free(oldXTab);
   return i+XRD_FTABSIZE;
}
 
/******************************************************************************/
/*                                   D e l                                    */
/******************************************************************************/
  
void XrdXrootdFileTable::Del(int fnum)
{
   XrdXrootdFile *fp;

   if (fnum < XRD_FTABSIZE) 
      {fp = FTab[fnum];
       FTab[fnum] = 0;
       if (fnum < FTfree) FTfree = fnum;
      } else {
       fnum -= XRD_FTABSIZE;
       if (XTab && fnum < XTnum)
          {fp = XTab[fnum];
           XTab[fnum] = 0;
           if (fnum < XTfree) XTfree = fnum;
          }
           else fp = 0;
      }

   if (fp) delete fp;  // Will do the close
}

/******************************************************************************/
/*                       P r i v a t e   M e t h o d s                        */
/******************************************************************************/
/******************************************************************************/
/*                               b i n 2 h e x                                */
/******************************************************************************/
  
int XrdXrootdFile::bin2hex(char *outbuff, char *inbuff, int inlen)
{
    static char hv[] = "0123456789abcdef";
    int i, j = 0;

// Skip leading zeroes
//
    for (i = 0; i < inlen; i++) if (inbuff[i]) break;
    if (i >= inlen)
       {outbuff[0] = '0'; outbuff[1] = '\0'; return 1;}

// Format the data
//
    for (i = i; i < inlen; i++)
       {outbuff[j++] = hv[(inbuff[i] >> 4) & 0x0f];
        outbuff[j++] = hv[ inbuff[i]       & 0x0f];
       }
     outbuff[j] = '\0';
     return j;
}
