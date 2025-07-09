project(fix
  LANGUAGES CXX
  DESCRIPTION
  "Market Simulator FIX protocol module")

#------------------------------------------------------------------------------#
# Internal components                                                          #
#------------------------------------------------------------------------------#

add_subdirectory(common)
add_subdirectory(acceptor)