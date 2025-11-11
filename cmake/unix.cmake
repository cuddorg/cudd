# Unix-specific CMake configuration for CUDD
# This file contains all Unix-specific build settings and configurations

# Configure shared library builds for Unix (no restrictions)
function(cudd_check_shared_libs)
  # Unix platforms support shared libraries without issues
  # No restrictions needed
endfunction()

# Configure C++11 support for Unix (non-MSVC compilers)
function(cudd_configure_cpp)
  if(CUDD_BUILD_CPP_API)
    # Check C++11 support for non-MSVC compilers (GCC, Clang, etc.)
    if(NOT MSVC)
      include(CheckCXXCompilerFlag)
      CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
      if(NOT COMPILER_SUPPORTS_CXX11)
        message(FATAL_ERROR "${CMAKE_CXX_COMPILER} has no C++11 support.")
      endif()
      message(STATUS "C++11 support verified for ${CMAKE_CXX_COMPILER_ID}")
    endif()
  endif()
endfunction()

# Configure Unix-specific libraries (math library)
function(cudd_configure_libraries target_name)
  # Link the math library (libm) on Unix-like systems
  # On Windows, math functions are built into the C runtime
  find_library(MATH_LIBRARY m)
  if(MATH_LIBRARY)
    target_link_libraries(${target_name} PUBLIC ${MATH_LIBRARY})
    message(STATUS "Linking with math library: ${MATH_LIBRARY}")
  else()
    message(STATUS "Math library not found, assuming math functions are built-in")
  endif()
endfunction()

# Configure Unix-specific development tools
function(cudd_configure_dev_tools)
  if(PROJECT_IS_TOP_LEVEL AND UNIX)
    # Create symlink to compile_commands.json for IDE to pick it up
    execute_process(
      COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
    )
    message(STATUS "Created compile_commands.json symlink for IDE integration")
  endif()
endfunction()
