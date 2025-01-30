function(MATHLIB_create_test_binary_name myKernelName)
  if(TARGET_PLATFORM STREQUAL "PC")
    set(exeName
        "test_${myKernelName}_${DEVICE}_x86_64"
        PARENT_SCOPE)
  else()
    set(exeName
        "test_${myKernelName}_${DEVICE}"
        PARENT_SCOPE)
  endif()
endfunction()

function(MATHLIB_create_binary_for_kernel_testing)

  file(GLOB_RECURSE SRC_FILES "*.cpp" "*.h")
  # message("source test files are: ${SRC_FILES}")
  get_filename_component(myKernelName "${CMAKE_CURRENT_LIST_DIR}" NAME)
  mathlib_create_test_binary_name(${myKernelName})
  # message ("Exe name is ${exeName}") message ("                      ")

  # TODO: is this acceptable, hanging exeName; returned from
  # MATHLIB_create_binary_extension()

  add_executable(${exeName} ${SRC_FILES})
  mathlib_set_bin_extension(${exeName})

  if(AUTO_TEST)
    target_link_libraries(${exeName} PRIVATE MATHLIB MATHLIB_test_common
                                             ${C7x_LIBS})
  else()
    target_link_libraries(${exeName} PRIVATE "${myKernelName}_obj"
                                             MATHLIB_test_common ${C7x_LIBS})
  endif()
  # target_compile_definitions(${exeName} PUBLIC TEST_CASE=${TEST_CASE}
  # TEST_CATEGORY=${TEST_CATEGORY} )

  if(NOT AUTO_TEST)
    mathlib_gen_test_cases(${myKernelName})
    add_dependencies(${exeName} gen_test_case_headers)
  endif()

  if(${TEST_CASE})
    target_compile_definitions(${exeName} PUBLIC TEST_CASE=${TEST_CASE})
  endif()

  if(${TEST_CATEGORY})
    target_compile_definitions(${exeName} PUBLIC TEST_CATEGORY=${TEST_CATEGORY})
  endif()

  if(${ALL_TEST_CASES})
    target_compile_definitions(${exeName} PUBLIC ALL_TEST_CASES)
  elseif(${DEMO_CASE})
    target_compile_definitions(${exeName} PUBLIC DEMO_CASE)
  endif()

  if(NOT TARGET_PLATFORM STREQUAL "PC")
    mathlib_set_linker_options_c7x(${exeName})
  endif()

  mathlib_finish_compilation(${exeName})

endfunction()
