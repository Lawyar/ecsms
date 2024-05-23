////////////////////////////////////////////////////////////////////////////////
//
/**
  Тесты для ExecutorEAVNamingRules
*/
//
////////////////////////////////////////////////////////////////////////////////

#include "TestSettings.h"

#include <gtest/gtest.h>

#include <IDatabaseManager.h>
#include <Utils/StringUtils.h>

#include <IExecutorEAVNamingRules.h>

// Тест для проверок
class ExecutorEAVNamingRules : public ::testing::Test {
protected:
  IConnectionPtr connection;
  IExecutorEAVPtr executorEAV;

protected:
  // Действия в начале теста
  virtual void SetUp() override {
    connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
    executorEAV = GetDatabaseManager().GetExecutorEAV(connection);
  }

  // Получить объект для проверок
  const IExecutorEAVNamingRules &GetRules() const {
    return executorEAV->GetNamingRules();
  }
};

/// Проверка методов IExecutorEAVNamingRules
TEST_F(ExecutorEAVNamingRules, Methods) {
  const std::string entityName = "EntityName";
  const std::string attributeType = "INTEGER";

  EXPECT_EQ(GetRules().GetEntityTable_Short_IdField(entityName), "id");
  EXPECT_EQ(
      GetRules().GetAttributeTable_Short_IdField(entityName, attributeType),
      "id");
  EXPECT_EQ(
      GetRules().GetAttributeTable_Short_NameField(entityName, attributeType),
      "name");
  EXPECT_EQ(
      GetRules().GetValueTable_Short_EntityIdField(entityName, attributeType),
      "entity_id");
  EXPECT_EQ(GetRules().GetValueTable_Short_AttributeIdField(entityName,
                                                            attributeType),
            "attribute_id");
  EXPECT_EQ(
      GetRules().GetValueTable_Short_ValueField(entityName, attributeType),
      "value");
  EXPECT_EQ(GetRules().GetEntityTableName(entityName), "entityname");
  EXPECT_EQ(GetRules().GetAttributeTableName(entityName, attributeType),
            "entityname_attribute_integer");
  EXPECT_EQ(GetRules().GetValueTableName(entityName, attributeType),
            "entityname_value_integer");
}
