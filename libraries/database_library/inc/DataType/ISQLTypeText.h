#pragma once

#include <DataType/ISQLType.h>

#include <optional>
#include <string>

//------------------------------------------------------------------------------
/**
  \brief Интерфейс SQL-типа данных "text"
*/
//---
class ISQLTypeText : public ISQLType {
public:
  /// Деструктор
  virtual ~ISQLTypeText() override = default;

public:
  /// Получить тип данных
  virtual SQLDataType GetType() const override final {
    return SQLDataType::Text;
  }

public:
  /// Получить значение
  /// \return Значение, содержащееся в переменной, если она непустая,
  ///         иначе \c std::nullopt.
  virtual const std::optional<std::string> &GetValue() const = 0;
  /// Установить значение
  /// \param value Строка, из которой нужно установить значение.
  ///              При успехе строка будет очищена, иначе останется неизменной.
  virtual void SetValue(std::string &&value) = 0;
};

/// Тип указателя на ISQLTypeText
using ISQLTypeTextPtr = std::shared_ptr<ISQLTypeText>;
