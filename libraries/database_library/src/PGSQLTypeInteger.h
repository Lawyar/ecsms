#pragma once

#include <DataType/ISQLTypeInteger.h>

//------------------------------------------------------------------------------
/**
  Реализация SQL-типа данных "integer" для PostgreSQL
*/
//---
class PGSQLTypeInteger : public ISQLTypeInteger
{
	std::optional<int> m_value; ///< Значение

public:
	/// Конструктор
	PGSQLTypeInteger() = default;
	/// Конструктор от аргумента
	PGSQLTypeInteger(int value);
	/// Конструктор копирования
	PGSQLTypeInteger(const PGSQLTypeInteger &) = default;
	/// Конструктор перемещения
	PGSQLTypeInteger(PGSQLTypeInteger &&) = default;
	/// Оператор присваивания копированием
	PGSQLTypeInteger& operator=(const PGSQLTypeInteger &) = default;
	/// Оператор присваивания перемещением
	PGSQLTypeInteger& operator=(PGSQLTypeInteger &&) = default;

public: // ISQLTypeInteger
	/// Получить значение
	virtual std::optional<int> GetValue() const override;
	/// Установить значение
	virtual void SetValue(int value) override;

public: // ISQLType
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
