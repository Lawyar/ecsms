#pragma once

#include <IExecutorEAV.h>
#include <IConnection.h>
#include <DataType/ISQLTypeConverter.h>

//------------------------------------------------------------------------------
/**
  Реализация исполнителя запросов EAV.
  todo : Возможная оптимизация - использование подготовленных операторов
  для запросов. См. PostgreSQL PREPARE; libpq : PQprepare.
*/
//---
class PGExecutorEAV : public IExecutorEAV
{
	IConnectionPtr m_connection;             ///< Соединение с БД
	ISQLTypeConverterPtr m_sqlTypeConverter; ///< Конвертер в SQL-типы
	EAVRegisterEntries m_registerEntries;    ///< Зарегистрированные сущности с типами атрибутов

public:
	/// Конструктор
	PGExecutorEAV(const IConnectionPtr & connection, const ISQLTypeConverterPtr & sqlTypeConverter);

public:
	/// Регистрация EAV-сущностей
	/// \param createTables Требуется ли пытаться создать таблицы по зарегистрированным сущностям
	virtual IExecuteResultStatusPtr RegisterEntities(const EAVRegisterEntries & entries,
		bool createTables) override;

	/// Получить название таблицы сущностей
	virtual std::string GetEntityTableName(const std::string & entityName) const override;
	/// Получить название таблицы атрибутов
	virtual std::string GetAttributeTableName(const std::string & entityName,
		const std::string & attributeType) const override;
	/// Получить название таблицы значений
	virtual std::string GetValueTableName(const std::string & entityName,
		const std::string & attributeType) const override;

	/// Получить полное название поля идентификатора таблицы сущностей
	virtual std::string GetEntityTable_Full_IdField(const std::string & entityName) const override;

	/// Получить полное название поля идентификатора таблицы атрибутов
	virtual std::string GetAttributeTable_Full_IdField(const std::string & entityName,
		const std::string & attributeType) const override;
	/// Получить полное название поля названия таблицы атрибутов
	virtual std::string GetAttributeTable_Full_NameField(const std::string & entityName,
		const std::string & attributeType) const override;

	/// Получить полное название поля идентификатора сущности таблицы значений
	virtual std::string GetValueTable_Full_EntityIdField(const std::string & entityName,
		const std::string & attributeType) const override;
	/// Получить полное название поля идентификатора атрибута таблицы значений
	virtual std::string GetValueTable_Full_AttributeIdField(const std::string & entityName,
		const std::string & attributeType) const override;
	/// Получить полное название поля значения атрибута таблицы значений
	virtual std::string GetValueTable_Full_ValueField(const std::string & entityName,
		const std::string & attributeType) const override;

private:
	/// Получить команду создания таблицы сущностей
	std::string createEntityTableCommand(const std::string & entityName) const;

	/// Получить команду создания таблицы атрибутов
	std::string createAttributeTableCommand(const std::string & entityName,
		const std::string & attributeType) const;

	/// Получить команду создания таблицы значений
	std::string createValueTableCommand(const std::string & entityName,
		const std::string & attributeType) const;

public: // Методы для создания новых сущностей и поиска уже существующих сущностей
	/// Создать новую сущность указанного типа
	virtual IExecuteResultStatusPtr CreateNewEntity(const EntityName & entityName, EntityId & entityId) override;
	/// Найти сущности, у которых есть все из указанных пар атрибут-значение
	virtual IExecuteResultStatusPtr FindEntitiesByAttrValues(const EntityName & entityName,
		const std::vector<AttrValue> & attrValues, std::vector<EntityId> & entityIds) override;

private:
	/// Получить команду "добавить сущность в таблицу сущностей и вернуть вставленный идентификатор"
	std::string insertNewEntityReturningIdCommand(const std::string & entityName) const;
	/// Получить часть команды "получить идентификаторы сущности по названию атрибута и его значению"
	std::optional<std::string> getEntityIdByAttrValuePartCommand(const EntityName & entityName,
		const AttrValue & attrValue) const;

public: // Методы для вставки/обновления данных
	/// Вставить значение для атрибута сущности
	virtual IExecuteResultStatusPtr Insert(const EntityName & entityName, EntityId entityId,
		const AttrName & attrName, const ValueType & value) override;
	/// Обновить значение для атрибута сущности
	virtual IExecuteResultStatusPtr Update(const EntityName & entityName, EntityId entityId,
		const AttrName & attrName, const ValueType & value) override;
	/// Обновить значение для атрибута сущности или вставить, если такого значения ещё не было
	virtual IExecuteResultStatusPtr InsertOrUpdate(const EntityName & entityName, EntityId entityId,
		const AttrName & attrName, const ValueType & value) override;

private:
	/// Получить команду "вставить атрибут в таблицу атрибутов, при конфликте ничего не делать"
	std::string insertAttributeOnConflictDoNothingCommand(const EntityName & entityName, const std::string & attributeType,
		const std::string & sqlAttrName) const;
	/// Получить часть команды "вставить значение в таблицу значений"
	std::optional<std::string> insertValuePartCommand(const EntityName & entityName, EntityId entityId,
		const std::string & sqlAttrName, const ValueType & value) const;
	/// Получить команду "вставить значение в таблицу значений"
	std::optional<std::string> insertValueCommand(const EntityName & entityName, EntityId entityId,
		const std::string & sqlAttrName, const ValueType & value) const;
	/// Получить команду "вставить значение в таблицу значений, при конфликте сделать обновление"
	std::optional<std::string> insertValueOnConflictDoUpdateCommand(const EntityName & entityName,
		EntityId entityId, const std::string & sqlAttrName, const ValueType & value) const;
	/// Получить команду "обновить значение в таблице значений"
	std::optional<std::string> updateValueCommand(const EntityName & entityName, EntityId entityId,
		const std::string & sqlAttrName, const ValueType & value) const;
	/// Получить внутреннюю команду "получить идентификатор атрибута по его названию"
	std::string selectAttributeIdByNameInnerCommand(const EntityName & entityName,
		const std::string & attributeType, const std::string & sqlAttrName) const;

public: // Методы для получения данных
	/// Получить значение атрибута сущности.
	/// Переменная value - должна быть пустой (сконструированной конструктором по умолчанию)
	/// переменной соответствующего типа.
	/// Например, если мы хотим получить значение текстового аттрибута, то нужно передать
	/// ненулевой указатель ISQLTypeTextPtr.
	/// Результат метода запишется в value.
	virtual IExecuteResultStatusPtr GetValue(const EntityName & entityName, EntityId entityId,
		const AttrName & attrName, ValueType value) override;
	/// Получить значения всех атрибутов сущности.
	virtual IExecuteResultStatusPtr GetAttributeValues(const EntityName & entityName,
		EntityId entityId, std::vector<AttrValue> & attrValues) override;

private:
	/// Получить команду "получить значение по идентификатору сущности и названию атрибута"
	std::string selectValueByEntityIdAndAttributeNameCommand(const EntityName & entityName,
		EntityId entityId, const std::string & attributeType, const std::string & sqlAttrName) const;
	/// Получить команду "получить названия атрибутов и их значения"
	std::string selectAttrValuesCommand(const EntityName & entityName, EntityId entityId,
		const std::string & attributeType) const;

	/// Получить значения атрибутов сущности из результата.
	IExecuteResultStatusPtr getAttributeValuesImpl(const IExecuteResultPtr & result,
		std::vector<AttrValue> & attrValues) const;

private: // Общие вспомогательные методы
	/// Выполнить команду.
	bool executeQuery(const std::string query, IExecuteResultPtr & result, IExecuteResultStatusPtr & status);
	/// Прочитать строку в SQL-переменную
	template <class SQLConcreteType, class CppConcreteType>
	bool readIntoSQLVariable(std::string && str, SQLDataType type, CppConcreteType & value,
		IExecuteResultStatusPtr & status) const;
	/// Получить название атрибута в форме, пригодной для вставления в запрос
	IExecuteResultStatusPtr getSQLAttrName(const AttrName & attrName, std::string & sqlAttrName) const;
	/// Получить название типа SQL
	IExecuteResultStatusPtr getSQLTypeName(SQLDataType sqlDataType, std::string & sqlTypeName) const;
};
