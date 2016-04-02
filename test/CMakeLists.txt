add_executable(context_unittest context_unittest.cpp)
target_link_libraries(context_unittest gtest gtest_main)
add_test(test_context context_unittest)
#
