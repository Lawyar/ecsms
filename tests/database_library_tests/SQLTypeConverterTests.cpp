////////////////////////////////////////////////////////////////////////////////
//
/**
  Тесты для ISQLTypeConverter
*/
//
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include <IDatabaseManager.h>

/// Можем получить конвертер
TEST(SQLTypeConverter, CanGetConverter) {
	auto && databaseManager = GetDatabaseManager();
	ASSERT_NE(databaseManager.GetSQLTypeConverter(), nullptr);
}


/// Можем получить SQL-переменную
TEST(SQLTypeConverter, CanGetSQLVariable) {
	auto && converter = GetDatabaseManager().GetSQLTypeConverter();
	for (int i = 0; i <= static_cast<int>(SQLDataType::LastValidType); ++i)
	{
		auto type = static_cast<SQLDataType>(i);
		auto sqlVar = converter->GetSQLVariable(type);
		ASSERT_NE(sqlVar, nullptr);
		ASSERT_EQ(sqlVar->GetType(), sqlVar->GetType());
	}

	ASSERT_EQ(converter->GetSQLVariable(SQLDataType::Unknown), nullptr);
	ASSERT_EQ(converter->GetSQLVariable(SQLDataType::Invalid), nullptr);

	ASSERT_TRUE(std::dynamic_pointer_cast<ISQLTypeInteger>(converter->GetSQLVariable(SQLDataType::Integer)));
	ASSERT_TRUE(std::dynamic_pointer_cast<ISQLTypeText>(converter->GetSQLVariable(SQLDataType::Text)));
	ASSERT_TRUE(std::dynamic_pointer_cast<ISQLTypeByteArray>(converter->GetSQLVariable(SQLDataType::ByteArray)));
}


/// Можем получить SQL-Integer
TEST(SQLTypeConverter, CanGetSQLTypeInteger) {
	auto && converter = GetDatabaseManager().GetSQLTypeConverter();
	auto sqlVar1 = converter->GetSQLTypeInteger();
	auto sqlVar2 = converter->GetSQLTypeInteger(0);
	auto sqlVar3 = converter->GetSQLTypeInteger(5);
	auto sqlVar4 = converter->GetSQLTypeInteger(-1);

	
	auto sqlVars = { sqlVar1, sqlVar2, sqlVar3, sqlVar4 };
	for (auto && sqlVar : sqlVars)
	{
		ASSERT_NE(sqlVar, nullptr);
		ASSERT_EQ(sqlVar->GetType(), SQLDataType::Integer);
		ASSERT_EQ(sqlVar->GetTypeName(), "INTEGER");
	}
}


/// Можем получить SQL-Text
TEST(SQLTypeConverter, CanGetSQLTypeText) {
	auto && converter = GetDatabaseManager().GetSQLTypeConverter();
	auto sqlVar1 = converter->GetSQLTypeText();
	auto sqlVar2 = converter->GetSQLTypeText("hello");
	auto sqlVar3 = converter->GetSQLTypeText(std::string("hello1"));
	auto sqlVar4 = converter->GetSQLTypeText("");

	auto sqlVars = { sqlVar1, sqlVar2, sqlVar3, sqlVar4 };
	for (auto && sqlVar : sqlVars)
	{
		ASSERT_NE(sqlVar, nullptr);
		ASSERT_EQ(sqlVar->GetType(), SQLDataType::Text);
		ASSERT_EQ(sqlVar->GetTypeName(), "TEXT");
	}
}


/// Можем получить SQL-ByteArray
TEST(SQLTypeConverter, CanGetSQLTypeByteArray) {
	auto && converter = GetDatabaseManager().GetSQLTypeConverter();
	auto sqlVar1 = converter->GetSQLTypeByteArray();
	auto sqlVar2 = converter->GetSQLTypeByteArray(std::vector<char>({1, 2, 3, 4, 5}));
	auto sqlVar3 = converter->GetSQLTypeByteArray(std::vector<char>{});

	auto sqlVars = { sqlVar1, sqlVar2, sqlVar3 };
	for (auto && sqlVar : sqlVars)
	{
		ASSERT_NE(sqlVar, nullptr);
		ASSERT_EQ(sqlVar->GetType(), SQLDataType::ByteArray);
		ASSERT_EQ(sqlVar->GetTypeName(), "BYTEA");
	}
}
