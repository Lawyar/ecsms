add_executable(database_library_tests
  DatabaseManagerTests.cpp
  ConnectionTests.cpp
  SQLTypeConverterTests.cpp
  SQLTypeIntegerTests.cpp
  SQLTypeTextTests.cpp
  SQLTypeByteArrayTests.cpp
  SQLTypeRemoteFileIdTests.cpp
  RemoteFileTests.cpp
  ExecutorEAVNamingRulesTests.cpp
  ExecutorEAVTests.cpp

  TestSettings.h
)

target_link_libraries(database_library_tests PRIVATE database_library gtest_main)
