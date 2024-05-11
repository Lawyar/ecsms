////////////////////////////////////////////////////////////////////////////////
//
/**
  Тесты для ISQLTypeFile
*/
//
////////////////////////////////////////////////////////////////////////////////


#include "TestSettings.h"

#include <gtest/gtest.h>

#include <Utils/StringUtils.h>

#include <IDatabaseManager.h>

TEST(SQLTypeRemoteFileId, Test)
{
	auto && connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);

	// Откроем транзакцию перед началом работы с файлом
	auto status = connection->BeginTransaction();
	ASSERT_FALSE(status->HasError());

	auto && remoteFile = connection->CreateRemoteFile();
	auto objId = remoteFile->GetFileName();
	ASSERT_FALSE(objId.empty());
	
	ASSERT_TRUE(remoteFile->Open({ FileOpenMode::Write }));

	std::vector<char> bytes;
	for (size_t i = 0; i < 1000; ++i)
		bytes.push_back(static_cast<char>(i));
	
	ASSERT_TRUE(remoteFile->WriteBytes(bytes));
	remoteFile->Close();

	// Закроем транзакцию, чтобы применить изменения
	status = connection->CommitTransaction();

	auto result = connection->Execute(utils::string::Format("SELECT lo_unlink({});", objId));
	status = result->GetCurrentExecuteStatus();
	ASSERT_FALSE(status->HasError());
}
