////////////////////////////////////////////////////////////////////////////////
//
/**
  ����� ��� ISQLTypeFile
*/
//
////////////////////////////////////////////////////////////////////////////////


#include "TestSettings.h"

#include <gtest/gtest.h>

#include <Utils/StringUtils.h>
#include <IDatabaseManager.h>


/// ����� ISQLTypeRemoteFileId::GetValue � ISQLTypeRemoteFileId::SetValue
TEST(SQLTypeRemoteFileId, GetValue) {
	auto && converter = GetDatabaseManager().GetSQLTypeConverter();
	auto && var1 = converter->GetSQLTypeRemoteFileId("");
	auto && var2 = converter->GetSQLTypeRemoteFileId("0");
	auto && var3 = converter->GetSQLTypeRemoteFileId("12345");
	auto && var4 = converter->GetSQLTypeRemoteFileId("hello");
	auto && var5 = converter->GetSQLTypeRemoteFileId("12345 hello");

	ASSERT_EQ(var1->GetId(), std::nullopt);
	ASSERT_EQ(var2->GetId(), "0");
	ASSERT_EQ(var3->GetId(), "12345");
	ASSERT_EQ(var4->GetId(), std::nullopt);
	ASSERT_EQ(var5->GetId(), std::nullopt);
}


/// ����� ISQLTypeRemoteFileId::GetType
TEST(SQLTypeRemoteFileId, GetType) {
	auto && converter = GetDatabaseManager().GetSQLTypeConverter();
	auto && var1 = converter->GetSQLTypeRemoteFileId("");
	auto && var2 = converter->GetSQLTypeRemoteFileId("0");
	auto && var3 = converter->GetSQLTypeRemoteFileId("12345");
	auto && var4 = converter->GetSQLTypeRemoteFileId("hello");
	auto && var5 = converter->GetSQLTypeRemoteFileId("12345 hello");

	for (auto && var : { var1, var2, var3, var4, var5 })
		ASSERT_EQ(var->GetType(), SQLDataType::RemoteFileId);
}


/// ����� ISQLTypeRemoteFileId::GetTypeName
TEST(SQLTypeRemoteFileId, GetTypeName) {
	auto && converter = GetDatabaseManager().GetSQLTypeConverter();
	auto && var1 = converter->GetSQLTypeRemoteFileId("");
	auto && var2 = converter->GetSQLTypeRemoteFileId("0");
	auto && var3 = converter->GetSQLTypeRemoteFileId("12345");
	auto && var4 = converter->GetSQLTypeRemoteFileId("hello");
	auto && var5 = converter->GetSQLTypeRemoteFileId("12345 hello");

	for (auto && var : { var1, var2, var3, var4, var5 })
		ASSERT_EQ(var->GetTypeName(), "OID");
}


/// ����� ISQLTypeRemoteFileId::ToSQLString
TEST(SQLTypeRemoteFileId, ToSQLString) {
	auto && converter = GetDatabaseManager().GetSQLTypeConverter();
	auto && var1 = converter->GetSQLTypeRemoteFileId("");
	auto && var2 = converter->GetSQLTypeRemoteFileId("0");
	auto && var3 = converter->GetSQLTypeRemoteFileId("12345");
	auto && var4 = converter->GetSQLTypeRemoteFileId("hello");
	auto && var5 = converter->GetSQLTypeRemoteFileId("12345 hello");

	ASSERT_EQ(var1->ToSQLString(), std::nullopt);
	ASSERT_EQ(var2->ToSQLString(), "0");
	ASSERT_EQ(var3->ToSQLString(), "12345");
	ASSERT_EQ(var4->ToSQLString(), std::nullopt);
	ASSERT_EQ(var4->ToSQLString(), std::nullopt);
}


/// ����� ISQLTypeRemoteFileId::ReadFromSQL
TEST(SQLTypeRemoteFileId, ReadFromSQL) {
	auto && converter = GetDatabaseManager().GetSQLTypeConverter();

	const std::vector<std::tuple<
		std::string, // ������ ��� ������
		bool, // ��������� ��������� ReadFromSQL
		std::optional<std::string> // ��������� �������� ����� ������
		>>
		testData{
	{"", false, std::nullopt},
	{"0", true, "0"},
	{"12345", true, "12345"},
	{"hello", false, std::nullopt},
	{"12345 hello", false, std::nullopt}
	};

	for (auto &&[sqlString, expectedReadResult, expectedGetValueResult] : testData)
	{
		auto sqlVars = { converter->GetSQLTypeRemoteFileId(),
			converter->GetSQLTypeRemoteFileId(""), converter->GetSQLTypeRemoteFileId("0"),
			converter->GetSQLTypeRemoteFileId("12345"), converter->GetSQLTypeRemoteFileId("hello"),
			converter->GetSQLTypeRemoteFileId("12345 hello")
		};
		for (auto && sqlVar : sqlVars)
		{
			auto sqlStringCopy = std::string(sqlString);
			bool result = sqlVar->ReadFromSQL(std::move(sqlStringCopy));
			ASSERT_EQ(result, expectedReadResult);
			ASSERT_EQ(sqlVar->GetId(), expectedGetValueResult);
			if (result)
				// ���� ��������� ��������, �� ������ ������ ���� �������������. �� ���� ����� ������.
				ASSERT_TRUE(sqlStringCopy.empty());
			else
				// ���� ��������� ����������, �� ������ �� ������ ���� ������������
				ASSERT_EQ(sqlString, sqlStringCopy);
		}
	}
}

