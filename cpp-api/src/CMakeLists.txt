# Add appropriate libraries
add_subdirectory (api)
add_subdirectory (cupslib)
add_subdirectory (generatelib)
add_subdirectory (httplib)
add_subdirectory (jsonlib)
add_subdirectory (testing)
add_subdirectory (utility)

# Add the executable
add_executable (main main.cpp)
target_compile_features (main PRIVATE cxx_std_20)

# Linking the other libs
include_directories (.) # this directive allows for #include <... > syntax
target_link_libraries (
  main PUBLIC 
  testing
  api
  generatelib
  jsonlib
  cupslib
  httplib
  utility
)
target_include_directories(
  main PUBLIC
  "${PROJECT_BINARY_DIR}"
  "${PROJECT_SOURCE_DIR}/api"
  "${PROJECT_SOURCE_DIR}/cupslib"
  "${PROJECT_SOURCE_DIR}/generatelib"
  "${PROJECT_SOURCE_DIR}/httplib"
  "${PROJECT_SOURCE_DIR}/jsonlib"
  "${PROJECT_SOURCE_DIR}/testing"
  "${PROJECT_SOURCE_DIR}/utility")
