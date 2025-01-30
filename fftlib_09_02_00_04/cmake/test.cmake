function(FFTLIB_create_test_binary_name myKernelName)
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

function(FFTLIB_create_binary_for_kernel_testing)

  if(TARGET_PLATFORM STREQUAL "PC")
    file(GLOB_RECURSE SRC_CPP_FILES "*.cpp")
    file(GLOB_RECURSE SRC_C_FILES "*.c")
    file(GLOB_RECURSE SRC_H_FILES "*.h")
    set_source_files_properties(${SRC_C_FILES} PROPERTIES LANGUAGE CXX)
    list(APPEND SRC_FILES ${SRC_CPP_FILES} ${SRC_C_FILES} ${SRC_H_FILES})
  else()
    file(GLOB_RECURSE SRC_FILES "*.cpp" "*.c" "*.h")
  endif()
  # message("source test files are: ${SRC_FILES}") file(GLOB_RECURSE SRC_FILES
  # "*.cpp" "*.h")
  get_filename_component(myKernelName "${CMAKE_CURRENT_LIST_DIR}" NAME)
  fftlib_create_test_binary_name(${myKernelName})

  # message ("Exe name is ${exeName}") message ("                      ")

  # TODO: is this acceptable, hanging exeName; returned from
  # FFTLIB_create_binary_extension()
  add_executable(${exeName} ${SRC_FILES})
  fftlib_set_bin_extension(${exeName})
  if(AUTO_TEST)
    target_link_libraries(${exeName} PRIVATE FFTLIB FFTLIB_common
                                             FFTLIB_test_common ${C7x_LIBS})
  else()
    target_link_libraries(
      ${exeName} PRIVATE "${myKernelName}_obj" "${extObjNames}" FFTLIB_common
                         FFTLIB_test_common ${C7x_LIBS})
  endif()

  if(TARGET_PLATFORM STREQUAL "PC")
    if(CMAKE_HOST_UNIX)
      target_link_options(${exeName} PRIVATE -coverage -lgcov)
    endif()
  endif()

  if(NOT AUTO_TEST)
    fftlib_add_coverage(${exeName})
    set(IDAT_DIR ${CMAKE_SOURCE_DIR}/test/FFTLIB_idat_gen/${myKernelName})
    if(EXISTS ${IDAT_DIR})
      fftlib_gen_test_cases(${myKernelName})
      add_dependencies(${exeName} gen_test_case_headers)
    endif()
  endif()

  # target_compile_definitions(${exeName} PUBLIC TEST_CASE=${TEST_CASE}
  # TEST_CATEGORY=${TEST_CATEGORY} )

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
    fftlib_set_linker_options_c7x(${exeName})
  endif()

  fftlib_finish_compilation(${exeName})

endfunction()
