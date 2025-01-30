# ##############################################################################
# Function to change CMake's default lib prefix and extensions ##
# ##############################################################################

function(FFTLIB_set_lib_extension LIB_NAME)
  # message("TARGET_PLATFORM: ${TARGET_PLATFORM}, LIB_NAME:${LIB_NAME},
  # DEVICE:${DEVICE}")
  if(TARGET_PLATFORM STREQUAL "PC")
    if(CMAKE_HOST_WIN32)
      set_target_properties(${LIB_NAME} PROPERTIES PREFIX "")
      set_target_properties(${LIB_NAME} PROPERTIES SUFFIX
                                                   "_${DEVICE}_x86_64.lib")
    else()
      set_target_properties(${LIB_NAME} PROPERTIES PREFIX "")
      set_target_properties(${LIB_NAME} PROPERTIES SUFFIX "_${DEVICE}_x86_64.a")
    endif()

  else()
    set_target_properties(${LIB_NAME} PROPERTIES PREFIX "")
    set_target_properties(${LIB_NAME} PROPERTIES SUFFIX "_${DEVICE}.lib")

    # set(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> ru2 <TARGET> <LINK_FLAGS>
    # <OBJECTS>") set(CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> ru2  <TARGET>
    # <LINK_FLAGS> <OBJECTS>")

  endif()
endfunction()

# ##############################################################################
# Function to to add all sources in a given kernel directory and create an
# object library ##
# ##############################################################################

function(FFTLIB_create_object_lib_for_kernel)

  string(TOLOWER ${DEVICE} DEVICE_DIR)
  if(${DEVICE} STREQUAL "C7504")
    if(EXISTS ${CMAKE_CURRENT_LIST_DIR}/${DEVICE_DIR})
      file(GLOB KERNEL_C_FILES "*.c" "c7504/*.c")
      file(GLOB KERNEL_CPP_FILES "*.cpp" "c7504/*.cpp")
      file(GLOB KERNEL_H_FILES "*.h" "c7504/*.h")
    else()
      file(GLOB KERNEL_C_FILES "*.c" "c71/*.c")
      file(GLOB KERNEL_CPP_FILES "*.cpp" "c71/*.cpp")
      file(GLOB KERNEL_H_FILES "*.h" "c71/*.h")
    endif()
  else()
    file(GLOB KERNEL_C_FILES "*.c" "c71/*.c")
    file(GLOB KERNEL_CPP_FILES "*.cpp" "c71/*.cpp")
    file(GLOB KERNEL_H_FILES "*.h" "c71/*.h")
  endif()

  if(TARGET_PLATFORM STREQUAL "PC")
    set_source_files_properties(${KERNEL_C_FILES} PROPERTIES LANGUAGE CXX)
  endif()

  list(APPEND KERNEL_FILES ${KERNEL_CPP_FILES} ${KERNEL_C_FILES}
       ${KERNEL_H_FILES})

  # message("kernel files ${KERNEL_FILES}")

  # file(GLOB_RECURSE KERNEL_FILES "*.cpp" "*.h") message("kernel files are:
  # ${KERNEL_FILES}")
  get_filename_component(myKernelName "${CMAKE_CURRENT_LIST_DIR}" NAME)
  add_library("${myKernelName}_obj" OBJECT ${KERNEL_FILES})
  target_include_directories(
    "${myKernelName}_obj" PUBLIC "${CMAKE_SOURCE_DIR}/src/common" .
                                 "${CMAKE_SOURCE_DIR}/src/" ${C7x_INCLUDES})
endfunction()

# function (FFTLIB_print_binary_details myBinary)

# if (TARGET_PLATFORM STREQUAL "PC") message(" ")
# message("${BoldYellow}---------------------------------------------")
# message("${BoldYellow}   Binary Built for Host Emulation(x86_64)   ")
# message("${BoldYellow}---------------------------------------------")
# message("${BoldYellow}   Binary: ${myBinary}       ") message(" ")

# else () message("                                          ")
# message("${BoldYellow}---------------------------------------------")
# message("${BoldYellow}   Binary Built for Target                   ")
# message("${BoldYellow}---------------------------------------------")
# message("${BoldYellow}   Binary: ${myBinary}       ") message(" ") endif()

# endfunction ()

# ##############################################################################
# Function to print build details ##
# ##############################################################################

function(FFTLIB_print_build_details)
  if(TARGET_PLATFORM STREQUAL "PC")
    message("                                          ")
    message("${BoldYellow}------------------------------------------")
    message("${BoldYellow}   Building for Host Emulation(x86_64)    ")
    message("${BoldYellow}------------------------------------------")
    message("${BoldYellow}   DEVICE: ${DEVICE}                      ")
    message("                                          ")

  else()
    message("                                          ")
    message("${BoldBlue}------------------------------------------")
    message("${BoldBlue}   Building for Target                    ")
    message("${BoldBlue}------------------------------------------")
    message("${BoldBlue}   DEVICE: ${DEVICE}                      ")
    message("                                          ")
  endif()

endfunction()

# TODO: scoping exeName
function(FFTLIB_finish_compilation exeName)

  if(TARGET_PLATFORM STREQUAL "PC")
    set(myColor "yellow")
  else()
    set(myColor "blue")
  endif()

  if(AUTO_TEST)

  else()
    add_custom_target(
      FinalMessage ALL
      ${CMAKE_COMMAND}
      -E
      cmake_echo_color
      --${myColor}
      --bold
      "Compilation is over for ${CMAKE_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}/${exeName}"
      # COMMENT "Final Message"
    )

    add_dependencies(FinalMessage ${exeName})
  endif()

endfunction()

set(FFTLIB_C7x_linker_SOC_defines
    --define=j721e=1 --define=j721s2=2 --define=j784s4=3 --define=AM62A=4
    --define=AM67A=5)
set(FFTLIB_C7x_SOC_defines -D=j721e=1 -D=j721s2=2 -D=j784s4=3 -D=AM62A=4
                           -D=AM67A=5)
set(FFTLIB_x86_SOC_defines j721e=1 j721s2=2 j784s4=3 AM62A=4 AM67A=5)

# how do I do add a long list without using concat set(foo
# --display_error_number --diag_remark=10068 --abi=eabi) set(foo ${foo}
# --opt_level=3 --gen_opt_info=2 ) set(foo ${foo} --symdebug:none -os -mw
# --disable:NLC) set(foo ${foo} --auto_inline -mo=on --preproc_with_compile)
# set(FFTLIB_C7x_compile_release_options ${foo})

function(FFTLIB_compile_options)

  if(TARGET_PLATFORM STREQUAL "PC")
    add_compile_definitions(_HOST_BUILD)
    add_compile_definitions(__${DEVICE}_)
    add_compile_definitions(__C7X_UNSTABLE_API)
    add_compile_definitions(SOC=${SOC})
    add_compile_definitions(${FFTLIB_x86_SOC_defines})
    add_compile_definitions(FFTLIB_DEBUGPRINT=${FFTLIB_DEBUGPRINT})
    # add_compile_definitions(DEVM_TEST=${EVM_TEST})
    if(CMAKE_HOST_WIN32)
      add_compile_definitions(WIN32)
    else()

      add_compile_options(
        # --std=c++14
        -coverage
        -fPIC
        -Wall
        -g
        # -Wextra -Wconversion -Wsign-conversion
        -fms-extensions
        -Wno-write-strings
        -Wno-unknown-pragmas
        -fno-strict-aliasing
        $<$<CONFIG:DEBUG>:-g3>
        $<$<CONFIG:DEBUG>:-Og>
        $<$<CONFIG:RELEASE>:-O3>)
    endif()

    # add_compile_definitions ( TEST_CASE=${TEST_CASE} )

  else()
    string(REGEX REPLACE "^C" "" siliconVersion ${DEVICE})
    # if (CMAKE_BUILD_TYPE)
    add_compile_options(
      --silicon_version=${siliconVersion}
      ${FFTLIB_C7x_SOC_defines}
      -DFFTLIB_DEBUGPRINT=${FFTLIB_DEBUGPRINT}
      -DEVM_TEST
      -DSOC=${SOC}
      -D__C7X_UNSTABLE_API
      -DDISABLE_STACK_PROFILING
      --c++14
      -os
      # Need to figure out a way to pass var here
      # $<$<CONFIG:RELEASE>:${FFTLIB_C7x_compile_release_options}>
      $<$<CONFIG:RELEASE>:--opt_level=3>
      $<$<CONFIG:RELEASE>:--display_error_number>
      $<$<CONFIG:RELEASE>:--diag_remark=10068>
      $<$<CONFIG:RELEASE>:--abi=eabi>
      $<$<CONFIG:RELEASE>:--gen_opt_info=2>
      $<$<CONFIG:RELEASE>:--symdebug:none>
      $<$<CONFIG:RELEASE>:-os>
      $<$<CONFIG:RELEASE>:-mw>
      $<$<CONFIG:RELEASE>:--disable:NLC>
      $<$<CONFIG:RELEASE>:--auto_inline>
      $<$<CONFIG:RELEASE>:-mo=on>
      $<$<CONFIG:RELEASE>:--preproc_with_compile>
      $<$<CONFIG:DEBUG>:--opt_level=0>
      $<$<CONFIG:DEBUG>:--disable_inlining>
      $<$<CONFIG:DEBUG>:-g>
      # $<$<CONFIG:RELEASE>:${FFTLIB_C7x_compile_release_options}>
    )
    # endif()
  endif()

endfunction()

function(FFTLIB_set_linker_options_C7x exeName)
  set(myLinkerFile ${CMAKE_SOURCE_DIR}/cmake/linkers/${DEVICE}/lnk.cmd)
  set(myLinkerCmd --library=${myLinkerFile} --search_path=$ENV{CGT7X_ROOT}/lib
                  --reread_libs --ram_model)
  target_link_options(${exeName} BEFORE PRIVATE --define=SOC=${SOC})
  target_link_options(${exeName} PRIVATE ${FFTLIB_C7x_linker_SOC_defines}
                      ${myLinkerCmd})
endfunction()

function(fftlib_include_xlib_headers)

  include_directories(BEFORE SYSTEM src/)

endfunction()

function(fftlib_set_bin_extension exeNAME)
  # message("TARGET_PLATFORM: ${TARGET_PLATFORM}, LIB_NAME:${LIB_NAME},
  # DEVICE:${DEVICE}")
  if(TARGET_PLATFORM STREQUAL "PC")
    if(CMAKE_HOST_WIN32)
      set_target_properties(${exeNAME} PROPERTIES PREFIX "")
      set_target_properties(${exeNAME} PROPERTIES SUFFIX ".exe")
    else()
      set_target_properties(${exeNAME} PROPERTIES PREFIX "")
      set_target_properties(${exeNAME} PROPERTIES SUFFIX "")
    endif()

  else()
    set_target_properties(${exeNAME} PROPERTIES PREFIX "")
    set_target_properties(${exeNAME} PROPERTIES SUFFIX ".out")

    # set(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> ru2 <TARGET> <LINK_FLAGS>
    # <OBJECTS>") set(CMAKE_C_ARCHIVE_CREATE "<CMAKE_AR> ru2  <TARGET>
    # <LINK_FLAGS> <OBJECTS>")

  endif()
endfunction()
