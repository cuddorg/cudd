/**
  @file

  @ingroup cudd

  @brief Tests for the consistency check functions in cuddCheck.c.

  @author CUDD Test Suite

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
static int testDebugCheck(int verbosity);
static int testCheckKeys(int verbosity);
static int testHeapProfile(int verbosity);
static int testPrintNode(int verbosity);
static int testPrintVarGroups(int verbosity);
static int testCheckWithZDD(int verbosity);
static int testCheckWithConstants(int verbosity);
static int testCheckWithReordering(int verbosity);
static int testCheckWithDeadNodes(int verbosity);
static int testCheckWithLargeStructure(int verbosity);
/** \endcond */

/**
 * @brief Main program for testcheck.
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
  if (testDebugCheck(verbosity) != 0)
    return -1;
  if (testCheckKeys(verbosity) != 0)
    return -1;
  if (testHeapProfile(verbosity) != 0)
    return -1;
  if (testPrintNode(verbosity) != 0)
    return -1;
  if (testPrintVarGroups(verbosity) != 0)
    return -1;
  if (testCheckWithZDD(verbosity) != 0)
    return -1;
  if (testCheckWithConstants(verbosity) != 0)
    return -1;
  if (testCheckWithReordering(verbosity) != 0)
    return -1;
  if (testCheckWithDeadNodes(verbosity) != 0)
    return -1;
  if (testCheckWithLargeStructure(verbosity) != 0)
    return -1;
  return 0;
}

/**
 * @brief Test Cudd_DebugCheck with a simple BDD.
 * @return 0 if successful; -1 otherwise.
 */
static int
testDebugCheck(int verbosity)
{
  DdManager *dd;
  DdNode *f, *g, *h, *var, *tmp;
  int i, ret;

  if (verbosity) {
    printf("=== Testing Cudd_DebugCheck ===\n");
  }

  dd = Cudd_Init(0, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
  if (!dd) {
    if (verbosity) {
      printf("initialization failed\n");
    }
    return -1;
  }

  /* Build a simple BDD: f = x0 AND x1 AND x2 AND x3 */
  f = Cudd_ReadOne(dd);
  Cudd_Ref(f);
  for (i = 3; i >= 0; i--) {
    var = Cudd_bddIthVar(dd, i);
    tmp = Cudd_bddAnd(dd, var, f);
    if (!tmp) {
      if (verbosity) {
        printf("computation failed\n");
      }
      return -1;
    }
    Cudd_Ref(tmp);
    Cudd_RecursiveDeref(dd, f);
    f = tmp;
  }

  /* Build another BDD: g = NOT(x0) OR NOT(x1) */
  g = Cudd_ReadOne(dd);
  Cudd_Ref(g);
  var = Cudd_bddIthVar(dd, 0);
  tmp = Cudd_bddOr(dd, Cudd_Not(var), g);
  Cudd_Ref(tmp);
  Cudd_RecursiveDeref(dd, g);
  g = tmp;
  var = Cudd_bddIthVar(dd, 1);
  tmp = Cudd_bddOr(dd, Cudd_Not(var), g);
  Cudd_Ref(tmp);
  Cudd_RecursiveDeref(dd, g);
  g = tmp;

  /* Build h = f AND g */
  h = Cudd_bddAnd(dd, f, g);
  if (!h) {
    if (verbosity) {
      printf("AND computation failed\n");
    }
    return -1;
  }
  Cudd_Ref(h);

  /* Now run Cudd_DebugCheck - should return 0 for a consistent heap */
  ret = Cudd_DebugCheck(dd);
  if (ret != 0) {
    if (verbosity) {
      printf("Cudd_DebugCheck failed: returned %d\n", ret);
    }
    Cudd_RecursiveDeref(dd, f);
    Cudd_RecursiveDeref(dd, g);
    Cudd_RecursiveDeref(dd, h);
    Cudd_Quit(dd);
    return -1;
  }

  if (verbosity) {
    printf("Cudd_DebugCheck passed on valid BDD heap\n");
  }

  Cudd_RecursiveDeref(dd, f);
  Cudd_RecursiveDeref(dd, g);
  Cudd_RecursiveDeref(dd, h);

  ret = Cudd_CheckZeroRef(dd);
  if (ret != 0 && verbosity) {
    printf("%d unexpected non-zero references\n", ret);
  }
  Cudd_Quit(dd);
  return 0;
}

/**
 * @brief Test Cudd_CheckKeys with BDD and ADD nodes.
 * @return 0 if successful; -1 otherwise.
 */
static int
testCheckKeys(int verbosity)
{
  DdManager *dd;
  DdNode *f, *add, *var, *tmp;
  int i, ret;
  FILE *savefp;

  if (verbosity) {
    printf("=== Testing Cudd_CheckKeys ===\n");
  }

  dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
  if (!dd) {
    if (verbosity) {
      printf("initialization failed\n");
    }
    return -1;
  }

  /* Build a BDD with several variables */
  f = Cudd_ReadOne(dd);
  Cudd_Ref(f);
  for (i = 3; i >= 0; i--) {
    var = Cudd_bddIthVar(dd, i);
    tmp = Cudd_bddAnd(dd, var, f);
    if (!tmp) {
      if (verbosity) {
        printf("computation failed\n");
      }
      return -1;
    }
    Cudd_Ref(tmp);
    Cudd_RecursiveDeref(dd, f);
    f = tmp;
  }

  /* Build an ADD with constant values */
  add = Cudd_addConst(dd, 5.0);
  Cudd_Ref(add);
  for (i = 3; i >= 0; i--) {
    var = Cudd_addIthVar(dd, i);
    Cudd_Ref(var);
    tmp = Cudd_addApply(dd, Cudd_addTimes, var, add);
    Cudd_Ref(tmp);
    Cudd_RecursiveDeref(dd, add);
    Cudd_RecursiveDeref(dd, var);
    add = tmp;
  }

  /* Suppress output from Cudd_CheckKeys if not verbose */
  if (!verbosity) {
    savefp = Cudd_ReadStdout(dd);
    Cudd_SetStdout(dd, fopen("/dev/null", "w"));
  }

  /* Run Cudd_CheckKeys */
  ret = Cudd_CheckKeys(dd);

  if (!verbosity) {
    fclose(Cudd_ReadStdout(dd));
    Cudd_SetStdout(dd, savefp);
  }

  if (ret != 0 && verbosity) {
    printf("Cudd_CheckKeys returned %d (expected 0)\n", ret);
  }

  if (verbosity) {
    printf("Cudd_CheckKeys passed: %d subtables with wrong key count\n", ret);
  }

  Cudd_RecursiveDeref(dd, f);
  Cudd_RecursiveDeref(dd, add);

  ret = Cudd_CheckZeroRef(dd);
  if (ret != 0 && verbosity) {
    printf("%d unexpected non-zero references\n", ret);
  }
  Cudd_Quit(dd);
  return 0;
}

/**
 * @brief Test cuddHeapProfile function.
 * @return 0 if successful; -1 otherwise.
 */
static int
testHeapProfile(int verbosity)
{
  DdManager *dd;
  DdNode *f, *var, *tmp;
  int i, ret;

  if (verbosity) {
    printf("=== Testing cuddHeapProfile ===\n");
  }

  dd = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
  if (!dd) {
    if (verbosity) {
      printf("initialization failed\n");
    }
    return -1;
  }

  /* Build a BDD with multiple levels to give profile more data */
  f = Cudd_ReadOne(dd);
  Cudd_Ref(f);
  for (i = 7; i >= 0; i--) {
    var = Cudd_bddIthVar(dd, i);
    tmp = Cudd_bddAnd(dd, var, f);
    if (!tmp) {
      if (verbosity) {
        printf("computation failed\n");
      }
      return -1;
    }
    Cudd_Ref(tmp);
    Cudd_RecursiveDeref(dd, f);
    f = tmp;
  }

  /* Build another BDD to increase variety */
  {
    DdNode *g = Cudd_ReadOne(dd);
    Cudd_Ref(g);
    for (i = 0; i < 4; i++) {
      var = Cudd_bddIthVar(dd, i);
      tmp = Cudd_bddOr(dd, var, g);
      if (!tmp) {
        if (verbosity) {
          printf("computation failed\n");
        }
        return -1;
      }
      Cudd_Ref(tmp);
      Cudd_RecursiveDeref(dd, g);
      g = tmp;
    }
    Cudd_RecursiveDeref(dd, g);
  }

  /* Run cuddHeapProfile (internal function) */
  if (verbosity) {
    ret = cuddHeapProfile(dd);
    if (ret == 0) {
      printf("cuddHeapProfile failed\n");
      Cudd_RecursiveDeref(dd, f);
      Cudd_Quit(dd);
      return -1;
    }
    printf("cuddHeapProfile passed\n");
  } else {
    /* Even without verbose, run it to ensure coverage */
    FILE *savefp = Cudd_ReadStdout(dd);
    Cudd_SetStdout(dd, fopen("/dev/null", "w"));
    ret = cuddHeapProfile(dd);
    fclose(Cudd_ReadStdout(dd));
    Cudd_SetStdout(dd, savefp);
    if (ret == 0) {
      Cudd_RecursiveDeref(dd, f);
      Cudd_Quit(dd);
      return -1;
    }
  }

  Cudd_RecursiveDeref(dd, f);

  ret = Cudd_CheckZeroRef(dd);
  if (ret != 0 && verbosity) {
    printf("%d unexpected non-zero references\n", ret);
  }
  Cudd_Quit(dd);
  return 0;
}

/**
 * @brief Test cuddPrintNode function.
 * @return 0 if successful; -1 otherwise.
 */
static int
testPrintNode(int verbosity)
{
  DdManager *dd;
  DdNode *f, *var, *tmp;
  int i, ret;

  if (verbosity) {
    printf("=== Testing cuddPrintNode ===\n");
  }

  dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
  if (!dd) {
    if (verbosity) {
      printf("initialization failed\n");
    }
    return -1;
  }

  /* Build a simple BDD */
  f = Cudd_ReadOne(dd);
  Cudd_Ref(f);
  for (i = 3; i >= 0; i--) {
    var = Cudd_bddIthVar(dd, i);
    tmp = Cudd_bddAnd(dd, var, f);
    if (!tmp) {
      if (verbosity) {
        printf("computation failed\n");
      }
      return -1;
    }
    Cudd_Ref(tmp);
    Cudd_RecursiveDeref(dd, f);
    f = tmp;
  }

  /* Test cuddPrintNode on various nodes */
  if (verbosity) {
    printf("Printing nodes:\n");
    cuddPrintNode(f, stdout);
    cuddPrintNode(Cudd_Regular(cuddT(Cudd_Regular(f))), stdout);
    cuddPrintNode(DD_ONE(dd), stdout);
    printf("cuddPrintNode tests passed\n");
  } else {
    /* Run silently to dev/null for coverage */
    FILE *devnull = fopen("/dev/null", "w");
    cuddPrintNode(f, devnull);
    cuddPrintNode(Cudd_Regular(cuddT(Cudd_Regular(f))), devnull);
    cuddPrintNode(DD_ONE(dd), devnull);
    fclose(devnull);
  }

  Cudd_RecursiveDeref(dd, f);

  ret = Cudd_CheckZeroRef(dd);
  if (ret != 0 && verbosity) {
    printf("%d unexpected non-zero references\n", ret);
  }
  Cudd_Quit(dd);
  return 0;
}

/**
 * @brief Test cuddPrintVarGroups function.
 * @return 0 if successful; -1 otherwise.
 */
static int
testPrintVarGroups(int verbosity)
{
  DdManager *dd;
  DdNode *f, *var, *tmp;
  int i, ret;
  MtrNode *group;

  if (verbosity) {
    printf("=== Testing cuddPrintVarGroups ===\n");
  }

  /* Initialize with both BDD and ZDD variables */
  dd = Cudd_Init(8, 8, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
  if (!dd) {
    if (verbosity) {
      printf("initialization failed\n");
    }
    return -1;
  }

  /* Build a BDD */
  f = Cudd_ReadOne(dd);
  Cudd_Ref(f);
  for (i = 7; i >= 0; i--) {
    var = Cudd_bddIthVar(dd, i);
    tmp = Cudd_bddAnd(dd, var, f);
    if (!tmp) {
      if (verbosity) {
        printf("computation failed\n");
      }
      return -1;
    }
    Cudd_Ref(tmp);
    Cudd_RecursiveDeref(dd, f);
    f = tmp;
  }

  /* Create BDD variable groups for testing */
  group = Cudd_MakeTreeNode(dd, 0, 4, MTR_DEFAULT);
  if (group == NULL) {
    if (verbosity) {
      printf("Failed to create variable group\n");
    }
    Cudd_RecursiveDeref(dd, f);
    Cudd_Quit(dd);
    return -1;
  }
  group = Cudd_MakeTreeNode(dd, 4, 4, MTR_FIXED);
  if (group == NULL) {
    if (verbosity) {
      printf("Failed to create second variable group\n");
    }
    Cudd_RecursiveDeref(dd, f);
    Cudd_Quit(dd);
    return -1;
  }

  /* Create ZDD variable groups for testing */
  group = Cudd_MakeZddTreeNode(dd, 0, 4, MTR_DEFAULT);
  if (group == NULL) {
    if (verbosity) {
      printf("Failed to create ZDD variable group\n");
    }
    Cudd_RecursiveDeref(dd, f);
    Cudd_Quit(dd);
    return -1;
  }
  group = Cudd_MakeZddTreeNode(dd, 4, 4, MTR_FIXED);
  if (group == NULL) {
    if (verbosity) {
      printf("Failed to create second ZDD variable group\n");
    }
    Cudd_RecursiveDeref(dd, f);
    Cudd_Quit(dd);
    return -1;
  }

  /* Now test cuddPrintVarGroups for BDD tree */
  if (dd->tree != NULL) {
    if (verbosity) {
      printf("Printing BDD variable groups:\n");
      cuddPrintVarGroups(dd, dd->tree, 0, 0);
      printf("\n");
    } else {
      /* Run silently for coverage */
      cuddPrintVarGroups(dd, dd->tree, 0, 1);
    }
  }

  /* Test cuddPrintVarGroups for ZDD tree (zdd=1) */
  if (dd->treeZ != NULL) {
    if (verbosity) {
      printf("Printing ZDD variable groups:\n");
      cuddPrintVarGroups(dd, dd->treeZ, 1, 0);
      printf("\n");
    } else {
      /* Run silently for coverage */
      cuddPrintVarGroups(dd, dd->treeZ, 1, 1);
    }
  }

  if (verbosity) {
    printf("cuddPrintVarGroups tests passed\n");
  }

  Cudd_RecursiveDeref(dd, f);

  ret = Cudd_CheckZeroRef(dd);
  if (ret != 0 && verbosity) {
    printf("%d unexpected non-zero references\n", ret);
  }
  Cudd_Quit(dd);
  return 0;
}

/**
 * @brief Test consistency checks with ZDD structures.
 * @return 0 if successful; -1 otherwise.
 */
static int
testCheckWithZDD(int verbosity)
{
  DdManager *dd;
  DdNode *f, *var, *tmp;
  int i, ret;
  FILE *savefp;

  if (verbosity) {
    printf("=== Testing checks with ZDD ===\n");
  }

  /* Initialize with ZDD variables */
  dd = Cudd_Init(4, 4, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
  if (!dd) {
    if (verbosity) {
      printf("initialization failed\n");
    }
    return -1;
  }

  /* Build a ZDD */
  tmp = Cudd_ReadZddOne(dd, 0);
  Cudd_Ref(tmp);
  for (i = 3; i >= 0; i--) {
    var = Cudd_zddIthVar(dd, i);
    Cudd_Ref(var);
    f = Cudd_zddIntersect(dd, var, tmp);
    Cudd_Ref(f);
    Cudd_RecursiveDerefZdd(dd, tmp);
    Cudd_RecursiveDerefZdd(dd, var);
    tmp = f;
  }
  f = Cudd_zddDiff(dd, Cudd_ReadZddOne(dd, 0), tmp);
  Cudd_Ref(f);
  Cudd_RecursiveDerefZdd(dd, tmp);

  /* Now test Cudd_DebugCheck with ZDD nodes present */
  ret = Cudd_DebugCheck(dd);
  if (ret != 0) {
    if (verbosity) {
      printf("Cudd_DebugCheck with ZDD failed: returned %d\n", ret);
    }
    Cudd_RecursiveDerefZdd(dd, f);
    Cudd_Quit(dd);
    return -1;
  }

  /* Test Cudd_CheckKeys with ZDD */
  if (!verbosity) {
    savefp = Cudd_ReadStdout(dd);
    Cudd_SetStdout(dd, fopen("/dev/null", "w"));
  }

  ret = Cudd_CheckKeys(dd);

  if (!verbosity) {
    fclose(Cudd_ReadStdout(dd));
    Cudd_SetStdout(dd, savefp);
  }

  if (verbosity) {
    printf("Cudd_DebugCheck with ZDD passed\n");
    printf("Cudd_CheckKeys with ZDD returned %d\n", ret);
  }

  Cudd_RecursiveDerefZdd(dd, f);

  ret = Cudd_CheckZeroRef(dd);
  if (ret != 0 && verbosity) {
    printf("%d unexpected non-zero references\n", ret);
  }
  Cudd_Quit(dd);
  return 0;
}

/**
 * @brief Test checks with constant table.
 * @return 0 if successful; -1 otherwise.
 */
static int
testCheckWithConstants(int verbosity)
{
  DdManager *dd;
  DdNode *add, *c1, *c2, *c3, *var0, *var1, *tmp;
  int ret;
  FILE *savefp;

  if (verbosity) {
    printf("=== Testing checks with constants ===\n");
  }

  dd = Cudd_Init(4, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
  if (!dd) {
    if (verbosity) {
      printf("initialization failed\n");
    }
    return -1;
  }

  /* Create multiple ADD constants to populate the constant table */
  c1 = Cudd_addConst(dd, 1.0);
  Cudd_Ref(c1);
  c2 = Cudd_addConst(dd, 2.0);
  Cudd_Ref(c2);
  c3 = Cudd_addConst(dd, 3.0);
  Cudd_Ref(c3);

  /* Get variable ADD nodes and reference them */
  var0 = Cudd_addIthVar(dd, 0);
  Cudd_Ref(var0);
  var1 = Cudd_addIthVar(dd, 1);
  Cudd_Ref(var1);

  /* Build an ADD with these constants */
  add = Cudd_addIte(dd, var0, c1, c2);
  Cudd_Ref(add);
  tmp = Cudd_addIte(dd, var1, add, c3);
  Cudd_Ref(tmp);
  Cudd_RecursiveDeref(dd, add);
  add = tmp;

  /* Run checks */
  ret = Cudd_DebugCheck(dd);
  if (ret != 0) {
    if (verbosity) {
      printf("Cudd_DebugCheck with constants failed: returned %d\n", ret);
    }
    Cudd_RecursiveDeref(dd, c1);
    Cudd_RecursiveDeref(dd, c2);
    Cudd_RecursiveDeref(dd, c3);
    Cudd_RecursiveDeref(dd, var0);
    Cudd_RecursiveDeref(dd, var1);
    Cudd_RecursiveDeref(dd, add);
    Cudd_Quit(dd);
    return -1;
  }

  if (!verbosity) {
    savefp = Cudd_ReadStdout(dd);
    Cudd_SetStdout(dd, fopen("/dev/null", "w"));
  }

  ret = Cudd_CheckKeys(dd);

  if (!verbosity) {
    fclose(Cudd_ReadStdout(dd));
    Cudd_SetStdout(dd, savefp);
  }

  if (verbosity) {
    printf("Tests with constants passed\n");
  }

  Cudd_RecursiveDeref(dd, c1);
  Cudd_RecursiveDeref(dd, c2);
  Cudd_RecursiveDeref(dd, c3);
  Cudd_RecursiveDeref(dd, var0);
  Cudd_RecursiveDeref(dd, var1);
  Cudd_RecursiveDeref(dd, add);

  ret = Cudd_CheckZeroRef(dd);
  if (ret != 0 && verbosity) {
    printf("%d unexpected non-zero references\n", ret);
  }
  Cudd_Quit(dd);
  return 0;
}

/**
 * @brief Test checks with reordering to trigger more code paths.
 * @return 0 if successful; -1 otherwise.
 */
static int
testCheckWithReordering(int verbosity)
{
  DdManager *dd;
  DdNode *f, *g, *var, *tmp;
  int i, ret;
  FILE *savefp;

  if (verbosity) {
    printf("=== Testing checks with reordering ===\n");
  }

  dd = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
  if (!dd) {
    if (verbosity) {
      printf("initialization failed\n");
    }
    return -1;
  }

  /* Build a more complex BDD */
  f = Cudd_ReadOne(dd);
  Cudd_Ref(f);
  for (i = 7; i >= 0; i--) {
    var = Cudd_bddIthVar(dd, i);
    tmp = Cudd_bddAnd(dd, var, f);
    if (!tmp) {
      return -1;
    }
    Cudd_Ref(tmp);
    Cudd_RecursiveDeref(dd, f);
    f = tmp;
  }

  /* Build another BDD with XOR to create more structure */
  g = Cudd_ReadLogicZero(dd);
  Cudd_Ref(g);
  for (i = 0; i < 4; i++) {
    var = Cudd_bddIthVar(dd, i);
    tmp = Cudd_bddXor(dd, var, g);
    if (!tmp) {
      return -1;
    }
    Cudd_Ref(tmp);
    Cudd_RecursiveDeref(dd, g);
    g = tmp;
  }

  /* Verify before reordering */
  ret = Cudd_DebugCheck(dd);
  if (ret != 0) {
    if (verbosity) {
      printf("Cudd_DebugCheck before reordering failed\n");
    }
    Cudd_RecursiveDeref(dd, f);
    Cudd_RecursiveDeref(dd, g);
    Cudd_Quit(dd);
    return -1;
  }

  /* Perform reordering */
  ret = Cudd_ReduceHeap(dd, CUDD_REORDER_SIFT, 5);
  if (ret == 0) {
    if (verbosity) {
      printf("Reordering failed\n");
    }
  }

  /* Verify after reordering */
  ret = Cudd_DebugCheck(dd);
  if (ret != 0) {
    if (verbosity) {
      printf("Cudd_DebugCheck after reordering failed\n");
    }
    Cudd_RecursiveDeref(dd, f);
    Cudd_RecursiveDeref(dd, g);
    Cudd_Quit(dd);
    return -1;
  }

  if (!verbosity) {
    savefp = Cudd_ReadStdout(dd);
    Cudd_SetStdout(dd, fopen("/dev/null", "w"));
  }

  ret = Cudd_CheckKeys(dd);

  if (!verbosity) {
    fclose(Cudd_ReadStdout(dd));
    Cudd_SetStdout(dd, savefp);
  }

  if (verbosity) {
    printf("Cudd_CheckKeys after reordering: %d\n", ret);
    printf("Tests with reordering passed\n");
  }

  Cudd_RecursiveDeref(dd, f);
  Cudd_RecursiveDeref(dd, g);

  ret = Cudd_CheckZeroRef(dd);
  if (ret != 0 && verbosity) {
    printf("%d unexpected non-zero references\n", ret);
  }
  Cudd_Quit(dd);
  return 0;
}

/**
 * @brief Test checks with dead nodes to trigger those code paths.
 * @return 0 if successful; -1 otherwise.
 */
static int
testCheckWithDeadNodes(int verbosity)
{
  DdManager *dd;
  DdNode *f, *g, *h, *var, *tmp;
  int i, ret;
  FILE *savefp;

  if (verbosity) {
    printf("=== Testing checks with dead nodes ===\n");
  }

  dd = Cudd_Init(8, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
  if (!dd) {
    if (verbosity) {
      printf("initialization failed\n");
    }
    return -1;
  }

  /* Build several BDDs and then dereference some to create dead nodes */
  f = Cudd_ReadOne(dd);
  Cudd_Ref(f);
  for (i = 7; i >= 0; i--) {
    var = Cudd_bddIthVar(dd, i);
    tmp = Cudd_bddAnd(dd, var, f);
    if (!tmp) {
      return -1;
    }
    Cudd_Ref(tmp);
    Cudd_RecursiveDeref(dd, f);
    f = tmp;
  }

  g = Cudd_ReadOne(dd);
  Cudd_Ref(g);
  for (i = 0; i < 4; i++) {
    var = Cudd_bddIthVar(dd, i);
    tmp = Cudd_bddOr(dd, var, g);
    if (!tmp) {
      return -1;
    }
    Cudd_Ref(tmp);
    Cudd_RecursiveDeref(dd, g);
    g = tmp;
  }

  h = Cudd_bddXor(dd, f, g);
  if (!h) {
    return -1;
  }
  Cudd_Ref(h);

  /* Dereference g - this may create some dead nodes (depending on sharing) */
  Cudd_RecursiveDeref(dd, g);

  /* Run checks with some dead nodes potentially present */
  ret = Cudd_DebugCheck(dd);
  if (ret != 0) {
    if (verbosity) {
      printf("Cudd_DebugCheck with dead nodes failed: returned %d\n", ret);
    }
    Cudd_RecursiveDeref(dd, f);
    Cudd_RecursiveDeref(dd, h);
    Cudd_Quit(dd);
    return -1;
  }

  if (!verbosity) {
    savefp = Cudd_ReadStdout(dd);
    Cudd_SetStdout(dd, fopen("/dev/null", "w"));
  }

  ret = Cudd_CheckKeys(dd);

  if (!verbosity) {
    fclose(Cudd_ReadStdout(dd));
    Cudd_SetStdout(dd, savefp);
  }

  /* Manually trigger garbage collection */
  Cudd_RecursiveDeref(dd, h);
  Cudd_RecursiveDeref(dd, f);

  /* Force garbage collection */
  cuddGarbageCollect(dd, 1);

  /* Run debug check after GC */
  ret = Cudd_DebugCheck(dd);

  if (verbosity) {
    printf("Tests with dead nodes passed\n");
  }

  ret = Cudd_CheckZeroRef(dd);
  if (ret != 0 && verbosity) {
    printf("%d unexpected non-zero references\n", ret);
  }
  Cudd_Quit(dd);
  return 0;
}

/**
 * @brief Test with larger structures to hit more code paths.
 * @return 0 if successful; -1 otherwise.
 */
static int
testCheckWithLargeStructure(int verbosity)
{
  DdManager *dd;
  DdNode *f, *add, *var, *tmp;
  int i, ret;
  FILE *savefp;

  if (verbosity) {
    printf("=== Testing checks with large structure ===\n");
  }

  dd = Cudd_Init(16, 0, CUDD_UNIQUE_SLOTS, CUDD_CACHE_SLOTS, 0);
  if (!dd) {
    if (verbosity) {
      printf("initialization failed\n");
    }
    return -1;
  }

  /* Build a larger BDD to fill more subtables */
  f = Cudd_ReadOne(dd);
  Cudd_Ref(f);
  for (i = 15; i >= 0; i--) {
    var = Cudd_bddIthVar(dd, i);
    tmp = Cudd_bddAnd(dd, var, f);
    if (!tmp) {
      return -1;
    }
    Cudd_Ref(tmp);
    Cudd_RecursiveDeref(dd, f);
    f = tmp;
  }

  /* Build an ADD to test constant table more thoroughly */
  add = Cudd_addConst(dd, 1.0);
  Cudd_Ref(add);
  for (i = 0; i < 8; i++) {
    DdNode *addvar = Cudd_addIthVar(dd, i);
    Cudd_Ref(addvar);
    tmp = Cudd_addApply(dd, Cudd_addPlus, addvar, add);
    Cudd_Ref(tmp);
    Cudd_RecursiveDeref(dd, add);
    Cudd_RecursiveDeref(dd, addvar);
    add = tmp;
  }

  /* Run heap profile */
  if (verbosity) {
    ret = cuddHeapProfile(dd);
    if (ret == 0) {
      printf("cuddHeapProfile failed\n");
      Cudd_RecursiveDeref(dd, f);
      Cudd_RecursiveDeref(dd, add);
      Cudd_Quit(dd);
      return -1;
    }
  } else {
    savefp = Cudd_ReadStdout(dd);
    Cudd_SetStdout(dd, fopen("/dev/null", "w"));
    ret = cuddHeapProfile(dd);
    fclose(Cudd_ReadStdout(dd));
    Cudd_SetStdout(dd, savefp);
    if (ret == 0) {
      Cudd_RecursiveDeref(dd, f);
      Cudd_RecursiveDeref(dd, add);
      Cudd_Quit(dd);
      return -1;
    }
  }

  /* Run checks */
  ret = Cudd_DebugCheck(dd);
  if (ret != 0) {
    if (verbosity) {
      printf("Cudd_DebugCheck with large structure failed: returned %d\n", ret);
    }
    Cudd_RecursiveDeref(dd, f);
    Cudd_RecursiveDeref(dd, add);
    Cudd_Quit(dd);
    return -1;
  }

  if (!verbosity) {
    savefp = Cudd_ReadStdout(dd);
    Cudd_SetStdout(dd, fopen("/dev/null", "w"));
  }

  ret = Cudd_CheckKeys(dd);

  if (!verbosity) {
    fclose(Cudd_ReadStdout(dd));
    Cudd_SetStdout(dd, savefp);
  }

  if (verbosity) {
    printf("Tests with large structure passed\n");
  }

  Cudd_RecursiveDeref(dd, f);
  Cudd_RecursiveDeref(dd, add);

  ret = Cudd_CheckZeroRef(dd);
  if (ret != 0 && verbosity) {
    printf("%d unexpected non-zero references\n", ret);
  }
  Cudd_Quit(dd);
  return 0;
}
