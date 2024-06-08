#include "PGSQLTypeByteArray.h"

#include <cassert>


//------------------------------------------------------------------------------
/**
  �����������
*/
//---
PGSQLTypeByteArray::PGSQLTypeByteArray(std::vector<char> && value)
	: m_value(std::move(value))
{
}


//------------------------------------------------------------------------------
/**
  �������� ��������
*/
//---
const std::optional<std::vector<char>> & PGSQLTypeByteArray::GetValue() const
{
	return m_value;
}


//------------------------------------------------------------------------------
/**
  ���������� ��������
*/
//---
void PGSQLTypeByteArray::SetValue(std::vector<char> && value)
{
	m_value = std::move(value);
}


//------------------------------------------------------------------------------
/**
  ���������� ��������
*/
//---
std::optional<std::string> PGSQLTypeByteArray::ToSQLString() const
{
	if (!m_value)
		return std::nullopt;

	assert(false); // todo : ���� �� �����������
	// ����������, �������� �����������
	return std::nullopt;
}


//------------------------------------------------------------------------------
/**
   �������� �������� SQL-����
*/
//---
const std::string & PGSQLTypeByteArray::GetTypeName() const
{
	static const std::string name = "BYTEA";
	return name;
}


//------------------------------------------------------------------------------
/**
   ��������� �������� �� ������
*/
//---
bool PGSQLTypeByteArray::ReadFromSQL(std::string && value)
{
	m_value = std::nullopt;
	assert(false); // todo : ���� �� �����������

	bool result = m_value.has_value();
	if (result)
		value.clear();
	return result;
}


//------------------------------------------------------------------------------
/**
   ��������� �������� �� ������� ����
*/
//---
bool PGSQLTypeByteArray::ReadFromSQL(std::vector<char> && value)
{
	SetValue(std::move(value));
	return m_value.has_value();
}
