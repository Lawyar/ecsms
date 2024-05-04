#include "PGSQLTypeByteArray.h"

#include <cassert>


//------------------------------------------------------------------------------
/**
  Конструктор
*/
//---
PGSQLTypeByteArray::PGSQLTypeByteArray(std::vector<char> && value)
	: m_value(std::move(value))
{
}


//------------------------------------------------------------------------------
/**
  Получить значение
*/
//---
const std::optional<std::vector<char>> & PGSQLTypeByteArray::GetValue() const
{
	return m_value;
}


//------------------------------------------------------------------------------
/**
  Установить значение
*/
//---
void PGSQLTypeByteArray::SetValue(std::vector<char> && value)
{
	m_value = std::move(value);
}


//------------------------------------------------------------------------------
/**
  Установить значение
*/
//---
std::optional<std::string> PGSQLTypeByteArray::ToSQLString() const
{
	if (!m_value)
		return std::nullopt;

	assert(false); // todo : Пока не реализовано
	// возвращать, обрамляя апострофами
	return std::nullopt;
}


//------------------------------------------------------------------------------
/**
   Получить название SQL-типа
*/
//---
const std::string & PGSQLTypeByteArray::GetTypeName() const
{
	static const std::string name = "BYTEA";
	return name;
}


//------------------------------------------------------------------------------
/**
   Прочитать значение из строки
*/
//---
bool PGSQLTypeByteArray::ReadFromSQL(std::string && value)
{
	m_value = std::nullopt;
	assert(false); // todo : Пока не реализовано

	bool result = m_value.has_value();
	if (result)
		value.clear();
	return result;
}


//------------------------------------------------------------------------------
/**
   Прочитать значение из массива байт
*/
//---
bool PGSQLTypeByteArray::ReadFromSQL(std::vector<char> && value)
{
	SetValue(std::move(value));
	return m_value.has_value();
}
