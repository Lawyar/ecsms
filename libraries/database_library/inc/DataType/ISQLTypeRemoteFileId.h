#pragma once

#include <DataType/ISQLType.h>

//------------------------------------------------------------------------------
/**
  Интерфейс SQL-переменной, представляющий идентификатор удаленного файла на
  сервере
*/
//---
class ISQLTypeRemoteFileId : public ISQLType {
public:
  /// Деструктор
  virtual ~ISQLTypeRemoteFileId() override = default;

public:
  /// Получить тип данных
  virtual SQLDataType GetType() const override final {
    return SQLDataType::RemoteFileId;
  }

public:
  /// Получить идентификатор удаленного файла
  virtual const std::optional<std::string> &GetValue() const = 0;
};

/// Указатель на ISQLTypeRemoteFileId
using ISQLTypeRemoteFileIdPtr = std::shared_ptr<ISQLTypeRemoteFileId>;
