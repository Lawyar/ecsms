add_executable(analytics_tests )

target_link_libraries(analytics_tests PRIVATE analytics gtest_main gmock_main)
