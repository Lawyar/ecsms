#pragma once

#include <IExecuteResult.h>
#include <IConnection.h>
#include <DataType/ISQLType.h>
#include <DataType/ISQLTypeText.h>

#include <memory>
#include <vector>
#include <map>

class IExecutorEAVNamingRules;

//------------------------------------------------------------------------------
/**
  Интерфейс исполнителя запросов EAV.
  Он предоставляет базовые запросы, которые могут оказаться полезными при работе
  с базой данных, представленной схемой EAV.

  Правила именования таблиц для базы данных определяются интерфейсом IExecutorEAVNamingRules.

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
	/// Получить зарегистрированные сущности
	virtual const EAVRegisterEntries & GetRegisteredEntities() const = 0;

	/// Получить объект, определяющий правила именования таблиц
	virtual const IExecutorEAVNamingRules & GetNamingRules() const = 0;

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
};

/// Тип указателя на исполнитель EAV-запросов
using IExecutorEAVPtr = std::shared_ptr<IExecutorEAV>;
