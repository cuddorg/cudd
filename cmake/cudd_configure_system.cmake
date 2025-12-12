# ============================================================================ #
# Compiler Settings
# ============================================================================ #

include(CheckCXXCompilerFlag)
include(CheckCSourceCompiles)
include(CheckCXXSourceCompiles)
include(CheckIncludeFile)

CHECK_INCLUDE_FILE("float.h" HAVE_FLOAT_H)
if(NOT (HAVE_FLOAT_H))
  message(FATAL_ERROR "'float.h' missing.")
endif()

CHECK_INCLUDE_FILE("inttypes.h" HAVE_INTTYPES_H)
if(NOT (HAVE_INTTYPES_H))
  message(FATAL_ERROR "'inttypes.h' missing.")
endif()

CHECK_INCLUDE_FILE("limits.h" HAVE_LIMITS_H)
if(NOT (HAVE_LIMITS_H))
  message(FATAL_ERROR "'limits.h' missing.")
endif()

CHECK_INCLUDE_FILE("stddef.h" HAVE_STDDEF_H)
if(NOT (HAVE_STDDEF_H))
  message(FATAL_ERROR "'stddef.h' missing.")
endif()

CHECK_INCLUDE_FILE("stdlib.h" HAVE_STDLIB_H)
if(NOT (HAVE_STDLIB_H))
  message(FATAL_ERROR "'stdlib.h' missing.")
endif()

CHECK_INCLUDE_FILE("string.h" HAVE_STRING_H)
if(NOT (HAVE_STRING_H))
  message(FATAL_ERROR "'string.h' missing.")
endif()

CHECK_INCLUDE_FILE("assert.h" HAVE_ASSERT_H)
if(NOT (HAVE_ASSERT_H))
  message(FATAL_ERROR "'assert.h' missing.")
endif()

CHECK_INCLUDE_FILE("math.h" HAVE_MATH_H)
if(NOT (HAVE_MATH_H))
  message(FATAL_ERROR "'math.h' missing.")
endif()

CHECK_INCLUDE_FILE("unistd.h" HAVE_UNISTD_H)
CHECK_INCLUDE_FILE("sys/time.h" HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILE("sys/times.h" HAVE_SYS_TIMES_H)
CHECK_INCLUDE_FILE("sys/resource.h" HAVE_SYS_RESOURCE_H)
CHECK_INCLUDE_FILE("sys/wait.h" HAVE_SYS_WAIT_H)
CHECK_INCLUDE_FILE("sys/stat.h" HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILE("dlfcn.h" HAVE_DLFCN_H) # libtool
CHECK_INCLUDE_FILE("memory.h" HAVE_MEMORY_H)
CHECK_INCLUDE_FILE("strings.h" HAVE_STRINGS_H)

set(STDC_HEADERS TRUE) # TODO: Test

CHECK_INCLUDE_FILE("stdbool.h" HAVE__BOOL)

include(CheckTypeSize)
CHECK_TYPE_SIZE(size_t SIZE_T)
CHECK_TYPE_SIZE(uint16_t UINT16_T)
CHECK_TYPE_SIZE(uint32_t UINT32_T)
CHECK_TYPE_SIZE(ptrdiff_t PTRDIFF_T)
CHECK_TYPE_SIZE(int SIZEOF_INT)
CHECK_TYPE_SIZE(long SIZEOF_LONG)
CHECK_TYPE_SIZE("void*" SIZEOF_VOID_P)
CHECK_TYPE_SIZE("long double" SIZEOF_LONG_DOUBLE)

include(CheckFunctionExists)

set(CMAKE_REQUIRED_INCLUDES "math.h")
set(CMAKE_REQUIRED_LIBRARIES m)

CHECK_FUNCTION_EXISTS(pow HAVE_POW)
if(NOT (HAVE_POW))
  message(FATAL_ERROR "'pow' function missing.")
endif()

CHECK_FUNCTION_EXISTS(sqrt HAVE_SQRT)
if(NOT (HAVE_SQRT))
  message(FATAL_ERROR "'sqrt' function missing.")
endif()

CHECK_FUNCTION_EXISTS(strchr HAVE_STRCHR)
if(NOT (HAVE_STRCHR))
  message(FATAL_ERROR "'strchr' function missing.")
endif()

CHECK_FUNCTION_EXISTS(strstr HAVE_STRSTR)
if(NOT (HAVE_STRSTR))
  message(FATAL_ERROR "'strstr' function missing.")
endif()

CHECK_FUNCTION_EXISTS(powl HAVE_POWL)
CHECK_FUNCTION_EXISTS(gethostname HAVE_GETHOSTNAME)
CHECK_FUNCTION_EXISTS(getrlimit HAVE_GETRLIMIT)
CHECK_FUNCTION_EXISTS(getrusage HAVE_GETRUSAGE)
CHECK_FUNCTION_EXISTS(sysconf HAVE_SYSCONF)

include(CheckCSourceCompiles)
CHECK_C_SOURCE_COMPILES(
  "#include <math.h>
  int main() { double x = INFINITY; }
  " HAVE_IEEE_754)

configure_file(
  ${CMAKE_SOURCE_DIR}/cmake/config.h.in
  ${CMAKE_BINARY_DIR}/src/config.h
)
message(STATUS "CUDD reconfiguration complete.")
