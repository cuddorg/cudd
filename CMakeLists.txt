cmake_minimum_required(VERSION 3.18)

# The default build type must be set before project()
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR AND NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/src/config.h" cfg)

string(REGEX MATCH "CUDD_VERSION_MAJOR ([0-9]*)" _ ${cfg})
set(cudd_version_major ${CMAKE_MATCH_1})

string(REGEX MATCH "CUDD_VERSION_MINOR ([0-9]*)" _ ${cfg})
set(cudd_version_minor ${CMAKE_MATCH_1})

string(REGEX MATCH "CUDD_VERSION_PATCH ([0-9]*)" _ ${cfg})
set(cudd_version_patch ${CMAKE_MATCH_1})

project(cudd
  VERSION "${cudd_version_major}.${cudd_version_minor}.${cudd_version_patch}"
  DESCRIPTION "CUDD Decision Diagram Library"
  HOMEPAGE_URL "https://github.com/cuddorg/cudd"
  LANGUAGES C CXX
)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(FATAL_ERROR "In-source builds not allowed.")
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GNUInstallDirs)

# ============================================================================ #
# Settings
# ============================================================================ #

option(CUDD_BUILD_CPP_API "Build C++ API" ON)
option(CUDD_BUILD_WITH_STATS "Build CUDD with statistics" OFF)
option(CUDD_BUILD_SHARED_LIBS "Build CUDD as a shared library" OFF)
option(CUDD_BUILD_TESTS "Build CUDD tests" OFF)
option(CUDD_BUILD_COVERAGE "Enable code coverage instrumentation" OFF)
option(CUDD_INSTALL "Enable installation of CUDD targets" ON)
option(CUDD_RECONFIGURE_SYSTEM "Configure system variables" OFF)

# option(BUILD_API_DOCS "Build API documentation with Doxygen" OFF)
# option(BUILD_USER_GUIDES "Build user guides with LaTeX" OFF)

# Use ucbqsort by default
set(USE_SYSTEM_QSORT FALSE)

if(CUDD_BUILD_CPP_API)
  set(HAVE_MODERN_CXX TRUE)
  set(HAVE_WORKING_THREAD TRUE)
endif()

find_package(Threads)
if(Threads_FOUND)
  add_compile_definitions(HAVE_PTHREADS=1)
  message(STATUS "Threads found and enabled.")
endif()

# System feature support is extensive on modern platforms,
# so reconfiguration is disabled by default for efficiency.
# You can re-enable it manually if targeting a non-standard or legacy system.
if(CUDD_RECONFIGURE_SYSTEM)
  include(cmake/cudd_configure_system.cmake)
endif()

### ============================================================================ #
# CUDD Library
### ============================================================================ #

# Public header directories
set(CUDD_PUBLIC_HEADERS
  "${CMAKE_CURRENT_SOURCE_DIR}/include/cudd/cudd.h"
)
set(CUDD_SOURCES
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddAddAbs.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddBddCorr.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddGenetic.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddReorder.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddZddGroup.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddAddApply.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddBddIte.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddGroup.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddSat.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddZddIsop.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddAddFind.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddBridge.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddHarwell.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddSign.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddZddLin.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddAddInv.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddCache.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddInit.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddSolve.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddZddMisc.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddAddIte.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddCheck.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddInteract.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddSplit.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddZddPort.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddAddNeg.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddClip.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddLCache.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddSubsetHB.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddZddReord.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddAddWalsh.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddCof.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddLevelQ.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddSubsetSP.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddZddSetop.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddAndAbs.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddCompose.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddLinear.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddSymmetry.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddZddSymm.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddAnneal.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddDecomp.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddLiteral.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddTable.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddZddUtil.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddApa.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddEssent.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddMatMult.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddUtil.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddAPI.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddExact.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddPriority.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddWindow.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddApprox.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddExport.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddRead.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddZddCount.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddBddAbs.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddGenCof.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddRef.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddZddFuncs.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/epd.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/mtrBasic.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/mtrGroup.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/st.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cpu_stats.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cpu_time.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/cstringstream.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/datalimit.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/pathsearch.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/pipefork.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/prtime.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/safe_mem.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/strsav.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/texpand.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/ucbqsort.c"
)

if(CUDD_BUILD_CPP_API)
  set(CUDD_PUBLIC_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/cudd/cudd.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/cudd/cuddObj.hh"
    ${CUDD_PUBLIC_HEADERS}
  )
  set(CUDD_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/cuddObj.cc"
    ${CUDD_SOURCES}
  )
endif()

###
### CUDD Target
###

# Define the cudd library target
if(CUDD_BUILD_SHARED_LIBS)
  add_library(cudd SHARED ${CUDD_SOURCES})
else()
  add_library(cudd STATIC ${CUDD_SOURCES})
endif()
add_library(cudd::cudd ALIAS cudd)

target_compile_options(cudd PRIVATE
  $<$<BOOL:${CUDD_BUILD_COVERAGE}>:-O0>
  $<$<BOOL:${CUDD_BUILD_COVERAGE}>:--coverage>
)

# Include directories (build and install time)
target_include_directories(cudd PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>  
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Define properties for the library
set_target_properties(cudd PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  OUTPUT_NAME "cudd"
  DEBUG_POSTFIX "_debug"
  POSITION_INDEPENDENT_CODE ON
  C_VISIBILITY_PRESET hidden
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
  PUBLIC_HEADER "${CUDD_PUBLIC_HEADERS}"
)

# Optional compilation flags
if(CUDD_BUILD_WITH_STATS)
  target_compile_definitions(cudd PUBLIC DD_STATS)
endif()

# Test Build
if(CUDD_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
  # add_subdirectory(extras/nanotrav)
endif()

# Install
if(CUDD_INSTALL)
  include(cmake/install.cmake)
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
