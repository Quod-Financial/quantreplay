include(cmake/build/CompilerWarnings.cmake)
include(cmake/utils/Sanitizers.cmake)
include(cmake/utils/TimeTrace.cmake)

function(target_init project_name)

  set_target_properties(${project_name}
    PROPERTIES
      CXX_STANDARD 20
      CXX_STANDARD_REQUIRED YES
      CXX_EXTENSIONS OFF
      ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
      LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
  )

  # Standard compiler warnings
  set_project_warnings(${project_name})

  # Sanitizer options if supported by compiler
  enable_sanitizers(${project_name})

  # Time trace options if supported by compiler
  enable_time_trace(${project_name})

endfunction()

# Declares a new STATIC LIBRARY cmake target.
#   NAME: The name of the target (single value, required).
#   ALIAS: The alias name for the target (single value, optional).
#   HEADERS: List of target header files (multiple values, optional).
#   SOURCES: List of target source files (multiple values, at least 1 required).
#   PUBLIC_INCLUDE_DIRECTORIES:
#     List of public include directories (multiple values, optional).
#   PRIVATE_INCLUDE_DIRECTORIES:
#     List of private include directories (multiple values, optional).
#   PUBLIC_DEPENDENCIES:
#     List of public targets to link with (multiple values, optional).
#   PRIVATE_DEPENDENCIES:
#     List of private targets to link with (multiple values, optional).
function(add_static_library)
  cmake_parse_arguments(
    TARGET
    ""
    "NAME;ALIAS"
    "HEADERS;SOURCES;PUBLIC_INCLUDE_DIRECTORIES;PRIVATE_INCLUDE_DIRECTORIES;PUBLIC_DEPENDENCIES;PRIVATE_DEPENDENCIES;PUBLIC_COMPILE_DEFINITIONS;PRIVATE_COMPILE_DEFINITIONS"
    ${ARGN})

  add_library(${TARGET_NAME} STATIC ${TARGET_HEADERS} ${TARGET_SOURCES})
  if(TARGET_ALIAS)
    add_library(${TARGET_ALIAS} ALIAS ${TARGET_NAME})
  endif()

  target_include_directories(${TARGET_NAME}
    PUBLIC ${TARGET_PUBLIC_INCLUDE_DIRECTORIES}
    PRIVATE ${TARGET_PRIVATE_INCLUDE_DIRECTORIES})

  target_link_libraries(${TARGET_NAME}
    PUBLIC ${TARGET_PUBLIC_DEPENDENCIES}
    PRIVATE ${TARGET_PRIVATE_DEPENDENCIES})

  target_compile_definitions(${TARGET_NAME}
    PUBLIC ${TARGET_PUBLIC_COMPILE_DEFINITIONS}
    PRIVATE ${TARGET_PRIVATE_COMPILE_DEFINITIONS})

  target_init(${TARGET_NAME})
endfunction()

# Declares a new EXECUTABLE cmake target.
#   NAME: The name of the target (single value, required).
#   HEADERS: List of target header files (multiple values, optional).
#   SOURCES: List of target source files (multiple values, at least 1 required).
#   PUBLIC_INCLUDE_DIRECTORIES:
#     List of public include directories (multiple values, optional).
#   PRIVATE_INCLUDE_DIRECTORIES:
#     List of private include directories (multiple values, optional).
#   PUBLIC_DEPENDENCIES:
#     List of public targets to link with (multiple values, optional).
#   PRIVATE_DEPENDENCIES:
#     List of private targets to link with (multiple values, optional).
function(add_executable_binary)
  cmake_parse_arguments(
    TARGET
    ""
    "NAME"
    "HEADERS;SOURCES;PUBLIC_INCLUDE_DIRECTORIES;PRIVATE_INCLUDE_DIRECTORIES;PUBLIC_DEPENDENCIES;PRIVATE_DEPENDENCIES"
    ${ARGN})

  add_executable(${TARGET_NAME} ${TARGET_HEADERS} ${TARGET_SOURCES})

  target_include_directories(${TARGET_NAME}
    PUBLIC ${TARGET_PUBLIC_INCLUDE_DIRECTORIES}
    PRIVATE ${TARGET_PRIVATE_INCLUDE_DIRECTORIES})

  target_link_libraries(${TARGET_NAME}
    PUBLIC ${TARGET_PUBLIC_DEPENDENCIES}
    PRIVATE ${TARGET_PRIVATE_DEPENDENCIES})

  target_init(${TARGET_NAME})
endfunction()