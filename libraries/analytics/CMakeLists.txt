add_library (analytics
      inc/analytics.h

      src/analytics.cpp)

target_include_directories(analytics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_include_directories(analytics PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)