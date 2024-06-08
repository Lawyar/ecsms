////////////////////////////////////////////////////////////////////////////////
//
/**
  Тесты для ISQLType
*/
//
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include <IDatabaseManager.h>

static const std::string testString =
    "`1234567890-=qwertyuiop[]\\asdfghjkl;'zxcvbnm,./ "
    "~!@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:\"ZXCVBNM<>?\t\r\n";
static const std::string testStringToSQLString =
    "'`1234567890-=qwertyuiop[]\\asdfghjkl;''zxcvbnm,./ "
    "~!@#$%^&*()_+QWERTYUIOP{}|ASDFGHJKL:\"ZXCVBNM<>?\t\r\n'";

/// Тесты ISQLTypeText::GetValue и ISQLTypeText::SetValue
TEST(SQLTypeText, GetSetValue) {

  const std::string invalidString1 = std::string("abc\0qwe", 7);
  const std::string invalidString2 = std::string("\0", 1);
  const std::string invalidString3 = std::string("simple string\0", 14);

  auto &&converter = GetDatabaseManager().GetSQLTypeConverter();
  std::vector<std::tuple<ISQLTypeTextPtr, // SQL-переменная
                         std::optional<std::string>, // Её ожидаемое значение
                         std::string, // Аргумент для SetValue
                         std::optional<std::string> // Ожидаемое значение после
                                                    // SetValue
                         >>
      testData = {
          // Конструктор с валидными данными и SetValue с валидными данными
          {converter->GetSQLTypeText(), std::nullopt, "abc", "abc"},
          {converter->GetSQLTypeText(""), "", testString, testString},
          {converter->GetSQLTypeText("simple string"), "simple string",
           "1 simple string", "1 simple string"},
          {converter->GetSQLTypeText(std::string(testString)), testString, "",
           ""},

          // Конструктор с валидными данными и SetValue с невалидными данными
          {converter->GetSQLTypeText(), std::nullopt, invalidString1,
           std::nullopt},
          {converter->GetSQLTypeText(""), "", invalidString2, std::nullopt},
          {converter->GetSQLTypeText("simple string"), "simple string",
           invalidString3, std::nullopt},
          {converter->GetSQLTypeText(std::string(testString)), testString,
           invalidString1, std::nullopt},

          // Конструктор с невалидными данными и SetValue с валидными и
          // невалидными данными
          {converter->GetSQLTypeText(std::string(invalidString1)), std::nullopt,
           invalidString1, std::nullopt},
          {converter->GetSQLTypeText(std::string(invalidString2)), std::nullopt,
           testString, testString},
          {converter->GetSQLTypeText(std::string(invalidString3)), std::nullopt,
           invalidString2, std::nullopt}};

  for (auto &&[sqlVar, expectedValueAfterCreating, setValueArg,
               expectedValueAfterSetValue] : testData) {
    ASSERT_EQ(sqlVar->GetValue(), expectedValueAfterCreating);
    std::string setValueArgCopy = setValueArg;
    sqlVar->SetValue(std::move(setValueArgCopy));
    ASSERT_EQ(sqlVar->GetValue(), expectedValueAfterSetValue);
    // Проверка, что при успехе SetValue значение передаваемого аргумента
    // очистилось, а при неуспехе - не изменилось
    if (sqlVar->GetValue().has_value())
      ASSERT_TRUE(setValueArgCopy.empty());
    else
      ASSERT_EQ(setValueArg, setValueArgCopy);
  }
}

/// Тесты ISQLTypeText::GetType
TEST(SQLTypeText, GetType) {
  auto &&converter = GetDatabaseManager().GetSQLTypeConverter();
  for (auto &&sqlVar :
       {converter->GetSQLTypeText(), converter->GetSQLTypeText(""),
        converter->GetSQLTypeText("some string"),
        converter->GetSQLTypeText("hello 'world'"),
        converter->GetSQLTypeText(std::string(testString))}) {
    ASSERT_EQ(sqlVar->GetType(), SQLDataType::Text);
  }
}

/// Тесты ISQLTypeText::GetTypeName
TEST(SQLTypeText, GetTypeName) {
  auto &&converter = GetDatabaseManager().GetSQLTypeConverter();
  for (auto &&sqlVar :
       {converter->GetSQLTypeText(), converter->GetSQLTypeText(""),
        converter->GetSQLTypeText("some string"),
        converter->GetSQLTypeText("hello 'world'"),
        converter->GetSQLTypeText(std::string(testString))}) {
    ASSERT_EQ(sqlVar->GetTypeName(), "TEXT");
  }
}

/// Тесты ISQLTypeText::ToSQLString
TEST(SQLTypeText, ToSQLString) {
  auto &&converter = GetDatabaseManager().GetSQLTypeConverter();
  std::vector<std::tuple<ISQLTypeTextPtr, // SQL переменная
                         std::optional<std::string> // Ожидаемое значение вызова
                                                    // ToSQLString
                         >>
      testData = {
          {converter->GetSQLTypeText(), std::nullopt},
          {converter->GetSQLTypeText(std::string("invalid string\0", 15)),
           std::nullopt},
          {converter->GetSQLTypeText("valid string"), "'valid string'"},
          {converter->GetSQLTypeText(""), "''"},
          {converter->GetSQLTypeText("'string'"), "'''string'''"},
          {converter->GetSQLTypeText("''"), "''''''"},
          {converter->GetSQLTypeText("'"), "''''"},
          {converter->GetSQLTypeText(std::string(testString)),
           testStringToSQLString},
      };

  for (auto &&[sqlVar, expectedValue] : testData) {
    auto actualValue = sqlVar->ToSQLString();
    ASSERT_EQ(actualValue, expectedValue);
  }
}

/// Тесты ISQLTypeText::ReadFromSQL
TEST(SQLTypeText, ReadFromSQL) {
  auto &&converter = GetDatabaseManager().GetSQLTypeConverter();

  const std::string invalidString1 = std::string("abc\0qwe", 7);
  const std::string invalidString2 = std::string("\0", 1);
  const std::string invalidString3 = std::string("simple string\0", 14);

  std::vector<std::tuple<ISQLTypeTextPtr, // SQL-переменная
                         std::string,     // Аргумент для ReadFromSQL
                         bool, // Ожидаемое значение ReadFromSQL
                         std::optional<std::string> // Ожидаемое значение после
                                                    // ReadFromSQL
                         >>
      testData = {
          // Конструктор с валидными данными и ReadFromSQL с валидными данными
          {converter->GetSQLTypeText(), "'abc'", true, "'abc'"},
          {converter->GetSQLTypeText(""), testString, true, testString},
          {converter->GetSQLTypeText("simple string"), "1 simple string", true,
           "1 simple string"},
          {converter->GetSQLTypeText(std::string(testString)), "", true, ""},

          // Конструктор с валидными данными и ReadFromSQL с невалидными данными
          {converter->GetSQLTypeText(), invalidString1, false, std::nullopt},
          {converter->GetSQLTypeText(""), invalidString2, false, std::nullopt},
          {converter->GetSQLTypeText("simple string"), invalidString3, false,
           std::nullopt},
          {converter->GetSQLTypeText(std::string(testString)), invalidString1,
           false, std::nullopt},

          // Конструктор с невалидными данными и ReadFromSQL с валидными и
          // невалидными данными
          {converter->GetSQLTypeText(std::string(invalidString1)),
           invalidString1, false, std::nullopt},
          {converter->GetSQLTypeText(std::string(invalidString2)), testString,
           true, testString},
          {converter->GetSQLTypeText(std::string(invalidString3)),
           invalidString2, false, std::nullopt}};

  for (auto &&[sqlVar, readFromSQLArg, expectedReadFromSQLResult,
               expectedValueAfterReadFromSQL] : testData) {
    auto readFromSQLArgCopy = std::string(readFromSQLArg);
    bool result = sqlVar->ReadFromSQL(std::move(readFromSQLArgCopy));
    ASSERT_EQ(result, expectedReadFromSQLResult);
    ASSERT_EQ(sqlVar->GetValue(), expectedValueAfterReadFromSQL);
    if (result)
      // Если результат успешный, то строка должна была переместиться. То есть
      // стать пустой.
      ASSERT_TRUE(readFromSQLArgCopy.empty());
    else
      // Если результат неуспешный, то строка не должна была перемещаться.
      ASSERT_EQ(readFromSQLArg, readFromSQLArgCopy);
  }
}
