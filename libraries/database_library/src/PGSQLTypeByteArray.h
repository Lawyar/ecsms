#pragma once

#include <DataType/ISQLTypeByteArray.h>

#include <vector>

//------------------------------------------------------------------------------
/**
  Реализация SQL-типа данных "bytea" для PostgreSQL
*/
//---
class PGSQLTypeByteArray : public ISQLTypeByteArray
{
	std::optional<std::vector<char>> m_value; ///< Значение

public:
	/// Конструктор
	PGSQLTypeByteArray() = default;
	/// Конструктор от аргумента
	PGSQLTypeByteArray(std::vector<char> && value);
	/// Конструктор копирования
	PGSQLTypeByteArray(const PGSQLTypeByteArray &) = default;
	/// Конструктор перемещения
	PGSQLTypeByteArray(PGSQLTypeByteArray &&) = default;
	/// Оператор присваивания копированием
	PGSQLTypeByteArray& operator=(const PGSQLTypeByteArray &) = default;
	/// Оператор присваивания перемещением
	PGSQLTypeByteArray& operator=(PGSQLTypeByteArray &&) = default;

public:
	/// Получить значение
	virtual const std::optional<std::vector<char>> & GetValue() const override;
	/// Установить значение
	virtual void SetValue(std::vector<char> && value) override;

public:
	/// Сконвертировать в строку
	virtual std::optional<std::string> ToSQLString() const override;
	/// Получить название SQL-типа
	virtual const std::string & GetTypeName() const override;

public:
	/// Прочитать значение из строки
	/// \return Валидно ли прочитанное значение
	virtual bool ReadFromSQL(std::string && value) override;

protected:
	/// Прочитать значение из массива байт
	/// \return Валидно ли прочитанное значение
	virtual bool ReadFromSQL(std::vector<char> && value) override;
};
