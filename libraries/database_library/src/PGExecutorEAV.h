#pragma once

#include <DataType/ISQLTypeConverter.h>
#include <IConnection.h>
#include <IExecutorEAV.h>
#include <PGExecutorEAVNamingRules.h>

//------------------------------------------------------------------------------
/**
  Реализация исполнителя запросов EAV.
  todo : Возможная оптимизация - использование подготовленных операторов
  для запросов. См. PostgreSQL PREPARE; libpq : PQprepare.
*/
//---
class PGExecutorEAV : public IExecutorEAV {
  IConnectionPtr m_connection;             ///< Соединение с БД
  ISQLTypeConverterPtr m_sqlTypeConverter; ///< Конвертер в SQL-типы
  EAVRegisterEntries
      m_registerEntries; ///< Зарегистрированные сущности с типами атрибутов
  const PGExecutorEAVNamingRules m_rules; ///< Правила именования таблиц

public:
  /// Конструктор
  PGExecutorEAV(const IConnectionPtr &connection,
                const ISQLTypeConverterPtr &sqlTypeConverter);

public:
  /// Регистрация EAV-сущностей для дальнешей работы с классом
  virtual IExecuteResultStatusPtr
  SetRegisteredEntities(const EAVRegisterEntries &entries,
                        bool createTables) override;
  /// Получить зарегистрированные сущности
  virtual const EAVRegisterEntries &GetRegisteredEntities() const override;

public:
  /// Получить объект, определяющий правила именования таблиц
  virtual const IExecutorEAVNamingRules &GetNamingRules() const override;

private:
  /// Получить команду создания таблицы сущностей
  std::string createEntityTableCommand(const std::string &entityName) const;

  /// Получить команду создания таблицы атрибутов
  std::string
  createAttributeTableCommand(const std::string &entityName,
                              const std::string &attributeType) const;

  /// Получить команду создания таблицы значений
  std::string createValueTableCommand(const std::string &entityName,
                                      const std::string &attributeType) const;

public
    : // Методы для создания новых сущностей и поиска уже существующих сущностей
  /// Создать новую сущность данного вида
  virtual IExecuteResultStatusPtr CreateNewEntity(const EntityName &entityName,
                                                  EntityId &result) override;
  /// Получить все идентификаторы сущности данного вида
  virtual IExecuteResultStatusPtr
  GetEntityIds(const EntityName &entityName,
               std::vector<EntityId> &result) override;
  /// Получить все наименования атрибутов указанного типа, которые использует
  /// данная сущность
  virtual IExecuteResultStatusPtr
  GetAttributeNames(const EntityName &entityName, SQLDataType sqlDataType,
                    std::vector<AttrName> &result) override;
  /// Найти сущности, у которых есть все из указанных пар атрибут-значение
  virtual IExecuteResultStatusPtr
  FindEntitiesByAttrValues(const EntityName &entityName,
                           const std::vector<AttrValue> &attrValues,
                           std::vector<EntityId> &result) override;

private:
  /// Получить команду "добавить сущность в таблицу сущностей и вернуть
  /// вставленный идентификатор"
  std::string
  insertNewEntityReturningIdCommand(const std::string &entityName) const;
  /// Получить команду "получить идентификаторы сущности"
  std::string getEntityIdsCommand(const std::string &entityName) const;
  /// Получить команду "получить названия атрибутов указанного типа, которые
  /// использует данная сущность"
  std::string getAttributeNamesCommand(const std::string &entityName,
                                       const std::string &attributeType) const;
  /// Получить внутреннюю команду "получить идентификаторы сущности по названию
  /// атрибута и его значению"
  std::optional<std::string>
  getEntityIdByAttrValueInnerCommand(const EntityName &entityName,
                                     const AttrValue &attrValue) const;

public: // Методы для вставки/обновления данных
  /// Вставить значение для атрибута сущности
  virtual IExecuteResultStatusPtr Insert(const EntityName &entityName,
                                         EntityId entityId,
                                         const AttrName &attrName,
                                         const ValueType &value) override;
  /// Обновить значение для атрибута сущности
  virtual IExecuteResultStatusPtr Update(const EntityName &entityName,
                                         EntityId entityId,
                                         const AttrName &attrName,
                                         const ValueType &value) override;
  /// Обновить значение для атрибута сущности или вставить, если такого значения
  /// ещё не было
  virtual IExecuteResultStatusPtr
  InsertOrUpdate(const EntityName &entityName, EntityId entityId,
                 const AttrName &attrName, const ValueType &value) override;

private:
  /// Получить команду "вставить атрибут в таблицу атрибутов, при конфликте
  /// ничего не делать"
  std::string insertAttributeOnConflictDoNothingCommand(
      const EntityName &entityName, const std::string &attributeType,
      const std::string &sqlAttrName) const;
  /// Получить часть команды "вставить значение в таблицу значений"
  std::string insertValuePartCommand(const EntityName &entityName,
                                     EntityId entityId,
                                     const std::string &sqlAttrName,
                                     const std::string &attributeType,
                                     const std::string &sqlValue) const;
  /// Получить команду "вставить значение в таблицу значений"
  std::string insertValueCommand(const EntityName &entityName,
                                 EntityId entityId,
                                 const std::string &sqlAttrName,
                                 const std::string &attributeType,
                                 const std::string &sqlValue) const;
  /// Получить команду "вставить значение в таблицу значений, при конфликте
  /// сделать обновление"
  std::string insertValueOnConflictDoUpdateCommand(
      const EntityName &entityName, EntityId entityId,
      const std::string &sqlAttrName, const std::string &attributeType,
      const std::string &sqlValue) const;
  /// Получить команду "обновить значение в таблице значений"
  std::string updateValueCommand(const EntityName &entityName,
                                 EntityId entityId,
                                 const std::string &sqlAttrName,
                                 const std::string &attributeType,
                                 const std::string &sqlValue) const;
  /// Получить команду "удалить значение в таблице значений"
  std::string removeValueCommand(const EntityName &entityName,
                                 EntityId entityId,
                                 const std::string &attributeType,
                                 const std::string &sqlAttrName) const;
  /// Получить команду "Бросить ошибку, если в таблице сущностей нет сущности с
  /// данным идентификатором"
  std::string
  throwErrorIfThereIsNoEntityWithSuchIdCommand(const EntityName &entityName,
                                               EntityId entityId) const;
  /// Получить команду "Бросить ошибку, если в таблице атрибутов нет атрибута с
  /// данным именем"
  std::string throwErrorIfThereIsNoAttributeWithSuchNameCommand(
      const EntityName &entityName, const std::string &attributeType,
      const std::string &sqlAttrName) const;

  /// Получить внутреннюю команду "получить идентификатор атрибута по его
  /// названию"
  std::string
  selectAttributeIdByNameInnerCommand(const EntityName &entityName,
                                      const std::string &attributeType,
                                      const std::string &sqlAttrName) const;

public: // Методы для получения данных
  /// Получить значение атрибута сущности.
  virtual IExecuteResultStatusPtr GetValue(const EntityName &entityName,
                                           EntityId entityId,
                                           const AttrName &attrName,
                                           ValueType value) override;
  /// Получить значения всех атрибутов сущности.
  virtual IExecuteResultStatusPtr GetAttributeValues(
      const EntityName &entityName, EntityId entityId,
      std::map<SQLDataType, std::vector<AttrValue>> &attrValuesByType) override;

private:
  /// Получить команду "получить значение по идентификатору сущности и названию
  /// атрибута"
  std::string selectValueByEntityIdAndAttributeNameCommand(
      const EntityName &entityName, EntityId entityId,
      const std::string &attributeType, const std::string &sqlAttrName) const;
  /// Получить команду "получить названия атрибутов и их значения"
  std::string selectAttrValuesCommand(const EntityName &entityName,
                                      EntityId entityId,
                                      const std::string &attributeType) const;

  /// Получить значения атрибутов сущности из результата.
  IExecuteResultStatusPtr
  getAttributeValuesImpl(const IExecuteResultPtr &result,
                         std::vector<AttrValue> &attrValues) const;

private: // Общие вспомогательные методы
  /// Выполнить команду.
  bool executeQuery(const std::string query, IExecuteResultPtr &result,
                    IExecuteResultStatusPtr &status);
  /// Прочитать строку в SQL-переменную
  template <class SQLConcreteType, class CppConcreteType>
  bool readIntoSQLVariable(std::string &&str, SQLDataType type,
                           CppConcreteType &value,
                           IExecuteResultStatusPtr &status) const;
  /// Получить название атрибута в форме, пригодной для вставления в запрос
  IExecuteResultStatusPtr getSQLAttrName(const AttrName &attrName,
                                         std::string &sqlAttrName) const;
  /// Получить название типа SQL
  IExecuteResultStatusPtr getSQLTypeName(SQLDataType sqlDataType,
                                         std::string &sqlTypeName) const;

  /// Проверка на то, что сущность зарегистрирована, и с ней ассоциирован тип
  /// данных
  IExecuteResultStatusPtr checkEntityWithDataTypeError(
      const std::string &entityName,
      std::optional<SQLDataType> sqlDataType = std::nullopt) const;
};
