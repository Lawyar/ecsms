////////////////////////////////////////////////////////////////////////////////
//
/**
  Тесты для IDatabaseManager
*/
//
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include "TestSettings.h"

#include <IDatabaseManager.h>

/// Можем получить (валидное) соединение к базе данных, передав валидный URL
TEST(DatabaseManager, CanGetConnectionWithValidURL) {
	auto && databaseManager = GetDatabaseManager();
	IConnectionPtr connection = databaseManager.GetConnection(c_PostgreSQLConnectionURL);
	ASSERT_NE(connection, nullptr);
}


/// Можем получить (невалидное) соединение к базе данных, передав невалидный URL
TEST(DatabaseManager, CanGetConnectionWithInvalidURL) {
	auto && databaseManager = GetDatabaseManager();
	IConnectionPtr connection = databaseManager.GetConnection("");
	ASSERT_NE(connection, nullptr);
}


/// Можем получить исполнитель EAV-запросов, передав валидное соединение
TEST(DatabaseManager, CanGetExecutorEAVWithValidConnection) {
	auto && databaseManager = GetDatabaseManager();
	auto && connection = databaseManager.GetConnection(c_PostgreSQLConnectionURL);
	auto && executorEAV = databaseManager.GetExecutorEAV(std::move(connection));
	ASSERT_NE(executorEAV, nullptr);
}


/// Не можем получить исполнитель EAV-запросов, передав невалидное соединение
TEST(DatabaseManager, CantGetExecutorEAVWithInvalidConnection) {
	auto && databaseManager = GetDatabaseManager();
	auto && connection = databaseManager.GetConnection("");
	auto && executorEAV1 = databaseManager.GetExecutorEAV(std::move(connection));
	auto && executorEAV2 = databaseManager.GetExecutorEAV(nullptr);
	ASSERT_EQ(executorEAV1, nullptr);
	ASSERT_EQ(executorEAV2, nullptr);
}


/// Можем получить конвертер SQL-типов
TEST(DatabaseManager, CanGetSQLTypeConverter) {
	auto && databaseManager = GetDatabaseManager();
	auto && sqlTypeConverter = databaseManager.GetSQLTypeConverter();
	ASSERT_NE(sqlTypeConverter, nullptr);
}
