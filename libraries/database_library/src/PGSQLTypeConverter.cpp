#include "PGSQLTypeConverter.h"

#include <PGSQLTypeByteArray.h>
#include <PGSQLTypeInteger.h>
#include <PGSQLTypeRemoteFileId.h>
#include <PGSQLTypeText.h>

#include <cassert>

//------------------------------------------------------------------------------
/**
  Получить пустую SQL-переменную
*/
//---
ISQLTypePtr PGSQLTypeConverter::GetSQLVariable(SQLDataType type) const {
  ISQLTypePtr result;
  switch (type) {
  case SQLDataType::Integer:
    result = GetSQLTypeInteger();
    break;
  case SQLDataType::Text:
    result = GetSQLTypeText();
    break;
  case SQLDataType::ByteArray:
    result = GetSQLTypeByteArray();
    break;
  case SQLDataType::RemoteFileId:
    result = GetSQLTypeRemoteFileId();
    break;
  case SQLDataType::Unknown:
    // Нельзя получить неизвестный тип данных, вернем nullptr
    break;
  case SQLDataType::Invalid:
    // Ок, вернем nullptr
    break;
  default:
    // Неизвестный тип данных
    assert(false);
    break;
  }

  return result;
}

//------------------------------------------------------------------------------
/**
  Получить пустую SQL-Integer переменную
*/
//---
ISQLTypeIntegerPtr PGSQLTypeConverter::GetSQLTypeInteger(
    const std::optional<StrongType<int>> &value) const {
  return value.has_value() ? std::make_shared<PGSQLTypeInteger>(value->value)
                           : std::make_shared<PGSQLTypeInteger>();
}

//------------------------------------------------------------------------------
/**
  Получить пустую SQL-Text переменную
*/
//---
ISQLTypeTextPtr PGSQLTypeConverter::GetSQLTypeText() const {
  return std::make_shared<PGSQLTypeText>();
}

//------------------------------------------------------------------------------
/**
  Получить пустую SQL-Text переменную по строке
*/
//---
ISQLTypeTextPtr PGSQLTypeConverter::GetSQLTypeText(std::string &&value) const {
  return std::make_shared<PGSQLTypeText>(std::move(value));
}

//------------------------------------------------------------------------------
/**
  Получить пустую SQL-ByteArray переменную
*/
//---
ISQLTypeByteArrayPtr PGSQLTypeConverter::GetSQLTypeByteArray() const {
  return std::make_shared<PGSQLTypeByteArray>();
}

//------------------------------------------------------------------------------
/**
  Получить пустую SQL-ByteArray переменную по массиву байт
*/
//---
ISQLTypeByteArrayPtr
PGSQLTypeConverter::GetSQLTypeByteArray(std::vector<char> &&value) const {
  return std::make_shared<PGSQLTypeByteArray>(std::move(value));
}

//------------------------------------------------------------------------------
/**
  Получить SQL-RemoteFileId переменную
*/
//---
ISQLTypeRemoteFileIdPtr PGSQLTypeConverter::GetSQLTypeRemoteFileId() const {
  return std::make_shared<PGSQLTypeRemoteFileId>();
}

//------------------------------------------------------------------------------
/**
  Получить SQL-RemoteFileId переменную
*/
//---
ISQLTypeRemoteFileIdPtr
PGSQLTypeConverter::GetSQLTypeRemoteFileId(const std::string &id) const {
  return std::make_shared<PGSQLTypeRemoteFileId>(id);
}
