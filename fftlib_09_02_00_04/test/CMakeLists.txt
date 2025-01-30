message("                                                     ")
message("-----------------------------------------------------")
message("   Generating configs for testing ${KERNEL_NAME}     ")
message("-----------------------------------------------------")
message("                                                     ")

include(${CMAKE_SOURCE_DIR}/cmake/coverage.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/test.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/gen_test_cases.cmake)
add_subdirectory(common)

if(AUTO_TEST)
  # TODO: complete dependency that can help with regression testing the library
  # file(GLOB SUBDIRS "${CMAKE_CURRENT_LIST_DIR}/*") message("sub dirs are
  # ${SUBDIRS}")

  # message("kernels_list: ${KERNEL_LIST}") foreach (kernel ${KERNEL_LIST})
  # message("kernel name: ${kernel}") endforeach ()

  # build fft_c7x
  file(GLOB SUBDIRS "${CMAKE_CURRENT_LIST_DIR}/fft_c7x/*")

  foreach(subdir ${SUBDIRS})
    if(IS_DIRECTORY ${subdir})
      get_filename_component(nameOnly ${subdir} NAME)

      if(NOT ${nameOnly} STREQUAL "FFTLIB_idat_gen")
        message("Adding kernel name ${nameOnly}")
        add_subdirectory(${subdir})
      endif()

    endif()
  endforeach()

  if(NOT ${DEVICE} STREQUAL "C7504")
    # build fft_c7xmma

    file(GLOB SUBDIRS "${CMAKE_CURRENT_LIST_DIR}/fft_c7xmma/*")

    foreach(subdir ${SUBDIRS})
      if(IS_DIRECTORY ${subdir})
        get_filename_component(nameOnly ${subdir} NAME)

        if(NOT ${nameOnly} STREQUAL "FFTLIB_idat_gen")
          message("Adding kernel name ${nameOnly}")
          add_subdirectory(${subdir})
        endif()
      endif()
    endforeach()
  endif()

else()
  add_subdirectory(${KERNEL_NAME})
endif()
