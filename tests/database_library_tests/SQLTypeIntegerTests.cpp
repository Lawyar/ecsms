////////////////////////////////////////////////////////////////////////////////
//
/**
  Тесты для ISQLType
*/
//
////////////////////////////////////////////////////////////////////////////////

#include <gtest/gtest.h>

#include <IDatabaseManager.h>

/// Тесты ISQLTypeInteger::GetValue и ISQLTypeInteger::SetValue
TEST(SQLTypeInteger, GetSetValue) {
  auto &&converter = GetDatabaseManager().GetSQLTypeConverter();
  auto &&int1 = converter->GetSQLTypeInteger();
  auto &&int2 = converter->GetSQLTypeInteger(5);
  auto &&int3 = converter->GetSQLTypeInteger(-47);

  ASSERT_EQ(int1->GetValue(), std::nullopt);
  ASSERT_EQ(*int2->GetValue(), 5);
  ASSERT_EQ(*int3->GetValue(), -47);

  int1->SetValue(-7);
  int2->SetValue(0);
  int3->SetValue(12);
  ASSERT_EQ(*int1->GetValue(), -7);
  ASSERT_EQ(*int2->GetValue(), 0);
  ASSERT_EQ(*int3->GetValue(), 12);
}

/// Тесты ISQLTypeInteger::GetType
TEST(SQLTypeInteger, GetType) {
  auto &&converter = GetDatabaseManager().GetSQLTypeConverter();
  auto &&int1 = converter->GetSQLTypeInteger();
  auto &&int2 = converter->GetSQLTypeInteger(5);
  auto &&int3 = converter->GetSQLTypeInteger(-47);

  ASSERT_EQ(int1->GetType(), SQLDataType::Integer);
  ASSERT_EQ(int2->GetType(), SQLDataType::Integer);
  ASSERT_EQ(int3->GetType(), SQLDataType::Integer);
}

/// Тесты ISQLTypeInteger::GetTypeName
TEST(SQLTypeInteger, GetTypeName) {
  auto &&converter = GetDatabaseManager().GetSQLTypeConverter();
  auto &&int1 = converter->GetSQLTypeInteger();
  auto &&int2 = converter->GetSQLTypeInteger(5);
  auto &&int3 = converter->GetSQLTypeInteger(-47);

  ASSERT_EQ(int1->GetTypeName(), "INTEGER");
  ASSERT_EQ(int2->GetTypeName(), "INTEGER");
  ASSERT_EQ(int3->GetTypeName(), "INTEGER");
}

/// Тесты ISQLTypeInteger::ToSQLString
TEST(SQLTypeInteger, ToSQLString) {
  auto &&converter = GetDatabaseManager().GetSQLTypeConverter();
  auto &&int1 = converter->GetSQLTypeInteger();
  auto &&int2 = converter->GetSQLTypeInteger(5);
  auto &&int3 = converter->GetSQLTypeInteger(-47);

  ASSERT_EQ(int1->ToSQLString(), std::nullopt);
  ASSERT_EQ(int2->ToSQLString(), "5");
  ASSERT_EQ(int3->ToSQLString(), "-47");
}

/// Тесты ISQLTypeInteger::ReadFromSQL
TEST(SQLTypeInteger, ReadFromSQL) {
  auto &&converter = GetDatabaseManager().GetSQLTypeConverter();

  const std::vector<std::tuple<std::string, // Строка для чтения
                               bool, // Ожидаемый результат ReadFromSQL
                               std::optional<int> // Ожидаемое значение после
                                                  // чтения
                               >>
      testData{
          {"0", true, 0},
          {"10", true, 10},
          {"147654", true, 147654},
          {"-10", true, -10},
          {"-125987", true, -125987},
          {"0123", true, 123},
          {"000", true, 0},
          {"-0012", true, -12},
          {"+12", true, 12},
          {"", false, std::nullopt},
          {"a", false, std::nullopt},
          {"abc", false, std::nullopt},
          {" 10", false, std::nullopt},
          {"10 ", false, std::nullopt},
          {" 10 ", false, std::nullopt},
          {"a10", false, std::nullopt},
          {"10a", false, std::nullopt},
          {" -10", false, std::nullopt},
          {"- 10", false, std::nullopt},
          {"+-1", false, std::nullopt},
          {" +10", false, std::nullopt},
          {" -10", false, std::nullopt},
          {"+10 ", false, std::nullopt},
          {"1.0", false, std::nullopt},
          {"-1-", false, std::nullopt},
          {"\t1", false, std::nullopt},
          {"1\n", false, std::nullopt},

      };

  for (auto &&[sqlString, expectedReadResult, expectedGetValueResult] :
       testData) {
    auto sqlVars = {converter->GetSQLTypeInteger(),
                    converter->GetSQLTypeInteger(5),
                    converter->GetSQLTypeInteger(-47)};

    for (auto &&sqlVar : sqlVars) {
      auto sqlStringCopy = std::string(sqlString);
      bool result = sqlVar->ReadFromSQL(std::move(sqlStringCopy));
      ASSERT_EQ(result, expectedReadResult);
      ASSERT_EQ(sqlVar->GetValue(), expectedGetValueResult);
      if (result)
        // Если результат успешный, то строка должна была переместиться. То есть
        // стать пустой.
        ASSERT_TRUE(sqlStringCopy.empty());
      else
        // Если результат неуспешный, то строка не должна была перемещаться
        ASSERT_EQ(sqlString, sqlStringCopy);
    }
  }
}
