#pragma once

#include <DataType/ISQLType.h>

#include <string>
#include <vector>
#include <optional>

//------------------------------------------------------------------------------
/**
  Интерфейс SQL-типа данных "integer"
*/
//---
class ISQLTypeInteger : public ISQLType
{
public:
	/// Деструктор
	virtual ~ISQLTypeInteger() override = default;

public:
	/// Получить тип данных
	virtual SQLDataType GetType() const override final { return SQLDataType::Integer; }

public:
	/// Получить значение
	virtual std::optional<int> GetValue() const = 0;
	/// Установить значение
	virtual void SetValue(int value) = 0;
};


/// Тип указателя на ISQLTypeInteger
using ISQLTypeIntegerPtr = std::shared_ptr<ISQLTypeInteger>;
