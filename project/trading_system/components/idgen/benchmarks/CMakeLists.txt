set(TESTED_TARGET ${COMPONENT_NAME})
set(PROJECT_BENCHMARKS_NAME ${TESTED_TARGET}_benchmarks)

#------------------------------------------------------------------------------#
# Benchmarks sources                                                           #
#------------------------------------------------------------------------------#

set(BENCHMARK_FILES idgen_benchmarks.cpp)

#------------------------------------------------------------------------------#
# Benchmarks target                                                            #
#------------------------------------------------------------------------------#

add_executable(${PROJECT_BENCHMARKS_NAME} idgen_benchmarks.cpp)
target_init(${PROJECT_BENCHMARKS_NAME})

#------------------------------------------------------------------------------#
# Benchmarks include directories                                               #
#------------------------------------------------------------------------------#

target_include_directories(${PROJECT_BENCHMARKS_NAME}
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  $<TARGET_PROPERTY:${TESTED_TARGET},INCLUDE_DIRECTORIES>)

#------------------------------------------------------------------------------#
# Benchmarks dependencies                                                      #
#------------------------------------------------------------------------------#

target_link_libraries(${PROJECT_BENCHMARKS_NAME}
  PRIVATE
    benchmark::benchmark
    simulator::cfg
    ${TESTED_TARGET}
    $<TARGET_PROPERTY:${TESTED_TARGET},LINK_LIBRARIES>)