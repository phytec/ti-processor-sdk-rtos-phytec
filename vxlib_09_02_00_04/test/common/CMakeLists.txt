enable_language(ASM)

if(TARGET_PLATFORM STREQUAL "PC")
  file(GLOB_RECURSE COMMON_FILES "*.cpp" "*.c")
  set_source_files_properties(${COMMON_FILES}/*.c PROPERTIES LANGUAGE CXX)
else()
  file(GLOB_RECURSE COMMON_FILES "*.cpp" "*.c" "*.asm")
endif()
# message("common files are: ${COMMON_FILES}")

add_library(VXLIB_test_common STATIC ${COMMON_FILES})

vxlib_set_lib_extension(VXLIB_test_common)

target_include_directories(VXLIB_test_common PUBLIC ../common .
                                                     ${C7x_INCLUDES})
