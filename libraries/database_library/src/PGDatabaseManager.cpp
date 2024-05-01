#include "PGDatabaseManager.h"

#include <PGConnection.h>
#include <PGExecutorEAV.h>

#include <PGSQLTypeConverter.h>

//------------------------------------------------------------------------------
/**
  Получить соединение
*/
//---
IConnectionPtr PGDatabaseManager::GetConnection(const std::string & connectionInfo)
{
	return std::make_shared<PGConnection>(connectionInfo);
}


//------------------------------------------------------------------------------
/**
  Получить исполнитель EAV-запросов
*/
//---
IExecutorEAVPtr PGDatabaseManager::GetExecutorEAV(IConnectionPtr && connection)
{
	return std::make_shared<PGExecutorEAV>(std::move(connection), GetSQLTypeConverter());
}


//------------------------------------------------------------------------------
/**
  Получить конвертер SQL-типов
*/
//---
ISQLTypeConverterPtr PGDatabaseManager::GetSQLTypeConverter() const
{
	return std::make_shared<PGSQLTypeConverter>();
}
