function(MATHLIB_create_example_binary_name myKernelName)
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

function(MATHLIB_create_binary_for_example_kernel)

  # file(GLOB_RECURSE SRC_FILES "*.c" "*.cpp" "*.h")
  file(GLOB_RECURSE SRC_FILES "*.cpp" "*.h")
  # message("source test files are: ${SRC_FILES}")
  get_filename_component(myKernelName "${CMAKE_CURRENT_LIST_DIR}" NAME)
  mathlib_create_example_binary_name(${myKernelName})
  add_executable(${exeName} ${SRC_FILES})
  mathlib_set_bin_extension(${exeName})
  target_link_libraries(${exeName} PRIVATE MATHLIB ${C7x_LIBS})

  if(NOT TARGET_PLATFORM STREQUAL "PC")
    mathlib_set_linker_options_c7x(${exeName})
  endif()

  mathlib_finish_compilation(${exeName})

endfunction()
