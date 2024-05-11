#include "PGDatabaseManager.h"

#include <PGConnection.h>
#include <PGExecutorEAV.h>

#include <PGSQLTypeConverter.h>


//------------------------------------------------------------------------------
/**
  Получить менеджер базы данных
*/
//---
template<>
IDatabaseManager & GetDatabaseManager<DatabaseType::PostgreSQL>()
{
	static PGDatabaseManager databaseManager;
	return databaseManager;
}


//------------------------------------------------------------------------------
/**
  Получить соединение
*/
//---
IConnectionPtr PGDatabaseManager::GetConnection(const std::string & connectionInfo)
{
	return PGConnection::Create(connectionInfo);
}


//------------------------------------------------------------------------------
/**
  Получить исполнитель EAV-запросов
*/
//---
IExecutorEAVPtr PGDatabaseManager::GetExecutorEAV(IConnectionPtr && connection)
{
	if (!connection || !connection->IsValid())
		return nullptr;

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
