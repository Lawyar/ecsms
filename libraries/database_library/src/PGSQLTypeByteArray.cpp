#include "PGSQLTypeByteArray.h"

#include <cassert>


//------------------------------------------------------------------------------
/**
  �����������
*/
//---
PGSQLTypeByteArray::PGSQLTypeByteArray(const std::vector<char>& value)
	: m_value(value)
{
}


//------------------------------------------------------------------------------
/**
  �������� ��������
*/
//---
std::optional<std::vector<char>> PGSQLTypeByteArray::GetValue() const
{
	return m_value;
}


//------------------------------------------------------------------------------
/**
  ���������� ��������
*/
//---
void PGSQLTypeByteArray::SetValue(const std::vector<char>& value)
{
	m_value = value;
}


//------------------------------------------------------------------------------
/**
  ���������� ��������
*/
//---
std::optional<std::string> PGSQLTypeByteArray::ToString() const
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
bool PGSQLTypeByteArray::ReadFrom(const std::string & value)
{
	assert(false); // todo : ���� �� �����������
	return false;
}


//------------------------------------------------------------------------------
/**
   ��������� �������� �� ������� ����
*/
//---
bool PGSQLTypeByteArray::ReadFrom(const std::vector<char>& value)
{
	SetValue(value);
	return m_value.has_value();
}
