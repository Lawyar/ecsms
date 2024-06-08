#include "PGSQLTypeText.h"

#include <algorithm>


//------------------------------------------------------------------------------
/**
  �����������
*/
//---
PGSQLTypeText::PGSQLTypeText(const std::string & value)
	: m_value(value)
{
}


//------------------------------------------------------------------------------
/**
  �������� ��������
*/
//---
std::optional<std::string> PGSQLTypeText::GetValue() const
{
	return m_value;
}


//------------------------------------------------------------------------------
/**
  ���������� ��������
*/
//---
void PGSQLTypeText::SetValue(const std::string & value)
{
	m_value = value;
}


//------------------------------------------------------------------------------
/**
  ��������������� � ������
*/
//---
std::optional<std::string> PGSQLTypeText::ToString() const
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
bool PGSQLTypeText::ReadFrom(const std::string & value)
{
	SetValue(value);
	return m_value.has_value();
}


//------------------------------------------------------------------------------
/**
  ��������� �������� �� ������� ����
*/
//---
bool PGSQLTypeText::ReadFrom(const std::vector<char>& value)
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
	SetValue(str);
	return m_value.has_value();
}
