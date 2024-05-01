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
	PGSQLTypeByteArray(const std::vector<char> & value);
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
	virtual std::optional<std::vector<char>> GetValue() const override;
	/// Установить значение
	virtual void SetValue(const std::vector<char> & value) override;

public:
	/// Сконвертировать в строку
	virtual std::optional<std::string> ToString() const override;
	/// Получить название SQL-типа
	virtual const std::string & GetTypeName() const override;

public:
	/// Прочитать значение из строки
	/// \return Валидно ли прочитанное значение
	virtual bool ReadFrom(const std::string & value) override;
	/// Прочитать значение из массива байт
	/// \return Валидно ли прочитанное значение
	virtual bool ReadFrom(const std::vector<char> & value) override;
};
