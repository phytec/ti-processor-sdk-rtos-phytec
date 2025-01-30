enable_language(ASM)

# if(TARGET_PLATFORM STREQUAL "PC") file(GLOB_RECURSE COMMON_FILES "*.cpp"
# "*.c") set_source_files_properties(${COMMON_FILES}/*.c PROPERTIES LANGUAGE
# CXX) else() file(GLOB_RECURSE COMMON_FILES "*.cpp" "*.c" "*.asm") endif()
# message("common files are: ${COMMON_FILES}")

string(TOLOWER ${DEVICE} DEVICE_DIR)
file(GLOB COMMON_C_FILES "*.c" "c71/*.c" "${DEVICE_DIR}/*.c")
file(GLOB COMMON_CPP_FILES "*.cpp" "c71/*.cpp" "${DEVICE_DIR}/*.cpp")
file(GLOB COMMON_H_FILES "*.h" "c71/*.h" "${DEVICE_DIR}/*.h")
file(GLOB COMMON_ASM_FILES "*.asm" "c71/*.asm" "${DEVICE_DIR}/*.asm")

# list (APPEND "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
if(TARGET_PLATFORM STREQUAL "PC")
  set_source_files_properties(${COMMON_C_FILES} PROPERTIES LANGUAGE CXX)
endif()

if(TARGET_PLATFORM STREQUAL "PC")
  list(APPEND COMMON_FILES ${COMMON_CPP_FILES} ${COMMON_C_FILES}
       ${COMMON_H_FILES})
else()
  list(APPEND COMMON_FILES ${COMMON_CPP_FILES} ${COMMON_C_FILES}
       ${COMMON_H_FILES} ${COMMON_ASM_FILES})
endif()

add_library(FFTLIB_test_common STATIC ${COMMON_FILES})

fftlib_set_lib_extension(FFTLIB_test_common)

target_include_directories(FFTLIB_test_common PUBLIC ../common .
                                                     ${C7x_INCLUDES})
