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
	auto && remoteFile = connection->CreateRemoteFile();
	ASSERT_TRUE(remoteFile->Open({ FileOpenMode::Write }));

	std::vector<char> bytes;
	for (size_t i = 0; i < 1000; ++i)
		bytes.push_back(static_cast<char>(i));
	
	ASSERT_TRUE(remoteFile->WriteBytes(bytes));
	remoteFile->Close();

	auto objId = remoteFile->GetFileName();
	ASSERT_FALSE(objId.empty());

	auto result = connection->Execute(utils::string::Format("SELECT lo_unlink({});", objId));
	auto status = result->GetCurrentExecuteStatus();
	ASSERT_FALSE(status->HasError());	
}
