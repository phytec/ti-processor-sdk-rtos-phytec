function(FFTLIB_create_example_binary_name myKernelName)
  if(TARGET_PLATFORM STREQUAL "PC")
    set(exeName
        "example_${myKernelName}_${DEVICE}_x86_64"
        PARENT_SCOPE)
  else()
    set(exeName
        "example_${myKernelName}_${DEVICE}"
        PARENT_SCOPE)
  endif()
endfunction()

function(FFTLIB_create_binary_for_example_kernel)

  # file(GLOB_RECURSE SRC_FILES "*.c" "*.cpp" "*.h")
  file(GLOB_RECURSE SRC_FILES "*.cpp" "*.h")
  # message("source test files are: ${SRC_FILES}")
  get_filename_component(myKernelName "${CMAKE_CURRENT_LIST_DIR}" NAME)
  fftlib_create_example_binary_name(${myKernelName})
  add_executable(${exeName} ${SRC_FILES})
  fftlib_set_bin_extension(${exeName})
  target_link_libraries(${exeName} PRIVATE FFTLIB FFTLIB_common ${C7x_LIBS})

  if(TARGET_PLATFORM STREQUAL "PC")
    if(CMAKE_HOST_UNIX)
      target_link_options(${exeName} PRIVATE -coverage -lgcov)
    endif()
  endif()

  if(NOT TARGET_PLATFORM STREQUAL "PC")
    fftlib_set_linker_options_c7x(${exeName})
  endif()

  fftlib_finish_compilation(${exeName})

endfunction()
