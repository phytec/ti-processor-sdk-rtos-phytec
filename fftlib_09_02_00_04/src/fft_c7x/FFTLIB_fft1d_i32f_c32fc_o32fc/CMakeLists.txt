# Call util function to create rules for building object libs for this kernel

if(NOT BUILD_LIB)
  set(extKernelName1 "fft_c7x/FFTLIB_fft1dBatched_i32fc_c32fc_o32fc")
  # set(extKernelName2 "fft_c7x/FFTLIB_fft1d_i32fc_c32fc_o32fc")
  add_subdirectory("../../${extKernelName1}"
                   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${extKernelName1})
  # add_subdirectory("../../${extKernelName2}"
  # ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${extKernelName2})

endif()

fftlib_create_object_lib_for_kernel()
