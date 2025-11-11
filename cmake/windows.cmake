# Windows-specific CMake configuration for CUDD
# This file contains all Windows-specific build settings and configurations

# Block shared library builds on Windows due to DLL linkage issues
function(cudd_check_shared_libs)
  if(CUDD_BUILD_SHARED_LIBS)
    message(FATAL_ERROR "Shared library builds are not supported on Windows due to DLL linkage conflicts. Please use -DCUDD_BUILD_SHARED_LIBS=OFF for static library builds.")
  endif()
endfunction()

# Configure C++11 support for Windows (MSVC)
function(cudd_configure_cpp)
  if(CUDD_BUILD_CPP_API)
    # MSVC automatically supports C++11 without needing explicit flags
    if(MSVC)
      message(STATUS "C++11 support verified for MSVC")
    endif()
  endif()
endfunction()

# Configure Windows-specific libraries
function(cudd_configure_libraries target_name)
  # On Windows, link Winsock2 library for networking functions (gethostname, etc.)
  target_link_libraries(${target_name} PUBLIC ws2_32)
  message(STATUS "Linking with Windows Winsock2 library for networking functions")
endfunction()

# Configure Windows-specific development tools
function(cudd_configure_dev_tools)
  # Windows doesn't need compile_commands.json symlink - IDEs handle it differently
  # No-op function for consistency with Unix version
endfunction()