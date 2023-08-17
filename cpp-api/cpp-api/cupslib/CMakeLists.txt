# CUPS Prerequisites
find_package (Cups REQUIRED)


# Debug Zone
include(CMakePrintHelpers)
cmake_print_variables(CUPS_FOUND)
cmake_print_variables(CUPS_VERSION_STRING)
cmake_print_variables(CUPS_LIBRARY)
cmake_print_variables(CUPS_INCLUDE_DIR)

add_library (
  cupslib SHARED
  cupslib.hpp
  cupslib.cpp)
 
# If found, set
if (CUPS_FOUND)
  include_directories(${CUPS_INCLUDE_DIR})
  target_link_libraries(cupslib ${CUPS_LIBRARIES})
endif (CUPS_FOUND)
