# Call util function to create rules for building object libs for this kernel

if(NOT BUILD_LIB)
  set(extKernelName
      "linalg_c7xmma/FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX")
  add_subdirectory("../../${extKernelName}"
                   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${extKernelName})
endif()

fftlib_create_object_lib_for_kernel()
