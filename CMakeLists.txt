cmake_minimum_required(VERSION 3.10)

project(cudd
  VERSION 3.0.0
  DESCRIPTION "CUDD: University of Colorado at Boulder Decision Diagrams"
  HOMEPAGE_URL "https://github.com/bounverif/cudd"
  LANGUAGES C CXX
)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(FATAL_ERROR "In-source builds not allowed.")
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============================================================================ #
# Settings
# ============================================================================ #

option(BUILD_CPP_API "Build C++ API" ON)
option(BUILD_SHARED_LIBS "Build CUDD as a shared library" ON)
option(BUILD_WITH_STATS "Build CUDD with statistics" OFF)
option(BUILD_DDDMP "Build DDDMP Library" OFF)
option(BUILD_TESTS "Build CUDD tests" OFF)
option(BUILD_WITH_SYSTEM_QSORT "Build with system qsort" OFF)
option(BUILD_TESTING "Enable testing via CTest" ON)

# option(BUILD_API_DOCS "Build API documentation with Doxygen" OFF)
# option(BUILD_USER_GUIDES "Build user guides with LaTeX" OFF)
# option(BUILD_NANOTRAV_EXAMPLE "Build Nanotrav Example" OFF)

if(BUILD_CPP_API)
  include(CheckCXXCompilerFlag)
  CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
  if(NOT COMPILER_SUPPORTS_CXX11)
    message(FATAL_ERROR "${CMAKE_CXX_COMPILER} has no C++11 support.")
  endif()
  include(CheckCXXSourceCompiles)
  set(HAVE_MODERN_CXX TRUE)
  set(HAVE_WORKING_THREAD TRUE)
endif()

# ============================================================================ #
# Compiler Settings
# ============================================================================ #

include(CheckCXXCompilerFlag)

if(BUILD_WITH_SYSTEM_QSORT)
  set(USE_SYSTEM_QSORT TRUE)
endif()

# set(__USE_MINGW_ANSI_STDIO ON) # TODO: Test.

find_package(Threads)
if(CMAKE_USE_PTHREADS_INIT)
  add_definitions(-DHAVE_PTHREADS=1)
  set(HAVE_PTHREADS TRUE)
  add_definitions("-pthread")
  # target_link_libraries(${name} ${CMAKE_THREAD_LIBS_INIT})
endif()

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


### ============================================================================ #
# CUDD Library
### ============================================================================ #

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.in
               ${CMAKE_CURRENT_BINARY_DIR}/config.h)

# Public header directories
set(CUDD_PUBLIC_HEADERS
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cudd.h"
)
set(CUDD_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddAddAbs.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddBddCorr.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddGenetic.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddReorder.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddZddGroup.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddAddApply.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddBddIte.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddGroup.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddSat.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddZddIsop.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddAddFind.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddBridge.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddHarwell.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddSign.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddZddLin.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddAddInv.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddCache.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddInit.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddSolve.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddZddMisc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddAddIte.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddCheck.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddInteract.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddSplit.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddZddPort.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddAddNeg.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddClip.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddLCache.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddSubsetHB.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddZddReord.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddAddWalsh.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddCof.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddLevelQ.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddSubsetSP.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddZddSetop.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddAndAbs.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddCompose.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddLinear.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddSymmetry.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddZddSymm.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddAnneal.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddDecomp.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddLiteral.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddTable.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddZddUtil.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddApa.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddEssent.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddMatMult.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddUtil.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddAPI.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddExact.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddPriority.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddWindow.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddApprox.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddExport.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddRead.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddZddCount.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddBddAbs.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddGenCof.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddRef.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/cudd/cuddZddFuncs.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/epd/epd.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/mtr/mtrBasic.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/mtr/mtrGroup.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/st/st.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/util/cpu_stats.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/util/cpu_time.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/util/cstringstream.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/util/datalimit.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/util/pathsearch.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/util/pipefork.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/util/prtime.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/util/safe_mem.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/util/strsav.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/util/texpand.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/util/ucbqsort.c"
)

if(BUILD_CPP_API)
  add_subdirectory(cplusplus)
endif()

###
### CUDD Target
###

# Define the cudd library target
add_library(cudd ${CUDD_SOURCES})

# Link the math library (libm)
find_library(MATH_LIBRARY m REQUIRED)
target_link_libraries(cudd PUBLIC ${MATH_LIBRARY})

# Include directories (build and install time)
target_include_directories(cudd PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/cudd>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/cplusplus>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/epd>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/mtr>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/st>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/util>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
  $<INSTALL_INTERFACE:include/cudd>
)

# Define properties for the library
set_target_properties(cudd PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  POSITION_INDEPENDENT_CODE ON
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
  PUBLIC_HEADER "${CUDD_PUBLIC_HEADERS}"
)

# Optional compilation flags
if(BUILD_WITH_STATS)
  target_compile_definitions(cudd PUBLIC DD_STATS)
endif()

###
### Export and Install
###

include(GNUInstallDirs)
include(GenerateExportHeader)
include(CMakePackageConfigHelpers)

generate_export_header(cudd
  EXPORT_MACRO_NAME CUDD_API
)

# Install the cudd library
install(TARGETS cudd
  EXPORT cuddTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Runtime 
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cudd COMPONENT Development
)

# Install version/config files for find_package
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/cuddConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cuddConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cuddConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cudd
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/cuddConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/cuddConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cudd
)

# DDDMP Target
if(BUILD_DDDMP)
  add_subdirectory(dddmp)
endif()

# Test Build
include(CTest)
if(BUILD_TESTS)
  add_subdirectory(nanotrav)
endif()

# Compile Commands
if (PROJECT_IS_TOP_LEVEL AND UNIX)
    # Create symlink to compile_commands.json for IDE to pick it up
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
    )
endif()