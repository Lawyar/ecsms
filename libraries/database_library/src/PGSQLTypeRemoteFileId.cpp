#include "PGSQLTypeRemoteFileId.h"

#include <Utils/StringUtils.h>

//------------------------------------------------------------------------------
/**
  �����������
*/
//---
PGSQLTypeRemoteFileId::PGSQLTypeRemoteFileId(const std::string & id)
	: m_id(id)
{
}


//------------------------------------------------------------------------------
/**
  �������� �������������
*/
//---
const std::optional<std::string>& PGSQLTypeRemoteFileId::GetId() const
{
	return m_id;
}


//------------------------------------------------------------------------------
/**
  ��������������� � ������
*/
//---
std::optional<std::string> PGSQLTypeRemoteFileId::ToSQLString() const
{
	return GetId();
}


//------------------------------------------------------------------------------
/**
  �������� �������� SQL-����
*/
//---
const std::string & PGSQLTypeRemoteFileId::GetTypeName() const
{
	static const std::string type = "OID";
	return type;
}


//------------------------------------------------------------------------------
/**
  ��������� �������� �� ������
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