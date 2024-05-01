#include <gtest/gtest.h>

#include <src/PGDatabaseManager.h>

TEST(database_library_tests, test_case_1) {
	IDatabaseManagerPtr databaseManager = std::make_shared<PGDatabaseManager>();
	IConnectionPtr connection = databaseManager->GetConnection("postgresql://postgres:password@localhost:5432/testdb");
	ASSERT_TRUE(connection->IsValid());
}
