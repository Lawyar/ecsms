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


////////////////////////////////////////////////////////////////////////////////
/// Тесты Insert/Update/InsertOrUpdate, которые проверяют, что
/// команды возвращают ошибку, если их выполнить для невалидного вида сущности
////////////////////////////////////////////////////////////////////////////////

/// Insert не вставляет значение при невалидном виде сущности
TEST_F(ExecutorEAVWithEmptyEnvironment, InsertDoesNotInsertsWithInvalidEntityName)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());

	const std::string existingEntityName = "SomeEntity1";
	const std::string nonExistingEntityName = "SomeEntity2";

	const SQLDataType attributeType = SQLDataType::Integer;
	const std::string attributeTypeName = converter->GetSQLVariable(SQLDataType::Integer)
		->GetTypeName();

	ASSERT_FALSE(executorEAV->SetRegisteredEntities({ {existingEntityName, {attributeType}} }, true)
		->HasError());

	int result = -1;
	ASSERT_FALSE(executorEAV->CreateNewEntity(existingEntityName, result)->HasError());
	ASSERT_EQ(result, 1);

	std::string attributeName = "SomeIntegerAttr";
	ASSERT_TRUE(executorEAV->Insert(nonExistingEntityName, result,
		converter->GetSQLTypeText(std::string(attributeName)), converter->GetSQLTypeInteger(5))
		->HasError());

	// Проверим, что таблицы не создались
	ASSERT_FALSE(IsTableExist(
		GetRules().GetEntityTableName(nonExistingEntityName),
		*connection
	));
	ASSERT_FALSE(IsTableExist(
		GetRules().GetAttributeTableName(nonExistingEntityName, attributeTypeName),
		*connection
		));
	ASSERT_FALSE(IsTableExist(
		GetRules().GetValueTableName(nonExistingEntityName, attributeTypeName),
		*connection
	));

	ASSERT_FALSE(connection->RollbackTransaction()->HasError());
}


/// InsertOrUpdate не вставляет значение при невалидном виде сущности
TEST_F(ExecutorEAVWithEmptyEnvironment, InsertOrUpdateDoesNotInsertsWithInvalidEntityName)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());

	const std::string existingEntityName = "SomeEntity1";
	const std::string nonExistingEntityName = "SomeEntity2";

	const SQLDataType attributeType = SQLDataType::Integer;
	const std::string attributeTypeName = converter->GetSQLVariable(SQLDataType::Integer)
		->GetTypeName();

	ASSERT_FALSE(executorEAV->SetRegisteredEntities({ {existingEntityName, {attributeType}} }, true)
		->HasError());

	int result = -1;
	ASSERT_FALSE(executorEAV->CreateNewEntity(existingEntityName, result)->HasError());
	ASSERT_EQ(result, 1);

	std::string attributeName = "SomeIntegerAttr";
	ASSERT_TRUE(executorEAV->InsertOrUpdate(nonExistingEntityName, result,
		converter->GetSQLTypeText(std::string(attributeName)), converter->GetSQLTypeInteger(5))
		->HasError());

	// Проверим, что таблицы не создались
	ASSERT_FALSE(IsTableExist(
		GetRules().GetEntityTableName(nonExistingEntityName),
		*connection
	));
	ASSERT_FALSE(IsTableExist(
		GetRules().GetAttributeTableName(nonExistingEntityName, attributeTypeName),
		*connection
	));
	ASSERT_FALSE(IsTableExist(
		GetRules().GetValueTableName(nonExistingEntityName, attributeTypeName),
		*connection
	));

	ASSERT_FALSE(connection->RollbackTransaction()->HasError());
}


/// Update не обновляет значение при невалидном виде сущности
TEST_F(ExecutorEAVWithEmptyEnvironment, UpdateDoesNotUpdatesWithInvalidEntityName)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());

	const std::string existingEntityName = "SomeEntity1";
	const std::string nonExistingEntityName = "SomeEntity2";

	const SQLDataType attributeType = SQLDataType::Integer;
	const std::string attributeTypeName = converter->GetSQLVariable(SQLDataType::Integer)
		->GetTypeName();

	ASSERT_FALSE(executorEAV->SetRegisteredEntities({ {existingEntityName, {attributeType}} }, true)
		->HasError());

	int result = -1;
	ASSERT_FALSE(executorEAV->CreateNewEntity(existingEntityName, result)->HasError());
	ASSERT_EQ(result, 1);

	std::string attributeName = "SomeIntegerAttr";
	executorEAV->Insert(existingEntityName, result,
		converter->GetSQLTypeText(std::string(attributeName)), converter->GetSQLTypeInteger(5));

	ASSERT_TRUE(executorEAV->Update(nonExistingEntityName, result,
		converter->GetSQLTypeText(std::string(attributeName)), converter->GetSQLTypeInteger(6))
		->HasError());

	// Проверим, что таблицы не создались
	ASSERT_FALSE(IsTableExist(
		GetRules().GetEntityTableName(nonExistingEntityName),
		*connection
	));
	ASSERT_FALSE(IsTableExist(
		GetRules().GetAttributeTableName(nonExistingEntityName, attributeTypeName),
		*connection
	));
	ASSERT_FALSE(IsTableExist(
		GetRules().GetValueTableName(nonExistingEntityName, attributeTypeName),
		*connection
	));

	ASSERT_FALSE(connection->RollbackTransaction()->HasError());
}


/// InsertOrUpdate не обновляет значение при невалидном виде сущности
TEST_F(ExecutorEAVWithEmptyEnvironment, InsertOrUpdateDoesNotUpdatesWithInvalidEntityName)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());

	const std::string existingEntityName = "SomeEntity1";
	const std::string nonExistingEntityName = "SomeEntity2";

	const SQLDataType attributeType = SQLDataType::Integer;
	const std::string attributeTypeName = converter->GetSQLVariable(SQLDataType::Integer)
		->GetTypeName();

	ASSERT_FALSE(executorEAV->SetRegisteredEntities({ {existingEntityName, {attributeType}} }, true)
		->HasError());

	int result = -1;
	ASSERT_FALSE(executorEAV->CreateNewEntity(existingEntityName, result)->HasError());
	ASSERT_EQ(result, 1);

	std::string attributeName = "SomeIntegerAttr";
	executorEAV->Insert(existingEntityName, result,
		converter->GetSQLTypeText(std::string(attributeName)), converter->GetSQLTypeInteger(5));

	ASSERT_TRUE(executorEAV->InsertOrUpdate(nonExistingEntityName, result,
		converter->GetSQLTypeText(std::string(attributeName)), converter->GetSQLTypeInteger(6))
		->HasError());

	// Проверим, что таблицы не создались
	ASSERT_FALSE(IsTableExist(
		GetRules().GetEntityTableName(nonExistingEntityName),
		*connection
	));
	ASSERT_FALSE(IsTableExist(
		GetRules().GetAttributeTableName(nonExistingEntityName, attributeTypeName),
		*connection
	));
	ASSERT_FALSE(IsTableExist(
		GetRules().GetValueTableName(nonExistingEntityName, attributeTypeName),
		*connection
	));

	ASSERT_FALSE(connection->RollbackTransaction()->HasError());
}


////////////////////////////////////////////////////////////////////////////////
/// Тесты Insert/Update/InsertOrUpdate, которые проверяют, что
/// команды возвращают ошибку, если их выполнить для невалидного идентификатора
/// сущности
////////////////////////////////////////////////////////////////////////////////

/// Insert не вставляет значение при невалидном идентификаторе сущности
TEST_F(ExecutorEAVWithEmptyEnvironment, InsertDoesNotInsertsWithInvalidEntityId)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());

	const std::string entityName = "SomeEntity1";

	const SQLDataType attributeType = SQLDataType::Integer;
	const std::string attributeTypeName = converter->GetSQLVariable(SQLDataType::Integer)
		->GetTypeName();

	ASSERT_FALSE(executorEAV->SetRegisteredEntities({ {entityName, {attributeType}} }, true)
		->HasError());

	int result = -1;
	ASSERT_FALSE(executorEAV->CreateNewEntity(entityName, result)->HasError());
	ASSERT_EQ(result, 1);

	std::string attributeName = "SomeIntegerAttr";
	ASSERT_TRUE(executorEAV->Insert(entityName, result + 1,
		converter->GetSQLTypeText(std::string(attributeName)), converter->GetSQLTypeInteger(5))
		->HasError());

	// Проверим, что таблицы не создались
	ASSERT_FALSE(IsTableExist(
		GetRules().GetAttributeTableName(entityName, attributeTypeName),
		*connection
	));
	ASSERT_FALSE(IsTableExist(
		GetRules().GetValueTableName(entityName, attributeTypeName),
		*connection
	));

	ASSERT_FALSE(connection->RollbackTransaction()->HasError());
}


/// InsertOrUpdate не вставляет значение при невалидном идентификаторе сущности
TEST_F(ExecutorEAVWithEmptyEnvironment, InsertOrUpdateDoesNotInsertsWithInvalidEntityId)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());

	const std::string entityName = "SomeEntity1";

	const SQLDataType attributeType = SQLDataType::Integer;
	const std::string attributeTypeName = converter->GetSQLVariable(SQLDataType::Integer)
		->GetTypeName();

	ASSERT_FALSE(executorEAV->SetRegisteredEntities({ {entityName, {attributeType}} }, true)
		->HasError());

	int result = -1;
	ASSERT_FALSE(executorEAV->CreateNewEntity(entityName, result)->HasError());
	ASSERT_EQ(result, 1);

	std::string attributeName = "SomeIntegerAttr";
	ASSERT_TRUE(executorEAV->InsertOrUpdate(entityName, result + 1,
		converter->GetSQLTypeText(std::string(attributeName)), converter->GetSQLTypeInteger(5))
		->HasError());

	// Проверим, что таблицы не создались
	ASSERT_FALSE(IsTableExist(
		GetRules().GetAttributeTableName(entityName, attributeTypeName),
		*connection
	));
	ASSERT_FALSE(IsTableExist(
		GetRules().GetValueTableName(entityName, attributeTypeName),
		*connection
	));

	ASSERT_FALSE(connection->RollbackTransaction()->HasError());
}


/// Update не обновляет значение при невалидном идентификаторе сущности
TEST_F(ExecutorEAVWithEmptyEnvironment, UpdateDoesNotUpdateWithInvalidEntityId)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());

	const std::string entityName = "SomeEntity1";

	const SQLDataType attributeType = SQLDataType::Integer;
	const std::string attributeTypeName = converter->GetSQLVariable(SQLDataType::Integer)
		->GetTypeName();

	ASSERT_FALSE(executorEAV->SetRegisteredEntities({ {entityName, {attributeType}} }, true)
		->HasError());

	int result = -1;
	ASSERT_FALSE(executorEAV->CreateNewEntity(entityName, result)->HasError());
	ASSERT_EQ(result, 1);

	std::string attributeName = "SomeIntegerAttr";
	// Добавим запись, чтобы было, что обновлять
	ASSERT_FALSE(executorEAV->Insert(entityName, result,
		converter->GetSQLTypeText(std::string(attributeName)), converter->GetSQLTypeInteger(5))
		->HasError());

	ASSERT_TRUE(executorEAV->Update(entityName, result + 1,
		converter->GetSQLTypeText(std::string(attributeName)), converter->GetSQLTypeInteger(6))
		->HasError());

	ASSERT_FALSE(connection->RollbackTransaction()->HasError());
}


/// InsertOrUpdate не обновляет значение при невалидном идентификаторе сущности
TEST_F(ExecutorEAVWithEmptyEnvironment, InsertOrUpdateDoesNotUpdateWithInvalidEntityId)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());

	const std::string entityName = "SomeEntity1";

	const SQLDataType attributeType = SQLDataType::Integer;
	const std::string attributeTypeName = converter->GetSQLVariable(SQLDataType::Integer)
		->GetTypeName();

	ASSERT_FALSE(executorEAV->SetRegisteredEntities({ {entityName, {attributeType}} }, true)
		->HasError());

	int result = -1;
	ASSERT_FALSE(executorEAV->CreateNewEntity(entityName, result)->HasError());
	ASSERT_EQ(result, 1);

	std::string attributeName = "SomeIntegerAttr";
	// Добавим запись, чтобы было, что обновлять
	ASSERT_FALSE(executorEAV->Insert(entityName, result,
		converter->GetSQLTypeText(std::string(attributeName)), converter->GetSQLTypeInteger(5))
		->HasError());

	ASSERT_TRUE(executorEAV->InsertOrUpdate(entityName, result + 1,
		converter->GetSQLTypeText(std::string(attributeName)), converter->GetSQLTypeInteger(6))
		->HasError());

	ASSERT_FALSE(connection->RollbackTransaction()->HasError());
}


////////////////////////////////////////////////////////////////////////////////
/// Дополнительные тесты Update
////////////////////////////////////////////////////////////////////////////////

/// Update не обновляет таблицы при несуществующем названии обновляемого атрибута
TEST_F(ExecutorEAVWithEmptyEnvironment, UpdateDoesNotUpdateWithNonExistingAttributeName)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());

	const std::string entityName = "SomeEntity1";

	const SQLDataType attributeType = SQLDataType::Integer;
	const std::string attributeTypeName = converter->GetSQLVariable(SQLDataType::Integer)
		->GetTypeName();

	ASSERT_FALSE(executorEAV->SetRegisteredEntities({ {entityName, {attributeType}} }, true)
		->HasError());

	int result = -1;
	ASSERT_FALSE(executorEAV->CreateNewEntity(entityName, result)->HasError());
	ASSERT_EQ(result, 1);

	std::string attributeName = "SomeIntegerAttr";
	// Добавим запись, чтобы было, что обновлять
	ASSERT_FALSE(executorEAV->Insert(entityName, result,
		converter->GetSQLTypeText(std::string(attributeName)), converter->GetSQLTypeInteger(5))
		->HasError());

	ASSERT_TRUE(executorEAV->Update(entityName, result,
		converter->GetSQLTypeText(std::string(attributeName + "Invalid")),
		converter->GetSQLTypeInteger(6))->HasError());

	ASSERT_FALSE(connection->RollbackTransaction()->HasError());
}


////////////////////////////////////////////////////////////////////////////////
/// Тесты Insert/Update/InsertOrUpdate, которые проверяют, что
/// команды возвращают ошибку, если передать нулевую или пустую переменную,
/// отвечающую за представление названия атрибута
////////////////////////////////////////////////////////////////////////////////

/// Insert возвращает ошибку, если попытаться вставить значение по невалидному названию атрибута
TEST_F(ExecutorEAVWithEmptyEnvironment, InsertDoesNotInsertsWithInvalidAttributeName)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());

	const std::string entityName = "SomeEntity1";

	const SQLDataType attributeType = SQLDataType::Integer;
	const std::string attributeTypeName = converter->GetSQLVariable(SQLDataType::Integer)
		->GetTypeName();

	ASSERT_FALSE(executorEAV->SetRegisteredEntities({ {entityName, {attributeType}} }, true)
		->HasError());

	int result = -1;
	ASSERT_FALSE(executorEAV->CreateNewEntity(entityName, result)->HasError());
	ASSERT_EQ(result, 1);

	std::string attributeName = "SomeIntegerAttr";
	ASSERT_TRUE(executorEAV->Insert(entityName, result, nullptr, converter->GetSQLTypeInteger(5))->HasError());
	ASSERT_TRUE(executorEAV->Insert(entityName, result, converter->GetSQLTypeText(), converter->GetSQLTypeInteger(5))->HasError());

	ASSERT_FALSE(connection->RollbackTransaction()->HasError());
}


/// InsertOrUpdate возвращает ошибку, если попытаться вставить значение по невалидному названию атрибута
TEST_F(ExecutorEAVWithEmptyEnvironment, InsertOrUpdateDoesNotInsertsWithInvalidAttributeName)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());

	const std::string entityName = "SomeEntity1";

	const SQLDataType attributeType = SQLDataType::Integer;
	const std::string attributeTypeName = converter->GetSQLVariable(SQLDataType::Integer)
		->GetTypeName();

	ASSERT_FALSE(executorEAV->SetRegisteredEntities({ {entityName, {attributeType}} }, true)
		->HasError());

	int result = -1;
	ASSERT_FALSE(executorEAV->CreateNewEntity(entityName, result)->HasError());
	ASSERT_EQ(result, 1);

	std::string attributeName = "SomeIntegerAttr";
	ASSERT_TRUE(executorEAV->InsertOrUpdate(entityName, result, nullptr, converter->GetSQLTypeInteger(5))->HasError());
	ASSERT_TRUE(executorEAV->InsertOrUpdate(entityName, result, converter->GetSQLTypeText(), converter->GetSQLTypeInteger(5))->HasError());

	ASSERT_FALSE(connection->RollbackTransaction()->HasError());
}


/// Update возвращает ошибку, если попытаться обновить значение по невалидному названию атрибута
TEST_F(ExecutorEAVWithEmptyEnvironment, UpdateDoesNotUpdateWithInvalidAttributeName)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());

	const std::string entityName = "SomeEntity1";

	const SQLDataType attributeType = SQLDataType::Integer;
	const std::string attributeTypeName = converter->GetSQLVariable(SQLDataType::Integer)
		->GetTypeName();

	ASSERT_FALSE(executorEAV->SetRegisteredEntities({ {entityName, {attributeType}} }, true)
		->HasError());

	int result = -1;
	ASSERT_FALSE(executorEAV->CreateNewEntity(entityName, result)->HasError());
	ASSERT_EQ(result, 1);

	std::string attributeName = "SomeIntegerAttr";
	// Добавим запись, чтобы было, что обновлять
	ASSERT_FALSE(executorEAV->Insert(entityName, result,
		converter->GetSQLTypeText(std::string(attributeName)), converter->GetSQLTypeInteger(5))
		->HasError());

	ASSERT_TRUE(executorEAV->Update(entityName, result, nullptr, converter->GetSQLTypeInteger(5))->HasError());
	ASSERT_TRUE(executorEAV->Update(entityName, result, converter->GetSQLTypeText(), converter->GetSQLTypeInteger(5))->HasError());

	ASSERT_FALSE(connection->RollbackTransaction()->HasError());
}


////////////////////////////////////////////////////////////////////////////////
/// Тесты Insert/Update/InsertOrUpdate, которые проверяют, что
/// команды возвращают ошибку, если передать нулевую или пустую переменную,
/// отвечающую за представление вставляемого/обновляемого значения
////////////////////////////////////////////////////////////////////////////////

/// Insert возвращает ошибку, если попытаться вставить невалидное значение
TEST_F(ExecutorEAVWithEmptyEnvironment, InsertDoesNotInsertWithInvalidValue)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());

	const std::string entityName = "SomeEntity1";

	const SQLDataType attributeType = SQLDataType::Integer;
	const std::string attributeTypeName = converter->GetSQLVariable(SQLDataType::Integer)
		->GetTypeName();

	ASSERT_FALSE(executorEAV->SetRegisteredEntities({ {entityName, {attributeType}} }, true)
		->HasError());

	int result = -1;
	ASSERT_FALSE(executorEAV->CreateNewEntity(entityName, result)->HasError());
	ASSERT_EQ(result, 1);

	std::string attributeName = "SomeIntegerAttr";
	ASSERT_TRUE(executorEAV->Insert(entityName, result,
		converter->GetSQLTypeText(std::string(attributeName)),
		nullptr)->HasError());
	ASSERT_TRUE(executorEAV->Insert(entityName, result,
		converter->GetSQLTypeText(std::string(attributeName)),
		converter->GetSQLTypeInteger())->HasError());
	
	ASSERT_FALSE(connection->RollbackTransaction()->HasError());
}


/// Update возвращает ошибку, если попытаться обновить значение невалидным значением
TEST_F(ExecutorEAVWithEmptyEnvironment, UpdateDoesNotUpdateWithInvalidValue)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());

	const std::string entityName = "SomeEntity1";

	const SQLDataType attributeType = SQLDataType::Integer;
	const std::string attributeTypeName = converter->GetSQLVariable(SQLDataType::Integer)
		->GetTypeName();

	ASSERT_FALSE(executorEAV->SetRegisteredEntities({ {entityName, {attributeType}} }, true)
		->HasError());

	int result = -1;
	ASSERT_FALSE(executorEAV->CreateNewEntity(entityName, result)->HasError());
	ASSERT_EQ(result, 1);

	std::string attributeName = "SomeIntegerAttr";
	// Добавим запись, чтобы было, что обновлять
	ASSERT_FALSE(executorEAV->Insert(entityName, result,
		converter->GetSQLTypeText(std::string(attributeName)), converter->GetSQLTypeInteger(5))
		->HasError());

	ASSERT_TRUE(executorEAV->Update(entityName, result,
		converter->GetSQLTypeText(std::string(attributeName)),
		nullptr)->HasError());
	ASSERT_TRUE(executorEAV->Update(entityName, result,
		converter->GetSQLTypeText(std::string(attributeName)),
		converter->GetSQLTypeInteger())->HasError());

	ASSERT_FALSE(connection->RollbackTransaction()->HasError());
}


/// InsertOrUpdate возвращает ошибку, если попытаться вставить невалидное значение
TEST_F(ExecutorEAVWithEmptyEnvironment, InsertOrUpdateDoesNotInsertWithInvalidValue)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());

	const std::string entityName = "SomeEntity1";

	const SQLDataType attributeType = SQLDataType::Integer;
	const std::string attributeTypeName = converter->GetSQLVariable(SQLDataType::Integer)
		->GetTypeName();

	ASSERT_FALSE(executorEAV->SetRegisteredEntities({ {entityName, {attributeType}} }, true)
		->HasError());

	int result = -1;
	ASSERT_FALSE(executorEAV->CreateNewEntity(entityName, result)->HasError());
	ASSERT_EQ(result, 1);

	std::string attributeName = "SomeIntegerAttr";
	ASSERT_TRUE(executorEAV->InsertOrUpdate(entityName, result,
		converter->GetSQLTypeText(std::string(attributeName)),
		nullptr)->HasError());
	ASSERT_TRUE(executorEAV->InsertOrUpdate(entityName, result,
		converter->GetSQLTypeText(std::string(attributeName)),
		converter->GetSQLTypeInteger())->HasError());

	ASSERT_FALSE(connection->RollbackTransaction()->HasError());
}


/// InsertOrUpdate возвращает ошибку, если попытаться обновить значение невалидным значением
TEST_F(ExecutorEAVWithEmptyEnvironment, InsertOrUpdateDoesNotUpdateWithInvalidValue)
{
	ASSERT_FALSE(connection->BeginTransaction()->HasError());

	const std::string entityName = "SomeEntity1";

	const SQLDataType attributeType = SQLDataType::Integer;
	const std::string attributeTypeName = converter->GetSQLVariable(SQLDataType::Integer)
		->GetTypeName();

	ASSERT_FALSE(executorEAV->SetRegisteredEntities({ {entityName, {attributeType}} }, true)
		->HasError());

	int result = -1;
	ASSERT_FALSE(executorEAV->CreateNewEntity(entityName, result)->HasError());
	ASSERT_EQ(result, 1);

	std::string attributeName = "SomeIntegerAttr";
	// Добавим запись, чтобы было, что обновлять
	ASSERT_FALSE(executorEAV->Insert(entityName, result,
		converter->GetSQLTypeText(std::string(attributeName)), converter->GetSQLTypeInteger(5))
		->HasError());

	ASSERT_TRUE(executorEAV->InsertOrUpdate(entityName, result,
		converter->GetSQLTypeText(std::string(attributeName)),
		nullptr)->HasError());
	ASSERT_TRUE(executorEAV->InsertOrUpdate(entityName, result,
		converter->GetSQLTypeText(std::string(attributeName)),
		converter->GetSQLTypeInteger())->HasError());

	ASSERT_FALSE(connection->RollbackTransaction()->HasError());
}


////////////////////////////////////////////////////////////////////////////////
/// Тесты Insert/Update/InsertOrUpdate, которые проверяют, что
/// данные методы не открывают и не закрывают транзакции
////////////////////////////////////////////////////////////////////////////////

/// Класс для проверок данной группы тестов
class ExecutorEAVForCheckTransactions : public ExecutorEAVWithEmptyEnvironment
{
protected:
	/// Создать простую таблицу
	void CreateSimpleTable(const std::string & tablename)
	{
		ASSERT_FALSE(
			connection->Execute(utils::string::Format(
				"CREATE TABLE {} (id INTEGER PRIMARY KEY);",
				tablename))
			->GetCurrentExecuteStatus()->HasError());
	}
};

/// Insert не открывает транзакцию
TEST_F(ExecutorEAVForCheckTransactions, InsertDoesNotBeginTransaction)
{
	const std::string entityName = "SomeEntity1";

	const SQLDataType attributeType = SQLDataType::Integer;
	const std::string attributeTypeName = converter->GetSQLVariable(SQLDataType::Integer)
		->GetTypeName();

	const IExecutorEAV::EAVRegisterEntries entries({ {entityName, {attributeType}} });
	ASSERT_FALSE(executorEAV->SetRegisteredEntities(entries, true)
		->HasError());

	int result = -1;
	ASSERT_FALSE(executorEAV->CreateNewEntity(entityName, result)->HasError());
	ASSERT_EQ(result, 1);

	std::string attributeName = "SomeIntegerAttr";

	// Зафиксируем активную транзакцию, если она вдруг есть
	ASSERT_FALSE(connection->CommitTransaction()->HasError());

	// Добавим запись
	ASSERT_FALSE(executorEAV->Insert(entityName, result,
		converter->GetSQLTypeText(std::string(attributeName)), converter->GetSQLTypeInteger(5))
		->HasError());

	const std::string newTableName = "SimpleTable";
	// Создадим пустую таблицу
	CreateSimpleTable(newTableName);
	// Откатим транзакцию
	ASSERT_FALSE(connection->RollbackTransaction()->HasError());

	// Если Insert открывает транзакцию, тогда таблица SimpleTable не создастся
	ASSERT_TRUE(IsTableExist(newTableName, *connection));

	// Удалим созданные таблицы
	ASSERT_TRUE(DropTable(newTableName, *connection));
	ASSERT_TRUE(DropAllTables({ entries }, *connection, GetRules(), *converter));
}


/// InsertOrUpdate не открывает транзакцию
TEST_F(ExecutorEAVForCheckTransactions, InsertOrUpdateDoesNotBeginTransaction)
{
	const std::string entityName = "SomeEntity1";

	const SQLDataType attributeType = SQLDataType::Integer;
	const std::string attributeTypeName = converter->GetSQLVariable(SQLDataType::Integer)
		->GetTypeName();

	const IExecutorEAV::EAVRegisterEntries entries({ {entityName, {attributeType}} });
	ASSERT_FALSE(executorEAV->SetRegisteredEntities(entries, true)
		->HasError());

	int result = -1;
	ASSERT_FALSE(executorEAV->CreateNewEntity(entityName, result)->HasError());
	ASSERT_EQ(result, 1);

	std::string attributeName = "SomeIntegerAttr";

	// Зафиксируем активную транзакцию, если она вдруг есть
	ASSERT_FALSE(connection->CommitTransaction()->HasError());

	// Добавим запись
	ASSERT_FALSE(executorEAV->InsertOrUpdate(entityName, result,
		converter->GetSQLTypeText(std::string(attributeName)), converter->GetSQLTypeInteger(5))
		->HasError());

	const std::string newTableName = "SimpleTable";
	// Создадим пустую таблицу
	CreateSimpleTable(newTableName);
	// Откатим транзакцию
	ASSERT_FALSE(connection->RollbackTransaction()->HasError());

	// Если InsertOrUpdate открывает транзакцию, тогда таблица SimpleTable не создастся
	ASSERT_TRUE(IsTableExist(newTableName, *connection));

	// Удалим созданные таблицы
	ASSERT_TRUE(DropTable(newTableName, *connection));
	ASSERT_TRUE(DropAllTables({ entries }, *connection, GetRules(), *converter));
}


/// Update не открывает транзакцию
TEST_F(ExecutorEAVForCheckTransactions, UpdateDoesNotBeginTransaction)
{
	const std::string entityName = "SomeEntity1";

	const SQLDataType attributeType = SQLDataType::Integer;
	const std::string attributeTypeName = converter->GetSQLVariable(SQLDataType::Integer)
		->GetTypeName();

	const IExecutorEAV::EAVRegisterEntries entries({ {entityName, {attributeType}} });
	ASSERT_FALSE(executorEAV->SetRegisteredEntities(entries, true)
		->HasError());

	int result = -1;
	ASSERT_FALSE(executorEAV->CreateNewEntity(entityName, result)->HasError());
	ASSERT_EQ(result, 1);

	std::string attributeName = "SomeIntegerAttr";

	// Добавим запись, чтобы было, что обновлять
	ASSERT_FALSE(executorEAV->Insert(entityName, result,
		converter->GetSQLTypeText(std::string(attributeName)), converter->GetSQLTypeInteger(5))
		->HasError());

	// Зафиксируем активную транзакцию, если она вдруг есть
	ASSERT_FALSE(connection->CommitTransaction()->HasError());

	// Обновим запись
	ASSERT_FALSE(executorEAV->Update(entityName, result,
		converter->GetSQLTypeText(std::string(attributeName)), converter->GetSQLTypeInteger(7))
		->HasError());

	const std::string newTableName = "SimpleTable";
	// Создадим пустую таблицу
	CreateSimpleTable(newTableName);
	// Откатим транзакцию
	ASSERT_FALSE(connection->RollbackTransaction()->HasError());

	// Если Update открывает транзакцию, тогда таблица SimpleTable не создастся
	ASSERT_TRUE(IsTableExist(newTableName, *connection));

	// Удалим созданные таблицы
	ASSERT_TRUE(DropTable(newTableName, *connection));
	ASSERT_TRUE(DropAllTables({ entries }, *connection, GetRules(), *converter));
}


/// Insert не фиксирует транзакцию
TEST_F(ExecutorEAVForCheckTransactions, InsertDoesNotEndTransaction)
{
	const std::string entityName = "SomeEntity1";

	const SQLDataType attributeType = SQLDataType::Integer;
	const std::string attributeTypeName = converter->GetSQLVariable(SQLDataType::Integer)
		->GetTypeName();

	const IExecutorEAV::EAVRegisterEntries entries({ {entityName, {attributeType}} });
	ASSERT_FALSE(executorEAV->SetRegisteredEntities(entries, true)
		->HasError());

	int result = -1;
	ASSERT_FALSE(executorEAV->CreateNewEntity(entityName, result)->HasError());
	ASSERT_EQ(result, 1);

	std::string attributeName = "SomeIntegerAttr";

	// Зафиксируем активную транзакцию, если она вдруг есть
	ASSERT_FALSE(connection->CommitTransaction()->HasError());

	// Начнем новую транзакцию
	ASSERT_FALSE(connection->BeginTransaction()->HasError());

	// Создадим пустую таблицу
	const std::string newTableName = "SimpleTable";
	CreateSimpleTable(newTableName);

	// Добавим запись
	ASSERT_FALSE(executorEAV->Insert(entityName, result,
		converter->GetSQLTypeText(std::string(attributeName)), converter->GetSQLTypeInteger(5))
		->HasError());

	// Проверим, что таблица ещё существует (что метод не производит откат транзакции)
	ASSERT_TRUE(IsTableExist(newTableName, *connection));

	// Откатим транзакцию
	ASSERT_FALSE(connection->RollbackTransaction()->HasError());

	// Проверим, что таблица теперь не существует (что метод не производит фиксацию транзакции)
	ASSERT_FALSE(IsTableExist(newTableName, *connection));

	// Удалим созданные таблицы
	ASSERT_TRUE(DropAllTables({ entries }, *connection, GetRules(), *converter));
}


/// InsertOrUpdate не фиксирует транзакцию
TEST_F(ExecutorEAVForCheckTransactions, InsertOrUpdateDoesNotEndTransaction)
{
	const std::string entityName = "SomeEntity1";

	const SQLDataType attributeType = SQLDataType::Integer;
	const std::string attributeTypeName = converter->GetSQLVariable(SQLDataType::Integer)
		->GetTypeName();

	const IExecutorEAV::EAVRegisterEntries entries({ {entityName, {attributeType}} });
	ASSERT_FALSE(executorEAV->SetRegisteredEntities(entries, true)
		->HasError());

	int result = -1;
	ASSERT_FALSE(executorEAV->CreateNewEntity(entityName, result)->HasError());
	ASSERT_EQ(result, 1);

	std::string attributeName = "SomeIntegerAttr";

	// Зафиксируем активную транзакцию, если она вдруг есть
	ASSERT_FALSE(connection->CommitTransaction()->HasError());

	// Начнем новую транзакцию
	ASSERT_FALSE(connection->BeginTransaction()->HasError());

	// Создадим пустую таблицу
	const std::string newTableName = "SimpleTable";
	CreateSimpleTable(newTableName);

	// Добавим запись
	ASSERT_FALSE(executorEAV->InsertOrUpdate(entityName, result,
		converter->GetSQLTypeText(std::string(attributeName)), converter->GetSQLTypeInteger(5))
		->HasError());

	// Проверим, что таблица ещё существует (что метод не производит откат транзакции)
	ASSERT_TRUE(IsTableExist(newTableName, *connection));

	// Откатим транзакцию
	ASSERT_FALSE(connection->RollbackTransaction()->HasError());

	// Проверим, что таблица теперь не существует (что метод не производит фиксацию транзакции)
	ASSERT_FALSE(IsTableExist(newTableName, *connection));

	// Удалим созданные таблицы
	ASSERT_TRUE(DropAllTables({ entries }, *connection, GetRules(), *converter));
}


/// Update не фиксирует транзакцию
TEST_F(ExecutorEAVForCheckTransactions, UpdateDoesNotEndTransaction)
{
	const std::string entityName = "SomeEntity1";

	const SQLDataType attributeType = SQLDataType::Integer;
	const std::string attributeTypeName = converter->GetSQLVariable(SQLDataType::Integer)
		->GetTypeName();

	const IExecutorEAV::EAVRegisterEntries entries({ {entityName, {attributeType}} });
	ASSERT_FALSE(executorEAV->SetRegisteredEntities(entries, true)
		->HasError());

	int result = -1;
	ASSERT_FALSE(executorEAV->CreateNewEntity(entityName, result)->HasError());
	ASSERT_EQ(result, 1);

	std::string attributeName = "SomeIntegerAttr";

	// Зафиксируем активную транзакцию, если она вдруг есть
	ASSERT_FALSE(connection->CommitTransaction()->HasError());

	// Добавим запись, чтобы было, что обновлять
	ASSERT_FALSE(executorEAV->Insert(entityName, result,
		converter->GetSQLTypeText(std::string(attributeName)), converter->GetSQLTypeInteger(5))
		->HasError());

	// Начнем новую транзакцию
	ASSERT_FALSE(connection->BeginTransaction()->HasError());

	// Обновим запись
	ASSERT_FALSE(executorEAV->Update(entityName, result,
		converter->GetSQLTypeText(std::string(attributeName)), converter->GetSQLTypeInteger(7))
		->HasError());

	// Создадим пустую таблицу
	const std::string newTableName = "SimpleTable";
	CreateSimpleTable(newTableName);

	// Проверим, что таблица ещё существует (что метод не производит откат транзакции)
	ASSERT_TRUE(IsTableExist(newTableName, *connection));

	// Откатим транзакцию
	ASSERT_FALSE(connection->RollbackTransaction()->HasError());

	// Проверим, что таблица теперь не существует (что метод не производит фиксацию транзакции)
	ASSERT_FALSE(IsTableExist(newTableName, *connection));

	// Удалим созданные таблицы
	ASSERT_TRUE(DropAllTables({ entries }, *connection, GetRules(), *converter));
}


////////////////////////////////////////////////////////////////////////////////
/// Тесты для GetEntityIds, GetAttributeNames, FindEntitiesByAttrValues,
/// GetValue, GetAttributeValues
////////////////////////////////////////////////////////////////////////////////

/// Класс для проверок с заполненным окружением
class ExecutorEAVWithFilledEnvironment : public ExecutorEAVWithEmptyEnvironment
{
protected:
	IExecutorEAV::EAVRegisterEntries entries;
	std::vector<std::string> createdFileNames;

private:
	/// ASSERT_ нельзя вызывать в функциях, которые не возвращают void
	void MyAssert(bool value)
	{
		ASSERT_TRUE(value);
	}

	/// Создает файл с данными и возвращает его идентификатор
	std::string CreateFileWithData(const std::vector<char> & data)
	{
		auto remoteFilePtr = connection->CreateRemoteFile();
		auto && filename = remoteFilePtr->GetFileName();
		createdFileNames.push_back(filename);

		bool res = remoteFilePtr->Open(FileOpenMode::Write);
		res = res && remoteFilePtr->WriteBytes(data);
		res = res && remoteFilePtr->Close();

		MyAssert(res);

		return filename;
	}

protected:
	// Действия в начале теста
	virtual void SetUp() override
	{
		ExecutorEAVWithEmptyEnvironment::SetUp();

		ASSERT_FALSE(connection->BeginTransaction()->HasError());

		entries = IExecutorEAV::EAVRegisterEntries({
			{"users", {SQLDataType::Text}},
			{"products", {SQLDataType::Integer, SQLDataType::Text}},
			{"blobs", {SQLDataType::Text, SQLDataType::RemoteFileId}},
			{"images", {SQLDataType::Text, SQLDataType::ByteArray}},
			{"todo", {SQLDataType::Text}}
		});
		ASSERT_FALSE(executorEAV->SetRegisteredEntities(entries, true)->HasError());
		ASSERT_EQ(executorEAV->GetRegisteredEntities(), entries);

		// Создадим сущности
		// Один пользователь
		for (int i = 1; i <= 1; ++i)
		{
			int temp = -1;
			ASSERT_FALSE(executorEAV->CreateNewEntity("users", temp)->HasError());
			ASSERT_EQ(temp, 1);
		}
		// Три продукта
		for (int i = 1; i <= 3; ++i)
		{
			int temp = -1;
			ASSERT_FALSE(executorEAV->CreateNewEntity("products", temp)->HasError());
			ASSERT_EQ(temp, i);
		}
		// Три больших бинарных объекта
		for (int i = 1; i <= 3; ++i)
		{
			int temp = -1;
			ASSERT_FALSE(executorEAV->CreateNewEntity("blobs", temp)->HasError());
			ASSERT_EQ(temp, i);
		}
		// Четыре картинки
		for (int i = 1; i <= 4; ++i)
		{
			int temp = -1;
			ASSERT_FALSE(executorEAV->CreateNewEntity("images", temp)->HasError());
			ASSERT_EQ(temp, i);
		}
		// Ноль элементов в todo

		const auto nameAttr = converter->GetSQLTypeText("Name");
		ASSERT_FALSE(executorEAV->Insert("users", 1, nameAttr, converter->GetSQLTypeText("Ivan"))->HasError());

		const auto priceAttr = converter->GetSQLTypeText("Price");
		const auto descAttr = converter->GetSQLTypeText("Description");
		ASSERT_FALSE(executorEAV->Insert("products", 1, priceAttr, converter->GetSQLTypeInteger(1000))->HasError());
		ASSERT_FALSE(executorEAV->Insert("products", 2, priceAttr, converter->GetSQLTypeInteger(50))->HasError());
		ASSERT_FALSE(executorEAV->Insert("products", 3, priceAttr, converter->GetSQLTypeInteger(777))->HasError());
		ASSERT_FALSE(executorEAV->Insert("products", 1, nameAttr, converter->GetSQLTypeText("House"))->HasError());
		ASSERT_FALSE(executorEAV->Insert("products", 2, nameAttr, converter->GetSQLTypeText("Icecream"))->HasError());
		ASSERT_FALSE(executorEAV->Insert("products", 1, descAttr, converter->GetSQLTypeText("Amazing house"))->HasError());
		ASSERT_FALSE(executorEAV->Insert("products", 2, descAttr, converter->GetSQLTypeText("Tasty and cold"))->HasError());

		const auto typeAttr = converter->GetSQLTypeText("Type");
		const auto idAttr = converter->GetSQLTypeText("Id");
		ASSERT_FALSE(executorEAV->Insert("blobs", 1, typeAttr, converter->GetSQLTypeText("zip"))->HasError());
		ASSERT_FALSE(executorEAV->Insert("blobs", 2, typeAttr, converter->GetSQLTypeText("zip"))->HasError());
		ASSERT_FALSE(executorEAV->Insert("blobs", 3, typeAttr, converter->GetSQLTypeText("tar"))->HasError());
		ASSERT_FALSE(executorEAV->Insert("blobs", 1, idAttr, converter->GetSQLTypeRemoteFileId(CreateFileWithData({1, 2, 3, 4, 5})))->HasError());
		ASSERT_FALSE(executorEAV->Insert("blobs", 2, idAttr, converter->GetSQLTypeRemoteFileId(CreateFileWithData({ 3, 2, 1})))->HasError());
		ASSERT_FALSE(executorEAV->Insert("blobs", 3, idAttr, converter->GetSQLTypeRemoteFileId(CreateFileWithData({ })))->HasError());

		const auto dataAttr = converter->GetSQLTypeText("Data");
		ASSERT_FALSE(executorEAV->Insert("images", 1, nameAttr, converter->GetSQLTypeText("Sun"))->HasError());
		ASSERT_FALSE(executorEAV->Insert("images", 2, nameAttr, converter->GetSQLTypeText("Tree"))->HasError());
		ASSERT_FALSE(executorEAV->Insert("images", 3, nameAttr, converter->GetSQLTypeText("OtherTree"))->HasError());
		ASSERT_FALSE(executorEAV->Insert("images", 1, dataAttr, converter->GetSQLTypeByteArray({1, 1, 1}))->HasError());
		ASSERT_FALSE(executorEAV->Insert("images", 2, dataAttr, converter->GetSQLTypeByteArray({ 2, 2, 3, 2 }))->HasError());
		ASSERT_FALSE(executorEAV->Insert("images", 3, dataAttr, converter->GetSQLTypeByteArray({ 2, 2, 3, 2 }))->HasError());

		ASSERT_FALSE(connection->CommitTransaction()->HasError());
	}

	// Действия в конце теста
	virtual void TearDown() override
	{
		for (auto && createdFileName : createdFileNames)
			ASSERT_TRUE(connection->DeleteRemoteFile(createdFileName));
		ASSERT_TRUE(DropAllTables({ entries }, *connection, GetRules(), *converter));

		ExecutorEAVWithEmptyEnvironment::TearDown();
	}
};


/// GetEntityIds работает для существующих сущностей
TEST_F(ExecutorEAVWithFilledEnvironment, GetEntityIdsWorksForExistingEntity)
{
	std::vector<IExecutorEAV::EntityId> result;

	// будем заполнять вектор мусором, чтобы дополнительно проверять, что метод его чистит
	result = { 7, 4, 5, 3 };
	EXPECT_FALSE(executorEAV->GetEntityIds("users", result)->HasError());
	EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({ 1 }));

	result = { 7, 4, 5, 3 };
	EXPECT_FALSE(executorEAV->GetEntityIds("products", result)->HasError());
	EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({ 1, 2, 3 }));

	result = { 7, 4, 5, 3 };
	EXPECT_FALSE(executorEAV->GetEntityIds("blobs", result)->HasError());
	EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({ 1, 2, 3 }));

	result = { 7, 4, 5, 3 };
	EXPECT_FALSE(executorEAV->GetEntityIds("images", result)->HasError());
	EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({ 1, 2, 3, 4 }));

	result = { 7, 4, 5, 3 };
	EXPECT_FALSE(executorEAV->GetEntityIds("todo", result)->HasError());
	EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({}));
}


/// GetEntityIds не работает для не существующих сущностей
TEST_F(ExecutorEAVWithFilledEnvironment, GetEntityIdsDoesNotWorkForNonExistingEntity)
{
	std::vector<IExecutorEAV::EntityId> result;

	// будем заполнять вектор мусором, чтобы дополнительно проверять, что метод его не чистит
	result = { 7, 4, 5, 3 };
	EXPECT_TRUE(executorEAV->GetEntityIds("birds", result)->HasError());
	EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({ 7, 4, 5, 3 }));
}


/// GetAttributeNames работает с существующими сущностями и атрибутами
TEST_F(ExecutorEAVWithFilledEnvironment,
	GetAttributeNamesWorksForExistingEntityAndExistingAttributes)
{
	std::vector<ISQLTypeTextPtr> result;

	// допишем nullptr, чтобы проверить, что вектор прочищается
	result.push_back(nullptr);
	EXPECT_FALSE(executorEAV->GetAttributeNames("users", SQLDataType::Text, result)->HasError());
	EXPECT_EQ(result.size(), 1);
	EXPECT_EQ(result[0]->GetValue(), "Name");

	result.clear();
	EXPECT_FALSE(executorEAV->GetAttributeNames("products", SQLDataType::Text, result)->HasError());
	EXPECT_EQ(result.size(), 2);
	EXPECT_EQ(result[0]->GetValue(), "Name");
	EXPECT_EQ(result[1]->GetValue(), "Description");
	EXPECT_FALSE(executorEAV->GetAttributeNames("products", SQLDataType::Integer, result)->HasError());
	EXPECT_EQ(result.size(), 1);
	EXPECT_EQ(result[0]->GetValue(), "Price");

	EXPECT_FALSE(executorEAV->GetAttributeNames("blobs", SQLDataType::Text, result)->HasError());
	EXPECT_EQ(result.size(), 1);
	EXPECT_EQ(result[0]->GetValue(), "Type");
	EXPECT_FALSE(executorEAV->GetAttributeNames("blobs", SQLDataType::RemoteFileId, result)->HasError());
	EXPECT_EQ(result.size(), 1);
	EXPECT_EQ(result[0]->GetValue(), "Id");

	EXPECT_FALSE(executorEAV->GetAttributeNames("images", SQLDataType::Text, result)->HasError());
	EXPECT_EQ(result.size(), 1);
	EXPECT_EQ(result[0]->GetValue(), "Name");
	EXPECT_FALSE(executorEAV->GetAttributeNames("images", SQLDataType::ByteArray, result)->HasError());
	EXPECT_EQ(result.size(), 1);
	EXPECT_EQ(result[0]->GetValue(), "Data");
}


/// GetAttributeNames не работает с существующими сущностями и несуществующими атрибутами
TEST_F(ExecutorEAVWithFilledEnvironment,
	GetAttributeNamesDoesNotWorkForExistingEntityAndNonExistingAttributes)
{
	std::vector<ISQLTypeTextPtr> result;

	// допишем nullptr, чтобы проверить, что вектор не прочищается
	result.push_back(nullptr);
	result.push_back(nullptr);
	result.push_back(nullptr);
	// У users нет атрибута SQLDataType::ByteArray
	EXPECT_TRUE(executorEAV->GetAttributeNames("users", SQLDataType::ByteArray, result)->HasError());
	EXPECT_EQ(result.size(), 3);
	EXPECT_EQ(result[0], nullptr);
	EXPECT_EQ(result[1], nullptr);
	EXPECT_EQ(result[2], nullptr);
}


/// GetAttributeNames не работает с несуществующими сущностями
TEST_F(ExecutorEAVWithFilledEnvironment,
	GetAttributeNamesDoesNotWorkForNonExistingEntity)
{
	std::vector<ISQLTypeTextPtr> result;

	// допишем nullptr, чтобы проверить, что вектор не прочищается
	result.push_back(nullptr);
	result.push_back(nullptr);
	result.push_back(nullptr);
	// Не существует сущности "birds"
	EXPECT_TRUE(executorEAV->GetAttributeNames("birds", SQLDataType::Integer, result)->HasError());
	EXPECT_EQ(result.size(), 3);
	EXPECT_EQ(result[0], nullptr);
	EXPECT_EQ(result[1], nullptr);
	EXPECT_EQ(result[2], nullptr);
}


/// FindEntitiesByAttrValues ищет существующие записи по одной паре атрибут-значение
TEST_F(ExecutorEAVWithFilledEnvironment, FindEntitiesByAttrValuesFindsExistingEntriesByAttrValue)
{
	std::vector<IExecutorEAV::EntityId> result;

	// I. Поиск одиночных записей
	//    1. С атрибутом Text

	// будем заполнять вектор мусором, чтобы дополнительно проверять, что метод его не чистит
	result = { 7, 4, 5, 3 };
	EXPECT_FALSE(executorEAV->FindEntitiesByAttrValues("users", std::vector<IExecutorEAV::AttrValue>({
			{converter->GetSQLTypeText("Name"), converter->GetSQLTypeText("Ivan")} }),
			result)->HasError());
	EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({ 1 }));

	//    2. С атрибутом Integer
	result = { 7, 4, 5, 3 };
	EXPECT_FALSE(executorEAV->FindEntitiesByAttrValues("products", std::vector<IExecutorEAV::AttrValue>({
			{converter->GetSQLTypeText("Price"), converter->GetSQLTypeInteger(777)} }),
			result)->HasError());
	EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({ 3 }));

	//    3. С атрибутом RemoteFileId
	result = { 7, 4, 5, 3 };
	EXPECT_FALSE(executorEAV->FindEntitiesByAttrValues("blobs", std::vector<IExecutorEAV::AttrValue>({
			{converter->GetSQLTypeText("Id"), converter->GetSQLTypeRemoteFileId(createdFileNames.at(1))} }),
			result)->HasError());
	EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({ 2 }));

	//    4. С атрибутом ByteArray
	result = { 7, 4, 5, 3 };
	EXPECT_FALSE(executorEAV->FindEntitiesByAttrValues("images", std::vector<IExecutorEAV::AttrValue>({
			{converter->GetSQLTypeText("Data"), converter->GetSQLTypeByteArray({1, 1, 1})} }),
			result)->HasError());
	EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({ 1 }));

	//    5. С пустым значением атрибута
	result = { 7, 4, 5, 3 };
	EXPECT_FALSE(executorEAV->FindEntitiesByAttrValues("images", std::vector<IExecutorEAV::AttrValue>({
		{converter->GetSQLTypeText("Name"), converter->GetSQLTypeText()} }),
		result)->HasError());
	EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({ 4 }));

	// II. Поиск нескольких записей с одинаковым значением одного атрибута
	result = { 7, 4, 5, 3 };
	EXPECT_FALSE(executorEAV->FindEntitiesByAttrValues("images", std::vector<IExecutorEAV::AttrValue>({
			{converter->GetSQLTypeText("Data"), converter->GetSQLTypeByteArray({ 2, 2, 3, 2 })} }),
			result)->HasError());
	EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({ 2, 3 }));

	EXPECT_FALSE(executorEAV->FindEntitiesByAttrValues("blobs", std::vector<IExecutorEAV::AttrValue>({
			{converter->GetSQLTypeText("Type"), converter->GetSQLTypeText("zip")} }),
			result)->HasError());
	EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({ 1, 2 }));
}


/// FindEntitiesByAttrValues ищет существующие записи по нескольких парам атрибут-значение
TEST_F(ExecutorEAVWithFilledEnvironment, FindEntitiesByAttrValuesFindsExistingEntriesByAttrValues)
{
	// Тестируем только две пары атрибут-значение
	// Первая пара атрибут-значение содержится в множестве сущностей A.
	// Вторая пара атрибут-значение содержится в множестве сущностей B.
	// Для двух пар атрибут-значение метод возвращает A ∩ B.

	std::vector<IExecutorEAV::EntityId> result;

	{
		// 1. A = ∅, B = ∅.

		auto nameAttr = converter->GetSQLTypeText("Name");
		auto nameValue = converter->GetSQLTypeText("Cat");
		auto dataAttr = converter->GetSQLTypeText("Data");
		auto dataValue = converter->GetSQLTypeByteArray({ 0, 0, 0 });

		// будем заполнять вектор мусором, чтобы дополнительно проверять, что метод его не чистит
		result = { 7, 4, 5, 3 };
		EXPECT_FALSE(executorEAV->FindEntitiesByAttrValues("images", std::vector<IExecutorEAV::AttrValue>({
				{nameAttr, nameValue} }),
				result)->HasError());
		// Проверка A = ∅
		EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({}));

		result = { 7, 4, 5, 3 };
		EXPECT_FALSE(executorEAV->FindEntitiesByAttrValues("images", std::vector<IExecutorEAV::AttrValue>({
				{dataAttr, dataValue} }),
				result)->HasError());
		// Проверка B = ∅
		EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({}));

		result = { 7, 4, 5, 3 };
		EXPECT_FALSE(executorEAV->FindEntitiesByAttrValues("images", std::vector<IExecutorEAV::AttrValue>({
				{nameAttr, nameValue},
				{dataAttr, dataValue} }),
				result)->HasError());
		// Проверка A ∩ B = ∅
		EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({}));
	}

	{
		// 2. A = ∅, B ≠ ∅

		auto nameAttr = converter->GetSQLTypeText("Name");
		auto nameValue = converter->GetSQLTypeText("Cat");
		auto dataAttr = converter->GetSQLTypeText("Data");
		auto dataValue = converter->GetSQLTypeByteArray({ 2, 2, 3, 2 });

		// будем заполнять вектор мусором, чтобы дополнительно проверять, что метод его не чистит
		result = { 7, 4, 5, 3 };
		EXPECT_FALSE(executorEAV->FindEntitiesByAttrValues("images", std::vector<IExecutorEAV::AttrValue>({
				{nameAttr, nameValue} }),
				result)->HasError());
		// Проверка A = ∅
		EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({}));

		result = { 7, 4, 5, 3 };
		EXPECT_FALSE(executorEAV->FindEntitiesByAttrValues("images", std::vector<IExecutorEAV::AttrValue>({
				{dataAttr, dataValue} }),
				result)->HasError());
		// Проверка B ≠ ∅
		EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({2, 3}));

		result = { 7, 4, 5, 3 };
		EXPECT_FALSE(executorEAV->FindEntitiesByAttrValues("images", std::vector<IExecutorEAV::AttrValue>({
				{nameAttr, nameValue},
				{dataAttr, dataValue} }),
				result)->HasError());
		// Проверка A ∩ B = ∅
		EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({}));
	}

	{
		// 3. A ≠ ∅, B ≠ ∅, A ∩ B = ∅

		auto nameAttr = converter->GetSQLTypeText("Name");
		auto nameValue = converter->GetSQLTypeText("Sun");
		auto dataAttr = converter->GetSQLTypeText("Data");
		auto dataValue = converter->GetSQLTypeByteArray({ 2, 2, 3, 2 });

		// будем заполнять вектор мусором, чтобы дополнительно проверять, что метод его не чистит
		result = { 7, 4, 5, 3 };
		EXPECT_FALSE(executorEAV->FindEntitiesByAttrValues("images", std::vector<IExecutorEAV::AttrValue>({
				{nameAttr, nameValue} }),
				result)->HasError());
		// Проверка A ≠ ∅
		EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({ 1 }));

		result = { 7, 4, 5, 3 };
		EXPECT_FALSE(executorEAV->FindEntitiesByAttrValues("images", std::vector<IExecutorEAV::AttrValue>({
				{dataAttr, dataValue} }),
				result)->HasError());
		// Проверка B ≠ ∅
		EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({ 2, 3 }));

		result = { 7, 4, 5, 3 };
		EXPECT_FALSE(executorEAV->FindEntitiesByAttrValues("images", std::vector<IExecutorEAV::AttrValue>({
				{nameAttr, nameValue},
				{dataAttr, dataValue} }),
				result)->HasError());
		// Проверка A ∩ B = ∅
		EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({}));
	}

	{
		// 4. A ≠ ∅, B ≠ ∅, A ∩ B ≠ ∅

		auto nameAttr = converter->GetSQLTypeText("Name");
		auto nameValue = converter->GetSQLTypeText("Tree");
		auto dataAttr = converter->GetSQLTypeText("Data");
		auto dataValue = converter->GetSQLTypeByteArray({ 2, 2, 3, 2 });

		// будем заполнять вектор мусором, чтобы дополнительно проверять, что метод его не чистит
		result = { 7, 4, 5, 3 };
		EXPECT_FALSE(executorEAV->FindEntitiesByAttrValues("images", std::vector<IExecutorEAV::AttrValue>({
				{nameAttr, nameValue} }),
				result)->HasError());
		// Проверка A ≠ ∅
		EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({ 2 }));

		result = { 7, 4, 5, 3 };
		EXPECT_FALSE(executorEAV->FindEntitiesByAttrValues("images", std::vector<IExecutorEAV::AttrValue>({
				{dataAttr, dataValue} }),
				result)->HasError());
		// Проверка B ≠ ∅
		EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({ 2, 3 }));

		result = { 7, 4, 5, 3 };
		EXPECT_FALSE(executorEAV->FindEntitiesByAttrValues("images", std::vector<IExecutorEAV::AttrValue>({
				{nameAttr, nameValue},
				{dataAttr, dataValue} }),
				result)->HasError());
		// Проверка A ∩ B ≠ ∅
		EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({ 2 }));
	}
}


/// FindEntitiesByAttrValues возвращает ошибку при невалидных аргументах
TEST_F(ExecutorEAVWithFilledEnvironment, FindEntitiesByAttrValuesDoesNotFindWithInvalidArgs)
{
	std::vector<IExecutorEAV::EntityId> result;

	// Подготовка. Сначала проверим, что поиск вообще работает.

	// будем заполнять вектор мусором, чтобы дополнительно проверять, что метод его не чистит
	result = { 7, 4, 5, 3 };
	EXPECT_FALSE(executorEAV->FindEntitiesByAttrValues("users", std::vector<IExecutorEAV::AttrValue>({
			{converter->GetSQLTypeText("Name"), converter->GetSQLTypeText("Ivan")} }),
			result)->HasError());
	EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({ 1 }));

	// I. Невалидное название сущности.

	//    1. Несуществующее название сущности
	result = { 7, 4, 5, 3 };
	auto status = executorEAV->FindEntitiesByAttrValues("birds", std::vector<IExecutorEAV::AttrValue>({
		{converter->GetSQLTypeText("Name"), converter->GetSQLTypeText("Ivan")} }),
		result);
	EXPECT_EQ(status->GetStatus(), ResultStatus::FatalError);
	EXPECT_TRUE(status->HasError());
	EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({ 7, 4, 5, 3 }));

	//    2. Пустое название сущности
	result = { 7, 4, 5, 3 };
	status = executorEAV->FindEntitiesByAttrValues("", std::vector<IExecutorEAV::AttrValue>({
		{converter->GetSQLTypeText("Name"), converter->GetSQLTypeText("Ivan")} }),
		result);
	EXPECT_EQ(status->GetStatus(), ResultStatus::FatalError);
	EXPECT_TRUE(status->HasError());
	EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({ 7, 4, 5, 3 }));

	// II. Невалидное название атрибута.

	//    1. Несуществующее название атрибута
	//       Думаю, несуществующее название атрибута - не повод возвращать ошибку.
	//       Хотя это и возможно реализовать.
	result = { 7, 4, 5, 3 };
	status = executorEAV->FindEntitiesByAttrValues("users", std::vector<IExecutorEAV::AttrValue>({
		{converter->GetSQLTypeText("Address"), converter->GetSQLTypeText("Ivan")} }),
		result);
	EXPECT_EQ(status->GetStatus(), ResultStatus::OkWithData);
	EXPECT_FALSE(status->HasError());
	EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({}));

	//    2. Пустое название атрибута
	result = { 7, 4, 5, 3 };
	status = executorEAV->FindEntitiesByAttrValues("users", std::vector<IExecutorEAV::AttrValue>({
		{converter->GetSQLTypeText(), converter->GetSQLTypeText("Ivan")} }),
		result);
	EXPECT_EQ(status->GetStatus(), ResultStatus::EmptyQuery);
	EXPECT_TRUE(status->HasError());
	EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({ 7, 4, 5, 3 }));

	//    3. Нулевое название атрибута
	result = { 7, 4, 5, 3 };
	status = executorEAV->FindEntitiesByAttrValues("users", std::vector<IExecutorEAV::AttrValue>({
		{ nullptr, converter->GetSQLTypeText("Ivan")} }),
		result);
	EXPECT_EQ(status->GetStatus(), ResultStatus::EmptyQuery);
	EXPECT_TRUE(status->HasError());
	EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({ 7, 4, 5, 3 }));

	// III. Невалидное значение атрибута.

	//    1. Пустое значение атрибута
	//       Это не ошибка.

	//    2. Нулевое значение атрибута
	result = { 7, 4, 5, 3 };
	status = executorEAV->FindEntitiesByAttrValues("users", std::vector<IExecutorEAV::AttrValue>({
		{converter->GetSQLTypeText("Name"), nullptr} }),
		result);
	EXPECT_EQ(status->GetStatus(), ResultStatus::EmptyQuery);
	EXPECT_TRUE(status->HasError());
	EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({ 7, 4, 5, 3 }));

	//    3. Значение атрибута принадлежит несуществующиму типу данных
	result = { 7, 4, 5, 3 };
	status = executorEAV->FindEntitiesByAttrValues("users", std::vector<IExecutorEAV::AttrValue>({
		{converter->GetSQLTypeText("Name"), converter->GetSQLTypeInteger(5)} }),
		result);
	EXPECT_EQ(status->GetStatus(), ResultStatus::FatalError);
	EXPECT_TRUE(status->HasError());
	EXPECT_EQ(result, std::vector<IExecutorEAV::EntityId>({ 7, 4, 5, 3 }));
}


/// GetValue дает значение для существующих сущностей c существующими атрибутами
TEST_F(ExecutorEAVWithFilledEnvironment, GetValueGetsForExistingEntities)
{
	{
		// 1. Можем получить текст
		ISQLTypeTextPtr textValue = converter->GetSQLTypeText();
		EXPECT_FALSE(executorEAV->GetValue("users", 1, converter->GetSQLTypeText("Name"), textValue)
			->HasError());
		EXPECT_EQ(textValue->GetValue(), "Ivan");
	}
	{
		// 2. Можем получить число
		ISQLTypeIntegerPtr intValue = converter->GetSQLTypeInteger();
		EXPECT_FALSE(executorEAV->GetValue("products", 2, converter->GetSQLTypeText("Price"),
			intValue)->HasError());
		EXPECT_EQ(intValue->GetValue(), 50);
	}
	{
		// 3. Можем получить массив байт
		ISQLTypeByteArrayPtr byteArrayValue = converter->GetSQLTypeByteArray();
		EXPECT_FALSE(executorEAV->GetValue("images", 3, converter->GetSQLTypeText("Data"),
			byteArrayValue)->HasError());
		EXPECT_EQ(byteArrayValue->GetValue(), std::vector<char>({ 2, 2, 3, 2 }));
	}
	{
		// 4. Можем получить идентификатор файла
		ISQLTypeRemoteFileIdPtr remoteFileIdValue = converter->GetSQLTypeRemoteFileId();
		EXPECT_FALSE(executorEAV->GetValue("blobs", 2, converter->GetSQLTypeText("Id"),
			remoteFileIdValue)->HasError());
		EXPECT_EQ(remoteFileIdValue->GetValue(), createdFileNames[1]);
	}
}



/// GetValue дает пустое значение для существующей сущности, если у него нет
/// переданного атрибута, который тем не менее присутствует среди атрибутов сущности
TEST_F(ExecutorEAVWithFilledEnvironment, GetValueGetsEmptyVarForExistingEntityAndMissingAttribute)
{
	ISQLTypeTextPtr textValue = converter->GetSQLTypeText();
	// Существует сущность "images" с идентификатором 4, однако у неё нет
	// значения по атрибуту "Name".
	auto status = executorEAV->GetValue("images", 4, converter->GetSQLTypeText("Name"),
		textValue);
	EXPECT_FALSE(status->HasError());
	EXPECT_EQ(status->GetStatus(), ResultStatus::OkWithData);
	EXPECT_EQ(textValue->GetValue(), std::nullopt);
}


/// GetValue возвращает ошибку, если ему передать несуществующий атрибут
TEST_F(ExecutorEAVWithFilledEnvironment, GetValuesDoesNotGetWithNonExistingAttribute)
{
	ISQLTypeTextPtr textValue = converter->GetSQLTypeText();
	// У "users" нет атрибута "Address"
	auto status = executorEAV->GetValue("users", 1, converter->GetSQLTypeText("Address"),
		textValue);
	EXPECT_TRUE(status->HasError());
	EXPECT_EQ(status->GetStatus(), ResultStatus::FatalError);
	EXPECT_EQ(textValue->GetValue(), std::nullopt);
}


/// GetValue возвращает ошибку, если ему передать несуществующий идентификатор
TEST_F(ExecutorEAVWithFilledEnvironment, GetValuesDoesNotGetWithNonExistingEntityId)
{
	ISQLTypeTextPtr textValue = converter->GetSQLTypeText();
	// Нет сущности "users" с идентификатором 2
	auto status = executorEAV->GetValue("users", 2, converter->GetSQLTypeText("Name"), textValue);
	EXPECT_TRUE(status->HasError());
	EXPECT_EQ(status->GetStatus(), ResultStatus::FatalError);
	EXPECT_EQ(textValue->GetValue(), std::nullopt);
}


/// GetValue возвращает ошибку, если ему передать невалидное название атрибута
TEST_F(ExecutorEAVWithFilledEnvironment, GetValuesDoesNotGetWithInvalidAttrName)
{
	{
		ISQLTypeTextPtr textValue = converter->GetSQLTypeText();
		auto status = executorEAV->GetValue("users", 1, converter->GetSQLTypeText(), textValue);
		EXPECT_TRUE(status->HasError());
		EXPECT_EQ(status->GetStatus(), ResultStatus::EmptyQuery);
		EXPECT_EQ(textValue->GetValue(), std::nullopt);
	}
	{
		ISQLTypeTextPtr textValue = converter->GetSQLTypeText();
		auto status = executorEAV->GetValue("users", 1, nullptr, textValue);
		EXPECT_TRUE(status->HasError());
		EXPECT_EQ(status->GetStatus(), ResultStatus::EmptyQuery);
		EXPECT_EQ(textValue->GetValue(), std::nullopt);
	}
}


/// GetValue возвращает ошибку, если ему передать невалидную переменную для записи значения
TEST_F(ExecutorEAVWithFilledEnvironment, GetValuesDoesNotGetWithInvalidValueVar)
{
	{
		auto status = executorEAV->GetValue("users", 1, converter->GetSQLTypeText("Name"),
			nullptr);
		EXPECT_TRUE(status->HasError());
		EXPECT_EQ(status->GetStatus(), ResultStatus::EmptyQuery);
	}
	{
		ISQLTypeTextPtr textValue = converter->GetSQLTypeText("Invalid");
		// Неправильно - переменная должна быть пустая
		auto status = executorEAV->GetValue("users", 1, converter->GetSQLTypeText("Name"),
			textValue);
		EXPECT_TRUE(status->HasError());
		EXPECT_EQ(status->GetStatus(), ResultStatus::EmptyQuery);
		EXPECT_EQ(textValue->GetValue(), "Invalid");
	}
}
