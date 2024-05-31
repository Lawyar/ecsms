add_executable(common_tests 
    SteadyClock_tests.cpp)
target_link_libraries(common_tests PRIVATE common gtest_main)
