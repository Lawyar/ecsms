#pragma once

#include <DataType/ISQLType.h>
#include <DataType/ISQLTypeByteArray.h>
#include <DataType/ISQLTypeInteger.h>
#include <DataType/ISQLTypeRemoteFileId.h>
#include <DataType/ISQLTypeText.h>

#include <Utils/StrongType.h>

#include <optional>
#include <string>
#include <vector>

//------------------------------------------------------------------------------
/**
  Интерфейс конвертера в SQL-тип данных
*/
//---
class ISQLTypeConverter {
public:
  /// Деструктор
  virtual ~ISQLTypeConverter() = default;

public: // Получение SQL-переменных
  /// Получить пустую SQL-переменную
  virtual ISQLTypePtr GetSQLVariable(SQLDataType type) const = 0;

  /// Получить SQL-Integer переменную
  virtual ISQLTypeIntegerPtr GetSQLTypeInteger(
      const std::optional<StrongType<int>> &value = std::nullopt) const = 0;

  /// Получить SQL-Text переменную
  virtual ISQLTypeTextPtr GetSQLTypeText() const = 0;
  /// Получить SQL-Text переменную по строке
  /// (поскольку копирование может быть затратным и ненужным, используется
  /// перемещение)
  virtual ISQLTypeTextPtr GetSQLTypeText(std::string &&value) const = 0;

  /// Получить SQL-ByteArray переменную
  virtual ISQLTypeByteArrayPtr GetSQLTypeByteArray() const = 0;
  /// Получить SQL-ByteArray переменную по массиву байт
  /// (поскольку копирование может быть затратным и ненужным, используется
  /// перемещение)
  virtual ISQLTypeByteArrayPtr
  GetSQLTypeByteArray(std::vector<char> &&value) const = 0;

  /// Получить SQL-RemoteFileId переменную
  virtual ISQLTypeRemoteFileIdPtr GetSQLTypeRemoteFileId() const = 0;
  /// Получить SQL-RemoteFileId переменную
  virtual ISQLTypeRemoteFileIdPtr
  GetSQLTypeRemoteFileId(const std::string &id) const = 0;
};

/// Тип указателя на ISQLTypeConverter
using ISQLTypeConverterPtr = std::shared_ptr<ISQLTypeConverter>;
