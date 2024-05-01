#pragma once

#include <IConnection.h>
#include <IExecutorEAV.h>
#include <DataType/ISQLTypeConverter.h>

#include <map>
#include <memory>
#include <string>

//------------------------------------------------------------------------------
/**
  Интерфейс менеджера базы данных
*/
//---
class IDatabaseManager
{
public:
	/// Деструктор
	virtual ~IDatabaseManager() = default;
public:
	/// Получить соединение
	virtual IConnectionPtr GetConnection(const std::string & connectionInfo) = 0;

	/// Получить исполнитель EAV-запросов
	virtual IExecutorEAVPtr GetExecutorEAV(IConnectionPtr && connection) = 0;

	/// Получить конвертер SQL-типов
	virtual ISQLTypeConverterPtr GetSQLTypeConverter() const = 0;
};


/// Указатель на IDatabaseManager
using IDatabaseManagerPtr = std::shared_ptr<IDatabaseManager>;
