if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  set(SANITIZERS)

  option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" FALSE)
  if(ENABLE_COVERAGE)
    message(STATUS "Code coverage enabled")
  endif()

  option(ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" FALSE)
  if(ENABLE_SANITIZER_ADDRESS)
    list(APPEND SANITIZERS "address")
    message(STATUS "Address sanitizer enabled")
  endif()

  option(ENABLE_SANITIZER_LEAK "Enable leak sanitizer" FALSE)
  if(ENABLE_SANITIZER_LEAK)
    list(APPEND SANITIZERS "leak")
    message(STATUS "Memory leak sanitizer enabled")
  endif()

  option(ENABLE_SANITIZER_UNDEFINED_BEHAVIOR "Enable undefined behavior sanitizer" FALSE)
  if(ENABLE_SANITIZER_UNDEFINED_BEHAVIOR)
    list(APPEND SANITIZERS "undefined")
    message(STATUS "Undefined behaviour sanitizer enabled")
  endif()

  option(ENABLE_SANITIZER_THREAD "Enable thread sanitizer" FALSE)
  if(ENABLE_SANITIZER_THREAD)
    if("address" IN_LIST SANITIZERS OR "leak" IN_LIST SANITIZERS)
      message(WARNING "Thread sanitizer does not work with Address and Leak sanitizer enabled")
    else()
      list(APPEND SANITIZERS "thread")
      message(STATUS "Thread sanitizer enabled")
    endif()
  endif()

  option(ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" FALSE)
  if(ENABLE_SANITIZER_MEMORY AND CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    if("address" IN_LIST SANITIZERS
        OR "thread" IN_LIST SANITIZERS
        OR "leak" IN_LIST SANITIZERS)
      message(WARNING "Memory sanitizer does not work with Address, Thread and Leak sanitizer enabled")
    else()
      list(APPEND SANITIZERS "memory")
      message(STATUS "Memory sanitizer enabled")
    endif()
  endif()

  list(JOIN SANITIZERS "," SANITIZERS)
endif()


function(enable_sanitizers project_name)
  string(COMPARE NOTEQUAL "${SANITIZERS}" "" SANITIZERS_SPECIFIED)

  target_compile_options(${project_name}
    INTERFACE
      $<$<BOOL:${ENABLE_COVERAGE}>:--coverage -O1 -g>
      $<$<BOOL:${SANITIZERS_SPECIFIED}>:-fsanitize=${SANITIZERS}>)

  target_link_options(${project_name}
    INTERFACE
      $<$<BOOL:${ENABLE_COVERAGE}>:--coverage>
      $<$<BOOL:${SANITIZERS_SPECIFIED}>:-fsanitize=${SANITIZERS}>)
endfunction()
