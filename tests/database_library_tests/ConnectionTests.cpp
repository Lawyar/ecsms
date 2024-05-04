////////////////////////////////////////////////////////////////////////////////
//
/**
  Тесты для IConnection
*/
//
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include "TestSettings.h"

#include <Utils/StringUtils.h>

#include <IDatabaseManager.h>


/// Можем получить валидное соединение к базе данных, передав валидный URL
TEST(Connection, ConnectionWithValidURLIsValid) {
	auto && databaseManager = GetDatabaseManager();
	IConnectionPtr connection = databaseManager.GetConnection(c_PostgreSQLConnectionURL);
	ASSERT_NE(connection, nullptr);
	ASSERT_TRUE(connection->IsValid());
	ASSERT_EQ(connection->GetStatus(), ConnectionStatus::Ok);
}


/// Можем получить невалидное соединение к базе данных, передав невалидный URL
TEST(Connection, ConnectionWithInvalidURLIsInvalid) {
	auto && databaseManager = GetDatabaseManager();
	IConnectionPtr connection = databaseManager.GetConnection("");
	ASSERT_NE(connection, nullptr);
	ASSERT_FALSE(connection->IsValid());
	ASSERT_EQ(connection->GetStatus(), ConnectionStatus::Bad);
}


/// Не можем выполнить запрос, переданный по невалидному соединению
TEST(Connection, InvalidConnectionExecuteReturnsInvalidResult) {
	auto && databaseManager = GetDatabaseManager();
	IConnectionPtr connection = databaseManager.GetConnection("");
	auto && result = connection->Execute(
		"CREATE TABLE IF NOT EXISTS InvalidConnectionExecuteReturnsInvalidResult(id integer PRIMARY KEY);");
	ASSERT_NE(result, nullptr);

	auto && status = result->GetCurrentExecuteStatus();
	ASSERT_NE(status, nullptr);
	ASSERT_TRUE(status->HasError());
	ASSERT_EQ(status->GetStatus(), ResultStatus::FatalError);
}


/// Можем выполнить запрос, переданный по валидному соединению
TEST(Connection, ValidConnectionExecuteReturnsValidResult) {
	auto && databaseManager = GetDatabaseManager();
	IConnectionPtr connection = databaseManager.GetConnection(c_PostgreSQLConnectionURL);

	// Создадим таблицу
	{
		auto && result = connection->Execute(
			"CREATE TABLE IF NOT EXISTS ValidConnectionExecuteReturnsValidResult(id integer PRIMARY KEY);");
		ASSERT_NE(result, nullptr);

		auto && status = result->GetCurrentExecuteStatus();
		ASSERT_NE(status, nullptr);
		ASSERT_FALSE(status->HasError());
		ASSERT_EQ(status->GetStatus(), ResultStatus::OkWithoutData);
	}

	// Удалим таблицу
	{
		auto && result = connection->Execute(
			"DROP TABLE ValidConnectionExecuteReturnsValidResult;");
		ASSERT_NE(result, nullptr);

		auto && status = result->GetCurrentExecuteStatus();
		ASSERT_NE(status, nullptr);
		ASSERT_FALSE(status->HasError());
		ASSERT_EQ(status->GetStatus(), ResultStatus::OkWithoutData);
	}
}


/// Выполнение запроса, содержащего ошибку, возвращает результат с ошибкой
TEST(Connection, InvalidQueryReturnsInvalidResult) {
	auto && databaseManager = GetDatabaseManager();
	IConnectionPtr connection = databaseManager.GetConnection(c_PostgreSQLConnectionURL);

	// Создадим таблицу
	{
		// Допущена ошибка в ключевом слове "EXIST" - правильно будет "EXISTS".
		auto && result = connection->Execute(
			"CREATE TABLE IF NOT EXIST InvalidQueryReturnsInvalidResult(id integer PRIMARY KEY);");
		ASSERT_NE(result, nullptr);

		auto && status = result->GetCurrentExecuteStatus();
		ASSERT_NE(status, nullptr);
		ASSERT_TRUE(status->HasError());
		ASSERT_EQ(status->GetStatus(), ResultStatus::FatalError);
		ASSERT_EQ(status->GetErrorMessage(),
			"[PostgreSQL Error] ERROR:  syntax error at or near \"EXIST\"\n"
			"LINE 1: CREATE TABLE IF NOT EXIST InvalidQueryReturnsInvalidResult(i...\n"
			"                            ^\n");
	}
}


/// Выполнение корректного запроса возвращает корректный результат
// В этом тесте проверяются все основные возможности IExecuteResult сразу
TEST(Connection, ValidQueryReturnsValidResult) {
	auto && databaseManager = GetDatabaseManager();
	IConnectionPtr connection = databaseManager.GetConnection(c_PostgreSQLConnectionURL);

	// Удалим таблицу, если она была
	{
		auto && result = connection->Execute(
			"DROP TABLE IF EXISTS ValidQueryReturnsValidResult;");
		ASSERT_NE(result, nullptr);

		auto && status = result->GetCurrentExecuteStatus();
		ASSERT_NE(status, nullptr);
		ASSERT_FALSE(status->HasError());
		ASSERT_EQ(status->GetStatus(), ResultStatus::OkWithoutData);
	}

	// Создадим таблицу, состоящую из двух полей - id и text
	{
		auto && result = connection->Execute(
			"CREATE TABLE ValidQueryReturnsValidResult(id integer PRIMARY KEY, data text);");
		ASSERT_NE(result, nullptr);

		auto && status = result->GetCurrentExecuteStatus();
		ASSERT_TRUE(status && !status->HasError() && status->GetStatus() == ResultStatus::OkWithoutData);
	}

	const int id = 1;
	const std::string data = "some text";

	// Вставим в эту таблицу одну строку
	{
		auto && result = connection->Execute(utils::string::Format(
			"INSERT INTO ValidQueryReturnsValidResult VALUES({}, '{}');",
			id, data)
		);
		ASSERT_NE(result, nullptr);

		auto && status = result->GetCurrentExecuteStatus();
		ASSERT_TRUE(status && !status->HasError() && status->GetStatus() == ResultStatus::OkWithoutData);
	}

	// Селектируем эту строку
	{
		auto && result = connection->Execute("SELECT * FROM ValidQueryReturnsValidResult;");
		ASSERT_NE(result, nullptr);

		auto && status = result->GetCurrentExecuteStatus();
		ASSERT_TRUE(status && !status->HasError() && status->GetStatus() == ResultStatus::OkWithData);

		ASSERT_EQ(result->GetRowCount(), 1);
		ASSERT_EQ(result->GetColCount(), 2);

		ASSERT_EQ(result->GetColName(0), "id");
		ASSERT_EQ(result->GetColName(1), "data");

		ASSERT_EQ(result->GetColType(0), SQLDataType::Integer);
		ASSERT_EQ(result->GetColType(1), SQLDataType::Text);
		ASSERT_EQ(result->GetColType(2), SQLDataType::Invalid);

		ASSERT_EQ(result->GetColIndex("id"), 0);
		ASSERT_EQ(result->GetColIndex("data"), 1);
		ASSERT_EQ(result->GetColIndex(""), IExecuteResult::InvalidIndex);

		ASSERT_TRUE(result->GetValue(0, 0).HasString());
		ASSERT_TRUE(result->GetValue(0, 1).HasString());

		ASSERT_EQ(result->GetValue(0, 0).ExtractString(), std::to_string(id));
		ASSERT_EQ(result->GetValue(0, 1).ExtractString(), data);
	}

	// Удалим таблицу
	{
		auto && result = connection->Execute(
			"DROP TABLE ValidQueryReturnsValidResult;");
		ASSERT_NE(result, nullptr);

		auto && status = result->GetCurrentExecuteStatus();
		ASSERT_NE(status, nullptr);
		ASSERT_FALSE(status->HasError());
		ASSERT_EQ(status->GetStatus(), ResultStatus::OkWithoutData);
	}
}


// Проверка GetColType для всех типов
TEST(Connection, CanGetColType)
{
	auto && databaseManager = GetDatabaseManager();
	IConnectionPtr connection = databaseManager.GetConnection(c_PostgreSQLConnectionURL);

	// Порядок (типов) аргументов в запросе должен соответствовать порядку типов в SQLDataType
	const std::string query = "SELECT 1, 'text', '\\x0123456789ABCDEF'::bytea, 1.5;";

	auto && result = connection->Execute(query);
	ASSERT_NE(result, nullptr);
	auto && status = result->GetCurrentExecuteStatus();
	ASSERT_TRUE(status && !status->HasError());

	for (int i = 0; i < static_cast<int>(SQLDataType::Count); ++i)
	{
		auto currentType = static_cast<SQLDataType>(i);
		ASSERT_EQ(result->GetColType(i), currentType);
	}
}


// Проверка GetColName
TEST(Connection, CanGetColName)
{
	auto && databaseManager = GetDatabaseManager();
	IConnectionPtr connection = databaseManager.GetConnection(c_PostgreSQLConnectionURL);

	const std::string query = "SELECT 1 as name, 2 as name, 'text' as \"Name 2\", 3;";
	auto && result = connection->Execute(query);
	ASSERT_NE(result, nullptr);
	auto && status = result->GetCurrentExecuteStatus();
	ASSERT_TRUE(status && !status->HasError());

	ASSERT_EQ(result->GetColName(0), "name");
	ASSERT_EQ(result->GetColName(1), "name");
	ASSERT_EQ(result->GetColName(2), "Name 2");
	ASSERT_EQ(result->GetColName(3), "?column?");
	ASSERT_TRUE(result->GetColName(4).empty());
}


// Проверка GetColIndex
TEST(Connection, CanGetColIndex)
{
	auto && databaseManager = GetDatabaseManager();
	IConnectionPtr connection = databaseManager.GetConnection(c_PostgreSQLConnectionURL);

	const std::string query = "SELECT 1 as name, 2 as name, 'text' as \"Name 2\", 3;";
	auto && result = connection->Execute(query);
	ASSERT_NE(result, nullptr);
	auto && status = result->GetCurrentExecuteStatus();
	ASSERT_TRUE(status && !status->HasError());

	ASSERT_EQ(result->GetColIndex("name"), 0);
	ASSERT_EQ(result->GetColIndex("naMe"), 0);
	ASSERT_EQ(result->GetColIndex("NAME"), 0);
	ASSERT_EQ(result->GetColIndex("Name 2"), IExecuteResult::InvalidIndex);
	ASSERT_EQ(result->GetColIndex("name 2"), IExecuteResult::InvalidIndex);
	ASSERT_EQ(result->GetColIndex("\"name 2\""), IExecuteResult::InvalidIndex);
	ASSERT_EQ(result->GetColIndex("\"Name 2\""), 2);
	ASSERT_EQ(result->GetColIndex("?column?"), 3);
	ASSERT_EQ(result->GetColIndex(""), IExecuteResult::InvalidIndex);
}


// Проверка GetRowCount
TEST(Connection, CanGetRowCount)
{
	auto && databaseManager = GetDatabaseManager();
	IConnectionPtr connection = databaseManager.GetConnection(c_PostgreSQLConnectionURL);

	{
		auto && result = connection->Execute("VALUES (1);");
		ASSERT_NE(result, nullptr);
		auto && status = result->GetCurrentExecuteStatus();
		ASSERT_TRUE(status && !status->HasError());

		ASSERT_EQ(result->GetRowCount(), 1);
	}

	{
		auto && result = connection->Execute("VALUES (1), (2);");
		ASSERT_NE(result, nullptr);
		auto && status = result->GetCurrentExecuteStatus();
		ASSERT_TRUE(status && !status->HasError());

		ASSERT_EQ(result->GetRowCount(), 2);
	}
}


// Проверка GetColCount
TEST(Connection, CanGetColCount)
{
	auto && databaseManager = GetDatabaseManager();
	IConnectionPtr connection = databaseManager.GetConnection(c_PostgreSQLConnectionURL);

	{
		auto && result = connection->Execute("SELECT;");
		ASSERT_NE(result, nullptr);
		auto && status = result->GetCurrentExecuteStatus();
		ASSERT_TRUE(status && !status->HasError());

		ASSERT_EQ(result->GetColCount(), 0);
	}

	{
		auto && result = connection->Execute("SELECT 1;");
		ASSERT_NE(result, nullptr);
		auto && status = result->GetCurrentExecuteStatus();
		ASSERT_TRUE(status && !status->HasError());

		ASSERT_EQ(result->GetColCount(), 1);
	}

	{
		auto && result = connection->Execute("SELECT 1, 2;");
		ASSERT_NE(result, nullptr);
		auto && status = result->GetCurrentExecuteStatus();
		ASSERT_TRUE(status && !status->HasError());

		ASSERT_EQ(result->GetColCount(), 2);
	}
}


// Проверка GetValue
TEST(Connection, CanGetValue)
{
	auto && databaseManager = GetDatabaseManager();
	IConnectionPtr connection = databaseManager.GetConnection(c_PostgreSQLConnectionURL);

	{
		auto && result = connection->Execute("SELECT 1, 'text', '\\xABCD'::bytea, NULL;");
		ASSERT_NE(result, nullptr);
		auto && status = result->GetCurrentExecuteStatus();
		ASSERT_TRUE(status && !status->HasError());

		{
			auto value = result->GetValue(0, 0);
			ASSERT_TRUE(value.HasValue());
			ASSERT_TRUE(value.HasString());
			ASSERT_FALSE(value.HasNull());
			ASSERT_EQ(value.ExtractString(), "1");
			ASSERT_FALSE(value.HasValue());
			ASSERT_FALSE(value.HasString());
			ASSERT_FALSE(value.HasNull());
		}

		{
			auto value = result->GetValue(0, 1);
			ASSERT_TRUE(value.HasValue());
			ASSERT_TRUE(value.HasString());
			ASSERT_FALSE(value.HasNull());
			ASSERT_EQ(value.ExtractString(), "text");
			ASSERT_FALSE(value.HasValue());
			ASSERT_FALSE(value.HasString());
			ASSERT_FALSE(value.HasNull());
		}

		{
			auto value = result->GetValue(0, 2);
			ASSERT_TRUE(value.HasValue());
			ASSERT_TRUE(value.HasString());
			ASSERT_FALSE(value.HasNull());
			ASSERT_EQ(value.ExtractString(), "\\xabcd");
			ASSERT_FALSE(value.HasValue());
			ASSERT_FALSE(value.HasString());
			ASSERT_FALSE(value.HasNull());
		}

		{
			auto value = result->GetValue(0, 3);
			ASSERT_TRUE(value.HasValue());
			ASSERT_FALSE(value.HasString());
			ASSERT_TRUE(value.HasNull());
			ASSERT_TRUE(value.ExtractString().empty());
			ASSERT_TRUE(value.HasValue());
			ASSERT_FALSE(value.HasString());
			ASSERT_TRUE(value.HasNull());
		}

		{
			auto value = result->GetValue(0, 4);
			ASSERT_FALSE(value.HasValue());
			ASSERT_FALSE(value.HasString());
			ASSERT_FALSE(value.HasNull());
			ASSERT_TRUE(value.ExtractString().empty());
			ASSERT_FALSE(value.HasValue());
			ASSERT_FALSE(value.HasString());
			ASSERT_FALSE(value.HasNull());
		}

		{
			auto value = result->GetValue(1, 0);
			ASSERT_FALSE(value.HasValue());
			ASSERT_FALSE(value.HasString());
			ASSERT_FALSE(value.HasNull());
			ASSERT_TRUE(value.ExtractString().empty());
			ASSERT_FALSE(value.HasValue());
			ASSERT_FALSE(value.HasString());
			ASSERT_FALSE(value.HasNull());
		}
	}
}


// Проверка конструкторов CellType
TEST(Connection, CellTypeConstructors)
{
	{
		IExecuteResult::CellType cell;
		ASSERT_FALSE(cell.HasValue());
		ASSERT_FALSE(cell.HasString());
		ASSERT_FALSE(cell.HasNull());
		ASSERT_TRUE(cell.ExtractString().empty());
		ASSERT_FALSE(cell.HasValue());
		ASSERT_FALSE(cell.HasString());
		ASSERT_FALSE(cell.HasNull());
	}

	{
		IExecuteResult::CellType cell(nullptr);
		ASSERT_TRUE(cell.HasValue());
		ASSERT_FALSE(cell.HasString());
		ASSERT_TRUE(cell.HasNull());
		ASSERT_TRUE(cell.ExtractString().empty());
		ASSERT_TRUE(cell.HasValue());
		ASSERT_FALSE(cell.HasString());
		ASSERT_TRUE(cell.HasNull());
	}

	{
		IExecuteResult::CellType cell("string");
		ASSERT_TRUE(cell.HasValue());
		ASSERT_TRUE(cell.HasString());
		ASSERT_FALSE(cell.HasNull());
		ASSERT_EQ(cell.ExtractString(), "string");
		ASSERT_FALSE(cell.HasValue());
		ASSERT_FALSE(cell.HasString());
		ASSERT_FALSE(cell.HasNull());
	}
}
