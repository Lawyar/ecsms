#include "PGSQLTypeRemoteFileId.h"

#include <Utils/StringUtils.h>
#include <libpq-fe.h>

//------------------------------------------------------------------------------
/**
  �����������
*/
//---
PGSQLTypeRemoteFileId::PGSQLTypeRemoteFileId(const std::string & id)
	: m_id(isValid(id) ? std::make_optional(id) : std::nullopt)
{
}


//------------------------------------------------------------------------------
/**
  �������� �������������
*/
//---
const std::optional<std::string>& PGSQLTypeRemoteFileId::GetValue() const
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
	return GetValue();
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
	m_id = std::nullopt;
	if (isValid(value))
		m_id = std::move(value);
	return m_id.has_value();
}


//------------------------------------------------------------------------------
/**
  ������ ������� ��� ��������� � ������
*/
//---
bool PGSQLTypeRemoteFileId::isValid(const std::string & str)
{
	if (str.empty() || !utils::string::HasOnlyDigits(str))
		return false;

	auto number = utils::string::StringToNumber(
		static_cast<long(*)(const std::string&, size_t *, int)>(&std::stol), str);
	if (!number || *number < std::numeric_limits<Oid>::min() || *number > std::numeric_limits<Oid>::max())
		return false;

	return true;
}
