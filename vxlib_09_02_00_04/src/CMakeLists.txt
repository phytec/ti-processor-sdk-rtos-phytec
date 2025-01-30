# Build the common library as it's needed by all kernels
add_subdirectory(common)

if(BUILD_LIB)

  add_library(VXLIB STATIC)
  vxlib_set_lib_extension(VXLIB)

  # add_subdirectory(VXLIB_round) add_subdirectory(VXLIB_recip)

  # ############################################################################
  # CMake people do not recommend this; not sure about the correct way, needs
  # review ##
  # ############################################################################

  file(GLOB SUBDIRS "${CMAKE_CURRENT_LIST_DIR}/*")
  # message("sub dirs are ${SUBDIRS}")

  foreach(subdir ${SUBDIRS})
    if(IS_DIRECTORY ${subdir})
      # message("Found directory ${subdir}")

      get_filename_component(nameOnly ${subdir} NAME)

      if(NOT ${nameOnly} STREQUAL "common")

        # message ("Adding kernel name ${nameOnly}")

        add_subdirectory(${subdir})
        target_link_libraries(VXLIB "${nameOnly}_obj")

      endif()
    endif()
  endforeach()

  # get_target_property(OUT VXLIB LINK_LIBRARIES) message("Linked objs are
  # ${OUT}")

elseif(BUILD_TEST)
  add_subdirectory(${KERNEL_NAME})

endif()
