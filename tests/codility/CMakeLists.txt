function(add_codility_test LIBRARY_TO_TEST TEST_SOURCE_FILE)
  set(TEST_TARGET_NAME ${LIBRARY_TO_TEST}_test)
  add_executable(${TEST_TARGET_NAME} ${TEST_SOURCE_FILE})
  target_link_libraries(
    ${TEST_TARGET_NAME} PRIVATE ${LIBRARY_TO_TEST} project_options project_warnings catch_main
  )
  set_target_properties(${TEST_TARGET_NAME} PROPERTIES FOLDER "codility")
  catch_discover_tests(
    ${TEST_TARGET_NAME} TEST_PREFIX "${UNIT_TEST_PREFIX}codility.${LIBRARY_TO_TEST}."
  )
endfunction()

add_codility_test(binary_gap BinaryGapTests.cpp)
add_codility_test(max_double_slice_sum MaxDoubleSliceSumTests.cpp)
add_codility_test(max_profit MaxProfitTests.cpp)
add_codility_test(max_slice_sum MaxSliceSumTests.cpp)
add_codility_test(min_abs_sum MinAbsSumTests.cpp)
add_codility_test(min_abs_sum_of_two MinAbsSumOfTwoTests.cpp)
add_codility_test(min_avg_two_slice MinAvgTwoSliceTests.cpp)
add_codility_test(molybdenum2019 MolybdenumTests.cpp)
add_codility_test(niobium2019 NiobiumTests.cpp)
add_codility_test(number_solitaire NumberSolitaireTests.cpp)
add_codility_test(rhodium2019 RhodiumTests.cpp)
add_codility_test(ruthenium2019 RutheniumTests.cpp)
add_codility_test(zirconium2019 ZirconiumTests.cpp)
