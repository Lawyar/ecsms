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
#include <algorithm>


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


//------------------------------------------------------------------------------
/**
  Удалить таблицу
*/
//---
static bool DropTable(const std::string & tableName, IConnection & connection)
{
	return !connection.Execute(utils::string::Format("DROP TABLE {};", tableName))
		->GetCurrentExecuteStatus()->HasError();
}


//------------------------------------------------------------------------------
/**
  Удалить все таблицы, которые есть в мапе
*/
//---
static bool DropAllTables(const std::vector<IExecutorEAV::EAVRegisterEntries> & maps,
	IConnection & connection, const IExecutorEAVNamingRules & rules,
	const ISQLTypeConverter & converter)
{
	bool result = true;
	for (auto && map : maps)
	{
		for (auto &&[entityName, attributeTypes] : map)
		{
			for (auto && type : attributeTypes)
			{
				auto && sqlVar = converter.GetSQLVariable(type);
				if (!sqlVar)
					continue;
				auto && attributeType = sqlVar->GetTypeName();
				result &= DropTable(rules.GetValueTableName(entityName, attributeType),
					connection);
				result &= DropTable(rules.GetAttributeTableName(entityName, attributeType),
					connection);
			}
			result &= DropTable(rules.GetEntityTableName(entityName), connection);
		}
	}

	return result;
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

	// Откатим транзакцию
	ASSERT_FALSE(connection->RollbackTransaction()->HasError());

	// Если SetRegisteredEntities фиксирует транзакцию, то изменения сохранятся
	// (а предыдущий вызов проигнорируется)
	// Проверим, что измнения они не сохранились
	ASSERT_TRUE(AllTablesDoNotExist({ entries1 }, *connection, GetRules(), *converter));
}


////////////////////////////////////////////////////////////////////////////////
// Тесты CreateNewEntity
////////////////////////////////////////////////////////////////////////////////


/// CreateNewEntity создает новую сущность
TEST_F(ExecutorEAVWithEmptyEnvironment, CreateNewEntityCreatesNewEntity)
{
	const std::string entityName1 = "SomeEntity1", entityName2 = "SomeEntity2";
	const IExecutorEAV::EAVRegisterEntries entries({
		{entityName1, {SQLDataType::Integer}},
		{entityName2, {SQLDataType::Text}} });
	ASSERT_TRUE(AllTablesDoNotExist({ entries }, *connection, GetRules(), *converter));

	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	ASSERT_FALSE(executorEAV->SetRegisteredEntities(entries, true)->HasError());

	// Создадим первую сущность первого типа
	int result = -1;
	ASSERT_FALSE(executorEAV->CreateNewEntity(entityName1, result)->HasError());
	ASSERT_EQ(result, 1);
	// Создадим вторую сущность первого типа
	result = -1;
	ASSERT_FALSE(executorEAV->CreateNewEntity(entityName1, result)->HasError());
	ASSERT_EQ(result, 2);
	// Создадим первую сущность второго типа
	result = -1;
	ASSERT_FALSE(executorEAV->CreateNewEntity(entityName2, result)->HasError());
	ASSERT_EQ(result, 1);
	// Создадим вторую сущность второго типа
	result = -1;
	ASSERT_FALSE(executorEAV->CreateNewEntity(entityName2, result)->HasError());
	ASSERT_EQ(result, 2);
	// Создадим третью сущность второго типа
	result = -1;
	ASSERT_FALSE(executorEAV->CreateNewEntity(entityName2, result)->HasError());
	ASSERT_EQ(result, 3);

	{
		// Проверим, что создано всего две сущности первого типа
		auto result = connection->Execute(utils::string::Format(
			"SELECT * FROM {};",
			GetRules().GetEntityTableName(entityName1)));
		ASSERT_FALSE(result->GetCurrentExecuteStatus()->HasError());
		ASSERT_EQ(result->GetRowCount(), 2);
		ASSERT_EQ(result->GetColCount(), 1);
		ASSERT_EQ(result->GetValue(0, 0).ExtractString(), "1");
		ASSERT_EQ(result->GetValue(1, 0).ExtractString(), "2");
	}

	{
		// Проверим, что создано всего три сущности второго типа
		auto result = connection->Execute(utils::string::Format(
			"SELECT * FROM {};",
			GetRules().GetEntityTableName(entityName2)));
		ASSERT_FALSE(result->GetCurrentExecuteStatus()->HasError());
		ASSERT_EQ(result->GetRowCount(), 3);
		ASSERT_EQ(result->GetColCount(), 1);
		ASSERT_EQ(result->GetValue(0, 0).ExtractString(), "1");
		ASSERT_EQ(result->GetValue(1, 0).ExtractString(), "2");
		ASSERT_EQ(result->GetValue(2, 0).ExtractString(), "3");
	}

	ASSERT_FALSE(connection->RollbackTransaction()->HasError());
}


/// CreateNewEntity не фиксирует транзакцию
TEST_F(ExecutorEAVWithEmptyEnvironment, CreateNewEntityDoesNotEndTransaction)
{
	const std::string entityName = "SomeEntity1";
	const IExecutorEAV::EAVRegisterEntries entries1({ {entityName, {SQLDataType::Integer}} });
	ASSERT_TRUE(AllTablesDoNotExist({ entries1 }, *connection, GetRules(), *converter));

	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	ASSERT_FALSE(executorEAV->SetRegisteredEntities(entries1, true)->HasError());

	int result = -1;
	// Создадим сущность
	ASSERT_FALSE(executorEAV->CreateNewEntity(entityName, result)->HasError());
	ASSERT_EQ(result, 1);

	ASSERT_FALSE(connection->RollbackTransaction()->HasError());

	// Если CreateNewEntity фиксирует транзакцию, то таблицы создадутся и новая сущность добавится
	// Проверим, что это не так
	ASSERT_TRUE(AllTablesDoNotExist({ entries1 }, *connection, GetRules(), *converter));
}


/// CreateNewEntity не открывает транзакцию
TEST_F(ExecutorEAVWithEmptyEnvironment, CreateNewEntityDoesNotBeginTransaction)
{
	const std::string entityName = "SomeEntity1";
	const IExecutorEAV::EAVRegisterEntries entries1({ {entityName, {SQLDataType::Integer}} });
	ASSERT_TRUE(AllTablesDoNotExist({ entries1 }, *connection, GetRules(), *converter));

	ASSERT_FALSE(connection->BeginTransaction()->HasError());
	ASSERT_FALSE(executorEAV->SetRegisteredEntities(entries1, true)->HasError());
	ASSERT_FALSE(connection->CommitTransaction()->HasError());

	int result = -1;
	// Создадим сущность
	ASSERT_FALSE(executorEAV->CreateNewEntity(entityName, result)->HasError());
	ASSERT_EQ(result, 1);

	// Попробуем откатить транзакцию.
	// Если это получится, значит CreateNewEntity открывает транзакцию
	ASSERT_FALSE(connection->RollbackTransaction()->HasError());

	{
		auto result = connection->Execute(utils::string::Format(
			"SELECT * FROM {};",
			GetRules().GetEntityTableName(entityName)));
		ASSERT_FALSE(result->GetCurrentExecuteStatus()->HasError());
		// Если количество строк равно нулю, значит транзакция откатилась
		ASSERT_EQ(result->GetRowCount(), 1);
		ASSERT_EQ(result->GetColCount(), 1);
	}

	ASSERT_TRUE(DropAllTables({ entries1 }, *connection, GetRules(), *converter));
}


////////////////////////////////////////////////////////////////////////////////
// Тесты Insert/Update/InsertOrUpdate, тестирующие доступность всех валидных типов
////////////////////////////////////////////////////////////////////////////////

// Тест для проверок с предварительным созданием таблиц для всех валидных типов
class ExecutorEAVWithPreparedEnvironment : public ExecutorEAVWithEmptyEnvironment
{
protected:
	std::vector<std::string> createdFileNames; ///< Созданные файлы
	IExecutorEAV::EAVRegisterEntries registeredEntries; ///< Зарегистрированные сущности
	std::map<SQLDataType, ISQLTypePtr> values; ///< Валидные SQL-переменные
	std::map<SQLDataType, ISQLTypePtr> values2; ///< Второй набор валидных переменных
	std::map<SQLDataType, std::string> expectedValuesFromSQL; ///< Ожидаемое текстовое представление SQL-переменных, которое хотим получить от сервера
	std::map<SQLDataType, std::string> expectedValues2FromSQL; ///< Второй набор ожидаемых значений

	std::map<SQLDataType, ISQLTypeTextPtr> attributeNames; ///< Названия атрибутов

protected:
	// Действия в начале теста
	virtual void SetUp() override
	{
		ExecutorEAVWithEmptyEnvironment::SetUp();

		std::vector<SQLDataType> allValidTypes;
		for (int i = 0; i <= static_cast<int>(SQLDataType::LastValidType); ++i)
			allValidTypes.push_back(static_cast<SQLDataType>(i));

		values[SQLDataType::Integer] = converter->GetSQLTypeInteger(5);
		expectedValuesFromSQL[SQLDataType::Integer] = "5";
		values2[SQLDataType::Integer] = converter->GetSQLTypeInteger(7);
		expectedValues2FromSQL[SQLDataType::Integer] = "7";
		attributeNames[SQLDataType::Integer] = converter->GetSQLTypeText("SomeIntegerAttr");

		values[SQLDataType::Text] = converter->GetSQLTypeText("hello");
		expectedValuesFromSQL[SQLDataType::Text] = "hello";
		values2[SQLDataType::Text] = converter->GetSQLTypeText("bye");
		expectedValues2FromSQL[SQLDataType::Text] = "bye";
		attributeNames[SQLDataType::Text] = converter->GetSQLTypeText("SomeTextAttr");

		values[SQLDataType::ByteArray] = converter->GetSQLTypeByteArray({ 1, 2, 3, 4, 5 });
		expectedValuesFromSQL[SQLDataType::ByteArray] = "\\x0102030405";
		values2[SQLDataType::ByteArray] = converter->GetSQLTypeByteArray({ 0, 1, 0, 1, 0 });
		expectedValues2FromSQL[SQLDataType::ByteArray] = "\\x0001000100";
		attributeNames[SQLDataType::ByteArray] = converter->GetSQLTypeText("SomeByteArrayAttr");

		{
			auto && remoteFilePtr = connection->CreateRemoteFile();
			values[SQLDataType::RemoteFileId] =
				converter->GetSQLTypeRemoteFileId(remoteFilePtr->GetFileName());
			expectedValuesFromSQL[SQLDataType::RemoteFileId] = remoteFilePtr->GetFileName();
			createdFileNames.push_back(remoteFilePtr->GetFileName());

			remoteFilePtr = connection->CreateRemoteFile();
			values2[SQLDataType::RemoteFileId] =
				converter->GetSQLTypeRemoteFileId(remoteFilePtr->GetFileName());
			expectedValues2FromSQL[SQLDataType::RemoteFileId] = remoteFilePtr->GetFileName();
			createdFileNames.push_back(remoteFilePtr->GetFileName());

			attributeNames[SQLDataType::RemoteFileId] = converter->GetSQLTypeText("SomeRemoteFileIdAttr");
		}

		// Нужно создать переменные всех типов
		ASSERT_EQ(values.size(), static_cast<int>(SQLDataType::LastValidType) + 1);
		ASSERT_EQ(values2.size(), static_cast<int>(SQLDataType::LastValidType) + 1);
		ASSERT_EQ(expectedValuesFromSQL.size(), static_cast<int>(SQLDataType::LastValidType) + 1);
		ASSERT_EQ(expectedValues2FromSQL.size(), static_cast<int>(SQLDataType::LastValidType) + 1);
		ASSERT_EQ(attributeNames.size(), static_cast<int>(SQLDataType::LastValidType) + 1);

		const std::string entityName1 = "SomeEntity1", entityName2 = "SomeEntity2";
		registeredEntries = IExecutorEAV::EAVRegisterEntries({
			{entityName1, allValidTypes},
			{entityName2, allValidTypes} });
		ASSERT_TRUE(AllTablesDoNotExist({ registeredEntries }, *connection, GetRules(), *converter));

		ASSERT_FALSE(executorEAV->SetRegisteredEntities(registeredEntries, true)->HasError());
	}

	// Действия в конце теста
	virtual void TearDown() override
	{
		// Отменим на всякий случай транзакцию, если сейчас есть активная
		ASSERT_FALSE(connection->RollbackTransaction()->HasError());

		for (auto && createdFileName : createdFileNames)
			ASSERT_TRUE(connection->DeleteRemoteFile(createdFileName));

		ASSERT_TRUE(DropAllTables({ registeredEntries }, *connection, GetRules(), *converter));

		ExecutorEAVWithEmptyEnvironment::TearDown();
	}
};

/// Insert вставляет значения всех доступных типов
TEST_F(ExecutorEAVWithPreparedEnvironment, InsertInsertsValue)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());

	for (auto &&[entityName, attributeTypes] : registeredEntries)
	{
		int result = -1;
		ASSERT_FALSE(executorEAV->CreateNewEntity(entityName, result)->HasError());
		ASSERT_EQ(result, 1);

		for (auto && attributeType : attributeTypes)
		{
			ASSERT_FALSE(executorEAV->Insert(entityName, result, attributeNames[attributeType],
				values[attributeType])->HasError());
		}
	}

	for (auto && [entityName, attributeTypes] : registeredEntries)
	{
		for (auto && attributeType : attributeTypes)
		{
			auto && attributeTypeName = converter->GetSQLVariable(attributeType)->GetTypeName();

			auto attributeTableName = GetRules().GetAttributeTableName(entityName, attributeTypeName);
			auto valueTableName = GetRules().GetValueTableName(entityName, attributeTypeName);

			{
				auto result = connection->Execute(utils::string::Format(
					"SELECT * FROM {};", attributeTableName));
				ASSERT_FALSE(result->GetCurrentExecuteStatus()->HasError());
				ASSERT_EQ(result->GetRowCount(), 1);
				ASSERT_EQ(result->GetColCount(), 2);
				auto attrId = GetRules().GetAttributeTable_Short_IdField(entityName, attributeTypeName);
				ASSERT_EQ(result->GetColIndex(attrId), 0);
				auto attrName = GetRules().GetAttributeTable_Short_NameField(entityName, attributeTypeName);
				ASSERT_EQ(result->GetColIndex(attrName), 1);

				ASSERT_EQ(result->GetValue(0, 0).ExtractString(), "1");
				ASSERT_EQ(result->GetValue(0, 1).ExtractString(), attributeNames[attributeType]->GetValue());
			}
			{
				auto result = connection->Execute(utils::string::Format(
					"SELECT * FROM {};", valueTableName));
				ASSERT_FALSE(result->GetCurrentExecuteStatus()->HasError());

				ASSERT_EQ(result->GetRowCount(), 1);
				ASSERT_EQ(result->GetColCount(), 3);
				auto entityId = GetRules().GetValueTable_Short_EntityIdField(entityName, attributeTypeName);
				ASSERT_EQ(result->GetColIndex(entityId), 0);
				auto attrId = GetRules().GetValueTable_Short_AttributeIdField(entityName, attributeTypeName);
				ASSERT_EQ(result->GetColIndex(attrId), 1);
				auto valueField = GetRules().GetValueTable_Short_ValueField(entityName, attributeTypeName);
				ASSERT_EQ(result->GetColIndex(valueField), 2);

				ASSERT_EQ(result->GetValue(0, 0).ExtractString(), "1");
				ASSERT_EQ(result->GetValue(0, 1).ExtractString(), "1");
				ASSERT_EQ(result->GetValue(0, 2).ExtractString(), expectedValuesFromSQL[attributeType]);
			}
		}
	}

	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}


/// Update обновляет значения всех доступных типов
TEST_F(ExecutorEAVWithPreparedEnvironment, UpdateUpdatesValue)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());

	for (auto &&[entityName, attributeTypes] : registeredEntries)
	{
		int result = -1;
		ASSERT_FALSE(executorEAV->CreateNewEntity(entityName, result)->HasError());
		ASSERT_EQ(result, 1);

		for (auto && attributeType : attributeTypes)
		{
			ASSERT_FALSE(executorEAV->Insert(entityName, result, attributeNames[attributeType],
				values[attributeType])->HasError());
			ASSERT_FALSE(executorEAV->Update(entityName, result, attributeNames[attributeType],
				values2[attributeType])->HasError());
		}
	}

	for (auto &&[entityName, attributeTypes] : registeredEntries)
	{
		for (auto && attributeType : attributeTypes)
		{
			auto && attributeTypeName = converter->GetSQLVariable(attributeType)->GetTypeName();

			auto attributeTableName = GetRules().GetAttributeTableName(entityName, attributeTypeName);
			auto valueTableName = GetRules().GetValueTableName(entityName, attributeTypeName);

			{
				auto result = connection->Execute(utils::string::Format(
					"SELECT * FROM {};", attributeTableName));
				ASSERT_FALSE(result->GetCurrentExecuteStatus()->HasError());
				ASSERT_EQ(result->GetRowCount(), 1);
				ASSERT_EQ(result->GetColCount(), 2);
				auto attrId = GetRules().GetAttributeTable_Short_IdField(entityName, attributeTypeName);
				ASSERT_EQ(result->GetColIndex(attrId), 0);
				auto attrName = GetRules().GetAttributeTable_Short_NameField(entityName, attributeTypeName);
				ASSERT_EQ(result->GetColIndex(attrName), 1);

				ASSERT_EQ(result->GetValue(0, 0).ExtractString(), "1");
				ASSERT_EQ(result->GetValue(0, 1).ExtractString(), attributeNames[attributeType]->GetValue());
			}
			{
				auto result = connection->Execute(utils::string::Format(
					"SELECT * FROM {};", valueTableName));
				ASSERT_FALSE(result->GetCurrentExecuteStatus()->HasError());

				ASSERT_EQ(result->GetRowCount(), 1);
				ASSERT_EQ(result->GetColCount(), 3);
				auto entityId = GetRules().GetValueTable_Short_EntityIdField(entityName, attributeTypeName);
				ASSERT_EQ(result->GetColIndex(entityId), 0);
				auto attrId = GetRules().GetValueTable_Short_AttributeIdField(entityName, attributeTypeName);
				ASSERT_EQ(result->GetColIndex(attrId), 1);
				auto valueField = GetRules().GetValueTable_Short_ValueField(entityName, attributeTypeName);
				ASSERT_EQ(result->GetColIndex(valueField), 2);

				ASSERT_EQ(result->GetValue(0, 0).ExtractString(), "1");
				ASSERT_EQ(result->GetValue(0, 1).ExtractString(), "1");
				ASSERT_EQ(result->GetValue(0, 2).ExtractString(), expectedValues2FromSQL[attributeType]);
			}
		}
	}

	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}


/// InsertOrUpdate вставляет или обновляет значения всех доступных типов
TEST_F(ExecutorEAVWithPreparedEnvironment, InsertOrUpdateInsertsOrUpdatesValue)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());

	for (auto &&[entityName, attributeTypes] : registeredEntries)
	{
		int result = -1;
		ASSERT_FALSE(executorEAV->CreateNewEntity(entityName, result)->HasError());
		ASSERT_EQ(result, 1);

		for (auto && attributeType : attributeTypes)
		{
			ASSERT_FALSE(executorEAV->InsertOrUpdate(entityName, result, attributeNames[attributeType],
				values[attributeType])->HasError());
		}
	}

	for (auto &&[entityName, attributeTypes] : registeredEntries)
	{
		for (auto && attributeType : attributeTypes)
		{
			auto && attributeTypeName = converter->GetSQLVariable(attributeType)->GetTypeName();

			auto attributeTableName = GetRules().GetAttributeTableName(entityName, attributeTypeName);
			auto valueTableName = GetRules().GetValueTableName(entityName, attributeTypeName);

			{
				auto result = connection->Execute(utils::string::Format(
					"SELECT * FROM {};", attributeTableName));
				ASSERT_FALSE(result->GetCurrentExecuteStatus()->HasError());
				ASSERT_EQ(result->GetRowCount(), 1);
				ASSERT_EQ(result->GetColCount(), 2);
				auto attrId = GetRules().GetAttributeTable_Short_IdField(entityName, attributeTypeName);
				ASSERT_EQ(result->GetColIndex(attrId), 0);
				auto attrName = GetRules().GetAttributeTable_Short_NameField(entityName, attributeTypeName);
				ASSERT_EQ(result->GetColIndex(attrName), 1);

				ASSERT_EQ(result->GetValue(0, 0).ExtractString(), "1");
				ASSERT_EQ(result->GetValue(0, 1).ExtractString(), attributeNames[attributeType]->GetValue());
			}
			{
				auto result = connection->Execute(utils::string::Format(
					"SELECT * FROM {};", valueTableName));
				ASSERT_FALSE(result->GetCurrentExecuteStatus()->HasError());

				ASSERT_EQ(result->GetRowCount(), 1);
				ASSERT_EQ(result->GetColCount(), 3);
				auto entityId = GetRules().GetValueTable_Short_EntityIdField(entityName, attributeTypeName);
				ASSERT_EQ(result->GetColIndex(entityId), 0);
				auto attrId = GetRules().GetValueTable_Short_AttributeIdField(entityName, attributeTypeName);
				ASSERT_EQ(result->GetColIndex(attrId), 1);
				auto valueField = GetRules().GetValueTable_Short_ValueField(entityName, attributeTypeName);
				ASSERT_EQ(result->GetColIndex(valueField), 2);

				ASSERT_EQ(result->GetValue(0, 0).ExtractString(), "1");
				ASSERT_EQ(result->GetValue(0, 1).ExtractString(), "1");
				ASSERT_EQ(result->GetValue(0, 2).ExtractString(), expectedValuesFromSQL[attributeType]);
			}
		}
	}

	for (auto &&[entityName, attributeTypes] : registeredEntries)
	{
		for (auto && attributeType : attributeTypes)
		{
			ASSERT_FALSE(executorEAV->InsertOrUpdate(entityName, 1, attributeNames[attributeType],
				values2[attributeType])->HasError());
		}
	}

	for (auto &&[entityName, attributeTypes] : registeredEntries)
	{
		for (auto && attributeType : attributeTypes)
		{
			auto && attributeTypeName = converter->GetSQLVariable(attributeType)->GetTypeName();

			auto attributeTableName = GetRules().GetAttributeTableName(entityName, attributeTypeName);
			auto valueTableName = GetRules().GetValueTableName(entityName, attributeTypeName);

			{
				auto result = connection->Execute(utils::string::Format(
					"SELECT * FROM {};", attributeTableName));
				ASSERT_FALSE(result->GetCurrentExecuteStatus()->HasError());
				ASSERT_EQ(result->GetRowCount(), 1);
				ASSERT_EQ(result->GetColCount(), 2);
				auto attrId = GetRules().GetAttributeTable_Short_IdField(entityName, attributeTypeName);
				ASSERT_EQ(result->GetColIndex(attrId), 0);
				auto attrName = GetRules().GetAttributeTable_Short_NameField(entityName, attributeTypeName);
				ASSERT_EQ(result->GetColIndex(attrName), 1);

				ASSERT_EQ(result->GetValue(0, 0).ExtractString(), "1");
				ASSERT_EQ(result->GetValue(0, 1).ExtractString(), attributeNames[attributeType]->GetValue());
			}
			{
				auto result = connection->Execute(utils::string::Format(
					"SELECT * FROM {};", valueTableName));
				ASSERT_FALSE(result->GetCurrentExecuteStatus()->HasError());

				ASSERT_EQ(result->GetRowCount(), 1);
				ASSERT_EQ(result->GetColCount(), 3);
				auto entityId = GetRules().GetValueTable_Short_EntityIdField(entityName, attributeTypeName);
				ASSERT_EQ(result->GetColIndex(entityId), 0);
				auto attrId = GetRules().GetValueTable_Short_AttributeIdField(entityName, attributeTypeName);
				ASSERT_EQ(result->GetColIndex(attrId), 1);
				auto valueField = GetRules().GetValueTable_Short_ValueField(entityName, attributeTypeName);
				ASSERT_EQ(result->GetColIndex(valueField), 2);

				ASSERT_EQ(result->GetValue(0, 0).ExtractString(), "1");
				ASSERT_EQ(result->GetValue(0, 1).ExtractString(), "1");
				ASSERT_EQ(result->GetValue(0, 2).ExtractString(), expectedValues2FromSQL[attributeType]);
			}
		}
	}

	ASSERT_FALSE(connection->CommitTransaction()->HasError());
}
