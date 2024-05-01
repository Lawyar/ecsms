add_executable(database_library_tests database_library_tests.cpp)
target_link_libraries(database_library_tests PRIVATE database_library gtest_main)

# todo : Удалить, когда не надо будет явно инстанцировать PGDatabaseManager
target_include_directories(database_library_tests PRIVATE ${CMAKE_SOURCE_DIR}/libraries/database_library)
