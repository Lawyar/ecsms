#include "PGSQLTypeByteArray.h"

#include <cassert>


//------------------------------------------------------------------------------
/**
  Конструктор
*/
//---
PGSQLTypeByteArray::PGSQLTypeByteArray(const std::vector<char>& value)
	: m_value(value)
{
}


//------------------------------------------------------------------------------
/**
  Получить значение
*/
//---
std::optional<std::vector<char>> PGSQLTypeByteArray::GetValue() const
{
	return m_value;
}


//------------------------------------------------------------------------------
/**
  Установить значение
*/
//---
void PGSQLTypeByteArray::SetValue(const std::vector<char>& value)
{
	m_value = value;
}


//------------------------------------------------------------------------------
/**
  Установить значение
*/
//---
std::optional<std::string> PGSQLTypeByteArray::ToString() const
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
bool PGSQLTypeByteArray::ReadFrom(const std::string & value)
{
	assert(false); // todo : Пока не реализовано
	return false;
}


//------------------------------------------------------------------------------
/**
   Прочитать значение из массива байт
*/
//---
bool PGSQLTypeByteArray::ReadFrom(const std::vector<char>& value)
{
	SetValue(value);
	return m_value.has_value();
}
