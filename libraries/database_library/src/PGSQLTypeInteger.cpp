#include "PGSQLTypeInteger.h"

#include <cassert>

//------------------------------------------------------------------------------
/**
  Конструктор
*/
//---
PGSQLTypeInteger::PGSQLTypeInteger(int value)
	: m_value(value)
{
}


//------------------------------------------------------------------------------
/**
  Получить значение
*/
//---
std::optional<int> PGSQLTypeInteger::GetValue() const
{
	return m_value;
}


//------------------------------------------------------------------------------
/**
  Установить значение
*/
//---
void PGSQLTypeInteger::SetValue(int value)
{
	m_value = value;
}


//------------------------------------------------------------------------------
/**
  Сконвертировать в строку
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
  Получить название SQL - типа
*/
//---
const std::string & PGSQLTypeInteger::GetTypeName() const
{
	static const std::string name = "INTEGER";
	return name;
}


//------------------------------------------------------------------------------
/**
  Прочитать значение из строки
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
			// строка должна состоять только из числа
			m_value = intValue;
	}
	catch (...)
	{
	}

	return m_value.has_value();
}


//------------------------------------------------------------------------------
/**
  Прочитать значение из массива байт
*/
//---
bool PGSQLTypeInteger::ReadFrom(const std::vector<char>& value)
{
	assert(false); // todo : Пока не реализовано
	return false;
}
