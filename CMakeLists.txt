cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(MarketSimulator LANGUAGES CXX)

#-----------------------------------------------------------------------------#
# Includes                                                                    #
#-----------------------------------------------------------------------------#

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# In-source build guard
include(build/PreventInSourceBuilds)

# Load standard settings
include(build/StandardProjectSettings)

# Load utility to setup target for creating a package
include(build/Archive)

# Allow for static analysis options
include(utils/StaticAnalyzers)

# Utils
include(CheckDependencyExist)
include(TargetInit)
include(TargetTest)

#-----------------------------------------------------------------------------#
# Conan                                                                       #
#-----------------------------------------------------------------------------#

include(Conan)
run_conan()

#-----------------------------------------------------------------------------#
# Version                                                                     #
#-----------------------------------------------------------------------------#

include(build/Version)
configure_version(${SIM_VERSION})

#-----------------------------------------------------------------------------#
# Tests                                                                       #
#-----------------------------------------------------------------------------#

if(ENABLE_TESTING)
  enable_testing()
endif()

#-----------------------------------------------------------------------------#
# Subdirectories                                                              #
#-----------------------------------------------------------------------------#

add_subdirectory(project)
