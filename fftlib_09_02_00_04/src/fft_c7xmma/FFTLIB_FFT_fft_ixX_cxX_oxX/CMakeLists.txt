# Call util function to create rules for building object libs for this kernel

if(NOT BUILD_LIB)
  set(extKernelName1 "fft_c7xmma/FFTLIB_FFT_dftSmall_ixX_cxX_oxX")
  set(extKernelName2 "fft_c7xmma/FFTLIB_FFT_dftLarge_ixX_cxX_oxX")
  set(extKernelName3
      "fft_c7xmma/FFTLIB_FFT_highRadixDecompositions_ixX_cxX_oxX")
  set(extKernelName4
      "linalg_c7xmma/FFTLIB_LINALG_matrixMatrixMultiply_ixX_ixX_oxX")

  add_subdirectory("../../${extKernelName1}"
                   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${extKernelName1})
  add_subdirectory("../../${extKernelName2}"
                   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${extKernelName2})
  add_subdirectory("../../${extKernelName3}"
                   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${extKernelName3})
  add_subdirectory("../../${extKernelName4}"
                   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${extKernelName4})
endif()

fftlib_create_object_lib_for_kernel()
