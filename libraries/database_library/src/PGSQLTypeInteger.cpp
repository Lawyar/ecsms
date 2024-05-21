#include "PGSQLTypeInteger.h"

#include <cassert>
#include <cctype>

#include <Utils/StringUtils.h>

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
std::optional<std::string> PGSQLTypeInteger::ToSQLString() const
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
  \param value Строка, из которой нужно читать значение.
               Если чтение прошло успешно, то строка будет очищена, иначе останется неизменной.
*/
//---
bool PGSQLTypeInteger::ReadFromSQL(std::string && value)
{
	m_value = std::nullopt;

	if (value.empty())
		return false;

	if (value.size() >= 100)
		// Если в строке больше 100 символов, то она никак не поместится в int
		return false;

	m_value = utils::string::StringToNumber(
		static_cast<int(*)(const std::string&, size_t *, int)>(&std::stoi), value);

	bool result = m_value.has_value();
	if (result)
		value.clear();
	return m_value.has_value();
}


//------------------------------------------------------------------------------
/**
  Прочитать значение из массива байт
  \param value Массив, из которого нужно читать значение.
               Если чтение прошло успешно, то массив будет очищен, иначе останется неизменным.
*/
//---
bool PGSQLTypeInteger::ReadFromSQL(std::vector<char> && value)
{
	assert(false); // todo : Пока не реализовано
	return false;
}
