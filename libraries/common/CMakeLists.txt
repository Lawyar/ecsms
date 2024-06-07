add_library(common
    inc/SteadyClock.h
    
    src/SteadyClock.cpp)

target_include_directories(common PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
