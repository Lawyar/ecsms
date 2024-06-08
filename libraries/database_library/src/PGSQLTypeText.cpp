#include "PGSQLTypeText.h"

#include <algorithm>


//------------------------------------------------------------------------------
/**
  �����������
*/
//---
PGSQLTypeText::PGSQLTypeText(std::string && value)
	: m_value(std::move(value))
{
}


//------------------------------------------------------------------------------
/**
  �������� ��������
*/
//---
const std::optional<std::string> & PGSQLTypeText::GetValue() const
{
	return m_value;
}


//------------------------------------------------------------------------------
/**
  ���������� ��������
*/
//---
void PGSQLTypeText::SetValue(std::string && value)
{
	m_value = std::move(value);
}


//------------------------------------------------------------------------------
/**
  ��������������� � ������
*/
//---
std::optional<std::string> PGSQLTypeText::ToSQLString() const
{
	if (!m_value)
		return std::nullopt;

	return "'" + *m_value + "'";
}


//------------------------------------------------------------------------------
/**
  �������� �������� SQL-����
*/
//---
const std::string & PGSQLTypeText::GetTypeName() const
{
	static const std::string name = "TEXT";
	return name;
}


//------------------------------------------------------------------------------
/**
  ��������� �������� �� ������
*/
//---
bool PGSQLTypeText::ReadFromSQL(std::string && value)
{
	SetValue(std::move(value));
	return m_value.has_value();
}


//------------------------------------------------------------------------------
/**
  ��������� �������� �� ������� ����
*/
//---
bool PGSQLTypeText::ReadFromSQL(std::vector<char> && value)
{
	// ���������� ���������� ������
	if (value.empty())
		// ������ ������ ���� �� �������������� � ������ ������,
		// ��������� � ������ ����������� ������ ���� ���� ������ - ����-���������� � ����� ������.
		// (�� ���� ������ ������ �������������� �������� ������ ���: {'\0'}).
		// ���� ������ ���� ������, ������ ������ ���������.
		return false;

	if (value.back() != '\0')
		// ���� ������ �� ������������ �� ����-����������, �� ��� ���������.
		return false;

	if (std::count(value.begin(), value.end(), '\0') != 1)
		// ���� � ������ ������ ������ ����-�����������, �� ��� ���������.
		return false;

	// ��������, ���� ����� �������� �������� �� ������� ���������� ��������, �� ���� � ��� ������

	std::string str(value.data(), value.size());
	SetValue(std::move(str));

	bool result = m_value.has_value();
	if (result)
		value.clear();
	return result;
}
