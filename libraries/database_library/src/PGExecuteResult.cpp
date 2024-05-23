#include "PGExecuteResult.h"

#include <PGExecuteResultStatus.h>
#include <PGSQLDataTypeUtility.h>

#include <cstring>

//------------------------------------------------------------------------------
/**
  Конструктор
*/
//---
PGExecuteResult::PGExecuteResult(PGresult *result) : m_result(result) {}

//------------------------------------------------------------------------------
/**
  Деструктор
*/
//---
PGExecuteResult::~PGExecuteResult() {
  PQclear(m_result);
  m_result = nullptr;
}

//------------------------------------------------------------------------------
/**
  Получить текущий статус запроса
*/
//---
IExecuteResultStatusPtr PGExecuteResult::GetCurrentExecuteStatus() const {
  IExecuteResultStatusPtr status(new PGExecuteResultStatus(m_result));
  return status;
}

//------------------------------------------------------------------------------
/**
  Получить количество строк в ответе
*/
//---
size_t PGExecuteResult::GetRowCount() const {
  int rowCount = PQntuples(m_result);
  if (rowCount < 0)
    rowCount = 0;
  return static_cast<size_t>(rowCount);
}

//------------------------------------------------------------------------------
/**
  Получить количество столбцов в ответе
*/
//---
size_t PGExecuteResult::GetColCount() const {
  int colCount = PQnfields(m_result);
  if (colCount < 0)
    colCount = 0;
  return static_cast<size_t>(colCount);
}

//------------------------------------------------------------------------------
/**
  Получить название столбца
  \return Название столбца, если передан валидный индекс, иначе пустую строку.
*/
//---
std::string PGExecuteResult::GetColName(size_t columnIndex) const {
  char *colName = PQfname(m_result, static_cast<int>(columnIndex));
  if (!colName)
    return {};
  return std::string(colName);
}

//------------------------------------------------------------------------------
/**
  Получить индекс столбца по имени
  \return Индекс столбца, если передан валидный индекс, иначе невалидный индекс.
*/
//---
size_t PGExecuteResult::GetColIndex(const std::string &columnName) {
  int colIndex = PQfnumber(m_result, columnName.c_str());
  if (colIndex < 0)
    return IExecuteResult::InvalidIndex;
  return static_cast<size_t>(colIndex);
}

//------------------------------------------------------------------------------
/**
  Получить тип данных столбца.
  \return Тип данных столбца, если передан валидный индекс, иначе невалидный
          тип.
*/
//---
SQLDataType PGExecuteResult::GetColType(size_t columnIndex) const {
  Oid id = PQftype(m_result, static_cast<int>(columnIndex));
  return OidToSQLDataType(id);
}

//------------------------------------------------------------------------------
/**
  Получить значение.
*/
//---
IExecuteResult::CellType PGExecuteResult::GetValue(size_t rowIndex,
                                                   size_t columnIndex) {
  if (rowIndex >= GetRowCount() || columnIndex >= GetColCount())
    return {};

  int isNull = PQgetisnull(m_result, rowIndex, columnIndex);
  if (static_cast<bool>(isNull))
    return nullptr;

  char *value = PQgetvalue(m_result, rowIndex, columnIndex);
  if (!value)
    return {};

  CellType cell;
  if (int format = PQfformat(m_result, columnIndex); format == 0) {
    cell = CellType(std::string(value));
  } else if (format == 1) {
    // todo: IConnection::Execute перегрузка с бинарными данными
    /*if (int intLength = PQgetlength(m_result, rowIndex, columnIndex);
            intLength >= 0)
    {
            size_t length = static_cast<size_t>(intLength);

            std::vector<char> byteArray;
            byteArray.resize(length);
            std::memcpy(byteArray.data(), value, length);
            cell = CellType(byteArray);
    }*/
  }

  return cell;
}
