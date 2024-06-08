#pragma once

#include <IConnection.h>
#include <IExecutorEAV.h>
#include <DataType/ISQLTypeConverter.h>

#include <map>
#include <memory>
#include <string>

//------------------------------------------------------------------------------
/**
  ��������� ��������� ���� ������
*/
//---
class IDatabaseManager
{
public:
	/// ����������
	virtual ~IDatabaseManager() = default;
public:
	/// �������� ����������
	virtual IConnectionPtr GetConnection(const std::string & connectionInfo) = 0;

	/// �������� ����������� EAV-��������
	virtual IExecutorEAVPtr GetExecutorEAV(IConnectionPtr && connection) = 0;

	/// �������� ��������� SQL-�����
	virtual ISQLTypeConverterPtr GetSQLTypeConverter() const = 0;
};


/// ��� ���� ������
enum class DatabaseType
{
	PostgreSQL
};


/// �������� �������� ���� ������
template<DatabaseType type = DatabaseType::PostgreSQL>
extern IDatabaseManager & GetDatabaseManager();
