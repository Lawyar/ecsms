#pragma once

#include <IExecutorEAVNamingRules.h>

//------------------------------------------------------------------------------
/**
  Класс, определяющий правила именования таблиц базы данных, представленной
  схемой EAV.

  Для одного конкретного типа сущности <EntityName> в базе данных присутствуют
  следующие таблицы:
  1) <EntityName> - таблица сущностей - содержит:
	 "id" - идентификатор сущности, является первичным ключом.
  2) <EntityName>_attribute_<AttributeType> - несколько таблиц атрибутов - по одной таблице
	 для каждого типа данных, который хотим хранить. Каждая таблица содержит поля:
	 "id" - идентификатор атрибута - является первичным ключом.
	 "name" - название атрибута - на него наложено ограничение UNIQUE (не может быть разных атрибутов с одинаковыми названиями);
  3) <EntityName>_value_<AttributeType> - таблица значений атрибутов сущностей - содержит поля:
	 "entity_id" - идентификатор сущности, с которой ассоциировано значение в этой строке - является внешним ключом.
	 "attribute_id" - идентификатор атрибута, с которым ассоциировано значение в этой строке - является внешним ключом.
	 Пара (entity_id, attribute_id) является первичным ключом.
	 "value" - значение атрибута для данной сущности. Это поле имеет тип <AttributeType>.
  Пример такой базы данных - имеем два типа сущностей: User и Product.
  У User есть три атрибута двух типов - имя (text), адрес (text) и дата рождения (timestamp).
  У Product есть два атрибуты двух типов - название (text) и цена (numeric).
  Тогда в базе данных будут таблицы: user, user_attribute_text, user_attribute_timestamp, user_value_text, user_value_timestamp,
  product, product_attribute_text, product_attribute_numeric, product_value_text, product_value_numeric.
*/
//---
class PGExecutorEAVNamingRules : public IExecutorEAVNamingRules
{
public:
	/// Получить название таблицы сущностей
	virtual std::string GetEntityTableName(const std::string & entityName) const override;
	/// Получить название таблицы атрибутов
	virtual std::string GetAttributeTableName(const std::string & entityName,
		const std::string & attributeType) const override;
	/// Получить название таблицы значений
	virtual std::string GetValueTableName(const std::string & entityName,
		const std::string & attributeType) const override;

public:
	/// Получить название поля идентификатора таблицы сущностей
	virtual std::string GetEntityTable_Short_IdField(const std::string & entityName) const override;

public:
	/// Получить название поля идентификатора таблицы атрибутов
	virtual std::string GetAttributeTable_Short_IdField(const std::string & entityName,
		const std::string & attributeType) const override;

	/// Получить название поля названия таблицы атрибутов
	virtual std::string GetAttributeTable_Short_NameField(const std::string & entityName,
		const std::string & attributeType) const override;

public:
	/// Получить название поля идентификатора сущности таблицы значений
	virtual std::string GetValueTable_Short_EntityIdField(const std::string & entityName,
		const std::string & attributeType) const override;

	/// Получить название поля идентификатора атрибута таблицы значений
	virtual std::string GetValueTable_Short_AttributeIdField(const std::string & entityName,
		const std::string & attributeType) const override;

	/// Получить название поля значения атрибута таблицы значений
	virtual std::string GetValueTable_Short_ValueField(const std::string & entityName,
		const std::string & attributeType) const override;
};
