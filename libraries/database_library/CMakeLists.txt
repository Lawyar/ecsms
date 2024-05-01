add_library(database_library public_inc/foo.h src/foo.cpp)

target_include_directories(database_library PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public_inc)

find_package(PostgreSQL REQUIRED)

target_link_libraries(database_library PRIVATE PostgreSQL::PostgreSQL)