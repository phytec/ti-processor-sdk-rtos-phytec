# Call util function to create rules for building object libs for this kernel

# if(NOT BUILD_LIB) set(extKernelName "FFTLIB_")
# add_subdirectory("../${extKernelName}" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
# endif()

fftlib_create_object_lib_for_kernel()
