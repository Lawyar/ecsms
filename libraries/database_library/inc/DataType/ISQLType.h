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
	virtual std::optional<std::string> ToSQLString() const = 0;
	/// Получить название SQL-типа
	virtual const std::string & GetTypeName() const = 0;

	/// Переменная является пустой
	bool IsEmpty() const { return !ToSQLString().has_value(); }

public:
	/// Получить тип данных
	virtual SQLDataType GetType() const = 0;

public:
	/// Прочитать значение из строки, полученной из результата SQL-запроса
	/// \param value Строка, из которой нужно читать значение.
	///              Если чтение прошло успешно, то строка будет очищена, иначе останется неизменной.
	/// \return Валидно ли прочитанное значение
	virtual bool ReadFromSQL(std::string && value) = 0;

protected:
	// todo: IConnection::Execute перегрузка с бинарными данными

	/// Прочитать значение из массива байт, полученного из результата SQL-запроса
	/// \param value Массив, из которого нужно читать значение.
	///              Если чтение прошло успешно, то массив будет очищен, иначе останется неизменным.
	/// \return Валидно ли прочитанное значение
	virtual bool ReadFromSQL(std::vector<char> && value) { return false; }
};


/// Тип указателя на ISQLType
using ISQLTypePtr = std::shared_ptr<ISQLType>;
