#include "PGSQLTypeInteger.h"

#include <cassert>
#include <cctype>

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
std::optional<std::string> PGSQLTypeInteger::ToSQLString() const
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
  \param value ������, �� ������� ����� ������ ��������.
               ���� ������ ������ �������, �� ������ ����� �������, ����� ��������� ����������.
*/
//---
bool PGSQLTypeInteger::ReadFromSQL(std::string && value)
{
	m_value = std::nullopt;

	if (value.empty())
		return false;

	if (value.size() >= 100)
		// ���� � ������ ������ 100 ��������, �� ��� ����� �� ���������� � int
		return false;

	const bool startsWithDigit = std::isdigit(static_cast<unsigned char>(value[0]));
	const bool startsWithPlus = value[0] == '+';
	const bool startsWithMinus = value[0] == '-';
	if (!startsWithDigit && !startsWithPlus && !startsWithMinus)
		// ����� ������ ���������� � �����, ����� ��� ������
		return false;

	try
	{
		std::size_t pos;
		int intValue = std::stoi(value, &pos);
		if (pos == value.size())
			// ���� �� ��������� ������ ���������, ������ � ����� �������� ���-��, �� ����������� � �����
			m_value = intValue;

	}
	catch (...)
	{
	}

	bool result = m_value.has_value();
	if (result)
		value.clear();
	return m_value.has_value();
}


//------------------------------------------------------------------------------
/**
  ��������� �������� �� ������� ����
  \param value ������, �� �������� ����� ������ ��������.
               ���� ������ ������ �������, �� ������ ����� ������, ����� ��������� ����������.
*/
//---
bool PGSQLTypeInteger::ReadFromSQL(std::vector<char> && value)
{
	assert(false); // todo : ���� �� �����������
	return false;
}
