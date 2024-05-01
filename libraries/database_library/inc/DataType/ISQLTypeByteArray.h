#pragma once

#include <DataType/ISQLType.h>

#include <vector>
#include <optional>

//------------------------------------------------------------------------------
/**
  Интерфейс SQL-типа данных "bytea"
*/
//---
class ISQLTypeByteArray : public ISQLType
{
public:
	/// Деструктор
	virtual ~ISQLTypeByteArray() override = default;

public:
	/// Получить тип данных
	virtual SQLDataType GetType() const override final { return SQLDataType::ByteArray; }

public:
	/// Получить значение
	virtual std::optional<std::vector<char>> GetValue() const = 0;
	/// Установить значение
	virtual void SetValue(const std::vector<char> & value) = 0;
};


/// Тип указателя на ISQLTypeByteArray
using ISQLTypeByteArrayPtr = std::shared_ptr<ISQLTypeByteArray>;
