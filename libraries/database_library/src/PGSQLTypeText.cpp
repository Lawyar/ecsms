#include "PGSQLTypeText.h"

#include <algorithm>


//------------------------------------------------------------------------------
/**
   онструктор
*/
//---
PGSQLTypeText::PGSQLTypeText(std::string && value)
	: m_value(std::move(value))
{
}


//------------------------------------------------------------------------------
/**
  ѕолучить значение
*/
//---
const std::optional<std::string> & PGSQLTypeText::GetValue() const
{
	return m_value;
}


//------------------------------------------------------------------------------
/**
  ”становить значение
*/
//---
void PGSQLTypeText::SetValue(std::string && value)
{
	m_value = std::move(value);
}


//------------------------------------------------------------------------------
/**
  —конвертировать в строку
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
  ѕолучить название SQL-типа
*/
//---
const std::string & PGSQLTypeText::GetTypeName() const
{
	static const std::string name = "TEXT";
	return name;
}


//------------------------------------------------------------------------------
/**
  ѕрочитать значение из строки
*/
//---
bool PGSQLTypeText::ReadFromSQL(std::string && value)
{
	SetValue(std::move(value));
	return m_value.has_value();
}


//------------------------------------------------------------------------------
/**
  ѕрочитать значение из массива байт
*/
//---
bool PGSQLTypeText::ReadFromSQL(std::vector<char> && value)
{
	// ќпредел€ем валидность строки
	if (value.empty())
		// ѕустой массив байт не конвертируетс€ в пустую строку,
		// поскольку в строке об€зательно должен быть один символ - нуль-терминатор в конце строки.
		// (то есть пуста€ строка представл€етс€ массивом байтов так: {'\0'}).
		// ≈сли массив байт пустой, значит строка невалидна.
		return false;

	if (value.back() != '\0')
		// ≈сли строка не оканчиваетс€ на нуль-терминатор, то она невалидна.
		return false;

	if (std::count(value.begin(), value.end(), '\0') != 1)
		// ≈сли в строке больше одного нуль-терминатора, то она невалидна.
		return false;

	// ¬озможно, надо также добавить проверки на наличие нечитаемых символов, но пока и так сойдет

	std::string str(value.data(), value.size());
	SetValue(std::move(str));

	bool result = m_value.has_value();
	if (result)
		value.clear();
	return result;
}
