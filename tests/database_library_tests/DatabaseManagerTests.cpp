////////////////////////////////////////////////////////////////////////////////
//
/**
  ����� ��� IDatabaseManager
*/
//
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include "TestSettings.h"

#include <IDatabaseManager.h>

/// ����� ������������ � ���� ������ �� ��������� URL
TEST(DatabaseManager, CanGetConnectionWithValidURL) {
	auto && databaseManager = GetDatabaseManager();
	IConnectionPtr connection = databaseManager.GetConnection(c_PostgreSQLConnectionURL);
	ASSERT_TRUE(connection->IsValid());
}


/// �� ����� ������������ � ���� ������ �� ����������� URL
TEST(DatabaseManager, CantGetConnectionWithValidURL) {
	auto && databaseManager = GetDatabaseManager();
	IConnectionPtr connection = databaseManager.GetConnection("");
	ASSERT_FALSE(connection->IsValid());
	ASSERT_EQ(connection->GetStatus(), ConnectionStatus::Bad);
}


/// ����� �������� ����������� EAV-��������, ������� �������� ����������
TEST(DatabaseManager, CanGetExecutorEAVWithValidConnection) {
	auto && databaseManager = GetDatabaseManager();
	auto && connection = databaseManager.GetConnection(c_PostgreSQLConnectionURL);
	auto && executorEAV = databaseManager.GetExecutorEAV(std::move(connection));
	ASSERT_NE(executorEAV, nullptr);
}


/// �� ����� �������� ����������� EAV-��������, ������� ���������� ����������
TEST(DatabaseManager, CantGetExecutorEAVWithValidConnection) {
	auto && databaseManager = GetDatabaseManager();
	auto && connection = databaseManager.GetConnection("");
	auto && executorEAV1 = databaseManager.GetExecutorEAV(std::move(connection));
	auto && executorEAV2 = databaseManager.GetExecutorEAV(nullptr);
	ASSERT_EQ(executorEAV1, nullptr);
	ASSERT_EQ(executorEAV2, nullptr);
}


/// ����� �������� ��������� SQL-�����
TEST(DatabaseManager, CanGetSQLTypeConverter) {
	auto && databaseManager = GetDatabaseManager();
	auto && sqlTypeConverter = databaseManager.GetSQLTypeConverter();
	ASSERT_NE(sqlTypeConverter, nullptr);
}
