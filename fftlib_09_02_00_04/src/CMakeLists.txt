# Build the common library as it's needed by all kernels
add_subdirectory(common)

if(BUILD_LIB)

  add_library(FFTLIB STATIC)
  fftlib_set_lib_extension(FFTLIB)

  # add_subdirectory(FFTLIB_round) add_subdirectory(FFTLIB_recip)

  # ############################################################################
  # CMake people do not recommend this; not sure about the correct way, needs
  # review ##
  # ############################################################################

  # build fft_c7x
  file(GLOB SUBDIRS "${CMAKE_CURRENT_LIST_DIR}/fft_c7x/*")
  # message("sub dirs are ${SUBDIRS}")

  foreach(subdir ${SUBDIRS})
    if(IS_DIRECTORY ${subdir})
      # message("Found directory ${subdir}")

      get_filename_component(nameOnly ${subdir} NAME)

      if(NOT ${nameOnly} STREQUAL "common")

        # message ("Adding kernel name ${nameOnly}")

        add_subdirectory(${subdir})
        target_link_libraries(FFTLIB "${nameOnly}_obj")

      endif()
    endif()
  endforeach()

  if(NOT ${DEVICE} STREQUAL "C7504")
    # build fft_c7xmma
    file(GLOB SUBDIRS "${CMAKE_CURRENT_LIST_DIR}/fft_c7xmma/*")
    # message("sub dirs are ${SUBDIRS}")

    foreach(subdir ${SUBDIRS})
      if(IS_DIRECTORY ${subdir})
        # message("Found directory ${subdir}")

        get_filename_component(nameOnly ${subdir} NAME)

        if(NOT ${nameOnly} STREQUAL "common")

          # message ("Adding kernel name ${nameOnly}")

          add_subdirectory(${subdir})
          target_link_libraries(FFTLIB "${nameOnly}_obj")

        endif()
      endif()
    endforeach()

    # build linalg_c7xmma
    file(GLOB SUBDIRS "${CMAKE_CURRENT_LIST_DIR}/linalg_c7xmma/*")
    # message("sub dirs are ${SUBDIRS}")

    foreach(subdir ${SUBDIRS})
      if(IS_DIRECTORY ${subdir})
        # message("Found directory ${subdir}")

        get_filename_component(nameOnly ${subdir} NAME)

        if(NOT ${nameOnly} STREQUAL "common")

          # message ("Adding kernel name ${nameOnly}")

          add_subdirectory(${subdir})
          target_link_libraries(FFTLIB "${nameOnly}_obj")

        endif()
      endif()
    endforeach()
  endif()
  # get_target_property(OUT FFTLIB LINK_LIBRARIES) message("Linked objs are
  # ${OUT}")

elseif(BUILD_TEST)
  add_subdirectory(${KERNEL_NAME})

endif()
