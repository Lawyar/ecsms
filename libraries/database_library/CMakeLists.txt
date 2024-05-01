add_library(database_library 
  src/InternalExecuteResultStatus.cpp
  src/InternalExecuteResultStatus.h
  src/PGConnection.cpp
  src/PGConnection.h
  src/PGDatabaseManager.cpp
  src/PGDatabaseManager.h
  src/PGExecuteResult.cpp
  src/PGExecuteResult.h
  src/PGExecuteResultStatus.cpp
  src/PGExecuteResultStatus.h
  src/PGExecutorEAV.cpp
  src/PGExecutorEAV.h
  src/PGSQLDataTypeUtility.cpp
  src/PGSQLDataTypeUtility.h
  src/SQLUtility.cpp
  src/SQLUtility.h
  src/PGSQLTypeByteArray.cpp
  src/PGSQLTypeByteArray.h
  src/PGSQLTypeConverter.cpp
  src/PGSQLTypeConverter.h
  src/PGSQLTypeInteger.cpp
  src/PGSQLTypeInteger.h
  src/PGSQLTypeText.cpp
  src/PGSQLTypeText.h

  public_inc/IConnection.h
  public_inc/IDatabaseManager.h
  public_inc/IExecuteResult.h
  public_inc/IExecuteResultStatus.h
  public_inc/IExecutorEAV.h

  public_inc/DataType/ISQLType.h
  public_inc/DataType/ISQLTypeByteArray.h
  public_inc/DataType/ISQLTypeConverter.h
  public_inc/DataType/ISQLTypeInteger.h
  public_inc/DataType/ISQLTypeText.h
  public_inc/DataType/SQLDataType.h

  public_inc/Utils/StringUtils.cpp
  public_inc/Utils/StringUtils.h
  public_inc/Utils/StrongType.h
)

target_include_directories(database_library PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/public_inc)

find_package(PostgreSQL REQUIRED)

target_link_libraries(database_library PRIVATE PostgreSQL::PostgreSQL)