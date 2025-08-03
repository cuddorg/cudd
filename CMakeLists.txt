cmake_minimum_required(VERSION 3.10)

# The default build type must be set before project()
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR AND NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

project(cudd
  VERSION 4.0.0
  DESCRIPTION "CUDD Decision Diagram Library"
  HOMEPAGE_URL "https://github.com/cuddorg/cudd"
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

option(CUDD_RECONFIGURE_SYSTEM "Configure system variables" OFF)
option(CUDD_BUILD_CPP_API "Build C++ API" ON)
option(CUDD_BUILD_SHARED_LIBS "Build CUDD as a shared library" OFF)
option(CUDD_BUILD_WITH_STATS "Build CUDD with statistics" OFF)
option(CUDD_BUILD_DDDMP "Build DDDMP Library" OFF)
option(CUDD_BUILD_TESTS "Build CUDD tests" OFF)
option(CUDD_BUILD_WITH_SYSTEM_QSORT "Build with system qsort" OFF)

# option(BUILD_API_DOCS "Build API documentation with Doxygen" OFF)
# option(BUILD_USER_GUIDES "Build user guides with LaTeX" OFF)
# option(BUILD_NANOTRAV_EXAMPLE "Build Nanotrav Example" OFF)

if(CUDD_USE_SYSTEM_QSORT)
  set(USE_SYSTEM_QSORT TRUE)
endif()

if(CUDD_BUILD_CPP_API)
  include(CheckCXXCompilerFlag)
  CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
  if(NOT COMPILER_SUPPORTS_CXX11)
    message(FATAL_ERROR "${CMAKE_CXX_COMPILER} has no C++11 support.")
  endif()
  include(CheckCXXSourceCompiles)
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
  "src/cuddAddAbs.c"
  "src/cuddBddCorr.c"
  "src/cuddGenetic.c"
  "src/cuddReorder.c"
  "src/cuddZddGroup.c"
  "src/cuddAddApply.c"
  "src/cuddBddIte.c"
  "src/cuddGroup.c"
  "src/cuddSat.c"
  "src/cuddZddIsop.c"
  "src/cuddAddFind.c"
  "src/cuddBridge.c"
  "src/cuddHarwell.c"
  "src/cuddSign.c"
  "src/cuddZddLin.c"
  "src/cuddAddInv.c"
  "src/cuddCache.c"
  "src/cuddInit.c"
  "src/cuddSolve.c"
  "src/cuddZddMisc.c"
  "src/cuddAddIte.c"
  "src/cuddCheck.c"
  "src/cuddInteract.c"
  "src/cuddSplit.c"
  "src/cuddZddPort.c"
  "src/cuddAddNeg.c"
  "src/cuddClip.c"
  "src/cuddLCache.c"
  "src/cuddSubsetHB.c"
  "src/cuddZddReord.c"
  "src/cuddAddWalsh.c"
  "src/cuddCof.c"
  "src/cuddLevelQ.c"
  "src/cuddSubsetSP.c"
  "src/cuddZddSetop.c"
  "src/cuddAndAbs.c"
  "src/cuddCompose.c"
  "src/cuddLinear.c"
  "src/cuddSymmetry.c"
  "src/cuddZddSymm.c"
  "src/cuddAnneal.c"
  "src/cuddDecomp.c"
  "src/cuddLiteral.c"
  "src/cuddTable.c"
  "src/cuddZddUtil.c"
  "src/cuddApa.c"
  "src/cuddEssent.c"
  "src/cuddMatMult.c"
  "src/cuddUtil.c"
  "src/cuddAPI.c"
  "src/cuddExact.c"
  "src/cuddPriority.c"
  "src/cuddWindow.c"
  "src/cuddApprox.c"
  "src/cuddExport.c"
  "src/cuddRead.c"
  "src/cuddZddCount.c"
  "src/cuddBddAbs.c"
  "src/cuddGenCof.c"
  "src/cuddRef.c"
  "src/cuddZddFuncs.c"
  "src/epd.c"
  "src/mtrBasic.c"
  "src/mtrGroup.c"
  "src/st.c"
  "src/util/cpu_stats.c"
  "src/util/cpu_time.c"
  "src/util/cstringstream.c"
  "src/util/datalimit.c"
  "src/util/pathsearch.c"
  "src/util/pipefork.c"
  "src/util/prtime.c"
  "src/util/safe_mem.c"
  "src/util/strsav.c"
  "src/util/texpand.c"
  "src/util/ucbqsort.c"
)

if(CUDD_BUILD_CPP_API)
  set(CUDD_PUBLIC_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/cudd/cudd.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/cudd/cuddObj.hh"
    ${CUDD_PUBLIC_HEADERS}
  )
  set(CUDD_SOURCES
    "src/cudd.cpp"
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

# Link the math library (libm)
find_library(MATH_LIBRARY m REQUIRED)
target_link_libraries(cudd PUBLIC ${MATH_LIBRARY})

# Include directories (build and install time)
target_include_directories(cudd PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/extras/dddmp/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/extras/dddmp/src>
  $<INSTALL_INTERFACE:include>
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
if(CUDD_BUILD_WITH_STATS)
  target_compile_definitions(cudd PUBLIC DD_STATS)
endif()

###
### Export and Install
###

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

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
if(CUDD_BUILD_DDDMP)
  add_subdirectory(extras/dddmp)
endif()

# Test Build
include(CTest)
if(CUDD_BUILD_TESTS)
  add_subdirectory(extras/nanotrav)
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
