#pragma once

#include <IDatabaseManager.h>


//------------------------------------------------------------------------------
/**
  Класс менеджера базы данных
  todo : Возможная оптимизация - создание пула соединений (создать на старте
  программы несколько соединений, затем их использовать, а не создавать каждый раз
  заново. Когда из пула забирают соединение - асинхронно добавлять в него новое
  соединение).
*/
//---
class PGDatabaseManager : public IDatabaseManager
{
public:
	/// Получить соединение
	virtual IConnectionPtr GetConnection(const std::string & connectionInfo) override;

	/// Получить исполнитель EAV-запросов
	virtual IExecutorEAVPtr GetExecutorEAV(const IConnectionPtr &  connection) override;

	/// Получить конвертер SQL-типов
	virtual ISQLTypeConverterPtr GetSQLTypeConverter() const override;
};
