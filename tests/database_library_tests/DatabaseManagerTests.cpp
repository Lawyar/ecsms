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

/// ����� �������� (��������) ���������� � ���� ������, ������� �������� URL
TEST(DatabaseManager, CanGetConnectionWithValidURL) {
	auto && databaseManager = GetDatabaseManager();
	IConnectionPtr connection = databaseManager.GetConnection(c_PostgreSQLConnectionURL);
	ASSERT_NE(connection, nullptr);
}


/// ����� �������� (����������) ���������� � ���� ������, ������� ���������� URL
TEST(DatabaseManager, CanGetConnectionWithInvalidURL) {
	auto && databaseManager = GetDatabaseManager();
	IConnectionPtr connection = databaseManager.GetConnection("");
	ASSERT_NE(connection, nullptr);
}


/// ����� �������� ����������� EAV-��������, ������� �������� ����������
TEST(DatabaseManager, CanGetExecutorEAVWithValidConnection) {
	auto && databaseManager = GetDatabaseManager();
	auto && connection = databaseManager.GetConnection(c_PostgreSQLConnectionURL);
	auto && executorEAV = databaseManager.GetExecutorEAV(std::move(connection));
	ASSERT_NE(executorEAV, nullptr);
}


/// �� ����� �������� ����������� EAV-��������, ������� ���������� ����������
TEST(DatabaseManager, CantGetExecutorEAVWithInvalidConnection) {
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
