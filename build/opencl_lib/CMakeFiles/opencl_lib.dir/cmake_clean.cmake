file(REMOVE_RECURSE
  "libopencl_lib.pdb"
  "libopencl_lib.a"
)

# Per-language clean rules from dependency scanning.
foreach(lang )
  include(CMakeFiles/opencl_lib.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
