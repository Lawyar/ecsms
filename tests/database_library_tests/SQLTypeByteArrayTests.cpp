////////////////////////////////////////////////////////////////////////////////
//
/**
  ����� ��� ISQLType
*/
//
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include <string>
#include <limits>

#include <IDatabaseManager.h>

/// ����� ISQLTypeByteArray::GetValue � ISQLTypeByteArray::SetValue
TEST(SQLTypeByteArray, GetSetValue) {
	auto && converter = GetDatabaseManager().GetSQLTypeConverter();
	std::vector<std::tuple<
		ISQLTypeByteArrayPtr, // SQL-����������
		std::optional<std::vector<char>>, // Ÿ ��������� ��������
		std::vector<char>, // �������� ��� SetValue
		std::optional<std::vector<char>> // ��������� �������� ����� SetValue
	>> testData = {
			// ����������� � ��������� ������� � SetValue � ��������� �������
			{converter->GetSQLTypeByteArray(),						std::nullopt,						std::vector<char>{1, 2, 3},		std::vector<char>{1, 2, 3}},
			{converter->GetSQLTypeByteArray(),						std::nullopt,						std::vector<char>{},			std::vector<char>{}},
			{converter->GetSQLTypeByteArray({}),					std::vector<char>{},				std::vector<char>{},			std::vector<char>{}},
			{converter->GetSQLTypeByteArray({1, 2, 3}),				std::vector<char>{1, 2, 3},			std::vector<char>{},			std::vector<char>{}},
			{converter->GetSQLTypeByteArray({1, 2, 3}),				std::vector<char>{1, 2, 3},			std::vector<char>{1, 2, 3},		std::vector<char>{1, 2, 3}},
			{converter->GetSQLTypeByteArray({1, 2, 3}),				std::vector<char>{1, 2, 3},			std::vector<char>{4, 5, 6},		std::vector<char>{4, 5, 6}},
			{converter->GetSQLTypeByteArray({1, 2, 3, 4, 0}),		std::vector<char>{1, 2, 3, 4, 0},	std::vector<char>{1, 2, 3},		std::vector<char>{1, 2, 3}},
	};

	for (auto &&[sqlVar, expectedValueAfterCreating, setValueArg, expectedValueAfterSetValue] : testData)
	{
		ASSERT_EQ(sqlVar->GetValue(), expectedValueAfterCreating);
		std::vector<char> setValueArgCopy = setValueArg;
		sqlVar->SetValue(std::move(setValueArgCopy));
		ASSERT_EQ(sqlVar->GetValue(), expectedValueAfterSetValue);
		// ��������, ��� ��� ������ SetValue �������� ������������� ��������� ����������,
		// � ��� �������� - �� ����������
		if (sqlVar->GetValue().has_value())
			ASSERT_TRUE(setValueArgCopy.empty());
		else
			ASSERT_EQ(setValueArg, setValueArgCopy);
	}
}


/// ����� ISQLTypeByteArray::GetType
TEST(SQLTypeByteArray, GetType) {
	auto && converter = GetDatabaseManager().GetSQLTypeConverter();
	for (auto && sqlVar : { converter->GetSQLTypeByteArray(), converter->GetSQLTypeByteArray({}),
		converter->GetSQLTypeByteArray({0}), converter->GetSQLTypeByteArray({1, 2, 3})})
	{
		ASSERT_EQ(sqlVar->GetType(), SQLDataType::ByteArray);
	}
}


/// ����� ISQLTypeByteArray::GetTypeName
TEST(SQLTypeByteArray, GetTypeName) {
	auto && converter = GetDatabaseManager().GetSQLTypeConverter();
	for (auto && sqlVar : { converter->GetSQLTypeByteArray(), converter->GetSQLTypeByteArray({}),
		converter->GetSQLTypeByteArray({0}), converter->GetSQLTypeByteArray({1, 2, 3}) })
	{
		ASSERT_EQ(sqlVar->GetTypeName(), "BYTEA");
	}
}


/// ������� �������� ������� � ������
static std::string CharToHex(char c)
{
	char buffer[128] = "";
	std::snprintf(buffer, std::size(buffer), "%x", c);
	std::string result(buffer);
	if (result.size() > 2)
	{
		auto firstIter = result.end();
		std::advance(firstIter, -2);
		return std::string(firstIter, result.end());
	}
	else if (result.size() == 2)
	{
		return result;
	}
	else if (result.size() == 1)
	{
		return "0" + result;
	}
	else
	{
		assert(false);
		return {};
	}
}


/// ����� ISQLTypeByteArray::ToSQLString
TEST(SQLTypeByteArray, ToSQLString) {
	auto && converter = GetDatabaseManager().GetSQLTypeConverter();
	std::vector<std::tuple<
		ISQLTypeByteArrayPtr, // SQL-����������
		std::optional<std::string> // ��������� �������� ������ ToSQLString
		>> testData = {
		{converter->GetSQLTypeByteArray(),				std::nullopt},
		{converter->GetSQLTypeByteArray({}),			"'\\x'::BYTEA"},
		{converter->GetSQLTypeByteArray({0, 1, 2}),		"'\\x000102'::BYTEA"},
		{converter->GetSQLTypeByteArray({0, -1, -2}),	"'\\x00fffe'::BYTEA"}
	};
	for (int i = std::numeric_limits<char>::min(); i <= std::numeric_limits<char>::max(); ++i)
	{
		char currentChar = static_cast<char>(i);
		testData.push_back({ converter->GetSQLTypeByteArray({currentChar}), "'\\x" + CharToHex(currentChar) + "'::BYTEA" });
	}

	for (auto &&[sqlVar, expectedValue] : testData)
	{
		auto actualValue = sqlVar->ToSQLString();
		ASSERT_EQ(actualValue, expectedValue);
	}
}


/// ����� ISQLTypeByteArray::ReadFromSQL
TEST(SQLTypeByteArray, ReadFromSQL) {
	auto && converter = GetDatabaseManager().GetSQLTypeConverter();

	std::vector<std::tuple<
		ISQLTypeByteArrayPtr, // SQL-����������
		std::string, // �������� ��� ReadFromSQL
		bool, // ��������� �������� ReadFromSQL
		std::optional<std::vector<char>> // ��������� �������� ����� ReadFromSQL
		>> testData = {
		// ����������� � ��������� ������� � ReadFromSQL � ��������� �������
		{converter->GetSQLTypeByteArray(),				"\\xAB",					true,	std::vector<char>{-85}},
		{converter->GetSQLTypeByteArray({}),			"\\xab",					true,	std::vector<char>{-85}},
		{converter->GetSQLTypeByteArray({1, 2, 3}),		"\\x0123456789ABCDEF00",	true,	std::vector<char>{1, 35, 69, 103, -119, -85, -51, -17, 0}},
		{converter->GetSQLTypeByteArray({0, -1, -2}),	"\\x0123456789abcdef",		true,	std::vector<char>{1, 35, 69, 103, -119, -85, -51, -17}},

		// ����������� � ��������� ������� � ReadFromSQL � ����������� �������
		{converter->GetSQLTypeByteArray(),				"10",				false,	std::nullopt},
		{converter->GetSQLTypeByteArray({}),			"\\x10\\x",			false,	std::nullopt},
		{converter->GetSQLTypeByteArray({1, 2, 3}),		"\\x10 ",			false,	std::nullopt},
		{converter->GetSQLTypeByteArray({1, 2, 3}),		"",					false,	std::nullopt},
		{converter->GetSQLTypeByteArray({0, -1, -2}),	" \\x10 ",			false,	std::nullopt}
	};

	for (auto &&[sqlVar, readFromSQLArg, expectedReadFromSQLResult, expectedValueAfterReadFromSQL] : testData)
	{
		auto readFromSQLArgCopy = std::string(readFromSQLArg);
		bool result = sqlVar->ReadFromSQL(std::move(readFromSQLArgCopy));
		ASSERT_EQ(result, expectedReadFromSQLResult);
		ASSERT_EQ(sqlVar->GetValue(), expectedValueAfterReadFromSQL);
		if (result)
			// ���� ��������� ��������, �� ������ ������ ���� �������������. �� ���� ����� ������.
			ASSERT_TRUE(readFromSQLArgCopy.empty());
		else
			// ���� ��������� ����������, �� ������ �� ������ ���� ������������.
			ASSERT_EQ(readFromSQLArg, readFromSQLArgCopy);
	}
}

