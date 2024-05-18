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


//------------------------------------------------------------------------------
/**
  Существует ли таблица
*/
//---
static bool IsTableExist(const std::string & tableName, IConnection & connection)
{
	auto result = connection.Execute(utils::string::Format(
			"SELECT * FROM {};",
			tableName));
	return !result->GetCurrentExecuteStatus()->HasError();
}


//------------------------------------------------------------------------------
/**
  Вспомогательная функция для реализации AllTablesExist/AllTablesDoNotExist
*/
//---
static bool allTablesExistImpl(const std::vector<IExecutorEAV::EAVRegisterEntries> & maps,
	IConnection & connection, const IExecutorEAVNamingRules & rules,
	const ISQLTypeConverter & converter, bool exist)
{
	bool result = true;
	for (auto && map : maps)
	{
		for (auto &&[entityName, attributeTypes] : map)
		{
			result &= (exist == IsTableExist(rules.GetEntityTableName(entityName), connection));
			for (auto && type : attributeTypes)
			{
				auto && sqlVar = converter.GetSQLVariable(type);
				if (!sqlVar)
					continue;
				auto && attributeType = sqlVar->GetTypeName();
				result &= (exist == IsTableExist(rules.GetAttributeTableName(entityName,
					attributeType), connection));
				result &= (exist == IsTableExist(rules.GetValueTableName(entityName,
					attributeType), connection));
			}
		}
	}

	return result;
}


//------------------------------------------------------------------------------
/**
  Все таблицы в векторе мап существуют
*/
//---
static bool AllTablesExist(const std::vector<IExecutorEAV::EAVRegisterEntries> & maps,
	IConnection & connection, const IExecutorEAVNamingRules & rules,
	const ISQLTypeConverter & converter)
{
	return allTablesExistImpl(maps, connection, rules, converter, true);
}


//------------------------------------------------------------------------------
/**
  Все таблицы в векторе мап не существуют
*/
//---
static bool AllTablesDoNotExist(const std::vector<IExecutorEAV::EAVRegisterEntries> & maps,
	IConnection & connection, const IExecutorEAVNamingRules & rules,
	const ISQLTypeConverter & converter)
{
	return allTablesExistImpl(maps, connection, rules, converter, false);
}


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
// Тесты SetRegisteredEntities/GetRegisteredEntities
////////////////////////////////////////////////////////////////////////////////


/// SetRegisteredEntities создает таблицы для всех валидных типов данных,
/// если передать флаг createTable = true
TEST_F(ExecutorEAVWithEmptyEnvironment, RegisterEntitiesCreatesTablesWithAllValidTypesWithCreateTableFlagEqualTrue)
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
	ASSERT_TRUE(AllTablesDoNotExist(maps, *connection, GetRules(), *converter));

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
	ASSERT_TRUE(AllTablesDoNotExist(maps, *connection, GetRules(), *converter));

	// Выполним запрос в рамках отмененной транзакции, чтобы не сохранять эти таблицы,
	// если они вдруг создадутся
	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	for (auto && map : maps)
	{
		ASSERT_FALSE(executorEAV->SetRegisteredEntities(map, false)->HasError());
		ASSERT_EQ(executorEAV->GetRegisteredEntities(), map);
	}

	ASSERT_TRUE(AllTablesDoNotExist(maps, *connection, GetRules(), *converter));
	ASSERT_FALSE(connection->RollbackTransaction()->HasError());
}


/// SetRegisteredEntities не создает таблицы, если с сущностями ассоциируются
/// невалидные типы атрибутов, даже если передать флаг createTable = true
TEST_F(ExecutorEAVWithEmptyEnvironment, RegisterEntitiesDoesNotCreateTablesWithInvalidTypesWithCreateTableFlagEqualTrue)
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

		ASSERT_TRUE(AllTablesDoNotExist({ map }, *connection, GetRules(), *converter));
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

		ASSERT_TRUE(AllTablesDoNotExist({ map }, *connection, GetRules(), *converter));
	}

	ASSERT_FALSE(connection->RollbackTransaction()->HasError());
}


/// SetRegisteredEntities не пересоздает уже существующие таблицы
/// при флаге createTable = true
TEST_F(ExecutorEAVWithEmptyEnvironment,
	SetRegisteredEntitiesDoesNotRecreateExistingTablesWithFlagCreateTableEqualTrue)
{
	const IExecutorEAV::EAVRegisterEntries entries({ {"SomeEntity", {SQLDataType::Integer}} });
	std::vector<std::string> tableNames;
	for (auto &&[entityName, attributeTypes] : entries)
	{
		tableNames.push_back(GetRules().GetEntityTableName(entityName));
		for (auto && attributeType : attributeTypes)
		{
			auto && attributeTypeName = converter->GetSQLVariable(attributeType)->GetTypeName();
			tableNames.push_back(GetRules().GetAttributeTableName(entityName,
				attributeTypeName));
			tableNames.push_back(GetRules().GetValueTableName(entityName,
				attributeTypeName));
		}
	}

	for (auto && tableName : tableNames)
	{
		{
			// Удалим существующие таблицы, если они есть
			auto result = connection->Execute(utils::string::Format(
				"DROP TABLE IF EXISTS {};",
				tableName));
			ASSERT_FALSE(result->GetCurrentExecuteStatus()->HasError());
		}

		{
			// Создадим такие таблицы
			auto result = connection->Execute(utils::string::Format(
				"CREATE TABLE {} (field TEXT PRIMARY KEY);",
				tableName));
			ASSERT_FALSE(result->GetCurrentExecuteStatus()->HasError());
		}

		{
			// Наполним их содержимым
			auto result = connection->Execute(utils::string::Format(
				"INSERT INTO {} VALUES('hello');",
				tableName));
			ASSERT_FALSE(result->GetCurrentExecuteStatus()->HasError());
		}
	}

	{
		// Вызовем команду executorEAV
		auto status = executorEAV->SetRegisteredEntities(entries, true);
		ASSERT_FALSE(status->HasError());
		ASSERT_EQ(executorEAV->GetRegisteredEntities(), entries);
	}

	for (auto && tableName : tableNames)
	{
		{
			// Проверим, что executorEAV не пересоздал таблицы
			auto result = connection->Execute(utils::string::Format("SELECT * FROM {};",
				tableName));
			ASSERT_FALSE(result->GetCurrentExecuteStatus()->HasError());
			ASSERT_EQ(result->GetRowCount(), 1);
			ASSERT_EQ(result->GetColCount(), 1);
		}

		{
			// Удалим таблицы
			auto result = connection->Execute(utils::string::Format(
				"DROP TABLE {};",
				tableName));
			ASSERT_FALSE(result->GetCurrentExecuteStatus()->HasError());
		}
	}
}


/// SetRegisteredEntities не пересоздает уже существующие таблицы
/// при флаге createTable = false
TEST_F(ExecutorEAVWithEmptyEnvironment,
	SetRegisteredEntitiesDoesNotRecreateExistingTablesWithFlagCreateTableEqualFalse)
{
	const IExecutorEAV::EAVRegisterEntries entries({ {"SomeEntity", {SQLDataType::Integer}} });
	std::vector<std::string> tableNames;
	for (auto &&[entityName, attributeTypes] : entries)
	{
		tableNames.push_back(GetRules().GetEntityTableName(entityName));
		for (auto && attributeType : attributeTypes)
		{
			auto && attributeTypeName = converter->GetSQLVariable(attributeType)->GetTypeName();
			tableNames.push_back(GetRules().GetAttributeTableName(entityName,
				attributeTypeName));
			tableNames.push_back(GetRules().GetValueTableName(entityName,
				attributeTypeName));
		}
	}

	for (auto && tableName : tableNames)
	{
		{
			// Удалим существующие таблицы, если они есть
			auto result = connection->Execute(utils::string::Format(
				"DROP TABLE IF EXISTS {};",
				tableName));
			ASSERT_FALSE(result->GetCurrentExecuteStatus()->HasError());
		}

		{
			// Создадим такие таблицы
			auto result = connection->Execute(utils::string::Format(
				"CREATE TABLE {} (field TEXT PRIMARY KEY);",
				tableName));
			ASSERT_FALSE(result->GetCurrentExecuteStatus()->HasError());
		}

		{
			// Наполним их содержимым
			auto result = connection->Execute(utils::string::Format(
				"INSERT INTO {} VALUES('hello');",
				tableName));
			ASSERT_FALSE(result->GetCurrentExecuteStatus()->HasError());
		}
	}

	{
		// Вызовем команду executorEAV
		auto status = executorEAV->SetRegisteredEntities(entries, false);
		ASSERT_FALSE(status->HasError());
		ASSERT_EQ(executorEAV->GetRegisteredEntities(), entries);
	}

	for (auto && tableName : tableNames)
	{
		{
			// Проверим, что executorEAV не пересоздал таблицы
			auto result = connection->Execute(utils::string::Format("SELECT * FROM {};",
				tableName));
			ASSERT_FALSE(result->GetCurrentExecuteStatus()->HasError());
			ASSERT_EQ(result->GetRowCount(), 1);
			ASSERT_EQ(result->GetColCount(), 1);
		}

		{
			// Удалим таблицы
			auto result = connection->Execute(utils::string::Format(
				"DROP TABLE {};",
				tableName));
			ASSERT_FALSE(result->GetCurrentExecuteStatus()->HasError());
		}
	}
}


/// SetRegisteredEntities позволяет добавить новые таблицы и не удаляет старые
/// при флаге createTable = true
TEST_F(ExecutorEAVWithEmptyEnvironment,
	SetRegisteredEntitiesAllowsToAddNewTablesAndDoesNotRemoveOldTablesWithCreateTableFlagEqualTrue)
{
	const IExecutorEAV::EAVRegisterEntries entries1({ {"SomeEntity1", {SQLDataType::Integer}} });
	const IExecutorEAV::EAVRegisterEntries entries2({ {"SomeEntity2", {SQLDataType::Integer}} });
	ASSERT_TRUE(AllTablesDoNotExist({ entries1, entries2 }, *connection, GetRules(), *converter));

	ASSERT_FALSE(connection->BeginTransaction()->HasError());

	ASSERT_FALSE(executorEAV->SetRegisteredEntities(entries1, true)->HasError());
	ASSERT_EQ(executorEAV->GetRegisteredEntities(), entries1);
	ASSERT_TRUE(AllTablesExist({ entries1 }, *connection, GetRules(), *converter));

	ASSERT_FALSE(executorEAV->SetRegisteredEntities(entries2, true)->HasError());
	ASSERT_EQ(executorEAV->GetRegisteredEntities(), entries2);
	ASSERT_TRUE(AllTablesExist({ entries1, entries2 }, *connection, GetRules(), *converter));

	ASSERT_FALSE(connection->RollbackTransaction()->HasError());
}


/// SetRegisteredEntities не закрывает транзакцию
/// при флаге createTable = true
TEST_F(ExecutorEAVWithEmptyEnvironment,
	SetRegisteredEntitiesDoesNotEndTransactionWithCreateTableFlagEqualTrue)
{
	const IExecutorEAV::EAVRegisterEntries entries1({ {"SomeEntity1", {SQLDataType::Integer}} });
	ASSERT_TRUE(AllTablesDoNotExist({ entries1 }, *connection, GetRules(), *converter));

	ASSERT_FALSE(connection->BeginTransaction()->HasError());

	ASSERT_FALSE(executorEAV->SetRegisteredEntities(entries1, true)->HasError());
	ASSERT_EQ(executorEAV->GetRegisteredEntities(), entries1);
	ASSERT_TRUE(AllTablesExist({ entries1 }, *connection, GetRules(), *converter));

	// Сбросим транзакцию, закрыв соединение
	executorEAV.reset();
	ASSERT_EQ(connection.use_count(), 1);
	connection.reset();

	// Получим новое соединение
	connection = GetDatabaseManager().GetConnection(c_PostgreSQLConnectionURL);
	executorEAV = GetDatabaseManager().GetExecutorEAV(connection);

	// Если SetRegisteredEntities фиксирует транзакцию, то изменения сохранятся.
	// Проверим, что они не сохранились
	ASSERT_TRUE(AllTablesDoNotExist({ entries1 }, *connection, GetRules(), *converter));
}
