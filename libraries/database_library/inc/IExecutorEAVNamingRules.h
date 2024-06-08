#pragma once

#include <string>

//------------------------------------------------------------------------------
/**
  \brief Интерфейс, определяющий правила именования таблиц для IExecutorEAV.
*/
//---
class IExecutorEAVNamingRules {
public:
  /// Деструктор
  virtual ~IExecutorEAVNamingRules() = default;

public:
  /// Получить название таблицы сущностей
  /// \param entityName Название сущности, для которой хотим получить название
  ///                   таблицы сущностей
  virtual std::string
  GetEntityTableName(const std::string &entityName) const = 0;
  /// Получить название таблицы атрибутов
  /// \param entityName Название сущности, для которой хотим получить название
  ///                   таблицы атрибутов
  /// \param attributeType SQL-тип атрибута
  virtual std::string
  GetAttributeTableName(const std::string &entityName,
                        const std::string &attributeType) const = 0;
  /// Получить название таблицы значений
  /// \param entityName Название сущности, для которой хотим получить название
  ///                   таблицы значений
  /// \param attributeType SQL-тип атрибута
  virtual std::string
  GetValueTableName(const std::string &entityName,
                    const std::string &attributeType) const = 0;

public:
  /// Получить название поля идентификатора таблицы сущностей
  /// \param entityName Название сущности, для которой хотим получить название
  ///                   таблицы поля идентификатора таблицы сущностей
  virtual std::string
  GetEntityTable_Short_IdField(const std::string &entityName) const = 0;
  /// Получить полное название поля идентификатора таблицы сущностей, включающее
  /// название таблицы
  /// \param entityName Название сущности, для которой хотим получить полное
  ///                   название поля идентификатора таблицы сущностей
  std::string GetEntityTable_Full_IdField(const std::string &entityName) const {
    return GetEntityTable_Short_IdField(entityName) + "." +
           GetEntityTableName(entityName);
  }

public:
  /// Получить название поля идентификатора таблицы атрибутов
  /// \param entityName Название сущности, для которой хотим получить название
  ///                   поля идентификатора таблицы атрибутов
  /// \param attributeType SQL-тип атрибута
  virtual std::string
  GetAttributeTable_Short_IdField(const std::string &entityName,
                                  const std::string &attributeType) const = 0;
  /// Получить полное название поля идентификатора таблицы атрибутов, включающее
  /// название таблицы атрибутов
  /// \param entityName Название сущности, для которой хотим получить полное
  ///                   название поля идентификатора таблицы атрибутов
  /// \param attributeType SQL-тип атрибута
  std::string
  GetAttributeTable_Full_IdField(const std::string &entityName,
                                 const std::string &attributeType) const {
    return GetAttributeTableName(entityName, attributeType) + "." +
           GetAttributeTable_Short_IdField(entityName, attributeType);
  }

  /// Получить название поля названия таблицы атрибутов
  /// \param entityName Название сущности, для которой хотим получить название
  ///                   поля названия таблицы атрибутов
  /// \param attributeType SQL-тип атрибута
  virtual std::string
  GetAttributeTable_Short_NameField(const std::string &entityName,
                                    const std::string &attributeType) const = 0;
  /// Получить полное название поля названия таблицы атрибутов, включающее
  /// название таблицы атрибутов
  /// \param entityName Название сущности, для которой хотим получить полное
  ///                   название поля названия таблицы атрибутов
  /// \param attributeType SQL-тип атрибута
  std::string
  GetAttributeTable_Full_NameField(const std::string &entityName,
                                   const std::string &attributeType) const {
    return GetAttributeTableName(entityName, attributeType) + "." +
           GetAttributeTable_Short_NameField(entityName, attributeType);
  }

public:
  /// Получить название поля идентификатора сущности таблицы значений
  /// \param entityName Название сущности, для которой хотим получить название
  ///                   поля идентификатора сущности таблицы значений
  /// \param attributeType SQL-тип атрибута
  virtual std::string
  GetValueTable_Short_EntityIdField(const std::string &entityName,
                                    const std::string &attributeType) const = 0;
  /// Получить полное название поля идентификатора сущности таблицы значений,
  /// включающее название таблицы значений
  /// \param entityName Название сущности, для которой хотим получить полное
  ///                   название поля идентификатора сущности таблицы значений
  /// \param attributeType SQL-тип атрибута
  std::string
  GetValueTable_Full_EntityIdField(const std::string &entityName,
                                   const std::string &attributeType) const {
    return GetValueTableName(entityName, attributeType) + "." +
           GetValueTable_Short_EntityIdField(entityName, attributeType);
  }

  /// Получить название поля идентификатора атрибута таблицы значений
  /// \param entityName Название сущности, для которой хотим получить название
  ///                   поля идентификатора атрибута таблицы значений
  /// \param attributeType SQL-тип атрибута
  virtual std::string GetValueTable_Short_AttributeIdField(
      const std::string &entityName,
      const std::string &attributeType) const = 0;
  /// Получить полное название поля идентификатора атрибута таблицы значений,
  /// включающее название таблицы значений
  /// \param entityName Название сущности, для которой хотим получить полное
  ///                   название поля идентификатора атрибута таблицы значений
  /// \param attributeType SQL-тип атрибута
  std::string
  GetValueTable_Full_AttributeIdField(const std::string &entityName,
                                      const std::string &attributeType) const {
    return GetValueTableName(entityName, attributeType) + "." +
           GetValueTable_Short_AttributeIdField(entityName, attributeType);
  }

  /// Получить название поля значения атрибута таблицы значений
  /// \param entityName Название сущности, для которой хотим получить название
  ///                   поля значения атрибута таблицы значений
  /// \param attributeType SQL-тип атрибута
  virtual std::string
  GetValueTable_Short_ValueField(const std::string &entityName,
                                 const std::string &attributeType) const = 0;
  /// Получить полное название поля значения атрибута таблицы значений,
  /// включающее название таблицы значений
  /// \param entityName Название сущности, для которой хотим получить полное
  ///                   название поля значения атрибута таблицы значений
  /// \param attributeType SQL-тип атрибута
  std::string
  GetValueTable_Full_ValueField(const std::string &entityName,
                                const std::string &attributeType) const {
    return GetValueTableName(entityName, attributeType) + "." +
           GetValueTable_Short_ValueField(entityName, attributeType);
  }
};
