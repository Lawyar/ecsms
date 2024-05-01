#include "PGSQLTypeConverter.h"

#include <PGSQLTypeInteger.h>
#include <PGSQLTypeText.h>
#include <PGSQLTypeByteArray.h>

#include <cassert>

//------------------------------------------------------------------------------
/**
  Получить пустую SQL-переменную
*/
//---
ISQLTypePtr PGSQLTypeConverter::GetSQLVariable(SQLDataType type) const
{
	ISQLTypePtr result;
	switch (type)
	{
	case SQLDataType::Integer:
		result = GetSQLTypeInteger();
		break;
	case SQLDataType::Text:
		result = GetSQLTypeText();
		break;
	case SQLDataType::ByteArray:
		result = GetSQLTypeByteArray();
		break;
	case SQLDataType::Unknown:
		// Нельзя получить неизвестный тип данных
		assert(false);
		break;
	case SQLDataType::Invalid:
		// Ок, вернем nullptr
		break;
	default:
		// Неизвестный тип данных
		assert(false);
		break;
	}

	return result;
}


//------------------------------------------------------------------------------
/**
  Получить пустую SQL-Integer переменную
*/
//---
ISQLTypeIntegerPtr PGSQLTypeConverter::GetSQLTypeInteger(const std::optional<StrongType<int>> & value) const
{
	return value.has_value()
		? std::make_shared<PGSQLTypeInteger>(value->value)
		: std::make_shared<PGSQLTypeInteger>();
}


//------------------------------------------------------------------------------
/**
  Получить пустую SQL-Text переменную
*/
//---
ISQLTypeTextPtr PGSQLTypeConverter::GetSQLTypeText(const std::optional<StrongType<std::string>> & value) const
{
	return value.has_value()
		? std::make_shared<PGSQLTypeText>(value->value)
		: std::make_shared<PGSQLTypeText>();
}


//------------------------------------------------------------------------------
/**
  Получить пустую SQL-ByteArray переменную
*/
//---
ISQLTypeByteArrayPtr PGSQLTypeConverter::GetSQLTypeByteArray(const std::optional<StrongType<std::vector<char>>>
	& value) const
{
	return value.has_value()
		? std::make_shared<PGSQLTypeByteArray>(value->value)
		: std::make_shared<PGSQLTypeByteArray>();
}
