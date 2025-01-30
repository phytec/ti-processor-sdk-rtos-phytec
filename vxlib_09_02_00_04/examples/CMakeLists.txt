message("------------------------------------------")
message("   Building example for ${KERNEL_NAME}    ")
message("------------------------------------------")

include(${CMAKE_SOURCE_DIR}/cmake/examples.cmake)
add_subdirectory(${KERNEL_NAME})
