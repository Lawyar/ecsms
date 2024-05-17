////////////////////////////////////////////////////////////////////////////////
//
/**
  Тесты для ExecutorEAV
*/
//
////////////////////////////////////////////////////////////////////////////////

#include "TestSettings.h"

#include <gtest/gtest.h>

#include <Utils/StringUtils.h>
#include <IDatabaseManager.h>
#include <IExecutorEAVNamingRules.h>

// Тест для проверок ExecutorEAV без создания таблиц
class ExecutorEAVWithEmptyEnvironment : public ::testing::Test
{
protected:
	IConnectionPtr connection;
	IExecutorEAVPtr executorEAV;
	ISQLTypeConverterPtr converter;

protected:
	// Действия в начале теста
	virtual void SetUp() override
	{
		connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
		ASSERT_NE(connection, nullptr);

		executorEAV = GetDatabaseManager().GetExecutorEAV(connection);
		ASSERT_NE(executorEAV, nullptr);

		converter = GetDatabaseManager().GetSQLTypeConverter();
		ASSERT_NE(converter, nullptr);
	}

	// Получить правила именования
	const IExecutorEAVNamingRules & GetRules() const
	{
		return executorEAV->GetNamingRules();
	}
};


////////////////////////////////////////////////////////////////////////////////
// Тесты SetRegisteredEntities
////////////////////////////////////////////////////////////////////////////////


/// SetRegisteredEntities создает таблицы для всех валидных типов данных
TEST_F(ExecutorEAVWithEmptyEnvironment, RegisterEntitiesCreatesTablesWithAllValidTypes)
{
	std::vector<SQLDataType> allTypes;
	for (int i = 0; i <= static_cast<int>(SQLDataType::LastValidType); ++i)
	{
		SQLDataType currentType = static_cast<SQLDataType>(i);
		allTypes.push_back(currentType);
	}

	std::vector<IExecutorEAV::EAVRegisterEntries> maps{{
		{"SomeEntity1", allTypes},
		{"SomeEntity2", {SQLDataType::Integer}},
		{"SomeEntity3", {SQLDataType::Integer, SQLDataType::ByteArray}},
	}};

	// Проверим, что таблицы сейчас отсутствуют
	for (auto && map : maps)
	{
		for (auto &&[entityName, attributeTypes] : map)
		{
			for (auto && type : attributeTypes)
			{
				auto && attributeType = converter->GetSQLVariable(type)->GetTypeName();
				auto result = connection->Execute(utils::string::Format("SELECT * FROM {};",
					GetRules().GetEntityTableName(entityName)));
				ASSERT_TRUE(result->GetCurrentExecuteStatus()->HasError());
				ASSERT_EQ(result->GetCurrentExecuteStatus()->GetStatus(), ResultStatus::FatalError);

				result = connection->Execute(utils::string::Format("SELECT * FROM {};",
					GetRules().GetAttributeTableName(entityName, attributeType)));
				ASSERT_TRUE(result->GetCurrentExecuteStatus()->HasError());
				ASSERT_EQ(result->GetCurrentExecuteStatus()->GetStatus(), ResultStatus::FatalError);

				result = connection->Execute(utils::string::Format("SELECT * FROM {};",
					executorEAV->GetNamingRules().GetValueTableName(entityName, attributeType)));
				ASSERT_TRUE(result->GetCurrentExecuteStatus()->HasError());
				ASSERT_EQ(result->GetCurrentExecuteStatus()->GetStatus(), ResultStatus::FatalError);
			}
		}
	}

	// Выполним запрос в рамках отмененной транзакции, чтобы не сохранять эти таблицы
	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	for (auto && map : maps)
	{
		ASSERT_FALSE(executorEAV->SetRegisteredEntities(map, true)->HasError());
		ASSERT_EQ(executorEAV->GetRegisteredEntities(), map);

		for (auto &&[entityName, attributeTypes] : map)
		{
			// Проверим, что таблицы появились
			for (auto && type : attributeTypes)
			{
				auto && attributeType = converter->GetSQLVariable(type)->GetTypeName();
				auto result = connection->Execute(utils::string::Format("SELECT * FROM {};",
					executorEAV->GetNamingRules().GetEntityTableName(entityName)));
				ASSERT_FALSE(result->GetCurrentExecuteStatus()->HasError());
				ASSERT_EQ(result->GetCurrentExecuteStatus()->GetStatus(), ResultStatus::OkWithData);
				ASSERT_EQ(result->GetRowCount(), 0);
				ASSERT_EQ(result->GetColCount(), 1);
				ASSERT_NE(result->GetColIndex(GetRules().GetEntityTable_Short_IdField(entityName)),
					IExecuteResult::InvalidIndex);

				result = connection->Execute(utils::string::Format("SELECT * FROM {};",
					executorEAV->GetNamingRules().GetAttributeTableName(entityName, attributeType)));
				ASSERT_FALSE(result->GetCurrentExecuteStatus()->HasError());
				ASSERT_EQ(result->GetCurrentExecuteStatus()->GetStatus(), ResultStatus::OkWithData);
				ASSERT_EQ(result->GetRowCount(), 0);
				ASSERT_EQ(result->GetColCount(), 2);
				ASSERT_NE(result->GetColIndex(GetRules().GetAttributeTable_Short_IdField(entityName,
					attributeType)), IExecuteResult::InvalidIndex);
				ASSERT_NE(result->GetColIndex(GetRules().GetAttributeTable_Short_NameField(entityName,
					attributeType)), IExecuteResult::InvalidIndex);

				result = connection->Execute(utils::string::Format("SELECT * FROM {};",
					executorEAV->GetNamingRules().GetValueTableName(entityName, attributeType)));
				ASSERT_FALSE(result->GetCurrentExecuteStatus()->HasError());
				ASSERT_EQ(result->GetCurrentExecuteStatus()->GetStatus(), ResultStatus::OkWithData);
				ASSERT_EQ(result->GetRowCount(), 0);
				ASSERT_EQ(result->GetColCount(), 3);
				ASSERT_NE(result->GetColIndex(GetRules().GetValueTable_Short_EntityIdField(entityName,
					attributeType)), IExecuteResult::InvalidIndex);
				ASSERT_NE(result->GetColIndex(GetRules().GetValueTable_Short_AttributeIdField(entityName,
					attributeType)), IExecuteResult::InvalidIndex);
				ASSERT_NE(result->GetColIndex(GetRules().GetValueTable_Short_ValueField(entityName,
					attributeType)), IExecuteResult::InvalidIndex);
			}
		}
	}

	ASSERT_FALSE(connection->RollbackTransaction()->HasError());
}


/// SetRegisteredEntities не создает таблицы для всех валидных типов данных, если
/// его вызвать с флагом createTable false
TEST_F(ExecutorEAVWithEmptyEnvironment,
	RegisterEntitiesDoesNotCreateTablesWithAllValidTypesIfCallsWithCreateTableFlagEqualFalse)
{
	std::vector<SQLDataType> allTypes;
	for (int i = 0; i <= static_cast<int>(SQLDataType::LastValidType); ++i)
	{
		SQLDataType currentType = static_cast<SQLDataType>(i);
		allTypes.push_back(currentType);
	}

	std::vector<IExecutorEAV::EAVRegisterEntries> maps{ {
		{"SomeEntity1", allTypes},
		{"SomeEntity2", {SQLDataType::Integer}},
		{"SomeEntity3", {SQLDataType::Integer, SQLDataType::ByteArray}},
	} };

	// Проверим, что таблицы сейчас отсутствуют
	for (auto && map : maps)
	{
		for (auto &&[entityName, attributeTypes] : map)
		{
			for (auto && type : attributeTypes)
			{
				auto && attributeType = converter->GetSQLVariable(type)->GetTypeName();
				auto result = connection->Execute(utils::string::Format("SELECT * FROM {};",
					GetRules().GetEntityTableName(entityName)));
				ASSERT_TRUE(result->GetCurrentExecuteStatus()->HasError());
				ASSERT_EQ(result->GetCurrentExecuteStatus()->GetStatus(), ResultStatus::FatalError);

				result = connection->Execute(utils::string::Format("SELECT * FROM {};",
					GetRules().GetAttributeTableName(entityName, attributeType)));
				ASSERT_TRUE(result->GetCurrentExecuteStatus()->HasError());
				ASSERT_EQ(result->GetCurrentExecuteStatus()->GetStatus(), ResultStatus::FatalError);

				result = connection->Execute(utils::string::Format("SELECT * FROM {};",
					executorEAV->GetNamingRules().GetValueTableName(entityName, attributeType)));
				ASSERT_TRUE(result->GetCurrentExecuteStatus()->HasError());
				ASSERT_EQ(result->GetCurrentExecuteStatus()->GetStatus(), ResultStatus::FatalError);
			}
		}
	}

	// Выполним запрос в рамках отмененной транзакции, чтобы не сохранять эти таблицы,
	// если они вдруг создадутся
	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	for (auto && map : maps)
	{
		ASSERT_FALSE(executorEAV->SetRegisteredEntities(map, false)->HasError());
		ASSERT_EQ(executorEAV->GetRegisteredEntities(), map);

		for (auto &&[entityName, attributeTypes] : map)
		{
			// Проверим, что таблицы не появились
			for (auto && type : attributeTypes)
			{
				auto && attributeType = converter->GetSQLVariable(type)->GetTypeName();
				auto result = connection->Execute(utils::string::Format("SELECT * FROM {};",
					executorEAV->GetNamingRules().GetEntityTableName(entityName)));
				ASSERT_TRUE(result->GetCurrentExecuteStatus()->HasError());

				result = connection->Execute(utils::string::Format("SELECT * FROM {};",
					executorEAV->GetNamingRules().GetAttributeTableName(entityName, attributeType)));
				ASSERT_TRUE(result->GetCurrentExecuteStatus()->HasError());

				result = connection->Execute(utils::string::Format("SELECT * FROM {};",
					executorEAV->GetNamingRules().GetValueTableName(entityName, attributeType)));
				ASSERT_TRUE(result->GetCurrentExecuteStatus()->HasError());
			}
		}
	}

	ASSERT_FALSE(connection->RollbackTransaction()->HasError());
}


/// SetRegisteredEntities не создает таблицы, если с сущностями ассоциируются
/// невалидные типы атрибутов
TEST_F(ExecutorEAVWithEmptyEnvironment, RegisterEntitiesDoesNotCreateTablesWithInvalidTypes)
{
	std::vector<IExecutorEAV::EAVRegisterEntries> maps{ {
		{"SomeEntity1", {SQLDataType::Unknown}},
		{"SomeEntity2", {SQLDataType::Invalid}},
		{"SomeEntity3", {SQLDataType::Integer, SQLDataType::ByteArray, SQLDataType::Unknown}},
		{"SomeEntity4", {SQLDataType::Invalid, SQLDataType::Text}}
	} };

	ASSERT_FALSE(connection->BeginTransaction()->HasError());

	for (auto && map : maps)
	{
		auto status = executorEAV->SetRegisteredEntities(map, true);
		ASSERT_TRUE(status->HasError());
		ASSERT_EQ(status->GetStatus(), ResultStatus::FatalError);
		ASSERT_TRUE(executorEAV->GetRegisteredEntities().empty());

		for (auto &&[entityName, attributeTypes] : map)
		{
			for (auto && type : attributeTypes)
			{
				auto sqlVariable = converter->GetSQLVariable(type);
				if (!sqlVariable)
					continue;

				auto && attributeType = sqlVariable->GetTypeName();
				auto result = connection->Execute(utils::string::Format("SELECT * FROM {};",
					GetRules().GetEntityTableName(entityName)));
				ASSERT_TRUE(result->GetCurrentExecuteStatus()->HasError());
				ASSERT_EQ(result->GetCurrentExecuteStatus()->GetStatus(), ResultStatus::FatalError);

				result = connection->Execute(utils::string::Format("SELECT * FROM {};",
					GetRules().GetAttributeTableName(entityName, attributeType)));
				ASSERT_TRUE(result->GetCurrentExecuteStatus()->HasError());
				ASSERT_EQ(result->GetCurrentExecuteStatus()->GetStatus(), ResultStatus::FatalError);

				result = connection->Execute(utils::string::Format("SELECT * FROM {};",
					executorEAV->GetNamingRules().GetValueTableName(entityName, attributeType)));
				ASSERT_TRUE(result->GetCurrentExecuteStatus()->HasError());
				ASSERT_EQ(result->GetCurrentExecuteStatus()->GetStatus(), ResultStatus::FatalError);
			}
		}
	}

	ASSERT_FALSE(connection->RollbackTransaction()->HasError());
}


/// SetRegisteredEntities не создает таблицы, если с сущностями ассоциируются
/// невалидные типы атрибутов, при флаге createTable = false
TEST_F(ExecutorEAVWithEmptyEnvironment,
	RegisterEntitiesDoesNotCreateTablesWithInvalidTypesWithCreateTableFlagEqualFalse)
{
	std::vector<IExecutorEAV::EAVRegisterEntries> maps{ {
		{"SomeEntity1", {SQLDataType::Unknown}},
		{"SomeEntity2", {SQLDataType::Invalid}},
		{"SomeEntity3", {SQLDataType::Integer, SQLDataType::ByteArray, SQLDataType::Unknown}},
		{"SomeEntity4", {SQLDataType::Invalid, SQLDataType::Text}}
	} };

	ASSERT_FALSE(connection->BeginTransaction()->HasError());

	for (auto && map : maps)
	{
		auto status = executorEAV->SetRegisteredEntities(map, false);
		ASSERT_TRUE(status->HasError());
		ASSERT_EQ(status->GetStatus(), ResultStatus::FatalError);
		ASSERT_TRUE(executorEAV->GetRegisteredEntities().empty());

		for (auto &&[entityName, attributeTypes] : map)
		{
			for (auto && type : attributeTypes)
			{
				auto sqlVariable = converter->GetSQLVariable(type);
				if (!sqlVariable)
					continue;

				auto && attributeType = sqlVariable->GetTypeName();
				auto result = connection->Execute(utils::string::Format("SELECT * FROM {};",
					GetRules().GetEntityTableName(entityName)));
				ASSERT_TRUE(result->GetCurrentExecuteStatus()->HasError());
				ASSERT_EQ(result->GetCurrentExecuteStatus()->GetStatus(), ResultStatus::FatalError);

				result = connection->Execute(utils::string::Format("SELECT * FROM {};",
					GetRules().GetAttributeTableName(entityName, attributeType)));
				ASSERT_TRUE(result->GetCurrentExecuteStatus()->HasError());
				ASSERT_EQ(result->GetCurrentExecuteStatus()->GetStatus(), ResultStatus::FatalError);

				result = connection->Execute(utils::string::Format("SELECT * FROM {};",
					executorEAV->GetNamingRules().GetValueTableName(entityName, attributeType)));
				ASSERT_TRUE(result->GetCurrentExecuteStatus()->HasError());
				ASSERT_EQ(result->GetCurrentExecuteStatus()->GetStatus(), ResultStatus::FatalError);
			}
		}
	}

	ASSERT_FALSE(connection->RollbackTransaction()->HasError());
}


/// SetRegisteredEntities не пересоздает уже существующие таблицы
TEST_F(ExecutorEAVWithEmptyEnvironment, test1)
{

}


/// SetRegisteredEntities позволяет добавить новые таблицы и не удаляет старые
TEST_F(ExecutorEAVWithEmptyEnvironment, test2)
{

}
