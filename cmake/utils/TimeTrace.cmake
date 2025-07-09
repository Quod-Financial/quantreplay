if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  option(ENABLE_BUILD_WITH_TIME_TRACE "Enable -ftime-trace to generate time tracing .json files on clang" OFF)
endif()

function(enable_time_trace project_name)

  if(ENABLE_BUILD_WITH_TIME_TRACE)
    add_compile_definitions(${project_name} INTERFACE -ftime-trace)
  endif()

endfunction()
