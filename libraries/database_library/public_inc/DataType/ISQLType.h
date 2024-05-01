#pragma once

#include <string>
#include <memory>
#include <vector>
#include <optional>

#include <DataType/SQLDataType.h>

//------------------------------------------------------------------------------
/**
  Интерфейс SQL-типа данных
*/
//---
class ISQLType
{
public:
	/// Деструктор
	virtual ~ISQLType() = default;

public:
	/// Сконвертировать в строку
	virtual std::optional<std::string> ToString() const = 0;
	/// Получить название SQL-типа
	virtual const std::string & GetTypeName() const = 0;

public:
	/// Получить тип данных
	virtual SQLDataType GetType() const = 0;

public:
	/// Прочитать значение из строки
	/// \return Валидно ли прочитанное значение
	virtual bool ReadFrom(const std::string & value) = 0;
	/// Прочитать значение из массива байт
	/// \return Валидно ли прочитанное значение
	virtual bool ReadFrom(const std::vector<char> & value) = 0;
};


/// Тип указателя на ISQLType
using ISQLTypePtr = std::shared_ptr<ISQLType>;
