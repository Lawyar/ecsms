#pragma once

#include <DataType/ISQLType.h>
#include <DataType/ISQLTypeInteger.h>
#include <DataType/ISQLTypeText.h>
#include <DataType/ISQLTypeByteArray.h>

#include <Utils/StrongType.h>

#include <string>
#include <vector>
#include <optional>

//------------------------------------------------------------------------------
/**
  Интерфейс конвертера в SQL-тип данных
*/
//---
class ISQLTypeConverter
{
public:
	/// Деструктор
	virtual ~ISQLTypeConverter() = default;

public: // Получение SQL-переменных
	/// Получить пустую SQL-переменную
	virtual ISQLTypePtr GetSQLVariable(SQLDataType type) const = 0;

	/// Получить SQL-Integer переменную
	virtual ISQLTypeIntegerPtr GetSQLTypeInteger(const std::optional<StrongType<int>> & value
		= std::nullopt) const = 0;
	/// Получить SQL-Text переменную
	virtual ISQLTypeTextPtr GetSQLTypeText(const std::optional<StrongType<std::string>> & value
		= std::nullopt) const = 0;
	/// Получить SQL-ByteArray переменную
	virtual ISQLTypeByteArrayPtr GetSQLTypeByteArray(const std::optional<StrongType<std::vector<char>>> &
		value = std::nullopt) const = 0;
};

/// Тип указателя на ISQLTypeConverter
using ISQLTypeConverterPtr = std::shared_ptr<ISQLTypeConverter>;
