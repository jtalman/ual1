/*
  Copyright (C) 2004, 2005 Ingo Berg

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
#ifndef MU_PARSER_TEST_H
#define MU_PARSER_TEST_H

#include <string>
#include <numeric> // for accumulate
#include "muParser.h"
#include "muParserInt.h"


namespace mu
{

/** \brief Namespace for test cases. */
namespace Test
{

/** \brief Test cases for unit testing.

  (C) 2004 Ingo Berg
*/
class ParserTester // final
{
private:
    typedef Parser::value_type value_type;

    // Multiarg callbacks
	  static value_type f1of1(value_type v) { return v;};
  	
	  static value_type f1of2(value_type v, value_type  ) {return v;};
	  static value_type f2of2(value_type  , value_type v) {return v;};

	  static value_type f1of3(value_type v, value_type  , value_type  ) {return v;};
	  static value_type f2of3(value_type  , value_type v, value_type  ) {return v;};
	  static value_type f3of3(value_type  , value_type  , value_type v) {return v;};
  	
	  static value_type f1of4(value_type v, value_type,   value_type  , value_type  ) {return v;}
	  static value_type f2of4(value_type  , value_type v, value_type  , value_type  ) {return v;}
	  static value_type f3of4(value_type  , value_type,   value_type v, value_type  ) {return v;}
	  static value_type f4of4(value_type  , value_type,   value_type  , value_type v) {return v;}

	  static value_type f1of5(value_type v, value_type,   value_type  , value_type  , value_type  ) { return v; }
	  static value_type f2of5(value_type  , value_type v, value_type  , value_type  , value_type  ) { return v; }
	  static value_type f3of5(value_type  , value_type,   value_type v, value_type  , value_type  ) { return v; }
	  static value_type f4of5(value_type  , value_type,   value_type  , value_type v, value_type  ) { return v; }
	  static value_type f5of5(value_type  , value_type,   value_type  , value_type  , value_type v) { return v; }

    static value_type Min(value_type a_fVal1, value_type a_fVal2) { return (a_fVal1<a_fVal2) ? a_fVal1 : a_fVal2; }
  	static value_type Max(value_type a_fVal1, value_type a_fVal2) { return (a_fVal1>a_fVal2) ? a_fVal1 : a_fVal2; }

    static value_type plus2(value_type v1) { return v1+2; }
    static value_type times3(value_type v1) { return v1*3; }

    static value_type Sum(const value_type *a_afArg, int a_iArgc)
    { 
      if (!a_iArgc)	
        throw mu::Parser::exception_type("too few arguments for function sum.");

      value_type fRes=0;
      for (int i=0; i<a_iArgc; ++i) fRes += a_afArg[i];
      return fRes;
    }


    static value_type Rnd(Parser::value_type v)
    {
      return (Parser::value_type)(1+(v*std::rand()/(RAND_MAX+1.0)));
    }

    static value_type RndWithString(const char *)
    {
      return (Parser::value_type)(1+(1000.0f*std::rand()/(RAND_MAX+1.0)));
    }

    static value_type ValueOf(const char *)
    {
      return 123;
    }

    // postfix operator callback
	  static value_type Milli(value_type v) { return v/(value_type)1e3; }
    
    static int c_iCount;

	  int TestNames();
	  int TestSyntax();
	  int TestMultiArg();
	  int TestVolatile();
	  int TestPostFix();
	  int TestFormula();
	  int TestInfixOprt();
	  int TestBinOprt();
	  int TestVarConst();
	  int TestInterface();
	  int TestException();
    int TestStrArg();

    void Abort() const;

public:
    typedef int (ParserTester::*testfun_type)();

	  ParserTester();

    /** \brief Destructor (trivial). */
   ~ParserTester() {};
	  
    /** \brief Copy constructor is deactivated. */
    ParserTester(const ParserTester &a_Obj)
    :m_vTestFun()
    ,m_stream(a_Obj.m_stream)
    {};

	  void Run();
    void SetStream(std::ostream *a_stream);

private:
    std::vector<testfun_type> m_vTestFun;
    std::ostream *m_stream;

	  void AddTest(testfun_type a_pFun);

    // Test Double Parser
    int EqnTest(const std::string &a_str, double a_fRes, bool a_fPass);
    int ThrowTest(const std::string &a_str, int a_iErrc, bool a_bFail = true);

    // Test Int Parser
    int EqnTestInt(const std::string &a_str, double a_fRes, bool a_fPass);
};

} // namespace Test

} // namespace mu

#endif


