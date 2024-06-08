#include "PGDatabaseManager.h"

#include <PGConnection.h>
#include <PGExecutorEAV.h>

#include <PGSQLTypeConverter.h>

//------------------------------------------------------------------------------
/**
  �������� ����������
*/
//---
IConnectionPtr PGDatabaseManager::GetConnection(const std::string & connectionInfo)
{
	return std::make_shared<PGConnection>(connectionInfo);
}


//------------------------------------------------------------------------------
/**
  �������� ����������� EAV-��������
*/
//---
IExecutorEAVPtr PGDatabaseManager::GetExecutorEAV(IConnectionPtr && connection)
{
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
