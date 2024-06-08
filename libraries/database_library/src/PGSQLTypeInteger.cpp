#include "PGSQLTypeInteger.h"

#include <cassert>

//------------------------------------------------------------------------------
/**
  �����������
*/
//---
PGSQLTypeInteger::PGSQLTypeInteger(int value)
	: m_value(value)
{
}


//------------------------------------------------------------------------------
/**
  �������� ��������
*/
//---
std::optional<int> PGSQLTypeInteger::GetValue() const
{
	return m_value;
}


//------------------------------------------------------------------------------
/**
  ���������� ��������
*/
//---
void PGSQLTypeInteger::SetValue(int value)
{
	m_value = value;
}


//------------------------------------------------------------------------------
/**
  ��������������� � ������
*/
//---
std::optional<std::string> PGSQLTypeInteger::ToString() const
{
	if (!m_value)
		return std::nullopt;

	return std::to_string(*m_value);
}


//------------------------------------------------------------------------------
/**
  �������� �������� SQL - ����
*/
//---
const std::string & PGSQLTypeInteger::GetTypeName() const
{
	static const std::string name = "INTEGER";
	return name;
}


//------------------------------------------------------------------------------
/**
  ��������� �������� �� ������
*/
//---
bool PGSQLTypeInteger::ReadFrom(const std::string & value)
{
	m_value = std::nullopt;

	try
	{
		std::size_t pos;
		int intValue = std::stoi(value, &pos);
		if (pos == value.size())
			// ������ ������ �������� ������ �� �����
			m_value = intValue;
	}
	catch (...)
	{
	}

	return m_value.has_value();
}


//------------------------------------------------------------------------------
/**
  ��������� �������� �� ������� ����
*/
//---
bool PGSQLTypeInteger::ReadFrom(const std::vector<char>& value)
{
	assert(false); // todo : ���� �� �����������
	return false;
}
