#ifndef __OUC_ENV__
#define __OUC_ENV__
/******************************************************************************/
/*                                                                            */
/*                          X r d O u c E n v . h h                           */
/*                                                                            */
/* (c) 2003 by the Board of Trustees of the Leland Stanford, Jr., University  */
/*                            All Rights Reserved                             */
/*   Produced by Andrew Hanushevsky for Stanford University under contract    */
/*              DE-AC03-76-SFO0515 with the Department of Energy              */
/******************************************************************************/
  
//         $Id: XrdOucEnv.hh,v 1.3 2004/09/15 15:54:02 heinz Exp $

#include <stdlib.h>
#include <strings.h>
#include "XrdOuc/XrdOucHash.hh"

class XrdOucEnv
{
public:

// Env() returns the full environment string and length passed to the
//       constructor.
//
inline char *Env(int &envlen) {envlen = global_len; return global_env;}

// Get() returns the address of the string associated with the variable
//       name. If no association exists, zero is returned.
//
       char *Get(const char *varname) {return env_Hash.Find(varname);}

// GetInt() returns a long integer value. If the variable varname is not found
//           in the hash table, return -999999999.       
//
       long  GetInt(const char *varname);

// Put() associates a string value with the a variable name. If one already
//       exists, it is replaced. The passed value and variable strings are
//       duplicated (value here, variable by env_Hash).
//
       void  Put(const char *varname, const char *value)
                {env_Hash.Rep((char *)varname, strdup(value), 0, Hash_dofree);}

// PutInt() puts a long integer value into the hash. Internally, the value gets
//          converted into a char*
//
       void  PutInt(const char *varname, long value);

// Delimit() search for the first occurrence of comma (',') in value and
//           replaces it with a null byte. It then returns the address of the
//           remaining string. If no comma was found, it returns zero.
//
       char *Delimit(char *value);

// Use the constructor to define the initial variable settings. The passed
// string is duplicated and the copy can be retrieved using Env().
//
       XrdOucEnv(const char *vardata=0, int vardlen=0);

      ~XrdOucEnv() {if (global_env) free((void *)global_env);}

private:

XrdOucHash<char> env_Hash;
char *global_env;
int   global_len;
};
#endif
