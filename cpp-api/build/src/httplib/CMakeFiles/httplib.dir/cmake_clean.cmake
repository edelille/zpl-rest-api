file(REMOVE_RECURSE
  "libhttplib.a"
  "libhttplib.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/httplib.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
