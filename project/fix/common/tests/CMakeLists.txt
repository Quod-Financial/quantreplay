set(TESTED_TARGET ${COMPONENT_NAME})
set(TEST_TARGET ${TESTED_TARGET}_tests)

#------------------------------------------------------------------------------#
# Component tests dependencies                                                 #
#------------------------------------------------------------------------------#

ensure_project_dependency_exist(GTest::GTest)
ensure_project_dependency_exist(simulator::cfg)

#------------------------------------------------------------------------------#
# Component tests files                                                        #
#------------------------------------------------------------------------------#

set(UNIT_TESTS
  unit_tests/mapping/checks_tests.cpp
  unit_tests/mapping/from_fix_conversion_tests.cpp
  unit_tests/mapping/from_fix_mapping_tests.cpp
  unit_tests/mapping/to_fix_conversion_tests.cpp
  unit_tests/mapping/setting/mapping_settings_tests.cpp
  unit_tests/session_conversion_tests.cpp
  unit_tests/settings_tests.cpp)

#------------------------------------------------------------------------------#
# Component tests                                                              #
#------------------------------------------------------------------------------#

add_executable(${TEST_TARGET}
  ${API_TESTS}
  ${UNIT_TESTS}
  test_main.cpp)

target_init(${TEST_TARGET})

#------------------------------------------------------------------------------#
# Component tests include directories                                          #
#------------------------------------------------------------------------------#

target_include_directories(${TEST_TARGET}
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    $<TARGET_PROPERTY:${TESTED_TARGET},INCLUDE_DIRECTORIES>)

#------------------------------------------------------------------------------#
# Component tests libraries                                                    #
#------------------------------------------------------------------------------#

target_link_libraries(${TEST_TARGET}
  PRIVATE
    GTest::GTest
    simulator::cfg
    ${TESTED_TARGET}
    $<TARGET_PROPERTY:${TESTED_TARGET},LINK_LIBRARIES>)

#------------------------------------------------------------------------------#
# Component tests target setup                                                 #
#------------------------------------------------------------------------------#

setup_gtest_target(${TEST_TARGET})