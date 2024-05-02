#pragma once

#include <DataType/SQLDataType.h>
#include <IExecuteResultStatus.h>

#include <optional>
#include <string>
#include <variant>
#include <vector>

//------------------------------------------------------------------------------
/**
  Формат данных в результате
*/
//---
enum class ResultFormat
{
	Text,   ///< Формат представлен в виде текстовых данных
	Binary, ///< Формат представлен в виде бинарных данных
	Invalid ///< Невалидный формат данных
};

//------------------------------------------------------------------------------
/**
  Интерфейс результата запроса к базе данных.
  Во всех методах индексация начинается с нуля.
*/
//---
class IExecuteResult
{
public:
	/// Тип данных ячейки таблицы.
	/// Ячейка содержит тот тип данных (бинарный или текстовый), который запрашивался.
	class CellType
	{
		std::optional<std::variant<std::string, std::vector<char>, nullptr_t>> m_value; ///< Значение

	public:
		/// Конструктор
		CellType() : m_value() {}
		/// Конструктор от NULL
		CellType(nullptr_t) : m_value(nullptr) {}
		/// Конструктор от строки
		CellType(const std::string & str) : m_value(str) {}
		/// Конструктор от массива байт
		CellType(const std::vector<char> & arr) : m_value(arr) {}

		/// Ячейка содержит значение
		bool HasValue() const { return !!m_value; }
		/// Ячейка содержит строку
		bool HasString() const { return m_value && std::holds_alternative<std::string>(*m_value); }
		/// Ячейка содержит NULL
		bool HasNull() const { return m_value && std::holds_alternative<nullptr_t>(*m_value); }

		/// Получить строку, если данные представлены строкой
		std::string GetString() const { return HasString() ? std::get<std::string>(*m_value) : std::string(); }

	private:
		// todo: IConnection::Execute перегрузка с бинарными данными
		// Единственный способ получить бинарный результат - через вторую перегрузку IConnection::Execute,
		// которая в данный момент не используется и перемещена в protected-секцию.
		// Поэтому и эти методы перемещу в private-секцию.

		/// Ячейка содержит бинарные данные
		bool HasByteArray() const { return m_value && std::holds_alternative<std::vector<char>>(*m_value); }
		/// Получить массив байт, если данные представлены массивом байт
		std::vector<char> GetByteArray() const { return HasByteArray() ? std::get<std::vector<char>>(*m_value) : std::vector<char>(); }
	};

public:
	static constexpr size_t InvalidIndex = (size_t)(-1); ///< Невалидный индекс
public:
	/// Деструктор
	virtual ~IExecuteResult() = default;

public:
	/// Получить текущий статус запроса
	virtual IExecuteResultStatusPtr GetCurrentExecuteStatus() const = 0;

public:
	/// Получить количество строк в ответе
	virtual size_t GetRowCount() const = 0;
	/// Получить количество столбцов в ответе
	virtual size_t GetColCount() const = 0;
	
	/// Получить название столбца (возвращает пустую строку, если передан невалидный индекс)
	virtual std::string GetColName(size_t columnIndex) const = 0;
	/// Получить индекс столбца по имени (возвращает невалидный индекс, если передано несуществующее имя)
	/// Имя (в соотвествии со стандартом SQL) по умолчанию приводится к нижнему регистру,
	/// так что вызовы GetColIndex("columnname") и GetColIndex("CoLuMnNaMe") равнозначны,
	/// кроме случая, если имя обрамлено кавычками.
	/// Таким образом, если запрос содержал явное именование столбцов с использованием кавычек
	/// (например, SELECT 1 AS "Number"),
	/// то в эту функцию надо передавать GetColIndex("\"Number\"") для определения индекса столбца,
	/// а вызовы GetColIndex("Number"), или GetColIndex("\"number\"") вернут невалидный индекс.
	virtual size_t GetColIndex(const std::string & columnName) = 0;
	
	/// Получить тип данных столбца (возвращает невалидный тип данных, если передан невалидный индекс)
	virtual SQLDataType GetColType(size_t columnIndex) const = 0;

	/// Получить значение.
	/// Возвращает std::nullopt, если ячейка содержит null или если переданы невалидные индексы.
	/// В противном случае возвращает текст или бинарные данные
	/// (в зависимости от того, в каком виде запрашивались данные в команде)
	virtual CellType GetValue(size_t rowIndex, size_t columnIndex) = 0;
};

/// Указатель на IExecuteResult
using IExecuteResultPtr = std::shared_ptr<IExecuteResult>;
