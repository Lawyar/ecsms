#pragma once

#include <IExecuteResult.h>
#include <IConnection.h>
#include <DataType/ISQLType.h>
#include <DataType/ISQLTypeText.h>

#include <cstdint>
#include <memory>
#include <vector>
#include <map>

//------------------------------------------------------------------------------
/**
  Интерфейс исполнителя запросов EAV.
  Он предоставляет базовые запросы, которые могут оказаться полезными при работе
  с базой данных, представленной схемой EAV.
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

  Метод RegisterEntities может создать таблицы по указанным правилам, если передать в него
  флаг createTables = true.
  Остальные методы не создают таблицы, а пользуются существующими, созданными этим методом.
  При отсутствии таких таблиц команды будут выдавать ошибку.
*/
//---
class IExecutorEAV
{
public:
	/// Тип для названия сущности (осмысленное название сущности).
	using EntityName = std::string;
	/// Тип для идентификатора сущности (числовой идентификатор конкретной сущности в её таблице сущностей).
	using EntityId = int;
	/// Тип для названия атрибута.
	using AttrName = ISQLTypeTextPtr;
	/// Тип значения атрибута
	using ValueType = ISQLTypePtr;

	/// Структура для хранения пары: атрибут и его значение
	struct AttrValue
	{
		AttrName attrName; ///< Название атрибута
		ValueType value; ///< Значение атрибута
	};

public:
	/// Названия полей в таблице сущностей
	struct EntityTable
	{
		static constexpr char * c_idField = "id"; ///< Название поля для идентификатора сущности в таблице сущностей
	};

	/// Названия полей в таблице атрибутов
	struct AttributeTable
	{
		static constexpr char * c_idField = "id"; ///< Название поля для идентификатора атрибута в таблице атрибутов
		static constexpr char * c_nameField = "name"; ///< Название поля для названия атрибута в таблице атрибутов
	};

	/// Название полей в таблице значений
	struct ValueTable
	{
		static constexpr char * c_entityIdField = "entity_id"; ///< Название поля для идентификатора сущности в таблице значений
		static constexpr char * c_attributeIdField = "attribute_id"; ///< Название поля для идентификатора атрибута в таблице значений
		static constexpr char * c_valueField = "value"; ///< Название поля для значения в таблице значений
	};

	/// Запись в реестре EAV
	using EAVRegisterEntries = std::map<
		EntityName, // Название сущности
		std::vector<SQLDataType>>; // Типы атрибутов, которые она будет использовать

public:
	/// Деструктор
	virtual ~IExecutorEAV() = default;

public:
	/// Регистрация EAV-сущностей
	/// \param createTables Требуется ли пытаться создать таблицы по зарегистрированным сущностям
	virtual IExecuteResultStatusPtr RegisterEntities(const EAVRegisterEntries & entries,
		bool createTables) = 0;

	/// Получить название таблицы сущностей
	virtual std::string GetEntityTableName(const std::string & entityName) const = 0;
	/// Получить название таблицы атрибутов
	virtual std::string GetAttributeTableName(const std::string & entityName,
		const std::string & attributeType) const = 0;
	/// Получить название таблицы значений
	virtual std::string GetValueTableName(const std::string & entityName,
		const std::string & attributeType) const = 0;

	/// Получить полное название поля идентификатора таблицы сущностей
	virtual std::string GetEntityTable_Full_IdField(const std::string & entityName) const = 0;

	/// Получить полное название поля идентификатора таблицы атрибутов
	virtual std::string GetAttributeTable_Full_IdField(const std::string & entityName,
		const std::string & attributeType) const = 0;
	/// Получить полное название поля названия таблицы атрибутов
	virtual std::string GetAttributeTable_Full_NameField(const std::string & entityName,
		const std::string & attributeType) const = 0;

	/// Получить полное название поля идентификатора сущности таблицы значений
	virtual std::string GetValueTable_Full_EntityIdField(const std::string & entityName,
		const std::string & attributeType) const = 0;
	/// Получить полное название поля идентификатора атрибута таблицы значений
	virtual std::string GetValueTable_Full_AttributeIdField(const std::string & entityName,
		const std::string & attributeType) const = 0;
	/// Получить полное название поля значения атрибута таблицы значений
	virtual std::string GetValueTable_Full_ValueField(const std::string & entityName,
		const std::string & attributeType) const = 0;

public: // Методы для создания новых сущностей и поиска уже существующих сущностей
	/// Создать новую сущность указанного типа
	virtual IExecuteResultStatusPtr CreateNewEntity(const EntityName & entityName, EntityId & result) = 0;
	/// Найти сущности, у которых есть все из указанных пар атрибут-значение
	virtual IExecuteResultStatusPtr FindEntitiesByAttrValues(const EntityName & entityName,
		const std::vector<AttrValue> & attrValues, std::vector<EntityId> & result) = 0;

public: // Методы для вставки/обновления данных
	/// Вставить значение для атрибута сущности
	virtual IExecuteResultStatusPtr Insert(const EntityName & entityName, EntityId entityId,
		const AttrName & attrName, const ValueType & value) = 0;
	/// Обновить значение для атрибута сущности
	virtual IExecuteResultStatusPtr Update(const EntityName & entityName, EntityId entityId,
		const AttrName & attrName, const ValueType & value) = 0;
	/// Обновить значение для атрибута сущности или вставить, если такого значения ещё не было
	virtual IExecuteResultStatusPtr InsertOrUpdate(const EntityName & entityName, EntityId entityId,
		const AttrName & attrName, const ValueType & value) = 0;

public: // Методы для получения данных
	/// Получить значение атрибута сущности.
	/// Переменная value - должна быть пустой (сконструированной конструктором по умолчанию)
	/// переменной соответствующего типа.
	/// Например, если мы хотим получить значение текстового аттрибута, то нужно передать
	/// ненулевой указатель ISQLTypeTextPtr.
	/// Результат метода запишется в value.
	virtual IExecuteResultStatusPtr GetValue(const EntityName & entityName, EntityId entityId,
		const AttrName & attrName, ValueType value) = 0;
	/// Получить значения всех атрибутов сущности.
	virtual IExecuteResultStatusPtr GetAttributeValues(const EntityName & entityName,
		EntityId entityId, std::vector<AttrValue> & attrValues) = 0;
	/// Получить все записи обо всех сущностях
	virtual IExecuteResultPtr GetEntries(const EntityName & entityName) = 0;
};

/// Тип указателя на исполнитель EAV-запросов
using IExecutorEAVPtr = std::shared_ptr<IExecutorEAV>;
