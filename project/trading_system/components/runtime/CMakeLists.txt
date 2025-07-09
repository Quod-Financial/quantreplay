set(COMPONENT_NAME ${PROJECT_NAME}_runtime)

#------------------------------------------------------------------------------#

add_static_library(
  NAME ${COMPONENT_NAME}
  ALIAS ts::runtime
  HEADERS
    ih/chained_mux.hpp
    ih/loop_impl.hpp
    ih/mux_impl.hpp
    ih/one_second_rate_loop.hpp
    ih/simple_thread_pool.hpp
    ih/thread_pool_impl.hpp
    include/runtime/loop.hpp
    include/runtime/mux.hpp
    include/runtime/service.hpp
    include/runtime/thread_pool.hpp
  SOURCES
    src/chained_mux.cpp
    src/one_second_rate_loop.cpp
    src/runtime.cpp
    src/simple_thread_pool.cpp
  PUBLIC_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  PRIVATE_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE_DEPENDENCIES
    simulator::log
    Microsoft.GSL::GSL)

#------------------------------------------------------------------------------#

add_subdirectory(tests)
