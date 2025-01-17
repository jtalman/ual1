/*
  Copyright (C) 2004 Ingo Berg

  Permission is hereby granted, free of charge, to any person obtaining a copy of this 
  software and associated documentation files (the "Software"), to deal in the Software
  without restriction, including without limitation the rights to use, copy, modify, 
  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
  permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or 
  substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/
#ifndef MU_PARSERDEF_H
#define MU_PARSERDEF_H


/** \brief Define the base datatype for values. 

  This datatype must be a built in value type. You can not use custom classes.
  It has been tested with float, double and long double types, int should 
  work as well.
*/
#define MU_PARSER_BASETYPE double
#define MU_PARSER_STRINGTYPE std::string

//------------------------------------------------------------------------------
//
// do not change anything beyond this point...
//
// !!! This section is devoted to macros that are used for debugging
// !!! or for features that are not fully implemented yet.
//
//#define MU_PARSER_DUMP_STACK
//#define MU_PARSER_DUMP_CMDCODE

namespace mu
{

//------------------------------------------------------------------------------
/** \brief Bytecode values.

    \attention The order of the operator entries must match the order in ParserBase::c_DefaultOprt!
*/
enum ECmdCode
{
  // The following are codes for built in binary operators
  // apart from built in operators the user has the opportunity to
  // add user defined operators.
  cmLE       = 0,   ///< Operator item:  less or equal
  cmGE       = 1,   ///< Operator item:  greater or equal
  cmNEQ      = 2,   ///< Operator item:  not equal
  cmEQ       = 3,   ///< Operator item:  equals
  cmLT       = 4,   ///< Operator item:  less than
  cmGT       = 5,   ///< Operator item:  greater than
  cmADD      = 6,   ///< Operator item:  add
  cmSUB      = 7,   ///< Operator item:  subtract
  cmMUL      = 8,   ///< Operator item:  multiply
  cmDIV      = 9,   ///< Operator item:  division
  cmPOW      = 10,  ///< Operator item:  y to the power of ...
  cmAND      = 11,  ///< Operator item:  logical and
  cmOR       = 12,  ///< Operator item:  logical or
  cmXOR      = 13,  ///< Operator item:  logical xor
  cmASSIGN   = 14,  ///< Operator item:  Assignment operator
  cmBO       = 15,  ///< Operator item:  opening bracket
  cmBC       = 16,  ///< Operator item:  closing bracket
  cmCOMMA    = 17,  ///< Operator item:  comma
  cmVAR      = 18,  ///< variable item
  cmSTRVAR   = 19,
  cmVAL      = 20,  ///< value item

  cmFUNC     = 21,  ///< Code for a function item
  cmFUNC_STR = 22,  ///< Code for a function with a string parameter

  cmSTRING   = 23,  ///< Code for a string token
  cmBINOP    = 24,  ///< user defined binary operator
  cmEND      = 25,  ///< end of formula 
  cmUNKNOWN  = 26,  ///< uninitialized item
  cmPOSTOP   = 27   /// NM ?
};

//------------------------------------------------------------------------------
enum ETypeCode
{
  tpSTR  = 0,
  tpDBL  = 1,
  tpVOID = 2
};

} // end fo namespace

#endif

