#include "PGSQLTypeRemoteFileId.h"

#include <Utils/StringUtils.h>

//------------------------------------------------------------------------------
/**
  Конструктор
*/
//---
PGSQLTypeRemoteFileId::PGSQLTypeRemoteFileId(const std::string & id)
	: m_id(id)
{
}


//------------------------------------------------------------------------------
/**
  Получить идентификатор
*/
//---
const std::optional<std::string>& PGSQLTypeRemoteFileId::GetId() const
{
	return m_id;
}


//------------------------------------------------------------------------------
/**
  Сконвертировать в строку
*/
//---
std::optional<std::string> PGSQLTypeRemoteFileId::ToSQLString() const
{
	return GetId();
}


//------------------------------------------------------------------------------
/**
  Получить название SQL-типа
*/
//---
const std::string & PGSQLTypeRemoteFileId::GetTypeName() const
{
	static const std::string type = "OID";
	return type;
}


//------------------------------------------------------------------------------
/**
  Прочитать значение из строки
*/
//---
bool PGSQLTypeRemoteFileId::ReadFromSQL(std::string && value)
{
	if (value.empty())
		return false;

	if (utils::string::HasOnlyDigits(value))
		m_id = value;

	return m_id.has_value();
}