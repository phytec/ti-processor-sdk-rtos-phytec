enable_language(ASM)

if(TARGET_PLATFORM STREQUAL "PC")
  file(GLOB_RECURSE COMMON_FILES "*.cpp" "*.c" "*.h")
else()
  file(GLOB_RECURSE COMMON_FILES "*.cpp" "*.c" "*.h" "*.asm")
endif()
# message("common files are: ${COMMON_FILES}")

add_library(MATHLIB_test_common STATIC ${COMMON_FILES})

mathlib_set_lib_extension(MATHLIB_test_common)

target_include_directories(MATHLIB_test_common PUBLIC ../common .
                                                      ${C7x_INCLUDES})
