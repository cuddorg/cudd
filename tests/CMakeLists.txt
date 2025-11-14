find_package(Catch2 CONFIG REQUIRED)

add_executable(
  cudd_tests
  main.test.cpp
)
target_link_libraries(cudd_tests PRIVATE
  Catch2::Catch2WithMain
  cudd::cudd
)
target_link_options(cudd_tests PRIVATE
  $<$<BOOL:${CUDD_BUILD_COVERAGE}>:--coverage>
)

if(CUDD_BUILD_COVERAGE)
  # Find tools
  find_program(GCOV gcov)
  find_program(LCOV lcov)
  find_program(GENHTML genhtml)

  if (NOT GCOV OR NOT LCOV OR NOT GENHTML)
    message(WARNING "Missing one or more coverage tools (gcov, lcov, genhtml)")
  else()
    # Ensure coverage flags are enabled for both cudd and cudd_tests
    target_compile_options(cudd PRIVATE --coverage)
    target_compile_options(cudd_tests PRIVATE --coverage)
    target_link_options(cudd_tests PRIVATE --coverage)

    # Coverage output
    set(COV_INFO ${CMAKE_BINARY_DIR}/coverage.info)
    set(COV_HTML_DIR ${CMAKE_BINARY_DIR}/coverage_html)

    add_custom_target(coverage
  
      # Capture baseline (all compiled files, even if not run)
      COMMAND ${LCOV} --gcov-tool ${GCOV}
                      --capture --initial
                      --directory .
                      --base-directory ${CMAKE_SOURCE_DIR}
                      --output-file coverage_base.info
                      --quiet

      # Run tests
      COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure

      # Capture test results
      COMMAND ${LCOV} --gcov-tool ${GCOV}
                      --capture
                      --directory .
                      --base-directory ${CMAKE_SOURCE_DIR}
                      --output-file coverage_test.info
                      --quiet

      # Combine and clean
      COMMAND ${LCOV} --add-tracefile coverage_base.info
                      --add-tracefile coverage_test.info
                      --output-file coverage.info
                      --quiet

      COMMAND ${LCOV} --remove coverage.info
                      '/usr/*' '/opt/*' '*/_deps/*' '*/tests/*' '*/external/*'
                      --output-file coverage.info
                      --quiet
                      --ignore-errors unused

      # View summary in terminal
      COMMAND ${LCOV} --list coverage.info

      # Generate HTML
      COMMAND ${GENHTML} coverage.info
                        --prefix ${CMAKE_SOURCE_DIR}
                        --output-directory coverage_html
                        --quiet

      COMMENT "Running tests and generating full coverage report..."
      DEPENDS cudd_tests
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )


    set_directory_properties(PROPERTIES ADDITIONAL_CLEAN_FILES "${COV_INFO};${COV_HTML_DIR}")
  endif()
endif()


include(Catch)
catch_discover_tests(cudd_tests)
