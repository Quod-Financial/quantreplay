function(ensure_dependency_target_exist TARGET_NAME DEPENDENT_NAME)
  if(NOT TARGET ${TARGET_NAME})
    string(CONCAT ERROR_MESSAGE
      "Failed to configure ${DEPENDENT_NAME}, "
      "required dependency target ${TARGET_NAME} does not exits")
    message(FATAL_ERROR ${ERROR_MESSAGE})
  else()
    message(DEBUG "Found ${DEPENDENT_NAME} dependency - ${TARGET_NAME}")
  endif()
endfunction()

macro(ensure_project_dependency_exist TARGET_NAME)
  ensure_dependency_target_exist(${TARGET_NAME} ${PROJECT_NAME})
endmacro()