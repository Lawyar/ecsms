add_library (analytics
      inc/analytics.h

      src/analytics.cpp)

target_include_directories(database_library PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_include_directories(database_library PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)