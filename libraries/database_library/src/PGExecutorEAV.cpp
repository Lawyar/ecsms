#include "PGExecutorEAV.h"

#include <InternalExecuteResultStatus.h>
#include <Utils/StringUtils.h>
#include <SQLUtility.h>

#include <type_traits>
#include <numeric>

#include <cassert>


//------------------------------------------------------------------------------
/**
  Конструктор
*/
//---
PGExecutorEAV::PGExecutorEAV(const IConnectionPtr & connection,
	const ISQLTypeConverterPtr & sqlTypeConverter)
	: m_connection(connection)
	, m_sqlTypeConverter(sqlTypeConverter)
{
}


//------------------------------------------------------------------------------
/**
  Регистрация EAV-сущностей
  \param createTables Требуется ли пытаться создать таблицы по зарегистрированным сущностям
*/
//---
IExecuteResultStatusPtr PGExecutorEAV::SetRegisteredEntities(const EAVRegisterEntries & entries,
	bool createTables)
{
	for (auto &&[entityName, attributeTypes] : entries)
	{
		if (IsSQLKeyword(entityName))
			return InternalExecuteResultStatus::GetInternalError(
				utils::string::Format(
					"IExecutorEAV::SetRegisteredEntities: "
					"The entity name ({}) matches the SQL keyword",
					entityName));
	}

	// В любом случае соберем запрос
	// Если на этом этапе произойдет ошибка, то не надо переустанавливать контейнер
	std::string query;
	for (auto &&[entityName, attributeTypes] : entries)
	{
		query += createEntityTableCommand(entityName);
		for (auto && attributeType : attributeTypes)
		{
			std::string attributeTypeStr;
			if (auto status = getSQLTypeName(attributeType, attributeTypeStr);
				status && status->HasError())
				return status;
			query += createAttributeTableCommand(entityName, attributeTypeStr);
			query += createValueTableCommand(entityName, attributeTypeStr);
		}
	}

	if (!createTables)
	{
		m_registerEntries = entries;
		return InternalExecuteResultStatus::GetSuccessStatus(ResultStatus::OkWithoutData);
	}

	IExecuteResultPtr result;
	IExecuteResultStatusPtr resultStatus;
	if (!executeQuery(query, result, resultStatus))
		return resultStatus;

	m_registerEntries = entries;
	return resultStatus;
}


//------------------------------------------------------------------------------
/**
  Получить зарегистрированные сущности
*/
//---
const IExecutorEAV::EAVRegisterEntries & PGExecutorEAV::GetRegisteredEntities() const
{
	return m_registerEntries;
}


//------------------------------------------------------------------------------
/**
  Получить объект, определяющий правила именования таблиц
*/
//---
const IExecutorEAVNamingRules & PGExecutorEAV::GetNamingRules() const
{
	return m_rules;
}


//------------------------------------------------------------------------------
/**
  Получить команду создания таблицы сущностей
*/
//---
std::string PGExecutorEAV::createEntityTableCommand(const std::string & entityName) const
{
	return utils::string::Format(
		"CREATE TABLE IF NOT EXISTS {} "
		"("
		"{} INTEGER NOT NULL GENERATED ALWAYS AS IDENTITY, "
		"PRIMARY KEY({})"
		");\n",
		GetNamingRules().GetEntityTableName(entityName),
		GetNamingRules().GetEntityTable_Short_IdField(entityName),
		GetNamingRules().GetEntityTable_Short_IdField(entityName));
}


//------------------------------------------------------------------------------
/**
  Получить команду создания таблицы атрибутов
*/
//---
std::string PGExecutorEAV::createAttributeTableCommand(const std::string & entityName,
	const std::string & attributeType) const
{
	return utils::string::Format(
		"CREATE TABLE IF NOT EXISTS {} "
		"("
		"{} INTEGER NOT NULL GENERATED ALWAYS AS IDENTITY, "
		"{} TEXT NOT NULL, "
		"PRIMARY KEY({}), "
		"UNIQUE ({})"
		");\n",
		GetNamingRules().GetAttributeTableName(entityName, attributeType),
		GetNamingRules().GetAttributeTable_Short_IdField(entityName, attributeType),
		GetNamingRules().GetAttributeTable_Short_NameField(entityName, attributeType),
		GetNamingRules().GetAttributeTable_Short_IdField(entityName, attributeType),
		GetNamingRules().GetAttributeTable_Short_NameField(entityName, attributeType)
	);
}


//------------------------------------------------------------------------------
/**
  Получить команду создания таблицы значений
  todo : Надо переиспользовать именованные константы из базового класса...
*/
//---
std::string PGExecutorEAV::createValueTableCommand(const std::string & entityName,
	const std::string & attributeType) const
{
	return utils::string::Format(
		"CREATE TABLE IF NOT EXISTS {} "
		"("
		"{} INTEGER NOT NULL REFERENCES {}({}), "
		"{} INTEGER NOT NULL REFERENCES {}({}), "
		"{} {} NOT NULL, "
		"PRIMARY KEY({}, {})"
		");\n",
		GetNamingRules().GetValueTableName(entityName, attributeType),

		GetNamingRules().GetValueTable_Short_EntityIdField(entityName, attributeType),
		GetNamingRules().GetEntityTableName(entityName),
		GetNamingRules().GetEntityTable_Short_IdField(entityName),

		GetNamingRules().GetValueTable_Short_AttributeIdField(entityName, attributeType),
		GetNamingRules().GetAttributeTableName(entityName, attributeType),
		GetNamingRules().GetAttributeTable_Short_IdField(entityName, attributeType),

		GetNamingRules().GetValueTable_Short_ValueField(entityName, attributeType),
		utils::string::ToUpper(attributeType),

		GetNamingRules().GetValueTable_Short_EntityIdField(entityName, attributeType),
		GetNamingRules().GetValueTable_Short_AttributeIdField(entityName, attributeType)
	);
}


//------------------------------------------------------------------------------
/**
  Создать новую сущность
*/
//---
IExecuteResultStatusPtr PGExecutorEAV::CreateNewEntity(const EntityName & entityName, EntityId & entityId)
{
	std::string query;
	query += insertNewEntityReturningIdCommand(entityName);

	IExecuteResultPtr result;
	IExecuteResultStatusPtr resultStatus;
	if (!executeQuery(query, result, resultStatus))
		return resultStatus;

	size_t idFieldIndex = result->GetColIndex(GetNamingRules().GetEntityTable_Short_IdField(entityName));
	IExecuteResult::CellType newId = result->GetValue(0, idFieldIndex);
	if (IExecuteResultStatusPtr readStatus;
		!readIntoSQLVariable<ISQLTypeInteger>(newId.ExtractString(), result->GetColType(idFieldIndex),
			entityId, readStatus))
		return readStatus;

	return resultStatus;
}


//------------------------------------------------------------------------------
/**
  Получить все идентификаторы сущности данного вида
*/
//---
IExecuteResultStatusPtr PGExecutorEAV::GetEntityIds(const EntityName & entityName,
	std::vector<EntityId> & entityIds)
{
	std::string query = getEntityIdsCommand(entityName);
	IExecuteResultPtr result;
	IExecuteResultStatusPtr resultStatus;
	if (!executeQuery(query, result, resultStatus))
		return resultStatus;

	auto colType = result->GetColType(0);
	if (result->GetColCount() != 1 || colType != SQLDataType::Integer)
	{
		// Запрос был сформирован таким образом, что в результате должен быть один столбец
		// типа INTEGER
		assert(false);

		return InternalExecuteResultStatus::GetInternalError(
			"IExecutorEAV::GetEntityIds: Unexpected result");
	}

	std::vector<EntityId> tempEntityIds;
	for (size_t rowIndex = 0, rowCount = result->GetRowCount(); rowIndex < rowCount; ++rowIndex)
	{
		auto cell = result->GetValue(rowIndex, 0);
		EntityId entityId;
		if (IExecuteResultStatusPtr readStatus;
			!readIntoSQLVariable<ISQLTypeInteger>(cell.ExtractString(), colType, entityId, readStatus))
			return readStatus;
		tempEntityIds.push_back(entityId);
	}

	// Теперь уже точно нет ошибок, можно сохранять результат
	entityIds = std::move(tempEntityIds);
	
	return resultStatus;
}


//------------------------------------------------------------------------------
/**
  Получить все наименования атрибутов указанного типа, которые использует данная сущность
*/
//---
IExecuteResultStatusPtr PGExecutorEAV::GetAttributeNames(const EntityName & entityName,
	SQLDataType sqlDataType, std::vector<AttrName> & attrNames)
{
	std::string attributeType;
	if (auto sqlVar = m_sqlTypeConverter->GetSQLVariable(sqlDataType))
		attributeType = sqlVar->GetTypeName();

	if (attributeType.empty())
		return InternalExecuteResultStatus::GetInternalError(
			"IExecutorEAV::GetAttributeNames: Invalid attribute type", ResultStatus::EmptyQuery);

	std::string query = getAttributeNamesCommand(entityName, attributeType);
	IExecuteResultPtr result;
	IExecuteResultStatusPtr resultStatus;
	if (!executeQuery(query, result, resultStatus))
		return resultStatus;

	auto colType = result->GetColType(0);
	if (result->GetColCount() != 1 || colType != SQLDataType::Text)
	{
		// Запрос был сформирован таким образом, что в результате должен быть один столбец
		// типа TEXT
		assert(false);

		return InternalExecuteResultStatus::GetInternalError(
			"IExecutorEAV::GetAttributeNames: Unexpected result");
	}

	std::vector<AttrName> tempAttrNames;
	for (size_t rowIndex = 0, rowCount = result->GetRowCount(); rowIndex < rowCount; ++rowIndex)
	{
		auto cell = result->GetValue(rowIndex, 0);
		std::string attrNameStr;
		if (IExecuteResultStatusPtr readStatus;
			!readIntoSQLVariable<ISQLTypeText>(cell.ExtractString(), colType, attrNameStr, readStatus))
			return readStatus;
		tempAttrNames.push_back(m_sqlTypeConverter->GetSQLTypeText(std::move(attrNameStr)));
	}

	// Теперь уже точно нет ошибок, можно сохранять результат
	attrNames = std::move(tempAttrNames);

	return resultStatus;
}


//------------------------------------------------------------------------------
/**
  Найти сущности, у которых есть все из указанных пар атрибут-значение
*/
//---
IExecuteResultStatusPtr PGExecutorEAV::FindEntitiesByAttrValues(const EntityName & entityName,
	const std::vector<AttrValue> & attrValues, std::vector<EntityId>& entityIds)
{
	if (attrValues.empty())
		return InternalExecuteResultStatus::GetInternalError(
			"IExecutorEAV::FindEntitiesByAttrValues: Empty array was passed", ResultStatus::EmptyQuery);

	std::string query;
	for (auto && attrValue : attrValues)
	{
		if (!query.empty())
			query += "  INTERSECT\n";
		if (auto currentPartCommand = getEntityIdByAttrValueInnerCommand(entityName, attrValue))
			query += *currentPartCommand;
		else
			return InternalExecuteResultStatus::GetInternalError(
				"IExecutorEAV::FindEntitiesByAttrValues: Empty value was passed", ResultStatus::EmptyQuery);
	}
	query += ";";

	IExecuteResultPtr result;
	IExecuteResultStatusPtr resultStatus;
	if (!executeQuery(query, result, resultStatus))
		return resultStatus;

	// Запрос формировался таким образом, что в ответе должен получиться один столбец
	assert(result->GetColCount() == 1);
	std::vector<EntityId> tempEntityIds;
	for (size_t j = 0, colCount = result->GetColCount(); j < colCount; ++j)
	{
		SQLDataType colType = result->GetColType(j);
		for (size_t i = 0, rowCount = result->GetRowCount(); i < rowCount; ++i)
		{
			IExecuteResult::CellType cellValue = result->GetValue(i, j);
			EntityId entityId;
			if (IExecuteResultStatusPtr readStatus;
				!readIntoSQLVariable<ISQLTypeInteger>(cellValue.ExtractString(), colType, entityId, readStatus))
				return readStatus;
			tempEntityIds.push_back(entityId);
		}
	}

	// Теперь уже точно нет ошибок, можно сохранять результат
	entityIds = std::move(tempEntityIds);

	return resultStatus;
}


//------------------------------------------------------------------------------
/**
  Получить команду "добавить сущность в таблицу сущностей и вернуть
  вставленный идентификатор".
*/
//---
std::string PGExecutorEAV::insertNewEntityReturningIdCommand(const std::string & entityName) const
{
	return utils::string::Format("INSERT INTO {} VALUES(DEFAULT) RETURNING {};\n",
		GetNamingRules().GetEntityTableName(entityName),
		GetNamingRules().GetEntityTable_Short_IdField(entityName));
}


//------------------------------------------------------------------------------
/**
  Получить команду "получить идентификаторы сущности".
*/
//---
std::string PGExecutorEAV::getEntityIdsCommand(const std::string & entityName) const
{
	return utils::string::Format("SELECT {} FROM {};\n",
		GetNamingRules().GetEntityTable_Short_IdField(entityName),
		GetNamingRules().GetEntityTableName(entityName));
}


//------------------------------------------------------------------------------
/**
  Получить команду "получить названия атрибутов указанного типа, которые
  использует данная сущность"
*/
//---
std::string PGExecutorEAV::getAttributeNamesCommand(const std::string & entityName,
	const std::string & attributeType) const
{
	return utils::string::Format("SELECT {} FROM {};",
		GetNamingRules().GetAttributeTable_Short_NameField(entityName, attributeType),
		GetNamingRules().GetAttributeTableName(entityName, attributeType));
}


//------------------------------------------------------------------------------
/**
  Получить внутреннюю команды "получить идентификаторы сущности по названию атрибута
  и его значению"
*/
//---
std::optional<std::string> PGExecutorEAV::getEntityIdByAttrValueInnerCommand(const EntityName & entityName,
	const AttrValue & attrValue) const
{
	if (!attrValue.value || !attrValue.attrName)
		return std::nullopt;

	auto attrNameStrOpt = attrValue.attrName->ToSQLString();
	if (!attrNameStrOpt)
		return std::nullopt;
	
	auto attrValueStrOpt = attrValue.value->ToSQLString();
	auto && attributeType = attrValue.value->GetTypeName();

	if (attrValueStrOpt.has_value())
	{
		return utils::string::Format(
			"(SELECT {} FROM {} WHERE {} = {} AND {} IN (SELECT {} FROM {} WHERE {} = {}))\n",
			GetNamingRules().GetValueTable_Short_EntityIdField(entityName, attributeType),
			GetNamingRules().GetValueTableName(entityName, attributeType),
			GetNamingRules().GetValueTable_Short_ValueField(entityName, attributeType),
			*attrValueStrOpt,
			GetNamingRules().GetValueTable_Short_AttributeIdField(entityName, attributeType),
			GetNamingRules().GetAttributeTable_Short_IdField(entityName, attributeType),
			GetNamingRules().GetAttributeTableName(entityName, attributeType),
			GetNamingRules().GetAttributeTable_Short_NameField(entityName, attributeType),
			*attrNameStrOpt
		);
	}
	else if (attrValue.value->IsEmpty())
	{
		// Если значение пустое, то найдем все идентификаторы сущностей, у которых данных
		// данный атрибут не задан. Для этого возьмем все идентификаторы и исключим из них те,
		// у которых значение по атрибуту задано (т.е. присутствует запись в таблице значений).
		return utils::string::Format(
			"(SELECT * FROM {} EXCEPT SELECT {} FROM {} WHERE {} IN (SELECT {} FROM {} WHERE {} = {}))\n",
			GetNamingRules().GetEntityTableName(entityName),
			GetNamingRules().GetValueTable_Short_EntityIdField(entityName, attributeType),
			GetNamingRules().GetValueTableName(entityName, attributeType),
			GetNamingRules().GetValueTable_Short_AttributeIdField(entityName, attributeType),
			GetNamingRules().GetAttributeTable_Short_IdField(entityName, attributeType),
			GetNamingRules().GetAttributeTableName(entityName, attributeType),
			GetNamingRules().GetAttributeTable_Short_NameField(entityName, attributeType),
			*attrNameStrOpt
		);
	}
	
	return std::nullopt;
}


//------------------------------------------------------------------------------
/**
  Вставить значение для атрибута сущности
*/
//---
IExecuteResultStatusPtr PGExecutorEAV::Insert(const EntityName & entityName,
	EntityId entityId, const AttrName & _attrName, const ValueType & value)
{
	if (!valueTypeIsValid(value))
		return InternalExecuteResultStatus::GetInternalError(
			"IExecutorEAV::Insert: Invalid value", ResultStatus::EmptyQuery);

	std::string sqlAttrName;
	if (auto status = getSQLAttrName(_attrName, sqlAttrName))
		return status;

	auto && attributeType = value->GetTypeName();
	auto && sqlValueOpt = value->ToSQLString();

	std::string query;
	if (value->IsEmpty())
	{
		// Пустое значение считаем ошибкой, потому что не имеет смысла вставлять пустое значение
		// Смысл EAV в том, что мы не храним пустые значения (null), если они есть.
		// А если мы их не храним, то и нечего вставлять.
		// Также не будем вставлять и атрибут в таблицу атрибутов.
		return InternalExecuteResultStatus::GetInternalError(
			"IExecutorEAV::Insert: Empty value was passed", ResultStatus::EmptyQuery);
	}
	else if (sqlValueOpt)
	{
		query += insertAttributeOnConflictDoNothingCommand(entityName, attributeType,
			sqlAttrName);
		query += insertValueCommand(entityName, entityId, sqlAttrName, attributeType,
			*sqlValueOpt);
	}

	IExecuteResultPtr result;
	IExecuteResultStatusPtr resultStatus;
	executeQuery(query, result, resultStatus);

	return resultStatus;
}


//------------------------------------------------------------------------------
/**
  Обновить значение для атрибута сущности
*/
//---
IExecuteResultStatusPtr PGExecutorEAV::Update(const EntityName & entityName,
	EntityId entityId, const AttrName & _attrName, const ValueType & value)
{
	if (!valueTypeIsValid(value))
		return InternalExecuteResultStatus::GetInternalError(
			"IExecutorEAV::Insert: Invalid value", ResultStatus::EmptyQuery);

	std::string sqlAttrName;
	if (auto status = getSQLAttrName(_attrName, sqlAttrName))
		return status;

	auto && attributeType = value->GetTypeName();
	auto && sqlValueOpt = value->ToSQLString();

	std::string query;
	if (value->IsEmpty())
	{
		query += throwErrorIfThereIsNoEntityWithSuchIdCommand(entityName, entityId);
		query += throwErrorIfThereIsNoAttributeWithSuchNameCommand(entityName,
			attributeType, sqlAttrName);
		query += removeValueCommand(entityName, entityId, attributeType, sqlAttrName);
	}
	else if (sqlValueOpt)
	{
		query += throwErrorIfThereIsNoEntityWithSuchIdCommand(entityName, entityId);
		query += throwErrorIfThereIsNoAttributeWithSuchNameCommand(entityName,
			attributeType, sqlAttrName);
		query += updateValueCommand(entityName, entityId, sqlAttrName, attributeType,
			*sqlValueOpt);
	}

	IExecuteResultPtr result;
	IExecuteResultStatusPtr resultStatus;
	executeQuery(query, result, resultStatus);

	return resultStatus;
}


//------------------------------------------------------------------------------
/**
  Обновить значение для атрибута сущности или вставить, если такого значения ещё не было
*/
//---
IExecuteResultStatusPtr PGExecutorEAV::InsertOrUpdate(const EntityName & entityName,
	EntityId entityId, const AttrName & _attrName, const ValueType & value)
{
	if (!valueTypeIsValid(value))
		return InternalExecuteResultStatus::GetInternalError(
			"IExecutorEAV::Insert: Invalid value", ResultStatus::EmptyQuery);

	std::string sqlAttrName;
	if (auto status = getSQLAttrName(_attrName, sqlAttrName))
		return status;

	auto && attributeType = value->GetTypeName();
	auto && sqlValueOpt = value->ToSQLString();
	
	std::string query;
	if (value->IsEmpty())
	{
		query += throwErrorIfThereIsNoEntityWithSuchIdCommand(entityName, entityId);
		query += insertAttributeOnConflictDoNothingCommand(entityName, attributeType,
			sqlAttrName);
		query += removeValueCommand(entityName, entityId, attributeType, sqlAttrName);
	}
	else if (sqlValueOpt)
	{
		query += insertAttributeOnConflictDoNothingCommand(entityName, attributeType,
			sqlAttrName);
		query += insertValueOnConflictDoUpdateCommand(entityName, entityId, sqlAttrName,
			attributeType, *sqlValueOpt);
	}

	IExecuteResultPtr result;
	IExecuteResultStatusPtr resultStatus;
	executeQuery(query, result, resultStatus);

	return resultStatus;
}


//------------------------------------------------------------------------------
/**
  Получить команду "вставить атрибут в таблицу атрибутов".
*/
//---
std::string PGExecutorEAV::insertAttributeOnConflictDoNothingCommand(const EntityName & entityName,
	const std::string & attributeType, const std::string & sqlAttrName) const
{
	return utils::string::Format("INSERT INTO {} VALUES(DEFAULT, {}) ON CONFLICT ({}) DO NOTHING;\n",
		GetNamingRules().GetAttributeTableName(entityName, attributeType),
		sqlAttrName,
		GetNamingRules().GetAttributeTable_Short_NameField(entityName, attributeType));
}


//------------------------------------------------------------------------------
/**
  Получить часть команды "вставить значение в таблицу значений".
*/
//---
std::string PGExecutorEAV::insertValuePartCommand(const EntityName & entityName, EntityId entityId,
	const std::string & sqlAttrName, const std::string & attributeType,
	const std::string & sqlValue) const
{
	return utils::string::Format(
		" INSERT INTO {} VALUES({}, {}, {}) ",
		GetNamingRules().GetValueTableName(entityName, attributeType),
		entityId,
		selectAttributeIdByNameInnerCommand(entityName, attributeType, sqlAttrName),
		sqlValue
	);
}


//------------------------------------------------------------------------------
/**
  Получить команду "вставить значение в таблицу значений".
*/
//---
std::string PGExecutorEAV::insertValueCommand(const EntityName & entityName,
	EntityId entityId, const std::string & sqlAttrName, const std::string & attributeType,
	const std::string & sqlValue) const
{
	auto && partCommand = insertValuePartCommand(entityName, entityId, sqlAttrName,
		attributeType, sqlValue);
	return partCommand + ";\n";
}


//------------------------------------------------------------------------------
/**
  Получить команду "вставить значение в таблицу значений, при конфликте сделать обновление"
*/
//---
std::string PGExecutorEAV::insertValueOnConflictDoUpdateCommand(const EntityName & entityName,
	EntityId entityId, const std::string & sqlAttrName, const std::string & attributeType,
	const std::string & sqlValue) const
{
	auto && partCommand = insertValuePartCommand(entityName, entityId, sqlAttrName,
		attributeType, sqlValue);
	return utils::string::Format("{}\nON CONFLICT ({}, {}) DO UPDATE SET {} = EXCLUDED.{};\n",
		partCommand,
		GetNamingRules().GetValueTable_Short_EntityIdField(entityName, attributeType),
		GetNamingRules().GetValueTable_Short_AttributeIdField(entityName, attributeType),
		GetNamingRules().GetValueTable_Short_ValueField(entityName, attributeType),
		GetNamingRules().GetValueTable_Short_ValueField(entityName, attributeType));
}


//------------------------------------------------------------------------------
/**
  Получить команду "обновить значение в таблице значений".
*/
//---
std::string PGExecutorEAV::updateValueCommand(const EntityName & entityName,
	EntityId entityId, const std::string & sqlAttrName, const std::string & attributeType,
	const std::string & sqlValue) const
{
	return utils::string::Format(
		"UPDATE {} SET {} = {}\n"
		"WHERE ({}, {}) = ({}, {});\n",
		GetNamingRules().GetValueTableName(entityName, attributeType),
		GetNamingRules().GetValueTable_Short_ValueField(entityName, attributeType),
		sqlValue,

		GetNamingRules().GetValueTable_Short_EntityIdField(entityName, attributeType),
		GetNamingRules().GetValueTable_Short_AttributeIdField(entityName, attributeType),
		entityId,
		selectAttributeIdByNameInnerCommand(entityName, attributeType, sqlAttrName)
	);
}


//------------------------------------------------------------------------------
/**
  Получить команду "удалить значение в таблице значений".
*/
//---
std::string PGExecutorEAV::removeValueCommand(const EntityName & entityName, EntityId entityId,
	const std::string & attributeType, const std::string & sqlAttrName) const
{
	return utils::string::Format(
		"DELETE FROM {}\n"
		"WHERE ({}, {}) = ({}, {});\n",
		GetNamingRules().GetValueTableName(entityName, attributeType),

		GetNamingRules().GetValueTable_Short_EntityIdField(entityName, attributeType),
		GetNamingRules().GetValueTable_Short_AttributeIdField(entityName, attributeType),
		entityId,
		selectAttributeIdByNameInnerCommand(entityName, attributeType, sqlAttrName)
	);
}


//------------------------------------------------------------------------------
/**
  Получить команду "Бросить ошибку, если в таблице сущностей нет сущности с
  данным идентификатором"
*/
//---
std::string PGExecutorEAV::throwErrorIfThereIsNoEntityWithSuchIdCommand(const EntityName & entityName,
	EntityId entityId) const
{
	return utils::string::Format(
		"DO $$DECLARE BEGIN IF NOT EXISTS (SELECT * FROM {} WHERE {} = {}) \n"
		"THEN RAISE EXCEPTION 'There is no entity with such id ({})'; END IF; END; $$;\n",
		GetNamingRules().GetEntityTableName(entityName),
		GetNamingRules().GetEntityTable_Short_IdField(entityName),
		entityId,

		entityId
	);
}


//------------------------------------------------------------------------------
/**
  Получить команду "Бросить ошибку, если в таблице атрибутов нет атрибута с
  данным именем"
*/
//---
std::string PGExecutorEAV::throwErrorIfThereIsNoAttributeWithSuchNameCommand(const EntityName & entityName, const std::string & attributeType, const std::string & sqlAttrName) const
{
	return utils::string::Format(
		"DO $$DECLARE BEGIN IF NOT EXISTS (SELECT * FROM {} WHERE {} = {}) \n"
		"THEN RAISE EXCEPTION 'There is no attribute with such name (%)', {}; END IF; END; $$;\n",
		GetNamingRules().GetAttributeTableName(entityName, attributeType),
		GetNamingRules().GetAttributeTable_Short_NameField(entityName, attributeType),
		sqlAttrName,

		sqlAttrName
	);
}


//------------------------------------------------------------------------------
/**
  Получить внутреннюю команду "получить идентификатор атрибута по его названию".
*/
//---
std::string PGExecutorEAV::selectAttributeIdByNameInnerCommand(const EntityName & entityName, const std::string & attributeType, const std::string & sqlAttrName) const
{
	return utils::string::Format(
		" (SELECT {} FROM {} WHERE {} = {}) ",
		GetNamingRules().GetAttributeTable_Short_IdField(entityName, attributeType),
		GetNamingRules().GetAttributeTableName(entityName, attributeType),
		GetNamingRules().GetAttributeTable_Short_NameField(entityName, attributeType),
		sqlAttrName
	);
}


//------------------------------------------------------------------------------
/**
  Получить значение атрибута сущности.
*/
//---
IExecuteResultStatusPtr PGExecutorEAV::GetValue(const EntityName & entityName, EntityId entityId,
	const AttrName & _attrName, ValueType value)
{
	if (!valueTypeIsValid(value))
		return InternalExecuteResultStatus::GetInternalError(
			"IExecutorEAV::Insert: Value is null", ResultStatus::EmptyQuery);

	if (value->ToSQLString().has_value())
		return InternalExecuteResultStatus::GetInternalError(
			"IExecutorEAV::Insert: Value is not empty", ResultStatus::EmptyQuery);

	std::string sqlAttrName;
	if (auto status = getSQLAttrName(_attrName, sqlAttrName))
		return status;

	std::string query = selectValueByEntityIdAndAttributeNameCommand(entityName, entityId,
		value->GetTypeName(), sqlAttrName);

	IExecuteResultPtr result;
	IExecuteResultStatusPtr resultStatus;
	if (!executeQuery(query, result, resultStatus))
		return resultStatus;

	auto && rowCount = result->GetRowCount();
	auto && colCount = result->GetColCount();
	if (rowCount > 1 || colCount != 1)
		return InternalExecuteResultStatus::GetInternalError(
			utils::string::Format(
				"IExecutorEAV::GetValue: Unexpected result. "
				"Returned table size ({}, {}), but expected (1, 1) or (0, 1).",
				rowCount, colCount));

	auto && cell = result->GetValue(0, 0);
	if (cell.HasString())
	{
		if (!value->ReadFromSQL(cell.ExtractString()))
			return InternalExecuteResultStatus::GetInternalError(ErrorMessages::ISQLType_ReadFromSQL);
	}
	else
	{
		// Если ячейка не содержит, это нормально.
		// Значит, у данной атрибута данной сущности не записано значение.
		// Считаем, что оно равно null и возвращаем пустую переменную
		// (ту же самую, что и приняли)
	}

	return resultStatus;
}


//------------------------------------------------------------------------------
/**
  Получить значения всех атрибутов сущности.
*/
//---
IExecuteResultStatusPtr PGExecutorEAV::GetAttributeValues(const EntityName & entityName,
	EntityId entityId, std::map<SQLDataType, std::vector<AttrValue>> & attrValuesByType)
{
	std::vector<SQLDataType> attributeTypes;
	if (auto iter = GetRegisteredEntities().find(entityName); iter != GetRegisteredEntities().end())
	{
		attributeTypes = iter->second;
	}
	else
	{
		return InternalExecuteResultStatus::GetInternalError(utils::string::Format(
			"IExecutorEAV::GetAttributeValues: Attribute types not found. "
			"The entity \"{}\" may not have been registered using the method \"SetRegisteredEntities\".",
			entityName
		), ResultStatus::EmptyQuery);
	}
	/*
	todo: С помощью такого запроса можно собрать данные по всем атрибутам одним запросом.
	Но пока поддержка массивов не реализована, буду делать через несколько запросов.

	WITH

	users_attribute_value_text AS
	(SELECT ARRAY_AGG(users_attribute_text.name), ARRAY_AGG(users_value_text.value)
	FROM users_value_text
	JOIN users_attribute_text ON users_value_text.entity_id = 1 AND users_attribute_text.id = users_value_text.attribute_id),

	users_attribute_value_integer AS
	(SELECT ARRAY_AGG(users_attribute_integer.name), ARRAY_AGG(users_value_integer.value)
	FROM users_value_integer
	JOIN users_attribute_integer ON users_value_integer.entity_id = 1 AND users_attribute_integer.id = users_value_integer.attribute_id)

	SELECT * FROM users_attribute_value_text CROSS JOIN users_attribute_value_integer;
	*/

	{
		// Если нет такого идентификатора, надо вернуть ошибку
		std::string prepareQuery = throwErrorIfThereIsNoEntityWithSuchIdCommand(entityName, entityId);
		IExecuteResultPtr result;
		IExecuteResultStatusPtr status;
		if (!executeQuery(prepareQuery, result, status))
			return status;
	}

	std::map<SQLDataType, std::vector<AttrValue>> tempAttrValuesByType;
	for (auto && attributeType : attributeTypes)
	{
		std::string attributeTypeStr;
		if (auto status = getSQLTypeName(attributeType, attributeTypeStr);
			status && status->HasError())
			return status;
		std::string query = selectAttrValuesCommand(entityName, entityId, attributeTypeStr);
		
		IExecuteResultPtr result;
		IExecuteResultStatusPtr status;
		if (!executeQuery(query, result, status))
			return status;

		if (auto status = getAttributeValuesImpl(result, tempAttrValuesByType[attributeType]);
			status && status->HasError())
			return status;
	}

	// Тут уже ошибок точно нет. Перепишем результат
	attrValuesByType = std::move(tempAttrValuesByType);

	return InternalExecuteResultStatus::GetSuccessStatus();
}


//------------------------------------------------------------------------------
/**
  Получить команду "получить значение по идентификатору сущности и названию атрибута".
*/
//---
std::string PGExecutorEAV::selectValueByEntityIdAndAttributeNameCommand(const EntityName & entityName,
	EntityId entityId, const std::string & attributeType, const std::string & sqlAttrName) const
{
	return utils::string::Format(
		// Бросим ошибку, если в таблице сущностей нет сущности с данным идентификатором
		"DO $$DECLARE BEGIN IF NOT EXISTS (SELECT * FROM {} WHERE {} = {}) THEN RAISE EXCEPTION 'There is no entity with such id (id={};entity={})'; END IF; END; $$;\n"
		// Бросим ошибку, если в таблице атрибутов нет данного атрибута
		"DO $$DECLARE BEGIN IF NOT EXISTS (SELECT * FROM {} WHERE {} = {}) THEN RAISE EXCEPTION 'There is no attribute with such name (name=%;entity={})', {}; END IF; END; $$;\n"
		"SELECT {} FROM {} WHERE {} = {} AND {} = {};\n",
		GetNamingRules().GetEntityTableName(entityName),
		GetNamingRules().GetEntityTable_Short_IdField(entityName),
		entityId,
		entityId,
		entityName,
		
		GetNamingRules().GetAttributeTableName(entityName, attributeType),
		GetNamingRules().GetAttributeTable_Short_NameField(entityName, attributeType),
		sqlAttrName,
		entityName,
		sqlAttrName,

		GetNamingRules().GetValueTable_Short_ValueField(entityName, attributeType),
		GetNamingRules().GetValueTableName(entityName, attributeType),
		GetNamingRules().GetValueTable_Short_EntityIdField(entityName, attributeType),
		entityId,
		GetNamingRules().GetValueTable_Short_AttributeIdField(entityName, attributeType),
		selectAttributeIdByNameInnerCommand(entityName, attributeType, sqlAttrName)
	);
}


//------------------------------------------------------------------------------
/**
  Получить команду "получить названия атрибутов и их значения"
*/
//---
std::string PGExecutorEAV::selectAttrValuesCommand(const EntityName & entityName, EntityId entityId,
	const std::string & attributeType) const
{
	return utils::string::Format(
		"SELECT {}, {} FROM {}\n"
		"RIGHT OUTER JOIN {}\n"
		"ON {} = {} AND {} = {};\n",
		GetNamingRules().GetAttributeTable_Full_NameField(entityName, attributeType),
		GetNamingRules().GetValueTable_Full_ValueField(entityName, attributeType),
		GetNamingRules().GetValueTableName(entityName, attributeType),

		GetNamingRules().GetAttributeTableName(entityName, attributeType),

		GetNamingRules().GetValueTable_Full_EntityIdField(entityName, attributeType),
		entityId,
		GetNamingRules().GetValueTable_Full_AttributeIdField(entityName, attributeType),
		GetNamingRules().GetAttributeTable_Full_IdField(entityName, attributeType)
	);
}


//------------------------------------------------------------------------------
/**
  Получить значения атрибутов сущности из результата
*/
//---
IExecuteResultStatusPtr PGExecutorEAV::getAttributeValuesImpl(const IExecuteResultPtr & result,
	std::vector<AttrValue> & attrValues) const
{
	if (!result)
	{
		// Должны были передать непустой результат
		assert(false);
		return InternalExecuteResultStatus::GetInternalError(
			"PGExecutorEAV::getAttributeValuesImpl: Empty result passed");
	}

	size_t rowCount = result->GetRowCount();
	size_t colCount = result->GetColCount();
	if (colCount != 2)
	{
		// SELECT должен был вернуть два столбца - название атрибута и его значение
		assert(false);
		return InternalExecuteResultStatus::GetInternalError(
			"PGExecutorEAV::getAttributeValuesImpl: the query returned the wrong number of columns");
	}
	constexpr size_t c_attributeNameIndex = 0;
	constexpr size_t c_attributeValueIndex = 1;
	auto attributeNameDataType = result->GetColType(c_attributeNameIndex);
	auto attributeValueDataType = result->GetColType(c_attributeValueIndex);

	for (size_t i = 0; i < rowCount; ++i)
	{
		auto attrNameSqlVar = std::dynamic_pointer_cast<AttrName::element_type> (
			m_sqlTypeConverter->GetSQLVariable(attributeNameDataType));
		auto valueSqlVar = std::dynamic_pointer_cast<ValueType::element_type>(
			m_sqlTypeConverter->GetSQLVariable(attributeValueDataType));
		if (!attrNameSqlVar || !valueSqlVar)
			return InternalExecuteResultStatus::GetInternalError(
				std::string("PGExecutorEAV::getAttributeValuesImpl\n") + 
				ErrorMessages::ISQLTypeConverter_GetSQLVariable);

		auto cellName = result->GetValue(i, 0);
		auto cellValue = result->GetValue(i, 1);
		if (!cellName.HasString())
		{
			// Название должно содержать строку, потому что они не могут содержать NULL
			// И не могут содержать массивы байт, потому что мы их не запрашивали
			assert(false);
			return InternalExecuteResultStatus::GetInternalError(
				"PGExecutorEAV::getAttributeValuesImpl: The result cells are empty");
		}
		if (!attrNameSqlVar->ReadFromSQL(cellName.ExtractString()))
			return InternalExecuteResultStatus::GetInternalError(ErrorMessages::ISQLType_ReadFromSQL);
		
		if (cellValue.HasString())
		{
			if (!valueSqlVar->ReadFromSQL(cellValue.ExtractString()))
				return InternalExecuteResultStatus::GetInternalError(
					ErrorMessages::ISQLType_ReadFromSQL);
		}
		else if (cellValue.HasNull())
		{
			// Окей, значит у данного атрибута нет значения. Оставим пустую переменную
		}
		else
		{
			// Таких случаев пока нет
			assert(false);
		}


		attrValues.push_back(AttrValue{ attrNameSqlVar, valueSqlVar });
	}

	return nullptr;
}


//------------------------------------------------------------------------------
/**
  Выполнить команду.
  \param query Команда
  \param result Переменная, в которую записывается результат выполнения команды.
  \param status Переменная, в которую записывается статус результата выполнения команды.
				Статус выполнения всегда записывается (даже при возникновении ошибки).
  \return true, если команда выполнена без ошибок, иначе false.
*/
//---
bool PGExecutorEAV::executeQuery(const std::string query, IExecuteResultPtr & result, IExecuteResultStatusPtr & status)
{
	result = m_connection->Execute(query);
	if (!result)
	{
		assert(false);
		status = InternalExecuteResultStatus::GetInternalError(ErrorMessages::IConnection_Execute);
		return false;
	}

	status = result->GetCurrentExecuteStatus();
	if (!status)
	{
		assert(false);
		status = InternalExecuteResultStatus::GetInternalError(
			ErrorMessages::IExecuteResult_GetCurrentExecuteStatus);
		return false;
	}

	return !status->HasError();
}


//------------------------------------------------------------------------------
/**
  Прочитать строку в SQL-переменную.
  \tparam SQLConcreteType Тип SQL-переменной, у которой надо вызывать
	метод GetValue
  \param str Строка
  \param type Тип переменной, которую надо создать.
  \param value Переменная, в которую будет записан результат
  \param status Переменная, в которую записывается статус результата
  \return true, если выполнено без ошибок, иначе false.
	Если функция вернула true, значит в переменную value записан результат.
	Если функция вернула false, значит переменная status будет гарантировано непустая.
*/
//---
template<class SQLConcreteType, class CppConcreteType>
bool PGExecutorEAV::readIntoSQLVariable(std::string && str, SQLDataType type,
	CppConcreteType & value, IExecuteResultStatusPtr & status) const
{
	ISQLTypePtr sqlVar = m_sqlTypeConverter->GetSQLVariable(type);
	if (!sqlVar)
	{
		// Для каждого SQL-типа должен быть класс
		assert(false);
		status = InternalExecuteResultStatus::GetInternalError(ErrorMessages::ISQLTypeConverter_GetSQLVariable);
		return false;
	}
	if (!sqlVar->ReadFromSQL(std::move(str)))
	{
		status = InternalExecuteResultStatus::GetInternalError(
			ErrorMessages::ISQLType_ReadFromSQL +
			utils::string::Format(" (Data: {}, SQL variable type: {})", str, sqlVar->GetTypeName()));
		return false;
	}

	auto sqlConcreteVar = std::dynamic_pointer_cast<SQLConcreteType>(sqlVar);
	if (!sqlConcreteVar)
	{
		status = InternalExecuteResultStatus::GetInternalError(
			"PGExecutorEAV::readIntoSQLVariable: Failed to cast pointer");
		return false;
	}

	auto optValue = sqlConcreteVar->GetValue();
	if (!optValue)
	{
		// Если ReadFromSQL вернул true, то здесь не должны проваливаться
		assert(false);
		status = InternalExecuteResultStatus::GetInternalError(ErrorMessages::ISQLType_GetValue);
		return false;
	}

	// Типы должны полностью совпадать (не хотелось бы здесь получить неявный каст int к double, например)
	static_assert(std::is_same_v<std::decay_t<decltype(value)>, std::decay_t<decltype(*optValue)>>);
	value = *optValue;
	return true;
}


//------------------------------------------------------------------------------
/**
  Получить название атрибута в форме, пригодной для вставления в запрос
  \param attrName Название атрибута
  \param sqlAttrName Переменная, в которую записывается результат
  \return nullptr, если операция выполнена успешно, иначе статус ошибки.
*/
//---
IExecuteResultStatusPtr PGExecutorEAV::getSQLAttrName(const AttrName & attrName,
	std::string & sqlAttrName) const
{
	static const IExecuteResultStatusPtr errorStatus = InternalExecuteResultStatus::GetInternalError(
		"PGExecutorEAV::getSQLAttrName: Empty attribute name passed", ResultStatus::EmptyQuery);
	if (!attrName)
		return errorStatus;

	auto && sqlTextStrOpt = attrName->ToSQLString();
	if (!sqlTextStrOpt)
		return errorStatus;

	sqlAttrName = *sqlTextStrOpt;
	return nullptr;
}


//------------------------------------------------------------------------------
/**
  Получить название типа SQL
*/
//---
IExecuteResultStatusPtr PGExecutorEAV::getSQLTypeName(SQLDataType sqlDataType,
	std::string & sqlTypeName) const
{
	auto sqlVar = m_sqlTypeConverter->GetSQLVariable(sqlDataType);
	if (!sqlVar)
	{
		return InternalExecuteResultStatus::GetInternalError(
			"PGExecutorEAV::getSQLTypeName: Unknown SQL data type\n" +
			std::string(ErrorMessages::ISQLTypeConverter_GetSQLVariable)
		);
	}

	sqlTypeName = sqlVar->GetTypeName();
	if (sqlTypeName.empty())
	{
		assert(false); // Название типа не должно быть пустым
		return InternalExecuteResultStatus::GetInternalError(
			"PGExecutorEAV::getSQLTypeName: SQL typename is empty");
	}
	return nullptr;
}


//------------------------------------------------------------------------------
/**
  Проверка значения на допустимость
*/
//---
bool PGExecutorEAV::valueTypeIsValid(const ValueType & value)
{
	// Если тип значения поменяется, то эту проверку надо будет поменять
	static_assert(std::is_same_v<ValueType, ISQLTypePtr>);

	return value != nullptr;
}
