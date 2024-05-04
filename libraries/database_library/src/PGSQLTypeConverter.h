#pragma once

#include <DataType/ISQLTypeConverter.h>

//------------------------------------------------------------------------------
/**
  Реализация конвертера в SQL-тип данных для PostgreSQL
*/
//---
class PGSQLTypeConverter : public ISQLTypeConverter
{
public: // Получение пустых SQL-переменных
	/// Получить пустую SQL-переменную
	virtual ISQLTypePtr GetSQLVariable(SQLDataType type) const override;

	/// Получить SQL-Integer переменную
	virtual ISQLTypeIntegerPtr GetSQLTypeInteger(const std::optional<StrongType<int>> & value
		= std::nullopt) const override;
	
	/// Получить SQL-Text переменную
	virtual ISQLTypeTextPtr GetSQLTypeText() const override;
	/// Получить SQL-Text переменную по строке
	virtual ISQLTypeTextPtr GetSQLTypeText(std::string && value) const override;

	/// Получить SQL-ByteArray переменную
	virtual ISQLTypeByteArrayPtr GetSQLTypeByteArray() const override;
	/// Получить SQL-ByteArray переменную по массиву байт
	virtual ISQLTypeByteArrayPtr GetSQLTypeByteArray(std::vector<char> && value) const override;
};
