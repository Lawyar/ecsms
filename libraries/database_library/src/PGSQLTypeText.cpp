#include "PGSQLTypeText.h"

#include <algorithm>


//------------------------------------------------------------------------------
/**
  �����������
*/
//---
PGSQLTypeText::PGSQLTypeText(std::string && value)
{
	SetValue(std::move(value));
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
  \param value ������, �� ������� ����� ���������� ��������.
               ��� ������ ������ ����� �������, ����� ��������� ����������.
*/
//---
void PGSQLTypeText::SetValue(std::string && value)
{
	if (std::count_if(value.begin(), value.end(), isInvalidChar) != 0)
	{
		// ���� � ������ ���� ������������ �������, �� ��� ���������.
		m_value = std::nullopt;
		return;
	}

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

	// ������ ������ � ����� SQL-������. ������ SQL ������ ��� ����� ������������ ����� �����.
	static constexpr char specialChar = '\'';

	std::string result;

	result.reserve(m_value->size() + static_cast <size_t>(2 + m_value->size() / 128));
	// + 2 ����� �� ����������� � ����������� �������
	// + (size / 128) ���� �� ���������
	// (������ �� �������, ��� ��������� ����������� � ������� � �������� 0.005...0.008)

	result.push_back(specialChar);
	for (auto ch : *m_value)
	{
		if (ch == specialChar)
		{
			// ���������� ����������
			result.push_back(specialChar);
			result.push_back(specialChar);
		}
		else
		{
			result.push_back(ch);
		}
	}
	result.push_back(specialChar);

	return result;
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
  \param value ������, �� ������� ����� ������ ��������.
               ���� ������ ������ �������, �� ������ ����� �������, ����� ��������� ����������.
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
  \param value ������, �� �������� ����� ������ ��������.
               ���� ������ ������ �������, �� ������ ����� ������, ����� ��������� ����������.
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

	std::string str(value.data(), value.size() - 1);
	SetValue(std::move(str));

	bool result = m_value.has_value();
	if (result)
		value.clear();
	return result;
}


//------------------------------------------------------------------------------
/**
  ��� ������������ ������
*/
//---
bool PGSQLTypeText::isInvalidChar(char c)
{
	// ��������, ���� ����� �������� �������� �� ������� ���������� ��������, �� ���� � ��� ������
	return c == '\0';
}
