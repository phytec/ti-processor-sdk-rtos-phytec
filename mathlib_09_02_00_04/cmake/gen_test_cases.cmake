function(mathlib_gen_test_cases kernelName)

  set(MATHLIB_TEST_PARAMS_DIR ${CMAKE_SOURCE_DIR}/test/MATHLIB_testParams_gen/)
  add_custom_command(
    OUTPUT "${CMAKE_SOURCE_DIR}/test/${kernelName}/${kernelName}_testParams.cpp"
    COMMAND python3 ${MATHLIB_TEST_PARAMS_DIR}/${kernelName}/gen_data.py
    DEPENDS ${MATHLIB_TEST_PARAMS_DIR}/${kernelName}/gen_data.py
            ${MATHLIB_TEST_PARAMS_DIR}/${kernelName}/test_cases_list.csv
    WORKING_DIRECTORY
      "${CMAKE_SOURCE_DIR}/test/MATHLIB_testParams_gen/${kernelName}")

  add_custom_target(
    gen_test_case_headers
    DEPENDS
      "${CMAKE_SOURCE_DIR}/test/${kernelName}/${kernelName}_testParams.cpp")

endfunction()

add_custom_target(
  gen_all_test_case_headers
  COMMAND ${CMAKE_SOURCE_DIR}/cmake/gen_all_test_case_headers.sh
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/cmake/)
