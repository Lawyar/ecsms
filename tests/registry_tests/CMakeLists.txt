add_executable(registry_tests registry_tests.cpp)
target_link_libraries(registry_tests INTERFACE registry)
target_include_directories(registry_tests PRIVATE ${CMAKE_SOURCE_DIR}/libraries/registry/inc)
target_link_libraries(registry_tests PRIVATE gtest_main)
