#include "PGExecutorEAVNamingRules.h"

#include <Utils/StringUtils.h>

//------------------------------------------------------------------------------
/**
  Получить название таблицы сущностей
*/
//---
std::string PGExecutorEAVNamingRules::GetEntityTableName(const std::string & entityName) const
{
	return utils::string::ToLower(entityName);
}


//------------------------------------------------------------------------------
/**
  Получить название таблицы аттрибутов
*/
//---
std::string PGExecutorEAVNamingRules::GetAttributeTableName(const std::string & entityName,
	const std::string & attributeType) const
{
	return utils::string::ToLower(entityName) + "_attribute_" + utils::string::ToLower(attributeType);
}


//------------------------------------------------------------------------------
/**
  Получить название таблицы значений
*/
//---
std::string PGExecutorEAVNamingRules::GetValueTableName(const std::string & entityName,
	const std::string & attributeType) const
{
	return utils::string::ToLower(entityName) + "_value_" + utils::string::ToLower(attributeType);
}



//------------------------------------------------------------------------------
/**
  Получить название поля идентификатора таблицы сущностей
*/
//---
std::string PGExecutorEAVNamingRules::GetEntityTable_Short_IdField(const std::string & entityName) const
{
	return "id";
}


//------------------------------------------------------------------------------
/**
  Получить название поля идентификатора таблицы атрибутов
*/
//---
std::string PGExecutorEAVNamingRules::GetAttributeTable_Short_IdField(const std::string & entityName,
	const std::string & attributeType) const
{
	return "id";
}


//------------------------------------------------------------------------------
/**
  Получить название поля навания таблицы атрибутов
*/
//---
std::string PGExecutorEAVNamingRules::GetAttributeTable_Short_NameField(const std::string & entityName,
	const std::string & attributeType) const
{
	return "name";
}


//------------------------------------------------------------------------------
/**
  Получить название поля идентификатора сущности таблицы значений
*/
//---
std::string PGExecutorEAVNamingRules::GetValueTable_Short_EntityIdField(const std::string & entityName,
	const std::string & attributeType) const
{
	return "entity_id";
}


//------------------------------------------------------------------------------
/**
  Получить название поля идентификатора атрибута таблицы значений
*/
//---
std::string PGExecutorEAVNamingRules::GetValueTable_Short_AttributeIdField(const std::string & entityName,
	const std::string & attributeType) const
{
	return "attribute_id";
}


//------------------------------------------------------------------------------
/**
  Получить название поля значения таблицы значений
*/
//---
std::string PGExecutorEAVNamingRules::GetValueTable_Short_ValueField(const std::string & entityName,
	const std::string & attributeType) const
{
	return "value";
}
