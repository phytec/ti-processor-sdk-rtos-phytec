cmake_minimum_required(VERSION 3.16.0)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/colorized_messages.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/cmake/clang-format.cmake)
# include(${CMAKE_CURRENT_LIST_DIR}/cmake/FindMathJax.cmake)

# Check if CGT7X_ROOT env variable is set message("CGT7X_ROOT:$ENV{CGT7X_ROOT}")
if(NOT DEFINED ENV{CGT7X_ROOT})
  message(
    FATAL_ERROR
      "${BoldRed} CGT7X_ROOT env variable is not set, CMake will exit.")
endif()

# Set release as default build type
if(NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE
        "Release"
        CACHE STRING "" FORCE)
  endif()
endif()

# CMake custom util functions
include(${CMAKE_CURRENT_LIST_DIR}/cmake/utils.cmake)
mathlib_print_build_details()

if(TARGET_PLATFORM STREQUAL "PC")
  set(CMAKE_EXPORT_COMPILE_COMMANDS
      ON
      CACHE INTERNAL "")

endif()

set(CMAKE_CXX_STANDARD 14)

# ##############################################################################
# Pick the correct tool chain file of HE vs C7x ##
# ##############################################################################

if(TARGET_PLATFORM STREQUAL "PC")

  set(C7x_INCLUDES "$ENV{CGT7X_ROOT}/host_emulation/include/${DEVICE}")

  if(CMAKE_HOST_WIN32)
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/cmake/msvc.cmake")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")

    # Place all bins static libs in known place
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib/")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/")
  else()
    set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/cmake/gcc.cmake")
    # Place all bins static libs in known place
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
        "${CMAKE_CURRENT_SOURCE_DIR}/lib/${CMAKE_BUILD_TYPE}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
        "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")
  endif()

else()

  # Place all bins static libs in known place
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
      "${CMAKE_CURRENT_SOURCE_DIR}/lib/${CMAKE_BUILD_TYPE}")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
      "${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")

  set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_LIST_DIR}/cmake/c7x.cmake")
  include_directories(BEFORE SYSTEM $ENV{CGT7X_ROOT}/include)

endif()

# Set compilation flags for release and debug based on TARGET_PLATFORM
mathlib_compile_options()
mathlib_include_xlib_headers()

project(MATHLIB)

set(CMAKE_EXPORT_COMPILE_COMMANDS
    ON
    CACHE INTERNAL "") # works
# ##############################################################################
# Create dependency based on testing a kernel or running the example for a given
# kernel #
# ##############################################################################

if(BUILD_TEST)

  add_subdirectory(test)

elseif(AUTO_TEST)

  set(BUILD_LIB 1)
  add_subdirectory(test)

elseif(BUILD_EXAMPLE)

  set(BUILD_LIB 1)
  add_subdirectory(examples)

  # if (TARGET_PLATFORM STREQUAL "DSP") # set(CMAKE_EXE_LINKER_FLAGS
  # "--library=${CMAKE_CURRENT_LIST_DIR}/build/bin/libMATHLIB.a
  # ${CMAKE_EXE_LINKER_FLAGS}") # message("This is the updated
  # CMAKE_EXE_LINKER_FLAGS:${CMAKE_EXE_LINKER_FLAGS}") endif()

endif()

# Rules for building src directory
add_subdirectory(src bin)
