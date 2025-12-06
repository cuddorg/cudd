include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# -------------------------------
# Install the library
# -------------------------------
install(
  TARGETS cudd
  EXPORT cuddTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cudd
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# -------------------------------
# Exported target file
# -------------------------------
install(
  EXPORT cuddTargets
  NAMESPACE cudd::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cudd
)

# -------------------------------
# Version and Config files
# -------------------------------
set(CONFIG_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/cudd")

# Version file (handles version checks)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/cuddConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

# Config file (relocatable, correct paths)
configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cuddConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cuddConfig.cmake"
  INSTALL_DESTINATION ${CONFIG_INSTALL_DIR}
)

install(
  FILES
    "${CMAKE_CURRENT_BINARY_DIR}/cuddConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/cuddConfigVersion.cmake"
  DESTINATION ${CONFIG_INSTALL_DIR}
)
