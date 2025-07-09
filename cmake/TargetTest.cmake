option(ENABLE_TESTING "Enable Test Builds" ON)

include(GoogleTest)

# This is a "meta" target that is used to collect all tests
add_custom_target(all_tests)

function(target_test name target)

  # Add the test to the CTest registry
  add_test(NAME ${name} COMMAND ${target})

  # Make the overall test meta-target depend on this test
  add_dependencies(all_tests ${target})

endfunction()

function(setup_gtest_target TARGET)

  # Add the test to the CTest registry
  gtest_discover_tests(${TARGET} PROPERTIES LABELS "${PROJECT_NAME}")

  # Make the overall test meta-target depend on this test
  add_dependencies(all_tests ${TARGET})

endfunction()


# Declares a new GTest EXECUTABLE target.
#   TARGET: The name of the target that is being tested (required).
#   MODULE: The name of the test module that the target belongs to (optional).
#   HEADERS: The list of header tests header files (optional).
#   API_TESTS: The list of API tests source files (optional).
#   UNIT_TESTS: The list of unit tests source files (optional).
#   DEPENDENCIES: The list of additional test target dependencies (optional).
# Notes: GTest and tested target dependencies are automatically.
#        A test_main.cpp source file must exist in ${CMAKE_CURRENT_SOURCE_DIR}.
function(add_target_tests)
  if(NOT ENABLE_TESTING)
    return()
  endif()

  cmake_parse_arguments(
    TEST
    ""
    "TARGET"
    "HEADERS;API_TESTS;UNIT_TESTS;MISC_SOURCES;DEPENDENCIES"
    ${ARGN})

  add_executable_binary(
    NAME ${TEST_TARGET}_tests
    HEADERS
      ${TEST_HEADERS}
    SOURCES
      ${TEST_UNIT_TESTS}
      ${TEST_API_TESTS}
      ${TEST_MISC_SOURCES}
      test_main.cpp
    PRIVATE_INCLUDE_DIRECTORIES
      ${CMAKE_CURRENT_SOURCE_DIR}
      $<TARGET_PROPERTY:${TEST_TARGET},INCLUDE_DIRECTORIES>
    PRIVATE_DEPENDENCIES
      GTest::GTest
      ${TEST_DEPENDENCIES}
      ${TEST_TARGET}
      $<TARGET_PROPERTY:${TEST_TARGET},LINK_LIBRARIES>)

  # Make the overall test meta-target depend on this test target
  add_dependencies(all_tests ${TEST_TARGET}_tests)
  gtest_discover_tests(${TEST_TARGET}_tests PROPERTIES LABELS ${TEST_MODULE})
endfunction()
