#pragma once

#include <DataType/ISQLTypeText.h>

//------------------------------------------------------------------------------
/**
  Реализация SQL-типа данных "text" для PostgreSQL
*/
//---
class PGSQLTypeText : public ISQLTypeText
{
	std::optional<std::string> m_value; ///< Значение

public:
	/// Конструктор
	PGSQLTypeText() = default;
	/// Конструктор от аргумента
	PGSQLTypeText(const std::string & value);
	/// Конструктор копирования
	PGSQLTypeText(const PGSQLTypeText &) = default;
	/// Конструктор перемещения
	PGSQLTypeText(PGSQLTypeText &&) = default;
	/// Оператор присваивания копированием
	PGSQLTypeText& operator=(const PGSQLTypeText &) = default;
	/// Оператор присваивания перемещением
	PGSQLTypeText& operator=(PGSQLTypeText &&) = default;

public: // ISQLTypeText
	/// Получить значение
	virtual std::optional<std::string> GetValue() const override;
	/// Установить значение
	virtual void SetValue(const std::string & value) override;


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
