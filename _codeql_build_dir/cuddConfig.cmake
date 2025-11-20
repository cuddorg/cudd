
####### Expanded from @PACKAGE_INIT@ by configure_package_config_file() #######
####### Any changes to this file will be overwritten by the next CMake run ####
####### The input file was cuddConfig.cmake.in                            ########

get_filename_component(PACKAGE_PREFIX_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

macro(set_and_check _var _file)
  set(${_var} "${_file}")
  if(NOT EXISTS "${_file}")
    message(FATAL_ERROR "File or directory ${_file} referenced by variable ${_var} does not exist !")
  endif()
endmacro()

macro(check_required_components _NAME)
  foreach(comp ${${_NAME}_FIND_COMPONENTS})
    if(NOT ${_NAME}_${comp}_FOUND)
      if(${_NAME}_FIND_REQUIRED_${comp})
        set(${_NAME}_FOUND FALSE)
      endif()
    endif()
  endforeach()
endmacro()

####################################################################################

# Check that the necessary targets file was found (installed by install(EXPORT...))
if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/cuddTargets.cmake")
    message(FATAL_ERROR "Could not find the installed CUDD targets file!")
endif()

# The library target is exposed under its namespace:
check_required_components(cudd)

if(NOT TARGET cudd::cudd)
    # If the target hasn't been defined, it means the targets file wasn't loaded 
    # automatically, so we load it manually as a fallback/check.
    include("${CMAKE_CURRENT_LIST_DIR}/cuddTargets.cmake")
endif()

# Check again after the include
if(TARGET cudd::cudd)
    # Set variables for backward compatibility (optional)
    # set(CUDD_LIBRARIES cudd::cudd)
    # set(CUDD_INCLUDE_DIRS $<TARGET_PROPERTY:cudd::cudd,INTERFACE_INCLUDE_DIRECTORIES>)
    
    message(STATUS "Found CUDD version 4.0.0.")
    message(STATUS "CUDD provides the imported target 'cudd::cudd'.")
else()
    # This should not happen if installation was successful
    message(FATAL_ERROR "CUDD targets were not found after loading 'cuddTargets.cmake'.")
endif()
