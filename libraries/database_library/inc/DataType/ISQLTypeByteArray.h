#pragma once

#include <DataType/ISQLType.h>

#include <optional>
#include <vector>

//------------------------------------------------------------------------------
/**
  \brief Интерфейс SQL-типа данных "bytea".
*/
//---
class ISQLTypeByteArray : public ISQLType {
public:
  /// Деструктор
  virtual ~ISQLTypeByteArray() override = default;

public:
  /// Получить тип данных
  virtual SQLDataType GetType() const override final {
    return SQLDataType::ByteArray;
  }

public:
  /// Получить значение
  /// \return Значение, содержащееся в переменной, если она непустая,
  ///         иначе \c std::nullopt.
  virtual const std::optional<std::vector<char>> &GetValue() const = 0;
  /// Установить значение
  /// \param value Массив, из которого нужно установить значение.
  ///              При успехе массив будет очищен, иначе останется неизменным.
  virtual void SetValue(std::vector<char> &&value) = 0;
};

/// Тип указателя на ISQLTypeByteArray
using ISQLTypeByteArrayPtr = std::shared_ptr<ISQLTypeByteArray>;
