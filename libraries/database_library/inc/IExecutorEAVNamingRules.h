#pragma once

#include <string>

//------------------------------------------------------------------------------
/**
  Интерфейс, определяющий правила именования таблиц для IExecutorEAV.
*/
//---
class IExecutorEAVNamingRules
{
public:
	/// Деструктор
	virtual ~IExecutorEAVNamingRules() = default;

public:
	/// Получить название таблицы сущностей
	virtual std::string GetEntityTableName(const std::string & entityName) const = 0;
	/// Получить название таблицы атрибутов
	virtual std::string GetAttributeTableName(const std::string & entityName,
		const std::string & attributeType) const = 0;
	/// Получить название таблицы значений
	virtual std::string GetValueTableName(const std::string & entityName,
		const std::string & attributeType) const = 0;

public:
	/// Получить название поля идентификатора таблицы сущностей
	virtual std::string GetEntityTable_Short_IdField(const std::string & entityName) const = 0;
	/// Получить полное название поля идентификатора таблицы сущностей, включающее
	/// название таблицы
	std::string GetEntityTable_Full_IdField(const std::string & entityName) const
	{
		return GetEntityTable_Short_IdField(entityName) + "." + GetEntityTableName(entityName);
	}

public:
	/// Получить название поля идентификатора таблицы атрибутов
	virtual std::string GetAttributeTable_Short_IdField(const std::string & entityName,
		const std::string & attributeType) const = 0;
	/// Получить полное название поля идентификатора таблицы атрибутов, включающее
	/// название таблицы атрибутов
	std::string GetAttributeTable_Full_IdField(const std::string & entityName,
		const std::string & attributeType) const
	{
		return GetAttributeTableName(entityName, attributeType) + "." +
			GetAttributeTable_Short_IdField(entityName, attributeType);
	}

	/// Получить название поля названия таблицы атрибутов
	virtual std::string GetAttributeTable_Short_NameField(const std::string & entityName,
		const std::string & attributeType) const = 0;
	/// Получить полное название поля названия таблицы атрибутов, включающее название
	/// таблицы атрибутов
	std::string GetAttributeTable_Full_NameField(const std::string & entityName,
		const std::string & attributeType) const
	{
		return GetAttributeTableName(entityName, attributeType) + "." +
			GetAttributeTable_Short_NameField(entityName, attributeType);
	}

public:
	/// Получить название поля идентификатора сущности таблицы значений
	virtual std::string GetValueTable_Short_EntityIdField(const std::string & entityName,
		const std::string & attributeType) const = 0;
	/// Получить полное название поля идентификатора сущности таблицы значений,
	/// включающее название таблицы значений
	std::string GetValueTable_Full_EntityIdField(const std::string & entityName,
		const std::string & attributeType) const
	{
		return GetValueTableName(entityName, attributeType) + "." +
			GetValueTable_Short_EntityIdField(entityName, attributeType);
	}

	/// Получить название поля идентификатора атрибута таблицы значений
	virtual std::string GetValueTable_Short_AttributeIdField(const std::string & entityName,
		const std::string & attributeType) const = 0;
	/// Получить полное название поля идентификатора атрибута таблицы значений,
	/// включающее название таблицы значений
	std::string GetValueTable_Full_AttributeIdField(const std::string & entityName,
		const std::string & attributeType) const
	{
		return GetValueTableName(entityName, attributeType) + "." +
			GetValueTable_Short_AttributeIdField(entityName, attributeType);
	}

	/// Получить название поля значения атрибута таблицы значений
	virtual std::string GetValueTable_Short_ValueField(const std::string & entityName,
		const std::string & attributeType) const = 0;
	/// Получить полное название поля значения атрибута таблицы значений,
	/// включающее название таблицы значений
	std::string GetValueTable_Full_ValueField(const std::string & entityName,
		const std::string & attributeType) const
	{
		return GetValueTableName(entityName, attributeType) + "." +
			GetValueTable_Short_ValueField(entityName, attributeType);
	}
};
