#include "PGSQLDataTypeUtility.h"

#include <server/catalog/pg_type_d.h>

#include <unordered_map>
#include <vector>

//------------------------------------------------------------------------------
/**
  Вспомогательный класс для конвертации внутренних типов данных во внешние и
  обратно При добавлении новых типов данных требуется менять только конструктор,
  добавляя туда новые пары соответствий.
*/
//---
class TypeConverter {
private:
  /// Структура, хранящяя соответствие между внутренним и внешним типами данных
  struct IdMatch {
    SQLDataType type; ///< Внутренний тип данных
    Oid id;           ///< Внешний тип данных
  };

public:
  /// Тип мапы соответствий внутренних типов данных к внешним
  using SQLDataTypeToOidMap = std::unordered_map<SQLDataType, Oid>;
  /// Тип мапы соответствий внешних типов данных к внутренним
  using OidToSQLDataTypeMap = std::unordered_map<Oid, SQLDataType>;

private:
  /// Массив соответствий между внутренними и внешними типами данных
  const std::vector<IdMatch>
      m_idMatches; ///< Список соответствий между внутренним и внешним типами
                   ///< данных
  const SQLDataTypeToOidMap
      m_SQLDataTypeToOidMap; ///< Мапа соответствий внутренних типов данных к
                             ///< внешним
  const OidToSQLDataTypeMap
      m_oidToSQLDataTypeMap; ///< Мапа соответствий внешних типов данных к
                             ///< внутренним

public:
  /// Конструктор
  TypeConverter()
      : m_idMatches({{SQLDataType::Integer, INT4OID},
                     {SQLDataType::Text, TEXTOID},
                     {SQLDataType::ByteArray, BYTEAOID},
                     {SQLDataType::RemoteFileId, OIDOID},
                     {SQLDataType::Invalid, InvalidOid}}),
        m_SQLDataTypeToOidMap(createSQLDataTypeToOidMap(m_idMatches)),
        m_oidToSQLDataTypeMap(createOidToSQLDataTypeMap(m_idMatches)) {}

  /// Получить мапу соответствий внутренних типов данных к внешним
  const SQLDataTypeToOidMap &GetSQLDataTypeToOidMap() const {
    return m_SQLDataTypeToOidMap;
  }

  /// Получить мапу соответствий внешних типов данных к внутренним
  const OidToSQLDataTypeMap &GetOidToSQLDataTypeMap() const {
    return m_oidToSQLDataTypeMap;
  }

private:
  /// Создать мапу соответствий внутренних типов данных к внешним
  static SQLDataTypeToOidMap
  createSQLDataTypeToOidMap(const std::vector<IdMatch> &idMatches) {
    SQLDataTypeToOidMap map;
    for (auto &&idMatch : idMatches)
      map.emplace(idMatch.type, idMatch.id);
    return map;
  }

  /// Создать мапу соответствий внешних типов данных к внутренним
  static OidToSQLDataTypeMap
  createOidToSQLDataTypeMap(const std::vector<IdMatch> &idMatches) {
    OidToSQLDataTypeMap map;
    for (auto &&idMatch : idMatches)
      map.emplace(idMatch.id, idMatch.type);
    return map;
  }
};

static const TypeConverter
    Converter; ///< Конвертер внешних типов данных во внутренние и обратно

//------------------------------------------------------------------------------
/**
  Перевести тип данных в представление, в котором он хранится в PostgreSQL
*/
//---
Oid SQLDataTypeToOid(SQLDataType type) {
  auto &&map = Converter.GetSQLDataTypeToOidMap();
  if (auto iter = map.find(type); iter != map.end())
    return iter->second;
  return InvalidOid;
}

//------------------------------------------------------------------------------
/**
  Перевести тип данных в представление, в котором он хранится в PostgreSQL
*/
//---
SQLDataType OidToSQLDataType(Oid id) {
  auto &&map = Converter.GetOidToSQLDataTypeMap();
  if (auto iter = map.find(id); iter != map.end())
    return iter->second;
  return SQLDataType::Unknown;
}
