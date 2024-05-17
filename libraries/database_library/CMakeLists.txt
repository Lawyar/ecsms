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
  src/SQLUtility.cpp
  src/SQLUtility.h
  
  src/PGSQLDataTypeUtility.cpp
  src/PGSQLDataTypeUtility.h
  src/PGSQLTypeByteArray.cpp
  src/PGSQLTypeByteArray.h
  src/PGSQLTypeConverter.cpp
  src/PGSQLTypeConverter.h
  src/PGSQLTypeInteger.cpp
  src/PGSQLTypeInteger.h
  src/PGSQLTypeText.cpp
  src/PGSQLTypeText.h
  src/PGSQLTypeRemoteFileId.cpp
  src/PGSQLTypeRemoteFileId.h
  src/PGRemoteFile.cpp
  src/PGRemoteFile.h
  src/PGExecutorEAVNamingRules.cpp
  src/PGExecutorEAVNamingRules.h

  inc/IConnection.h
  inc/IDatabaseManager.h
  inc/IExecuteResult.h
  inc/IExecuteResultStatus.h
  inc/IExecutorEAV.h
  inc/IFile.h
  inc/IExecutorEAVNamingRules.h

  inc/DataType/ISQLTypeRemoteFileId.h
  inc/DataType/ISQLType.h
  inc/DataType/ISQLTypeByteArray.h
  inc/DataType/ISQLTypeConverter.h
  inc/DataType/ISQLTypeInteger.h
  inc/DataType/ISQLTypeText.h
  inc/DataType/SQLDataType.h

  inc/Utils/StringUtils.cpp
  inc/Utils/StringUtils.h
  inc/Utils/StrongType.h
)

target_include_directories(database_library PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
target_include_directories(database_library PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

find_package(PostgreSQL REQUIRED)

target_link_libraries(database_library PRIVATE PostgreSQL::PostgreSQL)