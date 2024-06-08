#include "PGDatabaseManager.h"

#include <PGConnection.h>
#include <PGExecutorEAV.h>

#include <PGSQLTypeConverter.h>


//------------------------------------------------------------------------------
/**
  �������� �������� ���� ������
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
  �������� ����������
*/
//---
IConnectionPtr PGDatabaseManager::GetConnection(const std::string & connectionInfo)
{
	return PGConnection::Create(connectionInfo);
}


//------------------------------------------------------------------------------
/**
  �������� ����������� EAV-��������
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
  �������� ��������� SQL-�����
*/
//---
ISQLTypeConverterPtr PGDatabaseManager::GetSQLTypeConverter() const
{
	return std::make_shared<PGSQLTypeConverter>();
}
