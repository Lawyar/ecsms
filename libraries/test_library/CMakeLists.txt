add_library(test_library inc/foo.h src/foo.cpp)

target_include_directories(test_library PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
