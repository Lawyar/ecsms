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


// Проверка открытия транзакции без корректного закрытия
TEST(Connection, JustBeginTransaction)
{
	auto && databaseManager = GetDatabaseManager();

	{
		IConnectionPtr connection = databaseManager.GetConnection(c_PostgreSQLConnectionURL);
		auto status = connection->BeginTransaction();
		ASSERT_FALSE(status->HasError());

		connection->Execute("CREATE TABLE JustBeginTransaction(id INTEGER PRIMARY KEY);");
		// Не закрываем транзакцию
	}

	IConnectionPtr connection = databaseManager.GetConnection(c_PostgreSQLConnectionURL);
	auto result = connection->Execute("SELECT * FROM JustBeginTransaction;");
	ASSERT_TRUE(result->GetCurrentExecuteStatus()->HasError());
}


// Проверка открытия транзакции с последующим закрытием
TEST(Connection, BeginCommitTransaction)
{
	auto && databaseManager = GetDatabaseManager();

	{
		IConnectionPtr connection = databaseManager.GetConnection(c_PostgreSQLConnectionURL);
		auto status = connection->BeginTransaction();
		ASSERT_FALSE(status->HasError());

		connection->Execute("CREATE TABLE BeginCommitTransaction(id INTEGER PRIMARY KEY);");
		
		status = connection->CommitTransaction();
		ASSERT_FALSE(status->HasError());
	}

	IConnectionPtr connection = databaseManager.GetConnection(c_PostgreSQLConnectionURL);
	auto result = connection->Execute("SELECT * FROM BeginCommitTransaction;");
	ASSERT_FALSE(result->GetCurrentExecuteStatus()->HasError());

	// Удалим таблицу
	result = connection->Execute("DROP TABLE BeginCommitTransaction;");
	ASSERT_FALSE(result->GetCurrentExecuteStatus()->HasError());
}


// Проверка открытия транзакции с последующей отменой
TEST(Connection, BeginRollbackTransaction)
{
	auto && databaseManager = GetDatabaseManager();

	{
		IConnectionPtr connection = databaseManager.GetConnection(c_PostgreSQLConnectionURL);
		auto status = connection->BeginTransaction();
		ASSERT_FALSE(status->HasError());

		connection->Execute("CREATE TABLE BeginRollbackTransaction(id INTEGER PRIMARY KEY);");
		status = connection->RollbackTransaction();
		ASSERT_FALSE(status->HasError());
	}

	IConnectionPtr connection = databaseManager.GetConnection(c_PostgreSQLConnectionURL);
	auto result = connection->Execute("SELECT * FROM BeginRollbackTransaction;");
	ASSERT_TRUE(result->GetCurrentExecuteStatus()->HasError());
}


// Проверка GetColType для всех типов
TEST(Connection, CanGetColType)
{
	auto && databaseManager = GetDatabaseManager();
	IConnectionPtr connection = databaseManager.GetConnection(c_PostgreSQLConnectionURL);

	// Порядок (типов) аргументов в запросе должен соответствовать порядку типов в SQLDataType
	const std::string query = "SELECT 1, 'text', '\\x0123456789ABCDEF'::bytea, 20000::oid, 1.5;";

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


/// Команды выполняются транзакционно
TEST(Connection, CommandsAreExecutedTransactionally) {
	auto && databaseManager = GetDatabaseManager();
	IConnectionPtr connection = databaseManager.GetConnection(c_PostgreSQLConnectionURL);

	{
		auto && result = connection->Execute(
			"DROP TABLE IF EXISTS CommandsAreExecutedTransactionally;\n"
			"CREATE TABLE CommandsAreExecutedTransactionally(id INTEGER PRIMARY KEY);\n"
			"INSERT INTO CommandsAreExecutedTransactionally VALUES(1);\n"
			// В этой команде допущена ошибка - "FOM" вместо "FROM"
			"SELECT * FOM CommandsAreExecutedTransactionally;\n"
		);

		auto && status = result->GetCurrentExecuteStatus();
		ASSERT_TRUE(status->HasError());
	}

	{
		auto && result = connection->Execute(
			"SELECT * FROM CommandsAreExecutedTransactionally;\n"
		);

		auto && status = result->GetCurrentExecuteStatus();

		// Если бы команды выполнялись не транзакционно, то таблица бы создалась на предыдущем вызове.
		// И здесь мы бы получили корректный результат, а не ошибку.
		ASSERT_TRUE(status->HasError());
	}
}


/// По валидному соединению можно создать и удалить файл
TEST(Connection, CanCreateAndDeleteRemoteFileWithValidConnection)
{
	auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
	auto remoteFile = connection->CreateRemoteFile();
	ASSERT_NE(remoteFile, nullptr);
	ASSERT_FALSE(remoteFile->GetFileName().empty());
	ASSERT_TRUE(connection->DeleteRemoteFile(remoteFile->GetFileName()));
}


/// По невалидному соединению нельзя создать файл
TEST(Connection, CantCreateRemoteFileWithValidConnection)
{
	auto && connection = GetDatabaseManager().GetConnection("");
	auto remoteFile = connection->CreateRemoteFile();
	ASSERT_EQ(remoteFile, nullptr);
}


/// По невалидному соединению нельзя получить файл
TEST(Connection, CantGetExistingRemoteFileWithInvalidConnection)
{
	auto && connection = GetDatabaseManager().GetConnection("");
	auto remoteFilePtr = connection->GetRemoteFile("100000");
	ASSERT_EQ(remoteFilePtr, nullptr);
}


/// По валидному соединению можно получить существующий удаленный файл, и он доступен
TEST(Connection, CanGetExistingRemoteFileWithValidConnection)
{
	std::string createdFileName;
	{
		// Создадим файл
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto remoteFile = connection->CreateRemoteFile();
		createdFileName = remoteFile->GetFileName();
		ASSERT_FALSE(createdFileName.empty());
	}

	{
		// Попытаемся его получить
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto remoteFile = connection->GetRemoteFile(createdFileName);
		ASSERT_EQ(remoteFile->GetFileName(), createdFileName);

		auto status = connection->BeginTransaction();
		ASSERT_FALSE(status->HasError());

		// Проверка на доступность
		ASSERT_TRUE(remoteFile->Open());

		status = connection->CommitTransaction();
		ASSERT_FALSE(status->HasError());
	}

	{
		// Удалим файл
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		ASSERT_TRUE(connection->DeleteRemoteFile(createdFileName));
	}
}


/// По валидному соединению можно получить несуществующий удаленный файл, но он будет недоступен
TEST(Connection, CanGetNonExistingRemoteFileWithValidConnection)
{
	{
		// Попытаемся получить несуществующий файл
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto remoteFile = connection->GetRemoteFile("100000");
		ASSERT_NE(remoteFile, nullptr);

		auto status = connection->BeginTransaction();
		ASSERT_FALSE(status->HasError());

		// Проверка на недоступность (раз файл не существует, то не должен открываться)
		ASSERT_FALSE(remoteFile->Open());

		status = connection->CommitTransaction();
		ASSERT_FALSE(status->HasError());
	}
}


/// По валидному соединению нельзя получить файл с невалидным именем
TEST(Connection, CantGetRemoteFileWithInvalidName)
{
	auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
	auto remoteFilePtr = connection->GetRemoteFile("");
	ASSERT_EQ(remoteFilePtr, nullptr);
}


/// Файл не создается, если транзакция оборвалась
TEST(Connection, CantCreateRemoteFileWithFailedTransaction)
{
	std::string createdFileName;
	{
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto status = connection->BeginTransaction();
		ASSERT_FALSE(status->HasError());
		auto remoteFile = connection->CreateRemoteFile();
		createdFileName = remoteFile->GetFileName();
		// Обрыв транзакции
	}

	auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
	auto remoteFilePtr = connection->GetRemoteFile(createdFileName);
	auto status = connection->BeginTransaction();
	ASSERT_FALSE(status->HasError());
	ASSERT_FALSE(remoteFilePtr->Open());
	status = connection->CommitTransaction();
	ASSERT_FALSE(status->HasError());
}


/// Файл не создается, если транзакция была отменена
TEST(Connection, CantCreateRemoteFileWithRollbackTransaction)
{
	std::string createdFileName;
	{
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto status = connection->BeginTransaction();
		ASSERT_FALSE(status->HasError());
		auto remoteFile = connection->CreateRemoteFile();
		createdFileName = remoteFile->GetFileName();

		status = connection->RollbackTransaction();
		ASSERT_FALSE(status->HasError());
	}

	auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
	auto remoteFilePtr = connection->GetRemoteFile(createdFileName);
	auto status = connection->BeginTransaction();
	ASSERT_FALSE(status->HasError());
	ASSERT_FALSE(remoteFilePtr->Open());
	status = connection->CommitTransaction();
	ASSERT_FALSE(status->HasError());
}


/// Файл создается, если транзакция была зафиксирована
TEST(Connection, CanCreateRemoteFileWithCommitTransaction)
{
	std::string createdFileName;
	{
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto status = connection->BeginTransaction();
		ASSERT_FALSE(status->HasError());
		auto remoteFile = connection->CreateRemoteFile();
		createdFileName = remoteFile->GetFileName();

		status = connection->CommitTransaction();
		ASSERT_FALSE(status->HasError());
	}

	auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
	auto remoteFilePtr = connection->GetRemoteFile(createdFileName);
	auto status = connection->BeginTransaction();
	ASSERT_FALSE(status->HasError());
	ASSERT_TRUE(remoteFilePtr->Open());
	status = connection->CommitTransaction();
	ASSERT_FALSE(status->HasError());

	// Удалим файл в конце
	ASSERT_TRUE(connection->DeleteRemoteFile(createdFileName));
}


/// Файл не удаляется, если транзакция оборвалась
TEST(Connection, CantDeleteRemoteFileWithFailedTransaction)
{
	std::string createdFileName;
	{
		// Создадим файл для удаления
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto remoteFilePtr = connection->CreateRemoteFile();
		createdFileName = remoteFilePtr->GetFileName();
	}

	{
		// Попытаемся его удалить в рамках транзакции
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto status = connection->BeginTransaction();
		ASSERT_FALSE(status->HasError());

		connection->DeleteRemoteFile(createdFileName);
		// Обрыв транзакции
	}

	{
		// Проверим, что файл не удалился
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto remoteFilePtr = connection->GetRemoteFile(createdFileName);
		ASSERT_EQ(remoteFilePtr->GetFileName(), createdFileName);

		auto status = connection->BeginTransaction();
		ASSERT_FALSE(status->HasError());
		
		ASSERT_TRUE(remoteFilePtr->Open());
		
		status = connection->CommitTransaction();
		ASSERT_FALSE(status->HasError());
	}

	{
		// Удалим файл
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		ASSERT_TRUE(connection->DeleteRemoteFile(createdFileName));
	}	
}


/// Файл не удаляется, если транзакция была отменена
TEST(Connection, CantDeleteRemoteFileWithRollbackTransaction)
{
	std::string createdFileName;
	{
		// Создадим файл для удаления
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto remoteFilePtr = connection->CreateRemoteFile();
		createdFileName = remoteFilePtr->GetFileName();
	}

	{
		// Попытаемся его удалить в рамках транзакции
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto status = connection->BeginTransaction();
		ASSERT_FALSE(status->HasError());

		connection->DeleteRemoteFile(createdFileName);
		
		// Отмена транзакции
		status = connection->RollbackTransaction();
		ASSERT_FALSE(status->HasError());
	}

	{
		// Проверим, что файл не удалился
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto remoteFilePtr = connection->GetRemoteFile(createdFileName);
		ASSERT_EQ(remoteFilePtr->GetFileName(), createdFileName);

		auto status = connection->BeginTransaction();
		ASSERT_FALSE(status->HasError());

		ASSERT_TRUE(remoteFilePtr->Open());

		status = connection->CommitTransaction();
		ASSERT_FALSE(status->HasError());
	}

	{
		// Удалим файл
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		ASSERT_TRUE(connection->DeleteRemoteFile(createdFileName));
	}
}


/// Файл удаляется, если транзакция была зафиксирована
TEST(Connection, CanDeleteRemoteFileWithCommitTransaction)
{
	std::string createdFileName;
	{
		// Создадим файл для удаления
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto remoteFilePtr = connection->CreateRemoteFile();
		createdFileName = remoteFilePtr->GetFileName();
	}

	{
		// Попытаемся его удалить в рамках транзакции
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto status = connection->BeginTransaction();
		ASSERT_FALSE(status->HasError());

		connection->DeleteRemoteFile(createdFileName);

		// Фиксация транзакции
		status = connection->CommitTransaction();
		ASSERT_FALSE(status->HasError());
	}

	{
		// Проверим, что файл удалился
		auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		auto remoteFilePtr = connection->GetRemoteFile(createdFileName);
		ASSERT_EQ(remoteFilePtr->GetFileName(), createdFileName);

		auto status = connection->BeginTransaction();
		ASSERT_FALSE(status->HasError());

		ASSERT_FALSE(remoteFilePtr->Open());

		status = connection->CommitTransaction();
		ASSERT_FALSE(status->HasError());
	}
}
