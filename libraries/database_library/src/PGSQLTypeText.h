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
	PGSQLTypeText(std::string && value);
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
	virtual const std::optional<std::string> & GetValue() const override;
	/// Установить значение
	/// \param value Строка, из которой нужно установить значение.
	///              При успехе строка будет очищена, иначе останется неизменной.
	virtual void SetValue(std::string && value) override;


public: // ISQLType
	/// Сконвертировать в строку
	virtual std::optional<std::string> ToSQLString() const override;
	/// Получить название SQL-типа
	virtual const std::string & GetTypeName() const override;

public:
	/// Прочитать значение из строки
	/// \param value Строка, из которой нужно читать значение.
	///              Если чтение прошло успешно, то строка будет очищена, иначе останется неизменной.
	/// \return Валидно ли прочитанное значение
	virtual bool ReadFromSQL(std::string && value) override;

protected:
	/// Прочитать значение из массива байт
	/// \param value Массив, из которого нужно читать значение.
	///              Если чтение прошло успешно, то массив будет очищен, иначе останется неизменным.
	/// \return Валидно ли прочитанное значение
	virtual bool ReadFromSQL(std::vector<char> && value) override;

private:
	/// Это недопустимый символ
	static bool isInvalidChar(char c);
};
