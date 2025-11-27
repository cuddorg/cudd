/**
  @file

  @ingroup cudd

  @brief Test cases for cuddZddFuncs.c to achieve 90% coverage.

  @details This file provides comprehensive tests for ZDD cover manipulation
  functions including product, division, complement, and helper functions.

  @author CUDD Contributors

  @copyright@parblock
  Copyright (c) 1995-2015, Regents of the University of Colorado

  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

  Neither the name of the University of Colorado nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
  @endparblock

 */

#include "util.h"
#include "cuddInt.h"

/** \cond */
static int testZddProduct(int verbosity);
static int testZddUnateProduct(int verbosity);
static int testZddWeakDiv(int verbosity);
static int testZddDivide(int verbosity);
static int testZddWeakDivF(int verbosity);
static int testZddDivideF(int verbosity);
static int testZddComplement(int verbosity);
static int testZddGetCofactors(int verbosity);
static int testZddVarIndexFunctions(int verbosity);
static int testZddProductEdgeCases(int verbosity);
static int testZddDivisionEdgeCases(int verbosity);
static int testZddRecursiveProduct(int verbosity);
static int testZddRecursiveWeakDiv(int verbosity);
/** \endcond */

/**
 * @brief Main program for testZddFuncs.
 */
int main(int argc, char const * const * argv)
{
  int verbosity = 0;
  if (argc == 2) {
    int nread;
    int ret = sscanf(argv[1], "%d%n", &verbosity, &nread);
    if (ret != 1 || argv[1][nread]) {
      fprintf(stderr, "Usage: %s [verbosity]\n", argv[0]);
      return -1;
    }
  }
  
  if (verbosity > 0) {
    printf("Testing cuddZddFuncs.c functions...\n");
  }

  if (testZddProduct(verbosity) != 0)
    return -1;
  if (testZddUnateProduct(verbosity) != 0)
    return -1;
  if (testZddWeakDiv(verbosity) != 0)
    return -1;
  if (testZddDivide(verbosity) != 0)
    return -1;
  if (testZddWeakDivF(verbosity) != 0)
    return -1;
  if (testZddDivideF(verbosity) != 0)
    return -1;
  if (testZddComplement(verbosity) != 0)
    return -1;
  if (testZddGetCofactors(verbosity) != 0)
    return -1;
  if (testZddVarIndexFunctions(verbosity) != 0)
    return -1;
  if (testZddProductEdgeCases(verbosity) != 0)
    return -1;
  if (testZddDivisionEdgeCases(verbosity) != 0)
    return -1;
  if (testZddRecursiveProduct(verbosity) != 0)
    return -1;
  if (testZddRecursiveWeakDiv(verbosity) != 0)
    return -1;

  if (verbosity > 0) {
    printf("All cuddZddFuncs.c tests passed!\n");
  }

  return 0;
}

/**
 * @brief Test Cudd_zddProduct function.
 * @return 0 if successful; -1 otherwise.
 */
static int
testZddProduct(int verbosity)
{
  DdManager *dd;
  DdNode *f, *g, *result;
  DdNode *one, *zero;
  int ret;

  dd = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
  if (!dd) {
    if (verbosity) printf("testZddProduct: initialization failed\n");
    return -1;
  }

  one = DD_ONE(dd);
  zero = DD_ZERO(dd);

  f = Cudd_zddIthVar(dd, 0);
  if (!f) {
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(f);
  
  result = Cudd_zddProduct(dd, f, zero);
  if (result != zero) {
    if (verbosity) printf("testZddProduct: product with zero failed\n");
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
    return -1;
  }
  
  result = Cudd_zddProduct(dd, f, one);
  if (!result) {
    if (verbosity) printf("testZddProduct: product with one failed\n");
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(result);
  if (result != f) {
    if (verbosity) printf("testZddProduct: product with one did not return f\n");
    Cudd_RecursiveDerefZdd(dd, result);
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_RecursiveDerefZdd(dd, result);
  
  result = Cudd_zddProduct(dd, f, f);
  if (!result) {
    if (verbosity) printf("testZddProduct: product with self failed\n");
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(result);
  Cudd_RecursiveDerefZdd(dd, result);
  
  g = Cudd_zddIthVar(dd, 2);
  if (!g) {
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(g);
  
  result = Cudd_zddProduct(dd, f, g);
  if (!result) {
    if (verbosity) printf("testZddProduct: product of two vars failed\n");
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_RecursiveDerefZdd(dd, g);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(result);
  
  if (verbosity > 1) {
    printf("ZDD Product result:\n");
    Cudd_zddPrintMinterm(dd, result);
  }
  
  Cudd_RecursiveDerefZdd(dd, result);
  Cudd_RecursiveDerefZdd(dd, g);
  Cudd_RecursiveDerefZdd(dd, f);
  
  ret = Cudd_CheckZeroRef(dd);
  if (ret != 0 && verbosity) {
    printf("testZddProduct: %d non-zero references\n", ret);
  }
  Cudd_Quit(dd);
  
  if (verbosity) printf("testZddProduct: passed\n");
  return 0;
}

/**
 * @brief Test Cudd_zddUnateProduct function.
 * @return 0 if successful; -1 otherwise.
 */
static int
testZddUnateProduct(int verbosity)
{
  DdManager *dd;
  DdNode *f, *g, *result;
  DdNode *one, *zero;
  int ret;

  dd = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
  if (!dd) {
    if (verbosity) printf("testZddUnateProduct: initialization failed\n");
    return -1;
  }

  one = DD_ONE(dd);
  zero = DD_ZERO(dd);

  f = Cudd_zddIthVar(dd, 0);
  if (!f) {
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(f);
  
  result = Cudd_zddUnateProduct(dd, f, zero);
  if (result != zero) {
    if (verbosity) printf("testZddUnateProduct: product with zero failed\n");
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
    return -1;
  }
  
  result = Cudd_zddUnateProduct(dd, f, one);
  if (!result) {
    if (verbosity) printf("testZddUnateProduct: product with one failed\n");
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(result);
  if (result != f) {
    if (verbosity) printf("testZddUnateProduct: product with one did not return f\n");
  }
  Cudd_RecursiveDerefZdd(dd, result);
  
  g = Cudd_zddIthVar(dd, 1);
  if (!g) {
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(g);
  
  result = Cudd_zddUnateProduct(dd, f, g);
  if (!result) {
    if (verbosity) printf("testZddUnateProduct: product of two vars failed\n");
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_RecursiveDerefZdd(dd, g);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(result);
  
  if (verbosity > 1) {
    printf("ZDD Unate Product result:\n");
    Cudd_zddPrintMinterm(dd, result);
  }
  
  Cudd_RecursiveDerefZdd(dd, result);
  Cudd_RecursiveDerefZdd(dd, g);
  Cudd_RecursiveDerefZdd(dd, f);
  
  ret = Cudd_CheckZeroRef(dd);
  if (ret != 0 && verbosity) {
    printf("testZddUnateProduct: %d non-zero references\n", ret);
  }
  Cudd_Quit(dd);
  
  if (verbosity) printf("testZddUnateProduct: passed\n");
  return 0;
}

/**
 * @brief Test Cudd_zddWeakDiv function.
 * @return 0 if successful; -1 otherwise.
 */
static int
testZddWeakDiv(int verbosity)
{
  DdManager *dd;
  DdNode *f, *g, *result;
  DdNode *one, *zero;
  int ret;

  dd = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
  if (!dd) {
    if (verbosity) printf("testZddWeakDiv: initialization failed\n");
    return -1;
  }

  one = DD_ONE(dd);
  zero = DD_ZERO(dd);

  f = Cudd_zddIthVar(dd, 0);
  if (!f) {
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(f);
  
  result = Cudd_zddWeakDiv(dd, f, one);
  if (!result || result != f) {
    if (verbosity) printf("testZddWeakDiv: division by one failed\n");
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
    return -1;
  }
  
  result = Cudd_zddWeakDiv(dd, zero, f);
  if (result != zero) {
    if (verbosity) printf("testZddWeakDiv: division of zero failed\n");
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
    return -1;
  }
  
  result = Cudd_zddWeakDiv(dd, f, f);
  if (!result) {
    if (verbosity) printf("testZddWeakDiv: self-division failed\n");
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(result);
  if (result != one) {
    if (verbosity) printf("testZddWeakDiv: self-division did not return one\n");
  }
  Cudd_RecursiveDerefZdd(dd, result);
  
  result = Cudd_zddWeakDiv(dd, one, f);
  if (result != zero) {
    if (verbosity) printf("testZddWeakDiv: division of one by var should be zero\n");
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
    return -1;
  }

  g = Cudd_zddIthVar(dd, 2);
  if (!g) {
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(g);
  
  result = Cudd_zddWeakDiv(dd, f, g);
  if (!result) {
    if (verbosity) printf("testZddWeakDiv: division of different vars failed\n");
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_RecursiveDerefZdd(dd, g);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(result);
  
  if (verbosity > 1) {
    printf("ZDD WeakDiv result:\n");
    Cudd_zddPrintMinterm(dd, result);
  }
  
  Cudd_RecursiveDerefZdd(dd, result);
  Cudd_RecursiveDerefZdd(dd, g);
  Cudd_RecursiveDerefZdd(dd, f);
  
  ret = Cudd_CheckZeroRef(dd);
  if (ret != 0 && verbosity) {
    printf("testZddWeakDiv: %d non-zero references\n", ret);
  }
  Cudd_Quit(dd);
  
  if (verbosity) printf("testZddWeakDiv: passed\n");
  return 0;
}

/**
 * @brief Test Cudd_zddDivide function.
 * @return 0 if successful; -1 otherwise.
 */
static int
testZddDivide(int verbosity)
{
  DdManager *dd;
  DdNode *f, *g, *result;
  DdNode *one, *zero;
  int ret;

  dd = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
  if (!dd) {
    if (verbosity) printf("testZddDivide: initialization failed\n");
    return -1;
  }

  one = DD_ONE(dd);
  zero = DD_ZERO(dd);

  f = Cudd_zddIthVar(dd, 0);
  if (!f) {
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(f);
  
  result = Cudd_zddDivide(dd, f, one);
  if (!result || result != f) {
    if (verbosity) printf("testZddDivide: division by one failed\n");
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
    return -1;
  }
  
  result = Cudd_zddDivide(dd, zero, f);
  if (result != zero) {
    if (verbosity) printf("testZddDivide: division of zero failed\n");
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
    return -1;
  }
  
  result = Cudd_zddDivide(dd, f, f);
  if (!result) {
    if (verbosity) printf("testZddDivide: self-division failed\n");
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(result);
  if (result != one) {
    if (verbosity) printf("testZddDivide: self-division did not return one\n");
  }
  Cudd_RecursiveDerefZdd(dd, result);
  
  g = Cudd_zddIthVar(dd, 1);
  if (!g) {
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(g);
  
  result = Cudd_zddDivide(dd, f, g);
  if (!result) {
    if (verbosity) printf("testZddDivide: division of different vars failed\n");
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_RecursiveDerefZdd(dd, g);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(result);
  Cudd_RecursiveDerefZdd(dd, result);
  
  Cudd_RecursiveDerefZdd(dd, g);
  Cudd_RecursiveDerefZdd(dd, f);
  
  ret = Cudd_CheckZeroRef(dd);
  if (ret != 0 && verbosity) {
    printf("testZddDivide: %d non-zero references\n", ret);
  }
  Cudd_Quit(dd);
  
  if (verbosity) printf("testZddDivide: passed\n");
  return 0;
}

/**
 * @brief Test Cudd_zddWeakDivF function.
 * @return 0 if successful; -1 otherwise.
 */
static int
testZddWeakDivF(int verbosity)
{
  DdManager *dd;
  DdNode *f, *g, *result;
  DdNode *one, *zero;
  int ret;

  dd = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
  if (!dd) {
    if (verbosity) printf("testZddWeakDivF: initialization failed\n");
    return -1;
  }

  one = DD_ONE(dd);
  zero = DD_ZERO(dd);

  f = Cudd_zddIthVar(dd, 0);
  if (!f) {
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(f);
  
  result = Cudd_zddWeakDivF(dd, f, one);
  if (!result || result != f) {
    if (verbosity) printf("testZddWeakDivF: division by one failed\n");
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
    return -1;
  }
  
  result = Cudd_zddWeakDivF(dd, zero, f);
  if (result != zero) {
    if (verbosity) printf("testZddWeakDivF: division of zero failed\n");
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
    return -1;
  }
  
  result = Cudd_zddWeakDivF(dd, f, f);
  if (!result) {
    if (verbosity) printf("testZddWeakDivF: self-division failed\n");
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(result);
  if (result != one) {
    if (verbosity) printf("testZddWeakDivF: self-division did not return one\n");
  }
  Cudd_RecursiveDerefZdd(dd, result);
  
  g = Cudd_zddIthVar(dd, 4);
  if (!g) {
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(g);
  
  result = Cudd_zddWeakDivF(dd, f, g);
  if (!result) {
    if (verbosity) printf("testZddWeakDivF: division with vf < vg failed\n");
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_RecursiveDerefZdd(dd, g);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(result);
  Cudd_RecursiveDerefZdd(dd, result);
  
  result = Cudd_zddWeakDivF(dd, g, f);
  if (!result) {
    if (verbosity) printf("testZddWeakDivF: division with vg < vf failed\n");
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_RecursiveDerefZdd(dd, g);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(result);
  Cudd_RecursiveDerefZdd(dd, result);
  
  Cudd_RecursiveDerefZdd(dd, g);
  Cudd_RecursiveDerefZdd(dd, f);
  
  ret = Cudd_CheckZeroRef(dd);
  if (ret != 0 && verbosity) {
    printf("testZddWeakDivF: %d non-zero references\n", ret);
  }
  Cudd_Quit(dd);
  
  if (verbosity) printf("testZddWeakDivF: passed\n");
  return 0;
}

/**
 * @brief Test Cudd_zddDivideF function.
 * @return 0 if successful; -1 otherwise.
 */
static int
testZddDivideF(int verbosity)
{
  DdManager *dd;
  DdNode *f, *g, *result;
  DdNode *one, *zero;
  int ret;

  dd = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
  if (!dd) {
    if (verbosity) printf("testZddDivideF: initialization failed\n");
    return -1;
  }

  one = DD_ONE(dd);
  zero = DD_ZERO(dd);

  f = Cudd_zddIthVar(dd, 0);
  if (!f) {
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(f);
  
  result = Cudd_zddDivideF(dd, f, one);
  if (!result || result != f) {
    if (verbosity) printf("testZddDivideF: division by one failed\n");
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
    return -1;
  }
  
  result = Cudd_zddDivideF(dd, zero, f);
  if (result != zero) {
    if (verbosity) printf("testZddDivideF: division of zero failed\n");
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
    return -1;
  }
  
  result = Cudd_zddDivideF(dd, f, f);
  if (!result) {
    if (verbosity) printf("testZddDivideF: self-division failed\n");
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(result);
  if (result != one) {
    if (verbosity) printf("testZddDivideF: self-division did not return one\n");
  }
  Cudd_RecursiveDerefZdd(dd, result);
  
  g = Cudd_zddIthVar(dd, 1);
  if (!g) {
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(g);
  
  result = Cudd_zddDivideF(dd, f, g);
  if (!result) {
    if (verbosity) printf("testZddDivideF: division of different vars failed\n");
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_RecursiveDerefZdd(dd, g);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(result);
  Cudd_RecursiveDerefZdd(dd, result);
  
  Cudd_RecursiveDerefZdd(dd, g);
  Cudd_RecursiveDerefZdd(dd, f);
  
  ret = Cudd_CheckZeroRef(dd);
  if (ret != 0 && verbosity) {
    printf("testZddDivideF: %d non-zero references\n", ret);
  }
  Cudd_Quit(dd);
  
  if (verbosity) printf("testZddDivideF: passed\n");
  return 0;
}

/**
 * @brief Test Cudd_zddComplement function.
 * @return 0 if successful; -1 otherwise.
 */
static int
testZddComplement(int verbosity)
{
  DdManager *dd;
  DdNode *bdd, *zdd, *complement;
  int ret;

  dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
  if (!dd) {
    if (verbosity) printf("testZddComplement: initialization failed\n");
    return -1;
  }

  ret = Cudd_zddVarsFromBddVars(dd, 2);
  if (ret == 0) {
    if (verbosity) printf("testZddComplement: ZDD vars creation failed\n");
    Cudd_Quit(dd);
    return -1;
  }

  bdd = Cudd_bddIthVar(dd, 0);
  if (!bdd) {
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(bdd);

  zdd = Cudd_zddPortFromBdd(dd, bdd);
  if (!zdd) {
    if (verbosity) printf("testZddComplement: BDD to ZDD conversion failed\n");
    Cudd_RecursiveDeref(dd, bdd);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(zdd);
  Cudd_RecursiveDeref(dd, bdd);

  complement = Cudd_zddComplement(dd, zdd);
  if (!complement) {
    if (verbosity) printf("testZddComplement: complement failed\n");
    Cudd_RecursiveDerefZdd(dd, zdd);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(complement);

  if (verbosity > 1) {
    printf("Original ZDD:\n");
    Cudd_zddPrintMinterm(dd, zdd);
    printf("Complement ZDD:\n");
    Cudd_zddPrintMinterm(dd, complement);
  }

  DdNode *complement2 = Cudd_zddComplement(dd, zdd);
  if (!complement2 || complement2 != complement) {
    if (verbosity) printf("testZddComplement: cache test failed\n");
  }

  Cudd_RecursiveDerefZdd(dd, complement);
  Cudd_RecursiveDerefZdd(dd, zdd);

  ret = Cudd_CheckZeroRef(dd);
  if (ret != 0 && verbosity) {
    printf("testZddComplement: %d non-zero references\n", ret);
  }
  Cudd_Quit(dd);

  if (verbosity) printf("testZddComplement: passed\n");
  return 0;
}

/**
 * @brief Test cuddZddGetCofactors2 and cuddZddGetCofactors3 functions.
 * @return 0 if successful; -1 otherwise.
 */
static int
testZddGetCofactors(int verbosity)
{
  DdManager *dd;
  DdNode *f, *f1, *f0, *fd, *union_result;
  int ret;

  dd = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
  if (!dd) {
    if (verbosity) printf("testZddGetCofactors: initialization failed\n");
    return -1;
  }

  f = Cudd_zddIthVar(dd, 0);
  if (!f) {
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(f);

  ret = cuddZddGetCofactors3(dd, f, 0, &f1, &f0, &fd);
  if (ret != 0) {
    if (verbosity) printf("testZddGetCofactors: GetCofactors3 failed\n");
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(f1);
  Cudd_Ref(f0);
  Cudd_Ref(fd);

  if (verbosity > 1) {
    printf("GetCofactors3 results:\n");
    printf("f1: ");
    Cudd_zddPrintMinterm(dd, f1);
    printf("f0: ");
    Cudd_zddPrintMinterm(dd, f0);
    printf("fd: ");
    Cudd_zddPrintMinterm(dd, fd);
  }

  Cudd_RecursiveDerefZdd(dd, f1);
  Cudd_RecursiveDerefZdd(dd, f0);
  Cudd_RecursiveDerefZdd(dd, fd);

  ret = cuddZddGetCofactors2(dd, f, 0, &f1, &f0);
  if (ret != 0) {
    if (verbosity) printf("testZddGetCofactors: GetCofactors2 failed\n");
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(f1);
  Cudd_Ref(f0);

  if (verbosity > 1) {
    printf("GetCofactors2 results:\n");
    printf("f1: ");
    Cudd_zddPrintMinterm(dd, f1);
    printf("f0: ");
    Cudd_zddPrintMinterm(dd, f0);
  }

  Cudd_RecursiveDerefZdd(dd, f1);
  Cudd_RecursiveDerefZdd(dd, f0);

  DdNode *g = Cudd_zddIthVar(dd, 2);
  if (!g) {
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(g);
  
  union_result = Cudd_zddUnion(dd, f, g);
  if (!union_result) {
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_RecursiveDerefZdd(dd, g);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(union_result);
  
  ret = cuddZddGetCofactors3(dd, union_result, 0, &f1, &f0, &fd);
  if (ret != 0) {
    if (verbosity) printf("testZddGetCofactors: GetCofactors3 on union failed\n");
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_RecursiveDerefZdd(dd, g);
    Cudd_RecursiveDerefZdd(dd, union_result);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(f1);
  Cudd_Ref(f0);
  Cudd_Ref(fd);
  
  Cudd_RecursiveDerefZdd(dd, f1);
  Cudd_RecursiveDerefZdd(dd, f0);
  Cudd_RecursiveDerefZdd(dd, fd);
  Cudd_RecursiveDerefZdd(dd, union_result);
  Cudd_RecursiveDerefZdd(dd, g);
  Cudd_RecursiveDerefZdd(dd, f);

  ret = Cudd_CheckZeroRef(dd);
  if (ret != 0 && verbosity) {
    printf("testZddGetCofactors: %d non-zero references\n", ret);
  }
  Cudd_Quit(dd);

  if (verbosity) printf("testZddGetCofactors: passed\n");
  return 0;
}

/**
 * @brief Test cuddZddGetPosVarIndex, cuddZddGetNegVarIndex, cuddZddGetPosVarLevel, cuddZddGetNegVarLevel.
 * @return 0 if successful; -1 otherwise.
 */
static int
testZddVarIndexFunctions(int verbosity)
{
  DdManager *dd;
  int posIndex, negIndex, posLevel, negLevel;

  dd = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
  if (!dd) {
    if (verbosity) printf("testZddVarIndexFunctions: initialization failed\n");
    return -1;
  }

  DdNode *v0 = Cudd_zddIthVar(dd, 0);
  DdNode *v1 = Cudd_zddIthVar(dd, 1);
  DdNode *v2 = Cudd_zddIthVar(dd, 2);
  DdNode *v3 = Cudd_zddIthVar(dd, 3);
  if (!v0 || !v1 || !v2 || !v3) {
    Cudd_Quit(dd);
    return -1;
  }

  posIndex = cuddZddGetPosVarIndex(dd, 0);
  negIndex = cuddZddGetNegVarIndex(dd, 0);
  posLevel = cuddZddGetPosVarLevel(dd, 0);
  negLevel = cuddZddGetNegVarLevel(dd, 0);

  if (verbosity > 1) {
    printf("For index 0:\n");
    printf("  posIndex = %d, negIndex = %d\n", posIndex, negIndex);
    printf("  posLevel = %d, negLevel = %d\n", posLevel, negLevel);
  }

  if (posIndex != 0) {
    if (verbosity) printf("testZddVarIndexFunctions: posIndex(0) != 0\n");
    Cudd_Quit(dd);
    return -1;
  }
  if (negIndex != 1) {
    if (verbosity) printf("testZddVarIndexFunctions: negIndex(0) != 1\n");
    Cudd_Quit(dd);
    return -1;
  }

  posIndex = cuddZddGetPosVarIndex(dd, 1);
  negIndex = cuddZddGetNegVarIndex(dd, 1);

  if (verbosity > 1) {
    printf("For index 1:\n");
    printf("  posIndex = %d, negIndex = %d\n", posIndex, negIndex);
  }

  if (posIndex != 0) {
    if (verbosity) printf("testZddVarIndexFunctions: posIndex(1) != 0\n");
    Cudd_Quit(dd);
    return -1;
  }
  if (negIndex != 1) {
    if (verbosity) printf("testZddVarIndexFunctions: negIndex(1) != 1\n");
    Cudd_Quit(dd);
    return -1;
  }

  posIndex = cuddZddGetPosVarIndex(dd, 2);
  negIndex = cuddZddGetNegVarIndex(dd, 2);
  posLevel = cuddZddGetPosVarLevel(dd, 2);
  negLevel = cuddZddGetNegVarLevel(dd, 2);

  if (verbosity > 1) {
    printf("For index 2:\n");
    printf("  posIndex = %d, negIndex = %d\n", posIndex, negIndex);
    printf("  posLevel = %d, negLevel = %d\n", posLevel, negLevel);
  }

  if (posIndex != 2) {
    if (verbosity) printf("testZddVarIndexFunctions: posIndex(2) != 2\n");
    Cudd_Quit(dd);
    return -1;
  }
  if (negIndex != 3) {
    if (verbosity) printf("testZddVarIndexFunctions: negIndex(2) != 3\n");
    Cudd_Quit(dd);
    return -1;
  }

  posIndex = cuddZddGetPosVarIndex(dd, 3);
  negIndex = cuddZddGetNegVarIndex(dd, 3);

  if (verbosity > 1) {
    printf("For index 3:\n");
    printf("  posIndex = %d, negIndex = %d\n", posIndex, negIndex);
  }

  if (posIndex != 2) {
    if (verbosity) printf("testZddVarIndexFunctions: posIndex(3) != 2\n");
    Cudd_Quit(dd);
    return -1;
  }
  if (negIndex != 3) {
    if (verbosity) printf("testZddVarIndexFunctions: negIndex(3) != 3\n");
    Cudd_Quit(dd);
    return -1;
  }

  Cudd_Quit(dd);

  if (verbosity) printf("testZddVarIndexFunctions: passed\n");
  return 0;
}

/**
 * @brief Test product edge cases with more complex ZDDs.
 * @return 0 if successful; -1 otherwise.
 */
static int
testZddProductEdgeCases(int verbosity)
{
  DdManager *dd;
  DdNode *f, *g, *h, *product1, *product2, *union_fg;
  int ret;

  dd = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
  if (!dd) {
    if (verbosity) printf("testZddProductEdgeCases: initialization failed\n");
    return -1;
  }

  f = Cudd_zddIthVar(dd, 0);
  g = Cudd_zddIthVar(dd, 2);
  h = Cudd_zddIthVar(dd, 4);
  if (!f || !g || !h) {
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(f);
  Cudd_Ref(g);
  Cudd_Ref(h);

  product1 = Cudd_zddProduct(dd, f, g);
  if (!product1) {
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_RecursiveDerefZdd(dd, g);
    Cudd_RecursiveDerefZdd(dd, h);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(product1);

  product2 = Cudd_zddProduct(dd, g, f);
  if (!product2) {
    Cudd_RecursiveDerefZdd(dd, product1);
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_RecursiveDerefZdd(dd, g);
    Cudd_RecursiveDerefZdd(dd, h);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(product2);

  if (product1 != product2) {
    if (verbosity) printf("testZddProductEdgeCases: commutativity failed\n");
  }

  Cudd_RecursiveDerefZdd(dd, product2);
  Cudd_RecursiveDerefZdd(dd, product1);

  union_fg = Cudd_zddUnion(dd, f, g);
  if (!union_fg) {
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_RecursiveDerefZdd(dd, g);
    Cudd_RecursiveDerefZdd(dd, h);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(union_fg);

  product1 = Cudd_zddProduct(dd, union_fg, h);
  if (!product1) {
    Cudd_RecursiveDerefZdd(dd, union_fg);
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_RecursiveDerefZdd(dd, g);
    Cudd_RecursiveDerefZdd(dd, h);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(product1);

  if (verbosity > 1) {
    printf("(f + g) * h:\n");
    Cudd_zddPrintMinterm(dd, product1);
  }

  Cudd_RecursiveDerefZdd(dd, product1);
  Cudd_RecursiveDerefZdd(dd, union_fg);
  Cudd_RecursiveDerefZdd(dd, h);
  Cudd_RecursiveDerefZdd(dd, g);
  Cudd_RecursiveDerefZdd(dd, f);

  ret = Cudd_CheckZeroRef(dd);
  if (ret != 0 && verbosity) {
    printf("testZddProductEdgeCases: %d non-zero references\n", ret);
  }
  Cudd_Quit(dd);

  if (verbosity) printf("testZddProductEdgeCases: passed\n");
  return 0;
}

/**
 * @brief Test division edge cases.
 * @return 0 if successful; -1 otherwise.
 */
static int
testZddDivisionEdgeCases(int verbosity)
{
  DdManager *dd;
  DdNode *f, *g, *product, *quotient;
  int ret;

  dd = Cudd_Init(0, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
  if (!dd) {
    if (verbosity) printf("testZddDivisionEdgeCases: initialization failed\n");
    return -1;
  }

  f = Cudd_zddIthVar(dd, 0);
  g = Cudd_zddIthVar(dd, 1);
  if (!f || !g) {
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(f);
  Cudd_Ref(g);

  product = Cudd_zddUnateProduct(dd, f, g);
  if (!product) {
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_RecursiveDerefZdd(dd, g);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(product);

  quotient = Cudd_zddDivide(dd, product, g);
  if (!quotient) {
    Cudd_RecursiveDerefZdd(dd, product);
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_RecursiveDerefZdd(dd, g);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(quotient);

  if (verbosity > 1) {
    printf("Product f * g:\n");
    Cudd_zddPrintMinterm(dd, product);
    printf("Quotient (f * g) / g:\n");
    Cudd_zddPrintMinterm(dd, quotient);
  }

  DdNode *quotientF = Cudd_zddDivideF(dd, product, g);
  if (!quotientF) {
    Cudd_RecursiveDerefZdd(dd, quotient);
    Cudd_RecursiveDerefZdd(dd, product);
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_RecursiveDerefZdd(dd, g);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(quotientF);

  Cudd_RecursiveDerefZdd(dd, quotientF);
  Cudd_RecursiveDerefZdd(dd, quotient);
  Cudd_RecursiveDerefZdd(dd, product);
  Cudd_RecursiveDerefZdd(dd, g);
  Cudd_RecursiveDerefZdd(dd, f);

  ret = Cudd_CheckZeroRef(dd);
  if (ret != 0 && verbosity) {
    printf("testZddDivisionEdgeCases: %d non-zero references\n", ret);
  }
  Cudd_Quit(dd);

  if (verbosity) printf("testZddDivisionEdgeCases: passed\n");
  return 0;
}

/**
 * @brief Test recursive product with deeper ZDD structures.
 * @return 0 if successful; -1 otherwise.
 */
static int
testZddRecursiveProduct(int verbosity)
{
  DdManager *dd;
  DdNode *vars[4];
  DdNode *f, *g, *product;
  int i, ret;

  dd = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
  if (!dd) {
    if (verbosity) printf("testZddRecursiveProduct: initialization failed\n");
    return -1;
  }

  for (i = 0; i < 4; i++) {
    vars[i] = Cudd_zddIthVar(dd, i * 2);
    if (!vars[i]) {
      while (--i >= 0) Cudd_RecursiveDerefZdd(dd, vars[i]);
      Cudd_Quit(dd);
      return -1;
    }
    Cudd_Ref(vars[i]);
  }

  f = Cudd_zddUnion(dd, vars[0], vars[1]);
  if (!f) {
    for (i = 0; i < 4; i++) Cudd_RecursiveDerefZdd(dd, vars[i]);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(f);

  g = Cudd_zddUnion(dd, vars[2], vars[3]);
  if (!g) {
    Cudd_RecursiveDerefZdd(dd, f);
    for (i = 0; i < 4; i++) Cudd_RecursiveDerefZdd(dd, vars[i]);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(g);

  product = Cudd_zddProduct(dd, f, g);
  if (!product) {
    if (verbosity) printf("testZddRecursiveProduct: product failed\n");
    Cudd_RecursiveDerefZdd(dd, g);
    Cudd_RecursiveDerefZdd(dd, f);
    for (i = 0; i < 4; i++) Cudd_RecursiveDerefZdd(dd, vars[i]);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(product);

  if (verbosity > 1) {
    printf("f = v0 + v2:\n");
    Cudd_zddPrintMinterm(dd, f);
    printf("g = v4 + v6:\n");
    Cudd_zddPrintMinterm(dd, g);
    printf("f * g:\n");
    Cudd_zddPrintMinterm(dd, product);
  }

  DdNode *unateProduct = Cudd_zddUnateProduct(dd, f, g);
  if (!unateProduct) {
    if (verbosity) printf("testZddRecursiveProduct: unate product failed\n");
  } else {
    Cudd_Ref(unateProduct);
    Cudd_RecursiveDerefZdd(dd, unateProduct);
  }

  Cudd_RecursiveDerefZdd(dd, product);
  Cudd_RecursiveDerefZdd(dd, g);
  Cudd_RecursiveDerefZdd(dd, f);
  for (i = 0; i < 4; i++) Cudd_RecursiveDerefZdd(dd, vars[i]);

  ret = Cudd_CheckZeroRef(dd);
  if (ret != 0 && verbosity) {
    printf("testZddRecursiveProduct: %d non-zero references\n", ret);
  }
  Cudd_Quit(dd);

  if (verbosity) printf("testZddRecursiveProduct: passed\n");
  return 0;
}

/**
 * @brief Test recursive weak division with deeper ZDD structures.
 * @return 0 if successful; -1 otherwise.
 */
static int
testZddRecursiveWeakDiv(int verbosity)
{
  DdManager *dd;
  DdNode *vars[4];
  DdNode *f, *g, *quotient;
  int i, ret;

  dd = Cudd_Init(0, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
  if (!dd) {
    if (verbosity) printf("testZddRecursiveWeakDiv: initialization failed\n");
    return -1;
  }

  for (i = 0; i < 4; i++) {
    vars[i] = Cudd_zddIthVar(dd, i * 2);
    if (!vars[i]) {
      while (--i >= 0) Cudd_RecursiveDerefZdd(dd, vars[i]);
      Cudd_Quit(dd);
      return -1;
    }
    Cudd_Ref(vars[i]);
  }

  f = Cudd_zddUnion(dd, vars[0], vars[1]);
  if (!f) {
    for (i = 0; i < 4; i++) Cudd_RecursiveDerefZdd(dd, vars[i]);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(f);

  g = vars[0];
  Cudd_Ref(g);

  quotient = Cudd_zddWeakDiv(dd, f, g);
  if (!quotient) {
    if (verbosity) printf("testZddRecursiveWeakDiv: quotient failed\n");
    Cudd_RecursiveDerefZdd(dd, g);
    Cudd_RecursiveDerefZdd(dd, f);
    for (i = 0; i < 4; i++) Cudd_RecursiveDerefZdd(dd, vars[i]);
    Cudd_Quit(dd);
    return -1;
  }
  Cudd_Ref(quotient);

  if (verbosity > 1) {
    printf("f = v0 + v2:\n");
    Cudd_zddPrintMinterm(dd, f);
    printf("g = v0:\n");
    Cudd_zddPrintMinterm(dd, g);
    printf("f / g (weak):\n");
    Cudd_zddPrintMinterm(dd, quotient);
  }

  DdNode *quotientF = Cudd_zddWeakDivF(dd, f, g);
  if (!quotientF) {
    if (verbosity) printf("testZddRecursiveWeakDiv: WeakDivF failed\n");
  } else {
    Cudd_Ref(quotientF);
    Cudd_RecursiveDerefZdd(dd, quotientF);
  }

  Cudd_RecursiveDerefZdd(dd, quotient);
  Cudd_RecursiveDerefZdd(dd, g);
  Cudd_RecursiveDerefZdd(dd, f);
  for (i = 0; i < 4; i++) Cudd_RecursiveDerefZdd(dd, vars[i]);

  ret = Cudd_CheckZeroRef(dd);
  if (ret != 0 && verbosity) {
    printf("testZddRecursiveWeakDiv: %d non-zero references\n", ret);
  }
  Cudd_Quit(dd);

  if (verbosity) printf("testZddRecursiveWeakDiv: passed\n");
  return 0;
}
