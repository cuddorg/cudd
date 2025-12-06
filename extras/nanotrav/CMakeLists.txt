set(NANOTRAV_BUILD_TESTS ${CUDD_BUILD_TESTS} CACHE BOOL "Build tests for NanoTrav")

set(NANOTRAV_SOURCES
  src/bnet.c
  src/chkMterm.c
  src/ntr.c
  src/ntrBddTest.c
  src/ntrHeap.c
  src/ntrMflow.c
  src/ntrShort.c
  src/ntrZddTest.c
  src/main.c
)

include(FetchContent)

FetchContent_Declare(
  dddmp
  GIT_REPOSITORY https://github.com/cuddorg/dddmp.git
  GIT_TAG        main
  SYSTEM
  EXCLUDE_FROM_ALL
) 
FetchContent_MakeAvailable(dddmp)

add_executable(nanotrav ${NANOTRAV_SOURCES} ${CUDD_SOURCES})

target_link_libraries(nanotrav PRIVATE cudd::dddmp)

set_target_properties(nanotrav PROPERTIES
  OUTPUT_NAME               "nanotrav"
  C_STANDARD                "${CMAKE_C_STANDARD}"
  C_STANDARD_REQUIRED       "${CMAKE_C_STANDARD_REQUIRED}"
  C_EXTENSIONS              "${CMAKE_C_EXTENSIONS}"
)

if(NANOTRAV_BUILD_TESTS)
  add_subdirectory(tests)
endif()
