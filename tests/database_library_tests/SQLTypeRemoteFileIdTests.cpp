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


/// Тесты ISQLTypeRemoteFileId::GetValue и ISQLTypeRemoteFileId::SetValue
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


/// Тесты ISQLTypeRemoteFileId::GetType
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


/// Тесты ISQLTypeRemoteFileId::GetTypeName
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


/// Тесты ISQLTypeRemoteFileId::ToSQLString
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


/// Тесты ISQLTypeRemoteFileId::ReadFromSQL
TEST(SQLTypeRemoteFileId, ReadFromSQL) {
	auto && converter = GetDatabaseManager().GetSQLTypeConverter();

	const std::vector<std::tuple<
		std::string, // Строка для чтения
		bool, // Ожидаемый результат ReadFromSQL
		std::optional<std::string> // Ожидаемое значение после чтения
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
				// Если результат успешный, то строка должна была переместиться. То есть стать пустой.
				ASSERT_TRUE(sqlStringCopy.empty());
			else
				// Если результат неуспешный, то строка не должна была перемещаться
				ASSERT_EQ(sqlString, sqlStringCopy);
		}
	}
}

